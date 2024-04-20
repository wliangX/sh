#include <RTPPacket.h>

#if defined(WIN32) || defined(WIN64)
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif


void RTPPacket::Clear()
{
	hasextension = false;
	hasmarker = false;
	numcsrcs = 0;
	payloadtype = 0;
	extseqnr = 0;
	timestamp = 0;
	ssrc = 0;
	packet = 0;
	payload = 0;
	packetlength = 0;
	payloadlength = 0;
	extid = 0;
	extension = 0;
	extensionlength = 0;
	error = 0;
	externalbuffer = false;
}

RTPPacket::RTPPacket(RTPRawPacket& rawpack) : receivetime(rawpack.GetReceiveTime())
{
	Clear();
	error = ParseRawPacket(rawpack);
}

int RTPPacket::ParseRawPacket(RTPRawPacket& rawpack)
{
	uint8_t* packetbytes;
	size_t packetlen;
	uint8_t payloadtype;
	RTPHeader* rtpheader;
	bool marker;
	int csrccount;
	bool hasextension;
	int payloadoffset, payloadlength;
	int numpadbytes;
	RTPExtensionHeader* rtpextheader;
	uint16_t exthdrlen;

	if (!rawpack.IsRTP()) // If we didn't receive it on the RTP port, we'll ignore it
		return ERR_RTP_PACKET_INVALIDPACKET;

	// The length should be at least the size of the RTP header
	packetlen = rawpack.GetDataLength();
	if (packetlen < sizeof(RTPHeader))
		return ERR_RTP_PACKET_INVALIDPACKET;

	packetbytes = (uint8_t*)rawpack.GetData();
	rtpheader = (RTPHeader*)packetbytes;

	// The version number should be correct
	if (rtpheader->version != RTP_VERSION)
		return ERR_RTP_PACKET_INVALIDPACKET;

	// We'll check if this is possibly a RTCP packet. For this to be possible
	// the marker bit and payload type combined should be either an SR or RR
	// identifier
	marker = (rtpheader->marker == 0) ? false : true;
	payloadtype = rtpheader->payloadtype;
	if (marker)
	{
		if (payloadtype == (RTP_RTCPTYPE_SR & 127)) // don't check high bit (this was the marker!!)
			return ERR_RTP_PACKET_INVALIDPACKET;
		if (payloadtype == (RTP_RTCPTYPE_RR & 127))
			return ERR_RTP_PACKET_INVALIDPACKET;
	}

	csrccount = rtpheader->csrccount;
	payloadoffset = sizeof(RTPHeader) + (int)(csrccount * sizeof(uint32_t));

	if (rtpheader->padding) // adjust payload length to take padding into account
	{
		numpadbytes = (int)packetbytes[packetlen - 1]; // last byte contains number of padding bytes
		if (numpadbytes <= 0)
			return ERR_RTP_PACKET_INVALIDPACKET;
	}
	else
		numpadbytes = 0;

	hasextension = (rtpheader->extension == 0) ? false : true;
	if (hasextension) // got header extension
	{
		rtpextheader = (RTPExtensionHeader*)(packetbytes + payloadoffset);
		payloadoffset += sizeof(RTPExtensionHeader);
		exthdrlen = ntohs(rtpextheader->length);
		payloadoffset += ((int)exthdrlen) * sizeof(uint32_t);
	}
	else
	{
		rtpextheader = 0;
		exthdrlen = 0;
	}

	payloadlength = packetlen - numpadbytes - payloadoffset;
	if (payloadlength < 0)
		return ERR_RTP_PACKET_INVALIDPACKET;

	// Now, we've got a valid packet, so we can create a new instance of RTPPacket
	// and fill in the members

	RTPPacket::hasextension = hasextension;
	if (hasextension)
	{
		RTPPacket::extid = ntohs(rtpextheader->extid);
		RTPPacket::extensionlength = ((int)ntohs(rtpextheader->length)) * sizeof(uint32_t);
		RTPPacket::extension = ((uint8_t*)rtpextheader) + sizeof(RTPExtensionHeader);
	}

	RTPPacket::hasmarker = marker;
	RTPPacket::numcsrcs = csrccount;
	RTPPacket::payloadtype = payloadtype;

	// Note: we don't fill in the EXTENDED sequence number here, since we
	// don't have information about the source here. We just fill in the low
	// 16 bits
	RTPPacket::extseqnr = (uint32_t)ntohs(rtpheader->sequencenumber);

	RTPPacket::timestamp = ntohl(rtpheader->timestamp);
	RTPPacket::ssrc = ntohl(rtpheader->ssrc);
	RTPPacket::packet = packetbytes;
	RTPPacket::payload = packetbytes + payloadoffset;
	RTPPacket::packetlength = packetlen;
	RTPPacket::payloadlength = payloadlength;

	// We'll zero the data of the raw packet, since we're using it here now!
	rawpack.ZeroData();

	return 0;
}

uint32_t RTPPacket::GetCSRC(int num) const
{
	if (num >= numcsrcs)
		return 0;

	uint8_t* csrcpos;
	uint32_t* csrcval_nbo;
	uint32_t csrcval_hbo;

	csrcpos = packet + sizeof(RTPHeader) + num * sizeof(uint32_t);
	csrcval_nbo = (uint32_t*)csrcpos;
	csrcval_hbo = ntohl(*csrcval_nbo);
	return csrcval_hbo;
}