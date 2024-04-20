#include "Boost.h"
#include "StatisticsCommonTime.h"

namespace ICC
{
#define DATE_FORMAT_LEN 10
#define HOUR_LOCATION_BEGIN 11
#define MINUTE_LOCATION_BEGIN 14
#define HOUR_LEN 2

	std::string CStatisticsCommonTime::DateBeginString(DateTime::IDateTimePtr pDateTime, StringUtil::IStringUtilPtr pStringUtil)
	{
		std::string strTmpDate = CStatisticsCommonTime::_StatisticsDateString(pDateTime);
		std::string strBeginTime = pStringUtil->Format("%s 00:00:00", strTmpDate.c_str());
		return strBeginTime;
	}

	

	std::string CStatisticsCommonTime::DateEndString(DateTime::IDateTimePtr pDateTime, StringUtil::IStringUtilPtr pStringUtil)
	{		
		std::string strTmpDate = CStatisticsCommonTime::_StatisticsDateString(pDateTime);
		std::string strEndTime = pStringUtil->Format("%s 23:59:59", strTmpDate.c_str());
		return strEndTime;
	}

	std::string CStatisticsCommonTime::StatisticsDateString(DateTime::IDateTimePtr pDateTime)
	{
		return _StatisticsDateString(pDateTime);
	}

	std::string CStatisticsCommonTime::CurrentDateString(DateTime::IDateTimePtr pDateTime)
	{
		std::string strCurDate = pDateTime->CurrentDateTimeStr();
		std::string strTmpDate;
		if (strCurDate.length() > DATE_FORMAT_LEN)
		{
			strTmpDate = strCurDate.substr(0, DATE_FORMAT_LEN);
		}
		return strTmpDate;
	}

	std::string CStatisticsCommonTime::PreDateString(DateTime::IDateTimePtr pDateTime)
	{
		DateTime::CDateTime curdt = pDateTime->CurrentDateTime();
		DateTime::CDateTime statisticsDt = pDateTime->AddDays(curdt, -1);
		std::string strTmpTime = pDateTime->ToString(statisticsDt, DateTime::DATETIME_STRING_FORMAT_NO_MICROSECOND);
		std::string strTmpDate;
		if (strTmpTime.length() > DATE_FORMAT_LEN)
		{
			strTmpDate = strTmpTime.substr(0, DATE_FORMAT_LEN);
		}

		return strTmpDate;
	}

	std::string CStatisticsCommonTime::StatisticsBeginString(const std::string& strDate, StringUtil::IStringUtilPtr pStringUtil)
	{
		std::string strBeginTime = pStringUtil->Format("%s 00:00:00", strDate.c_str());
		return strBeginTime;
	}

	std::string CStatisticsCommonTime::StatisticsEndString(const std::string& strDate, StringUtil::IStringUtilPtr pStringUtil)
	{
		std::string strEndTime = pStringUtil->Format("%s 23:59:59", strDate.c_str());
		return strEndTime;
	}

	std::string CStatisticsCommonTime::CurrentHours(DateTime::IDateTimePtr pDateTime)
	{
		std::string strCurDate = pDateTime->CurrentDateTimeStr();
		std::string strTmpDate;
		if (strCurDate.length() > (HOUR_LOCATION_BEGIN + HOUR_LEN))
		{
			strTmpDate = strCurDate.substr(HOUR_LOCATION_BEGIN, HOUR_LEN);
		}
		return strTmpDate;
	}

	std::string CStatisticsCommonTime::CurrentMinute(DateTime::IDateTimePtr pDateTime)
	{
		std::string strCurDate = pDateTime->CurrentDateTimeStr();
		std::string strTmpDate;
		if (strCurDate.length() > (MINUTE_LOCATION_BEGIN + HOUR_LEN))
		{
			strTmpDate = strCurDate.substr(MINUTE_LOCATION_BEGIN, HOUR_LEN);
		}
		return strTmpDate;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCommonTime::CStatisticsCommonTime()
	{

	}

	CStatisticsCommonTime::~CStatisticsCommonTime()
	{

	}

	std::string CStatisticsCommonTime::_StatisticsDateString(DateTime::IDateTimePtr pDateTime)
	{
		DateTime::CDateTime curdt = pDateTime->CurrentDateTime();
		DateTime::CDateTime statisticsDt = pDateTime->AddDays(curdt, -1);
		std::string strTmpTime = pDateTime->ToString(statisticsDt, DateTime::DATETIME_STRING_FORMAT_NO_MICROSECOND);
		std::string strTmpDate;
		if (strTmpTime.length() > DATE_FORMAT_LEN)
		{
			strTmpDate = strTmpTime.substr(0, DATE_FORMAT_LEN);
		}
		
		return strTmpDate;
	}
}



