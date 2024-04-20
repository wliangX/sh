#include "Boost.h"
#include "StatisticsCountByCallInNumber.h"
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
	CStatisticsCountBy110::CStatisticsCountBy110(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_TOTAL_110;
		m_strStatisticCountSqlId = "statistics_count_by_callnumber_110";
		m_strRemark = "110";
	}

	CStatisticsCountBy110::~CStatisticsCountBy110()
	{

	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCountBy119::CStatisticsCountBy119(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_TOTAL_119;
		m_strStatisticCountSqlId = "statistics_count_by_callnumber_119";
		m_strRemark = "119";
	}

	CStatisticsCountBy119::~CStatisticsCountBy119()
	{

	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCountBy122::CStatisticsCountBy122(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatisticsCount(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_TOTAL_122;
		m_strStatisticCountSqlId = "statistics_count_by_callnumber_122";
		m_strRemark = "122";
	}

	CStatisticsCountBy122::~CStatisticsCountBy122()
	{

	}


}