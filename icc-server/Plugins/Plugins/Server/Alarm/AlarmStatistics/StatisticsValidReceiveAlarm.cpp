#include "Boost.h"
#include "StatisticsValidReceiveAlarm.h"
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
	CStatisticsValidReceiveAlarm::CStatisticsValidReceiveAlarm(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_VALID_ALARM;
		m_strStatisticCountSqlId = "statistics_valid_alarm_count";
	}

	CStatisticsValidReceiveAlarm::~CStatisticsValidReceiveAlarm()
	{

	}	

	
}