#include "AlarmProcesser.h"
#include "Protocol/CAlarmSync.h"
//#include "CommonLogger.h"
#include "Format.h"
//#include "StringConverter.h"
#include "CommonDef.h"

#define LIMIT_COUNT "5"

const unsigned long TIMERID_INIT = 30000;
const unsigned long TIMERINTERVAL_INIT = 2000;

const unsigned long TIMERID_QUERYACTUALALARM = 30001;
const unsigned long TIMERINTERVAL_QUERYACTUALALARM = 1000;

const unsigned long QUERY_INTERVAL = 30;

using namespace ICC;

/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string CAlarmProcesser::ProcNotify(const std::string& strMsg)
{
	std::string strActualAlarm = _QueryActualAlram();
	/*if (!strActualAlarm.empty())
	{
		std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
		m_ullPreUpdateTime = std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
	}*/	

	return strActualAlarm;
}

bool CAlarmProcesser::InitProcesser()
{	
	m_bInitCompleteFlag = _InitProcesser();	
	
	return true;
}

void CAlarmProcesser::UninitProcesser()
{

}


/************************************************************************/
/*                                                                      */
/************************************************************************/
CAlarmProcesser::CAlarmProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog,
	DataBase::IDBConnPtr pDbCon, Config::IConfigPtr pConfig)
	: m_pJsonFty(pJsonFty), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime), m_pLog(pLog), m_pDbCon(pDbCon), m_pConfig(pConfig)
{
	m_bInitCompleteFlag = false;
	m_ullPreUpdateTime = 0;
}

CAlarmProcesser::~CAlarmProcesser()
{

}

//void CAlarmProcesser::OnTimer(unsigned long uMsgId)
//{
//	if (uMsgId == TIMERID_INIT)
//	{
//		if (!m_bInitCompleteFlag)
//		{
//			m_bInitCompleteFlag = _InitProcesser();
//		}
//		
//		if (m_bInitCompleteFlag)
//		{
//			m_initTimer.RemoveTimer(TIMERID_INIT);
//			std::string strTmpLog = "alarm processer init complete";
//			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
//		}		
//	}
//	else if (uMsgId == TIMERID_QUERYACTUALALARM)
//	{
//		std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
//		unsigned long long ullCurCount = std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
//
//		if ((ullCurCount - m_ullPreUpdateTime) >= QUERY_INTERVAL)
//		{
//			_ProcActualAlarm();
//			m_ullPreUpdateTime = ullCurCount;
//		}
//	}
//}

bool CAlarmProcesser::_InitProcesser()
{
	std::string strConfigLanguage = m_pConfig->GetValue("ICC/Plugin/IFDS/TranslateLanguage", "");

	std::string strTmpLog = StringCommon::format("read config ICC/Plugin/IFDS/TranslateLanguage, value = %s", strConfigLanguage);
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	if (strConfigLanguage.empty())
	{
		strConfigLanguage = "ZH_CN";
	}

	std::string strLanguageGuid = _QuseryLangGuidByLangCode(strConfigLanguage);
	if (!strLanguageGuid.empty())
	{
		m_strLangGuid = strLanguageGuid;
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "%s", "query guid by code failed!");
		return false;
	}

	_ProcActualAlarm();

	return true;
}

std::string CAlarmProcesser::_QueryActualAlram()
{
	std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo> vecAlarms;
	bool bQuery = _InitActualAlarmBuffer(vecAlarms);
	if (!bQuery)
	{
		std::string strTmpLog = "Query alarm info failed";
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}
	_TransAlarmCode(vecAlarms);


	PROTOCOL::CIfdsAlarmInfo actualTimeAlarm;
	_PacketHeader(actualTimeAlarm.m_oHeader);
	_PacketBody(actualTimeAlarm.m_oBody, vecAlarms);

	std::string strMsg = actualTimeAlarm.ToString(m_pJsonFty->CreateJson());	

	return strMsg;
}

void CAlarmProcesser::_ProcActualAlarm()
{
	std::string strMsg = _QueryActualAlram();
	if (strMsg.empty())
	{
		return;
	}
	gfunc_SendMessage(strMsg);	
}

bool CAlarmProcesser::_InitActualAlarmBuffer(std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms)
{
	vecAlarms.clear();
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_limit_by_time";
	l_SqlRequest.param["limit_count"] = LIMIT_COUNT;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDbCon->Exec(l_SqlRequest, true);
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
		PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo actualAlarmInfo;
		actualAlarmInfo.m_strAlarmId = l_pResult->GetValue("id");
		actualAlarmInfo.m_strContent = l_pResult->GetValue("content");
		actualAlarmInfo.m_strAddr = l_pResult->GetValue("addr");
		actualAlarmInfo.m_strCallerNo = l_pResult->GetValue("caller_no");
		actualAlarmInfo.m_strCallerName = l_pResult->GetValue("caller_name");
		actualAlarmInfo.m_strTime = l_pResult->GetValue("receiving_time");
		actualAlarmInfo.m_strActualOccurTime = l_pResult->GetValue("actual_occur_time"); //实际发生时间呢
		actualAlarmInfo.m_strState = l_pResult->GetValue("state");
		actualAlarmInfo.m_strStateCode = actualAlarmInfo.m_strState;
		actualAlarmInfo.m_strFirstTypeCode = l_pResult->GetValue("first_type");
		actualAlarmInfo.m_strFirstType = l_pResult->GetValue("first_type");
		actualAlarmInfo.m_strSecondType = l_pResult->GetValue("second_type");
		actualAlarmInfo.m_strSecondTypeCode = actualAlarmInfo.m_strSecondType;
		vecAlarms.push_back(actualAlarmInfo);
	}

	strTmpLog = StringCommon::format("init actual buffer complete, size = %d", (int)vecAlarms.size());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	return true;
}

void CAlarmProcesser::_TransAlarmCode(std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms)
{	
	for (unsigned int i = 0; i < vecAlarms.size(); ++i)
	{
		std::string strTmp;
		strTmp = _TranslateCode(vecAlarms[i].m_strState);
		if (!strTmp.empty())
		{
			vecAlarms[i].m_strState = strTmp;
		}

		strTmp = _TranslateCode(vecAlarms[i].m_strFirstType);
		if (!strTmp.empty())
		{
			vecAlarms[i].m_strFirstType = strTmp;
		}

		strTmp = _TranslateCode(vecAlarms[i].m_strSecondType);
		if (!strTmp.empty())
		{
			vecAlarms[i].m_strSecondType = strTmp;
		}
	}
}

std::string CAlarmProcesser::_QuseryLangGuidByLangCode(const std::string& strLangCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_language_by_langcode";
	l_SqlRequest.param["code"] = strLangCode;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDbCon->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());


	if (l_pResult->Next())
	{
		return l_pResult->GetValue("guid");
	}

	return "";
}

std::string CAlarmProcesser::_QueryDictGuidByCode(const std::string& strCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dict_guid_by_code";
	l_SqlRequest.param["code"] = strCode;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDbCon->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	
	if (l_pResult->Next())
	{
		return l_pResult->GetValue("guid");
	}

	return "";
}

std::string CAlarmProcesser::_QueryDictValue(const std::string& strDictGuid, const std::string& strLangGuid)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dict_value_by_dictguid_and_langguid";
	l_SqlRequest.param["dict_guid"] = strDictGuid;
	l_SqlRequest.param["lang_guid"] = strLangGuid;

	std::string strTmpLog; 
	DataBase::IResultSetPtr l_pResult = m_pDbCon->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return "";
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());


	if (l_pResult->Next())
	{
		return l_pResult->GetValue("value");
	}

	return "";
}

std::string CAlarmProcesser::_TranslateCode(const std::string& strCode)
{
	if (strCode.empty())
	{
		return "";
	}

	if (m_strLangGuid.empty())
	{
		InitProcesser();
	}

	if (m_strLangGuid.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "%s", "lang guid is empty!");
		return "";
	}

	std::string strDictGuid = _QueryDictGuidByCode(strCode);
	if (strDictGuid.empty())
	{
		return "";
	}
	return _QueryDictValue(strDictGuid, m_strLangGuid);
}

bool CAlarmProcesser::_ParseMsg(const std::string& strMsg, PROTOCOL::CWebAlarmSync& alarmInfo)
{
	std::string strTmpLog;

	if (!alarmInfo.ParseString(strMsg, m_pJsonFty->CreateJson()))
	{
		strTmpLog = StringCommon::format("ParseString Error:[%s]", strMsg);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	return true;
}

void CAlarmProcesser::_TranslateMsg(PROTOCOL::CWebAlarmSync& alarmInfo)
{
	std::string strTmp;
	strTmp = _TranslateCode(alarmInfo.m_oBody.m_oAlarm.m_strState);
	if (!strTmp.empty())
	{
		alarmInfo.m_oBody.m_oAlarm.m_strState = strTmp;
	}

	strTmp = _TranslateCode(alarmInfo.m_oBody.m_oAlarm.m_strFirstType);
	if (!strTmp.empty())
	{
		alarmInfo.m_oBody.m_oAlarm.m_strFirstType = strTmp;
	}

	strTmp = _TranslateCode(alarmInfo.m_oBody.m_oAlarm.m_strSecondType);
	if (!strTmp.empty())
	{
		alarmInfo.m_oBody.m_oAlarm.m_strSecondType = strTmp;
	}
}

//void CAlarmProcesser::_InsertBuffer(PROTOCOL::CWebAlarmSync& alarmInfo)
//{
//	PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo info;
//	info.m_strAlarmId = alarmInfo.m_oBody.m_oAlarm.m_strID;
//	info.m_strContent = alarmInfo.m_oBody.m_oAlarm.m_strContent;
//	info.m_strCallerNo = alarmInfo.m_oBody.m_oAlarm.m_strCallerNo;
//	info.m_strCallerName = alarmInfo.m_oBody.m_oAlarm.m_strCallerName;
//	info.m_strTime = alarmInfo.m_oBody.m_oAlarm.m_strTime;
//	info.m_strAddr = alarmInfo.m_oBody.m_oAlarm.m_strAddr;
//	info.m_strActualOccurTime = alarmInfo.m_oBody.m_oAlarm.m_strActualOccurTime;
//	info.m_strState = alarmInfo.m_oBody.m_oAlarm.m_strState;
//	info.m_strFirstType = alarmInfo.m_oBody.m_oAlarm.m_strFirstType;
//	info.m_strSecondType = alarmInfo.m_oBody.m_oAlarm.m_strSecondType;	
//
//	if (m_vecActualAlarmBuffer.size() >= std::stoi(LIMIT_COUNT))
//	{
//		m_vecActualAlarmBuffer.push_front(info);
//		m_vecActualAlarmBuffer.pop_back();
//	}
//	else
//	{
//		m_vecActualAlarmBuffer.push_back(info);
//	}	
//}

void CAlarmProcesser::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "icc_actualtimealarm";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "topic_actualTimeAlarmReport";
	header.m_strRequestType = REQESTMODE_TOPIC;
}

void CAlarmProcesser::_PacketBody(PROTOCOL::CIfdsAlarmInfo::CBody& body, const std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms)
{
	body.m_strTotalCount = std::to_string(vecAlarms.size());

	body.m_vecInfos.assign(vecAlarms.begin(), vecAlarms.end());	
}