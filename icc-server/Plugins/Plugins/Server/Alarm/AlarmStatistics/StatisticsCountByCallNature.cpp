#include "Boost.h"
#include "StatisticsCountByCallNature.h"
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
	CStatisticsCallNatureByAccept::CStatisticsCallNatureByAccept(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_NATURE_RECEIVE_CALL;
		m_strStatisticCountSqlId = "statistics_receive_valid_alarm_call_count";
		m_strRemark = "accept";
	}

	CStatisticsCallNatureByAccept::~CStatisticsCallNatureByAccept()
	{

	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCallNatureByRelease::CStatisticsCallNatureByRelease(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_NATURE_RELEASE_CALL;
		m_strStatisticCountSqlId = "statistics_count_by_release_call";
		m_strRemark = "release";
	}

	CStatisticsCallNatureByRelease::~CStatisticsCallNatureByRelease()
	{

	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCallNatureByDisturbance::CStatisticsCallNatureByDisturbance(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_NATURE_DISTURBANCE_CALL;
		m_strStatisticCountSqlId = "statistics_count_by_disturbance_call";
		m_strRemark = "disturbance";
	}

	CStatisticsCallNatureByDisturbance::~CStatisticsCallNatureByDisturbance()
	{

	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCallNatureByInside::CStatisticsCallNatureByInside(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_NATURE_INSIDE_CALL;
		m_strStatisticCountSqlId = "statistics_count_by_inside_call";
		m_strRemark = "inside";
	}

	CStatisticsCallNatureByInside::~CStatisticsCallNatureByInside()
	{

	}
}