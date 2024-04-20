#pragma once
#include <RTPRawPacket.h>

//RTP的相关定义
#define RTP_VERSION							2
#define RTP_RTCPTYPE_SR							200
#define RTP_RTCPTYPE_RR							201
#define RTP_RTCPTYPE_SDES						202
#define RTP_RTCPTYPE_BYE						203
#define RTP_RTCPTYPE_APP						204

//RTP的错误定义
#define ERR_RTP_PACKET_INVALIDPACKET				-25

class RTPPacket
{
public:
	/** Creates an RTPPacket instance based upon the data in \c rawpack, optionally installing a memory manager.
	 *  Creates an RTPPacket instance based upon the data in \c rawpack, optionally installing a memory manager.
	 *  If successful, the data is moved from the raw packet to the RTPPacket instance.
	 */
	RTPPacket(RTPRawPacket& rawpack);

	virtual ~RTPPacket() {
		if (packet && !externalbuffer) { delete[] packet; packet = NULL; }
	}

	/** If an error occurred in one of the constructors, this function returns the error code. */
	int GetCreationError() const { return error; }

	/** Returns \c true if the RTP packet has a header extension and \c false otherwise. */
	bool HasExtension() const { return hasextension; }

	/** Returns \c true if the marker bit was set and \c false otherwise. */
	bool HasMarker() const { return hasmarker; }

	/** Returns the number of CSRCs contained in this packet. */
	int GetCSRCCount() const { return numcsrcs; }

	/** Returns a specific CSRC identifier.
	 *  Returns a specific CSRC identifier. The parameter \c num can go from 0 to GetCSRCCount()-1.
	 */
	uint32_t GetCSRC(int num) const;

	/** Returns the payload type of the packet. */
	uint8_t GetPayloadType() const { return payloadtype; }

	/** Returns the extended sequence number of the packet.
	 *  Returns the extended sequence number of the packet. When the packet is just received,
	 *  only the low $16$ bits will be set. The high 16 bits can be filled in later.
	 */
	uint32_t GetExtendedSequenceNumber() const { return extseqnr; }

	/** Returns the sequence number of this packet. */
	uint16_t GetSequenceNumber() const { return (uint16_t)(extseqnr & 0x0000FFFF); }

	/** Sets the extended sequence number of this packet to \c seq. */
	void SetExtendedSequenceNumber(uint32_t seq) { extseqnr = seq; }

	/** Returns the timestamp of this packet. */
	uint32_t GetTimestamp() const { return timestamp; }

	/** Returns the SSRC identifier stored in this packet. */
	uint32_t GetSSRC() const { return ssrc; }

	/** Returns a pointer to the data of the entire packet. */
	uint8_t* GetPacketData() const { return packet; }

	/** Returns a pointer to the actual payload data. */
	uint8_t* GetPayloadData() const { return payload; }

	/** Returns the length of the entire packet. */
	size_t GetPacketLength() const { return packetlength; }

	/** Returns the payload length. */
	size_t GetPayloadLength() const { return payloadlength; }

	/** If a header extension is present, this function returns the extension identifier. */
	uint16_t GetExtensionID() const { return extid; }

	/** Returns the length of the header extension data. */
	uint8_t* GetExtensionData() const { return extension; }

	/** Returns the length of the header extension data. */
	size_t GetExtensionLength() const { return extensionlength; }


	/** Returns the time at which this packet was received.
	 *  When an RTPPacket instance is created from an RTPRawPacket instance, the raw packet's
	 *  reception time is stored in the RTPPacket instance. This function then retrieves that
	 *  time.
	 */
	RTPTime GetReceiveTime() const { return receivetime; }
private:
	void Clear();
	int ParseRawPacket(RTPRawPacket& rawpack);

	int error;

	bool hasextension, hasmarker;
	int numcsrcs;

	uint8_t payloadtype;
	uint32_t extseqnr, timestamp, ssrc;
	uint8_t* packet, * payload;
	size_t packetlength, payloadlength;

	uint16_t extid;
	uint8_t* extension;
	size_t extensionlength;

	bool externalbuffer;

	RTPTime receivetime;
};