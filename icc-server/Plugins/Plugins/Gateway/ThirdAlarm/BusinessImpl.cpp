#include "Boost.h"
#include "BusinessImpl.h"

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
{
	g_pBusinessImpl = this;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_THIRDALARM_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/ThirdAlarm/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_GET_ALARM_BY_GUID_REQUEST, OnNotifiTAGetAlarmByGuidRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_GET_ALARM_REQUEST, OnNotifiTAGetAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_ADD_ALARM_REQUEST, OnNotifiTAAddAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_GET_NOT_READ_COUNT_REQUEST, OnNotifiTAGetNotReadCountRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_SET_STATE_REQUEST, OnNotifiTASetStateRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_SET_CHANGEINFO_STATE_REQUEST, OnNotifiTASetChangeInfoStateRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_GET_CHANGEINFO_REQUEST, OnNotifiTAGetChangeInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_SET_CHANGEINFO_REQUEST, OnNotifiTASetChangeInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_GET_LOCATION_REQUEST, OnNotifiGetLocationRequest);

	m_mapCmdName[TA_GET_ALARM_BY_GUID_REQUEST] = TA_GET_ALARM_BY_GUID_RESPOND;
	m_mapCmdName[TA_GET_ALARM_REQUEST] = TA_GET_ALARM_RESPOND;
	m_mapCmdName[TA_ADD_ALARM_REQUEST] = TA_ADD_ALARM_RESPOND;
	m_mapCmdName[TA_GET_NOT_READ_COUNT_REQUEST] = TA_GET_NOT_READ_COUNT_RESPOND;
	m_mapCmdName[TA_SET_STATE_REQUEST] = TA_SET_STATE_RESPOND;
	m_mapCmdName[TA_SET_CHANGEINFO_STATE_REQUEST] = TA_SET_CHANGEINFO_STATE_RESPOND;
	m_mapCmdName[TA_GET_CHANGEINFO_REQUEST] = TA_GET_CHANGEINFO_RESPOND;
	m_mapCmdName[TA_SET_CHANGEINFO_REQUEST] = TA_SET_CHANGEINFO_RESPOND;
	m_mapCmdName[TA_GET_LOCATION_REQUEST] = TA_GET_LOCATION_RESPOND;

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiTAGetAlarmByGuidRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAGetAlarmByGuidRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTAGetAlarmByGuidRespond l_oRespond;
	/*bool l_bRes = */BuildTAGetAlarmByGuidRespond(l_oRespond, l_oRequest);

	//if (l_bRes)
	//{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	//}
}

void CBusinessImpl::OnNotifiTAGetAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAGetAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTAGetAlarmRespond l_oRespond;
	/*bool l_bRes = */BuildTAGetAlarmRespond(l_oRespond, l_oRequest);

	/*if (l_bRes)
	{*/
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	//}
}

void CBusinessImpl::OnNotifiTAAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAAddAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTAAddAlarmRespond l_oRespond;
	bool l_bRes = BuildTAAddAlarmRespond(l_oRespond, l_oRequest);

	
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		// 发送第三方警情同步
		SendTAAlarmSync(l_oRequest.m_oBody.m_strGuid, ADD);

		// 发送未读警情数量同步
		SendTANotReadCountSync(l_oRequest.m_oBody.m_strGuid);
	}
}

void CBusinessImpl::OnNotifiTAGetNotReadCountRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAGetNotReadCountRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTAGetNotReadCountRespond l_oRespond;
	/*bool l_bRes =*/ BuildTAGetNotReadCountRespond(l_oRespond, l_oRequest);

	
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	
}

void CBusinessImpl::OnNotifiTASetStateRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTASetStateRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTASetStateRespond l_oRespond;
	bool l_bRes = BuildTASetStateRespond(l_oRespond, l_oRequest);

	
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		// 发送第三方未读数量同步
		SendTANotReadCountSync(l_oRequest.m_oBody.m_strGuid);

		// 发送第三方状态同步[客户端与对应的第三方网关都需要该消息]
		SendTAStateSync(l_oRequest.m_oBody.m_strGuid, l_oRequest.m_oBody.m_strState);

		// 发送交互未读数量同步
		SendTAChangeInfoNotReadCountSync(l_oRequest.m_oBody.m_strGuid,THIRDALARM);
	}
}

void CBusinessImpl::OnNotifiTASetChangeInfoStateRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTASetChangeInfoStateRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTASetChangeInfoStateRespond l_oRespond;
	bool l_bRes = BuildTASetChangeInfoStateRespond(l_oRespond, l_oRequest);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

		// 发送交互未读数量同步
		SendTAChangeInfoNotReadCountSync(l_oRequest.m_oBody.m_strGuid,CHANGEINFO);

		// 发送交互状态同步
		SendTAChangeInfoStateSync(l_oRequest.m_oBody.m_strGuid, l_oRequest.m_oBody.m_strState);
	}
}

void CBusinessImpl::OnNotifiTAGetChangeInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAGetChangeInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CTAGetChangeInfoRespond l_oRespond;
	/*bool l_bRes = */BuildTAGetChangeInfoRespond(l_oRespond, l_oRequest);

	
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	
}

void CBusinessImpl::OnNotifiTASetChangeInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTASetChangeInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	m_strChangeInfoGuid = m_pString->CreateGuid();

	PROTOCOL::CTASetChangeInfoRespond l_oRespond;
	bool l_bRes = BuildTASetChangeInfoRespond(l_oRespond, l_oRequest);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		// 发送交互信息同步[客户端与对应的第三方网关都需要该消息]
		SendTAChangeInfoSync(l_oRequest.m_oBody.m_strThirdAlarmGuid, ADD);

		// 发送未读交互数量同步
		SendTAChangeInfoNotReadCountSync(m_strChangeInfoGuid, CHANGEINFO);
	}
}

void CBusinessImpl::OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTAGetLocationRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发定位请求到SmsLJ
	bool l_bRes = TransmitLocationRequest(l_oRequest);

	// 发送回复消息
	PROTOCOL::CTAGetLocationRespond l_oRespond;
	BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader);
	l_oRespond.m_oBody.m_strResult = (l_bRes ? "0" : "1");

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::SendTAAlarmSync(std::string p_strGuid, SyncType p_SyncType)
{
	PROTOCOL::CTAAlarmSync l_oSync;

	l_oSync.m_oBody.m_strSyncType = m_pString->Number(p_SyncType);
	bool l_bRes = BuildTAAlarmSync(l_oSync, p_strGuid);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::SendTANotReadCountSync(std::string p_strGuid)
{
	PROTOCOL::CTANotReadCountSync l_oSync;

	bool l_bRes = BuildTANotReadCountSync(l_oSync, p_strGuid);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::SendTAStateSync(std::string p_strGuid, std::string p_strState)
{
	PROTOCOL::CTAStateSync l_oSync;

	bool l_bRes = BuildTAStateSync(l_oSync, p_strGuid, p_strState);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::SendTAChangeInfoSync(std::string p_strGuid, SyncType p_SyncType)
{
	PROTOCOL::CTAChangeInfoSync l_oSync;

	l_oSync.m_oBody.m_strSyncType = m_pString->Number(p_SyncType);
	bool l_bRes = BuildTAChangeInfoSync(l_oSync, p_strGuid);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::SendTAChangeInfoNotReadCountSync(std::string p_strGuid, IDType p_eIDTYpe)
{
	PROTOCOL::CTAChangeInfoNotReadCountSync l_oSync;

	bool l_bRes = BuildTAChangeInfoNotReadCountSync(l_oSync, p_strGuid, p_eIDTYpe);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::SendTAChangeInfoStateSync(std::string p_strGuid, std::string p_strState)
{
	PROTOCOL::CTAChangeInfoStateSync l_oSync;

	bool l_bRes = BuildTAChangeInfoStateSync(l_oSync, p_strGuid, p_strState);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

bool CBusinessImpl::TransmitLocationRequest(PROTOCOL::CTAGetLocationRequest p_oRequest)
{
	p_oRequest.m_oHeader.m_strSystemID = SYSTEMID;
	p_oRequest.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRequest.m_oHeader.m_strRelatedID = "";
	p_oRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRequest.m_oHeader.m_strCmd = SMS_GET_LOCATION_REQUEST;
	p_oRequest.m_oHeader.m_strRequest = ICC_QUEUE_SMS;
	p_oRequest.m_oHeader.m_strRequestType = "1";
	p_oRequest.m_oHeader.m_strResponse = "";
	p_oRequest.m_oHeader.m_strResponseType = "";

	std::string l_strSendMsg = p_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	return true;
}

bool CBusinessImpl::IsBlackAlarmTTL(std::string p_strPhone)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_ICC_T_BLACKLIST;
	l_Sql.param["limit_num"] = p_strPhone;
	l_Sql.param["is_delete"] = BLACKLIST_DELETE_FLAG;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();
	std::string l_strEndTime = "";
	// 获取相关信息
	if (l_pResult->Next())
	{
		l_strEndTime = l_pResult->GetValue("end_time");
	}

	if (l_strCurTime.compare(l_strEndTime)>=0)
	{
		ICC_LOG_DEBUG(m_pLog, "[%s] not in backlist TTL", p_strPhone.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "[%s] in backlist TTL", p_strPhone.c_str());
	return true;
}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader)
{
	p_oRespondHeader.m_strSystemID = SYSTEMID;
	p_oRespondHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = m_mapCmdName[p_oRequestHeader.m_strCmd];
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oSyncHeader, std::string p_strCmd)
{
	p_oSyncHeader.m_strSystemID = SYSTEMID;
	p_oSyncHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oSyncHeader.m_strMsgid = m_pString->CreateGuid();
	p_oSyncHeader.m_strRelatedID = "";
	p_oSyncHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oSyncHeader.m_strCmd = p_strCmd;
	p_oSyncHeader.m_strRequest = ICC_TOPIC_THIRD_ALARM;
	p_oSyncHeader.m_strRequestType = "1";
	p_oSyncHeader.m_strResponse = "";
	p_oSyncHeader.m_strResponseType = "";
}

bool CBusinessImpl::BuildTAGetAlarmByGuidRespond(PROTOCOL::CTAGetAlarmByGuidRespond& p_oRespond, const PROTOCOL::CTAGetAlarmByGuidRequest& p_oRequest)
{
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;

	if (!SelectThirdAlarmEx(p_oRespond.m_oBody.m_oThirdAlarm, p_oRequest.m_oBody.m_strGuid))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "select third alarm failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAGetAlarmRespond(PROTOCOL::CTAGetAlarmRespond& p_oRespond, const PROTOCOL::CTAGetAlarmRequest& p_oRequest)
{
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;

	if (!SelectThirdAlarmEx(p_oRespond, p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "select third alarm failed";
		return false;
	}
	
	return true;
}

bool CBusinessImpl::BuildTAAddAlarmRespond(PROTOCOL::CTAAddAlarmRespond& p_oRespond, const PROTOCOL::CTAAddAlarmRequest& p_oRequest)
{
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;
	if (IsBlackAlarmTTL(p_oRequest.m_oBody.m_strUserPhone))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "black alarm ttl" + p_oRequest.m_oBody.m_strUserPhone;
		return false;
	}

	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!InsertThirdAlarm(p_oRespond, p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "insert third alarm failed" ;
		return false;
	}

	if (!InsertThirdChangeInfoEx(p_oRespond, p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "insert third change info failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAGetNotReadCountRespond(PROTOCOL::CTAGetNotReadCountRespond& p_oRespond, const PROTOCOL::CTAGetNotReadCountRequest& p_oRequest)
{
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	p_oRespond.m_oBody.m_strCurrDeptCode = p_oRequest.m_oBody.m_strCurrDeptCode;
	if (!SelectThirdAlarmNCount(p_oRespond.m_oBody.m_strCount, p_oRequest.m_oBody.m_strCurrDeptCode))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "select third alarm count failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTASetStateRespond(PROTOCOL::CTASetStateRespond& p_oRespond, const PROTOCOL::CTASetStateRequest& p_oRequest)
{
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!UpdateThirdAlarm(p_oRespond,p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "update third alarm failed";
		return false;
	}

	if (!UpdateThirdChangeInfo(p_oRespond,p_oRequest.m_oBody.m_strGuid))
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "update third change info failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTASetChangeInfoStateRespond(PROTOCOL::CTASetChangeInfoStateRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoStateRequest& p_oRequest)
{
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!UpdateThirdChangeInfo(p_oRespond, p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "update third change info failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAGetChangeInfoRespond(PROTOCOL::CTAGetChangeInfoRespond& p_oRespond, const PROTOCOL::CTAGetChangeInfoRequest& p_oRequest)
{
	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);
	p_oRespond.m_oHeader = p_oRequest.m_oHeader; 

	if (!SelectThirdChangeinfoEx(p_oRespond, p_oRequest.m_oBody.m_strThridAlarmGuid))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "select third change info failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTASetChangeInfoRespond(PROTOCOL::CTASetChangeInfoRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoRequest& p_oRequest)
{
	p_oRespond.m_oHeader = p_oRequest.m_oHeader;
	
	if (IsBlackAlarmTTL(p_oRequest.m_oBody.m_strPhone))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "black alarm ttl "+ p_oRequest.m_oBody.m_strPhone;
		return false;
	}

	//BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!InsertThirdChangeInfo(p_oRespond, p_oRequest))
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "insert third change info failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAAlarmSync(PROTOCOL::CTAAlarmSync& p_oSync, const std::string p_strGuid)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_ALARM_SYNC);

	if (!SelectThirdAlarmEx(p_oSync.m_oBody.m_oThirdAlarm, p_strGuid))
	{
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTANotReadCountSync(PROTOCOL::CTANotReadCountSync& p_oSync, const std::string p_strGuid)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_NOT_READ_COUNT_SYNC);

	PROTOCOL::CThirdAlarm l_oThirdAlarm;
	if (!SelectThirdAlarm(l_oThirdAlarm, p_strGuid))
	{
		return false;
	}
	p_oSync.m_oBody.m_strCurrDeptCode = l_oThirdAlarm.m_strToOrgID;

	if (!SelectThirdAlarmNCount(p_oSync.m_oBody.m_strCount, p_oSync.m_oBody.m_strCurrDeptCode))
	{
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAStateSync(PROTOCOL::CTAStateSync& p_oSync, std::string p_strGuid, std::string p_strState)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_STATE_SYNC);

	p_oSync.m_oBody.m_strGuid = p_strGuid;
	p_oSync.m_oBody.m_strState = p_strState;

	return true;
}

bool CBusinessImpl::BuildTAChangeInfoSync(PROTOCOL::CTAChangeInfoSync& p_oSync, const std::string p_strGuid)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_CHANGEINFO_SYNC);

	PROTOCOL::CThirdAlarm l_oThirdAlarm;
	if (!SelectThirdAlarm(l_oThirdAlarm, p_strGuid))
	{
		return false;
	}
	p_oSync.m_oBody.m_strThirdType = l_oThirdAlarm.m_strType;

	if (!SelectThirdChangeinfo(p_oSync, m_strChangeInfoGuid))
	{
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAChangeInfoNotReadCountSync(PROTOCOL::CTAChangeInfoNotReadCountSync& p_oSync, const std::string p_strGuid, IDType p_eIDTYpe)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_CHANGEINFO_NOT_READ_COUNT_SYNC);

	if (THIRDALARM ==p_eIDTYpe)
	{
		p_oSync.m_oBody.m_strThirdAlarmGuid = p_strGuid;
	} 
	else
	{
		if (!SelectThirdChangeinfoTAGuid(p_oSync.m_oBody.m_strThirdAlarmGuid, p_strGuid))
		{
			return false;
		}
	}

	if (!SelectThirdChangeinfoNCount(p_oSync.m_oBody.m_strCount, p_oSync.m_oBody.m_strThirdAlarmGuid))
	{
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildTAChangeInfoStateSync(PROTOCOL::CTAChangeInfoStateSync& p_oSync, std::string p_strGuid, std::string p_strState)
{
	BuildRespondHeader(p_oSync.m_oHeader, TA_CHANGEINFO_STATE_SYNC);

	p_oSync.m_oBody.m_strGuid = p_strGuid;
	p_oSync.m_oBody.m_strState = p_strState;

	return true;
}

bool CBusinessImpl::SelectThirdAlarm(PROTOCOL::CThirdAlarm& p_oThirdAlarm, const std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{
		p_oThirdAlarm.m_strGuid = l_pResult->GetValue("guid");
		p_oThirdAlarm.m_strType = l_pResult->GetValue("type");
		p_oThirdAlarm.m_strAddress = l_pResult->GetValue("address");
		p_oThirdAlarm.m_strCrcontent = l_pResult->GetValue("content");
		p_oThirdAlarm.m_strIsPhone = l_pResult->GetValue("is_phone");
		p_oThirdAlarm.m_strCreateDate = l_pResult->GetValue("create_date");
		p_oThirdAlarm.m_strUserCode = l_pResult->GetValue("user_code");
		p_oThirdAlarm.m_strUserName = l_pResult->GetValue("user_name");
		p_oThirdAlarm.m_strUserPhone = l_pResult->GetValue("user_phone");
		p_oThirdAlarm.m_strUserAddress = l_pResult->GetValue("user_address");
		p_oThirdAlarm.m_strUnitContactor = l_pResult->GetValue("unit_contactor");
		p_oThirdAlarm.m_strHandPhone = l_pResult->GetValue("hand_phone");
		p_oThirdAlarm.m_strToOrgID = l_pResult->GetValue("to_org_id");
		p_oThirdAlarm.m_strToOrgName = l_pResult->GetValue("to_org_name");
		p_oThirdAlarm.m_strToUserID = l_pResult->GetValue("to_user_id");
		p_oThirdAlarm.m_strToUserName = l_pResult->GetValue("to_user_name");
		p_oThirdAlarm.m_strState = l_pResult->GetValue("state");
		p_oThirdAlarm.m_strComNo = l_pResult->GetValue("com_no");
		p_oThirdAlarm.m_strOpenID = l_pResult->GetValue("open_id");
		p_oThirdAlarm.m_strLong = l_pResult->GetValue("long");
		p_oThirdAlarm.m_strLat = l_pResult->GetValue("lat");
		p_oThirdAlarm.m_strRemark1 = l_pResult->GetValue("remark1");
		p_oThirdAlarm.m_strRemark2 = l_pResult->GetValue("remark2");
	}

	return true;
}

bool CBusinessImpl::SelectThirdAlarmEx(PROTOCOL::CThirdAlarm& p_oThirdAlarm, const std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{		
		p_oThirdAlarm.m_strGuid = l_pResult->GetValue("guid");
		p_oThirdAlarm.m_strType = l_pResult->GetValue("type");
		p_oThirdAlarm.m_strAddress = l_pResult->GetValue("address");
		p_oThirdAlarm.m_strCrcontent = l_pResult->GetValue("content");
		p_oThirdAlarm.m_strIsPhone = l_pResult->GetValue("is_phone");
		p_oThirdAlarm.m_strCreateDate = l_pResult->GetValue("create_date");
		p_oThirdAlarm.m_strUserCode = l_pResult->GetValue("user_code");
		p_oThirdAlarm.m_strUserName = l_pResult->GetValue("user_name");
		p_oThirdAlarm.m_strUserPhone = l_pResult->GetValue("user_phone");
		p_oThirdAlarm.m_strUserAddress = l_pResult->GetValue("user_address");
		p_oThirdAlarm.m_strUnitContactor = l_pResult->GetValue("unit_contactor");
		p_oThirdAlarm.m_strHandPhone = l_pResult->GetValue("hand_phone");
		p_oThirdAlarm.m_strToOrgID = l_pResult->GetValue("to_org_id");
		p_oThirdAlarm.m_strToOrgName = l_pResult->GetValue("to_org_name");
		p_oThirdAlarm.m_strToUserID = l_pResult->GetValue("to_user_id");
		p_oThirdAlarm.m_strToUserName = l_pResult->GetValue("to_user_name");
		p_oThirdAlarm.m_strState = l_pResult->GetValue("state");
		p_oThirdAlarm.m_strComNo = l_pResult->GetValue("com_no");
		p_oThirdAlarm.m_strOpenID = l_pResult->GetValue("open_id");
		p_oThirdAlarm.m_strLong = l_pResult->GetValue("long");
		p_oThirdAlarm.m_strLat = l_pResult->GetValue("lat");
		p_oThirdAlarm.m_strRemark1 = l_pResult->GetValue("remark1");
		p_oThirdAlarm.m_strRemark2 = l_pResult->GetValue("remark2");

		// 获取未读交互信息数量
		if (!SelectThirdChangeinfoNCount(p_oThirdAlarm.m_strNotReadChangeCount, p_oThirdAlarm.m_strGuid))
		{
			return false;
		}

		// 获取交互信息
		if (!SelectThirdChangeinfoEx(p_oThirdAlarm.m_vecMaterial, p_oThirdAlarm.m_strGuid))
		{
			return false;
		}
	}

	return true;
}

bool CBusinessImpl::SelectThirdAlarmEx(PROTOCOL::CTAGetAlarmRespond& p_oRespond, const PROTOCOL::CTAGetAlarmRequest& p_oRequest)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM_EX;
	l_Sql.param["page_size"] = p_oRequest.m_oBody.m_strPageSize;
	l_Sql.param["page_index"] = p_oRequest.m_oBody.m_strPageIndex;
	l_Sql.param["begin_time"] = p_oRequest.m_oBody.m_strBeginTime;
	l_Sql.param["end_time"] = p_oRequest.m_oBody.m_strEndTime;
	l_Sql.param["curr_dept_code"] = p_oRequest.m_oBody.m_strCurrDeptCode;
	l_Sql.param["keyword"] = p_oRequest.m_oBody.m_strKeyword;
	if (!p_oRequest.m_oBody.m_strState.empty())
	{
		l_Sql.param["type"] = p_oRequest.m_oBody.m_strState;
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM_EX, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取结果数量
	if (!SelectThirdAlarmCount(p_oRespond.m_oBody.m_strCount, p_oRequest))
	{
		return false;
	}

	// 获取第三方警情信息
	while (l_pResult->Next())
	{
		PROTOCOL::CThirdAlarm l_oThirdAlarm;	
		l_oThirdAlarm.m_strGuid = l_pResult->GetValue("guid");
		l_oThirdAlarm.m_strType = l_pResult->GetValue("type");
		l_oThirdAlarm.m_strAddress = l_pResult->GetValue("address");
		l_oThirdAlarm.m_strCrcontent = l_pResult->GetValue("content");
		l_oThirdAlarm.m_strIsPhone = l_pResult->GetValue("is_phone");
		l_oThirdAlarm.m_strCreateDate = l_pResult->GetValue("create_date");
		l_oThirdAlarm.m_strUserCode = l_pResult->GetValue("user_code");
		l_oThirdAlarm.m_strUserName = l_pResult->GetValue("user_name");
		l_oThirdAlarm.m_strUserPhone = l_pResult->GetValue("user_phone");
		l_oThirdAlarm.m_strUserAddress = l_pResult->GetValue("user_address");
		l_oThirdAlarm.m_strUnitContactor = l_pResult->GetValue("unit_contactor");
		l_oThirdAlarm.m_strHandPhone = l_pResult->GetValue("hand_phone");
		l_oThirdAlarm.m_strToOrgID = l_pResult->GetValue("to_org_id");
		l_oThirdAlarm.m_strToOrgName = l_pResult->GetValue("to_org_name");
		l_oThirdAlarm.m_strToUserID = l_pResult->GetValue("to_user_id");
		l_oThirdAlarm.m_strToUserName = l_pResult->GetValue("to_user_name");
		l_oThirdAlarm.m_strState = l_pResult->GetValue("state");
		l_oThirdAlarm.m_strComNo = l_pResult->GetValue("com_no");
		l_oThirdAlarm.m_strOpenID = l_pResult->GetValue("open_id");
		l_oThirdAlarm.m_strLong = l_pResult->GetValue("long");
		l_oThirdAlarm.m_strLat = l_pResult->GetValue("lat");
		l_oThirdAlarm.m_strRemark1 = l_pResult->GetValue("remark1");
		l_oThirdAlarm.m_strRemark2 = l_pResult->GetValue("remark2");

		// 获取未读交互信息数量
		if (!SelectThirdChangeinfoNCount(l_oThirdAlarm.m_strNotReadChangeCount, l_oThirdAlarm.m_strGuid))
		{
			return false;
		}

		// 获取交互信息
		if (!SelectThirdChangeinfoEx(l_oThirdAlarm.m_vecMaterial, l_oThirdAlarm.m_strGuid))
		{
			return false;
		}		

		p_oRespond.m_oBody.m_vecThirdAlarm.push_back(l_oThirdAlarm);
	}

	return true;
}

bool CBusinessImpl::SelectThirdAlarmCount(std::string& p_strCount, const PROTOCOL::CTAGetAlarmRequest& p_oRequest)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM_COUNT;
	l_Sql.param["begin_time"] = p_oRequest.m_oBody.m_strBeginTime;
	l_Sql.param["end_time"] = p_oRequest.m_oBody.m_strEndTime;
	l_Sql.param["curr_dept_code"] = p_oRequest.m_oBody.m_strCurrDeptCode;
	l_Sql.param["keyword"] = p_oRequest.m_oBody.m_strKeyword;
	if (!p_oRequest.m_oBody.m_strState.empty())
	{
		l_Sql.param["type"] = p_oRequest.m_oBody.m_strState;
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM_COUNT, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取未读数量
	if (l_pResult->Next())
	{		
		p_strCount = l_pResult->GetValue("num");
	}

	return true;
}

bool CBusinessImpl::SelectThirdAlarmNCount(std::string& p_strCount, const std::string p_strCurrDeptCode)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM_NCOUNT;
	
	l_Sql.param["curr_dept_code"] = p_strCurrDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM_NCOUNT, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取未读数量
	if (l_pResult->Next())
	{
		p_strCount = l_pResult->GetValue("num");
	}

	return true;
}

bool CBusinessImpl::SelectThirdChangeinfo(PROTOCOL::CTAChangeInfoSync& l_oSync, const std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_CHANGEINFO;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取附件相关信息
	if (l_pResult->Next())
	{
		l_oSync.m_oBody.m_strGuid = l_pResult->GetValue("guid");
		l_oSync.m_oBody.m_strThirdAlarmGuid = l_pResult->GetValue("third_alarm_guid");
		l_oSync.m_oBody.m_strOrientation = l_pResult->GetValue("orientation");
		l_oSync.m_oBody.m_strState = l_pResult->GetValue("state");
		l_oSync.m_oBody.m_strPhone = l_pResult->GetValue("phone");
		l_oSync.m_oBody.m_strOpenID = l_pResult->GetValue("open_id");
		l_oSync.m_oBody.m_strChangeType = l_pResult->GetValue("change_type");
		l_oSync.m_oBody.m_strChangeContent = l_pResult->GetValue("change_content");
		l_oSync.m_oBody.m_strChangeDate = l_pResult->GetValue("change_date");
		l_oSync.m_oBody.m_strLong = l_pResult->GetValue("long");
		l_oSync.m_oBody.m_strLat = l_pResult->GetValue("lat");
		l_oSync.m_oBody.m_strRemark1 = l_pResult->GetValue("remark1");
		l_oSync.m_oBody.m_strRemark2 = l_pResult->GetValue("remark2");
	}

	return true;
}

bool CBusinessImpl::SelectThirdChangeinfoEx(PROTOCOL::CTAGetChangeInfoRespond& p_oRespond, const std::string p_strThirdAlarmGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_CHANGEINFO;
	l_Sql.param["third_alarm_guid"] = p_strThirdAlarmGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取附件相关信息
	while (l_pResult->Next())
	{
		PROTOCOL::CTAGetChangeInfoRespond::CChangeInfo l_oChangeInfo;
		l_oChangeInfo.m_strGuid = l_pResult->GetValue("guid");
		l_oChangeInfo.m_strThirdAlarmGuid = l_pResult->GetValue("third_alarm_guid");
		l_oChangeInfo.m_strOrientation = l_pResult->GetValue("orientation");
		l_oChangeInfo.m_strState = l_pResult->GetValue("state");
		l_oChangeInfo.m_strPhone = l_pResult->GetValue("phone");
		l_oChangeInfo.m_strOpenID = l_pResult->GetValue("open_id");
		l_oChangeInfo.m_strChangeType = l_pResult->GetValue("change_type");
		l_oChangeInfo.m_strChangeContent = l_pResult->GetValue("change_content");
		l_oChangeInfo.m_strChangeDate = l_pResult->GetValue("change_date");
		l_oChangeInfo.m_strLong = l_pResult->GetValue("long");
		l_oChangeInfo.m_strLat = l_pResult->GetValue("lat");
		l_oChangeInfo.m_strRemark1 = l_pResult->GetValue("remark1");
		l_oChangeInfo.m_strRemark2 = l_pResult->GetValue("remark2");

		if (l_oChangeInfo.m_strOrientation == ORIENTATION_CONTECT_TO_POLICE 
			|| l_oChangeInfo.m_strOrientation ==ORIENTATION_POLICE_TO_CONTECT)
		{
			p_oRespond.m_oBody.m_vecChangeInfo.push_back(l_oChangeInfo);
		}
	}
	p_oRespond.m_oBody.m_strCount = m_pString->Number(p_oRespond.m_oBody.m_vecChangeInfo.size());

	return true;
}

bool CBusinessImpl::SelectThirdChangeinfoEx(std::vector<PROTOCOL::CMaterial>& p_vecMaterial, const std::string p_strThirdAlarmGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_CHANGEINFO;
	l_Sql.param["third_alarm_guid"] = p_strThirdAlarmGuid;
	l_Sql.param["orientation"] = ORIENTATION_FIRST;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取附件相关信息
	while (l_pResult->Next())
	{
		std::string l_strFGuid = l_pResult->GetValue("guid");
		if (!l_strFGuid.empty())
		{
			PROTOCOL::CMaterial l_oMaterial;
			l_oMaterial.m_strFGuid = l_pResult->GetValue("fguid");
			l_oMaterial.m_strFName = l_pResult->GetValue("fname");
			l_oMaterial.m_strAttachType = l_pResult->GetValue("change_type");
			l_oMaterial.m_strAttachPath = l_pResult->GetValue("change_content");
			p_vecMaterial.push_back(l_oMaterial);
		}
	}

	return true;
}

bool CBusinessImpl::SelectThirdChangeinfoNCount(std::string& p_strNotReadChangeCount, const std::string p_strThirdAlarmGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_CHANGEINFO_NCOUNT;
	l_Sql.param["third_alarm_guid"] = p_strThirdAlarmGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO_NCOUNT, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取未读数量
	if (l_pResult->Next())
	{		
		p_strNotReadChangeCount = l_pResult->GetValue("num");
	}

	return true;
}

bool CBusinessImpl::SelectThirdChangeinfoTAGuid(std::string& p_strTAGuid, const std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_CHANGEINFO;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取未读数量
	if (l_pResult->Next())
	{
		p_strTAGuid = l_pResult->GetValue("third_alarm_guid");
	}

	return true;
}

bool CBusinessImpl::UpdateThirdAlarm(PROTOCOL::CTASetStateRespond& p_oRespond, const PROTOCOL::CTASetStateRequest& p_oRequest)
{
	//p_oRespond.m_oBody.m_strResult = "1";

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = UPDATE_THIRD_ALARM;
	l_Sql.param["guid"] = p_oRequest.m_oBody.m_strGuid;

	l_Sql.set["state"] = p_oRequest.m_oBody.m_strState;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", UPDATE_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::UpdateThirdChangeInfo(PROTOCOL::CTASetChangeInfoStateRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoStateRequest& p_oRequest)
{
	//p_oRespond.m_oBody.m_strResult = "1";

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = UPDATE_THIRD_CHANGEINFO;

	l_Sql.param["guid"] = p_oRequest.m_oBody.m_strGuid;

	l_Sql.set["state"] = p_oRequest.m_oBody.m_strState;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", UPDATE_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool ICC::CBusinessImpl::UpdateThirdChangeInfo(PROTOCOL::CTASetStateRespond& p_oRespond, std::string l_strThirdAlarmID)
{
	//p_oRespond.m_oBody.m_strResult = "1";

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = UPDATE_THIRD_CHANGEINFO;

	l_Sql.param["third_alarm_guid"] = l_strThirdAlarmID;

	l_Sql.set["state"] = STATE_READ;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", UPDATE_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::InsertThirdChangeInfo(PROTOCOL::CTASetChangeInfoRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoRequest& p_oRequest)
{
	//p_oRespond.m_oBody.m_strResult = "1";	

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = INSERT_THIRD_CHANGEINFO;
	l_Sql.param["guid"] = m_strChangeInfoGuid;
	l_Sql.param["third_alarm_guid"] = p_oRequest.m_oBody.m_strThirdAlarmGuid;
	l_Sql.param["orientation"] = p_oRequest.m_oBody.m_strOrientation;
	l_Sql.param["state"] = p_oRequest.m_oBody.m_strState;
	l_Sql.param["phone"] = p_oRequest.m_oBody.m_strPhone;
	l_Sql.param["open_id"] = p_oRequest.m_oBody.m_strOpenID;
	l_Sql.param["change_type"] = p_oRequest.m_oBody.m_strChangeType;
	l_Sql.param["change_content"] = p_oRequest.m_oBody.m_strChangeContent;
	//std::string l_strTime = m_pDateTime->ToString(m_pDateTime->FromString(p_oRequest.m_oBody.m_strChangeDate));
	l_Sql.param["change_date"] = p_oRequest.m_oBody.m_strChangeDate;
	l_Sql.param["long"] = p_oRequest.m_oBody.m_strLong;
	l_Sql.param["lat"] = p_oRequest.m_oBody.m_strLat;
	l_Sql.param["remark1"] = p_oRequest.m_oBody.m_strRemark1;
	l_Sql.param["remark2"] = p_oRequest.m_oBody.m_strRemark2;

	// 如果客户端发送的交互，则服务端自动写入Phone与OpenID字段
	if (p_oRequest.m_oBody.m_strOrientation == ORIENTATION_POLICE_TO_CONTECT)
	{
		PROTOCOL::CThirdAlarm l_oThirdAlarm;
		if (!SelectThirdAlarm(l_oThirdAlarm, p_oRequest.m_oBody.m_strThirdAlarmGuid))
		{
			return false;
		}
		l_Sql.param["phone"] = l_oThirdAlarm.m_strUserPhone;
		l_Sql.param["open_id"] = l_oThirdAlarm.m_strOpenID;
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", INSERT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::InsertThirdChangeInfoEx(PROTOCOL::CTAAddAlarmRespond& p_oRespond,const PROTOCOL::CTAAddAlarmRequest& p_oRequest)
{
	//p_oRespond.m_oBody.m_strResult = "1";
	for (auto material : p_oRequest.m_oBody.m_vecMaterial)
	{
		DataBase::SQLRequest l_Sql;
		l_Sql.sql_id = INSERT_THIRD_CHANGEINFO;
		l_Sql.param["guid"] = m_pString->CreateGuid();
		l_Sql.param["third_alarm_guid"] = p_oRequest.m_oBody.m_strGuid;
		l_Sql.param["orientation"] = ORIENTATION_FIRST;
		l_Sql.param["state"] = STATE_UNREAD;
		l_Sql.param["phone"] = p_oRequest.m_oBody.m_strUserPhone;
		l_Sql.param["open_id"] = p_oRequest.m_oBody.m_strOpenID;
		l_Sql.param["change_type"] = material.m_strAttachType;
		l_Sql.param["change_content"] = material.m_strAttachPath;
		//std::string l_strTime = m_pDateTime->ToString(m_pDateTime->FromString(p_oRequest.m_oBody.m_strCreateDate));
		l_Sql.param["change_date"] = p_oRequest.m_oBody.m_strCreateDate;
		l_Sql.param["long"] = p_oRequest.m_oBody.m_strLong;
		l_Sql.param["lat"] = p_oRequest.m_oBody.m_strLat;
		l_Sql.param["remark1"] = p_oRequest.m_oBody.m_strRemark1;
		l_Sql.param["remark2"] = p_oRequest.m_oBody.m_strRemark2;
		l_Sql.param["fguid"] = material.m_strFGuid;
		l_Sql.param["fname"] = material.m_strFName;

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{ // 执行SQL失败
			ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", INSERT_THIRD_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}
	
	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::InsertThirdAlarm(PROTOCOL::CTAAddAlarmRespond& p_oRespond, const PROTOCOL::CTAAddAlarmRequest& p_oRequest)
{
	//p_oRespond.m_oBody.m_strResult = "1";

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = INSERT_THIRD_ALARM;

	l_Sql.param["guid"] = p_oRequest.m_oBody.m_strGuid;
	l_Sql.param["type"] = p_oRequest.m_oBody.m_strType;
	l_Sql.param["address"] = p_oRequest.m_oBody.m_strAddress;
	l_Sql.param["content"] = p_oRequest.m_oBody.m_strCrcontent;
	l_Sql.param["is_phone"] = p_oRequest.m_oBody.m_strIsPhone;
	//std::string l_strTime = m_pDateTime->ToString(m_pDateTime->FromString(p_oRequest.m_oBody.m_strCreateDate));
	l_Sql.param["create_date"] = p_oRequest.m_oBody.m_strCreateDate;
	l_Sql.param["user_code"] = p_oRequest.m_oBody.m_strUserCode;
	l_Sql.param["user_name"] = p_oRequest.m_oBody.m_strUserName;
	l_Sql.param["user_phone"] = p_oRequest.m_oBody.m_strUserPhone;
	l_Sql.param["user_address"] = p_oRequest.m_oBody.m_strUserAddress;
	l_Sql.param["unit_contactor"] = p_oRequest.m_oBody.m_strUnitContactor;
	l_Sql.param["hand_phone"] = p_oRequest.m_oBody.m_strHandPhone;
	l_Sql.param["to_org_id"] = p_oRequest.m_oBody.m_strToOrgID;
	l_Sql.param["to_org_name"] = p_oRequest.m_oBody.m_strToOrgName;
	l_Sql.param["to_user_id"] = p_oRequest.m_oBody.m_strToUserID;
	l_Sql.param["to_user_name"] = p_oRequest.m_oBody.m_strToUserName;
	//l_Sql.param["state"] = p_oRequest.m_oBody.m_strState;
	l_Sql.param["state"] = STATE_UNREAD; //未读
	l_Sql.param["com_no"] = p_oRequest.m_oBody.m_strComNo;
	l_Sql.param["open_id"] = p_oRequest.m_oBody.m_strOpenID;
	l_Sql.param["long"] = p_oRequest.m_oBody.m_strLong;
	l_Sql.param["lat"] = p_oRequest.m_oBody.m_strLat;
	l_Sql.param["remark1"] = p_oRequest.m_oBody.m_strRemark1;
	l_Sql.param["remark2"] = p_oRequest.m_oBody.m_strRemark2;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", INSERT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}