#include "Boost.h"
#include "BusinessImpl.h"

#define DIC020030				("DIC020030")	//处警单状态-已签收
#define DIC020073				("DIC020073")	//处警单状态-已退单
#define BS001002028				("BS001002028")	//流水状态-已退单
#define STAFF_ID_NO				"1"
#define STAFF_CODE				"2"

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_MPA_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_LockFacPtr = ICCGetILockFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/MPA/IsUsing", "0");
	m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/MPA/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/MPA/Police", "");
	m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/MPA/CodeMode", "");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	Connect();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_feedback_sync", AlarmFeedbackSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "app_edit_feedback_request", AppEditFeedback);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "app_get_feedback_request", AppGetFeedback);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "app_back_alarm_request", AppBackAlarmRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_process_sync", OnAlarmProcessSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "app_get_id_request", AppGetAlarmIdRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_respond", AppGetAlarmIdFromICCRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "app_comit_alarm_request", AppCommitAlarmInfoRequest); 
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_and_process_respond", OnCommitAlarmInfoResponse);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_feedback_respond", AddOrUpdateFeedbackRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_info_respond", GetFeedbackRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "mpa_edit_alarm_request", OnMpaEditAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_and_process_respond", OnMpaEditAlarmRespond);
	ICC_LOG_DEBUG(m_pLog, "%s plugin start,AssistantPolice=%s,Police=%s,CodeMode:[%s]", MODULE_NAME, m_strAssistantPolice.c_str(), m_strPolice.c_str(),m_strCodeMode.c_str());
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::Connect()
{
}

void CBusinessImpl::OnCommitAlarmInfoResponse(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string strMessage = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());
	// 解析请求消息
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRequest;
	if (!l_oRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse message error:[%s]", strMessage.c_str());
		return;
	}

	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond response;
	response.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	response.m_oHeader.m_strMsgid = l_oRequest.m_oHeader.m_strRelatedID;
	response.m_oHeader.m_strCmd = "app_comit_alarm_respond";
	response.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	response.m_oHeader.m_strRequest = QUEUE_ALARM;
	response.m_oHeader.m_strRequestType = "0";

	//response.m_oBody.m_strResult = l_oRequest.m_oBody.m_strResult;

	JsonParser::IJsonPtr l_pIJsonResponse = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSendMsg = response.ToString(l_pIJsonResponse);
	p_pRequest->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::AppCommitAlarmInfoRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string strMessage = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());
	// 解析请求消息
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest;
	if (!l_oRequest.ParseString4AddAlarm(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse message error:[%s]", strMessage.c_str());
		return;
	}

	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest requestFromICC;
	requestFromICC.m_oHeader.m_strMsgid = l_oRequest.m_oHeader.m_strMsgid;
	requestFromICC.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
	requestFromICC.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	requestFromICC.m_oHeader.m_strRequest = QUEUE_ALARM;
	requestFromICC.m_oHeader.m_strRequestType = "0";

	requestFromICC.m_oBody.m_oAlarm = l_oRequest.m_oBody.m_oAlarm;	
	//requestFromICC.m_oBody.m_oAlarm.m_strActualOccurTime = requestFromICC.m_oBody.m_oAlarm.m_strActualOccurTime.empty() ? l_oRequest.m_oBody.m_oAlarm.m_strTime : requestFromICC.m_oBody.m_oAlarm.m_strActualOccurTime;
	requestFromICC.m_oBody.m_oAlarm.m_strUpdateUser = requestFromICC.m_oBody.m_oAlarm.m_strReceiptCode;
	requestFromICC.m_oBody.m_oAlarm.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	requestFromICC.m_oBody.m_oAlarm.m_strState = "03";
	requestFromICC.m_oBody.m_oAlarm.m_strHandleType = "01";//"DIC002001";//报警或求助
	//requestFromICC.m_oBody.m_oAlarm.m_strContactGender = l_oRequest.m_oBody.m_oAlarm.m_strContactGender.empty() ? "DIC028003" : l_oRequest.m_oBody.m_oAlarm.m_strContactGender;
	requestFromICC.m_oBody.m_oAlarm.m_strCallerGender = l_oRequest.m_oBody.m_oAlarm.m_strCallerGender.empty() ? "DIC028003" : l_oRequest.m_oBody.m_oAlarm.m_strCallerGender;
	requestFromICC.m_oBody.m_oAlarm.m_strMsgSource = "mpa";


	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_processData;
	l_processData.m_bIsNewProcess = true;																	//是否是新增的处警单，true代表是，false代表否
	l_processData.m_strMsgSource = "mpa";								//消息来源
	l_processData.m_strID = "";							//处警ID
	l_processData.m_strAlarmID = l_oRequest.m_oBody.m_oAlarm.m_strID;										//警情ID
	l_processData.m_strState = l_oRequest.m_oBody.m_oAlarm.m_strState;																	//处警单状态 默认给"已下达"
	//l_processData.m_strTimeEdit = l_oRequest.m_oBody.m_oAlarm.m_strTime;										//派警单填写时间
	l_processData.m_strTimeSubmit = l_oRequest.m_oBody.m_oAlarm.m_strTime;									    //派警单提交时间
	l_processData.m_strTimeArrived = l_oRequest.m_oBody.m_oAlarm.m_strTime;
	l_processData.m_strTimeSigned = l_oRequest.m_oBody.m_oAlarm.m_strTime;

	l_processData.m_strDispatchDeptDistrictCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//派警单位行政区划
	l_processData.m_strDispatchDeptCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//派警单位代码
	l_processData.m_strDispatchDeptName = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName;					//派警单位名称
	l_processData.m_strDispatchCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode;							//派警人警号
	l_processData.m_strDispatchName = l_oRequest.m_oBody.m_oAlarm.m_strReceiptName;							//派警人姓名

	//l_processData.m_strProcessDeptDistrictCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//处警单位行政区划
	l_processData.m_strProcessDeptCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//处警单位代码
	l_processData.m_strProcessDeptName = l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName;					//处警单位名称
	l_processData.m_strProcessCode = l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode;							//处警人警号
	l_processData.m_strProcessName = l_oRequest.m_oBody.m_oAlarm.m_strReceiptName;							//处警人姓名

	l_processData.m_strUpdateUser = requestFromICC.m_oBody.m_oAlarm.m_strReceiptCode;
	l_processData.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

	requestFromICC.m_oBody.m_vecProcessData.push_back(l_processData);

	m_mapGuidAndCmds.insert(std::make_pair(l_oRequest.m_oHeader.m_strMsgid, "app_comit_alarm_request"));
	ICC_LOG_DEBUG(m_pLog, "cmd map size :[%d]", m_mapGuidAndCmds.size());

	std::string l_strSendMsg = requestFromICC.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg, ObserverPattern::ERequestMode::Request_Respond, p_pRequest));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

}

void CBusinessImpl::AppGetAlarmIdFromICCRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string strMessage = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());
	// 解析请求消息
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAppGetAlarmIdResponse l_oRequest;
	if (!l_oRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse message error:[%s]", strMessage.c_str());
		return;
	}
	 
	PROTOCOL::CAppGetAlarmIdResponse l_oRespond;
	l_oRespond.m_oBody.m_strAlarmId = l_oRequest.m_oBody.m_strAlarmId;
	l_oRespond.m_oBody.m_strAlarmCreateTime = l_oRequest.m_oBody.m_strAlarmCreateTime;

	_BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader, "app_get_id_respond");	
	JsonParser::IJsonPtr l_pIJsonResponse = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSendMsg = l_oRespond.ToString(l_pIJsonResponse);
	p_pRequest->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

}

void CBusinessImpl::AppGetAlarmIdRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string strMessage = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());
	// 解析请求消息
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAppGetAlarmIdRequest l_oRequest;
	if (!l_oRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse message error:[%s]", strMessage.c_str());
		return;
	}

	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();

	PROTOCOL::CAppGetAlarmIdFromICCRequest requestFromICC;
	requestFromICC.m_oBody.m_oData.m_strReceiptDeptCode = l_oRequest.m_oBody.m_oData.m_strReceiptDeptCode;
	requestFromICC.m_oBody.m_oData.m_strReceiptDeptName = l_oRequest.m_oBody.m_oData.m_strReceiptDeptName;
	requestFromICC.m_oBody.m_oData.m_strReceiptCode = l_oRequest.m_oBody.m_oData.m_strReceiptUserCode;
	requestFromICC.m_oBody.m_oData.m_strReceiptName = l_oRequest.m_oBody.m_oData.m_strReceiptUserName;
	requestFromICC.m_oBody.m_oData.m_strTime = l_oRequest.m_oHeader.m_strSendTime.empty() ? l_strCurTime : l_oRequest.m_oHeader.m_strSendTime;

	requestFromICC.m_oHeader.m_strMsgid = l_oRequest.m_oHeader.m_strMsgid;	
	requestFromICC.m_oHeader.m_strCmd = "add_alarm_request";
	requestFromICC.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	requestFromICC.m_oHeader.m_strRequest = QUEUE_ALARM;
	requestFromICC.m_oHeader.m_strRequestType = "0";


	std::string l_strSendMsg = requestFromICC.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg, ObserverPattern::ERequestMode::Request_Respond, p_pRequest));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

}

void CBusinessImpl::_BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd)
{
	p_oRespondHeader.m_strSystemID = "ICC";
	p_oRespondHeader.m_strSubsystemID = "APP";
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();	
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = p_strCmd;
	p_oRespondHeader.m_strRequest = "icc_update_feedback";
	p_oRespondHeader.m_strRequestType = "1";
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

bool CBusinessImpl::_BuildWebProcessDept(std::string m_strAlarmID, std::vector<PROTOCOL::CAppProcessDept>& p_vecProcessDept)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_process_web";
	l_SqlRequest.param["alarm_id"] = m_strAlarmID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		PROTOCOL::CAppProcessDept l_oProcessDept;
		l_oProcessDept.m_strDeptCode = l_pResult->GetValue("process_dept_code");
		l_oProcessDept.m_strDeptName = l_pResult->GetValue("process_dept_name");
		l_oProcessDept.m_strParentDeptCode = l_pResult->GetValue("parent_dept_code");
		p_vecProcessDept.push_back(l_oProcessDept);
	}
	return true;
}

void CBusinessImpl::OnAlarmProcessSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAppAlarmProcessSync l_oWebAlarmProcessSync;
	if (!l_oWebAlarmProcessSync.ParseString(p_pNotify->GetMessages(), ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	_BuildRespondHeader(l_oWebAlarmProcessSync.m_oHeader, "synthetical_proesss_info");
	_BuildWebProcessDept(l_oWebAlarmProcessSync.m_oBody.m_oProcess.m_strAlarmID, l_oWebAlarmProcessSync.m_oBody.m_vecProcessDept);

	std::string l_strSendMsg = l_oWebAlarmProcessSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::_BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader, const std::string& strResponseCmd)
{
	p_oRespondHeader.m_strSystemID = p_oRequestHeader.m_strSystemID;
	p_oRespondHeader.m_strSubsystemID = p_oRequestHeader.m_strSubsystemID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strRelatedID;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = strResponseCmd;
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

std::string CBusinessImpl::_QueryProcessIdByAlarmIdAndProcessDeptCode(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	std::string l_strProcessId;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_alarm_process_id_by_alarm_id_and_process_dept_code";
	l_SqlRequest.param["alarm_id"] = p_oRequest.m_oBody.m_oData.m_strAlarmID;
	l_SqlRequest.param["process_dept_code"] = p_oRequest.m_oBody.m_oData.m_strBackDeptID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	if (l_pResult->Next())
	{
		l_strProcessId = l_pResult->GetValue("id");
	}

	return l_strProcessId;
}

bool CBusinessImpl::_IsStateBeforAccepted(std::string p_strProcessID, const std::string& strTransGuid)
{
	std::string l_strProcessState;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strProcessID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strProcessID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		l_strProcessState = l_pResult->GetValue("state");
	}

	if (l_strProcessState.empty() || l_strProcessState.compare(DIC020030) >= 0)
	{
		return false;
	}

	return true;
}

bool CBusinessImpl::_UpdateProcessState(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "update_icc_t_pjdb";
	l_tSQLRequest.param["id"] = p_oRequest.m_oBody.m_oData.m_strProcessID;
	std::string strTime = m_pDateTime->GetDispatchIdTime(p_oRequest.m_oBody.m_oData.m_strProcessID);
	if (strTime != "")
	{
		l_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLRequest.set["state"] = DIC020073;
	l_tSQLRequest.set["update_user"] = p_oRequest.m_oBody.m_oData.m_strBackUserID;
	l_tSQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateProcess sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::_UpdateAlarmState(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "update_icc_t_alarm";
	l_tSQLRequest.param["id"] = p_oRequest.m_oBody.m_oData.m_strProcessID;
	l_tSQLRequest.param["state"] = "02";
	l_tSQLRequest.set["update_user"] = p_oRequest.m_oBody.m_oData.m_strBackUserID;
	l_tSQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateAlarm sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::_InsertBackReason(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "insert_icc_t_backalarm_reason";
	l_tSQLRequest.param["guid"] = m_pString->CreateGuid();
	l_tSQLRequest.param["process_id"] = p_oRequest.m_oBody.m_oData.m_strProcessID;
	l_tSQLRequest.param["back_reason"] = p_oRequest.m_oBody.m_oData.m_strReason;	

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "mpa insert back reason sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "mpa insert back reason failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}


std::string CBusinessImpl::_BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

bool CBusinessImpl::_InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		//TODO::select_icc_t_jjdb_jjsj 查询jjsj
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_strAlarmID);

		if (strTime != "")
		{
			DataBase::SQLRequest l_sqlReqeust;
			l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
			l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
			DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
			if (!l_pRSetPtr->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
			}
			if (l_pRSetPtr->Next())
			{
				p_AlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
			}
		}
	}

	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		p_AlarmLogInfo.m_strReceivedTime = l_strCurTime;
	}

	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		return false;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	return true;
}

void CBusinessImpl::_SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "APP";
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::_AddAlarmLogInfo(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	std::string l_strGUID(m_pString->CreateGuid());
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));

	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strID = l_strGUID;
	l_oAlarmLogInfo.m_strAlarmID = p_oRequest.m_oBody.m_oData.m_strAlarmID;
	l_oAlarmLogInfo.m_strProcessID = p_oRequest.m_oBody.m_oData.m_strProcessID;
	l_oAlarmLogInfo.m_strOperate = BS001002028;
	{//l_oAlarmLogInfo.m_strContent
		std::vector<std::string> l_vecParamList;
		Data::CStaffInfo l_oStaffInfo;
		if (!_GetStaffInfo(p_oRequest.m_oBody.m_oData.m_strBackUserID, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
		}
		std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, p_oRequest.m_oBody.m_oData.m_strBackUserName);
		l_vecParamList.push_back(l_strStaffName);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strBackUserID);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strBackDeptName);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strReason);
		l_oAlarmLogInfo.m_strOperateContent = _BuildAlarmLogContent(l_vecParamList);
	}

	l_oAlarmLogInfo.m_strCreateUser = p_oRequest.m_oBody.m_oData.m_strBackUserName;
	l_oAlarmLogInfo.m_strCreateTime = l_strCurTime;

	if (!_InsertAlarmLogInfo(l_oAlarmLogInfo, strTransGuid))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log failed!");
		return false;
	}

	_SyncAlarmLogInfo(l_oAlarmLogInfo);

	return true;
}

void CBusinessImpl::_SynAlarmInfo(const std::string& strAlarmId)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqSelectAlarm;
	l_tSQLReqSelectAlarm.sql_id = "select_icc_t_jjdb";

	l_tSQLReqSelectAlarm.param["id"] = strAlarmId;	
	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);

	if (strTime != "")
	{
		l_tSQLReqSelectAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReqSelectAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqSelectAlarm);
	ICC_LOG_DEBUG(m_pLog, "sql select_icc_t_alarm:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return;
	}

	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmSync.m_oHeader.m_strSubsystemID = "";
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmSync.m_oHeader.m_strCmd = "alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource = "mpa";	

	l_oAlarmSync.m_oBody.m_strSyncType = "4";//编辑	

	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strID = strAlarmId;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strState = "02";


	if (l_pRSet->Next())
	{				
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMergeID = l_pRSet->GetValue("merge_id");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSeatNo = l_pRSet->GetValue("receipt_seatno");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strTitle = l_pRSet->GetValue("title");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContent = l_pRSet->GetValue("content");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strTime = l_pRSet->GetValue("time");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strActualOccurTime = l_pRSet->GetValue("actual_occur_time");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAddr = l_pRSet->GetValue("addr");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLongitude = l_pRSet->GetValue("longitude");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLatitude = l_pRSet->GetValue("latitude");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLevel = l_pRSet->GetValue("level");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSourceType = l_pRSet->GetValue("source_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSourceID = l_pRSet->GetValue("source_id");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strHandleType = l_pRSet->GetValue("handle_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFirstType = l_pRSet->GetValue("first_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSecondType = l_pRSet->GetValue("second_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strThirdType = l_pRSet->GetValue("third_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFourthType = l_pRSet->GetValue("fourth_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strVehicleNo = l_pRSet->GetValue("vehicle_no");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strVehicleType = l_pRSet->GetValue("vehicle_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSymbolCode = l_pRSet->GetValue("symbol_code");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSymbolAddr = l_pRSet->GetValue("symbol_addr");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFireBuildingType = l_pRSet->GetValue("fire_building_type");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strEventType = l_pRSet->GetValue("event_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCalledNoType = l_pRSet->GetValue("called_no_type");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strActualCalledNoType = l_pRSet->GetValue("actual_called_no_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerNo = l_pRSet->GetValue("caller_no");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerName = l_pRSet->GetValue("caller_name");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerAddr = l_pRSet->GetValue("caller_addr");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerID = l_pRSet->GetValue("caller_id");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerIDType = l_pRSet->GetValue("caller_id_type");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerGender = l_pRSet->GetValue("caller_gender");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerAge = l_pRSet->GetValue("caller_age");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerBirthday = l_pRSet->GetValue("caller_birthday");

		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactNo = l_pRSet->GetValue("contact_no");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactName = l_pRSet->GetValue("contact_name");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactAddr = l_pRSet->GetValue("contact_addr");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactID = l_pRSet->GetValue("contact_id");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactIDType = l_pRSet->GetValue("contact_id_type");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactGender = l_pRSet->GetValue("contact_gender");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactAge = l_pRSet->GetValue("contact_age");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactBirthday = l_pRSet->GetValue("contact_birthday");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptDistrictCode = l_pRSet->GetValue("admin_dept_district_code");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptCode = l_pRSet->GetValue("admin_dept_code");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptName = l_pRSet->GetValue("admin_dept_name");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptDistrictCode = l_pRSet->GetValue("receipt_dept_district_code");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptCode = l_pRSet->GetValue("receipt_dept_code");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptName = l_pRSet->GetValue("receipt_dept_name");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLeaderCode = l_pRSet->GetValue("leader_code");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLeaderName = l_pRSet->GetValue("leader_name");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptCode = l_pRSet->GetValue("receipt_code");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptName = l_pRSet->GetValue("receipt_name");
		////l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strDispatchSuggestion = l_pRSet->GetValue("dispatch_suggestion");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strIsMerge = l_pRSet->GetValue("is_merge");

		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCreateUser = l_pRSet->GetValue("create_user");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCreateTime = l_pRSet->GetValue("create_time");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strUpdateUser = l_pRSet->GetValue("update_user");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strUpdateTime = l_pRSet->GetValue("update_time");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strPrivacy = l_pRSet->GetValue("is_privacy");
		//l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strRemark = l_pRSet->GetValue("remark");
		if (!l_oAlarmSync.m_oBody.m_oAlarmInfo.ParseAlarmRecord(l_pRSet))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}

	}

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::_BackAlarmHandle(PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{

	if (p_oRequest.m_oBody.m_oData.m_strBackDeptID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "dept code is null!");
		return false;
	}

	//1.获取processid
	std::string strProcessId = _QueryProcessIdByAlarmIdAndProcessDeptCode(p_oRequest, strTransGuid);
	if (strProcessId.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not find process id!");
		return false;
	}

	p_oRequest.m_oBody.m_oData.m_strProcessID = strProcessId;

	// 2.判断当前处警单状态是否为已签收及之前的状态
	if (!_IsStateBeforAccepted(p_oRequest.m_oBody.m_oData.m_strProcessID, strTransGuid))
	{
		return false;
	}

	// 3.如果当前处警单状态为已签收之前的状态，则更新处警单状态为"已退警"
	if (!_UpdateProcessState(p_oRequest, strTransGuid))
	{
		return false;
	}

	// 4.更新接警单状态为"处警中"
	if (!_UpdateAlarmState(p_oRequest, strTransGuid))
	{
		return false;
	}

	// 5.记录退单原因
	if (!_InsertBackReason(p_oRequest, strTransGuid))
	{
		return false;
	}

	//6.记录流水并发送流水同步
	if (!_AddAlarmLogInfo(p_oRequest, strTransGuid))
	{
		return false;
	}

	_SynAlarmInfo(p_oRequest.m_oBody.m_oData.m_strAlarmID);

	return true;
}

void CBusinessImpl::AppBackAlarmRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string strMessage = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());
	// 解析请求消息
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAppBackAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse message error:[%s]", strMessage.c_str());
		return;
	}

	bool l_bIsSucess(false);
	// 退单处理
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	if (_BackAlarmHandle(l_oRequest, l_strGuid))
	{
		l_bIsSucess = true;
	}
	l_bIsSucess ? m_pDBConn->Commit(l_strGuid) : m_pDBConn->Rollback(l_strGuid);

	// 发送退单申请应答
	PROTOCOL::CAppBackAlarmRespond l_oRespond;
	_BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader, "app_back_alarm_respond");
	l_oRespond.m_oBody.m_strResult = (l_bIsSucess ? "0" : "1");
	JsonParser::IJsonPtr l_pIJsonResponse = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSendMsg = l_oRespond.ToString(l_pIJsonResponse);
	p_pRequest->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	
}

void CBusinessImpl::AlarmFeedbackSync(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received FeedbackSync From ICC: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CFeedBackSync l_CFeedBackSync;

	if (!l_CFeedBackSync.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	l_CFeedBackSync.m_oHeader.m_strCmd = SYNTHETICAL_FEEDBACK_INFO;
	l_CFeedBackSync.m_oHeader.m_strRequest = ICC_UPDATE_FEEDBACK;
	l_CFeedBackSync.m_oHeader.m_strRequestType = "0";
	l_CFeedBackSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CFeedBackSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_CFeedBackSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));

	ICC_LOG_DEBUG(m_pLog, "send feedback to app [%s]", l_strMsg.c_str());
}

void CBusinessImpl::AppEditFeedback(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received Edit Feedback from APP[%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CFeedBackSync l_Feedback;

	if (!l_Feedback.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	l_Feedback.m_oHeader.m_strCmd = ADD_OR_UPDATE_FEEDBACK_REQUEST;
	l_Feedback.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_Feedback.m_oHeader.m_strRequestType = "0";
	l_Feedback.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_Feedback.m_oHeader.m_strRelatedID = l_Feedback.m_oHeader.m_strMsgid;
	l_Feedback.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_Feedback.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond, p_pRequest));
}

void CBusinessImpl::AppGetFeedback(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received SntheticalFeedback APP[%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetFeedBackByProcessRequest AppRequest;
 
	if (!AppRequest.ParseStringFromAPP(p_strMsg, l_pIJson))
 	{
 		return;
 	}

	//查询记录总数
	DataBase::SQLRequest l_tSQLReqCnt;
	l_tSQLReqCnt.sql_id = SELECT_ID_FROM_ICC_T_ALARM_PROCESS;
	l_tSQLReqCnt.param["alarm_id"] = AppRequest.m_oBody.m_strAlarmId;
	l_tSQLReqCnt.param["process_dept_code"] = AppRequest.m_oBody.m_strProcessDeptCode;
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

		return;
	}
	if (!l_result->Next())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return ;
	}
	std::string strProcessId = l_result->GetValue("id");

	PROTOCOL::CGetFeedBackByProcessRequest ICCRequest;
	//ICCRequest.m_oHeader.m_strMsgid = AppRequest.m_oHeader.m_strMsgid;
	//ICCRequest.m_oHeader.m_strRelatedID = AppRequest.m_oHeader.m_strMsgid;
	ICCRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	ICCRequest.m_oHeader.m_strRelatedID = ICCRequest.m_oHeader.m_strMsgid;
	ICCRequest.m_oHeader.m_strCmd = GET_ALARM_ALL_INFO_REQUEST;
	ICCRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	ICCRequest.m_oHeader.m_strRequest = QUEUE_ALARM;
	ICCRequest.m_oHeader.m_strRequestType = "0";
	//处警id
	ICCRequest.m_oBody.m_strAlarmId = AppRequest.m_oBody.m_strAlarmId;
	ICCRequest.m_oBody.m_strProcessID = strProcessId;

	std::string l_strMsg = ICCRequest.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond, p_pRequest));
}

void CBusinessImpl::AddOrUpdateFeedbackRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Received SntheticalFeedback Respond From ICC[%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSetFeedBackRespond l_FeedBack;
	
	if (!l_FeedBack.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	l_FeedBack.m_oHeader.m_strCmd = APP_EDIT_FEEDBACK_RESPOND;
	l_FeedBack.m_oHeader.m_strRequest = ICC_UPDATE_FEEDBACK;
	l_FeedBack.m_oHeader.m_strRequestType = "0";
	l_FeedBack.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_FeedBack.m_oHeader.m_strRelatedID = l_FeedBack.m_oHeader.m_strMsgid;
	l_FeedBack.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_FeedBack.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pRequest->Response(l_strMsg);
}

void CBusinessImpl::GetFeedbackRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Get SntheticalFeedback From ICC[%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetFeedBackByProcessRespond l_FeedBack;

	if (!l_FeedBack.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}
	//不存在反馈单ID，APP对接查询结果补上
	if (l_FeedBack.m_oBody.m_strID.empty())
	{
		l_FeedBack.m_oBody.m_strID = m_pDateTime->CreateSerial();
	}

	l_FeedBack.m_oHeader.m_strCmd = APP_GET_FEEDBACK_RESPOND;
	l_FeedBack.m_oHeader.m_strRequest = ICC_UPDATE_FEEDBACK;
	l_FeedBack.m_oHeader.m_strRequestType = "0";
	l_FeedBack.m_oHeader.m_strRelatedID = l_FeedBack.m_oHeader.m_strMsgid;
	l_FeedBack.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_FeedBack.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_FeedBack.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pRequest->Response(l_strMsg);
}

void CBusinessImpl::OnMpaEditAlarmRequest(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received edit alarm from app[%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CEditAlarm l_EditAlarm;
	if (!l_EditAlarm.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}	

	//走警情提交的流程，修改结果由警情同步触发
	l_EditAlarm.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
	l_EditAlarm.m_oHeader.m_strRequest = QUEUE_ALARM;
	l_EditAlarm.m_oHeader.m_strRequestType = "0";
	l_EditAlarm.m_oHeader.m_strMsgid = l_EditAlarm.m_oHeader.m_strMsgid;//m_pString->CreateGuid();
	l_EditAlarm.m_oHeader.m_strRelatedID = l_EditAlarm.m_oHeader.m_strMsgid;
	l_EditAlarm.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	//标识是从mpa发起的警情变更
	l_EditAlarm.m_oBody.m_strMsgSource = "mpa";

	m_mapGuidAndCmds.insert(std::make_pair(l_EditAlarm.m_oHeader.m_strMsgid, "mpa_edit_alarm_request"));
	ICC_LOG_DEBUG(m_pLog, "cmd map size :[%d]", m_mapGuidAndCmds.size());

	std::string l_strMsg = l_EditAlarm.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond, p_pRequest));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnMpaEditAlarmRespond(ObserverPattern::INotificationPtr p_pRequest)
{
	std::string p_strMsg = p_pRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received alarmsync from icc: [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;

	if (!l_oRespond.ParseString(p_strMsg, l_pIJson))
	{
		return;
	}

	std::string strCmd;
	if (m_mapGuidAndCmds.find(l_oRespond.m_oHeader.m_strRelatedID) != m_mapGuidAndCmds.end())
	{
		strCmd = (m_mapGuidAndCmds.find(l_oRespond.m_oHeader.m_strRelatedID))->second;
		if (!strCmd.empty())
		{
			m_mapGuidAndCmds.erase(strCmd);
			ICC_LOG_DEBUG(m_pLog, "cmd map size :[%d]", m_mapGuidAndCmds.size());
		}
	}	
	
	if (strCmd == "app_comit_alarm_request")
	{
		PROTOCOL::CAddOrUpdateAlarmWithProcessRespond response;
		response.m_oHeader.m_strMsgid = l_oRespond.m_oHeader.m_strMsgid;
		response.m_oHeader.m_strMsgid = l_oRespond.m_oHeader.m_strRelatedID;
		response.m_oHeader.m_strCmd = "app_comit_alarm_respond";
		response.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		response.m_oHeader.m_strRequest = QUEUE_ALARM;
		response.m_oHeader.m_strRequestType = "0";

		//response.m_oBody.m_strResult = l_oRespond.m_oBody.m_strResult;

		JsonParser::IJsonPtr l_pIJsonResponse = ICCGetIJsonFactory()->CreateJson();
		std::string l_strSendMsg = response.ToString(l_pIJsonResponse);
		p_pRequest->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
	else if (strCmd == "mpa_edit_alarm_request")
	{
		l_oRespond.m_oHeader.m_strCmd = "mpa_edit_alarm_respond";
		l_oRespond.m_oHeader.m_strRequest = "queue_alarm_to_icc";
		l_oRespond.m_oHeader.m_strRequestType = "0";
		l_oRespond.m_oHeader.m_strMsgid = l_oRespond.m_oHeader.m_strMsgid;//m_pString->CreateGuid();
		l_oRespond.m_oHeader.m_strRelatedID = l_oRespond.m_oHeader.m_strRelatedID;
		l_oRespond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

		std::string l_strMsg = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		p_pRequest->Response(l_strMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

bool CBusinessImpl::_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo)
{
	std::string strStaffInfo;
	if (m_strCodeMode == STAFF_ID_NO)
	{
		if (!m_pRedisClient->HGet("StaffIdInfo", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffIdInfo failed,staff_id_no:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	else if (m_strCodeMode == STAFF_CODE)
	{
		if (!m_pRedisClient->HGet("StaffInfoMap", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed,staff_code:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	if (!l_oStaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse staff info failed!!!");
		return false;
	}
	return true;
}

std::string	CBusinessImpl::_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName)
{
	std::string strEndStaffName;
	if (strStaffType == "JZLX101")
	{
		strEndStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), strStaffName.c_str());
	}
	else
	{
		strEndStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), strStaffName.c_str());
	}
	return strEndStaffName;
}