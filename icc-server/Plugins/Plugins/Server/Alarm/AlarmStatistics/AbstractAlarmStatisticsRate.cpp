#include "Boost.h"
#include "AbstractAlarmStatisticsRate.h"
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
	CAbstractStatisticsRate::CAbstractStatisticsRate(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractAlarmStatistics(pLog, pDBConn, pStringUtil, pDateTime)
	{
		//m_strStatisticsType = STATISTICS_TYPE_RECEIVE_ALARM_RATE;
	}

	CAbstractStatisticsRate::~CAbstractStatisticsRate()
	{

	}

	std::string CAbstractStatisticsRate::_StatisticsMolecularValue(const std::string& strDate)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strStatisticsMolecularValueSqlId;//"statistics_receive_valid_alarm_call_count";
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		std::string strResultFiledName = "count";
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("statistics_molecular_count failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
			strResult = "0";
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("statistics_molecular_count success! count = %s id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}

		return strResult;
	}

	std::string CAbstractStatisticsRate::_StatisticsDenominatorValue(const std::string& strDate)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strStatisticsDenominatorValueSqlId;//"statistics_call_in_count";
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		std::string strResultFiledName = "count";
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("statistics_denominator_count failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
			strResult = "0";
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("statistics_denominator_count success! count = %s, id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}

		return strResult;
	}

	std::string CAbstractStatisticsRate::_CalculateRate(const std::string& strReceiveAlarmCount, const std::string& strCallInTotalCount)
	{
		if (strReceiveAlarmCount.empty() || strCallInTotalCount.empty())
		{
			return "0";
		}

		float fReceiveAlarmCount = std::stof(strReceiveAlarmCount);
		float fCallInTotalCount = std::stof(strCallInTotalCount);

		if (fCallInTotalCount > 0)
		{
			float fResult = fReceiveAlarmCount * 100.0 / fCallInTotalCount;
			std::string strResult = m_pStringUtil->Format("%0.2f%%", fResult);
			return strResult;
		}

		return "0";
	}

	std::string CAbstractStatisticsRate::_StatisticsData(const std::string& strDate)
	{
		std::string strReceiveAlarmCount = _StatisticsMolecularValue(strDate);
		std::string strCallInTotalCount = _StatisticsDenominatorValue(strDate);
		std::string strResult = _CalculateRate(strReceiveAlarmCount, strCallInTotalCount);

		return strResult;
	}

}