#ifndef __StatisticsAlarmByLevel_H__
#define __StatisticsAlarmByLevel_H__

#include "AbstractAlarmStatisticsCount.h"
#include "StatisticsCommonDef.h"
#include "StatisticsCommonTime.h"

namespace ICC
{
	class CStatisticsAlarmByFirstType : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmByFirstType(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_FIRSTTYPE_ALARM_COUNT;
			m_strStatisticCountSqlId = "statistics_alarm_count_by_first_type";
		}
		~CStatisticsAlarmByFirstType(){};

	private:
		CStatisticsAlarmByFirstType();
	};	

	class CStatisticsAlarmBySecondType : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmBySecondType(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_SECONDTYPE_ALARM_COUNT;
			m_strStatisticCountSqlId = "statistics_alarm_count_by_second_type";
		}
		~CStatisticsAlarmBySecondType(){};

	private:
		CStatisticsAlarmBySecondType();
	};

	class CStatisticsAlarmByThirdType : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmByThirdType(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_THIRDTYPE_ALARM_COUNT;
			m_strStatisticCountSqlId = "statistics_alarm_count_by_third_type";
		}
		~CStatisticsAlarmByThirdType(){};

	private:
		CStatisticsAlarmByThirdType();
	};

	class CStatisticsAlarmByFourthType : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmByFourthType(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_FOURTHTYPE_ALARM_COUNT;
			m_strStatisticCountSqlId = "statistics_alarm_count_by_fourth_type";
		}
		~CStatisticsAlarmByFourthType(){};

	private:
		CStatisticsAlarmByFourthType();
	};
}

#endif
