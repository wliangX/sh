#include "Boost.h"
#include "DbBase.h"
#include "StatisticsCommonTime.h"
#include "StatisticsCommonDef.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	bool CDbBase::ExecSql(DataBase::SQLRequest sqlRequest)
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

	std::string CDbBase::QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName)
	{
		std::string strTmpLog;
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sqlRequest, true);
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

	void CDbBase::QueryDeptInfo(const std::string& strQuerySqlId, const std::string& strDeptCodeFiled, const std::string& strDeptNameFiled, const std::string& strDate, std::vector<StatisticsDeptInfo>& vecDeptInfos)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = strQuerySqlId;
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
			StatisticsDeptInfo deptInfo;
			deptInfo.strDeptCode = l_pResult->GetValue(strDeptCodeFiled);
			deptInfo.strDeptName = l_pResult->GetValue(strDeptNameFiled);

			if (!deptInfo.strDeptCode.empty())
			{
				vecDeptInfos.push_back(deptInfo);
			}

			strTmpLog = m_pStringUtil->Format("find deptcode = %s", deptInfo.strDeptCode.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}
	}

	void CDbBase::QueryDateInfo(const std::string& strQuerySqlId, const std::string& strDate, std::vector<std::string>& vecDateInfos)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = strQuerySqlId;
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
		    std::string strTmpDate = l_pResult->GetValue("substring");			
			if (!strTmpDate.empty())
			{
				vecDateInfos.push_back(strTmpDate);
			}			
		}
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CDbBase::CDbBase(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil)
		: m_pLog(pLog), m_pDBConn(pDBConn), m_pStringUtil(pStringUtil)
	{
		
	}

	CDbBase::~CDbBase()
	{

	}
}