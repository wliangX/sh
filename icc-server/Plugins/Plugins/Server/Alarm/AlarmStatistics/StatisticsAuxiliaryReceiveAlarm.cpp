#include "Boost.h"
#include "StatisticsAuxiliaryReceiveAlarm.h"
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
	CStatisticsAuxiliaryReceiveAlarm::CStatisticsAuxiliaryReceiveAlarm(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_AUXILIARY_ALARM;
		m_strStatisticCountSqlId = "statistics_auxiliaryreceivealarm_count";
	}

	CStatisticsAuxiliaryReceiveAlarm::~CStatisticsAuxiliaryReceiveAlarm()
	{

	}


}