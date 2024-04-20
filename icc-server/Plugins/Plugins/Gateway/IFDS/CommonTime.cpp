#include "CommonTime.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
std::time_t CCommonTime::ToTime_t()
{
	return std::chrono::system_clock::to_time_t(m_curtp);
}

long long CCommonTime::Microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(m_curtp.time_since_epoch()).count();
}

long long CCommonTime::Millseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(m_curtp.time_since_epoch()).count();
}

int CCommonTime::Year()
{
	return m_pcurTm->tm_year + 1900;
}

int CCommonTime::Month()
{
	return m_pcurTm->tm_mon + 1;
}

int CCommonTime::Day()
{
	return m_pcurTm->tm_mday;
}

int CCommonTime::Hour()
{
	return m_pcurTm->tm_hour;
}

int CCommonTime::Minute()
{
	return m_pcurTm->tm_min;
}

int CCommonTime::Second()
{
	return m_pcurTm->tm_sec;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CCommonTime::CCommonTime()
{
	m_curtp = std::chrono::system_clock::now();
	std::time_t curTmpTime = std::chrono::system_clock::to_time_t(m_curtp);	
	m_pcurTm = std::localtime(&curTmpTime);
}

CCommonTime::CCommonTime(std::time_t t)
{
	m_pcurTm = std::localtime(&t);
	m_curtp = std::chrono::system_clock::from_time_t(t);
}

CCommonTime::~CCommonTime()
{

}