#include "Boost.h"
#include "StatisticsWaitingCallCount.h"
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
	CStatisticsWaitingCallCount::CStatisticsWaitingCallCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_QUEUE_CALL;
		m_strStatisticCountSqlId = "statistics_count_by_waiting_call";
	}

	CStatisticsWaitingCallCount::~CStatisticsWaitingCallCount()
	{

	}


}