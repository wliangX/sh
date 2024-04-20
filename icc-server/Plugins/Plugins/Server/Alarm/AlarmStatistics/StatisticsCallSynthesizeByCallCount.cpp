#include "Boost.h"
#include "StatisticsCallSynthesizeByCallCount.h"
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
	CStatisticsCallSynthesizeByCallCount::CStatisticsCallSynthesizeByCallCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_SYNTHESIZE_CALLEVENT;
		m_strStatisticCountSqlId = "statistics_count_by_callevent";
	}

	CStatisticsCallSynthesizeByCallCount::~CStatisticsCallSynthesizeByCallCount()
	{

	}


}