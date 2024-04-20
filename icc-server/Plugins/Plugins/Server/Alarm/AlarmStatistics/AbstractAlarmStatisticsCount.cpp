#include "Boost.h"
#include "AbstractAlarmStatisticsCount.h"
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
	CAbstractAlarmStatisticsCount::CAbstractAlarmStatisticsCount(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatistics(pLog, pDBConn, pStringUtil, pDateTime)
	{
		
	}

	CAbstractAlarmStatisticsCount::~CAbstractAlarmStatisticsCount()
	{

	}	

	std::string CAbstractAlarmStatisticsCount::_StatisticsData(const std::string& strDate)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strStatisticCountSqlId;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		std::string strResultFiledName = "count";
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("statistics count failed!, id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
			strResult = "0";
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("statistics count success! count = %s, id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}

		return strResult;
	}
}

