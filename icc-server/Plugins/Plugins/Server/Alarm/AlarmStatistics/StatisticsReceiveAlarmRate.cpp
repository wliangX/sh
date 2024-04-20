#include "Boost.h"
#include "StatisticsReceiveAlarmRate.h"
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
	CStatisticsReceiveAlarmRate::CStatisticsReceiveAlarmRate(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractStatisticsRate(pLog, pDBConn, pStringUtil, pDateTime)
	{		
		m_strStatisticsType = STATISTICS_TYPE_RECEIVE_ALARM_RATE;

		m_strStatisticsMolecularValueSqlId = "statistics_receive_valid_alarm_call_count";
		m_strStatisticsDenominatorValueSqlId = "statistics_call_in_count";
	}

	CStatisticsReceiveAlarmRate::~CStatisticsReceiveAlarmRate()
	{

	}	


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsReceiveAlarmRateReceiveCount::CStatisticsReceiveAlarmRateReceiveCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_RECEIVE_ALARM_RATE_RECEIVECOUNT;
		m_strStatisticCountSqlId = "statistics_receive_valid_alarm_call_count";
	}

	CStatisticsReceiveAlarmRateReceiveCount::~CStatisticsReceiveAlarmRateReceiveCount()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsReceiveAlarmRateCallInCount::CStatisticsReceiveAlarmRateCallInCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_RECEIVE_ALARM_RATE_CALLINCOUNT;
		m_strStatisticCountSqlId = "statistics_call_in_count";
	}

	CStatisticsReceiveAlarmRateCallInCount::~CStatisticsReceiveAlarmRateCallInCount()
	{

	}
	
}