#pragma once
#include <netpacketdefine.h>
#include <string>

struct RTPIPAddress
{
	RTPIPAddress(const std::string& paraStrIp, uint16_t paraUsPort) :strIP(paraStrIp), usPort(paraUsPort) {}
	~RTPIPAddress() {}
	std::string strIP;
	uint16_t  usPort;
};
class RTPTime
{
public:
	static RTPTime CurrentTime();

	bool operator<(const RTPTime& other)
	{
		if (ulSec < other.ulSec)
		{
			return true;
		}
		else if (ulSec == other.ulSec)
		{
			return ulmSec < other.ulmSec;
		}
		return false;
	}
	bool operator == (const RTPTime& other)
	{
		return ulSec == other.ulSec && ulmSec == other.ulmSec;
	}
	bool operator > (const RTPTime& other)
	{
		if (ulSec > other.ulSec)
		{
			return true;
		}
		else if (ulSec == other.ulSec)
		{
			return ulmSec > other.ulmSec;
		}
		return false;
	}

	//增加毫秒数
	RTPTime operator + (unsigned long paraMSec)
	{
		unsigned long ulNewmSec = ulmSec + paraMSec * 1000;
		unsigned long ulAddSec = ulNewmSec / 1000000;
		ulmSec = ulNewmSec % 1000000;
		ulSec += ulAddSec;
	}

	//增加毫秒数
	RTPTime& operator += (unsigned long paraMSec)
	{
		unsigned long ulNewmSec = ulmSec + paraMSec * 1000;
		unsigned long ulAddSec = ulNewmSec / 1000000;
		ulmSec = ulNewmSec % 1000000;
		ulSec += ulAddSec;
		return *this;
	}

	//增加毫秒数
	RTPTime operator + (unsigned short paraMSec)
	{
		unsigned long ulNewmSec = ulmSec + paraMSec * 1000;
		unsigned long ulAddSec = ulNewmSec / 1000000;
		unsigned long ultmSec = ulNewmSec % 1000000;
		unsigned long ultSec = ulSec + ulAddSec;
		return RTPTime(ultSec, ultmSec);
	}

	//增加毫秒数
	RTPTime& operator += (unsigned short paraMSec)
	{
		unsigned long ulNewmSec = ulmSec + paraMSec * 1000;
		unsigned long ulAddSec = ulNewmSec / 1000000;
		ulmSec = ulNewmSec % 1000000;
		ulSec += ulAddSec;
		return *this;
	}


	/** Creates an instance corresponding to \c seconds and \c microseconds. */
	RTPTime(unsigned long seconds, unsigned long microseconds) { ulSec = seconds; ulmSec = microseconds; }

	/** Returns the number of seconds stored in this instance. */
	unsigned long GetSeconds() const { return ulSec; }

	/** Returns the number of microseconds stored in this instance. */
	unsigned long GetMicroSeconds() const { return ulmSec; }

	//YYYYmmDDHHMMSSiii
	std::string ToString();
	std::string ToDateTimeString();

	/** Return the RTPTime is valid or invalid **/
	bool IsInValidTime() { return ulSec == 0 && ulmSec == 0; }

private:
#if (defined(WIN32) || defined(WIN64) || defined(_WIN32_WCE))
	static inline unsigned __int64 CalculateMicroseconds(unsigned __int64 performancecount, unsigned __int64 performancefrequency);
#endif // WIN32 || _WIN32_WCE

	unsigned long ulSec; //秒数
	unsigned long ulmSec; //微秒数
};

/** This class is used by the transmission component to store the incoming RTP and RTCP data in. */
class RTPRawPacket 
{
public:
	/** Creates an instance which stores data from \c data with length \c datalen.
 *  Creates an instance which stores data from \c data with length \c datalen. Only the pointer
 *  to the data is stored, no actual copy is made! The address from which this packet originated
 *  is set to \c address and the time at which the packet was received is set to \c recvtime.
 *  The flag which indicates whether this data is RTP or RTCP data is set to \c rtp. A memory
 *  manager can be installed as well.
 */
	RTPRawPacket(uint8_t* data, size_t datalen, const RTPIPAddress& srcaddress, const RTPIPAddress& dstaddress, RTPTime& recvtime, bool rtp);
	~RTPRawPacket();

	/** Returns the pointer to the data which is contained in this packet. */
	uint8_t* GetData() { return packetdata; }

	/** Returns the length of the packet described by this instance. */
	size_t GetDataLength() const { return packetdatalength; }

	/** Returns the time at which this packet was received. */
	RTPTime GetReceiveTime() const { return receivetime; }

	/** Returns the address stored in this packet. */
	const RTPIPAddress GetSenderAddress() const { return senderaddress; }

	/** Returns the address stored in this packet. */
	const RTPIPAddress GetReceiverAddress() const { return receiveraddress; }

	/** Returns \c true if this data is RTP data, \c false if it is RTCP data. */
	bool IsRTP() const { return isrtp; }

	/** Sets the pointer to the data stored in this packet to zero.
	 *  Sets the pointer to the data stored in this packet to zero. This will prevent
	 *  a \c delete call for the actual data when the destructor of RTPRawPacket is called.
	 *  This function is used by the RTPPacket and RTCPCompoundPacket classes to obtain
	 *  the packet data (without having to copy it)	and to make sure the data isn't deleted
	 *  when the destructor of RTPRawPacket is called.
	 */
	void ZeroData() { packetdata = NULL; packetdatalength = 0; }
private:
	uint8_t* packetdata;
	size_t packetdatalength;
	RTPTime receivetime;
	RTPIPAddress senderaddress;
	RTPIPAddress receiveraddress;
	bool isrtp;
};