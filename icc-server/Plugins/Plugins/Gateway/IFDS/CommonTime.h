#ifndef __CommonTime_H__
#define __CommonTime_H__

#include <ctime>
#include <chrono>

class CCommonTime
{
public:
	std::time_t ToTime_t();
	long long   Millseconds();
	long long   Microseconds();

	int         Year();
	int         Month();
	int         Day();
	int         Hour();
	int         Minute();
	int         Second();
	

public:
	CCommonTime();
	CCommonTime(std::time_t t);	
	~CCommonTime();

private:
	std::chrono::system_clock::time_point m_curtp;
	std::tm* m_pcurTm;
};

#endif