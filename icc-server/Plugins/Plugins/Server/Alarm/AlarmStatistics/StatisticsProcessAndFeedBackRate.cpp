#include "Boost.h"
#include "StatisticsProcessAndFeedBackRate.h"
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
	CStatisticsProcessAndFeedBackRate::CStatisticsProcessAndFeedBackRate(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractStatisticsRate(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK;

		m_strStatisticsMolecularValueSqlId = "statistics_alarm_result_feekback_count";
		m_strStatisticsDenominatorValueSqlId = "statistics_alarm_result_processed_count";
	}

	CStatisticsProcessAndFeedBackRate::~CStatisticsProcessAndFeedBackRate()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsProcessAndFeedBackRateProcess::CStatisticsProcessAndFeedBackRateProcess(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_PROCESS;
		m_strStatisticCountSqlId = "statistics_alarm_result_processed_count";

		m_strNeedScanUpdateTimeFlag = FLAG_TRUE;
		m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_alarmtable";
	}

	CStatisticsProcessAndFeedBackRateProcess::~CStatisticsProcessAndFeedBackRateProcess()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsProcessAndFeedBackRateFeedBack::CStatisticsProcessAndFeedBackRateFeedBack(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_FEEDBACK;
		m_strStatisticCountSqlId = "statistics_alarm_result_feekback_count";

		m_strNeedScanUpdateTimeFlag = FLAG_TRUE;
		m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_alarmtable";
	}

	CStatisticsProcessAndFeedBackRateFeedBack::~CStatisticsProcessAndFeedBackRateFeedBack()
	{

	}

}