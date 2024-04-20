#ifndef __StatisticsAlarmByResult_H__
#define __StatisticsAlarmByResult_H__

#include "AbstractAlarmStatisticsCount.h"
#include "StatisticsCommonDef.h"
#include "StatisticsCommonTime.h"

namespace ICC
{
	class CStatisticsAlarmResultByProcessed : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmResultByProcessed(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_ALARM_RESULT_PROCESS;
			m_strStatisticCountSqlId = "statistics_alarm_result_processed_count";

			m_strNeedScanUpdateTimeFlag = FLAG_TRUE;
			m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_alarmtable";
		}
		~CStatisticsAlarmResultByProcessed(){};

	private:
		CStatisticsAlarmResultByProcessed();
	};

	class CStatisticsAlarmResultByFeedBack : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAlarmResultByFeedBack(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
			: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
		{
			m_strStatisticsType = STATISTICS_TYPE_ALARM_RESULT_FEEDBACK;
			m_strStatisticCountSqlId = "statistics_alarm_result_feekback_count";

			m_strNeedScanUpdateTimeFlag = FLAG_TRUE;
			m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_alarmtable";
		}
		~CStatisticsAlarmResultByFeedBack(){};

	private:
		CStatisticsAlarmResultByFeedBack();
	};
	
}

#endif
