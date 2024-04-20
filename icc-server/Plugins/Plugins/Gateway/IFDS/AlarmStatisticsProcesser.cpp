#include "AlarmStatisticsProcesser.h"
//#include "CommonLogger.h"
#include "Format.h"
#include "CommonTime.h"
#include "Boost.h"
#include "CommonDef.h"
#include "Protocol/StatisticsAlarm.h"





/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string CAlarmStatisticsProcesser::ProcNotify(const std::string& strMsg)
{
	_StatisticsAlarmCountInfo();
	return "";
}

bool CAlarmStatisticsProcesser::InitProcesser()
{
	return true;
}

void CAlarmStatisticsProcesser::UninitProcesser()
{
	
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CAlarmStatisticsProcesser::CAlarmStatisticsProcesser(JsonParser::IJsonFactoryPtr pJsonFty, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStrUtil, 
	DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog, Config::IConfigPtr pConfig)
	: m_pJsonFty(pJsonFty), m_pDBConn(pDBConn), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime), m_pLog(pLog), m_pConfig(pConfig)
{

}

CAlarmStatisticsProcesser::~CAlarmStatisticsProcesser()
{

}

int CAlarmStatisticsProcesser::_QueryCount(DataBase::SQLRequest& sqlRequest)
{
	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	if (l_pResult->Next())
	{
		std::string strCount = l_pResult->GetValue("count");
		if (!strCount.empty())
		{
			return std::stoi(strCount);
		}
	}
	return 0;
}

bool CAlarmStatisticsProcesser::_QueryAlarmDeptsByTime(const std::string& strBeginTime, const std::string& strEndTime, DEPTINFOMAP& depts)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_depts_by_time";
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;
	
	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}
	
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		DeptInfo info;
		info.strDeptCode = l_pResult->GetValue("dept_code");
		info.strDeptName = l_pResult->GetValue("dept_name");
		if (!info.strDeptCode.empty())
		{
			depts.insert(std::make_pair(info.strDeptCode, info));
		}		
	}

	return true;
}

int CAlarmStatisticsProcesser::_QueryAlarmCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_count_by_deptcode_and_time";
	l_SqlRequest.param["dept_code"] = strDeptCode;
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;
	
	return _QueryCount(l_SqlRequest);
}

void CAlarmStatisticsProcesser::_QueryAlarmCountInfo(DEPTINFOMAP& depts, const std::string& strBeginTime, const std::string& strEndTime)
{	
	if (!_QueryAlarmDeptsByTime(strBeginTime, strEndTime, depts))
	{
		return;
	}
	
	std::string strTmpLog;
	if (depts.empty())
	{
		strTmpLog = StringCommon::format("not find new alarm, begin = %s, end = %s", strBeginTime, strEndTime);
		ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = StringCommon::format("find new alarm, count = %d", (int)depts.size());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	for (DEPTINFOMAP::iterator itr = depts.begin(); itr != depts.end(); ++itr)
	{
		int iCount = _QueryAlarmCountByDeptCodeAndTime(itr->first, strBeginTime, strEndTime);
		itr->second.iAlarmCount = iCount;

		strTmpLog = StringCommon::format("alarm statistics, dept = %s, count = %d", itr->first, iCount);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
	}
}

bool CAlarmStatisticsProcesser::_QueryCallRefDeptsByTime(const std::string& strBeginTime, const std::string& strEndTime, DEPTINFOMAP& depts)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callevent_depts_by_time";
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;
	
	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		DeptInfo info;
		info.strDeptCode = l_pResult->GetValue("dept_code");
		info.strDeptName = l_pResult->GetValue("dept_name");
		if (!info.strDeptCode.empty())
		{
			depts.insert(std::make_pair(info.strDeptCode, info));
		}		
	}

	return true;
}

int CAlarmStatisticsProcesser::_QueryCallRefReleaseCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callevent_count_by_hangup_release";
	l_SqlRequest.param["dept_code"] = strDeptCode;
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;

	return _QueryCount(l_SqlRequest);
}

int CAlarmStatisticsProcesser::_QueryCallRefCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callevent_count_by_deptcode_and_time";
	l_SqlRequest.param["dept_code"] = strDeptCode;
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;

	return _QueryCount(l_SqlRequest);
}

int CAlarmStatisticsProcesser::_QueryCallBackRefCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callback_relation_count_by_deptcode_and_time";
	l_SqlRequest.param["dept_code"] = strDeptCode;
	l_SqlRequest.param["begin_time"] = strBeginTime;
	l_SqlRequest.param["end_time"] = strEndTime;

	return _QueryCount(l_SqlRequest);
}

void CAlarmStatisticsProcesser::_QueryCallRefInfo(DEPTINFOMAP& depts, const std::string& strBeginTime, const std::string& strEndTime)
{
	if (!_QueryCallRefDeptsByTime(strBeginTime, strEndTime, depts))
	{
		return;
	}

	std::string strTmpLog;
	if (depts.empty())
	{
		strTmpLog = StringCommon::format("not find callref, begin = %s, end = %s", strBeginTime, strEndTime);
		ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	for (DEPTINFOMAP::iterator itr = depts.begin(); itr != depts.end(); ++itr)
	{
		std::string strDeptCode = itr->first;
		strTmpLog = StringCommon::format("begin find call info, dept = %s", strDeptCode);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		int iReleaseCount = _QueryCallRefReleaseCountByDeptCodeAndTime(strDeptCode, strBeginTime, strEndTime);
		itr->second.iCallReleaseCount = iReleaseCount;

		int iCallCount = _QueryCallRefCountByDeptCodeAndTime(strDeptCode, strBeginTime, strEndTime);
		int iCallbackCallCount = _QueryCallBackRefCountByDeptCodeAndTime(strDeptCode, strBeginTime, strEndTime);
		itr->second.iCallRefCount = iCallCount + iCallbackCallCount;
	}
}

void CAlarmStatisticsProcesser::_MergeResult(DEPTINFOMAP& alarmDepts, DEPTINFOMAP& callRefDepts)
{
	std::string strTmpLog = StringCommon::format("alarm count = %d, callref = %d", (int)alarmDepts.size(), (int)callRefDepts.size());

	for (DEPTINFOMAP::iterator itr = alarmDepts.begin(); itr != alarmDepts.end(); ++itr)
	{
		DEPTINFOMAP::iterator itrCall = callRefDepts.find(itr->first);
		if (itrCall != callRefDepts.end())
		{
			itr->second.iCallRefCount = itrCall->second.iCallRefCount;
			itr->second.iCallReleaseCount = itrCall->second.iCallReleaseCount;
			callRefDepts.erase(itrCall);
		}
	}

	if (!callRefDepts.empty())
	{
		for (DEPTINFOMAP::iterator itrCall = alarmDepts.begin(); itrCall != alarmDepts.end(); ++itrCall)
		{
			alarmDepts.insert(std::make_pair(itrCall->first, itrCall->second));
		}
	}
}

void  CAlarmStatisticsProcesser::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "icc_statisticsinfo";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "topic_alarmInfoStatistics";
	header.m_strRequestType = REQESTMODE_TOPIC;
}

void CAlarmStatisticsProcesser::_PacketBody(const DEPTINFOMAP& alarmDepts, PROTOCOL::CStatisticsAlarm::CBody& body)
{
	body.m_strResult = "result";
	body.m_strTime = "";
	body.m_strTotalCount = StringCommon::format("%d", (int)alarmDepts.size());

	for (DEPTINFOMAP::const_iterator itr = alarmDepts.begin(); itr != alarmDepts.end(); ++itr)
	{
		PROTOCOL::CStatisticsAlarm::CInfo l_oAlarm;				
		l_oAlarm.strDeptCode = itr->second.strDeptCode;
		l_oAlarm.strDeptName = itr->second.strDeptName;
		l_oAlarm.strAlarmCount = StringCommon::format("%d", itr->second.iAlarmCount);
		l_oAlarm.strCallReleaseCount = StringCommon::format("%d", itr->second.iCallReleaseCount);
		l_oAlarm.strCallRefCount = StringCommon::format("%d", itr->second.iCallRefCount);

		body.m_vecInfos.push_back(l_oAlarm);
	}
}

std::string CAlarmStatisticsProcesser::_PacketResult(const DEPTINFOMAP& alarmDepts)
{
	PROTOCOL::CStatisticsAlarm alarmInfo;
	_PacketHeader(alarmInfo.m_oHeader);
	_PacketBody(alarmDepts, alarmInfo.m_oBody);
	return alarmInfo.ToString(m_pJsonFty->CreateJson());
}

void CAlarmStatisticsProcesser::_StatisticsAlarmCountInfo()
{
	CCommonTime curTime;
	std::string strBeginTime = StringCommon::format("%04d-%02d-%02d 00:00:00", curTime.Year(), curTime.Month(), curTime.Day());
	std::string strEndTime = StringCommon::format("%04d-%02d-%02d 23:59:59", curTime.Year(), curTime.Month(), curTime.Day());

	DEPTINFOMAP alarmDepts;
	_QueryAlarmCountInfo(alarmDepts, strBeginTime, strEndTime);	

	DEPTINFOMAP callRefDepts;
	_QueryCallRefInfo(callRefDepts, strBeginTime, strEndTime);

	if (alarmDepts.empty() && callRefDepts.empty())
	{
		ICC_LOG_WARNING(m_pLog, "%s", "not new record!");
		return;
	}

	_MergeResult(alarmDepts, callRefDepts);

	std::string strAlarmCountInfo = _PacketResult(alarmDepts);
	gfunc_SendMessage(strAlarmCountInfo);	
}


