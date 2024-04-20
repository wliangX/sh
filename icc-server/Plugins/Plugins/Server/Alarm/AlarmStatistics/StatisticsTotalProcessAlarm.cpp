#include "Boost.h"
#include "StatisticsTotalProcessAlarm.h"
#include "StatisticsCommonDef.h"
#include "StatisticsCommonTime.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/






	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsTotalProcessAlarm::CStatisticsTotalProcessAlarm(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_PROCESS_ALARM_TOTAL;
		m_strStatisticCountSqlId = "statistics_process_alarm_count";

		m_strNeedScanUpdateTimeFlag = FLAG_TRUE;
		m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_alarmtable";
	}

	CStatisticsTotalProcessAlarm::~CStatisticsTotalProcessAlarm()
	{

	}


}