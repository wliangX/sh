#include "Boost.h"
#include "BusinessImpl.h"

CBusinessImpl::CBusinessImpl()
{

}

CBusinessImpl::~CBusinessImpl()
{

}

void CBusinessImpl::OnInit()
{
	printf(" fast110 init enter!\n");
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_FAST110_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();	
	m_pLockFty = ICCGetILockFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pJsonFty = ICCGetIJsonFactory();
	m_TcpClient = ICCGetITcpClientFactory()->CreateTcpClient();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_ITcpClientCallbackPtr = boost::make_shared<TcpClientImpl>(m_pLog);

	SetBindMember();
	
	printf(" fast110 init end!\n");
}

void CBusinessImpl::OnStart()
{
	printf(" fast110 start enter!\n");
	
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/Fast110/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	bool bConnect = m_TcpClient->Connect(m_strPUCIP, m_PUCPort, m_ITcpClientCallbackPtr);
	if (!bConnect)
	{
		ICC_LOG_ERROR(m_pLog, "Connect PUC TCPServer Failed");
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DEVICE_STATE_SYNC, OnCTIDeviceStateNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ADD_CONFERENCE_PARTY_RESPOND, OnAddConferenceRespond);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DELETE_CONFERENCE_PARTY_RESPOND, OnDeleteConferenceRespond);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
	
	printf(" fast110 start end!\n");
}

void CBusinessImpl::OnStop()
{
	m_TcpClient->Close();
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCTIDeviceStateNotice(ObserverPattern::INotificationPtr p_pCTIStateInfo)
{
	std::string l_strMsg = p_pCTIStateInfo->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CDeviceStateSync l_oDeStateInfo;

	if (!l_oDeStateInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	// 过滤监听的号码 [5/22/2018 w26326]
	auto iter = m_BindMemberInfo.find(l_oDeStateInfo.m_oBody.m_strCalledId);
	if (iter == m_BindMemberInfo.end()
		|| l_oDeStateInfo.m_oBody.m_strCalledId != l_oDeStateInfo.m_oBody.m_strDevice)
	{
		ICC_LOG_DEBUG(m_pLog, "Invalid ADD:[%s]", l_strMsg.c_str());
		return;
	}
	std::string l_strTarget = iter->second;
	std::string l_strState = l_oDeStateInfo.m_oBody.m_strState;
	if (l_strState == TALKSTATE &&  l_oDeStateInfo.m_oBody.m_strCallDirection == CALL_DIRECTION_IN)
	{
		SendAddConRequest(l_oDeStateInfo, l_strTarget);
	}
	else if (l_strState == HANGEUP)
	{
		SendDisconnectMsg(l_oDeStateInfo, l_strTarget);
	}
}

void CBusinessImpl::OnAddConferenceRespond(ObserverPattern::INotificationPtr p_pRespInfo)
{
	std::string l_strMsg = p_pRespInfo->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CAddConferPartyRespond l_oRespInfo;

	if (!l_oRespInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}
	auto iter = m_mCallInfo.find(l_oRespInfo.m_oHeader.m_strRelatedID);
	if (iter == m_mCallInfo.end())
	{
		ICC_LOG_DEBUG(m_pLog, "Invalid Msg[%s]", l_strMsg.c_str());
		return;
	}
	tCallInfo l_tCallInfo = iter->second;
	m_mCallInfo.erase(iter);
	if (RESULT_STATE_SUCCESS == l_oRespInfo.m_oBody.m_strResult)
	{
		SendTopicConSync(RESULT_STATE_SUCCESS, l_tCallInfo);
	}
	else
	{
		SendTopicConSync(RESULT_STATE_FAIL, l_tCallInfo);
	}
}

void CBusinessImpl::SendTopicConSync(std::string p_strState, const tCallInfo& p_tCallInfo)
{
	PROTOCOL::CMakeConferPartySync l_oSync;

	l_oSync.m_oHeader.m_strCmd = TOPIC_CONFERENCE_SYNC;
	l_oSync.m_oHeader.m_strRequest = MAKE_CONFERENCE_SYNC;
	l_oSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;

	l_oSync.m_oBody.m_strCallrefID = p_tCallInfo.oCallInfo.m_strCallRefId;
	l_oSync.m_oBody.m_strSponsor = p_tCallInfo.oCallInfo.m_strCalledId;
	l_oSync.m_oBody.m_strCompere = p_tCallInfo.oCallInfo.m_strCalledId;
	l_oSync.m_oBody.m_strTarget = p_tCallInfo.oCallInfo.m_strCallerId;
	l_oSync.m_oBody.m_strResult = p_strState;

	std::string l_strMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnDeleteConferenceRespond(ObserverPattern::INotificationPtr p_pDeleteInfo)
{
	std::string l_strMsg = p_pDeleteInfo->GetMessages();
	PROTOCOL::CDeleteConferPartyRespond l_oDeleteInfo;

	if (!l_oDeleteInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}
	auto iter = m_mCallInfo.find(l_oDeleteInfo.m_oHeader.m_strRelatedID);
	if (iter == m_mCallInfo.end())
	{
		ICC_LOG_DEBUG(m_pLog, "Invalid Msg[%s]", l_strMsg.c_str());
		return;
	}
	tCallInfo l_tCallInfo = iter->second;
	m_mCallInfo.erase(iter);
	if (RESULT_STATE_SUCCESS == l_oDeleteInfo.m_oBody.m_strResult)
	{
		SendTopicConSync(RESULT_STATE_SUCCESS, l_tCallInfo);
	}
	else
	{
		SendTopicConSync(RESULT_STATE_FAIL, l_tCallInfo);
	}
}

void CBusinessImpl::SendAddConRequest(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strTarget)
{
	PROTOCOL::CAddConferPartyRequest l_oAddCon;
	l_oAddCon.m_oHeader.m_strCmd = ADD_CONFERENCE_PARTY_REQUEST;
	l_oAddCon.m_oHeader.m_strRequest = QUEUE_CTI_CONTROL;
	l_oAddCon.m_oHeader.m_strRequestType = MQTYPE_QUEUE;

	std::string l_strRelateID = m_pString->CreateGuid();
	l_oAddCon.m_oHeader.m_strMsgid = l_strRelateID;
	tCallInfo l_tCallInfo;
	l_tCallInfo.oCallInfo = p_oDeStateInfo.m_oBody;
	m_mCallInfo[l_strRelateID] = l_tCallInfo;

	PROTOCOL::CAddConferPartyRequest::CBody::CParty l_oParty;
	l_oParty.m_strTarget = p_strTarget;
	l_oParty.m_strTargetDeviceType = TARGET_DEVICE_TYPE_WIRELESS;

	l_oAddCon.m_oBody.m_vParty.push_back(l_oParty);
	l_oAddCon.m_oBody.m_strCallrefID = p_oDeStateInfo.m_oBody.m_strCallRefId;
	l_oAddCon.m_oBody.m_strCompere = p_oDeStateInfo.m_oBody.m_strCalledId;
	l_oAddCon.m_oBody.m_strCount = COUNFERENCE_COUNT;

	std::string l_strMsg = l_oAddCon.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SendDeleteConRequest(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strSponsor)
{
	PROTOCOL::CDeleteConferPartyRequest l_oDeleteCon;
	l_oDeleteCon.m_oHeader.m_strCmd = DELETE_CONFERENCE_PARTY_REQUEST;
	l_oDeleteCon.m_oHeader.m_strRequest = QUEUE_CTI_CONTROL;
	l_oDeleteCon.m_oHeader.m_strRequestType = MQTYPE_QUEUE;

	std::string l_strRelateID = m_pString->CreateGuid();
	l_oDeleteCon.m_oHeader.m_strMsgid = l_strRelateID;
	tCallInfo l_tCallInfo;
	l_tCallInfo.oCallInfo = p_oDeStateInfo.m_oBody;
	m_mCallInfo[l_strRelateID] = l_tCallInfo;

	l_oDeleteCon.m_oBody.m_strCallrefID = p_oDeStateInfo.m_oBody.m_strCallRefId;
	l_oDeleteCon.m_oBody.m_strTarget = p_strSponsor;
	l_oDeleteCon.m_oBody.m_strCompere = p_oDeStateInfo.m_oBody.m_strCalledId;

	std::string l_strMsg = l_oDeleteCon.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SetBindMember()
{
	std::string l_strMembers = m_pConfig->GetValue("ICC/Plugin/Fast110/Members","");
	std::vector<std::string> l_vecSplitTopics;
	m_pString->Split(m_pString->ReplaceAll(l_strMembers, " ", ""), ";", l_vecSplitTopics, 1);
	for (auto strSplit : l_vecSplitTopics)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			if (CORRECTSIZE == l_vecSubSplit.size())
			{
				m_BindMemberInfo[l_vecSubSplit.at(0)] = l_vecSubSplit.at(1);
			}
		}
	}
	m_strPUCIP = m_pConfig->GetValue("ICC/Plugin/Fast110/PUChost", "127.0.0.1");
	m_PUCPort = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Fast110/PUCPort", "6000"));
}

void CBusinessImpl::SendDisconnectMsg(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strSponsor)
{
	tTDiconnectMSG l_HeaderMsg;
	memset(&l_HeaderMsg, 0, sizeof(l_HeaderMsg));

	l_HeaderMsg.msgHeader = m_pHelpTool->HostToNetworkShort(0xAAAA);
	l_HeaderMsg.platfromID = m_pHelpTool->HostToNetworkShort(0);
	l_HeaderMsg.cmdType = m_pHelpTool->HostToNetworkShort(4);

	std::string l_strBody = p_oDeStateInfo.m_oBody.m_strCalledId +
							SOCKET_MSG_SEPARATE +
							p_strSponsor +
							SOCKET_MSG_SEPARATE +
							p_oDeStateInfo.m_oBody.m_strTime;
	//解决结构体内存对齐多出四个字节问题
	char Buf[1024] = { 0 };
	l_HeaderMsg.len = m_pHelpTool->HostToNetworkInt(l_strBody.length());
//	unsigned int l_strStart = sizeof(l_HeaderMsg)-2;
	memcpy(Buf, (char *)&l_HeaderMsg, sizeof(l_HeaderMsg)/*l_strStart*/);
	memcpy(Buf + sizeof(l_HeaderMsg)/*l_strStart*/, l_strBody.c_str(), l_strBody.length());

	std::string l_strSendMsg;
	l_strSendMsg.assign(Buf, sizeof(l_HeaderMsg) + l_strBody.length());
	//if (m_TcpClient->Send(l_strSendMsg.c_str(), l_strSendMsg.length()) < 0)
	if (m_TcpClient->Send(l_strSendMsg.c_str(), l_strSendMsg.length()) != l_strSendMsg.length())
	{
		ICC_LOG_DEBUG(m_pLog, "Send Message Failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "Send Socket HangUp Msg[%s]", l_strBody.c_str());
}


void TcpClientImpl::OnReceived(const char* p_pData, unsigned int p_iLength)
{
	std::string l_strRecvMsg = std::string(p_pData, p_iLength);
	ICC_LOG_DEBUG(m_pLog, "Fast110 Recv Socket Msg[%s]", l_strRecvMsg.c_str());
}

void TcpClientImpl::OnConnected(std::string p_strError)
{
	ICC_LOG_DEBUG(m_pLog, "Fast110 Recv Socket Msg[%s]", p_strError.c_str());
}

void TcpClientImpl::OnDisconnected(std::string p_strError)
{
	ICC_LOG_DEBUG(m_pLog, "DisConnect Msg[%s]", p_strError.c_str());
}
