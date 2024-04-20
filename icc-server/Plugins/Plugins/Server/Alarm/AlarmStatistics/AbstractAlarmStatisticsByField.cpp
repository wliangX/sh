#include "Boost.h"
#include "AbstractAlarmStatisticsByField.h"
#include "StatisticsCommonTime.h"
#include "StatisticsCommonDef.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int CAbstractStatisticsAlarmByField::Statistics()
	{
		_PrcessPreDateStatistics();

		if (m_strNeedStatisticsCurrentDayFlag == FLAG_TRUE)
		{
			_ProcessCurrDateStatistics();
		}

		return 0;
	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CAbstractStatisticsAlarmByField::CAbstractStatisticsAlarmByField(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: m_pLog(pLog), m_pDBConn(pDBConn), m_pStringUtil(pStringUtil), m_pDateTime(pDateTime)
	{
		m_strInsertSqlName = "insert_statistics_result_by_field";
		m_strQueryResultSqlName = "select_statistics_result_by_field";
		m_strUpdateSqlName = "update_statistics_result_by_field";	

		m_pStatisticsDb = boost::make_shared<CDbBase>(pLog, pDBConn, pStringUtil);
	}

	CAbstractStatisticsAlarmByField::~CAbstractStatisticsAlarmByField()
	{

	}

	void CAbstractStatisticsAlarmByField::_StatisticsFieldInfo(const std::string& strDate, std::vector<std::shared_ptr<BaseFieldInfo>>& vecFieldInfos)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strQueryFieldInfoSqlId;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
		if (l_pResult == nullptr)
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return;
		}

		if (!l_pResult->IsValid())
		{
			strTmpLog = m_pStringUtil->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
			ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			return;
		}

		strTmpLog = m_pStringUtil->Format("exec sql success!, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		while (l_pResult->Next())
		{
			std::shared_ptr<BaseFieldInfo> fieldInfo = std::make_shared<BaseFieldInfo>();
			fieldInfo->strField = l_pResult->GetValue(m_strFiledInfoName);

			if (!fieldInfo->strField.empty())
			{
				vecFieldInfos.push_back(fieldInfo);
			}

			strTmpLog = m_pStringUtil->Format("statistics_alarm_filedinfo find field = %s", fieldInfo->strField.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}
	}	

	void CAbstractStatisticsAlarmByField::_StatisticsFieldData(const std::string& strDate, std::shared_ptr<BaseFieldInfo> pFieldInfo)
	{
		std::string strPreResult = _QueryStatisticsData(m_strStatisticsType, strDate, pFieldInfo->strField);
		std::string strNewResult = _StatisticsDataByField(m_strStatisticsType, strDate, pFieldInfo->strField);
		if (!strPreResult.empty())
		{
			if (strNewResult != strPreResult)
			{
				_UpdateStatisticsResult(m_strStatisticsType, strNewResult, strDate, pFieldInfo->strField);
			}
		}
		else
		{
			_InsertStatisticsResult(m_strStatisticsType, strNewResult, strDate, "", "", pFieldInfo->strField);
		}
		
	}

	int CAbstractStatisticsAlarmByField::_PrcessPreDateStatistics()
	{
		std::string strPreDate = CStatisticsCommonTime::PreDateString(m_pDateTime);
		

		std::vector<std::shared_ptr<BaseFieldInfo>> vecFieldInfos;
		_StatisticsFieldInfo(strPreDate, vecFieldInfos);

		for (int i = 0; i < vecFieldInfos.size(); ++i)
		{
			_StatisticsFieldData(strPreDate, vecFieldInfos[i]);
		}
		
		return 0;
	}

	int CAbstractStatisticsAlarmByField::_ProcessCurrDateStatistics()
	{
		std::string strCurrDate = CStatisticsCommonTime::CurrentDateString(m_pDateTime);

		//strCurrDate = "2019-11-06";

		std::vector<std::shared_ptr<BaseFieldInfo>> vecFieldInfos;
		_StatisticsFieldInfo(strCurrDate, vecFieldInfos);

		for (int i = 0; i < vecFieldInfos.size(); ++i)
		{
			_StatisticsFieldData(strCurrDate, vecFieldInfos[i]);
		}

		return 0;
	}

	bool CAbstractStatisticsAlarmByField::_InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName, std::string& strField)
	{
		std::string strStatisticsId = m_pStringUtil->Format("%s-%s-%s", strDate.c_str(), strStatisticsType.c_str(), m_pStringUtil->CreateGuid().c_str());
		std::string strTime = m_pDateTime->CurrentDateTimeStr();

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strInsertSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_ID] = strStatisticsId;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_RESULT] = strResult;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE] = strDeptCode;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_NAME] = strDeptName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TIME] = strTime;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_REMARK] = strField;

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

	std::string CAbstractStatisticsAlarmByField::_QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strField)
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strQueryResultSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_REMARK] = strField;

		std::string strTmpLog;
		std::string strResultFiledName = STATISTICS_FIELD_NAME_STATISTICS_RESULT;
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("select_statistics_result failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("select_statistics_result success! count = %s, id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}

		return strResult;
	}

	bool CAbstractStatisticsAlarmByField::_UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strField)
	{
		std::string strTime = m_pDateTime->CurrentDateTimeStr();

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strUpdateSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_RESULT] = strResult;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_REMARK] = strField;
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

	bool CAbstractStatisticsAlarmByField::_ExecSql(DataBase::SQLRequest sqlRequest)
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

	std::string CAbstractStatisticsAlarmByField::_QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName)
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
}