#include "Boost.h"
#include "StatisticsTotalCallIn.h"
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
	CStatisticsTotalCallIn::CStatisticsTotalCallIn(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_TOTAL_CALL_IN;
		m_strStatisticCountSqlId = "statistics_call_in_count";
	}

	CStatisticsTotalCallIn::~CStatisticsTotalCallIn()
	{

	}


}