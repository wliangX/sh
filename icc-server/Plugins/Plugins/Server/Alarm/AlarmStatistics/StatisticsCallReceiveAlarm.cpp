#include "Boost.h"
#include "StatisticsCallReceiveAlarm.h"
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
	CStatisticsCallReceiveAlarm::CStatisticsCallReceiveAlarm(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{		
		m_strStatisticsType = STATISTICS_TYPE_RECEIVE_ALARM_CALL;
		m_strStatisticCountSqlId = "statistics_receive_valid_alarm_call_count";
	}

	CStatisticsCallReceiveAlarm::~CStatisticsCallReceiveAlarm()
	{

	}	
}

