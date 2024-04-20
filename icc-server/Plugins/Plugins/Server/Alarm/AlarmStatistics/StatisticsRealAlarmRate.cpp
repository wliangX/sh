#include "Boost.h"
#include "StatisticsRealAlarmRate.h"
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
	CStatisticsRealAlarmRate::CStatisticsRealAlarmRate(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractStatisticsRate(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_REAL_ALARM_RATE;

		m_strStatisticsMolecularValueSqlId = "statistics_real_alarm_count";
		m_strStatisticsDenominatorValueSqlId = "statistics_receive_valid_alarm_call_count";
	}

	CStatisticsRealAlarmRate::~CStatisticsRealAlarmRate()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsRealAlarmRateRealCount::CStatisticsRealAlarmRateRealCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_REAL_ALARM_RATE_REALCOUNT;
		m_strStatisticCountSqlId = "statistics_real_alarm_count";
	}

	CStatisticsRealAlarmRateRealCount::~CStatisticsRealAlarmRateRealCount()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsRealAlarmRateReceiveCount::CStatisticsRealAlarmRateReceiveCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_REAL_ALARM_RATE_RECEIVECOUNT;
		m_strStatisticCountSqlId = "statistics_receive_valid_alarm_call_count";
	}

	CStatisticsRealAlarmRateReceiveCount::~CStatisticsRealAlarmRateReceiveCount()
	{

	}

}