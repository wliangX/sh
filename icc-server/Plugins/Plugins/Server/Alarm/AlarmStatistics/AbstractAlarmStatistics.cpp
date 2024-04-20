#include "Boost.h"
#include "AbstractAlarmStatistics.h"
#include "StatisticsCommonTime.h"
#include "StatisticsCommonDef.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int CAbstractAlarmStatistics::Statistics()
	{
		_PrcessPreDateStatistics();

		if (m_strNeedStatisticsCurrentDayFlag == FLAG_TRUE)
		{
			_ProcessCurrDateStatistics();
		}	

		if (m_strNeedScanUpdateTimeFlag == FLAG_TRUE)
		{
			_ProcessStatusUpdateStatistics();
		}

		return 0;
	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CAbstractAlarmStatistics::CAbstractAlarmStatistics(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: m_pLog(pLog), m_pDBConn(pDBConn), m_pStringUtil(pStringUtil), m_pDateTime(pDateTime)
	{
		m_strInsertSqlName = "insert_statistics_result";
		m_strQueryResultSqlName = "select_statistics_result";
		m_strUpdateSqlName = "update_statistics_result";

		m_pStatisticsDb = boost::make_shared<CDbBase>(pLog, pDBConn, pStringUtil);
	}

	CAbstractAlarmStatistics::~CAbstractAlarmStatistics()
	{

	}

	int CAbstractAlarmStatistics::_PrcessPreDateStatistics()
	{
		std::string strPreDate = CStatisticsCommonTime::PreDateString(m_pDateTime);		

		std::string strPreResult = _QueryStatisticsData(strPreDate);
		std::string strNewResult = _StatisticsData(strPreDate);
		if (!strPreResult.empty())
		{
			if (strNewResult != strPreResult)
			{
				_UpdateStatisticsResult(strNewResult, strPreDate);
			}
		}
		else
		{
			_InsertStatisticsResult(strNewResult, strPreDate);
		}
		return 0;
	}

	int CAbstractAlarmStatistics::_ProcessCurrDateStatistics()
	{
		std::string strCurrDate = CStatisticsCommonTime::CurrentDateString(m_pDateTime);
		std::string strCurrResult = _QueryStatisticsData(strCurrDate);
		std::string strNewResult = _StatisticsData(strCurrDate);
		if (!strCurrResult.empty())
		{
			if (strNewResult != strCurrResult)
			{
				_UpdateStatisticsResult(strNewResult, strCurrDate);
			}
		}
		else
		{
			_InsertStatisticsResult(strNewResult, strCurrDate);
		}
		return 0;
	}

	bool CAbstractAlarmStatistics::_InsertStatisticsResult(const std::string& strResult, const std::string& strTmpDate)
	{
		std::string strStatisticsId = m_pStringUtil->Format("%s-%s-%s", strTmpDate.c_str(), m_strStatisticsType.c_str(), m_pStringUtil->CreateGuid().c_str());
		std::string strTime = m_pDateTime->CurrentDateTimeStr();

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strInsertSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_ID] = strStatisticsId;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = m_strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strTmpDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_RESULT] = strResult;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TIME] = strTime;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_REMARK] = m_strRemark;

		std::string strTmpLog;
		bool bExecResult = _ExecSql(l_SqlRequest);
		if (bExecResult)
		{
			strTmpLog = m_pStringUtil->Format("insert_statistics_result success! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
			return true;
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("insert_statistics_result failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
			return false;
		}
	}

	std::string CAbstractAlarmStatistics::_QueryStatisticsData(const std::string& strDate)
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strQueryResultSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = m_strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;

		std::string strTmpLog;
		std::string strResultFiledName = STATISTICS_FIELD_NAME_STATISTICS_RESULT;
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("select_statistics_result complete, result is null! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str())
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("select_statistics_result success! count = %s, id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str())
		}

		return strResult;
	}

	bool CAbstractAlarmStatistics::_UpdateStatisticsResult(const std::string& strResult, const std::string& strDate)
	{
		std::string strTime = m_pDateTime->CurrentDateTimeStr();

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strUpdateSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_RESULT] = strResult;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = m_strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TIME] = strTime;

		std::string strTmpLog;
		bool bExecResult = _ExecSql(l_SqlRequest);
		if (bExecResult)
		{
			strTmpLog = m_pStringUtil->Format("update_statistics_result success! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
			return true;
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("update_statistics_result failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
			return false;
		}
	}

	bool CAbstractAlarmStatistics::_ExecSql(DataBase::SQLRequest sqlRequest)
	{
		return m_pStatisticsDb->ExecSql(sqlRequest);

		/*std::string strTmpLog;
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sqlRequest);
		if (l_pResult == nullptr)
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return false;
		}


		if (!l_pResult->IsValid())
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return false;
		}

		strTmpLog = m_pStringUtil->Format("exec sql success!, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		return true;*/		
	}

	std::string CAbstractAlarmStatistics::_QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName)
	{
		return m_pStatisticsDb->QueryData(sqlRequest, strResultFiledName);

		/*std::string strTmpLog;
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sqlRequest);
		if (l_pResult == nullptr)
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return "";
		}


		if (!l_pResult->IsValid())
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return "";
		}

		strTmpLog = m_pStringUtil->Format("exec sql success!, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		if (l_pResult->Next())
		{
			std::string strCount = l_pResult->GetValue(strResultFiledName);
			strTmpLog = m_pStringUtil->Format("statistics_receive_valid_alarm_call_count success! count = %s", strCount.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
			return strCount;
		}

		return "";*/
	}

	int CAbstractAlarmStatistics::_ProcessStatusUpdateStatistics()
	{
		std::string strCurrDate = CStatisticsCommonTime::CurrentDateString(m_pDateTime);		

		std::vector<std::string> vecUpdateDates;
		m_pStatisticsDb->QueryDateInfo(m_strQueryUpdateDatesSqlId, strCurrDate, vecUpdateDates);

		for (int i = 0; i < vecUpdateDates.size(); ++i)
		{
			std::string strPreResult = _QueryStatisticsData(vecUpdateDates[i]);
			std::string strNewResult = _StatisticsData(vecUpdateDates[i]);
			if (!strPreResult.empty())
			{
				if (strNewResult != strPreResult)
				{
					_UpdateStatisticsResult(strNewResult, vecUpdateDates[i]);
				}
			}
		}

		return 0;
	}
}