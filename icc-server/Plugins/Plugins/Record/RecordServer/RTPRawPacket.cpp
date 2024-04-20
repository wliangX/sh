#include <RTPRawPacket.h>
#include <time.h>
#include <ctime>
#ifndef WIN32
#include <sys/time.h>
#else
#ifndef _WIN32_WCE
#include <windows.h>
#include <sys/timeb.h>
#endif // _WIN32_WINCE
#endif // WIN32


#if (defined(WIN32) || defined(WIN64)  || defined(_WIN32_WCE))
inline unsigned __int64 RTPTime::CalculateMicroseconds(unsigned __int64 performancecount, unsigned __int64 performancefrequency)
{
	unsigned __int64 f = performancefrequency;
	unsigned __int64 a = performancecount;
	unsigned __int64 b = a / f;
	unsigned __int64 c = a % f; // a = b*f+c => (a*1000000)/f = b*1000000+(c*1000000)/f

	return b * 1000000ui64 + (c * 1000000ui64) / f;
}
#endif

RTPTime RTPTime::CurrentTime()
{
#if defined(WIN32)||defined(WIN64)
	static int inited = 0;
	static unsigned __int64 microseconds, initmicroseconds;
	static LARGE_INTEGER performancefrequency;

	unsigned __int64 emulate_microseconds, microdiff;
	SYSTEMTIME systemtime;
	FILETIME filetime;

	LARGE_INTEGER performancecount;

	QueryPerformanceCounter(&performancecount);

	if (!inited) {
		inited = 1;
		QueryPerformanceFrequency(&performancefrequency);
		GetSystemTime(&systemtime);
		SystemTimeToFileTime(&systemtime, &filetime);
		microseconds = (((unsigned __int64)(filetime.dwHighDateTime) << 32) + (unsigned __int64)(filetime.dwLowDateTime)) / 10ui64;
		microseconds -= 11644473600000000ui64; // EPOCH
		initmicroseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);
	}

	emulate_microseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);

	microdiff = emulate_microseconds - initmicroseconds;

	return RTPTime((uint32_t)((microseconds + microdiff) / 1000000ui64), ((uint32_t)((microseconds + microdiff) % 1000000ui64)));
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return RTPTime((uint32_t)tv.tv_sec, (uint32_t)tv.tv_usec);
#endif

}

std::string RTPTime::ToString()
{
	char buffSec[20] = { 0 };
	char buffTime[24] = { 0 };
	time_t tSecs = ulSec;
	tm* tm_time =localtime(&tSecs);
	strftime(buffSec, 20, "%Y%m%d%H%M%S", tm_time);
	snprintf(buffTime, 24, "%s%03u", buffSec, ulmSec / 1000);
	return std::string(buffTime);
}

std::string RTPTime::ToDateTimeString()
{
	char buffSec[20] = { 0 };
	char buffTime[24] = { 0 };
	time_t tSecs = ulSec;
	tm* tm_time = localtime(&tSecs);
	strftime(buffSec, 20, "%Y-%m-%d %H:%M:%S", tm_time);
	//snprintf(buffTime, 24, "%s %03u", buffSec, ulmSec / 1000);
	return std::string(buffSec);
}

RTPRawPacket::RTPRawPacket(uint8_t* data, size_t datalen, const RTPIPAddress& srcaddress, const RTPIPAddress& dstaddress, RTPTime& recvtime, bool rtp) :senderaddress(srcaddress), receiveraddress(dstaddress),receivetime(recvtime)
{
	packetdata = data;
	packetdatalength = datalen;
	isrtp = rtp;
}

RTPRawPacket::~RTPRawPacket()
{
	if (packetdata)
	{
		delete[] packetdata;
		packetdata = NULL;
	}
}