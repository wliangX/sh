#include "Boost.h"
#include "StatisticsTotalFeedBack.h"
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
	CStatisticsTotalFeedBack::CStatisticsTotalFeedBack(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_FEEDBACK_ALARM_TOTAL;
		m_strStatisticCountSqlId = "statistics_feedback_alarm_count";
	}

	CStatisticsTotalFeedBack::~CStatisticsTotalFeedBack()
	{

	}


}