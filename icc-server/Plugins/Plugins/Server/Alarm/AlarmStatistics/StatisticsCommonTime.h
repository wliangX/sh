#ifndef __Statistics_CommonTime_H__
#define __Statistics_CommonTime_H__

#include <string>

namespace ICC
{
	class CStatisticsCommonTime
	{
	public:
		static std::string DateBeginString(DateTime::IDateTimePtr pDateTime, StringUtil::IStringUtilPtr pStringUtil);
		static std::string DateEndString(DateTime::IDateTimePtr pDateTime, StringUtil::IStringUtilPtr pStringUtil);		
		static std::string StatisticsDateString(DateTime::IDateTimePtr pDateTime);

		static std::string CurrentDateString(DateTime::IDateTimePtr pDateTime);
		static std::string PreDateString(DateTime::IDateTimePtr pDateTime);
		static std::string StatisticsBeginString(const std::string& strDate, StringUtil::IStringUtilPtr pStringUtil);
		static std::string StatisticsEndString(const std::string& strDate, StringUtil::IStringUtilPtr pStringUtil);

		static std::string CurrentHours(DateTime::IDateTimePtr pDateTime);
		static std::string CurrentMinute(DateTime::IDateTimePtr pDateTime);

	private:
		static std::string _StatisticsDateString(DateTime::IDateTimePtr pDateTime);

	private:
		CStatisticsCommonTime();
		~CStatisticsCommonTime();	

	};
};

#endif