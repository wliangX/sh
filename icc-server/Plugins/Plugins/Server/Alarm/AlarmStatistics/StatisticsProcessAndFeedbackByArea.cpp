#include "Boost.h"
#include "StatisticsProcessAndFeedbackByArea.h"
#include "StatisticsCommonTime.h"
#include "StatisticsCommonDef.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int CStatisticsProcessAndFeedbackByArea::Statistics()
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
	CStatisticsProcessAndFeedbackByArea::CStatisticsProcessAndFeedbackByArea(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: m_pLog(pLog), m_pDBConn(pDBConn), m_pStringUtil(pStringUtil), m_pDateTime(pDateTime)
	{
		m_pStatisticsDb = boost::make_shared<CDbBase>(pLog, pDBConn, pStringUtil);

		m_strInsertSqlName = "insert_statistics_result_by_area";
		m_strQueryResultSqlName = "select_statistics_result_by_area";
		m_strUpdateSqlName = "update_statistics_result_by_area";
		
		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA);
		m_vecStatisticsTypes.push_back(STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA);
		
		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA, "statistics_receive_alarm_count_by_area"));
		m_mapTypeAndSqlIds.insert(std::make_pair(STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA, "statistics_feedback_alarm_count_by_area"));

		m_strQueryDeptInfoSqlId = "statistics_process_deptinfo";
		m_strDeptCodeFiled = "process_dept_code";
		m_strDeptNameFiled = "name";

		m_strQueryUpdateDatesSqlId = "query_dateinfo_when_updatetime_is_update_for_processtable";
	}

	CStatisticsProcessAndFeedbackByArea::~CStatisticsProcessAndFeedbackByArea()
	{

	}	

	std::string CStatisticsProcessAndFeedbackByArea::_StatisticsDataByDeptCode(const std::string& strType, const std::string& strDate, const std::string& strDeptCode)
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
		std::string strResult = m_pStatisticsDb->QueryData(l_SqlRequest, strResultFiledName);
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

	void CStatisticsProcessAndFeedbackByArea::_StatisticsData(const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName)
	{
		for (int j = 0; j < m_vecStatisticsTypes.size(); ++j)
		{
			std::string strPreResult = _QueryStatisticsData(m_vecStatisticsTypes[j], strDate, strDeptCode);
			std::string strNewResult = _StatisticsDataByDeptCode(m_vecStatisticsTypes[j], strDate, strDeptCode);
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

			std::vector<std::string> vecUpdateDates;
			m_pStatisticsDb->QueryDateInfo(m_strQueryUpdateDatesSqlId, strDate, vecUpdateDates);
			for (int i = 0; i < vecUpdateDates.size(); ++i)
			{
				std::string strPreResult = _QueryStatisticsData(m_vecStatisticsTypes[j], strDate, strDeptCode);
				std::string strNewResult = _StatisticsDataByDeptCode(m_vecStatisticsTypes[j], strDate, strDeptCode);
				if (!strPreResult.empty())
				{
					if (strNewResult != strPreResult)
					{
						_UpdateStatisticsResult(m_vecStatisticsTypes[j], strNewResult, strDate, strDeptCode);
					}
				}
			}

		}
	}

	int CStatisticsProcessAndFeedbackByArea::_PrcessPreDateStatistics()
	{
		std::string strPreDate = CStatisticsCommonTime::PreDateString(m_pDateTime);

		//strPreDate = "2019-11-05";

		std::vector<StatisticsDeptInfo> vecDeptInfos;
		m_pStatisticsDb->QueryDeptInfo(m_strQueryDeptInfoSqlId, m_strDeptCodeFiled, m_strDeptNameFiled, strPreDate, vecDeptInfos);

		for (int i = 0; i < vecDeptInfos.size(); ++i)
		{
			_StatisticsData(strPreDate, vecDeptInfos[i].strDeptCode, vecDeptInfos[i].strDeptName);
		}

		return 0;
	}

	int CStatisticsProcessAndFeedbackByArea::_ProcessCurrDateStatistics()
	{
		std::string strCurrDate = CStatisticsCommonTime::CurrentDateString(m_pDateTime);

		std::vector<StatisticsDeptInfo> vecDeptInfos;
		m_pStatisticsDb->QueryDeptInfo(m_strQueryDeptInfoSqlId, m_strDeptCodeFiled, m_strDeptNameFiled, strCurrDate, vecDeptInfos);

		for (int i = 0; i < vecDeptInfos.size(); ++i)
		{
			_StatisticsData(strCurrDate, vecDeptInfos[i].strDeptCode, vecDeptInfos[i].strDeptName);
		}

		return 0;
	}

	bool CStatisticsProcessAndFeedbackByArea::_InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName)
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
		bool bExecResult = m_pStatisticsDb->ExecSql(l_SqlRequest);
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

	std::string CStatisticsProcessAndFeedbackByArea::_QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strDeptCode)
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = m_strQueryResultSqlName;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_TYPE] = strStatisticsType;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DATE] = strDate;
		l_SqlRequest.param[STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE] = strDeptCode;

		std::string strTmpLog;
		std::string strResultFiledName = STATISTICS_FIELD_NAME_STATISTICS_RESULT;
		std::string strResult = m_pStatisticsDb->QueryData(l_SqlRequest, strResultFiledName);
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

	bool CStatisticsProcessAndFeedbackByArea::_UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode)
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
		bool bExecResult = m_pStatisticsDb->ExecSql(l_SqlRequest);
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
	
}