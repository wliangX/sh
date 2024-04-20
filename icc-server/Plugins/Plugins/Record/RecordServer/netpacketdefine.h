#pragma once
#if defined(WIN32)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
//typedef long long int64_t;
//typedef unsigned long long uint64_t;
#endif

/*
 * Definitions for internet protocol version 4.
 * Per RFC 791, September 1981.
 */
#define	IPVERSION	4
#pragma pack(push)
#pragma pack(1)

#define	IP_V4 0x0008			/* ipv4 */
#define	IP_V6 0xDD86			/* ipv6 */
struct ETHERNETHEADER {
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t ipversion;
};

struct IPAddr {
	union {
		struct {
			uint8_t s_b1;
			uint8_t s_b2;
			uint8_t s_b3;
			uint8_t s_b4;
		} S_un_b;
		struct {
			uint16_t s_w1;
			uint16_t s_w2;
		} S_un_w;
		uint32_t S_addr;
	} S_un;
};

 /*
  * Structure of an internet header, naked of options.
  *
  * We declare ip_len and ip_off to be short, rather than u_short
  * pragmatically since otherwise unsigned comparisons can result
  * against negative integers quite easily, and fail in subtle ways.
  */
struct IPHeader {
#ifdef RTP_BIG_ENDIAN
	uint8_t		ip_version : 2;		        /* version*/
	uint8_t		ip_headerlength : 2;		/* header length*/
#else
	uint8_t		ip_headerlength : 2;		/* header length*/
	uint8_t		ip_version:2;		        /* version*/
#endif
#define IP_V(ip)	(((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip)	((ip)->ip_vhl & 0x0f)
	uint8_t		ip_tos;		        /* type of service */
	uint16_t	ip_len;		        /* total length */
	uint16_t	ip_id;		        /* identification */
	uint16_t	ip_off;		        /* fragment offset field */
#define	IP_DF 0x4000			    /* dont fragment flag */
#define	IP_MF 0x2000			    /* more fragments flag */
#define	IP_OFFMASK 0x1fff		    /* mask for fragmenting bits */
	uint8_t		ip_ttl;		        /* time to live */
	uint8_t		ip_p;		        /* protocol */
	uint16_t	ip_sum;		        /* checksum */
	struct IPAddr ip_src, ip_dst;	/* source and dest address */
};

struct UDPHeader {
	uint16_t	uh_sport;		/* source port */
	uint16_t	uh_dport;		/* destination port */
	uint16_t	uh_ulen;		/* udp length */
	uint16_t	uh_sum;			/* udp checksum */
};


typedef	uint32_t	tcp_seq;
/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct TCPHeader {
	uint16_t	th_sport;		/* source port */
	uint16_t	th_dport;		/* destination port */
	tcp_seq		th_seq;			/* sequence number */
	tcp_seq		th_ack;			/* acknowledgement number */
	uint8_t		th_offx2;		/* data offset, rsvd */
	uint8_t		th_flags;
	uint16_t	th_win;			/* window */
	uint16_t	th_sum;			/* checksum */
	uint16_t	th_urp;			/* urgent pointer */
};


struct RTPHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version : 2;
	uint8_t padding : 1;
	uint8_t extension : 1;
	uint8_t csrccount : 4;

	uint8_t marker : 1;
	uint8_t payloadtype : 7;
#else // little endian
	uint8_t csrccount : 4;
	uint8_t extension : 1;
	uint8_t padding : 1;
	uint8_t version : 2;

	uint8_t payloadtype : 7;
	uint8_t marker : 1;
#endif // RTP_BIG_ENDIAN

	uint16_t sequencenumber;
	uint32_t timestamp;
	uint32_t ssrc;
};

struct RTPExtensionHeader
{
	uint16_t extid;
	uint16_t length;
};

struct RTPSourceIdentifier
{
	uint32_t ssrc;
};

struct RTCPCommonHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version : 2;
	uint8_t padding : 1;
	uint8_t count : 5;
#else // little endian
	uint8_t count : 5;
	uint8_t padding : 1;
	uint8_t version : 2;
#endif // RTP_BIG_ENDIAN

	uint8_t packettype;
	uint16_t length;
};

struct RTCPSenderReport
{
	uint32_t ntptime_msw;
	uint32_t ntptime_lsw;
	uint32_t rtptimestamp;
	uint32_t packetcount;
	uint32_t octetcount;
};

struct RTCPReceiverReport
{
	uint32_t ssrc; // Identifies about which SSRC's data this report is...
	uint8_t fractionlost;
	uint8_t packetslost[3];
	uint32_t exthighseqnr;
	uint32_t jitter;
	uint32_t lsr;
	uint32_t dlsr;
};

struct RTCPSDESHeader
{
	uint8_t sdesid;
	uint8_t length;
};
#pragma pack(pop)
