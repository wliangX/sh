#include "Boost.h"
#include "StatisticsAlarmByArea.h"
#include "StatisticsCommonTime.h"
#include "StatisticsCommonDef.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int CStatisticsAlarmCountByArea::Statistics()
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
	CStatisticsAlarmCountByArea::CStatisticsAlarmCountByArea(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: m_pLog(pLog), m_pDBConn(pDBConn), m_pStringUtil(pStringUtil), m_pDateTime(pDateTime)
	{
		m_strInsertSqlName = "insert_statistics_result_by_area";
		m_strQueryResultSqlName = "select_statistics_result_by_area";
		m_strUpdateSqlName = "update_statistics_result_by_area";

		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_ALARM_AREA_AND_FIRSTTYPE);
		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_ALARM_AREA_AND_SECONDTYPE);
		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_ALARM_AREA_AND_THIRDTYPE);
		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_ALARM_AREA_AND_FOOURTHTYPE);
		//m_vecStatisticsTypes.push_back(STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA);
		//m_vecStatisticsTypes.push_back(STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA);

		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_ALARM_AREA_AND_FIRSTTYPE, "statistics_alarm_count_by_Area_and_first_type"));
		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_ALARM_AREA_AND_SECONDTYPE, "statistics_alarm_count_by_Area_and_second_type"));
		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_ALARM_AREA_AND_THIRDTYPE, "statistics_alarm_count_by_Area_and_third_type"));
		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_ALARM_AREA_AND_FOOURTHTYPE, "statistics_alarm_count_by_Area_and_fourth_type"));
		//m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA, "statistics_receive_alarm_count_by_area"));
		//m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA, "statistics_feedback_alarm_count_by_area"));
	}

	CStatisticsAlarmCountByArea::~CStatisticsAlarmCountByArea()
	{

	}

	void CStatisticsAlarmCountByArea::_StatisticsDeptInfo(const std::string& strDate, std::vector<StatisticsDeptInfo>& vecDeptInfos)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "statistics_alarm_deptinfo";
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
			return ;
		}

		strTmpLog = m_pStringUtil->Format("exec sql success!, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		while (l_pResult->Next())
		{
			StatisticsDeptInfo deptInfo;
			deptInfo.strDeptCode = l_pResult->GetValue("receipt_dept_code");
			deptInfo.strDeptName = l_pResult->GetValue("name");

			if (!deptInfo.strDeptCode.empty())
			{
				vecDeptInfos.push_back(deptInfo);
			}

			strTmpLog = m_pStringUtil->Format("statistics_alarm_deptinfo find deptcode = %s", deptInfo.strDeptCode.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());			
		}
	}

	std::string CStatisticsAlarmCountByArea::_StatisticsData(const std::string& strType, const std::string& strDate, const std::string& strDeptCode)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_mapTypeAndSqlIds[strType];
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE] = strDeptCode;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		std::string strResultFiledName = "count";
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("statistics count failed! id = %s", l_SqlRequest.sql_id.c_str());
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

	void CStatisticsAlarmCountByArea::_StatisticsDeptData(const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName)
	{
		for (int j = 0; j < m_vecStatisticsTypes.size(); ++j)
		{
			std::string strPreResult = _QueryStatisticsData(m_vecStatisticsTypes[j], strDate, strDeptCode);
			std::string strNewResult = _StatisticsData(m_vecStatisticsTypes[j], strDate, strDeptCode);
			if (!strPreResult.empty())
			{
				if (strNewResult != strPreResult)
				{
					_UpdateStatisticsResult(m_vecStatisticsTypes[j], strNewResult, strDate, strDeptCode);
				}
			}
			else
			{
				_InsertStatisticsResult(m_vecStatisticsTypes[j], strNewResult, strDate, strDeptCode, strDeptName);
			}
		}
	}

	int CStatisticsAlarmCountByArea::_PrcessPreDateStatistics()
	{
		std::string strPreDate = CStatisticsCommonTime::PreDateString(m_pDateTime);

		//strPreDate = "2019-11-05";

		std::vector<StatisticsDeptInfo> vecDeptInfos;
		_StatisticsDeptInfo(strPreDate, vecDeptInfos);

		for (int i = 0; i < vecDeptInfos.size(); ++i)
		{
			_StatisticsDeptData(strPreDate, vecDeptInfos[i].strDeptCode, vecDeptInfos[i].strDeptName);
		}
		
		return 0;
	}

	int CStatisticsAlarmCountByArea::_ProcessCurrDateStatistics()
	{
		std::string strCurrDate = CStatisticsCommonTime::CurrentDateString(m_pDateTime);

		std::vector<StatisticsDeptInfo> vecDeptInfos;
		_StatisticsDeptInfo(strCurrDate, vecDeptInfos);

		for (int i = 0; i < vecDeptInfos.size(); ++i)
		{
			_StatisticsDeptData(strCurrDate, vecDeptInfos[i].strDeptCode, vecDeptInfos[i].strDeptName);
		}

		return 0;
	}

	bool CStatisticsAlarmCountByArea::_InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName)
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

	std::string CStatisticsAlarmCountByArea::_QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strDeptCode)
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strQueryResultSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE] = strDeptCode;

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

	bool CStatisticsAlarmCountByArea::_UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode)
	{
		std::string strTime = m_pDateTime->CurrentDateTimeStr();

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strUpdateSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_RESULT] = strResult;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE] = strDeptCode;
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

	bool CStatisticsAlarmCountByArea::_ExecSql(DataBase::SQLRequest sqlRequest)
	{
		std::string strTmpLog;
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

		return true;
	}

	std::string CStatisticsAlarmCountByArea::_QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName)
	{
		std::string strTmpLog;
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

		return "";
	}
}