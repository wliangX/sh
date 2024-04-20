#include "Boost.h"
#include "StatisticsCallSynthesizeByCallTime.h"
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
	CStatisticsCallSynthesizeByCallTime::CStatisticsCallSynthesizeByCallTime(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_SYNTHESIZE_CALLTIME;
		m_strStatisticCountSqlId = "statistics_talktime";
	}

	CStatisticsCallSynthesizeByCallTime::~CStatisticsCallSynthesizeByCallTime()
	{

	}


}