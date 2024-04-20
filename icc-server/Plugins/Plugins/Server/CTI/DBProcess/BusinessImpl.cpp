#include "Boost.h"
#include "BusinessImpl.h"
#include "ViolationManager.h"

#define PARAM_INFO "ParamInfo"

const unsigned long ThreadId_DeviceStateChanged = 10000;
const unsigned long ThreadId_AlarmRelation = 10001;
const unsigned long ThreadId_QueryCallInfo = 10002;
const unsigned long ThreadId_Violation = 10003;

const unsigned long THREADID_RECEIVENOTIFY = 20000;

const std::string NumberType_Unknown = "99";
const std::string NumberType_OutLine = "1";
const std::string NumberType_ReceiveSeat = "2";
const std::string NumberType_ProcessSeat = "3";

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);

	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStringUtil = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pTimerManager = ICCGetITimerFactory()->CreateTimerManager();
	m_pInitDBConn = ICCGetIDBConnFactory()->CreateNewDBConn(DataBase::PostgreSQL);
	m_pRequestDBConn = ICCGetIDBConnFactory()->CreateNewDBConn(DataBase::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();

	m_uProcThreadCount = 0;
	m_uCurrentThreadIndex = 0;

	CViolationManager::Instance()->SetLogPtr(m_pLog);
	CViolationManager::Instance()->SetStringUtilPtr(m_pStringUtil);
	CViolationManager::Instance()->SetTimerManagerPtr(m_pTimerManager);
	CViolationManager::Instance()->SetDateTimePtr(m_pDateTime);

	CViolationManager::Instance()->SetRequestDBConnPtr(m_pRequestDBConn);
	CViolationManager::Instance()->SetJsonFactoryPtr(m_pJsonFty);
	CViolationManager::Instance()->SetObserverCenterPtr(m_pObserverCenter);
	CViolationManager::Instance()->SetRedisClientPtr(m_pRedisClient);

	CViolationManager::Instance()->SetHttpClientPtr(m_pHttpClient);
	CViolationManager::Instance()->SetConfigPtr(m_pConfig);
	CViolationManager::Instance()->SetStringPtr(m_pStringUtil);
	CViolationManager::Instance()->SetMsgCenterPtr(m_pMsgCenter);

	m_uIndex = 0;
}

void CBusinessImpl::_InitProcNotifys()
{
	m_mapFuncs.insert(std::make_pair(Cmd_ACDCallStateSync, &CBusinessImpl::OnNotifiCallStateSync));
	m_mapFuncs.insert(std::make_pair(Cmd_CallOverSync, &CBusinessImpl::OnNotifiCallOverSync));
	m_mapFuncs.insert(std::make_pair(Cmd_DetailCallOverSync, &CBusinessImpl::OnNotifiDetailCallOverSync));
	
	m_mapFuncs.insert(std::make_pair(Cmd_DeviceStateSync, &CBusinessImpl::OnNotifiDeviceStateSync));
	m_mapFuncs.insert(std::make_pair(Cmd_CallbackSync, &CBusinessImpl::OnNotifiCallBackSync));
	m_mapFuncs.insert(std::make_pair(Cmd_TakeOverCallSync, &CBusinessImpl::OnNotifiTakeOverCallSync));

	m_mapFuncs.insert(std::make_pair(Cmd_RemoveReleaseCallRequest, &CBusinessImpl::OnNotifiRemoveReleaseCallRequest));
	m_mapFuncs.insert(std::make_pair(Cmd_CallEventQueryRequest, &CBusinessImpl::OnNotifiQueryCallEventRequest));
	m_mapFuncs.insert(std::make_pair(Cmd_DetailCallQueryRequest, &CBusinessImpl::OnNotifiQueryDetailCallRequest));
	
	m_mapFuncs.insert(std::make_pair(Cmd_BlackCallQueryRequest, &CBusinessImpl::OnNotifiQueryBlackCallRequest));

	m_mapFuncs.insert(std::make_pair(Cmd_LoadAllBlackListRequest, &CBusinessImpl::OnNotifiLoadAllBlackListRequest));
	m_mapFuncs.insert(std::make_pair(Cmd_SetBlackListSync, &CBusinessImpl::OnNotifiSetBlackListSync));
	m_mapFuncs.insert(std::make_pair(Cmd_DeleteBlackListSync, &CBusinessImpl::OnNotifiDeleteBlackListSync));
	m_mapFuncs.insert(std::make_pair(Cmd_DeleteAllBlackListSync, &CBusinessImpl::OnNotifiDeleteAllBlackListSync));
	m_mapFuncs.insert(std::make_pair(Cmd_RedoSQLTimer, &CBusinessImpl::OnRedoSQLTimer));

	m_mapFuncs.insert(std::make_pair(Cmd_SetRedListRequest, &CBusinessImpl::OnNotifiSetRedListRequest));
	m_mapFuncs.insert(std::make_pair(Cmd_DeleteRedListRequest, &CBusinessImpl::OnNotifiDeleteRedListRequest));
	m_mapFuncs.insert(std::make_pair(Cmd_GetAllRedListRequest, &CBusinessImpl::OnNotifiGetRedListRequest));

	m_mapFuncs.insert(std::make_pair(Cmd_GetViolationRequest, &CBusinessImpl::OnNotifiGetViolationRequest));
	//m_mapFuncs.insert(std::make_pair("violation_check_timer", &CBusinessImpl::OnViolationCheckTimer));

	m_mapFuncs.insert(std::make_pair(Cmd_ACDAgentStateSync, &CBusinessImpl::OnRecvAcdAgentStateSync));

	m_mapFuncs.insert(std::make_pair("get_violation_timeout_request", &CBusinessImpl::OnGetViolationTimeoutCfgRequest));
	m_mapFuncs.insert(std::make_pair("syn_nacos_params", &CBusinessImpl::OnReceiveSynNacosParams));

	m_mapFuncs.insert(std::make_pair("set_process_call_info", &CBusinessImpl::OnReceiveAlarmRelationRequest));
	m_mapFuncs.insert(std::make_pair("get_process_call_info", &CBusinessImpl::OnReceiveQueryProcessCallRequest));

	//录音服务器录音成功,DBproces写库
	m_mapFuncs.insert(std::make_pair("record_file_up_sync", &CBusinessImpl::ReceiveRecordFileUpRequest));
	//录音服务请求历史话务后,CTI通知出来的历史话务数据
	m_mapFuncs.insert(std::make_pair("history_call_sync", &CBusinessImpl::ReceiveHistoryCallRequest));

}

void CBusinessImpl::OnStart()
{
	_CreateThreads();
	_InitProcNotifys();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ACDCallStateSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallOverSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DetailCallOverSync, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeviceStateSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallbackSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TakeOverCallSync, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RemoveReleaseCallRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallEventQueryRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DetailCallQueryRequest, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BlackCallQueryRequest, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_LoadAllBlackListRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetBlackListSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteBlackListSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteAllBlackListSync, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RedoSQLTimer, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetRedListRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteRedListRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAllRedListRequest, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetViolationRequest, _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "violation_check_timer", OnViolationCheckTimer);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ACDAgentStateSync, _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_violation_timeout_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_process_call_info", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_process_call_info", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "record_file_up_sync", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "history_call_sync", _OnReceiveNotify);

	m_pMsgCenter->Start();

	//Topic_ACDAgentStateSync

	this->LoadBlackList();

	this->LoadPhoneDivision();

	this->LoadRedList();
	m_pRedisClient->Del(CALL_INFO);
	std::string l_strTimeSpan = m_pConfig->GetValue("ICC/Plugin/CTI/RedoSQLTimeout", "120");
	std::string l_strTimer = m_pTimerManager->AddTimer(Cmd_RedoSQLTimer, atoi(l_strTimeSpan.c_str()), 0);
	ICC_LOG_DEBUG(m_pLog, "RedoSQL Timer %s Start", l_strTimer.c_str());

	m_strEnableProcessAlarm = m_pConfig->GetValue("ICC/Plugin/DBProcess/EnableProcessAlarm", "1");

	ICC_LOG_DEBUG(m_pLog, "m_strEnableProcessAlarm process alarm flag:[%s]", m_strEnableProcessAlarm.c_str());

	std::string l_strRingTimeOut = m_pConfig->GetValue("ICC/Plugin/CTI/ViolationCallRingTimeOut", "30");
	if (!l_strRingTimeOut.empty())
	{
		CViolationManager::Instance()->SetCallRingTimeOutValue(std::atoi(l_strRingTimeOut.c_str()));
	}
	std::string l_strTalkTimeOut = m_pConfig->GetValue("ICC/Plugin/CTI/ViolationCallTalkTimeOut", "60");
	if (!l_strTalkTimeOut.empty())
	{
		CViolationManager::Instance()->SetCallTalkTimeOutValue(std::atoi(l_strTalkTimeOut.c_str()));
	}

	std::string l_strBusyTimeOut = m_pConfig->GetValue("ICC/Plugin/CTI/ViolationBusyStateTimeOut", "60");
	if (!l_strBusyTimeOut.empty())
	{
		CViolationManager::Instance()->SetBusyTimeOutValue(std::atoi(l_strBusyTimeOut.c_str()));
	}

	std::string l_strAlarmViolationTimeOut = m_pConfig->GetValue("ICC/Plugin/CTI/ViolationAlarmTimeOut", "300");
	if (!l_strAlarmViolationTimeOut.empty())
	{
		m_pRedisClient->HSet("Violation_TimeoutValue", "AlarmTimeout", l_strAlarmViolationTimeOut);
	}
	
	//启动1s的违规检测定时器
	m_pTimerManager->AddTimer("violation_check_timer", 1, 0);
	CViolationManager::Instance()->OnStart();

	ICC_LOG_DEBUG(m_pLog, "dbprocess start success.");

	/*ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ACDCallStateSync, OnNotifiCallStateSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallOverSync, OnNotifiCallOverSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeviceStateSync, OnNotifiDeviceStateSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallbackSync, OnNotifiCallBackSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_TakeOverCallSync, OnNotifiTakeOverCallSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RemoveReleaseCallRequest, OnNotifiRemoveReleaseCallRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_CallEventQueryRequest, OnNotifiQueryCallEventRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_BlackCallQueryRequest, OnNotifiQueryBlackCallRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_LoadAllBlackListRequest, OnNotifiLoadAllBlackListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetBlackListSync, OnNotifiSetBlackListSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteBlackListSync, OnNotifiDeleteBlackListSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteAllBlackListSync, OnNotifiDeleteAllBlackListSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_RedoSQLTimer, OnRedoSQLTimer);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_SetRedListRequest, OnNotifiSetRedListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_DeleteRedListRequest, OnNotifiDeleteRedListRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetAllRedListRequest, OnNotifiGetRedListRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_GetViolationRequest, OnNotifiGetViolationRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "violation_check_timer", OnViolationCheckTimer);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, Cmd_ACDAgentStateSync, OnRecvAcdAgentStateSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_violation_timeout_request", OnGetViolationTimeoutCfgRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_process_call_info", OnReceiveAlarmRelationRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_process_call_info", OnReceiveQueryProcessCallRequest);*/
}

void CBusinessImpl::OnStop()
{
	_DestoryThreads();

	ICC_LOG_DEBUG(m_pLog, "dbprocess stop success.");
}

void CBusinessImpl::OnDestroy()
{
	//
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadPhoneDivision()
{
	DataBase::SQLRequest m_Query;
	m_Query.sql_id = "select_icc_t_phone_division";

	//	使用主线程的数据库链接
	DataBase::IResultSetPtr l_pResult = m_pInitDBConn->Exec(m_Query);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Load all phone division failed, SQL: [%s], Error: [%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Load all phone division success, SQL: [%s]",
			l_pResult->GetSQL().c_str());

		std::map<std::string, std::string> tmp_mapPhoneInfo;

		while (l_pResult->Next())
		{
			std::string l_strPhonePre = l_pResult->GetValue("phone_pre");
			std::string l_strOutCallHead = l_pResult->GetValue("out_call_head");

			if (!l_strOutCallHead.empty())
			{
				tmp_mapPhoneInfo[l_strPhonePre] = l_strOutCallHead;
			}
		}
		if (tmp_mapPhoneInfo.size() > 0)
		{
			m_pRedisClient->HMSet(CTI_OUTCALL_HEAD, tmp_mapPhoneInfo);
		}

		ICC_LOG_DEBUG(m_pLog, "End Load all phone division, Count: [%u]", tmp_mapPhoneInfo.size());
	}
}

PROTOCOL::CHeader CBusinessImpl::CreateProtocolHeader(const std::string& p_strCmd, const std::string& p_strRequest,
	const std::string& p_strRequestType, const std::string& p_strRelatedId)
{
	PROTOCOL::CHeader l_oHeader;

	l_oHeader.m_strSystemID = "ICC";
	l_oHeader.m_strSubsystemID = Server_Name_DBProcess;
	l_oHeader.m_strMsgid = m_pStringUtil->CreateGuid();
	l_oHeader.m_strRelatedID = p_strRelatedId;
	l_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oHeader.m_strCmd = p_strCmd;
	l_oHeader.m_strRequest = p_strRequest;
	l_oHeader.m_strRequestType = p_strRequestType;
	l_oHeader.m_strResponse = "";
	l_oHeader.m_strResponseType = "";

	return l_oHeader;
}

bool CBusinessImpl::CallRefIdIsExist(const std::string& p_strCTICallRefId)
{
	bool l_bKeyExist = false;

	std::string l_strInfo;
	if (m_pRedisClient->HGet(CALL_INFO, p_strCTICallRefId, l_strInfo))
	{
		if (!l_strInfo.empty())
		{
			l_bKeyExist = true;
		}
	}

	return l_bKeyExist;
}

bool CBusinessImpl::CallRefIdIsExistByCallTable(const std::string& p_strCTICallRefId)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_callevent";
	l_SQLRequest.param["callref_id"] = p_strCTICallRefId;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "CallRefIdIsExistByCallTable sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}
	unsigned int l_iRowNum = l_Result->RecordSize();
	return l_iRowNum == 0 ? false : true;
}

std::string CBusinessImpl::GetRelAlarmIDByCallRefID(const std::string p_strCTICallRefId)
{
	if (p_strCTICallRefId.empty()) {
		return "";
	}
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_callback_relation";
	l_SQLRequest.param["callref_id"] = p_strCTICallRefId;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "GetRelAlarmIDByCallRefID sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return "";
	}
	unsigned int l_iRowNum = l_Result->RecordSize();
	while (l_Result->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "GetRelAlarmIDByCallRefID :[%s]", l_Result->GetValue("alarm_id").c_str());
		return l_Result->GetValue("alarm_id");
	}
	return "";
}

void CBusinessImpl::DeleteCallRefId(const std::string& p_strCTICallRefId)
{
	m_pRedisClient->HDel(CALL_INFO, p_strCTICallRefId);

	ICC_LOG_DEBUG(m_pLog, "Delete CallInfo From Redis, CallRefId: [%s]", p_strCTICallRefId.c_str());
}
void CBusinessImpl::AddCallInfo(const std::string& p_strCTICallRefId, const std::string& p_strCallerId, const std::string& p_strCalledId,
	const std::string& p_strState, const std::string& p_strTime, const std::string& p_strCstCallrefId)
{
	// 避免Redis 的 key 值数据为空
	if (p_strCTICallRefId.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "Insert CallInfo To Redis, Callid is empty");
		return;
	}
	PROTOCOL::CCallInfo l_oCallInfo;
	l_oCallInfo.m_strCallRefId = p_strCTICallRefId;
	l_oCallInfo.m_strCallerId = p_strCallerId;
	l_oCallInfo.m_strCalledId = p_strCalledId;
	l_oCallInfo.m_strState = p_strState;
	l_oCallInfo.m_strTime = p_strTime;
	l_oCallInfo.m_strCstCallrefId = p_strCstCallrefId;

	
	std::string l_strInfo = l_oCallInfo.ToString(m_pJsonFty->CreateJson());

	if (!p_strCTICallRefId.empty() && !l_strInfo.empty())
	{
		m_pRedisClient->HSet(CALL_INFO, p_strCTICallRefId, l_strInfo);
		ICC_LOG_DEBUG(m_pLog, "Insert CallInfo To Redis, CallInfo: [%s]", l_strInfo.c_str());
	}
}

void CBusinessImpl::CheckCSTCallrefId(std::string& p_strCTICallRefId, const std::string& p_strCstCallrefId)
{
	if (p_strCTICallRefId.empty()) {
		return;
	}
	std::string strTemp = p_strCTICallRefId.substr(p_strCTICallRefId.size() - 4, 4);
	ICC_LOG_DEBUG(m_pLog, "CheckCSTCallrefId [%s] CallInfo: [%s]", strTemp.c_str(), p_strCTICallRefId.c_str());
	if (strTemp != p_strCstCallrefId) {
		std::map<std::string, std::string> l_mapCallInfo;
		m_pRedisClient->HGetAll(CALL_INFO, l_mapCallInfo);
		for (auto it = l_mapCallInfo.begin(); it != l_mapCallInfo.end(); it++) {
			if (!it->first.empty() && it->first.substr(it->first.size() - 4, 4) == p_strCstCallrefId && p_strCTICallRefId != it->first) {
				m_pRedisClient->HDel(CALL_INFO, it->first);
				return;
			}
		}
	}
}

void CBusinessImpl::InsertSubCallEvent(const std::string& p_strCallRefId, const std::string& p_strState, const std::string& p_strStateTime,
	const std::string& p_strSponsor, const std::string& p_strReveiver)
{
	DataBase::SQLRequest l_strSubCallEventInsertSql;
	l_strSubCallEventInsertSql.sql_id = "insert_icc_t_sub_callevent";
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("guid", m_pStringUtil->CreateGuid()));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("callref_id", p_strCallRefId));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("eventcall_state", p_strState));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("state_time", p_strStateTime));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("sponsor", p_strSponsor));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("receiver", p_strReveiver));
	l_strSubCallEventInsertSql.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->GetCallRefIdTime(p_strCallRefId)));

	DataBase::IResultSetPtr l_pSubCallEventResult;
	l_pSubCallEventResult = m_pRequestDBConn->Exec(l_strSubCallEventInsertSql);
	if (!l_pSubCallEventResult->IsValid())
	{
		std::string l_strSQL = l_pSubCallEventResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "Insert icc_t_sub_callevent table failed, SQL: [%s], Error: [%s]",
			l_strSQL.c_str(),
			l_pSubCallEventResult->GetErrorMsg().c_str());

		DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
		std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
		m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
		ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
			l_strField.c_str(), l_strSQL.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_sub_callevent table success, SQL: [%s]",
			l_pSubCallEventResult->GetSQL().c_str());
	}
}
void CBusinessImpl::OnNotifiCallStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive call state event: %s", l_strRequestMsg.c_str());

		PROTOCOL::CCallStateEvent l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid call state event request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strCTICallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
		std::string l_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
		std::string l_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
		std::string l_strState = l_oRequestObj.m_oBody.m_strState;
		std::string l_strStateTime = l_oRequestObj.m_oBody.m_strTime;

		this->InsertSubCallEvent(l_strCTICallRefId, l_strState, l_strStateTime, l_strCallerId, l_strCalledId);
	}
}

std::string CBusinessImpl::CalcRingLen(std::string l_strTalkTime, std::string l_strRingTime, std::string l_strHangupTime, std::string l_strRingBackTime)
{
	std::string first_time;
	std::string second_time;
	if (!l_strTalkTime.empty()) {
		first_time = l_strTalkTime;
	}
	else {
		if (!l_strHangupTime.empty()) {
			first_time = l_strHangupTime;
		}
		else {
			ICC_LOG_DEBUG(m_pLog, " call  event l_strTalkTime and l_strHangupTime : %s - %s", l_strTalkTime.c_str(), l_strHangupTime.c_str());
			return "";
		}
	}

	if (!l_strRingTime.empty()) {
		second_time = l_strRingTime;
	}
	else {
		if (!l_strRingBackTime.empty()) {
			second_time = l_strRingBackTime;
		}
		else {
			ICC_LOG_DEBUG(m_pLog, " call  event l_strRingTime and l_strRingBackTime : %s - %s", l_strRingTime.c_str(), l_strRingBackTime.c_str());
			return "";
		}
	}
	if (!l_strRingTime.empty() && !l_strRingBackTime.empty()) {
		DateTime::CDateTime t_RingTime = m_pDateTime->FromString(l_strRingTime);
		DateTime::CDateTime t_RingBackTime = m_pDateTime->FromString(l_strRingBackTime);
		second_time = t_RingTime > t_RingBackTime ? l_strRingBackTime : l_strRingTime;
	}
	int dif_day = m_pDateTime->DaysDifference(m_pDateTime->FromString(first_time), m_pDateTime->FromString(second_time));
	std::string dif_time = m_pDateTime->ToString(m_pDateTime->FromString(first_time) - m_pDateTime->FromString(second_time));
	dif_time = m_pStringUtil->Right(dif_time, dif_time.size() - 11);
	dif_time = std::to_string(m_pStringUtil->ToInt(m_pStringUtil->Left(dif_time, 2)) + dif_day * 24) + m_pStringUtil->Right(dif_time, dif_time.size() - 2);

	return dif_time;
}


std::string CBusinessImpl::BuildTimeLen(std::string first_time, std::string second_time)
{
	int dif_day = m_pDateTime->DaysDifference(m_pDateTime->FromString(first_time), m_pDateTime->FromString(second_time));
	std::string dif_time = m_pDateTime->ToString(m_pDateTime->FromString(first_time) - m_pDateTime->FromString(second_time));
	dif_time = m_pStringUtil->Right(dif_time, dif_time.size() - 11);
	dif_time = std::to_string(m_pStringUtil->ToInt(m_pStringUtil->Left(dif_time, 2)) + dif_day * 24) + m_pStringUtil->Right(dif_time, dif_time.size() - 2);

	return dif_time;
}

void CBusinessImpl::OnNotifiCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive call over event: %s", l_strRequestMsg.c_str());

		PROTOCOL::CCallOverEvent l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid call over request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
		std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
		std::string l_strOriginalACD = l_oRequestObj.m_oBody.m_strOriginalACD;
		std::string l_strACDDept = l_oRequestObj.m_oBody.m_strACDDept;
		std::string l_strCTICallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
		std::string l_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
		std::string l_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
		std::string l_strCallDirection = l_oRequestObj.m_oBody.m_strCallDirection;
		std::string l_strDialTime = l_oRequestObj.m_oBody.m_strDialTime;
		std::string l_strIncomingTime = l_oRequestObj.m_oBody.m_strIncomingTime;
		std::string l_strAssignTime = l_oRequestObj.m_oBody.m_strAssignTime;
		std::string l_strRingTime = l_oRequestObj.m_oBody.m_strRingTime;
		std::string l_strRingBackTime = l_oRequestObj.m_oBody.m_strRingBackTime;
		std::string l_strReleaseTime = l_oRequestObj.m_oBody.m_strReleaseTime;
		std::string l_strTalkTime = l_oRequestObj.m_oBody.m_strTalkTime;
		std::string l_strHangupTime = l_oRequestObj.m_oBody.m_strHangupTime;
		std::string l_strHangupType = l_oRequestObj.m_oBody.m_strHangupType;
		std::string l_strSwitchType = l_oRequestObj.m_oBody.m_strSwitchType;

		std::string l_strUpdateTime;
		
		std::string strAcdDept;
		std::string strAcdDeptName;
		std::string strAcdDeptPath;
		std::string strAcdDeptIdentify;
		if (!l_strOriginalACD.empty())
		{
			strAcdDept = _QueryAcdDept(l_strOriginalACD);
			if (!strAcdDept.empty())
			{
				if (!_QueryDeptNamePath(strAcdDept, strAcdDeptName, strAcdDeptPath, strAcdDeptIdentify))
				{
					ICC_LOG_ERROR(m_pLog, "not find dept name and path! code:[%s]", strAcdDept.c_str());
				}				
			}
		}

		std::string strCallerType = _QueryNumberType(l_strCallerId);
		std::string strCalledType = _QueryNumberType(l_strCalledId);

		std::string strCallerDept = _QueryDeptByNumber(l_strCallerId, strCallerType);
		std::string strCallerDeptName;
		std::string strCallerDeptPath;
		std::string strCallerDeptIdentify;
		if (!strCallerDept.empty())
		{
			if (!_QueryDeptNamePath(strCallerDept, strCallerDeptName, strCallerDeptPath, strCallerDeptIdentify))
			{
				ICC_LOG_ERROR(m_pLog, "not find dept name and path! code:[%s]", strAcdDept.c_str());
			}
		}

		std::string strCalledDept;
		std::string strCalledDeptName;
		std::string strCalledDeptPath;
		std::string strCalledDeptIdentify;
		if (!l_strCalledId.empty())
		{
			strCalledDept = _QueryDeptByNumber(l_strCalledId, strCalledType);
			if (!strCalledDept.empty())
			{
				if (!_QueryDeptNamePath(strCalledDept, strCalledDeptName, strCalledDeptPath, strCalledDeptIdentify))
				{
					ICC_LOG_ERROR(m_pLog, "not find dept name and path! code:[%s]", strAcdDept.c_str());
				}
			}
		}		

		std::string strReceiveCode;
		std::string strReceiveName;
		if (strCalledType == NumberType_ReceiveSeat)
		{
			_QueryReveiverInfo(l_strCalledId, strReceiveCode, strReceiveName);
		}

		DataBase::SQLRequest l_strCallEventSql;
		if (!this->CallRefIdIsExistByCallTable(l_strCTICallRefId))
		{
			//	数据库中不存在该话务记录，新增
			l_strCallEventSql.sql_id = "insert_icc_t_callevent";
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCTICallRefId));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd", l_strACD));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("original_acd", l_strOriginalACD));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd_dept", l_strACDDept));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_id", l_strCallerId));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_id", l_strCalledId));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("call_direction", l_strCallDirection));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("incoming_time", l_strIncomingTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("dial_time", l_strDialTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("ring_time", l_strRingTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("ringback_time", l_strRingBackTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("talk_time", l_strTalkTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("release_time", l_strReleaseTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("hangup_time", l_strHangupTime));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("hangup_type", l_strHangupType));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("switch_type", l_strSwitchType));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd_relation_dept", strAcdDept));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_relation_dept", strCallerDept));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_relation_dept", strCalledDept));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd_relation_dept_name", strAcdDeptName));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_relation_dept_name", strCallerDeptName));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_relation_dept_name", strCalledDeptName));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd_relation_dept_path", strAcdDeptPath));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_relation_dept_path", strCallerDeptPath));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_relation_dept_path", strCalledDeptPath));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("acd_relation_dept_identify", strAcdDeptIdentify));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_relation_dept_identify", strCallerDeptIdentify));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_relation_dept_identify", strCalledDeptIdentify));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("caller_type", strCallerType));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("called_type", strCalledType));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("receiver_code", strReceiveCode));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("receiver_name", strReceiveName));
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->GetCallRefIdTime(l_strCTICallRefId)));

			if (!l_strRingTime.empty() && !l_strIncomingTime.empty())
			{
				std::string dif_time = BuildTimeLen(l_strRingTime, l_strIncomingTime);
				l_strCallEventSql.param.insert(std::pair<std::string, std::string>("incoming_len", dif_time));
			} 
			else
				l_strCallEventSql.param.insert(std::pair<std::string, std::string>("incoming_len", ""));

			std::string strRing_time = CalcRingLen(l_strTalkTime, l_strRingTime, l_strHangupTime, l_strRingBackTime);
			if (!strRing_time.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("ring_len", strRing_time));
			}

			if (!l_strHangupTime.empty() && !l_strTalkTime.empty())
			{
				std::string dif_time = BuildTimeLen(l_strHangupTime, l_strTalkTime);
				l_strCallEventSql.param.insert(std::pair<std::string, std::string>("talk_len", dif_time));
			}
			else
				l_strCallEventSql.param.insert(std::pair<std::string, std::string>("talk_len", ""));

			if (!l_strIncomingTime.empty())
			{
				l_strUpdateTime = l_strIncomingTime;
			}
			else
			{
				l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			}
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));
		}
		else
		{
			//	数据库中已存在该话务记录，修改
			l_strCallEventSql.sql_id = "update_icc_t_callevent";
			l_strCallEventSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCTICallRefId));
			std::string strTime = m_pDateTime->GetCallRefIdTime(l_strCTICallRefId);
			if (strTime != "")
			{
				l_strCallEventSql.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_strCallEventSql.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
			}

			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd", l_strACD));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("original_acd", l_strOriginalACD));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd_dept", l_strACDDept));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_id", l_strCallerId));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_id", l_strCalledId));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("call_direction", l_strCallDirection));
			if (!l_strIncomingTime.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("incoming_time", l_strIncomingTime));
			}
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("dial_time", l_strDialTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("ring_time", l_strRingTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("ringback_time", l_strRingBackTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("talk_time", l_strTalkTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("release_time", l_strReleaseTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("hangup_time", l_strHangupTime));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("hangup_type", l_strHangupType));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("switch_type", l_strSwitchType));

			l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));

			if (!l_strRingTime.empty() && !l_strIncomingTime.empty())
			{
				std::string dif_time = BuildTimeLen(l_strRingTime, l_strIncomingTime);
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("incoming_len", dif_time));
			}

			std::string strRing_time = CalcRingLen(l_strTalkTime, l_strRingTime, l_strHangupTime, l_strRingBackTime);
			if (!strRing_time.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("ring_len", strRing_time));
			}

			if (!l_strHangupTime.empty() && !l_strTalkTime.empty())
			{
				std::string dif_time = BuildTimeLen(l_strHangupTime, l_strTalkTime);
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("talk_len", dif_time));
			}

			if (!strAcdDept.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd_relation_dept", strAcdDept));
			}
			
			if (!strCallerDept.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_relation_dept", strCallerDept));
			}			

			if (!strCalledDept.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_relation_dept", strCalledDept));
			}	

			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_type", strCallerType));
			l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_type", strCalledType));

			if (!strAcdDeptName.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd_relation_dept_name", strAcdDeptName));
			}

			if (!strCallerDeptName.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_relation_dept_name", strCallerDeptName));
			}
			
			if (!strCalledDeptName.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_relation_dept_name", strCalledDeptName));
			}
			
			if (!strAcdDeptPath.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd_relation_dept_path", strAcdDeptPath));
			}
			
			if (!strCallerDeptPath.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_relation_dept_path", strCallerDeptPath));
			}
			
			if (!strCalledDeptPath.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_relation_dept_path", strCalledDeptPath));
			}
			if (!strAcdDeptIdentify.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("acd_relation_dept_identify", strAcdDeptIdentify));
			}
			if (!strCallerDeptIdentify.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("caller_relation_dept_identify", strCallerDeptIdentify));
			}
			if (!strCalledDeptIdentify.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("called_relation_dept_identify", strCalledDeptIdentify));
			}

			if (!strReceiveCode.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("receiver_code", strReceiveCode));
			}

			if (!strReceiveName.empty())
			{
				l_strCallEventSql.set.insert(std::pair<std::string, std::string>("receiver_name", strReceiveName));
			}
			
		}

		if (strCalledType == NumberType_ProcessSeat && strCallerType == NumberType_OutLine && l_strHangupType == "release")
		{
			_UpdateProcessSeatReleaseAcceptState(l_strCTICallRefId, "5");//未接听
		}
		// 添加一种场景 主叫和被叫都是接警席号码，内部呼叫，状态改为其他
		if (strCallerType == NumberType_ProcessSeat || (strCalledType == NumberType_ReceiveSeat && strCallerType == NumberType_ReceiveSeat))
		{
			_UpdateProcessSeatReleaseAcceptState(l_strCTICallRefId, "99");//主叫为处警坐席
		}

		DataBase::IResultSetPtr l_pCallEventResult;
		l_pCallEventResult = m_pRequestDBConn->Exec(l_strCallEventSql);
		if (!l_pCallEventResult->IsValid())
		{
			std::string l_strSQL = l_pCallEventResult->GetSQL();
			ICC_LOG_ERROR(m_pLog, "Exec icc_t_callevent table failed, SQL: [%s]. Error: [%s]",
				l_strSQL.c_str(),
				l_pCallEventResult->GetErrorMsg().c_str());

			DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
			std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
			m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
			ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
				l_strField.c_str(), l_strSQL.c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Exec icc_t_callevent table success, SQL: [%s]",
				l_pCallEventResult->GetSQL().c_str());
		}

		//if (!_UpdateStatisticInfo(l_strHangupType, l_strCallerId, l_strCalledId, l_strCallDirection))
		//{
		//}

		this->DeleteCallRefId(l_strCTICallRefId);
	}
}

//子话务结束同步处理
void CBusinessImpl::OnNotifiDetailCallOverSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (!p_pNotifiRequest)
	{
		return;
	}

	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive detail call over event: %s", l_strRequestMsg.c_str());

	PROTOCOL::CDetailCallOverSync l_oDetailCallOverObj;
	if (!l_oDetailCallOverObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid detail call over request protocol: [%s]", l_strRequestMsg.c_str());

		return;
	}

	DataBase::SQLRequest l_sqlDetailCallOver;
	l_sqlDetailCallOver.sql_id = "insert_icc_t_call_details";

	std::string l_strGUID = m_pStringUtil->CreateGuid();
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("guid", l_strGUID));
	std::string l_strNumberType;
	std::string l_strDeptCode = _QueryDeptByNumber(l_oDetailCallOverObj.m_oBody.m_strDeviceNum, l_strNumberType);

	std::string l_strDeptName;
	std::string l_strDeptPath;
	std::string l_strDeptIdentify;
	if (!l_strDeptCode.empty() && _QueryDeptNamePath(l_strDeptCode, l_strDeptName, l_strDeptPath, l_strDeptIdentify))
	{
		l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("dept_code", l_strDeptCode));
		l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("dept_path", l_strDeptPath));
		l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("dept_name", l_strDeptName));
		l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("dept_identify", l_strDeptIdentify));
	}

	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("callref_id", l_oDetailCallOverObj.m_oBody.m_strCallId));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("switch_type", l_oDetailCallOverObj.m_oBody.m_strSwitchType));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("relation_callrefid", l_oDetailCallOverObj.m_oBody.m_strRelationCallId));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("role", l_oDetailCallOverObj.m_oBody.m_strRole));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("device", l_oDetailCallOverObj.m_oBody.m_strDeviceNum));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("device_type", l_oDetailCallOverObj.m_oBody.m_strDeviceType));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("caller_id", l_oDetailCallOverObj.m_oBody.m_strCaller));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("called_id", l_oDetailCallOverObj.m_oBody.m_strCalled));

	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("setup_time", ""));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("ring_time", l_oDetailCallOverObj.m_oBody.m_strRingTime));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("talk_time", l_oDetailCallOverObj.m_oBody.m_strTalkTime));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("in_meeting_time", l_oDetailCallOverObj.m_oBody.m_strJoinTime));
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("hangup_time", l_oDetailCallOverObj.m_oBody.m_strHangupTime));
	//l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("record_id", l_oDetailCallOverObj.m_oBody));

	if (l_oDetailCallOverObj.m_oBody.m_strRingTime.empty() && l_oDetailCallOverObj.m_oBody.m_strTalkTime.empty())
	{
		DateTime::CDateTime l_dtRingTime = m_pDateTime->FromString(l_oDetailCallOverObj.m_oBody.m_strRingTime);
		DateTime::CDateTime l_dtTalkTime = m_pDateTime->FromString(l_oDetailCallOverObj.m_oBody.m_strTalkTime);

		unsigned int l_diffTime = m_pDateTime->SecondsDifference(l_dtTalkTime, l_dtRingTime);
		l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("ring_len", m_pStringUtil->Format("%d", l_diffTime)));
	}
	
	if (!l_oDetailCallOverObj.m_oBody.m_strHangupTime.empty())
	{
		DateTime::CDateTime l_dtHangupTime = m_pDateTime->FromString(l_oDetailCallOverObj.m_oBody.m_strHangupTime);
		if (!l_oDetailCallOverObj.m_oBody.m_strTalkTime.empty())
		{
			DateTime::CDateTime l_dtTalkTime = m_pDateTime->FromString(l_oDetailCallOverObj.m_oBody.m_strTalkTime);
			unsigned int l_diffTime = m_pDateTime->SecondsDifference(l_dtHangupTime, l_dtTalkTime);
			l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("talk_len", m_pStringUtil->Format("%d", l_diffTime)));
		}

		if (!l_oDetailCallOverObj.m_oBody.m_strJoinTime.empty())
		{
			DateTime::CDateTime l_dtJoinTime = m_pDateTime->FromString(l_oDetailCallOverObj.m_oBody.m_strJoinTime);
			unsigned int l_diffTime = m_pDateTime->SecondsDifference(l_dtHangupTime, l_dtJoinTime);
			l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("meeting_time", m_pStringUtil->Format("%d", l_diffTime)));
		}
	}
	
	l_sqlDetailCallOver.set.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->CurrentDateTimeStr()));
	DataBase::IResultSetPtr l_pCallEventResult;
	l_pCallEventResult = m_pRequestDBConn->Exec(l_sqlDetailCallOver);
	if (!l_pCallEventResult->IsValid())
	{
		std::string l_strSQL = l_pCallEventResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "Exec insert_icc_t_detailcall table failed, SQL: [%s]. Error: [%s]",
			l_strSQL.c_str(),
			l_pCallEventResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Exec insert_icc_t_detailcall table success, SQL: [%s]",l_pCallEventResult->GetSQL().c_str());
	}
}

bool CBusinessImpl::_UpdateStatisticInfo(const std::string& hangup_type, const std::string& caller_id, const std::string& called_id, const std::string& call_direction)
{
	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_statistic_by_callevent";
	l_oSQLUpDate.param["hangup_type"] = hangup_type;
	l_oSQLUpDate.param["caller_id"] = caller_id;
	l_oSQLUpDate.param["called_id"] = called_id;
	l_oSQLUpDate.param["call_direction"] = call_direction;

	DataBase::IResultSetPtr l_oResult = m_pRequestDBConn->Exec(l_oSQLUpDate);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for call num Failed: sql id[%s]", l_oSQLUpDate.sql_id.c_str());
		return false;
	}

	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for call num Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;;
	}

	ICC_LOG_DEBUG(m_pLog, "update statistic info for call num success:[%s][%s]", l_oSQLUpDate.sql_id.c_str(), l_oResult->GetSQL().c_str());

	return true;
}

void CBusinessImpl::OnNotifiDeviceStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive device state event: %s, m_strEnableProcessAlarm : %s ", l_strRequestMsg.c_str(), m_strEnableProcessAlarm.c_str());
		boost::shared_ptr< PROTOCOL::CDeviceStateEvent> l_oRequestObj = boost::make_shared<PROTOCOL::CDeviceStateEvent>();
		if (!l_oRequestObj->ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid device state event request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strDevice = l_oRequestObj->m_oBody.m_strDevice;
		std::string l_strDeviceType = l_oRequestObj->m_oBody.m_strDeviceType;
		std::string l_strCTICallRefId = l_oRequestObj->m_oBody.m_strCallRefId;
		std::string l_strCallerId = l_oRequestObj->m_oBody.m_strCallerId;
		std::string l_strCalledId = l_oRequestObj->m_oBody.m_strCalledId;
		std::string l_strCallDirection = l_oRequestObj->m_oBody.m_strCallDirection;
		std::string l_strState = l_oRequestObj->m_oBody.m_strState;
		std::string l_strStateTime = l_oRequestObj->m_oBody.m_strTime;
		std::string l_strDeptCode = l_oRequestObj->m_oBody.m_strDeptCode;
		std::string l_strCSTACallRefId = l_oRequestObj->m_oBody.m_strCSTACallRefId;

		std::string l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

		if (l_strState.compare(CallStateString[STATE_RING]) == 0)
		{
			if (!this->CallRefIdIsExistByCallTable(l_strCTICallRefId))
			{
				//2023.4.18添加，主动告警坐席无警员登录
				std::string l_strConfigMsg;
				std::string  l_strClientRegisterInfo;
				if (!m_pRedisClient->HGet("ClientRegisterInfo", l_strDevice, l_strClientRegisterInfo))
				{
					ICC_LOG_DEBUG(m_pLog, " Telephone Coming ClientRegisterInfo hget failed!!!");
				}
				ICC_LOG_DEBUG(m_pLog, "l_strClientRegisterInfo:[%s]", l_strClientRegisterInfo.c_str());
				if (l_strClientRegisterInfo.empty())
				{
					BuildManualAcceptConfig("TelephoneComingAlarm", l_strConfigMsg);

					l_strConfigMsg = m_pStringUtil->ReplaceFirst(l_strConfigMsg, "$", l_strDevice);
					ICC_LOG_DEBUG(m_pLog, "TelephoneComingAlarm Message:[%s]", l_strConfigMsg.c_str());
					m_pMsgCenter->Send(l_strConfigMsg, l_strDevice, l_strDeptCode, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
				}

				//	振铃状态，插入 insert_icc_t_callevent 表
				DataBase::SQLRequest l_strCallEventInsertSql;
				l_strCallEventInsertSql.sql_id = "insert_icc_t_callevent";
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCTICallRefId));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("caller_id", l_strCallerId));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("called_id", l_strCalledId));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("call_direction", l_strCallDirection));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("ring_time", l_strStateTime));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->GetCallRefIdTime(l_strCTICallRefId)));
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));
				std::string strRelAlarmID = GetRelAlarmIDByCallRefID(l_strCTICallRefId);
				l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("rel_alarm_id", strRelAlarmID));
				DataBase::IResultSetPtr l_pCallEventResult;
				l_pCallEventResult = m_pRequestDBConn->Exec(l_strCallEventInsertSql);
				if (!l_pCallEventResult->IsValid())
				{
					std::string l_strSQL = l_pCallEventResult->GetSQL();
					ICC_LOG_ERROR(m_pLog, "Insert icc_t_callevent table failed, SQL: [%s], Error: [%s]",
						l_strSQL.c_str(),
						l_pCallEventResult->GetErrorMsg().c_str());

					DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
					std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
					m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
					ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
						l_strField.c_str(), l_strSQL.c_str());
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "Insert icc_t_callevent table success, SQL: [%s]",
						l_pCallEventResult->GetSQL().c_str());
				}
			}
		}	// end if ring
		if (l_strState.compare(CallStateString[STATE_TALK]) == 0)
		{
			//	通话状态，更新 update_icc_t_callevent 表
			DataBase::SQLRequest l_strCallEventUpdateSql;
			if (!this->CallRefIdIsExistByCallTable(l_strCTICallRefId))
			{
				l_strCallEventUpdateSql.sql_id = "insert_icc_t_callevent";
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCTICallRefId));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("caller_id", l_strCallerId));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("called_id", l_strCalledId));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("call_direction", l_strCallDirection));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("talk_time", l_strStateTime));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->GetCallRefIdTime(l_strCTICallRefId)));
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));
				std::string strRelAlarmID = GetRelAlarmIDByCallRefID(l_strCTICallRefId);
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("rel_alarm_id", strRelAlarmID));
			}
			else
			{
				l_strCallEventUpdateSql.sql_id = "update_icc_t_callevent";
				l_strCallEventUpdateSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCTICallRefId));
				std::string strTime = m_pDateTime->GetCallRefIdTime(l_strCTICallRefId);
				if (strTime != "")
				{
					l_strCallEventUpdateSql.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
					l_strCallEventUpdateSql.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
				}

				l_strCallEventUpdateSql.set.insert(std::pair<std::string, std::string>("caller_id", l_strCallerId));
				l_strCallEventUpdateSql.set.insert(std::pair<std::string, std::string>("called_id", l_strCalledId));
				l_strCallEventUpdateSql.set.insert(std::pair<std::string, std::string>("call_direction", l_strCallDirection));
				l_strCallEventUpdateSql.set.insert(std::pair<std::string, std::string>("talk_time", l_strStateTime));

				l_strCallEventUpdateSql.set.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));
			}
			
			DataBase::IResultSetPtr l_pCallEventResult;
			l_pCallEventResult = m_pRequestDBConn->Exec(l_strCallEventUpdateSql);
			if (!l_pCallEventResult->IsValid())
			{
				std::string l_strSQL = l_pCallEventResult->GetSQL();
				ICC_LOG_ERROR(m_pLog, "Exec icc_t_callevent table failed, SQL: [%s], Error: [%s]",
					l_strSQL.c_str(),
					l_pCallEventResult->GetErrorMsg().c_str());

				DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
				std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
				m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
				ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
					l_strField.c_str(), l_strSQL.c_str());
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Exec icc_t_callevent table success, SQL: [%s]",
					l_pCallEventResult->GetSQL().c_str());
			}
		}	// end if talk
		CheckCSTCallrefId(l_strCTICallRefId, l_strCSTACallRefId);
		AddCallInfo(l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strState, l_strStateTime, l_strCSTACallRefId);
		//	设备状态保存到话务子表 icc_t_sub_callevent
		this->InsertSubCallEvent(l_strCTICallRefId, l_strState, l_strStateTime, l_strDevice, "");

		//违规检查
		CViolationManager::Instance()->SetViolationCheck(0, l_strState, l_strDevice,"", l_strCTICallRefId);

		if (m_strEnableProcessAlarm.compare("1") == 0 && !_IsAutoUrgeCallNum(l_strCallerId) && !_IsAutoUrgeCallNum(l_strCalledId))
		{
			_GetThread(0).AddMessage(this, ThreadId_DeviceStateChanged, 0, 0, 0, "", "", nullptr, nullptr, l_oRequestObj);
		}
		else 
		{
			ICC_LOG_DEBUG(m_pLog, "unenable config ThreadId_DeviceStateChanged: [%s]", m_strEnableProcessAlarm.c_str());
		}
	}
}

bool CBusinessImpl::_IsAutoUrgeCallNum(std::string strCaller)
{
	if (m_strAutoUrgeCallNumber.empty())
	{
		if (!m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", m_strAutoUrgeCallNumber))
		{
			ICC_LOG_ERROR(m_pLog, "Get AutoUrge Call Num Error, Use Default Configuration : 8888");
			m_strAutoUrgeCallNumber = "8888";
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Get AutoUrge Call Num %s", m_strAutoUrgeCallNumber.c_str());
		}
	}

	if (m_strAutoUrgeCallNumber == strCaller)
	{
		return true;
	}

	return false;
}

void CBusinessImpl::OnNotifiTakeOverCallSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive take over call sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CTakeOverCallRespond l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid take over call sync request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strCTICallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
		std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
		std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
		std::string l_strCaseId = this->QueryAlarmId(l_strCTICallRefId);
		if (!l_strCaseId.empty())
		{
			PROTOCOL::CTakeOverCallSync l_oSyncObj;
			l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_TakeOverCallSync, Topic_CTIService, SendType_Topic);
			l_oSyncObj.m_oBody.m_strCallRefId = l_strCTICallRefId;
			l_oSyncObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oSyncObj.m_oBody.m_strTarget = l_strTarget;
			l_oSyncObj.m_oBody.m_strCaseId = l_strCaseId;

			std::string l_strSyncMsg = l_oSyncObj.ToString(m_pJsonFty->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
			ICC_LOG_DEBUG(m_pLog, "Send TakeOverCall Sync Topic: [%s]", l_strSyncMsg.c_str());
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "TakeOverCall Sync Get CaseId Failed, RelatedCallRefId: [%s]", l_strCTICallRefId.c_str());
		}
	}
}

void CBusinessImpl::SyncAlarmLogInfo(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strLogContent, const std::string& p_strTime, const std::string& strGuid, const std::string& strReceivedTime)
{
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader = CreateProtocolHeader(Cmd_AlarmLogSync, Topic_Alarm, SendType_Topic);

	l_oAlarmLogSync.m_oBody.m_strID = strGuid;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_strCaseId;
	l_oAlarmLogSync.m_oBody.m_strOperate = Dic_CallBackLog;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_strLogContent;
	std::string type = Dic_ResouceType_Phone;
	std::string id = p_strCTICallRefId;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = GenAlarmLogAttach(type, id);
	l_oAlarmLogSync.m_oBody.m_strCreateUser = Server_Name_DBProcess;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_strTime;
	l_oAlarmLogSync.m_oBody.m_strSourceName = "icc";
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = strReceivedTime;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSyncMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "Send AlarmLogInfo Sync: [%s]", l_strSyncMessage.c_str());
}

std::string CBusinessImpl::GenAlarmLogAttach(std::string& type, std::string& id)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

	std::string l_strPath = "/";
	std::string l_strNum = std::to_string(0);

	if (0 == type.compare("7"))
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", "jjlyh");
	}
	else
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", type);
	}

	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/id", id);
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/name", "");
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/path", "");

	return l_pIJson->ToString();
}


void CBusinessImpl::InsertCallBackLog(const std::string& p_strCTICallRefId, const std::string& p_strCaseId, const std::string& p_strCallerId,
	const std::string& p_strCalledId, const std::string& p_strTime)
{
	//<Resource Key = "BS001009003" Value = "回拨[主叫-{0}, 被叫-{1}]呼叫时间[{2}]" / >
	//	回拨流水内容
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	l_pIJson->SetNodeValue("/param/0", p_strCallerId);
	l_pIJson->SetNodeValue("/param/1", p_strCalledId);
	l_pIJson->SetNodeValue("/param/2", l_strCurTime);
	std::string l_strContent = l_pIJson->ToString();

	DataBase::SQLRequest l_strCallBackLogSql;
	l_strCallBackLogSql.sql_id = "insert_icc_t_alarm_log";
	std::string strGuid = m_pStringUtil->CreateGuid();
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("id", strGuid));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("jjdbh", p_strCaseId));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("operate", Dic_CallBackLog));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("operate_content", l_strContent));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("create_user", Server_Name_DBProcess));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("create_time", l_strCurTime));
	std::string type = Dic_ResouceType_Phone;
	std::string id = p_strCTICallRefId;
	std::string attach = GenAlarmLogAttach(type, id);
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("source_name", "icc"));
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("operate_attach_desc", attach));

	//TODO::select_icc_t_jjdb_jjsj 查询jjsj
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strCaseId);
	std::string l_strReceivedTime;
	if (strTime != "")
	{
		DataBase::SQLRequest l_sqlReqeust;
		l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
		l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		l_sqlReqeust.param["jjdbh"] = p_strCaseId;
		DataBase::IResultSetPtr l_pRSetPtr = m_pRequestDBConn->Exec(l_sqlReqeust);
		if (!l_pRSetPtr->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
		}
		if (l_pRSetPtr->Next())
		{
			l_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
		}
	}
	if (l_strReceivedTime.empty())
	{
		l_strReceivedTime = l_strCurTime;
	}
	l_strCallBackLogSql.param.insert(std::pair<std::string, std::string>("jjsj", l_strReceivedTime));

	DataBase::IResultSetPtr l_pInsertResult;
	l_pInsertResult = m_pRequestDBConn->Exec(l_strCallBackLogSql);
	if (!l_pInsertResult->IsValid())
	{
		std::string l_strSQL = l_pInsertResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "Insert icc_t_alarm_log table failed, SQL: [%s], Error: [%s]",
			l_strSQL.c_str(),
			l_pInsertResult->GetErrorMsg().c_str());

		DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
		std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
		m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
		ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
			l_strField.c_str(), l_strSQL.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log table success, SQL: [%s]",
			l_pInsertResult->GetSQL().c_str());
	}

	this->SyncAlarmLogInfo(p_strCTICallRefId, p_strCaseId, l_strContent, l_strCurTime, strGuid, l_strReceivedTime);
}
std::string CBusinessImpl::QueryAlarmId(const std::string& p_strRelateCTICallRefId)
{
	std::string l_strAlarmId = "";

	DataBase::SQLRequest m_Query;
	m_Query.sql_id = "select_icc_t_jjdb";
	m_Query.param.insert(std::pair<std::string, std::string>("source_id", p_strRelateCTICallRefId));

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_strRelateCTICallRefId);
	if (strTime != "")
	{
		m_Query.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		m_Query.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(m_Query);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Get related caseid failed, SQL: [%s], Error: [%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Get related caseid success, SQL: [%s] ",
			l_pResult->GetSQL().c_str());

		if (l_pResult->Next())
		{
			l_strAlarmId = l_pResult->GetValue(0, "id");

			ICC_LOG_DEBUG(m_pLog, "Get CTICallRefId: [%s] related CaseId: [%s]",
				p_strRelateCTICallRefId.c_str(),
				l_strAlarmId.c_str());
		}
	}

	return l_strAlarmId;
}
void CBusinessImpl::InsertCallBackRelation(const std::string& p_strCTICallRefId, const std::string& p_strCaseId,
	const std::string& p_strReceiptCode, const std::string& p_strReceiptName, const std::string& p_strTime)
{
	std::string l_strAlarmId = p_strCaseId;

	DataBase::SQLRequest l_strCallBackRelationSql;
	l_strCallBackRelationSql.sql_id = "insert_icc_t_callback_relation";
	l_strCallBackRelationSql.param.insert(std::pair<std::string, std::string>("callref_id", p_strCTICallRefId));
	l_strCallBackRelationSql.param.insert(std::pair<std::string, std::string>("alarm_id", l_strAlarmId));
	l_strCallBackRelationSql.param.insert(std::pair<std::string, std::string>("receipt_code", p_strReceiptCode));
	l_strCallBackRelationSql.param.insert(std::pair<std::string, std::string>("receipt_name", p_strReceiptName));
	l_strCallBackRelationSql.param.insert(std::pair<std::string, std::string>("time", p_strTime));

	if (p_strCTICallRefId.empty())
	{
		ICC_LOG_WARNING(m_pLog, "Insert icc_t_callback_relation table failed, CTICallRefId empty !");

		return;
	}

	DataBase::IResultSetPtr l_pInsertResult = m_pRequestDBConn->Exec(l_strCallBackRelationSql);
	if (!l_pInsertResult->IsValid())
	{
		std::string l_strSQL = l_pInsertResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "Insert icc_t_callback_relation table failed, SQL: [%s], Error: [%s]",
			l_strSQL.c_str(),
			l_pInsertResult->GetErrorMsg().c_str());

		DateTime::CDateTime l_CurrentTime = m_pDateTime->CurrentDateTime();
		std::string l_strField = m_pDateTime->ToString(l_CurrentTime, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
		m_pRedisClient->HSet(CTI_REDO_SQL, l_strField, l_strSQL);
		ICC_LOG_DEBUG(m_pLog, "Failed SQL add to Redis, Field: [%s], Value: [%s]",
			l_strField.c_str(), l_strSQL.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_callback_relation table success, SQL: [%s]",
			l_pInsertResult->GetSQL().c_str());
	}

	//更新话务表关联警单ID
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "update_icc_t_callevent";
	l_tSQLReq.param["callref_id"] = p_strCTICallRefId;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_strCTICallRefId);
	if (strTime != "")
	{
		l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLReq.set["rel_alarm_id"] = l_strAlarmId;
	l_tSQLReq.set["receiver_code"] = p_strReceiptCode;
	l_tSQLReq.set["receiver_name"] = p_strReceiptName;

	DataBase::IResultSetPtr l_pRSet = m_pRequestDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update callevent rel_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "update callevent rel_alarm_id success! callrefid:[%s]", p_strCTICallRefId.c_str());
	}
}
void CBusinessImpl::OnNotifiCallBackSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive call back sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CCallBackSync l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid call back sync protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strCTICallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
		std::string l_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
		std::string l_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
		std::string l_strCaseId = l_oRequestObj.m_oBody.m_strCaseId;
		std::string l_strRelateCallRefId = l_oRequestObj.m_oBody.m_strRelateCallRefId;
		std::string l_strReceiptCode = l_oRequestObj.m_oBody.m_strReceiptCode;
		std::string l_strReceiptName = l_oRequestObj.m_oBody.m_strReceiptName;
		std::string l_strStateTime = m_pDateTime->CurrentDateTimeStr();

		if (l_strCaseId.empty() && !l_strRelateCallRefId.empty())
		{
			//	警情 ID 为空，通过关联话务 ID 查找警情 ID
			l_strCaseId = this->QueryAlarmId(l_strRelateCallRefId);
		}

		//	回拨流水
		this->InsertCallBackLog(l_strCTICallRefId, l_strCaseId, l_strCallerId, l_strCalledId, l_strStateTime);

		//	回拨话务与警情关联记录
		this->InsertCallBackRelation(l_strCTICallRefId, l_strCaseId, l_strReceiptCode, l_strReceiptName, l_strStateTime);
	}
}

void CBusinessImpl::CR_RemoveReleaseCall(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strMsgId)
{
	PROTOCOL::CRemoveReleaseCallRespond l_oRespondObj;
//	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_RemoveReleaseCallRespond, "", SendType_Queue, p_strMsgId);
	l_oRespondObj.m_oHeader.m_strMsgId = p_strMsgId;
	l_oRespondObj.m_oBody.m_strResult = Result_Success;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_pJsonFty->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_pLog, "Send CR_RemoveReleaseCall Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Send CR_RemoveReleaseCall Respond: [%s] Failed. Error: ResCallbackPtr Invalid",
			l_strRespondMsg.c_str());
	}
}
void CBusinessImpl::Sync_RemoveReleaseCall(std::vector<std::string>& p_vCallRefIdQueue)
{
	PROTOCOL::CRemoveReleaseCallSync l_oSyncObj;

	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_RemoveReleaseCallSync, Topic_RemoveReleaseCallSync, SendType_Topic);

	l_oSyncObj.m_oBody.m_strCount = std::to_string(p_vCallRefIdQueue.size());
	for (std::size_t i = 0; i < p_vCallRefIdQueue.size(); ++i)
	{
		PROTOCOL::CRemoveReleaseCallSync::CBody::CData l_oData;

		l_oData.m_strCallRefId = p_vCallRefIdQueue[i];
		l_oSyncObj.m_oBody.m_vecData.push_back(l_oData);
	}

	std::string l_strSyncMessage = l_oSyncObj.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "Send RemoveReleaseCall Sync: [%s]", l_strSyncMessage.c_str());
}
void CBusinessImpl::OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive device dtate event: %s", l_strRequestMsg.c_str());

		PROTOCOL::CRemoveReleaseCallRequest l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid RemoveReleaseCall request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
		std::string l_strCount = l_oRequestObj.m_oBody.m_strCount;

		std::vector<std::string> l_vCallRefIdQueue;
		int l_nCount = atoi(l_strCount.c_str());
		for (int i = 0; i < l_nCount; ++i)
		{
			std::string l_strCallRefId = l_oRequestObj.m_oBody.m_vecData[i].m_strCallRefId;
			l_vCallRefIdQueue.push_back(l_strCallRefId);

			DataBase::SQLRequest l_strCallEventInsertSql;
			l_strCallEventInsertSql.sql_id = "update_icc_t_callevent";
			l_strCallEventInsertSql.param.insert(std::pair<std::string, std::string>("callref_id", l_strCallRefId));
			std::string strTime = m_pDateTime->GetCallRefIdTime(l_strCallRefId);
			if (strTime != "")
			{
				l_strCallEventInsertSql.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_strCallEventInsertSql.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
			}

			l_strCallEventInsertSql.set.insert(std::pair<std::string, std::string>("is_callback", ReleaseCall_IsCallBack));

			std::string l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			l_strCallEventInsertSql.set.insert(std::pair<std::string, std::string>("update_time", l_strUpdateTime));

			DataBase::IResultSetPtr l_pCallEventResult = m_pRequestDBConn->Exec(l_strCallEventInsertSql);
			if (!l_pCallEventResult->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "Update icc_t_callevent table failed, SQL: [%s]. Error: [%s]",
					l_pCallEventResult->GetSQL().c_str(),
					l_pCallEventResult->GetErrorMsg().c_str());
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Update icc_t_callevent table success, SQL: [%s]",
					l_pCallEventResult->GetSQL().c_str());
			}
		}

		//	移除早释话务应答
		this->CR_RemoveReleaseCall(p_pNotifiRequest, l_strMsgId);

		//	移除早释话务同步
		this->Sync_RemoveReleaseCall(l_vCallRefIdQueue);
	}
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadBlackList()
{
	//	加载所有未删除的黑名单
	PROTOCOL::CGetAllBlackListRespond l_oGetAllBlackListRespond;
//	l_oGetAllBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_LoadAllBlackListSync, Queue_CTIControl, SendType_Queue);
	l_oGetAllBlackListRespond.m_oHeader.m_strSystemID = "ICC";
	l_oGetAllBlackListRespond.m_oHeader.m_strSubsystemID = Server_Name_DBProcess;
	l_oGetAllBlackListRespond.m_oHeader.m_strMsgid = m_pStringUtil->CreateGuid();
	l_oGetAllBlackListRespond.m_oHeader.m_strRelatedID = "";
	l_oGetAllBlackListRespond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oGetAllBlackListRespond.m_oHeader.m_strCmd = Cmd_LoadAllBlackListSync;
	l_oGetAllBlackListRespond.m_oHeader.m_strRequest = Queue_CTIControl;
	l_oGetAllBlackListRespond.m_oHeader.m_strRequestType = SendType_Queue;
	l_oGetAllBlackListRespond.m_oHeader.m_strResponse = "";
	l_oGetAllBlackListRespond.m_oHeader.m_strResponseType = "";

	DataBase::SQLRequest m_Query;
	m_Query.sql_id = "select_icc_t_blacklist";
	m_Query.param.insert(std::pair<std::string, std::string>("is_delete", "false"));

	//	使用主线程的数据库链接
	DataBase::IResultSetPtr l_pResult = m_pInitDBConn->Exec(m_Query);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Load all blacklist failed, SQL: [%s], Error: [%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Load all blacklist success, SQL: [%s]",
			l_pResult->GetSQL().c_str());

		if (l_pResult->Next())
		{
			unsigned int l_iRowNum = l_pResult->RecordSize();
			for (unsigned int i = 0; i < l_iRowNum; i++)
			{
				PROTOCOL::CGetAllBlackListRespond::CBody::CData l_CData;
				l_CData.m_strLimitType = l_pResult->GetValue(i, "limit_type");
				l_CData.m_strLimitNum = l_pResult->GetValue(i, "limit_num");
				l_CData.m_strLimitMin = l_pResult->GetValue(i, "limit_minute");
				l_CData.m_strLimitReason = l_pResult->GetValue(i, "limit_reason");
				l_CData.m_strStaffCode = l_pResult->GetValue(i, "staff_code");
				l_CData.m_strStaffName = l_pResult->GetValue(i, "staff_name");
				l_CData.m_strBeginTime = l_pResult->GetValue(i, "begin_time");
				l_CData.m_strEndTime = l_pResult->GetValue(i, "end_time");
				l_CData.m_strApprover = l_pResult->GetValue(i, "approver");
				l_CData.m_strApprovedInfo = l_pResult->GetValue(i, "approved_info");
				l_CData.m_strReceiptDeptName = l_pResult->GetValue(i, "receipt_dept_name");
				l_CData.m_strReceiptDeptDistrictCode = l_pResult->GetValue(i, "receipt_dept_district_code");

				l_oGetAllBlackListRespond.m_oBody.m_vecData.push_back(l_CData);
			}

			l_oGetAllBlackListRespond.m_oBody.m_strCount = std::to_string(l_iRowNum);
		}
	}

	std::string l_strSyncMessage = l_oGetAllBlackListRespond.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "Send LoadAllBlackList Sync: [%s]", l_strSyncMessage.c_str());
}

void CBusinessImpl::OnNotifiLoadAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive load all black list request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CGetAllBlackListRequest l_oGetAllBlackListRequest;
		if (!l_oGetAllBlackListRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid load all black list request request: [%s]", l_strRequestMsg.c_str());

			return;
		}

		PROTOCOL::CLoadAllBlackListRespond l_oGetAllBlackListRespond;
		l_oGetAllBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_LoadAllBlackListRespond, Queue_CTIControl, SendType_Queue);

		DataBase::SQLRequest m_Query;
		m_Query.sql_id = "select_icc_t_blacklist";
		m_Query.param.insert(std::make_pair("is_delete", "false"));

		//	使用请求、应答模式的数据库链接
		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(m_Query);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Get all blacklist failed, SQL: [%s], Error: [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Get all blacklist success, SQL: [%s]",
				l_pResult->GetSQL().c_str());

			if (l_pResult->Next())
			{
				unsigned int l_iRowNum = l_pResult->RecordSize();
				for (unsigned int i = 0; i < l_iRowNum; i++)
				{
					PROTOCOL::CLoadAllBlackListRespond::CBody::CData l_CData;
					l_CData.m_strLimitType = l_pResult->GetValue(i, "limit_type");
					l_CData.m_strLimitNum = l_pResult->GetValue(i, "limit_num");
					l_CData.m_strLimitMin = l_pResult->GetValue(i, "limit_minute");
					l_CData.m_strLimitReason = l_pResult->GetValue(i, "limit_reason");
					l_CData.m_strStaffCode = l_pResult->GetValue(i, "staff_code");
					l_CData.m_strStaffName = l_pResult->GetValue(i, "staff_name");
					l_CData.m_strBeginTime = l_pResult->GetValue(i, "begin_time");
					l_CData.m_strEndTime = l_pResult->GetValue(i, "end_time");
					l_CData.m_strApprover = l_pResult->GetValue(i, "approver");
					l_CData.m_strApprovedInfo = l_pResult->GetValue(i, "approved_info");
					l_CData.m_strReceiptDeptName = l_pResult->GetValue(i, "receipt_dept_name");
					l_CData.m_strReceiptDeptDistrictCode = l_pResult->GetValue(i, "receipt_dept_district_code");

					l_oGetAllBlackListRespond.m_oBody.m_vecData.push_back(l_CData);
				}

				l_oGetAllBlackListRespond.m_oBody.m_strCount = std::to_string(l_iRowNum);
			}
		}

		std::string l_strMessage = l_oGetAllBlackListRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send LoadAllBlackList Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send LoadAllBlackList Respond Failed:[%s]. Error: NotifiRequestPtr Invalid",
				l_strMessage.c_str());
		}
	}
}
void CBusinessImpl::OnNotifiSetBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive set black list syn: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSetBlackListSync l_oSetBlackListSync;
		if (!l_oSetBlackListSync.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid set black list request sync: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oSetBlackListSync.m_oHeader.m_strMsgid;
		std::string l_strLimitType = l_oSetBlackListSync.m_oBody.m_strLimitType;
		std::string l_strLimitNum = l_oSetBlackListSync.m_oBody.m_strLimitNum;
		std::string l_strLimitMinute = l_oSetBlackListSync.m_oBody.m_strlimitMinute;
		std::string l_strLimitReason = l_oSetBlackListSync.m_oBody.m_strLimitReason;
		std::string l_strStaffCode = l_oSetBlackListSync.m_oBody.m_strStaffCode;
		std::string l_strStaffName = l_oSetBlackListSync.m_oBody.m_strStaffName;
		std::string l_strBeginTime = l_oSetBlackListSync.m_oBody.m_strBeginTime;
		std::string l_strEndTime = l_oSetBlackListSync.m_oBody.m_strEndTime;
		std::string l_strIsUpdate = l_oSetBlackListSync.m_oBody.m_strIsUpdate;
		std::string l_strApprovere = l_oSetBlackListSync.m_oBody.m_strApprover;
		std::string l_strApprovedInfo = l_oSetBlackListSync.m_oBody.m_strApprovedInfo;
		std::string l_strReceiptDeptName = l_oSetBlackListSync.m_oBody.m_strReceiptDeptName;
		std::string l_strReceiptDeptDistrictCode = l_oSetBlackListSync.m_oBody.m_strReceiptDeptDistrictCode;

		DataBase::SQLRequest l_oSetBlackListSql;
		if (l_strIsUpdate.compare(Result_True) == 0)
		{
			//	更新黑名单
			l_oSetBlackListSql.sql_id = "update_icc_t_blacklist";
			l_oSetBlackListSql.param.insert(std::make_pair("limit_num", l_strLimitNum));
			l_oSetBlackListSql.param.insert(std::make_pair("is_delete", "false"));
			l_oSetBlackListSql.set["limit_type"] = l_strLimitType;
			l_oSetBlackListSql.set["limit_minute"] = l_strLimitMinute;
			l_oSetBlackListSql.set["limit_reason"] = l_strLimitReason;
			l_oSetBlackListSql.set["staff_code"] = l_strStaffCode;
			l_oSetBlackListSql.set["staff_name"] = l_strStaffName;
			l_oSetBlackListSql.set["end_time"] = l_strEndTime;
			l_oSetBlackListSql.set["approver"] = l_strApprovere;
			l_oSetBlackListSql.set["approved_info"] = l_strApprovedInfo;
			l_oSetBlackListSql.set["receipt_dept_name"] = l_strReceiptDeptName;
			l_oSetBlackListSql.set["receipt_dept_district_code"] = l_strReceiptDeptDistrictCode;
			l_oSetBlackListSql.set["update_user"] = Server_Name_DBProcess;
			l_oSetBlackListSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		}
		else
		{
			//	新增黑名单
			l_oSetBlackListSql.sql_id = "insert_icc_t_blacklist";
			l_oSetBlackListSql.param.insert(std::make_pair("guid", m_pStringUtil->CreateGuid()));
			l_oSetBlackListSql.param.insert(std::make_pair("limit_type", l_strLimitType));
			l_oSetBlackListSql.param.insert(std::make_pair("limit_num", l_strLimitNum));
			l_oSetBlackListSql.param.insert(std::make_pair("limit_minute", l_strLimitMinute));
			l_oSetBlackListSql.param.insert(std::make_pair("limit_reason", l_strLimitReason));
			l_oSetBlackListSql.param.insert(std::make_pair("staff_code", l_strStaffCode));
			l_oSetBlackListSql.param.insert(std::make_pair("staff_name", l_strStaffName));
			l_oSetBlackListSql.param.insert(std::make_pair("begin_time", l_strBeginTime));
			l_oSetBlackListSql.param.insert(std::make_pair("end_time", l_strEndTime));
			l_oSetBlackListSql.param.insert(std::make_pair("approver", l_strApprovere));
			l_oSetBlackListSql.param.insert(std::make_pair("approved_info", l_strApprovedInfo));
			l_oSetBlackListSql.param.insert(std::make_pair("receipt_dept_name", l_strReceiptDeptName));
			l_oSetBlackListSql.param.insert(std::make_pair("receipt_dept_district_code", l_strReceiptDeptDistrictCode));
			l_oSetBlackListSql.param.insert(std::make_pair("is_delete", "false"));
			l_oSetBlackListSql.param.insert(std::make_pair("create_user", Server_Name_DBProcess));
			l_oSetBlackListSql.param.insert(std::make_pair("create_time", m_pDateTime->CurrentDateTimeStr()));
		}

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSetBlackListSql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Set icc_t_blacklist table failed, SQL: [%s], Error: [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Set icc_t_blacklist table success, SQL: [%s]",
				l_pResult->GetSQL().c_str());
		}
	}
}
void CBusinessImpl::OnNotifiDeleteBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive delete black list syn: %s", l_strRequestMsg.c_str());

		PROTOCOL::CDeleteBlackListSync l_oDeleteBlackListSync;
		if (!l_oDeleteBlackListSync.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid delete black list sync protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strLimitNum = l_oDeleteBlackListSync.m_oBody.m_strLimitNum;

		DataBase::SQLRequest l_oDeleteBlackListSql;
		l_oDeleteBlackListSql.sql_id = "update_icc_t_blacklist";
		l_oDeleteBlackListSql.param.insert(std::make_pair("limit_num", l_strLimitNum));
		l_oDeleteBlackListSql.set["is_delete"] = "true";
		l_oDeleteBlackListSql.set["update_user"] = Server_Name_DBProcess;
		l_oDeleteBlackListSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oDeleteBlackListSql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Delete icc_t_blacklist table failed, SQL: [%s], Error:[%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Delete icc_t_blacklist table success, SQL: %s",
				l_pResult->GetSQL().c_str());
		}
	}
}
void CBusinessImpl::OnNotifiDeleteAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive delete all black list request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSetBlackListRequest l_oRequest;
		if (!l_oRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid delete all black list request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		DataBase::SQLRequest l_oDeleteAllBlackListSql;
		l_oDeleteAllBlackListSql.sql_id = "update_icc_t_blacklist";
		l_oDeleteAllBlackListSql.set["is_delete"] = "true";
		l_oDeleteAllBlackListSql.param.insert(std::pair<std::string, std::string>("is_delete", "false"));

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oDeleteAllBlackListSql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Delete all icc_t_blacklist table failed. SQL: [%s], Error: [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Delete all icc_t_blacklist table success, SQL: [%s]",
				l_pResult->GetSQL().c_str());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//	本插件启动时加载红名单列表
void CBusinessImpl::LoadRedList()
{
	//	加载所有红名单
	DataBase::SQLRequest m_Query;
	m_Query.sql_id = "select_icc_t_redlist";
	m_Query.param.insert(std::pair<std::string, std::string>("is_delete", "false"));

	//	使用主线程的数据库链接
	DataBase::IResultSetPtr l_pResult = m_pInitDBConn->Exec(m_Query);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Load all redlist failed, SQL: [%s], Error: [%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Load all redlist success, SQL: [%s]",
			l_pResult->GetSQL().c_str());

		if (l_pResult->Next())
		{
			std::map<std::string, std::string> l_mapRedList;

			unsigned int l_iRowNum = l_pResult->RecordSize();
			for (unsigned int i = 0; i < l_iRowNum; i++)
			{
				PROTOCOL::CSetRedListRequest l_redListInfo;
				l_redListInfo.m_oBody.m_strId = l_pResult->GetValue(i, "id");
				l_redListInfo.m_oBody.m_strPhoneNum = l_pResult->GetValue(i, "phone_num");
				l_redListInfo.m_oBody.m_strName = l_pResult->GetValue(i, "name");
				l_redListInfo.m_oBody.m_strDepartment = l_pResult->GetValue(i, "department");
				l_redListInfo.m_oBody.m_strPosition = l_pResult->GetValue(i, "position");
				l_redListInfo.m_oBody.m_strRemark = l_pResult->GetValue(i, "remark");

				std::string l_strInfo = l_redListInfo.ToString(m_pJsonFty->CreateJson());
				l_mapRedList[l_redListInfo.m_oBody.m_strPhoneNum] = l_strInfo;
				ICC_LOG_LOWDEBUG(m_pLog, "Load Redlist Info: [%s]", l_strInfo.c_str());
			}

			if (!l_mapRedList.empty())
			{
				m_pRedisClient->HMSet(RED_LIST, l_mapRedList);
			}
		}
	}
}

void CBusinessImpl::SetRedListSync(PROTOCOL::CSetRedListRequest& p_oRedListObj, /*const std::string& p_strGuid,*/ const std::string& p_strSynctype)
{
	//红名单同步 
	PROTOCOL::CSetRedListSync l_oSetRedListSync;
	l_oSetRedListSync.m_oHeader = CreateProtocolHeader(Cmd_SetRedListSync, Topic_RedList, SendType_Topic);
	l_oSetRedListSync.m_oHeader.m_strSystemID = "ICC";
	l_oSetRedListSync.m_oHeader.m_strSubsystemID = Server_Name_DBProcess;
	l_oSetRedListSync.m_oHeader.m_strMsgid = m_pStringUtil->CreateGuid();
	l_oSetRedListSync.m_oHeader.m_strRelatedID = "";
	l_oSetRedListSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oSetRedListSync.m_oHeader.m_strCmd = Cmd_SetRedListSync;
	l_oSetRedListSync.m_oHeader.m_strRequest = Topic_RedList;
	l_oSetRedListSync.m_oHeader.m_strRequestType = SendType_Topic;
	l_oSetRedListSync.m_oHeader.m_strResponse = "";
	l_oSetRedListSync.m_oHeader.m_strResponseType = "";

	//l_oSetRedListSync.m_oBody.m_strId = p_strGuid;
	l_oSetRedListSync.m_oBody.m_strPhoneNum = p_oRedListObj.m_oBody.m_strPhoneNum;
	l_oSetRedListSync.m_oBody.m_strName = p_oRedListObj.m_oBody.m_strName;
	l_oSetRedListSync.m_oBody.m_strDepartment = p_oRedListObj.m_oBody.m_strDepartment;
	l_oSetRedListSync.m_oBody.m_strPosition = p_oRedListObj.m_oBody.m_strPosition;
	l_oSetRedListSync.m_oBody.m_strRemark = p_oRedListObj.m_oBody.m_strRemark;
	l_oSetRedListSync.m_oBody.m_strSyncType = p_strSynctype;

	std::string l_strSyncMessage = l_oSetRedListSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "Send SetRedList Sync: [%s]", l_strSyncMessage.c_str());
}

//	设置红名单
void CBusinessImpl::OnNotifiSetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive set redlist request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSetRedListRequest l_oSetRedListRequest;
		if (!l_oSetRedListRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid set redlist request sync: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oSetRedListRequest.m_oHeader.m_strMsgid;
		std::string l_strGuid = l_oSetRedListRequest.m_oBody.m_strId;
		std::string l_strPhoneNum = l_oSetRedListRequest.m_oBody.m_strPhoneNum;			//电话
		std::string l_strName = l_oSetRedListRequest.m_oBody.m_strName;					//姓名
		std::string l_strDepartment = l_oSetRedListRequest.m_oBody.m_strDepartment;		//部门
		std::string l_strPosition = l_oSetRedListRequest.m_oBody.m_strPosition;			//职务
		std::string l_strRemark = l_oSetRedListRequest.m_oBody.m_strRemark;
		std::string l_strSyncType = SyncType_Add;

		DataBase::SQLRequest l_oSetRedListSql;
		if (!m_pRedisClient->HExists(RED_LIST, l_strPhoneNum))
		{
			std::string l_strGuid = m_pStringUtil->CreateGuid();
			//l_oSetRedListRequest.m_oBody.m_strId = l_strGuid;

			//	红名单不存在，新增
			l_oSetRedListSql.sql_id = "insert_icc_t_redlist";
			l_oSetRedListSql.param.insert(std::make_pair("id", l_strGuid));
			l_oSetRedListSql.param.insert(std::make_pair("phone_num", l_strPhoneNum));
			l_oSetRedListSql.param.insert(std::make_pair("name", l_strName));
			l_oSetRedListSql.param.insert(std::make_pair("department", l_strDepartment));
			l_oSetRedListSql.param.insert(std::make_pair("position", l_strPosition));
			l_oSetRedListSql.param.insert(std::make_pair("remark", l_strRemark));
			l_oSetRedListSql.param.insert(std::make_pair("is_delete", "false"));
			l_oSetRedListSql.param.insert(std::make_pair("create_user", Server_Name_DBProcess));
			l_oSetRedListSql.param.insert(std::make_pair("create_time", m_pDateTime->CurrentDateTimeStr()));
		}
		else
		{
			l_strSyncType = SyncType_Modify;

			//	红名单已存在，更新
			l_oSetRedListSql.sql_id = "update_icc_t_redlist";
			//l_oSetRedListSql.param.insert(std::pair<std::string, std::string>("id", l_strGuid));
			l_oSetRedListSql.param.insert(std::make_pair("phone_num", l_strPhoneNum));
			//l_oSetRedListSql.set["phone_num"] = l_strPhoneNum;
			l_oSetRedListSql.set["name"] = l_strName;
			l_oSetRedListSql.set["department"] = l_strDepartment;
			l_oSetRedListSql.set["position"] = l_strPosition;
			l_oSetRedListSql.set["remark"] = l_strRemark;
			l_oSetRedListSql.set["update_user"] = Server_Name_DBProcess;
			l_oSetRedListSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		}

		bool l_bSqlResult = false;
		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSetRedListSql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Set icc_t_redlist table failed, SQL: [%s], Error: [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			l_bSqlResult = true;
			ICC_LOG_DEBUG(m_pLog, "Set icc_t_redlist table success, SQL: [%s]",
				l_pResult->GetSQL().c_str());

			std::string l_strInfo = l_oSetRedListRequest.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(RED_LIST, l_strPhoneNum, l_strInfo);
			ICC_LOG_LOWDEBUG(m_pLog, "HSet Redlist Info: [%s]", l_strInfo.c_str());
		}

		//	应答
		PROTOCOL::CSetRedListRespond l_oSetRedListRespond;
		l_oSetRedListRespond.m_oHeader.m_strMsgId = l_oSetRedListRequest.m_oHeader.m_strMsgId;
		l_oSetRedListRespond.m_oHeader.m_strResult = l_bSqlResult ? Result_Success : Result_Failed;
		//l_oSetRedListRespond.m_oHeader = CreateProtocolHeader(Cmd_SetRedListRespond, "", SendType_Queue, l_strMsgId);
		//l_oSetRedListRespond.m_oBody.m_strResult = l_bSqlResult ? Result_Success : Result_Failed;
		std::string l_strMessage = l_oSetRedListRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send SetRedList Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send SetRedList Respond Failed: [%s]. Error: ResCallbackPtr Invalid",
				l_strMessage.c_str());
		}

		if (l_bSqlResult)
		{
			SetRedListSync(l_oSetRedListRequest, /*l_strGuid,*/ l_strSyncType);
		}
	}
}
//	删除红名单
void CBusinessImpl::OnNotifiDeleteRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive delete redlist request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CDeleteRedListRequest l_oDeleteRedListRequest;
		if (!l_oDeleteRedListRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid delete redlist request: [%s]", l_strRequestMsg.c_str());

			return;
		}

		/*std::string l_strMsgId = l_oDeleteRedListRequest.m_oHeader.m_strMsgid;
		std::string l_strGuid = l_oDeleteRedListRequest.m_oBody.m_strGuid;	*/		
		std::string l_strPhoneNum = l_oDeleteRedListRequest.m_oBody.m_strPhoneNum;

		bool l_bDelResult = false;
		if (m_pRedisClient->HExists(RED_LIST, l_strPhoneNum))
		{
			//	红名单已存在，删除
			DataBase::SQLRequest l_oDelRedListSql;
			l_oDelRedListSql.sql_id = "update_icc_t_redlist";
			//l_oDelRedListSql.param.insert(std::pair<std::string, std::string>("id", l_strGuid));
			l_oDelRedListSql.param.insert(std::make_pair("phone_num", l_strPhoneNum));
			l_oDelRedListSql.set["is_delete"] = "true";
			l_oDelRedListSql.set["update_user"] = Server_Name_DBProcess;
			l_oDelRedListSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

			DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oDelRedListSql);
			if (!l_pResult->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "Del icc_t_redlist table failed, SQL: [%s], Error: [%s]",
					l_pResult->GetSQL().c_str(),
					l_pResult->GetErrorMsg().c_str());
			}
			else
			{
				l_bDelResult = true;
				ICC_LOG_DEBUG(m_pLog, "Del icc_t_redlist table success, SQL: [%s]",
					l_pResult->GetSQL().c_str());

				//	删除 Redis
				m_pRedisClient->HDel(RED_LIST, l_strPhoneNum);
				ICC_LOG_LOWDEBUG(m_pLog, "HDel Redlist Del Key: [%s]", l_strPhoneNum.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "RedList is not exist key: [%s]",
				l_strPhoneNum.c_str());
		}

		//	应答
		PROTOCOL::CDeleteRedListRespond l_oDelRedListRespond;
		l_oDelRedListRespond.m_oHeader.m_strMsgId = l_oDeleteRedListRequest.m_oHeader.m_strMsgId;
		l_oDelRedListRespond.m_oHeader.m_strResult = l_bDelResult ? Result_Success : Result_Failed;
		//l_oDelRedListRespond.m_oHeader = CreateProtocolHeader(Cmd_DeleteRedListRespond, "", SendType_Queue, l_strMsgId);
		//l_oDelRedListRespond.m_oBody.m_strResult = l_bDelResult ? Result_Success : Result_Failed;
		std::string l_strMessage = l_oDelRedListRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send DelRedList Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send DelRedList Respond Failed: [%s]. Error: ResCallbackPtr Invalid",
				l_strMessage.c_str());
		}

		if (l_bDelResult)
		{
			PROTOCOL::CSetRedListRequest l_redListInfo;
			//l_redListInfo.m_oBody.m_strId = l_strGuid;

			SetRedListSync(l_redListInfo, /*l_strGuid,*/ SyncType_Delete);
		}
	}
}
//	获取所有红名单
void CBusinessImpl::OnNotifiGetRedListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive get all redlist request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CGetAllRedListRequest l_oGetRedListRequest;
		if (!l_oGetRedListRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid get all redlist request: [%s]", l_strRequestMsg.c_str());

			return;
		}

		//	应答
		/*std::string l_strMsgId = l_oGetRedListRequest.m_oHeader.m_strMsgid;
		PROTOCOL::CGetAllRedListRespond l_oGetAllRedListRespond;
		l_oGetAllRedListRespond.m_oHeader = CreateProtocolHeader(Cmd_GetAllRedListRespond, "", SendType_Queue, l_strMsgId);*/
		PROTOCOL::CGetAllRedListRespond l_oGetAllRedListRespond;
		l_oGetAllRedListRespond.m_oHeader.m_strMsgId = l_oGetRedListRequest.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> l_mapRedList;
		if (m_pRedisClient->HGetAll(RED_LIST, l_mapRedList))
		{
			std::vector<std::string> l_vecFields;
			for (auto l_FieldObj : l_mapRedList)
			{
				PROTOCOL::CSetRedListRequest l_oRedListInfo;
				if (l_oRedListInfo.ParseString(l_FieldObj.second, m_pJsonFty->CreateJson()))
				{
					PROTOCOL::CGetAllRedListRespond::CBody::CData l_CData;
					l_CData.m_strId = l_oRedListInfo.m_oBody.m_strId;
					l_CData.m_strPhoneNum = l_oRedListInfo.m_oBody.m_strPhoneNum;
					l_CData.m_strName = l_oRedListInfo.m_oBody.m_strName;
					l_CData.m_strDepartment = l_oRedListInfo.m_oBody.m_strDepartment;
					l_CData.m_strPosition = l_oRedListInfo.m_oBody.m_strPosition;
					l_CData.m_strRemark = l_oRedListInfo.m_oBody.m_strRemark;

					if (l_oGetRedListRequest.m_oBody.m_strPhoneNumber.empty())
					{
						l_oGetAllRedListRespond.m_oBody.m_vecData.push_back(l_CData);
					}
					else
					{
						int l_nIndex = l_CData.m_strPhoneNum.find(l_oGetRedListRequest.m_oBody.m_strPhoneNumber);
						//if (l_oGetRedListRequest.m_oBody.m_strPhoneNumber == l_CData.m_strPhoneNum)
						if (l_nIndex != std::string::npos)
						{
							l_oGetAllRedListRespond.m_oBody.m_vecData.push_back(l_CData);
						}
					}
					
				}
			}

			//l_oGetAllRedListRespond.m_oBody.m_strCount = std::to_string(l_mapRedList.size());
			l_oGetAllRedListRespond.m_oBody.m_strCount = std::to_string(l_oGetAllRedListRespond.m_oBody.m_vecData.size());

		}

		std::string l_strMessage = l_oGetAllRedListRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send Get All RedList Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send Get All RedList Respond Failed: [%s]. Error: ResCallbackPtr Invalid",
				l_strMessage.c_str());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive query call request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CCallEventQueryRequest l_oCallEventQueryRequest;
		if (!l_oCallEventQueryRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid query call request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oCallEventQueryRequest.m_oHeader.m_strMsgId;
		std::string l_strSeatNo = l_oCallEventQueryRequest.m_oBody.m_strSeatNo;
		std::string l_strCallerId = l_oCallEventQueryRequest.m_oBody.m_strCallerId;
		std::string l_strACDCall = l_oCallEventQueryRequest.m_oBody.m_strAcdCall;
		std::string l_strCount = l_oCallEventQueryRequest.m_oBody.m_strCount;

		PROTOCOL::CCallEventQueryRespond l_oCallEventQueryRespond;
		//l_oCallEventQueryRespond.m_oHeader = CreateProtocolHeader(Cmd_CallEventQueryRespond, "", SendType_Queue, l_strMsgId);
		l_oCallEventQueryRespond.m_oHeader.m_strMsgId = l_strMsgId;

		std::string l_strQuerySql = "";
		if (!l_strSeatNo.empty() && l_strACDCall.compare("0") == 0)
		{
			//	查询某一坐席最近 N 条 ACD 话务记录
			l_strQuerySql = "SELECT t.callref_id,t.acd,t.caller_id,t.called_id,t.call_direction,t.incoming_time,t.dial_time,t.talk_time,t.hangup_time,t.hangup_type,t.is_callback"
				" FROM public.icc_t_callevent t"
				" WHERE(t.caller_id = '" + l_strSeatNo + "' OR t.called_id = '" + l_strSeatNo + "')"
				" AND (t.acd <>'' AND t.acd IS NOT NULL)"
				" ORDER BY t.hangup_time DESC"
				" LIMIT  " + l_strCount + "";
		}
		else if (!l_strSeatNo.empty() && l_strACDCall.compare("1") == 0)
		{
			//	查询某一坐席最近 N 条话务记录
			l_strQuerySql = "SELECT t.callref_id,t.acd,t.caller_id,t.called_id,t.call_direction,t.incoming_time,t.dial_time,t.talk_time,t.hangup_time,t.hangup_type,t.is_callback"
				" FROM public.icc_t_callevent t"
				" WHERE (t.caller_id = '" + l_strSeatNo + "' OR t.called_id = '" + l_strSeatNo + "')"
				" ORDER BY t.hangup_time DESC"
				" LIMIT  " + l_strCount + "";
		}
		else if (l_strSeatNo.empty() && l_strACDCall.compare("0") == 0)
		{
			//	查询最近 N 条 ACD 话务记录
			l_strQuerySql = "SELECT t.callref_id,t.acd,t.caller_id,t.called_id,t.call_direction,t.incoming_time,t.dial_time,t.talk_time,t.hangup_time,t.hangup_type,t.is_callback"
				" FROM public.icc_t_callevent t"
				" WHERE (t.acd <>'' AND t.acd IS NOT NULL)"
				" ORDER BY t.hangup_time DESC"
				" LIMIT  " + l_strCount + "";
		}
		else
		{
			//	查询最近 N 条话务记录
			l_strQuerySql = "SELECT t.callref_id,t.acd,t.caller_id,t.called_id,t.call_direction,t.incoming_time,t.dial_time,t.talk_time,t.hangup_time,t.hangup_type,t.is_callback"
				" FROM public.icc_t_callevent t"
				" ORDER BY t.hangup_time DESC"
				" LIMIT  " + l_strCount + "";
		}

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_strQuerySql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Query icc_t_callevent table failed, SQL: [%s], Error : [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			unsigned int l_nRowNum = l_pResult->RecordSize();
			l_oCallEventQueryRespond.m_oBody.m_strCount = std::to_string(l_nRowNum);

			if (l_pResult->Next())
			{
				for (unsigned int i = 0; i < l_nRowNum; i++)
				{
					PROTOCOL::CCallEventQueryRespond::CBody::CData l_CData;
					l_CData.m_strCallrefId = l_pResult->GetValue(i, "callref_id");
					l_CData.m_strAcd = l_pResult->GetValue(i, "acd");
					l_CData.m_strCallerId = l_pResult->GetValue(i, "caller_id");
					l_CData.m_strCalledId = l_pResult->GetValue(i, "called_id");
					l_CData.m_strCallDirection = l_pResult->GetValue(i, "call_direction");
					l_CData.m_strIncomingTime = l_pResult->GetValue(i, "incoming_time");
					l_CData.m_strDialTime = l_pResult->GetValue(i, "dial_time");
					l_CData.m_strTalkTime = l_pResult->GetValue(i, "talk_time");
					l_CData.m_strHangupTime = l_pResult->GetValue(i, "hangup_time");
					l_CData.m_strHangupType = l_pResult->GetValue(i, "hangup_type");

					l_oCallEventQueryRespond.m_oBody.m_vecData.push_back(l_CData);
				}
			}

			ICC_LOG_DEBUG(m_pLog, "Query icc_t_callevent table Success, SQL: [%s], Record Size: [%d]",
				l_pResult->GetSQL().c_str(), l_nRowNum);
		}

		std::string l_strMessage = l_oCallEventQueryRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send QueryCallEvent Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send QueryCallEvent Respond Failed:[%s]. Error: ResCallbackPtr Invalid",
				l_strMessage.c_str());
		}
	}
}

void CBusinessImpl::OnNotifiQueryDetailCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDetailCallQueryRespond::CBody::CData l_CData;
}

void CBusinessImpl::OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive black call request: %s", l_strRequestMsg.c_str());

		PROTOCOL::CBlackCallQueryRequest l_oBlackCallQueryRequest;
		if (!l_oBlackCallQueryRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid black call request protocol: [%s]", l_strRequestMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oBlackCallQueryRequest.m_oHeader.m_strMsgid;
		std::string l_strBeginTime = l_oBlackCallQueryRequest.m_oBody.m_strBeginTime;
		std::string l_strEndTime = l_oBlackCallQueryRequest.m_oBody.m_strEndTime;

		PROTOCOL::CBlackCallQueryRespond l_oBlackCallQueryRespond;
		l_oBlackCallQueryRespond.m_oHeader = CreateProtocolHeader(Cmd_BlackCallQueryRespond, "", SendType_Queue, l_strMsgId);

		DataBase::SQLRequest l_oQueryBlackCallSql;
		l_oQueryBlackCallSql.sql_id = "select_icc_t_callevent_and_blacklist";
		l_oQueryBlackCallSql.param.insert(std::pair<std::string, std::string>("begin_time", l_strBeginTime));
		l_oQueryBlackCallSql.param.insert(std::pair<std::string, std::string>("end_time", l_strEndTime));

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oQueryBlackCallSql);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Query black call failed, SQL: [%s], Error: [%s][",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			unsigned int l_nRowNum = l_pResult->RecordSize();
			l_oBlackCallQueryRespond.m_oBody.m_strCount = std::to_string(l_nRowNum);

			if (l_pResult->Next())
			{
				for (unsigned int i = 0; i < l_nRowNum; i++)
				{
					PROTOCOL::CBlackCallQueryRespond::CBody::CData l_CData;
					l_CData.m_strLimitNum = l_pResult->GetValue(i, "limit_num");
					l_CData.m_strIncomingTime = l_pResult->GetValue(i, "incoming_time");
					l_CData.m_strLimitReason = l_pResult->GetValue(i, "limit_reason");
					l_CData.m_strStaffCode = l_pResult->GetValue(i, "staff_code");

					l_oBlackCallQueryRespond.m_oBody.m_vecData.push_back(l_CData);
				}
			}

			ICC_LOG_DEBUG(m_pLog, "Query black call success, SQL: [%s], Record Size: [%d]",
				l_pResult->GetSQL().c_str(), l_nRowNum);
		}

		std::string l_strMessage = l_oBlackCallQueryRespond.ToString(m_pJsonFty->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_pLog, "Send QueryBlackCall Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Send QueryBlackCall Respond Failed: [%s]. Error: ResCallbackPtr Invalid",
				l_strMessage.c_str());
		}
	}
}

void CBusinessImpl::OnRedoSQLTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::map<std::string, std::string> l_mapValues;

	m_pRedisClient->HGetAll(CTI_REDO_SQL, l_mapValues);
	for (auto l_sqlObj : l_mapValues)
	{
		std::string l_strKey = l_sqlObj.first;
		std::string l_strSQL = l_sqlObj.second;

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_strSQL);
		if (!l_pResult->IsValid())
		{
			std::string l_strErrorMsg = l_pResult->GetErrorMsg();
			ICC_LOG_ERROR(m_pLog, "Key: [%s], Redo SQL: [%s] failed, Error: [%s]",
				l_strKey.c_str(),
				l_strSQL.c_str(),
				l_strErrorMsg.c_str());

			if (l_strErrorMsg.find("no connection to the server") == std::string::npos)
			{
				//	非数据库链接断开导致的 SQL 失败
				m_pRedisClient->HDel(CTI_REDO_SQL, l_strKey);
				ICC_LOG_DEBUG(m_pLog, "Remove Redo SQL, Del Key: [%s]",
					l_strKey.c_str());
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Key: [%s], Redo SQL: [%s] success",
				l_strKey.c_str(),
				l_strSQL.c_str());

			m_pRedisClient->HDel(CTI_REDO_SQL, l_strKey);
			ICC_LOG_DEBUG(m_pLog, "Remove Redo SQL, Del Key: [%s]",
				l_strKey.c_str());
		}
	}
}

void CBusinessImpl::OnRecvAcdAgentStateSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "ttReceive AcdAgentStateSync: %s", l_strRequestMsg.c_str());
		PROTOCOL::CAgentStateEvent l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid AcdAgentStateSync protocol: [%s]", l_strRequestMsg.c_str());
			return;
		}

		CViolationManager::Instance()->SetViolationCheck(1, l_oSyncObj.m_oBody.m_strReadyState, l_oSyncObj.m_oBody.m_strAgent,"", l_oSyncObj.m_oBody.m_strAgent);
	}
}

void CBusinessImpl::OnViolationCheckTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//CViolationManager::Instance()->ViolationCheckTimerProcess();
	if (!m_bViolationIsRunning)
	{
		m_threadViolation.AddMessage(this, ThreadId_Violation);
		ICC_LOG_INFO(m_pLog, "violation is not running, add!!!");
	}
	else
	{
		ICC_LOG_INFO(m_pLog, "violation is running, not add!!!");
	}
}

void CBusinessImpl::OnNotifiGetViolationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive GetViolation Request: %s", l_strRequestMsg.c_str());
		
		PROTOCOL::CGetViolationRequest l_oViolationRequest;
		if (!l_oViolationRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid black call request protocol: [%s]", l_strRequestMsg.c_str());
			return;
		}

		/////////////////////////////////////////////////////////////////////////////////////
		PROTOCOL::CGetViolationRespond l_oGetViolationRespond;

		l_oGetViolationRespond.m_oHeader = l_oViolationRequest.m_oHeader;
		l_oGetViolationRespond.m_oBody.m_strAllCount = "0";
		l_oGetViolationRespond.m_oBody.m_strCount = "0";

		do
		{

			if (l_oViolationRequest.m_oBody.m_strStartTime.empty())
			{
				l_oGetViolationRespond.m_oHeader.m_strResult = "1";
				l_oGetViolationRespond.m_oHeader.m_strMsg = "start time is empty";
				break;
			}


			if (l_oViolationRequest.m_oBody.m_strEndTime.empty())
			{
				l_oGetViolationRespond.m_oHeader.m_strResult = "1";
				l_oGetViolationRespond.m_oHeader.m_strMsg = "end time is empty";
				break;
			}

			if (!CViolationManager::Instance()->GetViolationList(l_oViolationRequest, l_oGetViolationRespond))
			{
				l_oGetViolationRespond.m_oHeader.m_strResult = "2";
			}


		} while (0);

		std::string l_strMessage = l_oGetViolationRespond.ToString(m_pJsonFty->CreateJson());
		
		//ICC_LOG_DEBUG(m_pLog, "Send GetViolation Respond:[%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Send GetViolation Respond Failed:.Error:NotifiRequestPtr Invalid");
	}
}

void CBusinessImpl::OnGetViolationTimeoutCfgRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (!p_pNotifiRequest)
	{
		return;
	}
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive GetViolationTimeout Request: %s", l_strRequestMsg.c_str());

	PROTOCOL::CGetViolationTimeOutRequest l_oViolationTimeoutRequest;
	if (!l_oViolationTimeoutRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid black call request protocol: [%s]", l_strRequestMsg.c_str());
		return;
	}

	PROTOCOL::CGetViolationTimeOutRespond l_oGetViolationTimeoutRespond;
	l_oGetViolationTimeoutRespond.m_oHeader = l_oViolationTimeoutRequest.m_oHeader;
	l_oGetViolationTimeoutRespond.m_oHeader.m_strResult = "0";

	l_oGetViolationTimeoutRespond.m_oBody.m_strCallringTimeout = m_pStringUtil->Format("%d", CViolationManager::Instance()->GetCallRingTimeOutValue());
	l_oGetViolationTimeoutRespond.m_oBody.m_strCalltalkTimeout = m_pStringUtil->Format("%d", CViolationManager::Instance()->GetCallTalkTimeOutValue());
	l_oGetViolationTimeoutRespond.m_oBody.m_strBusyStateTimeout = m_pStringUtil->Format("%d", CViolationManager::Instance()->GetBusyTimeOutValue());
	std::string l_strAlarmTimeout = "300";
	m_pRedisClient->HGet("Violation_TimeoutValue", "AlarmTimeout", l_strAlarmTimeout);
	l_oGetViolationTimeoutRespond.m_oBody.m_strAlarmTimeout = l_strAlarmTimeout;

	std::string l_strMessage = l_oGetViolationTimeoutRespond.ToString(m_pJsonFty->CreateJson());

	ICC_LOG_DEBUG(m_pLog, "Send GetViolationTimeout Respond:[%s]", l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);

}


std::string CBusinessImpl::_QueryAcdDept(const std::string& strAcd)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_acd_dept_from_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = strAcd;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return "";
	}

	if (l_Result->Next())
	{
		return l_Result->GetValue("dept_code");
	}

	return "";
}

bool CBusinessImpl::_QueryDeptNamePath(const std::string& strDeptCode, std::string& strDeptName, std::string& strDeptPath, std::string& strDeptIdentify)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_name_path_from_icc_t_dept";
	l_SQLRequest.param["dept_code"] = strDeptCode;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		strDeptName = l_Result->GetValue("shortcut");
		strDeptPath = l_Result->GetValue("path");
		strDeptIdentify = l_Result->GetValue("pucorgidentifier");
	}

	return true;
}

std::string CBusinessImpl::_QueryDeptByNumber(const std::string& strNumber, const std::string& strNumberType)
{
	DataBase::SQLRequest l_SQLRequest;
	if (strNumberType == NumberType_ReceiveSeat)
	{
		l_SQLRequest.sql_id = "query_number_dept_from_icc_t_seat";
		l_SQLRequest.param["number"] = strNumber;
	}
	else if (strNumberType == NumberType_ProcessSeat)
	{
		l_SQLRequest.sql_id = "select_icc_t_process_seat_telephones_by_phone";
		l_SQLRequest.param["telephone"] = strNumber;
	}	
	else
	{
		return "";
	}

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return "";
	}

	if (l_Result->Next())
	{
		return l_Result->GetValue("dept_code");
	}

	return "";
}

std::string CBusinessImpl::_QueryNumberType(const std::string& strNumber)
{	
	ICC_LOG_DEBUG(m_pLog, "_QueryNumberType enter, number:[%s]", strNumber.c_str());

	std::string strResult = "99";
	do 
	{
		if (strNumber.empty())
		{
			ICC_LOG_ERROR(m_pLog, "number is empty:[%s]", strNumber.c_str());
			break;
		}

		std::string strCount;
		bool bQuery = _QueryAgentNumber(strNumber, strCount);
		if (!bQuery)
		{
			ICC_LOG_ERROR(m_pLog, "query is agent number failed:[%s]", strNumber.c_str());
			break;
		}

		if (strCount != "0")
		{
			strResult = "2";
			break;
		}

		bQuery = _QueryProcessNumber(strNumber, strCount);
		if (!bQuery)
		{
			ICC_LOG_ERROR(m_pLog, "query is process number failed:[%s]", strNumber.c_str());
			break;
		}

		if (strCount != "0")
		{
			strResult = "3";
			break;
		}

		strResult = "1";

	} while (false);	

	ICC_LOG_DEBUG(m_pLog, "_QueryNumberType complete, number:[%s], type:[%s]", strNumber.c_str(), strResult.c_str());

	return strResult;	
}

bool CBusinessImpl::_QueryAgentNumber(const std::string& strNumber, std::string& strCount)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_count_icc_t_seat_by_phone";
	l_SQLRequest.param["phone"] = strNumber;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_count_icc_t_seat_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		strCount = l_Result->GetValue("acount");		
	}
	return true;
}

bool CBusinessImpl::_QueryProcessNumber(const std::string& strNumber, std::string& strCount)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_dept_process_seat_by_phone";
	l_SQLRequest.param["phone"] = strNumber;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		strCount = l_Result->GetValue("acount");		
	}
	return true;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	CViolationManager::Instance()->SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::OnReceiveAlarmRelationRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (!p_pNotifiRequest)
	{
		ICC_LOG_ERROR(m_pLog, "Receive AlarmRelation state failed");
		return;
	}

	_GetThread(0).AddMessage(this, ThreadId_AlarmRelation, 0, 0, 0, "", "", p_pNotifiRequest);
}

void CBusinessImpl::OnReceiveQueryProcessCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (!p_pNotifiRequest)
	{
		ICC_LOG_ERROR(m_pLog, "Receive QueryProcessCall state failed");
		return;
	}
	
	_GetThread(0).AddMessage(this, ThreadId_QueryCallInfo, 0, 0, 0, "", "", p_pNotifiRequest);
}

/*****************************************************************
 * 
 ****************************************************************/
#define CLIENT_REGISTER_INFO "ClientRegisterInfo"
bool CBusinessImpl::_QueryReveiverInfo(const std::string& strSeatNumber, std::string& strStaffCode, std::string& strStaffName)
{
	ICC_LOG_DEBUG(m_pLog, "_QueryReveiverInfo enter， seat:[%s]", strSeatNumber.c_str());

	std::string strClientInfo;
	if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO, strSeatNumber, strClientInfo))
	{
		ICC_LOG_ERROR(m_pLog, "_QueryReveiverInfo get json failed! seat:[%s]", strSeatNumber.c_str());
		return false;
	}

	JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
	if (nullptr == pJson)
	{
		ICC_LOG_ERROR(m_pLog, "_QueryReveiverInfo create json failed!, seat:[%s]", strSeatNumber.c_str());
		return false;
	}
	if (!pJson->LoadJson(strClientInfo))
	{
		ICC_LOG_ERROR(m_pLog, "_QueryReveiverInfo parse json failed!, seat:[%s]", strSeatNumber.c_str());
		return false;
	}
	
	strStaffCode = pJson->GetNodeValue("/staff_code", "");
	strStaffName = pJson->GetNodeValue("/staff_name", "");

	ICC_LOG_DEBUG(m_pLog, "_QueryReveiverInfo complete， seat:[%s], code:[%s], name:[%s]", strSeatNumber.c_str(), strStaffCode.c_str(), strStaffName.c_str());
	
	return true;
}


/*****************************************************************
 * 
 ****************************************************************/
CCommonWorkThread& CBusinessImpl::_GetThread(int iCallId)
{
	int iIndex = 0;
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uIndex >= 0x0fffffff)
		{
			m_uIndex = 0;
		}
		m_uIndex++;
		iIndex = m_uIndex & (THREADCOUNT - 1);
	}

	return m_threadProc[iIndex];
}

bool CBusinessImpl::_IsProcessSeat(const std::string& strCalled, std::string& strDeptCode)
{	
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_process_seat_telephones_by_phone";
	l_SQLRequest.param["telephone"] = strCalled;

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		strDeptCode = l_Result->GetValue("dept_code");
	}

	ICC_LOG_DEBUG(m_pLog, "sql:[%s], dept:[%s]", l_Result->GetSQL().c_str(), strDeptCode.c_str());

	return true;
}

void CBusinessImpl::_RealDeviceState(const std::string& strCallrefId, std::string& strState)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_sub_callevent_by_callrefid_and_state";
	l_SQLRequest.param["callref_id"] = strCallrefId;

	std::string strTime = m_pDateTime->GetCallRefIdTime(strCallrefId);
	if (strTime != "")
	{
		l_SQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return ;
	}

	std::string strCount;
	if (l_Result->Next())
	{
		strCount = l_Result->GetValue("count");
		if (strCount == "0")
		{
			strState = "release";
		}
	}

	ICC_LOG_DEBUG(m_pLog, "count:[%s]", strCount.c_str());
}

void CBusinessImpl::_SyncDeviceState(PROTOCOL::IRespondPtr pResponse, const std::string& strDeptCode)
{
	boost::shared_ptr< PROTOCOL::CDeviceStateEvent> pState = boost::dynamic_pointer_cast<PROTOCOL::CDeviceStateEvent>(pResponse);
	if (pState == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "CDeviceStateEvent parse is failed!!!");
		return;
	}

	PROTOCOL::CDeviceStateToVcs deviceSync;

	deviceSync.m_oHeader.m_strSystemID = "ICC";
	deviceSync.m_oHeader.m_strSubsystemID = Server_Name_CTIServer;	
	deviceSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	deviceSync.m_oHeader.m_strCmd = "call_to_vcs_sync";
	deviceSync.m_oHeader.m_strRequest = "topic_call_to_vcs_sync";
	deviceSync.m_oHeader.m_strRequestType = "1";
	deviceSync.m_oHeader.m_strResponse = "";
	deviceSync.m_oHeader.m_strResponseType = "";	
	deviceSync.m_oHeader.m_strMsgid = m_pStringUtil->CreateGuid();

	deviceSync.m_oBody.m_strCallRefId = pState->m_oBody.m_strCallRefId;
	deviceSync.m_oBody.m_strCaller = pState->m_oBody.m_strCallerId;
	deviceSync.m_oBody.m_strCalled = pState->m_oBody.m_strCalledId;
	deviceSync.m_oBody.m_strOrgCalled = pState->m_oBody.m_strOriginalCalledId;
	deviceSync.m_oBody.m_strCalledDeptCode = strDeptCode;
	deviceSync.m_oBody.m_strState = pState->m_oBody.m_strState;
	if (deviceSync.m_oBody.m_strState == "hangupstate")
	{
		_RealDeviceState(pState->m_oBody.m_strCallRefId, deviceSync.m_oBody.m_strState);
	}
	deviceSync.m_oBody.m_strTime = pState->m_oBody.m_strTime;

	std::string strMessage = deviceSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strMessage));

	ICC_LOG_ERROR(m_pLog, "send device state to vcs [%s]", strMessage.c_str());
}

void CBusinessImpl::_ProcDeviceStateChanged(PROTOCOL::IRespondPtr pResponse)
{
	boost::shared_ptr< PROTOCOL::CDeviceStateEvent> pState = boost::dynamic_pointer_cast<PROTOCOL::CDeviceStateEvent>(pResponse);
	if (pState == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "CDeviceStateEvent parse is failed!!!");
		return;
	}	

	if (pState->m_oBody.m_strCalledId.empty())
	{
		ICC_LOG_INFO(m_pLog, "_ProcDeviceStateChanged called is empty, return!!!");
		return;
	}

	std::string strDeptCode;
	if (!_IsProcessSeat(pState->m_oBody.m_strCalledId, strDeptCode) )
	{
		ICC_LOG_ERROR(m_pLog, "query process seat is failed!!!");
		return;
	}

	if (strDeptCode.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "called is not process seat!!!");
		return;
	}

	std::string strCallerType = _QueryNumberType(pState->m_oBody.m_strCallerId);
	if (strCallerType != NumberType_OutLine)
	{
		ICC_LOG_DEBUG(m_pLog, "caller is not out line, not send to process seat!!!");
		return;
	}	

	_SyncDeviceState(pState, strDeptCode);
}

bool CBusinessImpl::_CheckParam(const PROTOCOL::CVcsAlarmRelationRequest& request,std::string& strError)
{
	if (request.m_oBody.m_strCallRefId.empty())
	{
		strError = "input callref id is empty!!";
		return false;
	}

	/*if (request.m_oBody.m_strAcceptState.empty() && request.m_oBody.m_strRelAlarmId.empty())
	{
		strError = "accept state and relation alarm id all empty!!";
		return false;
	}*/

	return true;
}

bool CBusinessImpl::_AlarmRelation(const PROTOCOL::CVcsAlarmRelationRequest& request, std::string& strError)
{
	std::string strCondition;
	strCondition = m_pStringUtil->Format(" rel_alarm_id = '%s' ", request.m_oBody.m_strRelAlarmId.c_str());

	if (!request.m_oBody.m_strAcceptState.empty())
	{
		strCondition += " ,";
		strCondition += m_pStringUtil->Format(" accept_state = '%s' ", request.m_oBody.m_strAcceptState.c_str());		
	}

	if (!request.m_oBody.m_strRelType.empty())
	{
		strCondition += " ,";
		strCondition += m_pStringUtil->Format(" rel_alarm_type = '%s' ", request.m_oBody.m_strRelType.c_str());
	}

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_icc_t_callevent_alarm_relation";
	l_SQLRequest.param["condition"] = strCondition;
	l_SQLRequest.param["callref_id"] = request.m_oBody.m_strCallRefId;
	std::string strTime = m_pDateTime->GetCallRefIdTime(request.m_oBody.m_strCallRefId);
	if (strTime != "")
	{
		l_SQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		strError = "exec sql failed!";
		return false;
	}

	return true;
}

void CBusinessImpl::_ProcAlarmRelation(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	std::string strMessage = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "proc alarm relation begin: %s", strMessage.c_str());

	PROTOCOL::CVcsAlarmRelationRequest request;
	if (!request.ParseString(strMessage, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse alarm relation json failed, message:[%s]", strMessage.c_str());
		return;
	}

	bool bResult = true;
	std::string strErrMessage = "success";
	do 
	{
		if (!_CheckParam(request, strErrMessage))
		{
			bResult = false;
			break;
		}

		if (!_AlarmRelation(request, strErrMessage))
		{
			bResult = false;
			break;
		}

	} while (false);
	
	PROTOCOL::CSetCallInfoRespond l_oRespond;
	l_oRespond.m_strMesID = request.m_oBody.m_strMessageId;
	if (!bResult)
	{
		ICC_LOG_ERROR(m_pLog, "Insert icc_t_callevent table failed, Error");
		l_oRespond.m_strCode = "1";
		l_oRespond.m_strMessage = strErrMessage;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_callevent table success,");
		//回复消息
		l_oRespond.m_strCode = "200";
		l_oRespond.m_strMessage = "success";
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiRequest->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

}

bool CBusinessImpl::_CheckQueryCallParam(const PROTOCOL::CVcsQueryCallInfoRequest& request, std::string& strError)
{
	if (request.m_oBody.m_strBeginTime.empty())
	{
		strError = "query begin time is empty";
		return false;
	}

	if (request.m_oBody.m_strEndTime.empty())
	{
		strError = "query end time is empty";
		return false;
	}

	if (request.m_oBody.m_strDeptCode.empty())
	{
		strError = "dept code is empty";
		return false;
	}

	if (request.m_oBody.m_strIsNeedPaging == "1")
	{
		int l_iPageSize = m_pStringUtil->ToInt(request.m_oBody.m_strPageSize);
		int l_iPageIndex = m_pStringUtil->ToInt(request.m_oBody.m_strPageIndex);

		if (l_iPageSize <= 0 || l_iPageSize > 1000)
		{//检查客户端传入的page_size参数
			strError = "page size is error, valid value:[1:1000]";		
			return false;
		}

		if (l_iPageIndex < 1)
		{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
			strError = "page index is error";
			return false;
		}
	}

	return true;
}

std::string CBusinessImpl::_BuildQueryCallInfoCondition(const PROTOCOL::CVcsQueryCallInfoRequest& request, std::string& strUnNeedPaging)
{
	std::string strCondition = "  ";
	//部门
	if (request.m_oBody.m_strIsRecursive == "1")
	{
		// strCondition = m_pStringUtil->Format(" and called_relation_dept like '%%s%' ", request.m_oBody.m_strDeptCode.c_str());
		strCondition = " and called_relation_dept_path like '%" + request.m_oBody.m_strDeptCode + "%'";
	}
	else
	{
		// strCondition = m_pStringUtil->Format(" and called_relation_dept = '%%s%' ", request.m_oBody.m_strDeptCode.c_str());
		strCondition = " and called_relation_dept = '" + request.m_oBody.m_strDeptCode + "'";
	}

	//查询号码
	if (!request.m_oBody.m_strCallNumber.empty() && !request.m_oBody.m_strCallType.empty())
	{
		if (request.m_oBody.m_strCallType == "1")
		{
			strCondition += " and called_id like '%" + request.m_oBody.m_strCallNumber + "%'";
		}
		else if (request.m_oBody.m_strCallType == "2")
		{
			strCondition += " and caller_id like '%" + request.m_oBody.m_strCallNumber + "%'";
		}
		else if (request.m_oBody.m_strCallType == "3")
		{
			strCondition += " and (caller_id like '%" + request.m_oBody.m_strCallNumber + "%'" + " or called_id like '%" + request.m_oBody.m_strCallNumber + "%')";
		}
	}
	if (!request.m_oBody.m_strCallType.empty()&& request.m_oBody.m_strCallNumber.empty())
	{
		if (request.m_oBody.m_strCallType == "1")
		{
			strCondition += " and call_direction = 'in'";
		}
		else if (request.m_oBody.m_strCallType == "2")
		{
			strCondition += " and call_direction = 'out'";
		}
		else if (request.m_oBody.m_strCallType == "3")
		{
			//当CallType为3时，查所有方向录音，不给条件限制
		}
	}

	if (!request.m_oBody.m_strAlarmId.empty())
	{
		// strCondition += m_pStringUtil->Format(" and rel_alarm_id = '%s' ", request.m_oBody.m_strAlarmId.c_str());
		strCondition = " and rel_alarm_id like '%" + request.m_oBody.m_strAlarmId + "%'";
	}

	if (!request.m_oBody.m_strAccpectState.empty())
	{
		strCondition += m_pStringUtil->Format(" and accept_state = '%s' ", request.m_oBody.m_strAccpectState.c_str());
	}

	if (!request.m_oBody.m_strTalkingTimeMin.empty())
	{
		strCondition += m_pStringUtil->Format(" and (hangup_time - ring_time) >= '%s minute' ", request.m_oBody.m_strTalkingTimeMin.c_str());
	}

	if (!request.m_oBody.m_strTalkingTimeMax.empty())
	{
		strCondition += m_pStringUtil->Format(" and (hangup_time - ring_time) <= '%s minute' ", request.m_oBody.m_strTalkingTimeMax.c_str());
	}
	std::string l_autoUrgeCallerNum = "";
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum) && !l_autoUrgeCallerNum.empty())
	{
		strCondition += m_pStringUtil->Format(" and caller_id != '%s' ", l_autoUrgeCallerNum.c_str());
	}
	strCondition += " order by hangup_time desc ";
	strUnNeedPaging = strCondition;
	//分页
	if (request.m_oBody.m_strIsNeedPaging == "1")
	{
		int l_iPageSize = m_pStringUtil->ToInt(request.m_oBody.m_strPageSize);
		int l_iPageIndex = m_pStringUtil->ToInt(request.m_oBody.m_strPageIndex);

		unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
		unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始

		strCondition += m_pStringUtil->Format(" limit %d offset %d ", l_iPageSize, l_iRequestIdxBegin);
	}

	return strCondition;
}

bool CBusinessImpl::_QueryCallInfo(const PROTOCOL::CVcsQueryCallInfoRequest& request, PROTOCOL::CVcsQueryCallInfoRespond& respond, std::string& strError)
{
	// 记录不分页的条件
	std::string strUnNeedPaging = "";
	std::string strCondition = _BuildQueryCallInfoCondition(request, strUnNeedPaging);

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_icc_t_callevent_by_process_info";
	l_SQLRequest.param["begin_time"] = request.m_oBody.m_strBeginTime;
	l_SQLRequest.param["end_time"] = request.m_oBody.m_strEndTime;
	l_SQLRequest.param["condition"] = strCondition;	

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		strError = "exec sql failed!";
		return false;
	}

	int iCount = 0;
	std::vector<std::string> vecCloumns = l_Result->GetFieldNames();
	unsigned int iCloumnsCount = vecCloumns.size();
	ICC_LOG_DEBUG(m_pLog, "iCloumnsCount:[%d]", iCloumnsCount);
	while (l_Result->Next())
	{
		if (iCount > 1000)
		{
			ICC_LOG_WARNING(m_pLog, "result is more than 1000, will break!!");
			break;
		}
		std::map<std::string, std::string> mapTmps;
		for (unsigned int i = 0; i < iCloumnsCount; ++i)
		{
			// ICC_LOG_DEBUG(m_pLog, "respond [%d]:[%s]-[%s]", i, vecCloumns[i].c_str(), l_Result->GetValue(vecCloumns[i]).c_str());
			mapTmps.insert(std::make_pair(vecCloumns[i], l_Result->GetValue(vecCloumns[i])));
		}

		respond.m_vecDatas.push_back(mapTmps);
	}
	if (request.m_oBody.m_strIsNeedPaging == "1")
	{
		DataBase::SQLRequest t_SQLRequest;
		t_SQLRequest.sql_id = "query_icc_t_callevent_by_process_info";
		t_SQLRequest.param["begin_time"] = request.m_oBody.m_strBeginTime;
		t_SQLRequest.param["end_time"] = request.m_oBody.m_strEndTime;
		t_SQLRequest.param["condition"] = strUnNeedPaging;

		DataBase::IResultSetPtr t_Result = m_pRequestDBConn->Exec(t_SQLRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", t_Result->GetSQL().c_str());

		if (!t_Result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "query_icc_t_callevent_by_process_info failed,error msg:[%s]", t_Result->GetErrorMsg().c_str());
			strError = "exec sql failed!";
			return false;
		}
		respond.m_strTotalCount = std::to_string(t_Result->RecordSize());
	}
	else {
		respond.m_strTotalCount = std::to_string(respond.m_vecDatas.size());
	}
	return true;
}

void CBusinessImpl::_ProcQueryCallInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string strMessage = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "proc alarm relation begin: %s", strMessage.c_str());

	PROTOCOL::CVcsQueryCallInfoRequest request;
	if (!request.ParseString(strMessage, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse query call info json failed, message:[%s]", strMessage.c_str());
		return;
	}

	bool bResult = true;
	PROTOCOL::CVcsQueryCallInfoRespond respond;
	respond.m_strResultMessage = "success";
	do
	{
		if (!_CheckQueryCallParam(request, respond.m_strResultMessage))
		{
			bResult = false;
			break;
		}

		
		if (!_QueryCallInfo(request, respond, respond.m_strResultMessage))
		{
			bResult = false;
			break;
		}

	} while (false);

	if (bResult)
	{
		respond.m_strResultCode = "200";
	}
	else
	{
		respond.m_strResultCode = "1";
	}

	std::string tmp_strMsg(respond.ToString(m_pJsonFty->CreateJson()));
	p_pNotifiRequest->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send alarm relation response message:[%s]", tmp_strMsg.c_str());
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case ThreadId_DeviceStateChanged:
	{
		try
		{
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_DeviceStateChanged begin!");
			_ProcDeviceStateChanged(msg_data.pRespond);
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_DeviceStateChanged end!");
		}		
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "_ProcDeviceStateChanged exception!");
		}		
	}
	break;
	case ThreadId_AlarmRelation:		// 话务设置
	{
		try
		{
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_AlarmRelation begin!");
			_ProcAlarmRelation(msg_data.pTask);
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_AlarmRelation end!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "_ProcAlarmRelation exception!");
		}
		
	}
	break;
	case ThreadId_QueryCallInfo:	// 处理话务查询
	{
		try
		{
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_QueryCallInfo begin!");
			_ProcQueryCallInfo(msg_data.pTask);
			ICC_LOG_LOWDEBUG(m_pLog, "ThreadId_QueryCallInfo end!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "_ProcQueryCallInfo exception!");
		}
		
	}
	break;
	case THREADID_RECEIVENOTIFY:
	{
		try
		{
			ICC_LOG_LOWDEBUG(m_pLog, "THREADID_RECEIVENOTIFY begin! cmd:%s", msg_data.str_msg.c_str());
			_DispatchNotify(msg_data.pTask, msg_data.str_msg);
			ICC_LOG_LOWDEBUG(m_pLog, "THREADID_RECEIVENOTIFY end! cmd:%s", msg_data.str_msg.c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception! cmd:%s", msg_data.str_msg.c_str());
		}
	}
	break;
	case ThreadId_Violation:
	{
		try
		{
			m_bViolationIsRunning = true;
			ICC_LOG_DEBUG(m_pLog, "ThreadId_Violation begin!");
			CViolationManager::Instance()->ViolationCheckTimerProcess();
			ICC_LOG_DEBUG(m_pLog, "ThreadId_Violation end!");
			m_bViolationIsRunning = false;
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "ThreadId_Violation exception!");
		}
	}
	break;
	default:
		break;
	}
}

bool CBusinessImpl::_UpdateProcessSeatReleaseAcceptState(const std::string& strCallrefId, const std::string& strAcceptState)
{
	std::string strCondition;
	strCondition = m_pStringUtil->Format(" accept_state = '%s' ", strAcceptState.c_str());

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_icc_t_callevent_alarm_relation";
	l_SQLRequest.param["condition"] = strCondition;
	l_SQLRequest.param["callref_id"] = strCallrefId;

	std::string strTime = m_pDateTime->GetCallRefIdTime(strCallrefId);
	if (strTime != "")
	{
		l_SQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_Result = m_pRequestDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_callevent_alarm_relation failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());		
		return false;
	}

	return true;
}

/*****************************************************************
 *
 ****************************************************************/
void CBusinessImpl::_CreateThreads()
{
	Config::IConfigPtr l_pCfgReader = ICCGetIConfigFactory()->CreateConfig();
	if (nullptr == l_pCfgReader)
	{
		ICC_LOG_ERROR(m_pLog, "dbagent create config failed!!!");
		return;
	}
	m_strDispatchMode = l_pCfgReader->GetValue("ICC/Plugin/DBProcess/procthreaddispatchmode", "1");
	m_uProcThreadCount = m_pStringUtil->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/DBProcess/procthreadcount", "4"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (m_uProcThreadCount > uConCurrency)
	{
		m_uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "dbagent proc thread count:%d", m_uProcThreadCount);

	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			m_vecProcThreads.push_back(pThread);
		}
	}

	ICC_LOG_INFO(m_pLog, "dbagent real proc thread count:%d", m_vecProcThreads.size());
}

void CBusinessImpl::_DestoryThreads()
{
	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		m_vecProcThreads[i]->Stop(0);
	}
	m_vecProcThreads.clear();
}


CommonWorkThreadPtr CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	if (m_strDispatchMode == "1")
	{
		std::lock_guard<std::mutex> lock(m_mutexDispatchThread);
		if (m_uCurrentThreadIndex >= m_uProcThreadCount)
		{
			m_uCurrentThreadIndex = 0;
		}
		iIndex = m_uCurrentThreadIndex;
		m_uCurrentThreadIndex++;
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_mutexDispatchThread);
		int iMessageCount = 0;
		for (int i = 0; i < m_uProcThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}

	return m_vecProcThreads[iIndex];
}

void CBusinessImpl::_OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	CommonWorkThreadPtr pThread = _GetThread();
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVENOTIFY, 0, 0, 0, p_pNotifiReqeust->GetCmdName(), "", p_pNotifiReqeust);
	}
}

void CBusinessImpl::_DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName)
{
	std::map<std::string, ProcNotify>::const_iterator itr;
	itr = m_mapFuncs.find(strCmdName);
	if (itr != m_mapFuncs.end())
	{
		(this->*itr->second)(p_pNotifiReqeust);
	}
}

//void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
//{
//	switch (msg_data.msg_id)
//	{
//	case THREADID_RECEIVENOTIFY:
//	{
//		try
//		{
//			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY begin!");
//			_DispatchNotify(msg_data.pTask, msg_data.str_msg);
//			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY end!");
//		}
//		catch (...)
//		{
//			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception!");
//		}
//	}
//	break;
//
//	default:
//		break;
//	}
//}

void CBusinessImpl::BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value)
{
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, l_manualAcceptType, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
		return;
	}
	l_value = l_ParamInfo.m_strValue;
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::ReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive record file up sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CRecordFileUpSyncRequest l_oRecordFileUpSyncRequest;
		if (!l_oRecordFileUpSyncRequest.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid record file up sync protocol: [%s]", l_strRequestMsg.c_str());
			return;
		}
		if (l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime.empty())
		{
			l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime = m_pDateTime->CurrentDateTimeStr();
		}
		std::string strRecordID;
		std::string strEndRecordFileId;
		if (!SearchRecordID(l_oRecordFileUpSyncRequest, strRecordID))
		{
			ICC_LOG_DEBUG(m_pLog, "Build Record ID Failed");
			return ;
		}
		DataBase::SQLRequest l_oSQLRequest;
		PROTOCOL::CRecordIDToDBRequest l_oRecordIDToDBRequest;
		PROTOCOL::CRecordIDToDBRequest::CData data;
		if (!strRecordID.empty())
		{
			if (!l_oRecordFileUpSyncRequest.m_oBody.m_strFileID.empty()&&!l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime.empty())
			{
				if(!l_oRecordIDToDBRequest.ParseString(strRecordID, m_pJsonFty->CreateJson()))
				{
					ICC_LOG_DEBUG(m_pLog, "parse history record failed");
				}
				data.m_strFileID = l_oRecordFileUpSyncRequest.m_oBody.m_strFileID;
				data.m_strStartTime = l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime;
				l_oRecordIDToDBRequest.m_oBody.m_vecData.push_back(data);
				strEndRecordFileId = l_oRecordIDToDBRequest.ToString(m_pJsonFty->CreateJson());
			}
		}
		else
		{
			if (!l_oRecordFileUpSyncRequest.m_oBody.m_strFileID.empty() && !l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime.empty())
			{
				data.m_strFileID = l_oRecordFileUpSyncRequest.m_oBody.m_strFileID;
				data.m_strStartTime = l_oRecordFileUpSyncRequest.m_oBody.m_strStartTime;
				l_oRecordIDToDBRequest.m_oBody.m_vecData.push_back(data);
			}
			strEndRecordFileId = l_oRecordIDToDBRequest.ToString(m_pJsonFty->CreateJson());
		}
		if(!strEndRecordFileId.empty())
		{
			if (!_UpdateCalleventWithRecord(strEndRecordFileId, l_oRecordFileUpSyncRequest.m_oBody.m_strCallrefID))
			{
				ICC_LOG_DEBUG(m_pLog, "update icc_t_callevent table with record failed,callrefID:[%s],recordID:[%s]",
					l_oRecordFileUpSyncRequest.m_oBody.m_strCallrefID.c_str(), strEndRecordFileId.c_str());
				return;
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "strEndRecordFileId is Empty!!!");
		}
		ICC_LOG_DEBUG(m_pLog, "Receive Record FileUp Request success!!!");
		return;
	}
}

bool CBusinessImpl::SearchRecordID(PROTOCOL::CRecordFileUpSyncRequest l_oRecordFileUpSyncRequest,std::string &strRecordID)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "select_icc_t_callevent";
	l_oSQLRequest.param["callref_id"] = l_oRecordFileUpSyncRequest.m_oBody.m_strCallrefID;
	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select icc_t_callevent table failed, SQL: [%s], Error:[%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		strRecordID = l_pResult->GetValue("record_file_id");
	}
	return true;
}

bool CBusinessImpl::_UpdateCalleventWithRecord(const std::string& strRecordFileID, const std::string& strCallrefID)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "update_icc_t_callevent";
	l_oSQLRequest.set["record_file_id"] = strRecordFileID;
	l_oSQLRequest.param["callref_id"] = strCallrefID;

	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update icc_t_callevent table failed, SQL: [%s], Error:[%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "update icc_t_callevent table success, SQL: %s",
			l_pResult->GetSQL().c_str());
		return true;
	}
	return true;
}

void CBusinessImpl::ReceiveHistoryCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_LOWDEBUG(m_pLog, "Receive history call sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CGetHistoryCallListRequest l_oGetHistoryCallReq;
		if (!l_oGetHistoryCallReq.ParseExString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_DEBUG(m_pLog, "Parse history call sync failed");
			return;
		}
		for (int i = 0; i < stoi(l_oGetHistoryCallReq.m_oBody.m_strCount); i++)
		{
			bool bExist=true;
			if (!_IsExistsCallrefID(l_oGetHistoryCallReq.m_oBody.m_vecData[i].m_strCallRefID, bExist))
			{
				ICC_LOG_ERROR(m_pLog, "Judge callrefID exists failed");
				continue;
			}
			if (bExist == false)
			{
				if (!_InsertCalleventWithHistoryCall(l_oGetHistoryCallReq, i))
				{
					ICC_LOG_DEBUG(m_pLog, "insert icc_t_callevent failed,callref_id:[%s]",
						l_oGetHistoryCallReq.m_oBody.m_vecData[i].m_strCallRefID.c_str());
					continue;
				}
			}
			else if (bExist == true)
			{
				bool is_NeedUpdate = true;
				//如果有挂断时间则说明数据库里存在该完整的话务
				if (!_IsNeedUpdateCallevent(l_oGetHistoryCallReq.m_oBody.m_vecData[i].m_strCallRefID, is_NeedUpdate))
				{
					ICC_LOG_ERROR(m_pLog, "Judge callrefID is need update failed,callref_id:[%s]",
						l_oGetHistoryCallReq.m_oBody.m_vecData[i].m_strCallRefID.c_str());
					continue;
				}
				if (is_NeedUpdate == true)
				{
					if (!_UpdateCalleventWithHistoryCall(l_oGetHistoryCallReq, i))
					{
						ICC_LOG_DEBUG(m_pLog, "update icc_t__callevent failed,callref_id:[%s]",
							l_oGetHistoryCallReq.m_oBody.m_vecData[i].m_strCallRefID.c_str());
						continue;
					}
				}
			}
		}
		ICC_LOG_DEBUG(m_pLog, "Receive Record FileUp Request success!!!");
		return;
	}
}

bool CBusinessImpl::_IsExistsCallrefID(const std::string& strCallRefID, bool& bExist)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "select_icc_t_callevent_count";
	l_oSQLRequest.param["callref_id"] = strCallRefID;
	std::string strTime = m_pDateTime->GetCallRefIdTime(strCallRefID);
	l_oSQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
	l_oSQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);

	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select icc_t_callevent table failed, SQL: [%s], Error:[%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string strCount = l_pResult->GetValue("count");
		if (stoi(strCount) > 0)
		{
			bExist = true;
		}
		else
		{
			bExist = false;
		}
	}
	return true;
}

bool CBusinessImpl::_InsertCalleventWithHistoryCall(const PROTOCOL::CGetHistoryCallListRequest& request, int i)
{
	if (request.m_oBody.m_vecData[i].m_strCallRefID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "callref id is empty!");
		return false;
	}
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "insert_icc_t_callevent";
	l_oSQLRequest.param["callref_id"] = request.m_oBody.m_vecData[i].m_strCallRefID;
	if (!request.m_oBody.m_vecData[i].m_strACD.empty())
	{
		l_oSQLRequest.param["acd"] = request.m_oBody.m_vecData[i].m_strACD;
		l_oSQLRequest.param["original_acd"] = request.m_oBody.m_vecData[i].m_strACD;
		PROTOCOL::CAcdInfo l_AcdInfo;
		if (!_GetAcdInfo(request.m_oBody.m_vecData[i].m_strACD, l_AcdInfo))
		{
			ICC_LOG_DEBUG(m_pLog,"get acd info failed,acd:[%s]", request.m_oBody.m_vecData[i].m_strACD.c_str());
		}
		l_oSQLRequest.param["acd_dept"] = l_AcdInfo.m_dept_code;
		l_oSQLRequest.param["acd_relation_dept_name"] = l_AcdInfo.m_dept_name;
		l_oSQLRequest.param["acd_relation_dept"] = l_AcdInfo.m_dept_code;
		l_oSQLRequest.param["acd_relation_dept_path"] = l_AcdInfo.m_dept_code_path;
		if (!l_AcdInfo.m_dept_code.empty())
		{
			PROTOCOL::CDeptInfo l_DeptInfo;
			if(!_GetDeptInfo(l_AcdInfo.m_dept_code, l_DeptInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get acd dept info failed,dept code:[%s]", l_AcdInfo.m_dept_code.c_str());
			}
			if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
			{
				l_oSQLRequest.param["acd_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
			}
		}
	}
	if (!request.m_oBody.m_vecData[i].m_strCalledID.empty())
	{
		l_oSQLRequest.param["called_id"] = request.m_oBody.m_vecData[i].m_strCalledID;
		if (request.m_oBody.m_vecData[i].m_strCallDirection == "1" || request.m_oBody.m_vecData[i].m_strCallDirection == "3")
		{
			PROTOCOL::CSeatDataInfo l_SeatDataInfo;
			if (!_GetSeatInfoInRedis(request.m_oBody.m_vecData[i].m_strCalledID, l_SeatDataInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get dept info by seat failed,called id:[%s]", request.m_oBody.m_vecData[i].m_strCalledID.c_str());
			}
			if (!l_SeatDataInfo.m_oBody.m_strDeptCode.empty())
			{
				l_oSQLRequest.param["called_relation_dept_name"] = l_SeatDataInfo.m_oBody.m_strDeptName;
				l_oSQLRequest.param["called_relation_dept"] = l_SeatDataInfo.m_oBody.m_strDeptCode;
				PROTOCOL::CDeptInfo l_DeptInfo;
				if (!_GetDeptInfo(l_SeatDataInfo.m_oBody.m_strDeptCode, l_DeptInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get dept info failed,dept code:[%s]", l_SeatDataInfo.m_oBody.m_strDeptCode.c_str());
				}
				if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
				{
					l_oSQLRequest.param["called_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
				}
				if (!l_DeptInfo.m_strCodeLevel.empty())
				{
					l_oSQLRequest.param["called_relation_dept_path"] = l_DeptInfo.m_strCodeLevel;
				}
			}
		}
		std::string strCalledType = _QueryNumberType(request.m_oBody.m_vecData[i].m_strCalledID);
		l_oSQLRequest.param["called_type"] = strCalledType;
	}

	if (!request.m_oBody.m_vecData[i].m_strCallerID.empty())
	{
		l_oSQLRequest.param["caller_id"] = request.m_oBody.m_vecData[i].m_strCallerID;
		if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
		{
			PROTOCOL::CSeatDataInfo l_SeatDataInfo;
			if (!_GetSeatInfoInRedis(request.m_oBody.m_vecData[i].m_strCallerID, l_SeatDataInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get dept info by seat failed,caller id:[%s]", request.m_oBody.m_vecData[i].m_strCallerID.c_str());
			}
			if (!l_SeatDataInfo.m_oBody.m_strDeptCode.empty())
			{
				l_oSQLRequest.param["caller_relation_dept_name"] = l_SeatDataInfo.m_oBody.m_strDeptName;
				l_oSQLRequest.param["caller_relation_dept"] = l_SeatDataInfo.m_oBody.m_strDeptCode;
				PROTOCOL::CDeptInfo l_DeptInfo;
				if (!_GetDeptInfo(l_SeatDataInfo.m_oBody.m_strDeptCode, l_DeptInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get dept info failed,dept code:[%s]", l_SeatDataInfo.m_oBody.m_strDeptCode.c_str());
				}
				if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
				{
					l_oSQLRequest.param["caller_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
				}
				if (!l_DeptInfo.m_strCodeLevel.empty())
				{
					l_oSQLRequest.param["caller_relation_dept_path"] = l_DeptInfo.m_strCodeLevel;
				}
			}
		}
		std::string strCallerType = _QueryNumberType(request.m_oBody.m_vecData[i].m_strCallerID);
		l_oSQLRequest.param["caller_type"] = strCallerType;

		if (request.m_oBody.m_vecData[i].m_strCallDirection == "1" || request.m_oBody.m_vecData[i].m_strCallDirection == "3")
		{
			l_oSQLRequest.param["call_direction"] = "in";
		}
		else if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
		{
			l_oSQLRequest.param["call_direction"] = "out";
		}
		l_oSQLRequest.param["talk_time"] = request.m_oBody.m_vecData[i].m_strTalkTime;
		l_oSQLRequest.param["hangup_time"] = request.m_oBody.m_vecData[i].m_strHangupTime;
		l_oSQLRequest.param["hangup_type"] ="unknown";
		l_oSQLRequest.param["incoming_time"] = request.m_oBody.m_vecData[i].m_strIncomingTime;
		l_oSQLRequest.param["ring_time"] = request.m_oBody.m_vecData[i].m_strRingTime;
		std::string strRingBackTime;
		if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
		{
			strRingBackTime = request.m_oBody.m_vecData[i].m_strRingTime;
			l_oSQLRequest.param["ringback_time"] = strRingBackTime;
		}
		l_oSQLRequest.param["switch_type"] ="3";
		l_oSQLRequest.param["create_time"] = m_pDateTime->GetCallRefIdTime(request.m_oBody.m_vecData[i].m_strCallRefID);
		if (!request.m_oBody.m_vecData[i].m_strRingTime.empty() && !request.m_oBody.m_vecData[i].m_strIncomingTime.empty())
		{
			std::string strIncomingLen = BuildTimeLen(request.m_oBody.m_vecData[i].m_strRingTime, request.m_oBody.m_vecData[i].m_strIncomingTime);
			l_oSQLRequest.param["incoming_len"] = strIncomingLen;
		}
		std::string strRingLen = CalcRingLen(request.m_oBody.m_vecData[i].m_strTalkTime, request.m_oBody.m_vecData[i].m_strRingTime,
			request.m_oBody.m_vecData[i].m_strHangupTime, strRingBackTime);
		if (!strRingLen.empty())
		{
			l_oSQLRequest.param["ring_len"] = strRingLen;
		}
		if (!request.m_oBody.m_vecData[i].m_strHangupTime.empty() && !request.m_oBody.m_vecData[i].m_strTalkTime.empty())
		{
			std::string strTalkLen = BuildTimeLen(request.m_oBody.m_vecData[i].m_strHangupTime, request.m_oBody.m_vecData[i].m_strTalkTime);
			l_oSQLRequest.param["talk_len"] = strTalkLen;
		}
		std::string strAlarmID;
		std::string strReceiverCode;
		std::string strReceiverName;
		if (!_FindIfCreateAlarm(request.m_oBody.m_vecData[i].m_strCallRefID, strAlarmID, strReceiverCode, strReceiverName))
		{
			ICC_LOG_ERROR(m_pLog, "find alarm id by callref_id in jjdb faliled,callref_id:[%s]", request.m_oBody.m_vecData[i].m_strCallRefID.c_str());
		}
		if (!strAlarmID.empty())
		{
			l_oSQLRequest.param["rel_alarm_id"] = strAlarmID;
		}
		if (!strReceiverCode.empty())
		{
			std::string strStaffCode = _StaffIdNoToStaffCode(strReceiverCode);
			if (!strStaffCode.empty())
			{
				l_oSQLRequest.param["receiver_code"] = strStaffCode;
			}
		}
		if (!strReceiverName.empty())
		{
			l_oSQLRequest.param["receiver_name"] = strReceiverName;
		}

		bool is_Exist = false;
		if (!_JudgeIsCallback(request.m_oBody.m_vecData[i].m_strCallRefID, is_Exist))
		{
			ICC_LOG_DEBUG(m_pLog, "judge is callback failed,callref id:[%s]", request.m_oBody.m_vecData[i].m_strCallRefID.c_str());
		}
		if (is_Exist == true)
		{
			l_oSQLRequest.param["is_callback"] = strAlarmID;
		}

		DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "select icc_t_callevent table failed, SQL: [%s], Error:[%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::_GetAcdInfo(const std::string& strAcd, PROTOCOL::CAcdInfo& l_AcdInfo)
{
	std::string strAcdInfo;
	if (!m_pRedisClient->HGet("acd_dept", strAcd, strAcdInfo))
	{
		ICC_LOG_ERROR(m_pLog, "Hget acd_dept failed,acd:[%s]", strAcd.c_str());
		return false;
	}
	if (!l_AcdInfo.Parse(strAcdInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse acd info failed,acd info:[%s]", strAcdInfo.c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::_FindIfCreateAlarm(const std::string& strCallrefID, std::string& strAlarmID, std::string& strReceiverCode, std::string& strReceiverName)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "select_icc_t_jjdb";
	l_oSQLRequest.param["source_id"] = strCallrefID;
	std::string strTime = m_pDateTime->GetCallRefIdTime(strCallrefID);
	l_oSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
	l_oSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select icc_t_jjdb exec failed:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		strAlarmID = l_pResult->GetValue("id");
		strReceiverCode = l_pResult->GetValue("receipt_code");
		strReceiverName= l_pResult->GetValue("receipt_name");
		return true;
	}
	return true;
}

bool CBusinessImpl::_GetDeptInfo(const std::string& strDeptCode, PROTOCOL::CDeptInfo& l_DeptInfo)
{
	std::string strDeptInfo;
	if (!m_pRedisClient->HGet("DeptCodeInfoKey", strDeptCode, strDeptInfo))
	{
		ICC_LOG_ERROR(m_pLog, "Hget DeptCodeInfoKey failed,dept code:[%s]", strDeptCode.c_str());
		return false;
	}
	if (!l_DeptInfo.Parse(strDeptInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse dept info failed,dept info:[%s]", strDeptInfo.c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::_JudgeIsCallback(const std::string& strCallrefID, bool& isExist)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "select_icc_t_callback_relation_count";
	l_oSQLRequest.param["callref_id"] = strCallrefID;
	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select icc_t_callback_relation exec failed:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string strCount = l_pResult->GetValue("count");
		if (stoi(strCount) > 0)
		{
			isExist = true;
		}
		return true;
	}
	return true;
}

bool CBusinessImpl::_UpdateCalleventWithHistoryCall(const PROTOCOL::CGetHistoryCallListRequest& request, int i)
{
	if (request.m_oBody.m_vecData[i].m_strCallRefID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "callref id is empty!");
		return false;
	}
		//根据话务ID以及incoming_time升序排序,如果一个话务多条录音，第二条数据的各种状态时间不去覆盖第一个的
	if ((i>1&&request.m_oBody.m_vecData[i].m_strCallRefID != request.m_oBody.m_vecData[i - 1].m_strCallRefID)||i==0)
	{
		DataBase::SQLRequest l_oSQLRequest;
		l_oSQLRequest.sql_id = "update_icc_t_callevent";
		l_oSQLRequest.param["callref_id"] = request.m_oBody.m_vecData[i].m_strCallRefID;
		if (!request.m_oBody.m_vecData[i].m_strACD.empty())
		{
			l_oSQLRequest.set["acd"] = request.m_oBody.m_vecData[i].m_strACD;
			l_oSQLRequest.set["original_acd"] = request.m_oBody.m_vecData[i].m_strACD;
			PROTOCOL::CAcdInfo l_AcdInfo;
			if (!_GetAcdInfo(request.m_oBody.m_vecData[i].m_strACD, l_AcdInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get acd info failed,acd:[%s]", request.m_oBody.m_vecData[i].m_strACD.c_str());
			}
			l_oSQLRequest.set["acd_dept"] = l_AcdInfo.m_dept_code;
			l_oSQLRequest.set["acd_relation_dept_name"] = l_AcdInfo.m_dept_name;
			l_oSQLRequest.set["acd_relation_dept"] = l_AcdInfo.m_dept_code;
			l_oSQLRequest.set["acd_relation_dept_path"] = l_AcdInfo.m_dept_code_path;
			if (!l_AcdInfo.m_dept_code.empty())
			{
				PROTOCOL::CDeptInfo l_DeptInfo;
				if (!_GetDeptInfo(l_AcdInfo.m_dept_code, l_DeptInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get acd dept info failed,dept code:[%s]", l_AcdInfo.m_dept_code.c_str());
				}
				if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
				{
					l_oSQLRequest.param["acd_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
				}
			}
		}
		//以incoming时间排序,来电早的为第一通话务，第二个成员历史话务来了不更新主被叫
		if (!request.m_oBody.m_vecData[i].m_strCalledID.empty())
		{
			if (request.m_oBody.m_vecData[i].m_strCallDirection == "1" || request.m_oBody.m_vecData[i].m_strCallDirection == "3")
			{
				PROTOCOL::CSeatDataInfo l_SeatDataInfo;
				if (!_GetSeatInfoInRedis(request.m_oBody.m_vecData[i].m_strCalledID, l_SeatDataInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get dept info by seat failed,called id:[%s]", request.m_oBody.m_vecData[i].m_strCalledID.c_str());
				}
				if (!l_SeatDataInfo.m_oBody.m_strDeptCode.empty())
				{
					l_oSQLRequest.set["called_relation_dept_name"] = l_SeatDataInfo.m_oBody.m_strDeptName;
					l_oSQLRequest.set["called_relation_dept"] = l_SeatDataInfo.m_oBody.m_strDeptCode;
					PROTOCOL::CDeptInfo l_DeptInfo;
					if (!_GetDeptInfo(l_SeatDataInfo.m_oBody.m_strDeptCode, l_DeptInfo))
					{
						ICC_LOG_DEBUG(m_pLog, "get dept info failed,dept code:[%s]", l_SeatDataInfo.m_oBody.m_strDeptCode.c_str());
					}
					if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
					{
						l_oSQLRequest.set["called_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
					}
					if (!l_DeptInfo.m_strCodeLevel.empty())
					{
						l_oSQLRequest.set["called_relation_dept_path"] = l_DeptInfo.m_strCodeLevel;
					}
				}
				if (!l_SeatDataInfo.m_oBody.m_strStaffCode.empty())
				{
					l_oSQLRequest.set["receiver_code"] = l_SeatDataInfo.m_oBody.m_strStaffCode;
				}
				if (!l_SeatDataInfo.m_oBody.m_strStaffName.empty())
				{
					l_oSQLRequest.set["receiver_name"] = l_SeatDataInfo.m_oBody.m_strStaffName;
				}
			}
			std::string strCalledType = _QueryNumberType(request.m_oBody.m_vecData[i].m_strCalledID);
			l_oSQLRequest.set["called_type"] = strCalledType;
		}


		if (!request.m_oBody.m_vecData[i].m_strCallerID.empty())
		{
			//l_oSQLRequest.set["caller_id"] = request.m_oBody.m_vecData[i].m_strCallerID;
			PROTOCOL::CSeatDataInfo l_SeatDataInfo;
			if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
			{
				if (!_GetSeatInfoInRedis(request.m_oBody.m_vecData[i].m_strCallerID, l_SeatDataInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get dept info by seat failed,caller id:[%s]", request.m_oBody.m_vecData[i].m_strCallerID.c_str());
				}
				if (!l_SeatDataInfo.m_oBody.m_strDeptCode.empty())
				{
					l_oSQLRequest.set["caller_relation_dept_name"] = l_SeatDataInfo.m_oBody.m_strDeptName;
					l_oSQLRequest.set["caller_relation_dept"] = l_SeatDataInfo.m_oBody.m_strDeptCode;
					PROTOCOL::CDeptInfo l_DeptInfo;
					if (!_GetDeptInfo(l_SeatDataInfo.m_oBody.m_strDeptCode, l_DeptInfo))
					{
						ICC_LOG_DEBUG(m_pLog, "get dept info failed,dept code:[%s]", l_SeatDataInfo.m_oBody.m_strDeptCode.c_str());
					}
					if (!l_DeptInfo.m_strPucOrgIdentifier.empty())
					{
						l_oSQLRequest.set["caller_relation_dept_identify"] = l_DeptInfo.m_strPucOrgIdentifier;
					}
					if (!l_DeptInfo.m_strCodeLevel.empty())
					{
						l_oSQLRequest.set["caller_relation_dept_path"] = l_DeptInfo.m_strCodeLevel;
					}
				}
			}

			std::string strCallerType = _QueryNumberType(request.m_oBody.m_vecData[i].m_strCallerID);
			l_oSQLRequest.set["caller_type"] = strCallerType;
			if (request.m_oBody.m_vecData[i].m_strCallDirection == "1" || request.m_oBody.m_vecData[i].m_strCallDirection == "3")
			{
				l_oSQLRequest.set["call_direction"] = "in";
			}
			else if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
			{
				l_oSQLRequest.set["call_direction"] = "out";
			}
			l_oSQLRequest.set["talk_time"] = request.m_oBody.m_vecData[i].m_strTalkTime;
			l_oSQLRequest.set["hangup_time"] = request.m_oBody.m_vecData[i].m_strHangupTime;
			l_oSQLRequest.set["hangup_type"] = "unknown";
			l_oSQLRequest.set["incoming_time"] = request.m_oBody.m_vecData[i].m_strIncomingTime;
			l_oSQLRequest.set["ring_time"] = request.m_oBody.m_vecData[i].m_strRingTime;
			std::string strRingBackTime;
			if (request.m_oBody.m_vecData[i].m_strCallDirection == "2")
			{
				strRingBackTime = request.m_oBody.m_vecData[i].m_strRingTime;
				l_oSQLRequest.set["ringback_time"] = strRingBackTime;
			}
			l_oSQLRequest.set["switch_type"] = "3";
			l_oSQLRequest.set["create_time"] = m_pDateTime->GetCallRefIdTime(request.m_oBody.m_vecData[i].m_strCallRefID);
			if (!request.m_oBody.m_vecData[i].m_strRingTime.empty() && !request.m_oBody.m_vecData[i].m_strIncomingTime.empty())
			{
				std::string strIncomingLen = BuildTimeLen(request.m_oBody.m_vecData[i].m_strRingTime, request.m_oBody.m_vecData[i].m_strIncomingTime);
				l_oSQLRequest.set["incoming_len"] = strIncomingLen;
			}
			std::string strRingLen = CalcRingLen(request.m_oBody.m_vecData[i].m_strTalkTime, request.m_oBody.m_vecData[i].m_strRingTime,
				request.m_oBody.m_vecData[i].m_strHangupTime, strRingBackTime);
			if (!strRingLen.empty())
			{
				l_oSQLRequest.set["ring_len"] = strRingLen;
			}
			if (!request.m_oBody.m_vecData[i].m_strHangupTime.empty() && !request.m_oBody.m_vecData[i].m_strTalkTime.empty())
			{
				std::string strTalkLen = BuildTimeLen(request.m_oBody.m_vecData[i].m_strHangupTime, request.m_oBody.m_vecData[i].m_strTalkTime);
				l_oSQLRequest.set["talk_len"] = strTalkLen;
			}

			std::string strAlarmID;
			std::string strReceiverCode;
			std::string strReceiverName;
			if (!_FindIfCreateAlarm(request.m_oBody.m_vecData[i].m_strCallRefID, strAlarmID, strReceiverCode, strReceiverName))
			{
				ICC_LOG_ERROR(m_pLog, "find alarm id by callref_id in jjdb faliled,callref_id:[%s]", request.m_oBody.m_vecData[i].m_strCallRefID.c_str());
			}
			if (!strAlarmID.empty())
			{
				l_oSQLRequest.set["rel_alarm_id"] = strAlarmID;
			}
			if (!strReceiverCode.empty())
			{
				std::string strStaffCode = _StaffIdNoToStaffCode(strReceiverCode);
				if (!strStaffCode.empty())
				{
					l_oSQLRequest.set["receiver_code"] = strStaffCode;
				}
			}
			if (!strReceiverName.empty())
			{
				l_oSQLRequest.set["receiver_name"] = strReceiverName;
			}

			bool is_Exist = false;
			if (!_JudgeIsCallback(request.m_oBody.m_vecData[i].m_strCallRefID, is_Exist))
			{
				ICC_LOG_DEBUG(m_pLog, "judge is callback failed,callref id:[%s]", request.m_oBody.m_vecData[i].m_strCallRefID.c_str());
			}
			if (is_Exist == true)
			{
				l_oSQLRequest.set["is_callback"] = strAlarmID;
			}

			DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
			if (!l_pResult->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select icc_t_callevent table failed, SQL: [%s], Error:[%s]",
					l_pResult->GetSQL().c_str(),
					l_pResult->GetErrorMsg().c_str());
				return false;
			}
		}
	}
	return true;
}

bool CBusinessImpl::_IsNeedUpdateCallevent(const std::string& strCallrefID, bool& isNeedUpdate)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "select_icc_t_callevent";
	l_oSQLRequest.param["callref_id"] = strCallrefID;
	std::string strTime = m_pDateTime->GetCallRefIdTime(strCallrefID);
	l_oSQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
	l_oSQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);

	DataBase::IResultSetPtr l_pResult = m_pRequestDBConn->Exec(l_oSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select icc_t_callevent table failed, SQL: [%s], Error:[%s]",
			l_pResult->GetSQL().c_str(),
			l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string strHangupTime = l_pResult->GetValue("hangup_time");
		if (!strHangupTime.empty())
		{
			isNeedUpdate = false;
		}
	}
	return true;
}

bool CBusinessImpl::_GetSeatInfoInRedis(const std::string& strSeat, PROTOCOL::CSeatDataInfo& l_SeatDataInfo)
{
	std::string strSeatInfo;
	if (!m_pRedisClient->HGet("SeatDataInfo", strSeat, strSeatInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "hget SeatDataInfo failed,seat:[%s]", strSeat.c_str());
		return false;
	}
	if (!l_SeatDataInfo.ParseString(strSeatInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse seat info failed,seat info:[%s]", strSeatInfo.c_str());
		return false;
	}
	return true;
}

std::string CBusinessImpl::_StaffIdNoToStaffCode(const std::string& strStaffIdNo)
{
	PROTOCOL::CStaffInfo l_StaffInfo;
	std::string strStaffInfo;
	if (!m_pRedisClient->HGet("StaffIdInfo", strStaffIdNo, strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "staff_id_no to staff_code failed,staffIdNo:[%s]", strStaffIdNo.c_str());
		return "";
	}
	if (!l_StaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse sraff info failed,staff info:[%s]", strStaffInfo.c_str());
		return "";
	}
	return l_StaffInfo.m_strCode;
}