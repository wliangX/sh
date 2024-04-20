#include "Boost.h"

#include "HytProtocol.h"
#include "RequestMemory.h"
#include "BlackListManager.h"
#include "TransferCallManager.h"
#include "ConferenceManager.h"
#include "BusinessImpl.h"
#include "SeatInfo.h"
#include "CWaitCallInfo.h"
#define SYSTEMID	("ICC")
#define SUBSYSTEMID ("Alarm-CTI")
#define LOG_FEEDBACK	"BS001003001"
#define FEEDBACK    "2"
#define AGENT_PHONE_STATE_LOGOUT			"0"
#define AGENT_PHONE_STATE_LOGIN				"1"

#define AGENT_PHONE_STATE_OFFHOOK 			"2"		//摘机
#define AGENT_PHONE_STATE_HANGUP			"3"		//挂机
#define AGENT_PHONE_STATE_BUSY				"4"		//忙
#define AGENT_PHONE_STATE_FREE				"5"		//空闲

#define AGENT_PHONE_STATE_RINGING 			"6"		//振铃
#define AGENT_PHONE_STATE_TALK 				"7"		//通话

#define ACD_INFO							"acd_dept"
#define STAFF_ID_NO							 "1"
#define STAFF_CODE							 "2"

//////////////////////////////////////////////////////////////////////////
CHytProtocol::CHytProtocol(void)
{
	m_pObserverCenter = nullptr;
	m_SwitchClientPtr = nullptr;
	m_JsonFactoryPtr = nullptr;
	m_StringUtilPtr = nullptr;
	m_DateTimePtr = nullptr;
	m_LogPtr = nullptr;
	m_pDBConnPtr = nullptr;
	m_pHelpTool = nullptr;
	m_pRedisClient=nullptr;

	m_bFSAesMode = false;

}
CHytProtocol::~CHytProtocol(void)
{
	//
}

boost::shared_ptr<CHytProtocol> CHytProtocol::m_pInstance = nullptr;
boost::shared_ptr<CHytProtocol> CHytProtocol::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CHytProtocol>();
	}

	return m_pInstance;
}
void CHytProtocol::ExitInstance()
{
	//
}
//////////////////////////////////////////////////////////////////////////

PROTOCOL::CHeaderEx CHytProtocol::CreateProtocolHeader(const std::string& p_strCmd, const std::string& p_strRequest,
	const std::string& p_strRequestType, const std::string& p_strRelatedId)
{
	PROTOCOL::CHeaderEx l_oHeader;

	l_oHeader.m_strSystemID = "ICC";
	l_oHeader.m_strSubsystemID = Server_Name_CTIServer;
	//l_oHeader.m_strMsgid = m_StringUtilPtr->CreateGuid();
	//l_oHeader.m_strMsgId = l_oHeader.m_strMsgid;
	//l_oHeader.m_strRelatedID = p_strRelatedId;
	l_oHeader.m_strSendTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oHeader.m_strCmd = p_strCmd;
	l_oHeader.m_strRequest = p_strRequest;
	l_oHeader.m_strRequestType = p_strRequestType;
	l_oHeader.m_strResponse = "";
	l_oHeader.m_strResponseType = "";

    //如果为空表示要广播， 不为空，表示为响应的，
	if (p_strRelatedId.empty())
	{
		l_oHeader.m_strMsgid = m_StringUtilPtr->CreateGuid();
		l_oHeader.m_strMsgId = l_oHeader.m_strMsgid;
		l_oHeader.m_strRequestFlag = "1";
	}
	else
	{
		l_oHeader.m_strMsgid = p_strRelatedId;
		l_oHeader.m_strMsgId = p_strRelatedId;
	}

	return l_oHeader;
}

void CHytProtocol::LoadAllBlackList()
{
	PROTOCOL::CLoadAllBlackListRequest l_oGetAllBlackListRequest;
	l_oGetAllBlackListRequest.m_oHeader = CreateProtocolHeader(Cmd_LoadAllBlackListRequest, Queue_CTIDBProcess, SendType_Queue);

	std::string l_strRequestMessage = l_oGetAllBlackListRequest.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strRequestMessage, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_LogPtr, "Send LoadAllBlackList Request Queue: [%s]", l_strRequestMessage.c_str());
}

// --------------------- 以下为具体命令的解析函数 CC: Call Control command ----------------------------
void CHytProtocol::CC_ClientRegisterSync(const std::string& p_strNetMsg)
{
	PROTOCOL::CClientRegisterSync l_oSyncObj;
	if (!l_oSyncObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ClientRegisterSync protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strClientId = l_oSyncObj.m_oBody.m_strClientID;
	std::string l_strClientType = l_oSyncObj.m_oBody.m_strClientType;
	std::string l_strClientStatus = l_oSyncObj.m_oBody.m_strClientStatus;

	// 客户端下线
	if (l_strClientStatus.compare("0") == 0)
	{
		//ACD 为空，不同ACD组成员是否都下线？
		//	long l_lTaskId = m_SwitchClientPtr->AgentLogout(l_strClientId, "");
	}

	std::string l_strAgentPhoneInfo;
	if (m_pRedisClient->HGet("AgentPhoneState", l_strClientId, l_strAgentPhoneInfo))
	{
		CAgentPhoneState l_CAgentPhoneState;
		l_CAgentPhoneState.m_strPhoneNum = l_strClientId;
		l_CAgentPhoneState.m_strUpdateTime = m_DateTimePtr->CurrentDateTimeStr();
		if (l_strClientStatus.compare("0") == 0)
		{
			l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_LOGOUT;
			CAgentPhoneState l_LastCAgentPhoneState;
			l_CAgentPhoneState.m_strBMDM = l_oSyncObj.m_oBody.m_strDeptCode;
			l_CAgentPhoneState.m_strBMMC = l_oSyncObj.m_oBody.m_strDeptName;
			if (l_LastCAgentPhoneState.ParseString(l_strAgentPhoneInfo, m_JsonFactoryPtr->CreateJson()))
			{
				if (l_oSyncObj.m_oBody.m_strDeptCode.empty())
				{
					l_CAgentPhoneState.m_strBMDM = l_LastCAgentPhoneState.m_strBMDM;
				}

				if (l_oSyncObj.m_oBody.m_strDeptName.empty())
				{
					l_CAgentPhoneState.m_strBMMC = l_LastCAgentPhoneState.m_strBMMC;
				}

				l_CAgentPhoneState.m_strZZJGDM = l_LastCAgentPhoneState.m_strZZJGDM;
				l_CAgentPhoneState.m_strDistrictCode = l_LastCAgentPhoneState.m_strZZJGDM;
				l_CAgentPhoneState.m_strSID = l_LastCAgentPhoneState.m_strSID;
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState data error, AgentPhoneInfo:[%s]", l_strAgentPhoneInfo.c_str());
			}
		}
		else
		{
			if (!l_oSyncObj.m_oBody.m_strDistrictCode.empty())
			{
				//l_CAgentPhoneState.m_strZZJGDM = l_oSyncObj.m_oBody.m_strDistrictCode;
				std::string l_strDistrictCode = l_oSyncObj.m_oBody.m_strDistrictCode;

				l_strDistrictCode = m_StringUtilPtr->Mid(l_strDistrictCode, 0, 6);
				l_CAgentPhoneState.m_strSID = l_strDistrictCode + l_strClientId;
				l_CAgentPhoneState.m_strDistrictCode = l_strDistrictCode;
			}
			
			l_CAgentPhoneState.m_strAgentID = l_oSyncObj.m_oBody.m_strClientName;
			l_CAgentPhoneState.m_strReceiptCode = l_oSyncObj.m_oBody.m_strStaffCode;
			l_CAgentPhoneState.m_strReceiptName = l_oSyncObj.m_oBody.m_strStaffName;

			l_CAgentPhoneState.m_strZZJGDM = l_oSyncObj.m_oBody.m_strDeptCode;
			l_CAgentPhoneState.m_strBMDM = l_oSyncObj.m_oBody.m_strDeptCode;
			l_CAgentPhoneState.m_strBMMC = l_oSyncObj.m_oBody.m_strDeptName;
			l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_LOGIN;
		}
		
		l_strAgentPhoneInfo = l_CAgentPhoneState.ToString(m_JsonFactoryPtr->CreateJson());
		m_pRedisClient->HSet("AgentPhoneState", l_strClientId, l_strAgentPhoneInfo);
		ICC_LOG_DEBUG(m_LogPtr, "HSet AgentPhoneState:[%s], [%s]", l_strClientId.c_str(), l_strAgentPhoneInfo.c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "HGet,Invalid ClientRegisterSync not find device, device:[%s]", l_strClientId.c_str());
	}
}

void CHytProtocol::CC_AgentLogin(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAgentLoginModeRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AgentLogin request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strLoginMode = l_oRequestObj.m_oBody.m_strLoginMode;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->AgentLogin(l_strAgent, l_strACD);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_AgentLogout(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAgentLoginModeRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AgentLogout request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strLoginMode = l_oRequestObj.m_oBody.m_strLoginMode;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->AgentLogout(l_strAgent, l_strACD);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_SetAgentState(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CSetAgentStateRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid SetAgentState request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strReadyState = l_oRequestObj.m_oBody.m_strReadyState;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->SetAgentState(l_strAgent, l_strACD, l_strReadyState.compare(ReadyState_Idle) == 0 ? ReadyState_Ready : ReadyState_NotReady);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_MakeCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CMakeCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid MakeCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
	std::string l_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
	std::string l_strTargetPhoneType = l_oRequestObj.m_oBody.m_strTargetPhoneType;
	std::string l_strCaseId = l_oRequestObj.m_oBody.m_strCaseId;
	std::string l_strRelateCallRefId = l_oRequestObj.m_oBody.m_strRelateCallRefId;
	std::string l_strReceiptCode = l_oRequestObj.m_oBody.m_strReceiptCode;
	std::string l_strReceiptName = l_oRequestObj.m_oBody.m_strReceiptName;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->MakeCall(l_strCallerId, l_strCalledId, l_strTargetPhoneType, l_strCaseId);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetCaseId(l_strCaseId);
		l_pRequestMemory->SetRelateCallRefId(l_strRelateCallRefId);
		l_pRequestMemory->SetReceiptCode(l_strReceiptCode);
		l_pRequestMemory->SetReceiptName(l_strReceiptName);
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);

	}
	bool l_bIsSuccess(false);
	std::string tmp_strMsg("success");
	if (l_oRequestObj.m_oBody.m_strCallType.compare(FEEDBACK)==0)
	{
		l_bIsSuccess = AddSyntInfo(l_oRequestObj);
		if (!l_bIsSuccess)
		{
			tmp_strMsg = "add synt info failed";
		}
	}
}
void CHytProtocol::CC_AnswerCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAnswerCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AnswerCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->AnswerCall(l_strCallRefId, l_strDevice);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_RefuseAnswer(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAnswerCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid RefuseAnswer request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->RefuseAnswer(l_strCallRefId, l_strDevice);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_ClearCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CClearCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ClearCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ClearCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		if (l_pRequestMemory)
		{
			l_pRequestMemory->SetMsgId(l_strMsgId);
			l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
			CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
		}
	}
}

void CHytProtocol::CC_ListenCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CListenCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ListenCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ListenCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_PickupCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CPickupCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid PickupCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->PickupCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_Hangup(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CHangupRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid Hangup request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;

	std::string l_strIsBargein = l_oRequestObj.m_oBody.m_strIsbargein;
	if (m_SwitchClientPtr)
	{
		if (m_bFSAesMode && l_strIsBargein == "1")
		{
			PROTOCOL::CHangupRespond l_oRespondObj;
			l_oRespondObj.m_oBody.m_strCallRefId = l_strCallRefId;
			l_oRespondObj.m_oBody.m_strDevice = l_strDevice;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;

			l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			l_oRespondObj.m_oHeader.m_strMsgId = l_strMsgId;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (p_pNotifiRequest)
			{
				ICC_LOG_DEBUG(m_LogPtr, "xxxx Send CR_Hangup Respond: [%s]", l_strRespondMsg.c_str());
				p_pNotifiRequest->Response(l_strRespondMsg);
			}
		}
		else
		{
			long l_lTaskId = m_SwitchClientPtr->Hangup(l_strCallRefId, l_strSponsor, l_strDevice);
			if (l_lTaskId == -2 && m_bFSAesMode)
			{
				PROTOCOL::CHangupRespond l_oRespondObj;
				l_oRespondObj.m_oBody.m_strCallRefId = l_strCallRefId;
				l_oRespondObj.m_oBody.m_strDevice = l_strDevice;
				l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;

				l_oRespondObj.m_oHeader.m_strResult = Result_Success;
				l_oRespondObj.m_oHeader.m_strMsgId = l_strMsgId;

				std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
				if (p_pNotifiRequest)
				{
					ICC_LOG_DEBUG(m_LogPtr, "xxxx Send CR_Hangup Respond: [%s]", l_strRespondMsg.c_str());
					p_pNotifiRequest->Response(l_strRespondMsg);
				}
			}
			else
			{
				boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
				l_pRequestMemory->SetMsgId(l_strMsgId);
				l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
				CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
			}
			
		}
	}
}
void CHytProtocol::CC_BargeInCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CBargeInCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid BargeInCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->BargeInCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_ForcePopCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CForcePopCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ForcePopCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ForcePopCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_ConsultationCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CConsultationCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ConsultationCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	std::string l_strTargetDeviceType = l_oRequestObj.m_oBody.m_strTargetDeviceType;
	std::string l_strIsConference = l_oRequestObj.m_oBody.m_strIsConference;
	if (m_SwitchClientPtr)
	{
		if (m_bFSAesMode)
		{
			PROTOCOL::CConsultationCallRespond l_oRespondObj;

			l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			l_oRespondObj.m_oHeader.m_strMsgId = l_strMsgId;
			l_oRespondObj.m_oBody.m_strActiveCallRefId = l_strCallRefId;
			l_oRespondObj.m_oBody.m_strHeldCallRefId = l_strCallRefId;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oRespondObj.m_oBody.m_strTarget = l_strTarget;
			if (l_strIsConference == "1")
			{
				//添加会议的咨询呼叫
				std::string l_strConferenceId = l_strCallRefId;
				int l_nRes = m_SwitchClientPtr->CreateConference(l_strSponsor, l_strConferenceId);
				if (!l_strConferenceId.empty() && l_nRes > 0)
				{
					m_SwitchClientPtr->AddConferenceParty(l_strConferenceId, l_strTarget, l_strTargetDeviceType);
				}
				else
				{
					l_oRespondObj.m_oHeader.m_strResult = "1";
				}
			}

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (p_pNotifiRequest)
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send CR_ConsultationCallEx Respond: [%s]", l_strRespondMsg.c_str());
				p_pNotifiRequest->Response(l_strRespondMsg);
			}
		}
		else
		{
			long l_lTaskId = m_SwitchClientPtr->ConsultationCall(l_strCallRefId, l_strSponsor, l_strTarget, l_strTargetDeviceType);

			boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
			l_pRequestMemory->SetMsgId(l_strMsgId);
			l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
			CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);

			ICC_LOG_DEBUG(m_LogPtr, "ConsultationCall request:TaskId:[%d],HeldCallRefId:[%s],Sponsor:[%s],Target:[%s]", 
				l_lTaskId, l_strCallRefId.c_str(), l_strSponsor.c_str(), l_strTarget.c_str());
		}
	}
}

void CHytProtocol::CC_TransferCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CTransferCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid TransferCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strActiveCallRefId = l_oRequestObj.m_oBody.m_strActiveCallRefId;
	std::string l_strHeldCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = 0;
		if (m_bFSAesMode)
		{
			//l_lTaskId = m_SwitchClientPtr->TransferCall(l_strActiveCallRefId, "", l_strSponsor, l_strTarget);

			std::string l_strState = "";// = m_SwitchClientPtr->GetReadyAgentEx(l_strTarget);
			if (l_strState != ReadyState_Ready && !l_strState.empty())
			{
				//设备忙，回应失败
				CR_TransferCallEx(p_pNotifiRequest, l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strMsgId, false);
				Sync_TransferCall(l_strActiveCallRefId, "", l_strSponsor, l_strTarget, "", ERROR_RESOURCE_BUSY);
				return;
			}
			else
			{
				std::string l_strTargetDeviceType;
				if (!l_strState.empty())
				{
					l_strTargetDeviceType = TARGET_DEVICE_TYPE_INSIDE;
				}
				else
				{
					l_strTargetDeviceType = TARGET_DEVICE_TYPE_OUTSIDE;
				}
				CTransferCallManager::Instance()->AddNewTransferCall(l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strTargetDeviceType, l_strMsgId,false, p_pNotifiRequest);
				//CR_TransferCallEx(p_pNotifiRequest, l_strActiveCallRefId, l_strSponsor, l_strTarget, l_strMsgId, true);
			}
			return;
		}
		else
		{
			l_lTaskId = m_SwitchClientPtr->TransferCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);
		}

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_TransferCallEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CConsultationCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid TransferCallEx request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	std::string l_strTargetDeviceType = l_oRequestObj.m_oBody.m_strTargetDeviceType;
	std::string l_strIsACD = l_oRequestObj.m_oBody.m_strIsACD;

	if (m_bFSAesMode)
	{
		std::string l_strState;
		if (l_strIsACD == "1")
		{
			std::string l_strFreeDevice = m_SwitchClientPtr->GetReadyAgentEx(l_strTarget);
			//	转机构，按 ACD查找空闲坐席
			//l_pTransferCall->m_lGetReadyAgentTaskId = GetReadyAgent(p_strTarget);
			
			if (l_strFreeDevice.empty())
			{
				//没有找到空闲坐席，回应失败
				CR_TransferCallEx(p_pNotifiRequest, l_strCallRefId, l_strSponsor, l_strTarget, l_strMsgId, false);

				Sync_TransferCall(l_strCallRefId, "", l_strSponsor, l_strTarget, "", false);
				return;
			}
			
			l_strTarget = l_strFreeDevice;
			l_strState = CallStateString[STATE_FREE];
		}
		else
		{
			//l_strState = m_SwitchClientPtr->GetReadyAgentEx(l_strTarget);
		}

		/*
		//在前端做限制，服务端不做限制
		if (l_strState != ReadyState_Ready && !l_strState.empty())
		{
			//设备忙，回应失败
			CR_TransferCallEx(p_pNotifiRequest, l_strCallRefId, l_strSponsor, l_strTarget, l_strMsgId, false);
			Sync_TransferCall(l_strCallRefId, "", l_strSponsor, l_strTarget, "", ERROR_RESOURCE_BUSY);
			return;
		}
		*/

		std::string l_strBargeInSponsor = "";
		std::string l_strCompere = "";
		std::string l_strConferenceId = m_SwitchClientPtr->QueryConferenceByCallid(l_strCallRefId, l_strCompere, l_strBargeInSponsor);
		if (!l_strConferenceId.empty())
		{
			//会议里禁止转警
			CR_TransferCallEx(p_pNotifiRequest, l_strCallRefId, l_strSponsor, l_strTarget, l_strMsgId, false);
			Sync_TransferCall(l_strCallRefId, "", l_strSponsor, l_strTarget, "", 1);
			return;
		}

	}
	
	CTransferCallManager::Instance()->AddNewTransferCall(l_strCallRefId, l_strSponsor, l_strTarget, l_strTargetDeviceType, l_strMsgId, l_strIsACD.compare("1") == 0 ? true : false);
	CR_TransferCallEx(p_pNotifiRequest, l_strCallRefId, l_strSponsor, l_strTarget, l_strMsgId, true);
	
}

void CHytProtocol::CC_CancelTransferCallEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CConsultationCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid TransferCallEx request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	if (m_bFSAesMode)
	{
		CTransferCallManager::Instance()->CancelTransferCall(l_strCallRefId, l_strSponsor, l_strTarget);
	}
	else
	{
		CTransferCallManager::Instance()->CancelTransferCall(l_strCallRefId, l_strSponsor, l_strTarget);
	}
	

	CR_CancelTransferCallEx(p_pNotifiRequest, l_strCallRefId, l_strSponsor, l_strTarget, l_strMsgId);
}

void CHytProtocol::CC_DeflectCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeflectCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid DeflectCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->DeflectCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}


void CHytProtocol::CC_HoldCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CHoldCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid HoldCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->HoldCall(l_strCallRefId, l_strSponsor, l_strDevice);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_RetrieveCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CRetrieveCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid CC_RetrieveCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strHeldDevice;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->RetrieveCall(l_strCallRefId, l_strSponsor, l_strDevice);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_ReconnectCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CReconnectCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ReconnectCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strActiveCallRefId = l_oRequestObj.m_oBody.m_strActiveCallRefId;
	std::string l_strHeldCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strDevice);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_MakeConference(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CMakeConferenceRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid MakeConference request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strConferenceId = "";
	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	//	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string l_strCount = l_oRequestObj.m_oBody.m_strCount;
	std::vector<CConferenceParty> l_vPartyQueue;
	int l_nCount = atoi(l_strCount.c_str());
	for (int i = 0; i < l_nCount; ++i)
	{
		CConferenceParty l_oConferenceParty;

		l_oConferenceParty.m_strTarget = l_oRequestObj.m_oBody.m_vecData[i].m_strTarget;
		l_oConferenceParty.m_strTargetDeviceType = l_oRequestObj.m_oBody.m_vecData[i].m_strTargetDeviceType;
		l_vPartyQueue.push_back(l_oConferenceParty);
	}

	if (!m_bFSAesMode)
	{
		//l_strConferenceId = CConferenceManager::Instance()->AddConference(l_strCompere, l_vPartyQueue);
		CR_MakeConference(l_strConferenceId, p_pNotifiRequest);
	}
	else
	{
		if (m_SwitchClientPtr)
		{
			std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
			int l_nRes = m_SwitchClientPtr->CreateConference(l_strCompere, l_strConferenceId);
			if (!l_strConferenceId.empty() && l_nRes > 0)
			{
				for (std::size_t i = 0; i < l_vPartyQueue.size(); ++i)
				{
					m_SwitchClientPtr->AddConferenceParty(l_strConferenceId, l_vPartyQueue[i].m_strTarget, l_vPartyQueue[i].m_strTargetDeviceType);
				}
			}
			CR_MakeConference(l_strConferenceId, p_pNotifiRequest);
		}
	}
}


void CHytProtocol::CC_MakeConferenceEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr p_pJson = m_JsonFactoryPtr->CreateJson();
	if (!p_pJson->LoadJson(p_strNetMsg))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid make conference request protocol: [%s]", p_strNetMsg.c_str());
		return;
	}

	std::string l_strActiveCallId = p_pJson->GetNodeValue("/body/active_callref_id", "");
	std::string l_strHoldCallId = p_pJson->GetNodeValue("/body/held_callref_id", "");
	std::string l_strCompere = p_pJson->GetNodeValue("/body/compere", "");
	std::string l_strTarget = p_pJson->GetNodeValue("/body/target", "");
	bool bRes = true;

	std::string l_strConferenceId = "";
	if (l_strActiveCallId.empty() || l_strHoldCallId.empty() || l_strCompere.empty() || l_strTarget.empty())
	{
		bRes = false;
	}
	else
	{
		if (m_bFSAesMode)
		{
			int l_nRes = m_SwitchClientPtr->CreateConference(l_strCompere, l_strConferenceId);
			if (!l_strConferenceId.empty() && l_nRes > 0)
			{
				m_SwitchClientPtr->AddConferenceParty(l_strConferenceId, l_strTarget, "");
			}
		}
		else
		{
			l_strConferenceId = CConferenceManager::Instance()->CreateConferenceEx(l_strActiveCallId, l_strHoldCallId, l_strTarget, l_strCompere);
			if (l_strConferenceId.empty())
			{
				bRes = false;
			}
		}
		
	}
	CR_MakeConference(l_strConferenceId, p_pNotifiRequest, bRes);
}

void CHytProtocol::CC_AddConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAddConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AddConferenceParty request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string l_strCount = l_oRequestObj.m_oBody.m_strCount;

	std::string l_strTarget;
	std::string l_strTargetType;
	int l_nCount = atoi(l_strCount.c_str());
	for (int i = 0; i < l_nCount; ++i)
	{
		l_strTarget = l_oRequestObj.m_oBody.m_vecData[i].m_strTarget;
		l_strTargetType = l_oRequestObj.m_oBody.m_vecData[i].m_strTargetDeviceType;
		break;
	}

	std::string l_strConferenceId;
	if (!m_bFSAesMode)
	{
		if (l_nCount > 0)
		{
			//	在已有通话或会议上新增会议成员
			l_strConferenceId = CConferenceManager::Instance()->AddConferencePartyNew(l_strCallRefId, l_strCompere, l_strTarget, l_strTargetType);
		}
		CR_AddConferenceParty(l_strConferenceId, p_pNotifiRequest);
	}
	else
	{
		if (m_SwitchClientPtr)
		{
			m_SwitchClientPtr->AddConferencePartyEx(l_strCallRefId, l_strCompere, "", l_strConferenceId);

			ICC_LOG_DEBUG(m_LogPtr, "AddConferenceParty request,ConferenceId:[%s],Target:[%s],TargetType:[%s]", l_strConferenceId.c_str(), l_strTarget.c_str(), l_strTargetType.c_str());
			if (!l_strConferenceId.empty())
			{
				m_SwitchClientPtr->AddConferenceParty(l_strConferenceId, l_strTarget, l_strTargetType);
			}
			CR_AddConferenceParty(l_strConferenceId, p_pNotifiRequest);
		}
	}
	
}

void CHytProtocol::CC_QueryConferenceByCallid(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid QueryConferenceByCallid request protocol: [%s]", p_strNetMsg.c_str());
		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;

	//应答
	PROTOCOL::CQueryConferenceByCallidRespond l_oQueryConferenceByCallidRespond;
	l_oQueryConferenceByCallidRespond.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;

	std::string l_strConferenceID = "";
	std::string l_strBargeinSponSor;
	std::string l_strBargeinTarget;
	if (!m_bFSAesMode)
	{
		std::string l_strOriginalCTICallRefId;
		std::string l_strCompere;
		l_strConferenceID = CConferenceManager::Instance()->FindConferenceIdByCallId(l_strCallRefId, l_strCompere, l_strOriginalCTICallRefId);
		if (!l_strConferenceID.empty())
		{
			if (CConferenceManager::Instance()->GetBargeinCreateSponsor(l_strConferenceID, l_strBargeinSponSor, l_strBargeinTarget))
			{
				l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "1";
				l_oQueryConferenceByCallidRespond.m_oBody.m_strBargeinSponSor = l_strBargeinSponSor;
				l_oQueryConferenceByCallidRespond.m_oBody.m_strBargeinTarget = l_strBargeinTarget;
			}
			else
			{
				l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
			}
			l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "1";
			l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = l_strConferenceID;

			//--------------2023/8/25应急修改一下--------
			if (!l_strConferenceID.empty())
			{
				PROTOCOL::CGetConferencePartyRespond l_oGetConferencePartyRespond;
				bool bRes = CConferenceManager::Instance()->GetConferenceParty(l_strConferenceID, l_oGetConferencePartyRespond);
				if (bRes)
				{
					l_oQueryConferenceByCallidRespond.m_oBody.m_strCompere = l_oGetConferencePartyRespond.m_oBody.m_strCompere;
				}
			}
			//-------------------------------------------
		}
		else
		{
			l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "0";
			l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
			l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = l_strConferenceID;
		}
		
	}
	else
	{
		if (m_SwitchClientPtr)
		{
			int nCreateType = 0;
			std::string l_strBargeInSponsor = "";
			std::string l_strCompere = "";
			l_strConferenceID = m_SwitchClientPtr->QueryConferenceByCallid(l_strCallRefId, l_strCompere, l_strBargeInSponsor);
			if (!l_strConferenceID.empty())
			{
				l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = l_strConferenceID;
				l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "1";
				l_oQueryConferenceByCallidRespond.m_oBody.m_strCompere = l_strCompere;
				if (l_strBargeInSponsor.empty())
				{
					l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
				}
				else
				{
					l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "1";
					l_oQueryConferenceByCallidRespond.m_oBody.m_strBargeinSponSor = l_strBargeInSponsor;
				}
			}
			else
			{
				l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = "";
				l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "0";
				l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
			}
		}
	}

	std::string l_strMessage = l_oQueryConferenceByCallidRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send QueryConferenceByCallid Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send QueryConferenceByCallid Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strMessage.c_str());
	}
}

void CHytProtocol::CC_GetConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetConferenceParty request protocol: [%s]", p_strNetMsg.c_str());
		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string l_strConferenceId = l_oRequestObj.m_oBody.m_strConferenceId;
	//应答
	PROTOCOL::CGetConferencePartyRespond l_oGetConferencePartyRespond;
	//l_oGetConferencePartyRespond.m_oHeader = CreateProtocolHeader(Cmd_GetConferencePartyRespond);
	if (!m_bFSAesMode)
	{
		l_oGetConferencePartyRespond.m_oBody.m_strConferenceId = l_strConferenceId;
		l_oGetConferencePartyRespond.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
		bool bRes = CConferenceManager::Instance()->GetConferenceParty(l_strConferenceId, l_oGetConferencePartyRespond);
		if (bRes)
		{
			l_oGetConferencePartyRespond.m_oBody.m_strResult = Result_Success;
		}
		else
		{
			l_oGetConferencePartyRespond.m_oBody.m_strResult = Result_Failed;
		}

		std::string l_strMessage = l_oGetConferencePartyRespond.ToString(m_JsonFactoryPtr->CreateJson());
		if (p_pNotifiRequest)
		{
			ICC_LOG_DEBUG(m_LogPtr, "Send GetConferenceParty Respond: [%s]", l_strMessage.c_str());
			p_pNotifiRequest->Response(l_strMessage);
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "Send GetConferenceParty Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
				l_strMessage.c_str());
		}
	}
	else
	{
		std::string l_strData;
		int nRes = m_SwitchClientPtr->GetConferenceParty(l_strConferenceId, l_strData);

		JsonParser::IJsonPtr l_JsonPtr = m_JsonFactoryPtr->CreateJson();
		if (!l_JsonPtr->LoadJson(l_strData))
		{
			ICC_LOG_ERROR(m_LogPtr, "load json object failed.");
			return;
		}
		l_JsonPtr->SetNodeValue("/header/msgid", l_strMsgId);
		std::string l_strRespondMsg = l_JsonPtr->ToString();
		//if (!l_strData.empty())
		{
			ICC_LOG_DEBUG(m_LogPtr, "nRes:[%d],Send GetConferenceParty Respond:[%s]", nRes, l_strRespondMsg.c_str());
			p_pNotifiRequest->Response(l_strRespondMsg);
		}
	}
}

void CHytProtocol::CC_DisenableConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string strConferenceId = l_oRequestObj.m_oBody.m_strConferenceId;
	std::string strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string strTarget = l_oRequestObj.m_oBody.m_strTarget;

	PROTOCOL::CDeleteConferencePartyRespond l_oRespondObj;
	
	if (!m_bFSAesMode)
	{
		if (CConferenceManager::Instance()->DisenableConferenceParty(strConferenceId, strTarget))
		{
			l_oRespondObj.m_oBody.m_strResult = Result_Success;
			l_oRespondObj.m_oHeader.m_strResult = Result_Success;
		}
		else
		{
			l_oRespondObj.m_oBody.m_strResult = Result_Failed;
			l_oRespondObj.m_oHeader.m_strResult = Result_Failed;
		}
	}
	else
	{
		if (m_SwitchClientPtr)
		{
			if (m_SwitchClientPtr->DisenableConferenceParty(strConferenceId, strTarget,"") > 0)
			{
				l_oRespondObj.m_oBody.m_strResult = Result_Success;
				l_oRespondObj.m_oHeader.m_strResult = Result_Success;
			}
			else
			{
				l_oRespondObj.m_oBody.m_strResult = Result_Failed;
				l_oRespondObj.m_oHeader.m_strResult = Result_Failed;
			}
		}
	}
	
	//l_oRespondObj.m_oHeader = CreateProtocolHeader("disenable_conference_party_respond");
	l_oRespondObj.m_oHeader.m_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	l_oRespondObj.m_oBody.m_strConferenceId = strConferenceId;
	l_oRespondObj.m_oBody.m_strTarget = strTarget;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send Respond:[%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}


void CHytProtocol::CC_DeleteConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid DeleteConferenceParty request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string strConferenceId = l_oRequestObj.m_oBody.m_strConferenceId;
	std::string strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (!m_bFSAesMode)
	{
		CConferenceManager::Instance()->DeleteConferenceParty(strConferenceId, strTarget);
	}
	else
	{
		if (m_SwitchClientPtr)
		{
			m_SwitchClientPtr->DeleteConferenceParty(strConferenceId, strTarget,"");
		}
	}
	CR_DeleteConferenceParty(strConferenceId, p_pNotifiRequest);
}

void CHytProtocol::CC_TakeOverCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CTakeOverCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid TakeOverCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	if (m_SwitchClientPtr)
	{
		//if (m_bFSAesMode)
		//{
		//	std::string l_strState = m_SwitchClientPtr->GetReadyAgentEx(l_strTarget);
		//	if (l_strState != ReadyState_Ready && !l_strState.empty())
		//	{
		//		//设备忙，回应失败
		//		CR_TakeOverCallEx(p_pNotifiRequest, l_strSponsor, l_strTarget, l_strMsgId, false);
		//		return;
		//	}
		//}

		long l_lTaskId = m_SwitchClientPtr->TakeOverCall(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetCTIConnState(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetCTIConnStateRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetCTIConnState request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetCTIConnectState();

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_GetDeviceList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetDeviceListRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetDeviceList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetDeviceList();

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);

		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_GetACDList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetACDListRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetACDList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetACDList();

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetFreeAgentList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetAgentListRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetAgentList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strACDGrp = l_oRequestObj.m_oBody.m_strACDGrp;
	std::string l_strDeptCode = l_oRequestObj.m_oBody.m_strDeptCode;
	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetFreeAgentList(l_strACDGrp, l_strDeptCode);

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetAgentList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetAgentListRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetAgentList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
	std::string l_strACDGrp = l_oRequestObj.m_oBody.m_strACDGrp;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetAgentList(l_strACDGrp);

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetCallList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetCallListRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetCallList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetCallList();

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetReadyAgent(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetReadyAgentRequest l_oGetReadyAgentRequest;
	if (!l_oGetReadyAgentRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetReadyAgent request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oGetReadyAgentRequest.m_oHeader.m_strMsgid;
	std::string l_strACDGrp = l_oGetReadyAgentRequest.m_oBody.m_strACDGrp;
	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->GetReadyAgent(l_strACDGrp);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::Sync_SetBlackList(const PROTOCOL::CSetBlackListRequest l_oSetBlackListRequest, bool p_bIsUpdate)
{
	PROTOCOL::CSetBlackListSync l_oSetBlackListSync;

	l_oSetBlackListSync.m_oBody.m_strLimitType = l_oSetBlackListRequest.m_oBody.m_strLimitType;
	l_oSetBlackListSync.m_oBody.m_strLimitNum = l_oSetBlackListRequest.m_oBody.m_strLimitNum;
	l_oSetBlackListSync.m_oBody.m_strlimitMinute = l_oSetBlackListRequest.m_oBody.m_strLimitMin;
	l_oSetBlackListSync.m_oBody.m_strLimitReason = l_oSetBlackListRequest.m_oBody.m_strLimitReason;
	l_oSetBlackListSync.m_oBody.m_strStaffCode = l_oSetBlackListRequest.m_oBody.m_strStaffCode;
	l_oSetBlackListSync.m_oBody.m_strStaffName = l_oSetBlackListRequest.m_oBody.m_strStaffName;
	l_oSetBlackListSync.m_oBody.m_strBeginTime = l_oSetBlackListRequest.m_oBody.m_strBeginTime;
	l_oSetBlackListSync.m_oBody.m_strEndTime = l_oSetBlackListRequest.m_oBody.m_strEndTime;
	l_oSetBlackListSync.m_oBody.m_strApprover = l_oSetBlackListRequest.m_oBody.m_strApprover;
	l_oSetBlackListSync.m_oBody.m_strApprovedInfo = l_oSetBlackListRequest.m_oBody.m_strApprovedInfo;
	l_oSetBlackListSync.m_oBody.m_strReceiptDeptName = l_oSetBlackListRequest.m_oBody.m_strReceiptDeptName;
	l_oSetBlackListSync.m_oBody.m_strReceiptDeptDistrictCode = l_oSetBlackListRequest.m_oBody.m_strReceiptDeptDistrictCode;
	l_oSetBlackListSync.m_oBody.m_strIsUpdate = p_bIsUpdate ? Result_True : Result_False;

	//	新增或修改数据库黑名单
	l_oSetBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_SetBlackListSync, Queue_CTIDBProcess, SendType_Queue);

	std::string l_strSyncMessage = l_oSetBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send SetBlackList Sync Queu: [%s]", l_strSyncMessage.c_str());

	//	广播接警席
	l_oSetBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_SetBlackListSync, Topic_BlackListSync, SendType_Topic);

	l_strSyncMessage = l_oSetBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send SetBlackList Sync Topic: [%s]", l_strSyncMessage.c_str());
}
void CHytProtocol::Sync_DeleteBlackList(const std::string& p_strLimitType, const std::string& p_strLimitNum)
{
	PROTOCOL::CDeleteBlackListSync l_oDeleteBlackListSync;

	l_oDeleteBlackListSync.m_oBody.m_strLimitType = p_strLimitType;
	l_oDeleteBlackListSync.m_oBody.m_strLimitNum = p_strLimitNum;

	//	删除数据库黑名单
	l_oDeleteBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_DeleteBlackListSync, Queue_CTIDBProcess, SendType_Queue);

	std::string l_strSyncMessage = l_oDeleteBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send DeleteBlackList Sync Queue: [%s]", l_strSyncMessage.c_str());

	//	广播接警席
	l_oDeleteBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_DeleteBlackListSync, Topic_BlackListSync, SendType_Topic);

	l_strSyncMessage = l_oDeleteBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send DeleteBlackList Sync Topic: [%s]", l_strSyncMessage.c_str());
}
void CHytProtocol::Sync_DeleteAllBlackList()
{
	PROTOCOL::CDeleteAllBlackListSync l_oDeleteAllBlackListSync;

	//	删除数据库所有黑名单
	l_oDeleteAllBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_DeleteAllBlackListSync, Queue_CTIDBProcess, SendType_Queue);

	std::string l_strSyncMessage = l_oDeleteAllBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send DeleteAllBlackList Sync Queue: [%s]", l_strSyncMessage.c_str());

	//	广播接警席
	l_oDeleteAllBlackListSync.m_oHeader = CreateProtocolHeader(Cmd_DeleteAllBlackListSync, Topic_BlackListSync, SendType_Topic);

	l_strSyncMessage = l_oDeleteAllBlackListSync.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send DeleteAllBlackList Sync Topic: [%s]", l_strSyncMessage.c_str());
}
void CHytProtocol::CC_SetBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CSetBlackListRequest l_oSetBlackListRequest;
	if (!l_oSetBlackListRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid SetBlackList request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oSetBlackListRequest.m_oHeader.m_strMsgId;
	std::string l_strLimitType = l_oSetBlackListRequest.m_oBody.m_strLimitType;
	std::string l_strLimitNum = l_oSetBlackListRequest.m_oBody.m_strLimitNum;
	std::string l_strLimitMin = l_oSetBlackListRequest.m_oBody.m_strLimitMin;
	std::string l_strLimitReason = l_oSetBlackListRequest.m_oBody.m_strLimitReason;
	std::string l_strStaffCode = l_oSetBlackListRequest.m_oBody.m_strStaffCode;
	std::string l_strStaffName = l_oSetBlackListRequest.m_oBody.m_strStaffName;
	std::string l_strBegintime = l_oSetBlackListRequest.m_oBody.m_strBeginTime;
	std::string l_strEndTime = l_oSetBlackListRequest.m_oBody.m_strEndTime;
	std::string l_strApprover = l_oSetBlackListRequest.m_oBody.m_strApprover;
	std::string l_strApprovedInfo = l_oSetBlackListRequest.m_oBody.m_strApprovedInfo;
	std::string l_strReceiptDeptName = l_oSetBlackListRequest.m_oBody.m_strReceiptDeptName;
	std::string l_strReceiptDeptDistrictCode = l_oSetBlackListRequest.m_oBody.m_strReceiptDeptDistrictCode;

	CBlackListManager::Instance()->GetLimitTime(l_strLimitMin, l_strBegintime, l_strEndTime);
	l_oSetBlackListRequest.m_oBody.m_strBeginTime = l_strBegintime;
	l_oSetBlackListRequest.m_oBody.m_strBeginTime = l_strEndTime;
	bool l_bIsUpdate = false;
	if (CBlackListManager::Instance()->FindBlackNumber(l_strLimitNum))
	{
		//	存在该黑名单记录，更新数据库
		l_bIsUpdate = true;
	}

	boost::shared_ptr<CLimitNum> l_pLimitNumObj = boost::make_shared<CLimitNum>();
	l_pLimitNumObj->SetLimitType(l_strLimitType);
	l_pLimitNumObj->SetLimitNum(l_strLimitNum);
	l_pLimitNumObj->SetLimitMin(l_strLimitMin);
	l_pLimitNumObj->SetLimitReason(l_strLimitReason);
	l_pLimitNumObj->SetStaffCode(l_strStaffCode);
	l_pLimitNumObj->SetStaffName(l_strStaffName);
	l_pLimitNumObj->SetBeginTime(l_strBegintime);
	l_pLimitNumObj->SetEndTime(l_strEndTime);
	l_pLimitNumObj->SetApprover(l_strApprover);
	l_pLimitNumObj->SetApprovedInfo(l_strApprovedInfo);
	l_pLimitNumObj->SetReceiptDeptName(l_strReceiptDeptName);
	l_pLimitNumObj->SetReceiptDeptDistrictCode(l_strReceiptDeptDistrictCode);
	// 更新内存
	CBlackListManager::Instance()->AddBlackList(l_strLimitNum, l_pLimitNumObj);
	// 通知交换机
	CBlackListManager::Instance()->SetBlackListToSwitch(l_strLimitType, l_strLimitNum);

	PROTOCOL::CSetBlackListRespond l_oSetBlackListRespond;
	l_oSetBlackListRespond.m_oHeader.m_strMsgId = l_oSetBlackListRequest.m_oHeader.m_strMsgId;
	l_oSetBlackListRespond.m_oHeader.m_strResult = Result_Success;
	/*l_oSetBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_SetBlackListRespond);
	l_oSetBlackListRespond.m_oBody.m_strResult = Result_Success;*/

	std::string l_strMessage = l_oSetBlackListRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send SetBlackList Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send SetBlackList Respond Failed: [%s]. Error: NotifiRequestPtr Invalid",
			l_strMessage.c_str());
	}
	
	Sync_SetBlackList(l_oSetBlackListRequest, l_bIsUpdate);
}
void CHytProtocol::CC_DeleteBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteBlackListRequest l_oDeleteBlackListRequest;
	if (!l_oDeleteBlackListRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid DeleteBlackList request protocol : [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oDeleteBlackListRequest.m_oHeader.m_strMsgId;
	std::string l_strLimitType = l_oDeleteBlackListRequest.m_oBody.m_strLimitType;
	std::string l_strLimitNum = l_oDeleteBlackListRequest.m_oBody.m_strLimitNum;

	CBlackListManager::Instance()->DeleteBlackList(l_strLimitNum);
	CBlackListManager::Instance()->DeleteBlackListFromSwitch(l_strLimitType, l_strLimitNum);

	PROTOCOL::CDeleteBlackListRespond l_oDeleteBlackListRespond;
	l_oDeleteBlackListRespond.m_oHeader.m_strMsgId = l_strMsgId;
	/*l_oDeleteBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_DeleteBlackListRespond);
	l_oDeleteBlackListRespond.m_oBody.m_strResult = Result_Success;*/

	std::string l_strMessage = l_oDeleteBlackListRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send DeleteBlackList Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send DeleteBlackList Respond Failed: [%s]. Error: NotifiRequestPtr Invalid",
			l_strMessage.c_str());
	}

	Sync_DeleteBlackList(l_strLimitType, l_strLimitNum);
}

void CHytProtocol::SyncCallRefIdToMrcc(const std::string& p_strCallRefId, const std::string& p_strOldCallRefId)
{
	PROTOCOL::CCallMrccEvent l_oRequestObj;
	l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_CallRefIdSyncMrcc, Queue_CTI_CallRefId_Mrcc, SendType_Queue);
	l_oRequestObj.m_oBody.m_strCallRefId = p_strCallRefId;
	l_oRequestObj.m_oBody.m_strOldCallRefId = p_strOldCallRefId;
	std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
	ICC_LOG_DEBUG(m_LogPtr, "Send  Sync Queue To Mrcc: [%s]", p_strCallRefId.c_str(), p_strOldCallRefId.c_str());
}

void CHytProtocol::CC_DeleteAllBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteAllBlackListRequest l_oDeleteAllBlackListRequest;
	if (!l_oDeleteAllBlackListRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid DeleteAllBlackList request protocol : [%s]", p_strNetMsg.c_str());

		return;
	}

	CBlackListManager::Instance()->ClearBlackList();
	CBlackListManager::Instance()->DeleteAllBlackListFromSwitch();

	PROTOCOL::CDeleteAllBlackListRespond l_oDeleteAllBlackListRespond;
	l_oDeleteAllBlackListRespond.m_oHeader.m_strMsgId = l_oDeleteAllBlackListRequest.m_oHeader.m_strMsgId;
	/*l_oDeleteAllBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_DeleteAllBlackListRespond);
	l_oDeleteAllBlackListRespond.m_oBody.m_strResult = Result_Success;*/

	std::string l_strMessage = l_oDeleteAllBlackListRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send DeleteAllBlackList Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send DeleteAllBlackList Respond Failed: [%s]. Error: NotifiRequestPtr Invalid",
			l_strMessage.c_str());
	}

	Sync_DeleteAllBlackList();
}

void CHytProtocol::CC_GetCurrentTime(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetCurrentTimeRespond l_oGetCurrentTimeRespond;

	time_t l_tCurrentTime = time(0);
	char l_szDayTime[256] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y-%m-%d %H:%M:%S", localtime(&l_tCurrentTime));
	std::string l_strTmpTime = std::string(l_szDayTime);

	l_oGetCurrentTimeRespond.m_oBody.m_strCurrentUTCTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oGetCurrentTimeRespond.m_oBody.m_strCurrentTime = l_strTmpTime;
	std::string l_strMessage = l_oGetCurrentTimeRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		//ICC_LOG_DEBUG(m_LogPtr, "Send GetCurrentTime Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
}

void CHytProtocol::CC_AgentHeartBeatTimeout(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAgentTimeoutSync l_oCAgentTimeoutSync;
	if (!l_oCAgentTimeoutSync.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AgentHeartBeatTimeout sysn protocol : [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strAgent = l_oCAgentTimeoutSync.m_oBody.m_strAgent;
	if (l_strAgent.empty())
	{
		return;
	}

	std::string l_strMsgId = "";
	std::string l_strACD;// = l_oCAgentTimeoutSync.m_oBody.m_strACD;
	std::string l_strReadyState = ReadyState_Busy;

	if (m_SwitchClientPtr)
	{
		long l_lTaskId = m_SwitchClientPtr->SetAgentState(l_strAgent, l_strACD, l_strReadyState.compare(ReadyState_Idle) == 0 ? ReadyState_Ready : ReadyState_NotReady);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_GetAllBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetAllBlackListRequest l_oGetAllBlackListRequest;
	if (!l_oGetAllBlackListRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetAllBlackList request protocol : [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strLimitNum = l_oGetAllBlackListRequest.m_oBody.m_strLimitNum;
	std::string l_strPageSize = l_oGetAllBlackListRequest.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oGetAllBlackListRequest.m_oBody.m_strPageIndex;

	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	PROTOCOL::CGetAllBlackListRespond l_oGetAllBlackListRespond;
	l_oGetAllBlackListRespond.m_oHeader.m_strMsgId = l_oGetAllBlackListRequest.m_oHeader.m_strMsgId;
	//l_oGetAllBlackListRespond.m_oHeader = CreateProtocolHeader(Cmd_GetAllBlackListRespond);
	CBlackListManager::Instance()->GetAllBlackList(l_oGetAllBlackListRespond, l_strLimitNum, l_nPageSize, l_nPageIndex);

	std::string l_strMessage = l_oGetAllBlackListRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send GetAllBlackList Respond: [%s]", l_strMessage.c_str());
		p_pNotifiRequest->Response(l_strMessage);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send GetAllBlackList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strMessage.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
void CHytProtocol::Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CCTIConnStateEvent l_oRequestObj;
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_CTIConnectStateSync, Topic_CTIConnectStateSync, SendType_Topic);
		l_oRequestObj.m_oBody.m_strState = p_pSwitchNotif->GetStreamOpen() ? ConnectState_Connect : ConnectState_Disconnect;
		l_oRequestObj.m_oBody.m_strSwitchType = p_pSwitchNotif->GetSwitchType();

		if (l_oRequestObj.m_oBody.m_strState == ConnectState_Disconnect)
		{
			CConferenceManager::Instance()->ClearConferenceList();
			CTransferCallManager::Instance()->ClearTransferCallList();
		}
		
		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send AESState Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}

void CHytProtocol::Sync_AgentState(IAgentStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CAgentStateEvent l_oRequestObj;

		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_ACDAgentStateSync, Queue_ACDAgentStateSync, SendType_Queue);

		std::string l_strReadyState = p_pSwitchNotif->GetReadyState();
		l_oRequestObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strLoginMode = p_pSwitchNotif->GetLoginMode();
		l_oRequestObj.m_oBody.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();

		std::string l_strAgentPhoneInfo;
		if (m_pRedisClient->HGet("AgentPhoneState", p_pSwitchNotif->GetAgentId(), l_strAgentPhoneInfo))
		{
			CAgentPhoneState l_CAgentPhoneState;
			l_CAgentPhoneState.ParseString(l_strAgentPhoneInfo, m_JsonFactoryPtr->CreateJson());
			if (!l_CAgentPhoneState.m_strSID.empty())
			{
				//如果是振铃或者通话状态
				if (l_CAgentPhoneState.m_strState == AGENT_PHONE_STATE_RINGING || l_CAgentPhoneState.m_strState == AGENT_PHONE_STATE_TALK)
				{
					//有电话呼入时忙闲状态不更新
				}
				else
				{
					if (l_oRequestObj.m_oBody.m_strReadyState == ReadyState_Busy)
					{
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_BUSY;
					}
					else
					{
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_FREE;
					}
					l_CAgentPhoneState.m_strUpdateTime = m_DateTimePtr->CurrentDateTimeStr();
					l_strAgentPhoneInfo = l_CAgentPhoneState.ToString(m_JsonFactoryPtr->CreateJson());
					m_pRedisClient->HSet("AgentPhoneState", p_pSwitchNotif->GetAgentId(), l_strAgentPhoneInfo);
					ICC_LOG_DEBUG(m_LogPtr, "HSet AgentPhoneState:[%s], [%s]", p_pSwitchNotif->GetAgentId().c_str(), l_strAgentPhoneInfo.c_str());
				}
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState,SID is empty, device:[%s]", p_pSwitchNotif->GetAgentId().c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState,Invalid not find device, device:[%s]", p_pSwitchNotif->GetAgentId().c_str());
		}

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send AgentState Sync Queue: [%s]", l_strSyncMsg.c_str());
	}
}
void CHytProtocol::Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CAgentStateEvent l_oRequestObj;

		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_ACDAgentStateSync, Topic_ACDAgentStateSync, SendType_Topic);

		std::string l_strReadyState = p_pSwitchNotif->GetReadyState();
		l_oRequestObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strLoginMode = p_pSwitchNotif->GetLoginMode();
		l_oRequestObj.m_oBody.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();

		l_oRequestObj.m_oBody.m_strMinOnlineNum = p_pSwitchNotif->GetMinOnlineNum();
		l_oRequestObj.m_oBody.m_strCurrOnlineNum  = p_pSwitchNotif->GetCurrOnlineNum();

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send AgentState Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}
void CHytProtocol::Event_CallState(ICallStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CCallStateEvent l_oRequestObj;

		long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
		std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();

		l_oRequestObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strOriginalACD = p_pSwitchNotif->GetACDAlias();
		l_oRequestObj.m_oBody.m_strACDDept = p_pSwitchNotif->GetACDDept();
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strState = p_pSwitchNotif->GetCallState();
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();
		l_oRequestObj.m_oBody.m_strCallRefId = l_strCTICallRefId;
		l_oRequestObj.m_oBody.m_strCSTACallRefId = std::to_string(l_lCSTACallRefId);

		std::string strACDDeptInfo;
		if (!m_pRedisClient->HGet("acd_dept", l_oRequestObj.m_oBody.m_strOriginalACD, strACDDeptInfo))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Hget acd_dept failed:[%s]!!!", l_oRequestObj.m_oBody.m_strOriginalACD.c_str());
		}

		PROTOCOL::CACDDeptInfo l_ACDDeptInfo;
		if (!l_ACDDeptInfo.Parse(strACDDeptInfo, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Parse acd_dept failed:[%s]!!!", strACDDeptInfo.c_str());
		}
		l_oRequestObj.m_oBody.m_strOriginalACDDept = l_ACDDeptInfo.m_strDeptCode;

		//
		if (l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_QUEUE]) //CTI_WAITING "waiting" || l_oRequestObj.m_oBody.m_strState == "incoming"
		{
			CWaitCallInfo l_WaitCallInfo;
			l_WaitCallInfo.m_strACD = l_oRequestObj.m_oBody.m_strACD;
			l_WaitCallInfo.m_strACDDept = _QueryAcdDept(l_oRequestObj.m_oBody.m_strOriginalACD);
			l_WaitCallInfo.m_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
			l_WaitCallInfo.m_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
			l_WaitCallInfo.m_strOriginalACD = l_oRequestObj.m_oBody.m_strOriginalACD;
			l_WaitCallInfo.m_strTime = l_oRequestObj.m_oBody.m_strTime;
			l_WaitCallInfo.m_strOriginalACDDept = l_oRequestObj.m_oBody.m_strOriginalACDDept;
			
			std::string l_strData = l_WaitCallInfo.ToString(m_JsonFactoryPtr->CreateJson());
			m_pRedisClient->HSet("WaitingCallList", l_strCTICallRefId, l_strData);

			ICC_LOG_DEBUG(m_LogPtr, "WaitingCallList write redis: [%s]", l_strData.c_str());
		}
		else if (l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_FREE] ||
			l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_TALK]  ||
			l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_RELEASE] ||
			l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_CALLOVER] ||
			l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_RING] ||
			l_oRequestObj.m_oBody.m_strState == CallStateString[STATE_ASSIGN])
		{
			m_pRedisClient->HDel("WaitingCallList", l_strCTICallRefId);
		}

		//	发送话务状态给DBProcess
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_ACDCallStateSync, Queue_CTIDBProcess, SendType_Queue);

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send CallState Sync Queue: [%s]", l_strSyncMsg.c_str());

		//	广播话务状态给接警席
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_ACDCallStateSync, Topic_ACDCallStateSync, SendType_Topic);

		l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send CallState Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}
void CHytProtocol::Event_CallOver(ICallOverNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CCallOverEvent l_oRequestObj;
		
		l_oRequestObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strOriginalACD = p_pSwitchNotif->GetACDAlias();
		l_oRequestObj.m_oBody.m_strACDDept = p_pSwitchNotif->GetACDDept();
		l_oRequestObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strCallDirection = p_pSwitchNotif->GetCallDirection();
		l_oRequestObj.m_oBody.m_strDialTime = p_pSwitchNotif->GetDialTime();
		l_oRequestObj.m_oBody.m_strIncomingTime = p_pSwitchNotif->GetIncomingTime();
		l_oRequestObj.m_oBody.m_strAssignTime = p_pSwitchNotif->GetAssignTime();
		l_oRequestObj.m_oBody.m_strRingTime = p_pSwitchNotif->GetRingTime();
		l_oRequestObj.m_oBody.m_strRingBackTime = p_pSwitchNotif->GetSignalBackTime();
		l_oRequestObj.m_oBody.m_strReleaseTime = p_pSwitchNotif->GetReleaseTime();
		l_oRequestObj.m_oBody.m_strTalkTime = p_pSwitchNotif->GetTalkTime();
		l_oRequestObj.m_oBody.m_strHangupTime = p_pSwitchNotif->GetHangupTime();
		l_oRequestObj.m_oBody.m_strHangupType = p_pSwitchNotif->GetHangupType();
		l_oRequestObj.m_oBody.m_strSwitchType = p_pSwitchNotif->GetSwitchType();

		l_oRequestObj.m_oBody.m_strRecordFilePath = p_pSwitchNotif->GetRecordFilePath();
		if (l_oRequestObj.m_oBody.m_strHangupType == "release")
		{
			if ((l_oRequestObj.m_oBody.m_strTalkTime.empty() && l_oRequestObj.m_oBody.m_strHangupTime.empty()) || (l_oRequestObj.m_oBody.m_strRingTime.empty() && l_oRequestObj.m_oBody.m_strRingBackTime.empty()))
			{
				l_oRequestObj.m_oBody.m_strReleaseType = "wait";
			}
			else {
				l_oRequestObj.m_oBody.m_strReleaseType = "ring";
			}
		}
		m_pRedisClient->HDel("WaitingCallList", p_pSwitchNotif->GetCTICallRefId());

		std::string m_strAcdDept;
		m_pRedisClient->HGet("acd_dept", l_oRequestObj.m_oBody.m_strACD, m_strAcdDept);
		l_oRequestObj.m_oBody.m_strACDDeptEx = m_strAcdDept;

		//	发送话务结束状态给DBProcess
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_CallOverSync, Queue_CTIDBProcess, SendType_Queue);

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send CallOverEvent Sync Queue: [%s]", l_strSyncMsg.c_str());

		//	广播话务结束状态给接警席
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_CallOverSync, Topic_CallOverSync, SendType_Topic);

		l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send CallOverEvent Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}

void CHytProtocol::Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif,const std::string &l_strConferenceId, const std::string& p_strConferenceCompere, const std::string& l_ConfBargeinSp)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CDeviceStateEvent l_oRequestObj;
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strDevice = p_pSwitchNotif->GetDeviceNum();
		l_oRequestObj.m_oBody.m_strDeviceType = p_pSwitchNotif->GetDeviceType();
		l_oRequestObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
		l_oRequestObj.m_oBody.m_strState = p_pSwitchNotif->GetDeviceState();
		l_oRequestObj.m_oBody.m_strTransferType = p_pSwitchNotif->GetTransferType();
		std::string l_strCTICallId = l_oRequestObj.m_oBody.m_strCallRefId;
		if (m_bFSAesMode && !l_strCTICallId.empty())
		{
			std::string l_strBargeInSponsor = "";
			std::string l_strCompere = "";
			std::string l_strConferenceId = p_pSwitchNotif->GetConferenceId();// = m_SwitchClientPtr->QueryConferenceByCallid(l_strCTICallId, l_strCompere, l_strBargeInSponsor);
			if (!l_strConferenceId.empty() && l_oRequestObj.m_oBody.m_strState != CallStateString[STATE_SILENTMONITOR])
			{
				l_oRequestObj.m_oBody.m_strConferenceId = l_strConferenceId;
				l_oRequestObj.m_oBody.m_strConferenceCompere = p_pSwitchNotif->GetConfCompere();

				l_strBargeInSponsor = p_pSwitchNotif->GetConfBargeinSponsor();
				if (!l_strBargeInSponsor.empty())
				{
					l_oRequestObj.m_oBody.m_strConfBargeinSponsor = l_strBargeInSponsor;
				}

				if (p_pSwitchNotif->GetDeviceState() == CallStateString[STATE_TALK] && l_strBargeInSponsor.empty())
				{
					l_oRequestObj.m_oBody.m_strState = CallStateString[STATE_CONFERENCE];
				}
			}
		}
		else
		{
			if (!l_strConferenceId.empty())
			{
				l_oRequestObj.m_oBody.m_strConferenceId = l_strConferenceId;
				l_oRequestObj.m_oBody.m_strConferenceCompere = p_strConferenceCompere;
				l_oRequestObj.m_oBody.m_strConfBargeinSponsor = l_ConfBargeinSp;

				if (p_pSwitchNotif->GetDeviceState() == CallStateString[STATE_TALK] && l_ConfBargeinSp.empty())
				{
					l_oRequestObj.m_oBody.m_strState = CallStateString[STATE_CONFERENCE];
				}
			}
			else
			{
				if (p_pSwitchNotif->GetDeviceState() == CallStateString[STATE_CONFERENCE])
				{
					l_oRequestObj.m_oBody.m_strState = CallStateString[STATE_TALK];
				}
			}
		}
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strOriginalCallerId = p_pSwitchNotif->GetOriginalCallerId();
		l_oRequestObj.m_oBody.m_strOriginalCalledId = p_pSwitchNotif->GetOriginalCalledId();
		l_oRequestObj.m_oBody.m_strCallDirection = p_pSwitchNotif->GetCallDirection();
		
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();
		/*l_oRequestObj.m_oBody.m_strHangupType = p_pSwitchNotif->GetHangupType();
		l_oRequestObj.m_oBody.m_strHangupDevice = p_pSwitchNotif->GetHangupDevice();*/
		l_oRequestObj.m_oBody.m_strCSTACallRefId = std::to_string(p_pSwitchNotif->GetCSTACallRefId());
		l_oRequestObj.m_oBody.m_strBeginTalkTime = p_pSwitchNotif->GetTalkTime();

		l_oRequestObj.m_oBody.m_strRecordFilePath = p_pSwitchNotif->GetRecordFilePath();
		l_oRequestObj.m_oBody.m_strSwitchAddress =  p_pSwitchNotif->GetSwitchAddress();
		std::string l_strAgentPhoneInfo;
		if (m_pRedisClient->HGet("AgentPhoneState", p_pSwitchNotif->GetDeviceNum(), l_strAgentPhoneInfo))
		{
			CAgentPhoneState l_CAgentPhoneState;
			if (l_CAgentPhoneState.ParseString(l_strAgentPhoneInfo, m_JsonFactoryPtr->CreateJson()))
			{
				if (!l_CAgentPhoneState.m_strSID.empty())
				{
					bool l_bFlag = false;
					std::string l_strDeviceState = p_pSwitchNotif->GetDeviceState();
					if (l_strDeviceState == "talkstate")
					{
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_TALK;
						l_bFlag = true;
					}
					else if (l_strDeviceState == "ringstate")
					{
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_RINGING;
						l_bFlag = true;
					}
					else if (l_strDeviceState == "dialstate")
					{
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_OFFHOOK; //摘机
						l_bFlag = true;
					}
					else if (l_strDeviceState == "hangupstate")
					{
						l_bFlag = true;
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_HANGUP;
					}
					else if (l_strDeviceState == "freestate")
					{
						l_bFlag = true;
						l_CAgentPhoneState.m_strState = AGENT_PHONE_STATE_FREE;
					}

					//l_oRequestObj.m_oBody.m_strDeptName = l_CAgentPhoneState.m_strBMMC;
					//l_oRequestObj.m_oBody.m_strDeptCode = l_CAgentPhoneState.m_strBMDM;					

					if (l_bFlag)
					{
						l_CAgentPhoneState.m_strUpdateTime = m_DateTimePtr->CurrentDateTimeStr();
						l_strAgentPhoneInfo = l_CAgentPhoneState.ToString(m_JsonFactoryPtr->CreateJson());
						m_pRedisClient->HSet("AgentPhoneState", p_pSwitchNotif->GetDeviceNum(), l_strAgentPhoneInfo);
						ICC_LOG_DEBUG(m_LogPtr, "HSet AgentPhoneState:[%s], [%s]", p_pSwitchNotif->GetDeviceNum().c_str(), l_strAgentPhoneInfo.c_str());
					}
					else
					{
						ICC_LOG_DEBUG(m_LogPtr, "HGet AgentPhoneState,DeviceNum:[%s], DeviceState:[%s]", p_pSwitchNotif->GetDeviceNum().c_str(), l_strDeviceState.c_str());
					}
				}
				else
				{
					ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState, SID is empty:[%s]", l_strAgentPhoneInfo.c_str());
				}
				
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState data Invalid:[%s]", l_strAgentPhoneInfo.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "HGet AgentPhoneState,Invalid not find device, device:[%s]", p_pSwitchNotif->GetDeviceNum().c_str());
		}

		std::string l_strSeatInfo;
		if (m_pRedisClient->HGet("SeatInfo", l_oRequestObj.m_oBody.m_strDevice, l_strSeatInfo))
		{
			CSeatInfo cSeatInfo;
			if (cSeatInfo.Parse(l_strSeatInfo, m_JsonFactoryPtr->CreateJson()))
			{
				l_oRequestObj.m_oBody.m_strDeptCode = cSeatInfo.m_strDeptCode;
				l_oRequestObj.m_oBody.m_strDeptName = cSeatInfo.m_strDeptName;
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "HGet SeatInfo, parse json failed:[%s]", l_strSeatInfo.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "HGet SeatInfo, get seat info failed:[%s]", l_oRequestObj.m_oBody.m_strDevice.c_str());
		}

		if (p_pSwitchNotif->GetDeviceState() == CallStateString[STATE_TALK])
		{
			std::string l_strHeldCallRefId = _GetGeneralConsultationInfo(p_pSwitchNotif->GetCTICallRefId());
			if (!l_strHeldCallRefId.empty())
			{
				
				l_oRequestObj.m_oBody.m_strTransferType = "2"; //常规转移标示
			}
		}

		//	发送设备状态给DBProcess
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_DeviceStateSync, Queue_CTIDBProcess, SendType_Queue);

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send DeviceState Sync Queue,device:[%s]: [%s]", l_oRequestObj.m_oBody.m_strDevice.c_str(), l_strSyncMsg.c_str());

		//	广播设备状态给接警席
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_DeviceStateSync, Topic_DeviceStateSync, SendType_Topic);

		l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send DeviceState Sync Topic: [%s]", l_strSyncMsg.c_str());

		if (p_pSwitchNotif->GetDeviceState() == CallStateString[STATE_HANGUP])
		{
			std::string l_strHeldCallRefId = _GetGeneralConsultationInfo(p_pSwitchNotif->GetCTICallRefId());
			if (!l_strHeldCallRefId.empty())
			{
				std::string l_strCalledState = m_SwitchClientPtr->GetDeviceState(p_pSwitchNotif->GetCalledId());
				bool l_bReconnectCall = true;
				if (!l_strCalledState.empty() && p_pSwitchNotif->GetCalledId() != p_pSwitchNotif->GetDeviceNum())
				{
					l_bReconnectCall = false;
				}

				if (l_bReconnectCall)
				{
					DeleteGeneralConsultationInfo(p_pSwitchNotif->GetCTICallRefId());
					std::string l_strDevice = p_pSwitchNotif->GetDeviceNum();
					//发取回
					long l_lRequestId = m_SwitchClientPtr->ReconnectCall(p_pSwitchNotif->GetCTICallRefId(), l_strHeldCallRefId, l_strDevice, l_strDevice);

					ICC_LOG_DEBUG(m_LogPtr, "ReconnectCall,ActiveCallRefId:[%s],HeldCallRefId:[%s],requestId:[%d]",
						p_pSwitchNotif->GetCTICallRefId().c_str(), l_strHeldCallRefId.c_str(), l_lRequestId);
				}
			}
		}
		
		//_SyncAgentState(p_pSwitchNotif);
	}
}

bool CHytProtocol::_IsAutoUrgeCallNum(std::string strCaller)
{
	if (m_strAutoUrgeCallNumber.empty())
	{
		if (!m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", m_strAutoUrgeCallNumber))
		{
			ICC_LOG_ERROR(m_LogPtr, "Get AutoUrge Call Num Error, Use Default Configuration : 8888");
			m_strAutoUrgeCallNumber = "8888";
		}
		else
		{
			ICC_LOG_DEBUG(m_LogPtr, "Get AutoUrge Call Num %s", m_strAutoUrgeCallNumber.c_str());
		}
	}

	if (m_strAutoUrgeCallNumber == strCaller)
	{
		return true;
	}

	return false;
}

void CHytProtocol::Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CRefuseCallEvent l_oRequestObj;

		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_BlackListRefuseCallSync, Topic_RefuseCallSync, SendType_Topic);

		//long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();

		l_oRequestObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strRefuseReason = p_pSwitchNotif->GetRefuseReason();
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send RefuseCall Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}
void CHytProtocol::Event_Failed(IFailedEventNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		return; 
		// 通知客户端
		PROTOCOL::CFailedCallEvent l_oRequestObj;
		l_oRequestObj.m_oHeader = CreateProtocolHeader("call_failed_sync", "topic_call_failed_sync", SendType_Topic);
		l_oRequestObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strErrorCode = std::to_string(p_pSwitchNotif->GetErrorCode());
		l_oRequestObj.m_oBody.m_strErrorMsg = p_pSwitchNotif->GetErrorMsg();
		l_oRequestObj.m_oBody.m_strFailedDeviceNum = p_pSwitchNotif->GetFailedDeviceNum();

		std::string l_strFailedSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strFailedSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send RefuseCall Sync Topic: [%s]", l_strFailedSyncMsg.c_str());
	}
}

std::string CHytProtocol::BuildUrgeId()
{
	std::string l_curTime = m_DateTimePtr->CurrentDateTimeStr();

	for (auto l_itera = l_curTime.begin(); l_itera != l_curTime.end();)
	{
		if (*l_itera >= '0' && *l_itera <= '9')
			l_itera++;
		else
			l_itera = l_curTime.erase(l_itera);
	}

	l_curTime += std::to_string(rand() % 1000000);

	return l_curTime;
}

void CHytProtocol::Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		//使用自动催办呼叫该黑名单号码进行提示
		PROTOCOL::CBlackTransferEvent l_oRequestObj;
		std::string p_strCallId = BuildUrgeId();
		std::string p_strSmsId = BuildUrgeId();
		while (p_strCallId == p_strSmsId)
			p_strSmsId = BuildUrgeId();

		l_oRequestObj.m_oHeader = CreateProtocolHeader("add_auto_urge_call_request", "alarm_process_timeout_sync", SendType_Topic);

		l_oRequestObj.m_oBody.m_strAlarmId = "000000";
		l_oRequestObj.m_oBody.m_strCreateUser = "black_call";
		l_oRequestObj.m_oBody.m_strDepartment = "000000";
		l_oRequestObj.m_oBody.m_strRemarks = p_pSwitchNotif->GetCalled();
		l_oRequestObj.m_oBody.m_strSeatNo = "000000";
		l_oRequestObj.m_oBody.m_strSMSContent = "301";
		l_oRequestObj.m_oBody.m_strTTSContent = "";
		l_oRequestObj.m_oBody.m_strUpdateUser = "";
		l_oRequestObj.m_oBody.m_strUrgeCount = "1";
		l_oRequestObj.m_oBody.m_strUrgeId = p_strCallId + ";" + p_strSmsId;
		l_oRequestObj.m_oBody.m_strUrgeInterval = "10";
		l_oRequestObj.m_oBody.m_strUrgeLevel = "1";
		l_oRequestObj.m_oBody.m_strUrgeStartTime = m_DateTimePtr->CurrentDateTimeStr();
		l_oRequestObj.m_oBody.m_strVoiceId = "301";
		l_oRequestObj.m_oBody.m_strVoiceType = "0;2";
		l_oRequestObj.m_oBody.m_strUrgeCalled = p_pSwitchNotif->GetCaller();
		l_oRequestObj.m_oBody.m_strExtensionNo = std::to_string(rand() % 100000); //五位随机数
		
		/*m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strBlackTransferSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send add_auto_urge_call_request Sync Topic: [%s]", l_strBlackTransferSyncMsg.c_str());*/

		//将消息中心发送转至Server发送
		l_oRequestObj.m_oHeader = CreateProtocolHeader("black_info_send", "topic_call_over_sync", SendType_Topic);
		std::string l_strBlackTransferSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strBlackTransferSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send black_info_send Sync Topic: [%s]", l_strBlackTransferSyncMsg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
// 具体命令执行结果反馈( CR-- Command Result)
std::string CHytProtocol::ErrorCodeToString(int p_nErrorCode)
{
	std::string l_strErrorCode = Result_Failed;

	switch (p_nErrorCode)
	{
	case ERROR_CMD_SUCCESS:
		l_strErrorCode = std::to_string(ERROR_CMD_SUCCESS);			//	成功
		break;
	case ERROR_NET_DISCONNECT:
		l_strErrorCode = std::to_string(ERROR_NET_DISCONNECT);		//	CTI 与 交换机网络连接断开
		break;
	case ERROR_INVALID_DEVICE:
	case ERROR_INVALID_CALLING_DEVICE:
	case ERROR_INVALID_CALLED_DEVICE:
	case ERROR_INVALID_CSTA_DEVICE_IDENTIFIER:
		l_strErrorCode = std::to_string(ERROR_INVALID_DEVICE);		//	设备无效
		break;
	case ERROR_INVALID_CALL:
	case ERROR_INVALID_CSTA_CALL_IDENTIFIER:
	case ERROR_INVALID_CSTA_CONNECTION_IDENTIFIER:
	case ERROR_INVALID_CONNECTION_ID_FOR_ACTIVE_CALL:
	case ERROR_NO_ACTIVE_CALL:
	case ERROR_NO_HELD_CALL:
	case ERROR_NO_CALL_TO_CLEAR:
	case ERROR_NO_CONNECTION_TO_CLEAR:
	case ERROR_NO_CALL_TO_ANSWER:
		l_strErrorCode = std::to_string(ERROR_INVALID_CALL);		//	话务不存在
		break;
	case ERROR_MEMBER_LIMIT:
	case ERROR_CONFERENCE_MEMBER_LIMIT_EXCEEDED:					//	会议成员数量达到最大值
		l_strErrorCode = std::to_string(ERROR_MEMBER_LIMIT);
		break;
	case ERROR_DEVICE_BUSY:
	case ERROR_SERVICE_BUSY:
	case ERROR_RESOURCE_BUSY:
		l_strErrorCode = std::to_string(ERROR_DEVICE_BUSY);			//	设备忙
		break;
	case ERROR_DEST_NOT_CONNECT:
	case ERROR_INVALID_DESTINATION:									//	被叫无法接通
		l_strErrorCode = std::to_string(ERROR_DEST_NOT_CONNECT);
		break;
	case ERROR_MIN_ONLINE_AGENT:									//	最小在席数限制
		l_strErrorCode = std::to_string(ERROR_MIN_ONLINE_AGENT);
		break;
	case ERROR_NO_READY_AGENT:										//	无空闲坐席
		l_strErrorCode = std::to_string(ERROR_NO_READY_AGENT);
		break;
	case ERROR_RING_TIMEOUT:										//	振铃超时
		l_strErrorCode = std::to_string(ERROR_RING_TIMEOUT);
		break;
	case ERROR_CALLER_HANGUP:										//	报警人挂机
		l_strErrorCode = std::to_string(ERROR_CALLER_HANGUP);
		break;
	case ERROR_CANCEL_TRANSFER_CALL:								//	取消转警
		l_strErrorCode = std::to_string(ERROR_CANCEL_TRANSFER_CALL);
		break;	
	case ERROR_CALL_HANGUP:											//	转警方或目标方挂机
		l_strErrorCode = std::to_string(ERROR_CALL_HANGUP);
		break;	
	case ERROR_OPERATION_TRANSFER:									//	转移操作失败
		l_strErrorCode = std::to_string(ERROR_OPERATION_TRANSFER);
		break;
	default:														//	未知错误
		l_strErrorCode = std::to_string(ERROR_UNKNOWN);
		break;
	}

	return l_strErrorCode;
}
void CHytProtocol::CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CAgentLoginModeRespond l_oRespondObj;

			std::string l_strAgent = p_pSwitchNotif->GetAgentId();
			std::string l_strACD = p_pSwitchNotif->GetACDGrp();
			std::string l_strLoginMode = p_pSwitchNotif->GetLoginMode();
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());;
			l_oRespondObj.m_oBody.m_strAgent = l_strAgent;
			l_oRespondObj.m_oBody.m_strACD = l_strACD;
			l_oRespondObj.m_oBody.m_strLoginMode = l_strLoginMode;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_AgentLogin Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_AgentLogin Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CAgentLoginModeRespond l_oRespondObj;

			std::string l_strAgent = p_pSwitchNotif->GetAgentId();
			std::string l_strACD = p_pSwitchNotif->GetACDGrp();
			std::string l_strLoginMode = p_pSwitchNotif->GetLoginMode();
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oBody.m_strAgent = l_strAgent;
			l_oRespondObj.m_oBody.m_strACD = l_strACD;
			l_oRespondObj.m_oBody.m_strLoginMode = l_strLoginMode;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_AgentLogout Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_AgentLogout Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CSetAgentStateRespond l_oRespondObj;

			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			std::string l_strReadyState = p_pSwitchNotif->GetReadyState();
			l_oRespondObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
			l_oRespondObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
			l_oRespondObj.m_oBody.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_SetAgentState Respond,Agent: [%s],ReadyState: [%s],l_strResult: [%s]", 
				l_lRequestId,
				l_oRespondObj.m_oBody.m_strAgent.c_str(),
				l_oRespondObj.m_oBody.m_strReadyState.c_str(),
				l_strResult.c_str());

			if (!l_oRespondObj.m_oHeader.m_strMsgId.empty())
			{
				std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
				if (l_pRequestMemory->GetNotifiRequest())
				{
					ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_SetAgentState Respond: [%s]",
						l_lRequestId, l_strRespondMsg.c_str());
					l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
				}
				else
				{
					ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_SetAgentState Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
						l_lRequestId, l_strRespondMsg.c_str());
				}
			}
			

			if (!p_pSwitchNotif->GetResult())
			{
				_SendSetAgentStateFailedSync(l_oRespondObj, l_strResult);
			}
			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::_SendSetAgentStateFailedSync(const PROTOCOL::CSetAgentStateRespond &p_oRespondObj,const std::string & p_strResult)
{
	PROTOCOL::CSetAgentStateSetFailedSync l_oFailedSynObj;
	//发同步消息
	l_oFailedSynObj.m_oHeader.m_strMsgid = m_StringUtilPtr->CreateGuid();
	l_oFailedSynObj.m_oHeader.m_strCmd = "set_agent_state_failed_sync";
	l_oFailedSynObj.m_oHeader.m_strRequest = "topic_acd_agent_state_sync";
	l_oFailedSynObj.m_oHeader.m_strRequestType = "1";//主题
	l_oFailedSynObj.m_oHeader.m_strSendTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oFailedSynObj.m_oHeader.m_strRequestFlag = "MQ";

	l_oFailedSynObj.m_oBody.m_strACD = p_oRespondObj.m_oBody.m_strACD;
	l_oFailedSynObj.m_oBody.m_strAgent = p_oRespondObj.m_oBody.m_strAgent;
	l_oFailedSynObj.m_oBody.m_strReadyState = p_oRespondObj.m_oBody.m_strReadyState;
	l_oFailedSynObj.m_oBody.m_strCase = p_strResult;
	std::string l_strSyncMessage = l_oFailedSynObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_LogPtr, "SetAgentStateFailed Sync: [%s]", l_strSyncMessage.c_str());
}

void CHytProtocol::Sync_Callback(const std::string& p_strCTICallRefId, const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strCaseId,
	const std::string& p_strRelateCallRefId, const std::string& p_strReceiptCode, const std::string& p_strReceiptName)
{
	PROTOCOL::CCallBackSync l_oSyncObj;
	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_CallbackSync, Queue_CTIDBProcess, SendType_Queue);
	l_oSyncObj.m_oBody.m_strCallRefId = p_strCTICallRefId;
	l_oSyncObj.m_oBody.m_strCallerId = p_strCallerId;
	l_oSyncObj.m_oBody.m_strCalledId = p_strCalledId;
	l_oSyncObj.m_oBody.m_strCaseId = p_strCaseId;
	l_oSyncObj.m_oBody.m_strRelateCallRefId = p_strRelateCallRefId;
	l_oSyncObj.m_oBody.m_strReceiptCode = p_strReceiptCode;
	l_oSyncObj.m_oBody.m_strReceiptName = p_strReceiptName;

	std::string l_strSyncMsg = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
	ICC_LOG_DEBUG(m_LogPtr, "Send Calback Sync Queue: [%s]", l_strSyncMsg.c_str());
}

void CHytProtocol::CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			std::string l_strCaseId = l_pRequestMemory->GetCaseId();
			std::string l_strRelateCallRefId = l_pRequestMemory->GetRelateCallRefId();
			std::string l_strReceiptCode = l_pRequestMemory->GetReceiptCode();
			std::string l_strReceiptName = l_pRequestMemory->GetReceiptName();

			bool l_bResult = p_pSwitchNotif->GetResult();
		//	long l_lCSTACallRefId = p_pSwitchNotif->GetCSTACallRefId();
			std::string l_strCallerId = p_pSwitchNotif->GetCallerId();
			std::string l_strCalledId = p_pSwitchNotif->GetCalledId();
			std::string l_strCTICallRefId = p_pSwitchNotif->GetCTICallRefId();
			std::string l_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			PROTOCOL::CMakeCallRespond l_oRespondObj;
			l_oRespondObj.m_oBody.m_strCallRefId = l_strCTICallRefId;
			l_oRespondObj.m_oBody.m_strCallerId = l_strCallerId;
			l_oRespondObj.m_oBody.m_strCalledId = l_strCalledId;
			l_oRespondObj.m_oBody.m_strCaseId = l_strCaseId;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_MakeCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_MakeCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			//	回拨记录存库
			this->Sync_Callback(l_strCTICallRefId, l_strCallerId, l_strCalledId, l_strCaseId, l_strRelateCallRefId, l_strReceiptCode, l_strReceiptName);

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CAnswerCallRespond l_oRespondObj;

			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			
			/*l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->>GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strDevice = p_pSwitchNotif->GetDeviceNum();*/
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_AnswerCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_AnswerCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CAnswerCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());	
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_RefuseAnswer Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_RefuseAnswer Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CHangupRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());			
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_Hangup Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_Hangup Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CClearCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ClearCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ClearCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CListenCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			/*
			l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ListenCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ListenCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CPickupCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			/*
			l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_PickupCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_PickupCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CBargeInCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());	
		
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			std::string l_strSponsor;
			int l_bFlag = 0;

			if (l_strResult == Result_Success)
			{
				std::string l_strMsg = l_pRequestMemory->GetNotifiRequest()->GetMessages();

				std::string l_strRelateCallRefId = l_pRequestMemory->GetRelateCallRefId();
				PROTOCOL::CBargeInCallRequest l_oRequestObj;

				l_bFlag = 1;
				if (l_oRequestObj.ParseString(l_strMsg, m_JsonFactoryPtr->CreateJson()))
				{
					l_bFlag = 2;
					l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
					CConferenceManager::Instance()->AddConferencePartyNew(l_oRequestObj.m_oBody.m_strCallRefId,
						l_oRequestObj.m_oBody.m_strSponsor,
						l_oRequestObj.m_oBody.m_strTarget,
						TARGET_DEVICE_TYPE_INSIDE, true);
				}

				ICC_LOG_DEBUG(m_LogPtr, "CR_BargeInCall msg=%s,Sponsor=%s,l_bFlag=%d,l_strRelateCallRefId=%s", l_strMsg.c_str(), l_strSponsor.c_str(), l_bFlag, l_strRelateCallRefId.c_str());
			}

			
			

			/*l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_BargeInCall Respond: [%s],Sponsor=%s",
					l_lRequestId, l_strRespondMsg.c_str(), l_strSponsor.c_str());

				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_BargeInCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}


void CHytProtocol::CR_ConsultationCall(IConsultationCallResultNotifPtr p_pRespondNotif)
{
	if (p_pRespondNotif)
	{
		bool l_bResult = p_pRespondNotif->GetResult();
		long l_lRequestId = p_pRespondNotif->GetRequestId();
		std::string l_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pRespondNotif->GetErrorCode());
		std::string l_strSponsor = p_pRespondNotif->GetSponsor();
		std::string l_strTargetDevice = p_pRespondNotif->GetTargetDevice();
		std::string l_strActiveCallRefId = p_pRespondNotif->GetActiveCTICallRefId();
		std::string l_strHeldCallRefId = p_pRespondNotif->GetHeldCTICallRefId();

		if (l_strResult == Result_Success)
		{
			_AddGeneralConsultationInfo(l_strActiveCallRefId, l_strHeldCallRefId);
		}

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CConsultationCallRespond l_oRespondObj;		

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strActiveCallRefId = l_strActiveCallRefId;
			l_oRespondObj.m_oBody.m_strHeldCallRefId = l_strHeldCallRefId;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oRespondObj.m_oBody.m_strTarget = l_strTargetDevice;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ConsultationCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ConsultationCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_TransferCall(ITransferCallResultNotifPtr p_pRespondNotif)
{
	if (p_pRespondNotif)
	{
		std::string l_strSponsor = p_pRespondNotif->GetSponsor();
		std::string l_strTargrt = p_pRespondNotif->GetTargetDevice();
		std::string l_strResult = p_pRespondNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pRespondNotif->GetErrorCode());
		std::string l_strActiveCallRefId = p_pRespondNotif->GetActiveCTICallRefId();
		std::string l_strHeldCallRefId = p_pRespondNotif->GetHeldCTICallRefId();

		long l_lRequestId = p_pRespondNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CTransferCallRespond l_oRespondObj;
	
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			/*l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();*/
			l_oRespondObj.m_oBody.m_strHeldCallRefId = l_strHeldCallRefId;
			l_oRespondObj.m_oBody.m_strAvtiveCallRefId = l_strActiveCallRefId;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oRespondObj.m_oBody.m_strTarget = l_strTargrt;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_TransferCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_TransferCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}
			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::Sync_TransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, const std::string& p_strSponsor,
	const std::string& p_strTarget, const std::string& p_strRelatedId, const int p_nResult)
{
	PROTOCOL::CTransferCallRespond l_oSyncObj;

	std::string l_strResult = (p_nResult == Result_GetReadyAgent_Success ? std::to_string(Result_GetReadyAgent_Success) : ErrorCodeToString(p_nResult));
	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExSync, Topic_TransferCallExSync, SendType_Topic);

	l_oSyncObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	l_oSyncObj.m_oBody.m_strAvtiveCallRefId = p_strActiveCallRefId;
	l_oSyncObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oSyncObj.m_oBody.m_strTarget = p_strTarget;
	l_oSyncObj.m_oBody.m_strResult = l_strResult;
	l_oSyncObj.m_oHeader.m_strResult = l_strResult;

	std::string l_strSyncMessage = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_LogPtr, "Send TransferCall Sync: [%s]", l_strSyncMessage.c_str());
}
void CHytProtocol::CR_TransferCallEx(long p_lRequestId, const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strResult)
{
	boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(p_lRequestId);
	if (l_pRequestMemory)
	{
		PROTOCOL::CTransferCallRespond l_oRespondObj;
		l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
		
		l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
		l_oRespondObj.m_oBody.m_strAvtiveCallRefId = p_strActiveCallRefId;
		l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
		l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
		l_oRespondObj.m_oBody.m_strResult = p_strResult;
		l_oRespondObj.m_oHeader.m_strResult = p_strResult;

		std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
		if (l_pRequestMemory->GetNotifiRequest())
		{
			ICC_LOG_DEBUG(m_LogPtr, "Send CR_TransferCallEx Respond: [%s]", l_strRespondMsg.c_str());
			l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "Send CR_TransferCallEx Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
				l_strRespondMsg.c_str());
		}
		CRequestMemoryManager::Instance()->DeleteRequestMemory(p_lRequestId);
	}
}

void CHytProtocol::CR_TransferCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId, bool p_bResult)
{
	PROTOCOL::CTransferCallRespond l_oRespondObj;
	std::string l_strResult = (p_bResult ? Result_Success : Result_Failed);
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExRespond, "", SendType_Queue, p_strMsgId);
	if (p_bResult)
	{
		l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	}
	l_oRespondObj.m_oBody.m_strAvtiveCallRefId = "";
	l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
	l_oRespondObj.m_oBody.m_strResult = l_strResult;
	l_oRespondObj.m_oHeader.m_strResult = l_strResult;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_TransferCallEx Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send CR_TransferCallEx Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}

void CHytProtocol::CR_CancelTransferCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strHeldCallRefId,
	const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId)
{
	PROTOCOL::CTransferCallRespond l_oRespondObj;
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_CancelTransferCallExRespond, "", SendType_Queue, p_strMsgId);

	l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	l_oRespondObj.m_oBody.m_strAvtiveCallRefId = "";
	l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
	l_oRespondObj.m_oBody.m_strResult = Result_Success;
	l_oRespondObj.m_oHeader.m_strResult = Result_Success;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_CancelTransferCallEx Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send CR_CAncelTransferCallEx Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}

void CHytProtocol::CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CForcePopCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ForcePopCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ForcePopCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}
			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory != nullptr)
		{
			PROTOCOL::CDeflectCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_DeflectCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_DeflectCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		bool l_bResult = p_pSwitchNotif->GetResult();
		long l_lRequestId = p_pSwitchNotif->GetRequestId();

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			//	普通保留
			PROTOCOL::CHoldCallRespond l_oRespondObj;
			std::string l_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_HoldCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_HoldCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CRetrieveCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_RetrieveCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_RetrieveCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}
			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CReconnectCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ReconnectCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ReconnectCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CConferenceCallRespond l_oRespondObj;
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();

			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_ConferenceCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_ConferenceCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_MakeConference(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest, bool p_bIsSuccess)
{
	PROTOCOL::CMakeConferenceRespond l_oRespondObj;
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_MakeConferenceRespond);

	l_oRespondObj.m_oBody.m_strConferenceId = p_strConferenceId;
	if (p_bIsSuccess && !p_strConferenceId.empty())
	{
		l_oRespondObj.m_oBody.m_strResult = Result_Success;
		l_oRespondObj.m_oHeader.m_strResult = Result_Success;
	}
	else
	{
		l_oRespondObj.m_oBody.m_strResult = Result_Failed;
		l_oRespondObj.m_oHeader.m_strResult = Result_Failed;
	}
	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_MakeConference Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
}

void CHytProtocol::Sync_MakeConference(const std::string& p_strSyncBodyData)
{
	PROTOCOL::CConferenceSync l_oSyncObj;
	if (!l_oSyncObj.ParseString(p_strSyncBodyData, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid protocol: [%s]", p_strSyncBodyData.c_str());
		return;
	}
	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_ConferenceSync, Topic_ConferenceSync, SendType_Topic);

	std::string l_strSyncMessage = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_LogPtr, "Send Conference Sync: [%s]", l_strSyncMessage.c_str());
}

void CHytProtocol::Sync_MakeConference(const std::string& p_strConferenceId, const std::string& p_strCallRefId, const std::string& p_strCompere,
	const std::string& p_strTarget, const std::string& p_strState, const std::string& p_strResult)
{
	PROTOCOL::CConferenceSync l_oSyncObj;
	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_ConferenceSync, Topic_ConferenceSync, SendType_Topic);

	l_oSyncObj.m_oBody.m_strConferenceId = p_strConferenceId;
	l_oSyncObj.m_oBody.m_strCallRefId = p_strCallRefId;
	l_oSyncObj.m_oBody.m_strCompere = p_strCompere;
	l_oSyncObj.m_oBody.m_strState = p_strState;
	l_oSyncObj.m_oBody.m_strTarget = p_strTarget;
	l_oSyncObj.m_oBody.m_strResult = p_strResult;

	std::string l_strSyncMessage = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_LogPtr, "Send Conference Sync: [%s]", l_strSyncMessage.c_str());
}

void CHytProtocol::Sync_ConferenceDetailCallOver(const std::string& p_strDeviceNum,
	const std::string& p_strTargetDeviceType,
	const std::string& p_strCompere,
	const std::string& p_strCallId,
	const std::string& p_strActiveCallRefId,
	const std::string& p_strRingTime,
	const std::string& p_strTalkTime,
	const std::string& p_strJoinTime,
	const std::string& p_strHangupTime)
{
	std::string l_strCurrCallId = p_strActiveCallRefId;
	PROTOCOL::CDetailCallOverSync l_oSyncObj;
	if (p_strActiveCallRefId.empty())  
	{
		//为源呼叫
		l_strCurrCallId = p_strCallId;
		std::string l_strCalledId = "";
		std::string l_strCllerId = m_SwitchClientPtr->GetCallerID(l_strCurrCallId, l_strCalledId);
		if (p_strDeviceNum == l_strCalledId)
		{
			//呼入到坐席后拉会
			l_oSyncObj.m_oBody.m_strCaller = l_strCllerId;
			if (m_SwitchClientPtr->GetDeviceState(l_strCllerId).empty())
			{
				l_oSyncObj.m_oBody.m_strCallerType = TARGET_DEVICE_TYPE_OUTSIDE;
			}
			else
			{
				l_oSyncObj.m_oBody.m_strCallerType = TARGET_DEVICE_TYPE_INSIDE;
			}

			l_oSyncObj.m_oBody.m_strCalled = p_strDeviceNum;
			l_oSyncObj.m_oBody.m_strCalledType = p_strTargetDeviceType;
		}
		else 
		{
			//坐席呼出后再拉会
			l_oSyncObj.m_oBody.m_strCaller = p_strDeviceNum;
			l_oSyncObj.m_oBody.m_strCallerType = p_strTargetDeviceType;

			l_oSyncObj.m_oBody.m_strCalled = l_strCalledId;
			if (m_SwitchClientPtr->GetDeviceState(l_strCalledId).empty())
			{
				l_oSyncObj.m_oBody.m_strCalledType = TARGET_DEVICE_TYPE_OUTSIDE;
			}
			else
			{
				l_oSyncObj.m_oBody.m_strCalledType = TARGET_DEVICE_TYPE_INSIDE;
			}
		}
	}
	else
	{
		l_oSyncObj.m_oBody.m_strCaller = p_strCompere;
		l_oSyncObj.m_oBody.m_strCallerType = TARGET_DEVICE_TYPE_INSIDE;

		l_oSyncObj.m_oBody.m_strCalled = p_strDeviceNum;
		l_oSyncObj.m_oBody.m_strCalledType = p_strTargetDeviceType;
	}
	
	l_oSyncObj.m_oBody.m_strSwitchType = "1";
	l_oSyncObj.m_oBody.m_strDeviceNum = p_strDeviceNum;
	l_oSyncObj.m_oBody.m_strDeviceType = p_strTargetDeviceType;
	l_oSyncObj.m_oBody.m_strRelationCallId = p_strCallId;
	l_oSyncObj.m_oBody.m_strCallId = l_strCurrCallId;
	if (p_strCompere == p_strDeviceNum)
	{
		l_oSyncObj.m_oBody.m_strRole = "3";
	}
	else
	{
		l_oSyncObj.m_oBody.m_strRole = "4";
	}
	l_oSyncObj.m_oBody.m_strRingTime = p_strRingTime;
	l_oSyncObj.m_oBody.m_strTalkTime = p_strTalkTime;
	l_oSyncObj.m_oBody.m_strJoinTime = p_strJoinTime;
	l_oSyncObj.m_oBody.m_strHangupTime = p_strHangupTime;
	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_DetailCallOverSync, Queue_CTIDBProcess, SendType_Queue);

	//发送子话务结束同步消息
	std::string l_strSyncMsg = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->NotifyPrivate(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
	ICC_LOG_DEBUG(m_LogPtr, "Send DetailCallOver Sync Queue: [%s]", l_strSyncMsg.c_str());
}

void CHytProtocol::CR_AddConferenceParty(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CMakeConferenceRespond l_oRespondObj;
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_AddConferencePartyRespond);
	
	l_oRespondObj.m_oBody.m_strConferenceId = p_strConferenceId;
	if (!p_strConferenceId.empty())
	{
		l_oRespondObj.m_oBody.m_strResult = Result_Success;
		l_oRespondObj.m_oHeader.m_strResult = Result_Success;
	}
	else
	{
		l_oRespondObj.m_oBody.m_strResult = "1";
		l_oRespondObj.m_oHeader.m_strResult = "1";
	}

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_AddConferenceParty Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send CR_AddConferenceParty Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}

void CHytProtocol::CR_DeleteConferenceParty(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteConferencePartyRespond l_oRespondObj;
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_DeleteConferencePartyRespond);
	
	l_oRespondObj.m_oBody.m_strConferenceId = p_strConferenceId;
	l_oRespondObj.m_oBody.m_strResult = Result_Success;
	l_oRespondObj.m_oHeader.m_strResult = Result_Success;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_DeleteConferenceParty Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send CR_DeleteConferenceParty Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}

void CHytProtocol::CR_TakeOverCallEx(ObserverPattern::INotificationPtr p_pNotifiRequest,
	const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strMsgId, bool p_bResult)
{
	PROTOCOL::CTakeOverCallRespond l_oRespondObj;
	std::string l_strResult = (p_bResult ? Result_Success : Result_Failed);
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TakeOverCallRespond, "", SendType_Queue, p_strMsgId);

	l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
	l_oRespondObj.m_oBody.m_strResult = l_strResult;
	l_oRespondObj.m_oHeader.m_strResult = l_strResult;

	std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send CR_TakeOverCallEx Respond: [%s]", l_strRespondMsg.c_str());
		p_pNotifiRequest->Response(l_strRespondMsg);
	}
	else
	{
		ICC_LOG_ERROR(m_LogPtr, "Send CR_TakeOverCallEx Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
			l_strRespondMsg.c_str());
	}
}

void CHytProtocol::CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			bool l_bResult = p_pSwitchNotif->GetResult();

			PROTOCOL::CTakeOverCallRespond l_oRespondObj;
			l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();
			

			if (l_bResult)
			{
				//	发送话务接管结果给DBProcess
				l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TakeOverCallSync, Queue_CTIDBProcess, SendType_Queue);
				l_oRespondObj.m_oHeader.m_strResult = Result_Success;

				std::string l_strSyncMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
				ICC_LOG_DEBUG(m_LogPtr, "Send TakeOverCall Sync Queue: [%s]", l_strSyncMsg.c_str());
			}

			//	返回客户端
			l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TakeOverCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_TakeOverCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_TakeOverCall Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CHytProtocol::CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetCTIConnStateRespond l_oRespondObj;

			bool bOpen = p_pSwitchNotif->GetStreamOpen();
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetCTIConnStateRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strState = bOpen ? ConnectState_Connect : ConnectState_Disconnect;
			l_oRespondObj.m_oBody.m_strSwitchType = p_pSwitchNotif->GetSwitchType();

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetCTIConnState Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetCTIConnState Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}
			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetDeviceListRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetDeviceListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetDeviceListSize());

			auto iter = p_pSwitchNotif->m_DeviceList.begin();
			while (iter != p_pSwitchNotif->m_DeviceList.end())
			{
				boost::shared_ptr<CDeviceStateNotif> l_pNotifObj = *iter;
				if (l_pNotifObj)
				{
					long l_lCSTACallRefId = l_pNotifObj->GetCSTACallRefId();
					std::string l_strDeviceNum = l_pNotifObj->GetDeviceNum();
					std::string l_strDeviceType = l_pNotifObj->GetDeviceType();
					std::string l_strStateTime = l_pNotifObj->GetStateTime();
					std::string l_strDeviceState = l_pNotifObj->GetDeviceState();
					std::string l_strCTICallRefId = l_pNotifObj->GetCTICallRefId();
					std::string l_strCallerId = l_pNotifObj->GetCallerId();
					std::string l_strCalledParty = l_pNotifObj->GetCalledId();
					std::string l_strCallDirection = l_pNotifObj->GetCallDirection();
					std::string l_strLoginMode = l_pNotifObj->GetLoginMode();
					std::string l_strReadyState = l_pNotifObj->GetReadyState();

					std::string l_strAcdGrp = l_pNotifObj->GetACDGrp(); 
					std::string l_strOriginalCalled = l_pNotifObj->GetOriginalCalledId();
					std::string l_strTalkTime = l_pNotifObj->GetTalkTime();

					PROTOCOL::CGetDeviceListRespond::CBody::CData l_oData;
					l_oData.m_strDevice = l_strDeviceNum;
					l_oData.m_strDeviceType = l_strDeviceType;
					l_oData.m_strState = l_strDeviceState;
					l_oData.m_strTime = l_strStateTime;
					l_oData.m_strCallRefId = l_strCTICallRefId;
					l_oData.m_strCallerId = l_strCallerId;
					l_oData.m_strCalledId = l_strCalledParty;
					l_oData.m_strCallDirection = l_strCallDirection;
					l_oData.m_strLoginMode = l_strLoginMode;
					l_oData.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;;
					l_oData.m_strBeginTalkTime = l_strTalkTime;
					l_oData.m_strDeptCode = l_pNotifObj->GetDeptCode();
					l_oData.m_strAcdGrp = l_strAcdGrp;
					l_oData.m_strOriginalCalled = l_strOriginalCalled;

					if (!l_strCTICallRefId.empty())
					{
						std::string l_strCompere;
						std::string l_strOrgCallId;
						std::string l_strConferenceId = "";
						if (m_bFSAesMode)
						{
							std::string l_strBargeInSponsor = "";
							std::string l_strCompere = "";
							l_strConferenceId = l_pNotifObj->GetConferenceId();// m_SwitchClientPtr->QueryConferenceByCallid(l_strCTICallRefId, l_strCompere, l_strBargeInSponsor);
							if (!l_strConferenceId.empty())
							{
								l_oData.m_strConferenceId = l_strConferenceId;
								l_oData.m_strConferenceCompere = l_pNotifObj->GetConfCompere();
								l_strBargeInSponsor = l_pNotifObj->GetConfBargeinSponsor();
								if (!l_strBargeInSponsor.empty())
								{
									l_oData.m_strConfBargeinSponsor = l_strBargeInSponsor;
								}
								else
								{
									l_oData.m_strConfBargeinSponsor = "";
								}

								if (l_strDeviceState == CallStateString[STATE_TALK] && l_strBargeInSponsor.empty())
								{
									l_oData.m_strState = CallStateString[STATE_CONFERENCE];
								}
							}
						}
						else
						{
							l_strConferenceId = CConferenceManager::Instance()->FindConferenceIdByCallId(l_strCTICallRefId, l_strCompere, l_strOrgCallId);
							if (!l_strConferenceId.empty())
							{
								l_oData.m_strConferenceId = l_strConferenceId;
								l_oData.m_strConferenceCompere = l_strCompere;
								std::string l_strBargeinSponsor = "";
								if (CConferenceManager::Instance()->IsBargeinCreate(l_strConferenceId, l_strBargeinSponsor))
								{
									l_oData.m_strConfBargeinSponsor = l_strBargeinSponsor;
								}
								else
								{
									l_oData.m_strConfBargeinSponsor = "";
								}

								if (l_strDeviceState == CallStateString[STATE_TALK] && l_strBargeinSponsor.empty())
								{
									l_oData.m_strState = CallStateString[STATE_CONFERENCE];
								}
							}
							else
							{
								if (l_strDeviceState == CallStateString[STATE_CONFERENCE])
								{
									l_oData.m_strState = CallStateString[STATE_TALK];
								}
							}
						}
					}
					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}

				++iter;
			}

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetDeviceList Respond: [%s]", 
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetDeviceList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetACDListRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetACDListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetAgentListSize());

			std::list<boost::shared_ptr<CAgentStateNotif>> l_agentList = p_pSwitchNotif->GetAgentList();
			auto iter = l_agentList.begin();
			while (iter != l_agentList.end())
			{
				boost::shared_ptr<CAgentStateNotif> l_pNotifObj = *iter;
				if (l_pNotifObj)
				{
					PROTOCOL::CGetACDListRespond::CBody::CData l_oData;

					std::string strReadyState = l_pNotifObj->GetReadyState();
					l_oData.m_strAgent = l_pNotifObj->GetAgentId();
					l_oData.m_strACD = l_pNotifObj->GetACDGrp();
					l_oData.m_strTime = l_pNotifObj->GetStateTime();

					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}

				++iter;
			}

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetACDList Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetACDList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		
		std::map<std::string, CSeatInfo> l_mapSeatInfo;
		{
			std::map<std::string, std::string> l_mapCachedSeatInfo;
			if (!m_pRedisClient->HGetAll("SeatInfo", l_mapCachedSeatInfo))
			{
				ICC_LOG_ERROR(m_LogPtr, "an error occured when getting seatinfo from redis");
			}
			/*int count = 0;*/
			for (auto it = l_mapCachedSeatInfo.begin(); it != l_mapCachedSeatInfo.end(); it++)
			{
				CSeatInfo l_oSeat;
				l_oSeat.Parse(it->second, m_JsonFactoryPtr->CreateJson());
				l_mapSeatInfo[it->first] = l_oSeat;
			}
		}
		
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		std::string m_strDeptCode = p_pSwitchNotif->GetDeptCode();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetFreeAgentRespond l_oRespondObj;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			std::list<boost::shared_ptr<CAgentStateNotif>> l_agentList = p_pSwitchNotif->GetAgentList();
			auto iter = l_agentList.begin();
			while (iter != l_agentList.end())
			{
				boost::shared_ptr<CAgentStateNotif> l_pNotifObj = *iter;
				if (l_pNotifObj)
				{
					PROTOCOL::CGetFreeAgentRespond::CBody::CData l_oData;

					std::string strReadyState = l_pNotifObj->GetReadyState();
					l_oData.m_strAgent = l_pNotifObj->GetAgentId();
					//l_oData.m_strACD = l_pNotifObj->GetACDGrp();
					l_oData.m_strLoginMode = l_pNotifObj->GetLoginMode();
					l_oData.m_strReadyState = strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
					//l_oData.m_strTime = l_pNotifObj->GetStateTime();
					auto tmpIt = l_mapSeatInfo.find(l_oData.m_strAgent);
					if (tmpIt != l_mapSeatInfo.end())
					{
						l_oData.m_strDeptCode = tmpIt->second.m_strDeptCode;
						l_oData.m_strDeptName = tmpIt->second.m_strDeptName;
					}
					if (!m_strDeptCode.empty())
					{
						if (m_strDeptCode == l_oData.m_strDeptCode)
						{
							l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
						}
					}
					else
					{
						l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
					}
					
				}

				++iter;
			}

			l_oRespondObj.m_oBody.m_strCount = std::to_string(l_oRespondObj.m_oBody.m_vecData.size());
			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetFreeAgentList Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetFreeAgentList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetAgentListRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetAgentListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetAgentListSize());

			std::list<boost::shared_ptr<CAgentStateNotif>> l_agentList = p_pSwitchNotif->GetAgentList();
			auto iter = l_agentList.begin();
			while (iter != l_agentList.end())
			{
				boost::shared_ptr<CAgentStateNotif> l_pNotifObj = *iter;
				if (l_pNotifObj)
				{
					PROTOCOL::CGetAgentListRespond::CBody::CData l_oData;

					std::string strReadyState = l_pNotifObj->GetReadyState();
					l_oData.m_strAgent = l_pNotifObj->GetAgentId();
					l_oData.m_strACD = l_pNotifObj->GetACDGrp();
					l_oData.m_strLoginMode = l_pNotifObj->GetLoginMode();
					l_oData.m_strReadyState = strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
					l_oData.m_strTime = l_pNotifObj->GetStateTime();

					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}

				++iter;
			}

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetAgentList Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetAgentList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		DataBase::SQLRequest tmp_oSQLRequest;
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		std::string strMessage = l_pRequestMemory->GetNotifiRequest()->GetMessages();
		PROTOCOL::CGetCallListRequest l_oRequestObj;
		if (!l_oRequestObj.ParseString(strMessage, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_ERROR(m_LogPtr, "Invalid GetCallList request protocol: [%s]", strMessage.c_str());

			return;
		}
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetCallListRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetCallListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetCallListSize());

			auto iter = p_pSwitchNotif->m_CallList.begin();
			while (iter != p_pSwitchNotif->m_CallList.end())
			{
				PROTOCOL::CGetCallListRespond::CBody::CData l_oData;

				l_oData.m_strCallRefId = iter->m_strCTICallRefId;
				l_oData.m_strAgentId = iter->m_strCalledParty;
				l_oData.m_strACDGrp = iter->m_strCalledId;
				l_oData.m_strCallerId = iter->m_strCallerId;
				l_oData.m_strCalledId = iter->m_strCalledParty;
				l_oData.m_strState = iter->m_strCallState;
				l_oData.m_strStateTime = iter->m_strStateTime;

				if (l_oRequestObj.m_oBody.calling_dept.empty())
				{
					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}
				else
				{
					std::string l_strDeptCode = "";
					if (!l_oData.m_strACDGrp.empty())
					{
						l_strDeptCode = _QueryAcdDept(l_oData.m_strACDGrp);
					}
					else
					{
						std::string l_strSeatInfo;
						if (m_pRedisClient->HGet("SeatInfo", l_oData.m_strAgentId, l_strSeatInfo))
						{
							CSeatInfo cSeatInfo;
							if (cSeatInfo.Parse(l_strSeatInfo, m_JsonFactoryPtr->CreateJson()))
							{
								l_strDeptCode = cSeatInfo.m_strDeptCode;
							}
						}
					}

					if (l_oRequestObj.m_oBody.calling_dept == l_strDeptCode )
					{
						l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
					}
				}
				
				++iter;
			}

			l_oRespondObj.m_oBody.m_strCount = std::to_string(l_oRespondObj.m_oBody.m_vecData.size());

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetCallList Respond: [%s]", 
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetCallList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}
void CHytProtocol::CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetReadyAgentRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetReadyAgentRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oBody.m_strReadyAgent = p_pSwitchNotif->GetReadyAgent();
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_GetReadyAgent Respond: [%s]", 
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetReadyAgent Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
}

void CHytProtocol::CC_RecordFileUp(const std::string& p_strMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (m_bFSAesMode)
	{
		m_SwitchClientPtr->FSAesAsyncRequestEx("del_history_record", p_strMsg);
	}
}

void CHytProtocol::CC_GetHistoryCallList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (m_bFSAesMode)
	{
		PROTOCOL::CGetHistoryCallListRequest l_oRequestObj;
		if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_ERROR(m_LogPtr, "Invalid CC_GetHistoryCallList request protocol: [%s]", p_strNetMsg.c_str());

			return;
		}

		std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgId;
		if (m_SwitchClientPtr)
		{
			long l_lTaskId = m_SwitchClientPtr->FSAesAsyncRequestEx(p_pNotifiRequest->GetCmdName(), p_strNetMsg);

			boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
			l_pRequestMemory->SetMsgId(l_strMsgId);
			l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
			CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
		}
	}
}

void CHytProtocol::CR_GetHistoryCallList(IFSAesExEventNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			JsonParser::IJsonPtr l_JsonPtr = m_JsonFactoryPtr->CreateJson();
			if (!l_JsonPtr->LoadJson(p_pSwitchNotif->GetEventData()))
			{
				ICC_LOG_ERROR(m_LogPtr, "load json[%s] object failed.", p_pSwitchNotif->GetEventData().c_str());
				return;
			}
			l_JsonPtr->SetNodeValue("/header/msgid", l_pRequestMemory->GetMsgId());
			std::string l_strRespondMsg = l_JsonPtr->ToString();

			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_LOWDEBUG(m_LogPtr, "Send TaskId [%u] CR_GetHistoryCallList Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
				l_pRequestMemory->GetNotifiRequest()->Response(l_strRespondMsg);
				PROTOCOL::CGetHistoryCallListRespond l_oGetHisrotyCallListRespond;
				if (!l_oGetHisrotyCallListRespond.ParseExString(l_strRespondMsg, m_JsonFactoryPtr->CreateJson()))
				{
					ICC_LOG_DEBUG(m_LogPtr, "Parse history call list failed");
				}
				l_oGetHisrotyCallListRespond.m_oHeader = CreateProtocolHeader(Cmd_HistoryCallSync, Topic_HistoryCallSync,SendType_Topic);
				std::string strHistoryMsg = l_oGetHisrotyCallListRespond.ToExString(m_JsonFactoryPtr->CreateJson());
				ICC_LOG_DEBUG(m_LogPtr, "send history_call_sync msg: [%s]", strHistoryMsg.c_str());
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strHistoryMsg));
			}
			else
			{
				ICC_LOG_ERROR(m_LogPtr, "Send TaskId [%u] CR_GetHistoryCallList Respond: [%s] Failed. Error: NotifiRequestPtr Invalid",
					l_lRequestId, l_strRespondMsg.c_str());
			}

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
		else
		{
			ICC_LOG_ERROR(m_LogPtr, "failed,RequestId:[%d]!!!", l_lRequestId);
		}
	}
}

bool CHytProtocol::AddSyntInfo(PROTOCOL::CMakeCallRequest &l_oRequestObj, const std::string& strTransGuid)
{
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	l_oAlarmLogInfo.m_strAlarmID = l_oRequestObj.m_oBody.m_strCaseId;
	std::string m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/PoliceCode/AssistantPolice", "");
	std::string m_strPolice = m_pConfig->GetValue("ICC/Plugin/PoliceCode/Police", "");
	std::string m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/PoliceCode/CodeMode", "1");
	ICC_LOG_DEBUG(m_LogPtr, "AssistantPolice=%s,Police=%s,CodeMode=%s", m_strAssistantPolice.c_str(), m_strPolice.c_str(), m_strCodeMode.c_str());
	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(l_oRequestObj.m_oBody.m_strReceiptCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_LogPtr, "get staff info failed!");
	}
	std::string strStaffName = "";
	if (l_oStaffInfo.m_strType == "JZLX101")
	{
		strStaffName = m_StringUtilPtr->Format("%s%s", m_strAssistantPolice.c_str(), l_oRequestObj.m_oBody.m_strReceiptName.c_str());
	}
	else
	{
		strStaffName = m_StringUtilPtr->Format("%s%s", m_strPolice.c_str(), l_oRequestObj.m_oBody.m_strReceiptName.c_str());
	}
	l_vecParamList.push_back(l_oRequestObj.m_oBody.m_strReceiptCode);
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_oRequestObj.m_oBody.m_strCallType);
	l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = l_oRequestObj.m_oBody.m_oAlarm.m_strReceiptName;
	l_oAlarmLogInfo.m_strOperate = LOG_FEEDBACK;
	l_oAlarmLogInfo.m_strID = m_StringUtilPtr->CreateGuid();
	l_oAlarmLogInfo.m_strCreateTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oAlarmLogInfo.m_strSourceName = "icc";
	AddAlarmLogInfo(l_oAlarmLogInfo);
	return true;
}

std::string CHytProtocol::BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = m_JsonFactoryPtr->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

bool CHytProtocol::AddAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_pAlarmLogInfo, const std::string& strTransGuid /* = "" */)
{
	PROTOCOL::CAlarmLogSync::CBody l_oData;
	//std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DATETIME_STRING_FORMAT_NO_MICROSECOND));
	//std::string l_strCurTime(m_pDateTime->CurrentDateTimeStr());
	//使用带毫秒的流水
	std::string l_strCurTime(m_DateTimePtr->ToString(m_DateTimePtr->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	std::string l_strGUID(m_StringUtilPtr->CreateGuid());
	if (p_pAlarmLogInfo.m_strID.empty())
	{
		l_oData.m_strID = l_strGUID;
	}
	else
	{
		l_oData.m_strID = p_pAlarmLogInfo.m_strID;
	}

	l_oData.m_strAlarmID = p_pAlarmLogInfo.m_strAlarmID;
	l_oData.m_strProcessID = p_pAlarmLogInfo.m_strProcessID;
	l_oData.m_strFeedbackID = p_pAlarmLogInfo.m_strFeedbackID;
	l_oData.m_strSeatNo = p_pAlarmLogInfo.m_strSeatNo;
	l_oData.m_strOperate = p_pAlarmLogInfo.m_strOperate;
	l_oData.m_strOperateContent = p_pAlarmLogInfo.m_strOperateContent;
	l_oData.m_strFromType = p_pAlarmLogInfo.m_strFromType;
	l_oData.m_strFromObject = p_pAlarmLogInfo.m_strFromObject;
	l_oData.m_strFromObjectName = p_pAlarmLogInfo.m_strFromObjectName;
	l_oData.m_strFromObjectOrgName = p_pAlarmLogInfo.m_strFromObjectOrgName;
	l_oData.m_strFromObjectOrgCode = p_pAlarmLogInfo.m_strFromObjectOrgCode;
	l_oData.m_strToType = p_pAlarmLogInfo.m_strToType;
	l_oData.m_strToObject = p_pAlarmLogInfo.m_strToObject;
	l_oData.m_strToObjectName = p_pAlarmLogInfo.m_strToObjectName;
	l_oData.m_strToObjectOrgName = p_pAlarmLogInfo.m_strToObjectOrgName;
	l_oData.m_strToObjectOrgCode = p_pAlarmLogInfo.m_strToObjectOrgCode;
	l_oData.m_strCreateUser = p_pAlarmLogInfo.m_strCreateUser;
	l_oData.m_strCreateTime = p_pAlarmLogInfo.m_strCreateTime;
	l_oData.m_strDeptOrgCode = p_pAlarmLogInfo.m_strDeptOrgCode;
	l_oData.m_strSourceName = p_pAlarmLogInfo.m_strSourceName;
	l_oData.m_strOperateAttachDesc = p_pAlarmLogInfo.m_strOperateAttachDesc;
	l_oData.m_strCreateTime = l_strCurTime;

	//TODO::select_icc_t_jjdb_jjsj 查询jjsj
	std::string strTime = m_DateTimePtr->GetAlarmIdTime(p_pAlarmLogInfo.m_strAlarmID);

	if (strTime != "")
	{
		DataBase::SQLRequest l_sqlReqeust;
		l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
		l_sqlReqeust.param["jjsj_begin"] = m_DateTimePtr->GetFrontTime(strTime, 30 * 86400);
		l_sqlReqeust.param["jjsj_end"] = m_DateTimePtr->GetAfterTime(strTime, 30 * 86400);
		l_sqlReqeust.param["jjdbh"] = p_pAlarmLogInfo.m_strAlarmID;
		DataBase::IResultSetPtr l_pRSetPtr = m_pDBConnPtr->Exec(l_sqlReqeust);
		if (!l_pRSetPtr->IsValid())
		{
			ICC_LOG_ERROR(m_LogPtr, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
		}
		if (l_pRSetPtr->Next())
		{
			l_oData.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
		}
	}

	if (l_oData.m_strReceivedTime.empty())
	{
		l_oData.m_strReceivedTime = l_strCurTime;
	}

	if (!InsertAlarmLogInfo(l_oData))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Insert icc_t_alarm_log failed!");
		return false;
	}
	SyncAlarmLogInfo(l_oData);
	return true;
}


void CHytProtocol::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_StringUtilPtr->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;
	JsonParser::IJsonPtr l_pIJson = m_JsonFactoryPtr->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_LogPtr, "send message:[%s]", l_strMessage.c_str());
}

IResourceManagerPtr CHytProtocol::GetResourceManager()
{
	return m_pResourceManager;
}

bool CHytProtocol::InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
{
	DataBase::SQLRequest p_tSQLReqInsertAlarm;
	p_tSQLReqInsertAlarm.sql_id = "insert_icc_t_alarm_log";

	p_tSQLReqInsertAlarm.param["id"] = p_AlarmLogInfo.m_strID;
	p_tSQLReqInsertAlarm.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
	p_tSQLReqInsertAlarm.param["pjdbh"] = p_AlarmLogInfo.m_strProcessID;
	p_tSQLReqInsertAlarm.param["fkdbh"] = p_AlarmLogInfo.m_strFeedbackID;
	p_tSQLReqInsertAlarm.param["operate"] = p_AlarmLogInfo.m_strOperate;
	p_tSQLReqInsertAlarm.param["operate_content"] = p_AlarmLogInfo.m_strOperateContent;
	p_tSQLReqInsertAlarm.param["from_type"] = p_AlarmLogInfo.m_strFromType;
	p_tSQLReqInsertAlarm.param["from_object"] = p_AlarmLogInfo.m_strFromObject;
	p_tSQLReqInsertAlarm.param["from_object_name"] = p_AlarmLogInfo.m_strFromObjectName;
	p_tSQLReqInsertAlarm.param["from_object_org_name"] = p_AlarmLogInfo.m_strFromObjectOrgName;
	p_tSQLReqInsertAlarm.param["from_object_org_code"] = p_AlarmLogInfo.m_strFromObjectOrgCode;
	p_tSQLReqInsertAlarm.param["from_object_org_identifier"] = p_AlarmLogInfo.m_strFromOrgIdentifier;
	p_tSQLReqInsertAlarm.param["to_type"] = p_AlarmLogInfo.m_strToType;
	p_tSQLReqInsertAlarm.param["to_object"] = p_AlarmLogInfo.m_strToObject;
	p_tSQLReqInsertAlarm.param["to_object_name"] = p_AlarmLogInfo.m_strToObjectName;
	p_tSQLReqInsertAlarm.param["to_object_org_name"] = p_AlarmLogInfo.m_strToObjectOrgName;
	p_tSQLReqInsertAlarm.param["to_object_org_code"] = p_AlarmLogInfo.m_strToObjectOrgCode;
	p_tSQLReqInsertAlarm.param["to_object_org_identifier"] = p_AlarmLogInfo.m_strToObjectOrgIdentifier;
	p_tSQLReqInsertAlarm.param["create_time"] = p_AlarmLogInfo.m_strCreateTime;
	p_tSQLReqInsertAlarm.param["source_name"] = p_AlarmLogInfo.m_strSourceName;
	p_tSQLReqInsertAlarm.param["operate_attach_desc"] = p_AlarmLogInfo.m_strOperateAttachDesc;
	p_tSQLReqInsertAlarm.param["description"] = p_AlarmLogInfo.m_strDescription;
	p_tSQLReqInsertAlarm.param["create_user"] = p_AlarmLogInfo.m_strCreateUser;
	p_tSQLReqInsertAlarm.param["create_org"] = p_AlarmLogInfo.m_strCreateOrg;
	p_tSQLReqInsertAlarm.param["seat_no"] = p_AlarmLogInfo.m_strSeatNo;
	p_tSQLReqInsertAlarm.param["dept_org_code"] = p_AlarmLogInfo.m_strDeptOrgCode;
	p_tSQLReqInsertAlarm.param["jjsj"] = p_AlarmLogInfo.m_strReceivedTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConnPtr->Exec(p_tSQLReqInsertAlarm, false, strTransGuid);

	ICC_LOG_DEBUG(m_LogPtr, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_LogPtr, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	m_pHelpTool->Sleep(1);
	return true;
}

std::string CHytProtocol::_QueryAcdDept(const std::string& strAcd)
{
	//更改为Redis查询
	/*DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_acd_dept_from_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = strAcd;

	DataBase::IResultSetPtr l_Result = m_pDBConnPtr->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_LogPtr, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_LogPtr, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return "";
	}

	if (l_Result->Next())
	{
		return l_Result->GetValue("dept_code");
	}*/
	std::string strDeptCode;
	if (m_pRedisClient->HGet(ACD_INFO, strAcd, strDeptCode))
	{
		PROTOCOL::CAcdInfo l_AcdInfo;
		if (!l_AcdInfo.Parse(strDeptCode, m_JsonFactoryPtr->CreateJson()))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Parse ACD INFO failed!!!");
		}
		ICC_LOG_DEBUG(m_LogPtr, "HGet ACD INFO success,ACD:[%s],DeptCode:[%s]!!!", strAcd.c_str(), l_AcdInfo.m_dept_code.c_str());
		return l_AcdInfo.m_dept_code;

	}

	return "";
}

void CHytProtocol::_SyncAgentState(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CAgentStateEvent l_oRequestObj;

		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_ACDAgentStateSync, Topic_ACDAgentStateSync, SendType_Topic);

		std::string l_strReadyState;
		l_oRequestObj.m_oBody.m_strAgent = p_pSwitchNotif->GetDeviceNum();
		l_oRequestObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
		l_oRequestObj.m_oBody.m_strLoginMode = p_pSwitchNotif->GetLoginMode();

		std::string l_strDeviceState = p_pSwitchNotif->GetDeviceState();
		if (l_strDeviceState == "ringstate")
		{
			if (p_pSwitchNotif->GetCallCount() == 1)
			{
				l_strReadyState = ReadyState_Busy;
			}
		}		
		else if (l_strDeviceState == "dialstate")
		{
			if (p_pSwitchNotif->GetCallCount() == 1)
			{
				l_strReadyState = ReadyState_Busy;
			}
		}
		else if (l_strDeviceState == "hangupstate")
		{
			if (p_pSwitchNotif->GetCallCount() == 0)
			{
				l_strReadyState = ReadyState_Idle;
			}			
		}		

		l_oRequestObj.m_oBody.m_strReadyState = l_strReadyState;
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();		

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send AgentState Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
}

bool CHytProtocol::_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo)
{
	std::string strStaffInfo;
	if (m_strCodeMode == STAFF_ID_NO)
	{
		if (!m_pRedisClient->HGet("StaffIdInfo", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Hget StaffIdInfo failed,staff_id_no:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	else if (m_strCodeMode == STAFF_CODE)
	{
		if (!m_pRedisClient->HGet("StaffInfoMap", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_LogPtr, "Hget StaffInfoMap failed,staff_code:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	if (!l_oStaffInfo.Parse(strStaffInfo, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_DEBUG(m_LogPtr, "parse staff info failed!!!");
		return false;
	}
	return true;
}

void CHytProtocol::_AddGeneralConsultationInfo(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId)
{
	m_mapGeneralConsultCalls[p_strActiveCallRefId] = p_strHeldCallRefId;
	ICC_LOG_DEBUG(m_LogPtr, "GeneralConsultationId,set,ActiveCallRefId:[%s],holdCallId:[%s]", p_strActiveCallRefId.c_str(), p_strHeldCallRefId.c_str());
}

std::string CHytProtocol::_GetGeneralConsultationInfo(const std::string& p_strActiveCallRefId)
{
	std::string l_strHeldCallRefId = "";
	auto it = m_mapGeneralConsultCalls.find(p_strActiveCallRefId);
	if (it != m_mapGeneralConsultCalls.end())
	{
		l_strHeldCallRefId = it->second;
	}

	return l_strHeldCallRefId;
}

void CHytProtocol::DeleteGeneralConsultationInfo(const std::string& p_strActiveCallRefId)
{
	m_mapGeneralConsultCalls.erase(p_strActiveCallRefId);
	ICC_LOG_DEBUG(m_LogPtr, "GeneralConsultationId,delete,callId:[%s]",p_strActiveCallRefId.c_str());
}