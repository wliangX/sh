#include "Boost.h"

#include "HytProtocol.h"
#include "TaskManager.h"
#include "RequestMemory.h"
#include "BlackListManager.h"
#include "TransferCallManager.h"
#include "ConferenceManager.h"
#include <Protocol/CSeatInfo.h>
#define ACD_INFO "acd_dept"
//////////////////////////////////////////////////////////////////////////
CHytProtocol::CHytProtocol(void)
{
	m_pObserverCenter = nullptr;
	m_JsonFactoryPtr = nullptr;
	m_StringUtilPtr = nullptr;
	m_DateTimePtr = nullptr;
	m_LogPtr = nullptr;
	m_pRedisClient = nullptr;
	m_pDBConnPtr = nullptr;
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

	std::string l_strClientId = l_oSyncObj.m_oBody.m_strClientId;
	std::string l_strClientType = l_oSyncObj.m_oBody.m_strClientType;
	std::string l_strClientStatus = l_oSyncObj.m_oBody.m_strClientStatus;

	// 客户端下线
	if (l_strClientStatus.compare("0") == 0)
	{
		//ACD 为空，不同ACD组成员是否都下线？
		//	long l_lTaskId = m_SwitchClientPtr->AgentLogout(l_strClientId, "");
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strLoginMode = l_oRequestObj.m_oBody.m_strLoginMode;

	long l_lTaskId = CTaskManager::Instance()->AgentLogin(l_strAgent, l_strACD);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strLoginMode = l_oRequestObj.m_oBody.m_strLoginMode;

	long l_lTaskId = CTaskManager::Instance()->AgentLogout(l_strAgent, l_strACD);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strAgent = l_oRequestObj.m_oBody.m_strAgent;
	std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
	std::string l_strReadyState = l_oRequestObj.m_oBody.m_strReadyState;
	
	//std::string l_strLogoutReason = l_oRequestObj.m_oBody.m_strLogoutReason;

	long l_lTaskId = CTaskManager::Instance()->SetAgentState(l_strAgent, l_strACD, l_strReadyState.compare(ReadyState_Idle) == 0 ? ReadyState_Ready : ReadyState_NotReady,"");

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallerId = l_oRequestObj.m_oBody.m_strCallerId;
	std::string l_strCalledId = l_oRequestObj.m_oBody.m_strCalledId;
	std::string l_strTargetPhoneType = l_oRequestObj.m_oBody.m_strTargetPhoneType;
	std::string l_strCaseId = l_oRequestObj.m_oBody.m_strCaseId;
	std::string l_strRelateCallRefId = l_oRequestObj.m_oBody.m_strRelateCallRefId;
	std::string l_strReceiptCode = l_oRequestObj.m_oBody.m_strReceiptCode;
	std::string l_strReceiptName = l_oRequestObj.m_oBody.m_strReceiptName;

	long l_lTaskId = CTaskManager::Instance()->MakeCall(l_strCallerId, l_strCalledId, l_strTargetPhoneType, l_strCaseId);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetCaseId(l_strCaseId);
		l_pRequestMemory->SetRelateCallRefId(l_strRelateCallRefId);
		l_pRequestMemory->SetReceiptCode(l_strReceiptCode);
		l_pRequestMemory->SetReceiptName(l_strReceiptName);
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	long l_lTaskId = CTaskManager::Instance()->AnswerCall(l_strCallRefId, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	long l_lTaskId = CTaskManager::Instance()->RefuseAnswer(l_strCallRefId, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;

	long l_lTaskId = CTaskManager::Instance()->ClearCall(l_strCallRefId, l_strSponsor);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->ListenCall(l_strCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->PickupCall(l_strCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;

	long l_lTaskId = CTaskManager::Instance()->Hangup(l_strCallRefId, l_strSponsor, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->BargeInCall(l_strCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->ForcePopCall(l_strCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	std::string l_strTargetDeviceType = l_oRequestObj.m_oBody.m_strTargetDeviceType;

	long l_lTaskId = CTaskManager::Instance()->ConsultationCall(l_strCallRefId, l_strSponsor, l_strTarget, l_strTargetDeviceType);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_ConsultationCallEx(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CConsultationCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid ConsultationCallEx request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	//long l_lHeldCSTACallRefId = CCallManager::Instance()->GetCSTACallRefId(l_strCallRefId);

	/*long l_lTaskId = CTaskManager::Instance()->ConsultationCallEx(l_strCallRefId, l_strSponsor, l_strTarget);

		boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);*/
}
void CHytProtocol::CC_TransferCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CTransferCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid TransferCall request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strActiveCallRefId = l_oRequestObj.m_oBody.m_strActiveCallRefId;
	std::string l_strHeldCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->TransferCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;
	std::string l_strTargetDeviceType = l_oRequestObj.m_oBody.m_strTargetDeviceType;
	std::string l_strIsACD = l_oRequestObj.m_oBody.m_strIsACD;

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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	CTransferCallManager::Instance()->CancelTransferCall(l_strCallRefId, l_strSponsor, l_strTarget);

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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->DeflectCall(l_strCallRefId, l_strSponsor, l_strTarget);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	long l_lTaskId = CTaskManager::Instance()->HoldCall(l_strCallRefId, l_strSponsor, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strHeldDevice;

	long l_lTaskId = CTaskManager::Instance()->RetrieveCall(l_strCallRefId, l_strSponsor, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strActiveCallRefId = l_oRequestObj.m_oBody.m_strActiveCallRefId;
	std::string l_strHeldCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strDevice = l_oRequestObj.m_oBody.m_strDevice;

	long l_lTaskId = CTaskManager::Instance()->ReconnectCall(l_strActiveCallRefId, l_strHeldCallRefId, l_strSponsor, l_strDevice);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}
void CHytProtocol::CC_ConferenceCall(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CConferenceCallRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid Conference request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strActiveCallRefId = l_oRequestObj.m_oBody.m_strActiveCallRefId;
	std::string l_strHeldCallRefId = l_oRequestObj.m_oBody.m_strHeldCallRefId;
	std::string l_strSponsor = l_oRequestObj.m_oBody.m_strSponsor;
	std::string l_strTarget = l_oRequestObj.m_oBody.m_strTarget;

	long l_lTaskId = CTaskManager::Instance()->ConferenceCall(l_strTarget, l_strActiveCallRefId, l_strHeldCallRefId);

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::CC_AddConferenceParty(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CAddConferencePartyRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid AddConferenceParty request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
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

	std::string l_strConferenceId;
	if (l_nCount > 0)
	{
		l_strConferenceId = CConferenceManager::Instance()->AddConferenceParty(l_strCallRefId, l_strCompere, l_vPartyQueue);
	}

	CR_AddConferenceParty(l_strConferenceId, p_pNotifiRequest);
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
	bool bRes = CConferenceManager::Instance()->GetConferenceByCTICallRefId(l_strCallRefId, l_strConferenceID);
	if (bRes)
	{
		std::string l_strBargeinSponSor;
		std::string l_strBargeinTarget;
		/*if (CConferenceManager::Instance()->GetBargeinCreateSponsor(l_strConferenceID, l_strBargeinSponSor, l_strBargeinTarget))
		{
			l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "1";
			l_oQueryConferenceByCallidRespond.m_oBody.m_strBargeinSponSor = l_strBargeinSponSor;
			l_oQueryConferenceByCallidRespond.m_oBody.m_strBargeinTarget = l_strBargeinTarget;
		}
		else*/
		{
			l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
		}
		l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "1";
		l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = l_strConferenceID;
	}
	else
	{
		l_oQueryConferenceByCallidRespond.m_oBody.m_strIsMeeting = "0";
		l_oQueryConferenceByCallidRespond.m_oBody.m_strIsBargein = "0";
		l_oQueryConferenceByCallidRespond.m_oBody.m_strConferenceId = l_strConferenceID;
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string strConferenceId = l_oRequestObj.m_oBody.m_strConferenceId;
	std::string strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
	std::string strCompere = l_oRequestObj.m_oBody.m_strCompere;
	std::string strTarget = l_oRequestObj.m_oBody.m_strTarget;
	CConferenceManager::Instance()->DeleteConferenceParty(strConferenceId, strTarget);

	CR_DeleteConferenceParty(strConferenceId, p_pNotifiRequest);
}

void CHytProtocol::CC_GetCTIConnState(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CGetCTIConnStateRequest l_oRequestObj;
	if (!l_oRequestObj.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid GetCTIConnState request protocol: [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	long l_lTaskId = CTaskManager::Instance()->GetCTIConnectState();

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	long l_lTaskId = CTaskManager::Instance()->GetDeviceList();

	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	long l_lTaskId = CTaskManager::Instance()->GetACDList();

	boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;
	std::string l_strACDGrp = l_oRequestObj.m_oBody.m_strACDGrp;

	long l_lTaskId = CTaskManager::Instance()->GetAgentList(l_strACDGrp);

	boost::shared_ptr<CRequestMemory> l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	std::string l_strMsgId = l_oRequestObj.m_oHeader.m_strMsgid;

	long l_lTaskId = CTaskManager::Instance()->GetCallList();
	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
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

	long l_lTaskId = CTaskManager::Instance()->GetReadyAgent(l_strACDGrp);
	boost::shared_ptr<CRequestMemory>  l_pRequestMemory = boost::make_shared<CRequestMemory>();
	if (l_pRequestMemory)
	{
		l_pRequestMemory->SetMsgId(l_strMsgId);
		l_pRequestMemory->SetNotifiRequest(p_pNotifiRequest);
		CRequestMemoryManager::Instance()->AddRequestMemory(l_lTaskId, l_pRequestMemory);
	}
}

void CHytProtocol::Sync_SetBlackList(const std::string& p_strLimitNum, const std::string& p_strLimitMin, const std::string& p_strLimitReason,
	const std::string& p_strBeginTime, const std::string& p_strEndTime, const std::string& p_strStaffCode, const std::string& p_strStaffName, bool p_bIsUpdate)
{
	PROTOCOL::CSetBlackListSync l_oSetBlackListSync;

	l_oSetBlackListSync.m_oBody.m_strLimitNum = p_strLimitNum;
	l_oSetBlackListSync.m_oBody.m_strlimitMinute = p_strLimitMin;
	l_oSetBlackListSync.m_oBody.m_strLimitReason = p_strLimitReason;
	l_oSetBlackListSync.m_oBody.m_strStaffCode = p_strStaffCode;
	l_oSetBlackListSync.m_oBody.m_strStaffName = p_strStaffName;
	l_oSetBlackListSync.m_oBody.m_strBeginTime = p_strBeginTime;
	l_oSetBlackListSync.m_oBody.m_strEndTime = p_strEndTime;
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
void CHytProtocol::Sync_DeleteBlackList(const std::string& p_strLimitNum)
{
	PROTOCOL::CDeleteBlackListSync l_oDeleteBlackListSync;

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

	std::string l_strMsgId = l_oSetBlackListRequest.m_oHeader.m_strMsgid;
	std::string l_strLimitNum = l_oSetBlackListRequest.m_oBody.m_strLimitNum;
	std::string l_strLimitMin = l_oSetBlackListRequest.m_oBody.m_strLimitMin;
	std::string l_strLimitReason = l_oSetBlackListRequest.m_oBody.m_strLimitReason;
	std::string l_strStaffCode = l_oSetBlackListRequest.m_oBody.m_strStaffCode;
	std::string l_strStaffName = l_oSetBlackListRequest.m_oBody.m_strStaffName;
	std::string l_strBegintime = l_oSetBlackListRequest.m_oBody.m_strBeginTime;
	std::string l_strEndTime = l_oSetBlackListRequest.m_oBody.m_strEndTime;
	CBlackListManager::Instance()->GetLimitTime(l_strLimitMin, l_strBegintime, l_strEndTime);

	bool l_bIsUpdate = false;
	if (CBlackListManager::Instance()->FindBlackNumber(l_strLimitNum))
	{
		//	存在该黑名单记录，更新数据库
		l_bIsUpdate = true;
	}

	boost::shared_ptr<CLimitNum> l_pLimitNumObj = boost::make_shared<CLimitNum>();
	l_pLimitNumObj->SetLimitNum(l_strLimitNum);
	l_pLimitNumObj->SetLimitMin(l_strLimitMin);
	l_pLimitNumObj->SetLimitReason(l_strLimitReason);
	l_pLimitNumObj->SetStaffCode(l_strStaffCode);
	l_pLimitNumObj->SetStaffName(l_strStaffName);
	l_pLimitNumObj->SetBeginTime(l_strBegintime);
	l_pLimitNumObj->SetEndTime(l_strEndTime);
	// 更新内存
	CBlackListManager::Instance()->AddBlackList(l_strLimitNum, l_pLimitNumObj);

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

	Sync_SetBlackList(l_strLimitNum, l_strLimitMin, l_strLimitReason, l_strBegintime, l_strEndTime, l_strStaffCode, l_strStaffName, l_bIsUpdate);
}
void CHytProtocol::CC_DeleteBlackList(const std::string& p_strNetMsg, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CDeleteBlackListRequest l_oDeleteBlackListRequest;
	if (!l_oDeleteBlackListRequest.ParseString(p_strNetMsg, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_LogPtr, "Invalid DeleteBlackList request protocol : [%s]", p_strNetMsg.c_str());

		return;
	}

	std::string l_strMsgId = l_oDeleteBlackListRequest.m_oHeader.m_strMsgid;
	std::string l_strLimitNum = l_oDeleteBlackListRequest.m_oBody.m_strLimitNum;

	CBlackListManager::Instance()->DeleteBlackList(l_strLimitNum);

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

	Sync_DeleteBlackList(l_strLimitNum);
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
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y-%m-%d %H:%M:%S", localtime(&l_tCurrentTime));
	std::string l_strTmpTime = std::string(l_szDayTime);

	l_oGetCurrentTimeRespond.m_oBody.m_strCurrentUTCTime = m_DateTimePtr->CurrentDateTimeStr();
	l_oGetCurrentTimeRespond.m_oBody.m_strCurrentTime = l_strTmpTime;
	std::string l_strMessage = l_oGetCurrentTimeRespond.ToString(m_JsonFactoryPtr->CreateJson());
	if (p_pNotifiRequest)
	{
		ICC_LOG_DEBUG(m_LogPtr, "Send GetCurrentTime Respond: [%s]", l_strMessage.c_str());
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

	//if (m_SwitchClientPtr)
	{
		long l_lTaskId = CTaskManager::Instance()->SetAgentState(l_strAgent, l_strACD, l_strReadyState.compare(ReadyState_Idle) == 0 ? ReadyState_Ready : ReadyState_NotReady, "clientTimeout");

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
	//CBlackListManager::Instance()->GetAllBlackList(l_oGetAllBlackListRespond);
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

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send CTIConnectState Sync Topic: [%s]", l_strSyncMsg.c_str());
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
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strState = p_pSwitchNotif->GetCallState();
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();
		l_oRequestObj.m_oBody.m_strCallRefId = l_strCTICallRefId;
		l_oRequestObj.m_oBody.m_strCSTACallRefId = std::to_string(l_lCSTACallRefId);

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

void CHytProtocol::Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		PROTOCOL::CDeviceStateEvent l_oRequestObj;

		l_oRequestObj.m_oBody.m_strDevice = p_pSwitchNotif->GetDeviceNum();
		l_oRequestObj.m_oBody.m_strDeviceType = p_pSwitchNotif->GetDeviceType();
		l_oRequestObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
		l_oRequestObj.m_oBody.m_strCallerId = p_pSwitchNotif->GetCallerId();
		l_oRequestObj.m_oBody.m_strCalledId = p_pSwitchNotif->GetCalledId();
		l_oRequestObj.m_oBody.m_strOriginalCallerId = p_pSwitchNotif->GetOriginalCallerId();
		l_oRequestObj.m_oBody.m_strOriginalCalledId = p_pSwitchNotif->GetOriginalCalledId();
		l_oRequestObj.m_oBody.m_strCallDirection = p_pSwitchNotif->GetCallDirection();
		l_oRequestObj.m_oBody.m_strState = p_pSwitchNotif->GetDeviceState();
		l_oRequestObj.m_oBody.m_strTime = p_pSwitchNotif->GetStateTime();
		/*l_oRequestObj.m_oBody.m_strHangupType = p_pSwitchNotif->GetHangupType();
		l_oRequestObj.m_oBody.m_strHangupDevice = p_pSwitchNotif->GetHangupDevice();*/
		l_oRequestObj.m_oBody.m_strCSTACallRefId = std::to_string(p_pSwitchNotif->GetCSTACallRefId());

		//	发送设备状态给DBProcess
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_DeviceStateSync, Queue_CTIDBProcess, SendType_Queue);

		std::string l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send DeviceState Sync Queue: [%s]", l_strSyncMsg.c_str());

		//	广播设备状态给接警席
		l_oRequestObj.m_oHeader = CreateProtocolHeader(Cmd_DeviceStateSync, Topic_DeviceStateSync, SendType_Topic);

		l_strSyncMsg = l_oRequestObj.ToString(m_JsonFactoryPtr->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
		ICC_LOG_DEBUG(m_LogPtr, "Send DeviceState Sync Topic: [%s]", l_strSyncMsg.c_str());
	}
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
		// 通知客户端，待实现
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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_AgentLoginRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			std::string l_strAgent = p_pSwitchNotif->GetAgentId();
			std::string l_strACD = p_pSwitchNotif->GetACDGrp();
			std::string l_strLoginMode = p_pSwitchNotif->GetLoginMode();
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;

			l_oRespondObj.m_oBody.m_strAgent = l_strAgent;
			l_oRespondObj.m_oBody.m_strACD = l_strACD;
			l_oRespondObj.m_oBody.m_strLoginMode = l_strLoginMode;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_AgentLogoutRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			std::string l_strAgent = p_pSwitchNotif->GetAgentId();
			std::string l_strACD = p_pSwitchNotif->GetACDGrp();
			std::string l_strLoginMode = p_pSwitchNotif->GetLoginMode();
			std::string l_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			l_oRespondObj.m_oBody.m_strAgent = l_strAgent;
			l_oRespondObj.m_oBody.m_strACD = l_strACD;
			l_oRespondObj.m_oBody.m_strLoginMode = l_strLoginMode;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_SetAgentStateRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			std::string l_strReadyState = p_pSwitchNotif->GetReadyState();
			l_oRespondObj.m_oBody.m_strAgent = p_pSwitchNotif->GetAgentId();
			l_oRespondObj.m_oBody.m_strACD = p_pSwitchNotif->GetACDGrp();
			l_oRespondObj.m_oBody.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			CRequestMemoryManager::Instance()->DeleteRequestMemory(l_lRequestId);
		}
	}
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

			PROTOCOL::CMakeCallRespond l_oRespondObj;
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_MakeCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strCallRefId = l_strCTICallRefId;
			l_oRespondObj.m_oBody.m_strCallerId = l_strCallerId;
			l_oRespondObj.m_oBody.m_strCalledId = l_strCalledId;
			l_oRespondObj.m_oBody.m_strCaseId = l_strCaseId;
			//l_oRespondObj.m_oBody.m_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_AnswerCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			/*l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->>GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strDevice = p_pSwitchNotif->GetDeviceNum();*/
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_RefuseAnswerRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_HangupRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ClearCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ListenCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			/*
			l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_PickupCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			/*
			l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_BargeInCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			/*l_oRespondObj.m_oBody.m_strCallRefId = p_pSwitchNotif->GetCTICallRefId();
			l_oRespondObj.m_oBody.m_strSponsor = p_pSwitchNotif->GetSponsor();
			l_oRespondObj.m_oBody.m_strTarget = p_pSwitchNotif->GetTarget();*/
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			std::string l_strRespondMsg = l_oRespondObj.ToString(m_JsonFactoryPtr->CreateJson());
			if (l_pRequestMemory->GetNotifiRequest())
			{
				ICC_LOG_DEBUG(m_LogPtr, "Send TaskId [%u] CR_BargeInCall Respond: [%s]",
					l_lRequestId, l_strRespondMsg.c_str());
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

		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CConsultationCallRespond l_oRespondObj;

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ConsultationCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strActiveCallRefId = l_strActiveCallRefId;
			l_oRespondObj.m_oBody.m_strHeldCallRefId = l_strHeldCallRefId;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oRespondObj.m_oBody.m_strTarget = l_strTargetDevice;
			//l_oRespondObj.m_oBody.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;

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

			l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strHeldCallRefId = l_strHeldCallRefId;
			l_oRespondObj.m_oBody.m_strAvtiveCallRefId = l_strActiveCallRefId;
			l_oRespondObj.m_oBody.m_strSponsor = l_strSponsor;
			l_oRespondObj.m_oBody.m_strTarget = l_strTargrt;
			l_oRespondObj.m_oBody.m_strResult = l_strResult;
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;
			/*l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = l_strResult;*/

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

	l_oSyncObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExSync, Topic_TransferCallExSync, SendType_Topic, p_strRelatedId);
	l_oSyncObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	l_oSyncObj.m_oBody.m_strAvtiveCallRefId = p_strActiveCallRefId;
	l_oSyncObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oSyncObj.m_oBody.m_strTarget = p_strTarget;
	l_oSyncObj.m_oBody.m_strResult = (p_nResult == Result_GetReadyAgent_Success ? std::to_string(Result_GetReadyAgent_Success) : ErrorCodeToString(p_nResult));
	l_oSyncObj.m_oHeader.m_strResult = l_oSyncObj.m_oBody.m_strResult;
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

		l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());;

		l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
		l_oRespondObj.m_oBody.m_strAvtiveCallRefId = p_strActiveCallRefId;
		l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
		l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
		l_oRespondObj.m_oBody.m_strResult = p_strResult;

		l_oRespondObj.m_oHeader.m_strResult = p_strResult;
		/*l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
		l_oRespondObj.m_oHeader.m_strResult = p_strResult;*/

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

	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_TransferCallExRespond, "", SendType_Queue, p_strMsgId);;

	l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	l_oRespondObj.m_oBody.m_strAvtiveCallRefId = "";
	l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
	l_oRespondObj.m_oBody.m_strResult = (p_bResult ? Result_Success : Result_Failed);
	l_oRespondObj.m_oHeader.m_strResult = l_oRespondObj.m_oBody.m_strResult;
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

	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_CancelTransferCallExRespond, "", SendType_Queue, p_strMsgId);;

	l_oRespondObj.m_oBody.m_strHeldCallRefId = p_strHeldCallRefId;
	l_oRespondObj.m_oBody.m_strAvtiveCallRefId = "";
	l_oRespondObj.m_oBody.m_strSponsor = p_strSponsor;
	l_oRespondObj.m_oBody.m_strTarget = p_strTarget;
	l_oRespondObj.m_oBody.m_strResult = Result_Success;

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ForcePopCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			//l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_DeflectCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/

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
			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_HoldCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = l_bResult ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_RetrieveCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ReconnectCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			/*l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_ConferenceCallRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());*/

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
	l_oSyncObj.m_oHeader.m_strResult = p_strResult;
	std::string l_strSyncMessage = l_oSyncObj.ToString(m_JsonFactoryPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_LogPtr, "Send Conference Sync: [%s]", l_strSyncMessage.c_str());
}
void CHytProtocol::CR_AddConferenceParty(const std::string& p_strConferenceId, ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	PROTOCOL::CMakeConferenceRespond l_oRespondObj;
	l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_AddConferencePartyRespond);
	l_oRespondObj.m_oBody.m_strConferenceId = p_strConferenceId;
	l_oRespondObj.m_oBody.m_strResult = Result_Success;

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
			l_oRespondObj.m_oBody.m_strState = bOpen ? ConnectState_Connect : ConnectState_Disconnect;
			l_oRespondObj.m_oBody.m_strSwitchType = p_pSwitchNotif->GetSwitchType();
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetDeviceListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetDeviceListSize());
			for each (auto l_pDeviceObj in p_pSwitchNotif->m_DeviceList)
			{
				if (l_pDeviceObj)
				{
					long l_lCSTACallRefId = l_pDeviceObj->GetCSTACallRefId();
					std::string l_strDeviceNum = l_pDeviceObj->GetDeviceNum();
					std::string l_strDeviceType = l_pDeviceObj->GetDeviceType();
					std::string l_strStateTime = l_pDeviceObj->GetStateTime();
					std::string l_strDeviceState = l_pDeviceObj->GetDeviceState();
					std::string l_strCTICallRefId = l_pDeviceObj->GetCTICallRefId();
					std::string strCallerId = l_pDeviceObj->GetCallerId();
					std::string strCalledParty = l_pDeviceObj->GetCalledId();
					std::string strCallDirection = l_pDeviceObj->GetCallDirection();
					std::string l_strReadyState = l_pDeviceObj->GetReadyState();

					PROTOCOL::CGetDeviceListRespond::CBody::CData l_oData;
					l_oData.m_strDevice = l_strDeviceNum;
					l_oData.m_strDeviceType = l_strDeviceType;
					l_oData.m_strState = l_strDeviceState;
					l_oData.m_strTime = l_strStateTime;
					l_oData.m_strCallRefId = l_strCTICallRefId;
					l_oData.m_strCallerId = strCallerId;
					l_oData.m_strCalledId = strCalledParty;
					l_oData.m_strCallDirection = strCallDirection;

					l_oData.m_strReadyState = l_strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;;
					l_oData.m_strLoginMode = l_pDeviceObj->GetLoginMode(); //	login/logout ״̬
					l_oData.m_strDeptCode = l_pDeviceObj->GetDeptCode();
					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}
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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetACDListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetAgentListSize());

			for each (auto l_pDeviceObj in p_pSwitchNotif->GetAgentList())
			{
				if (l_pDeviceObj)
				{
					PROTOCOL::CGetACDListRespond::CBody::CData l_oData;

					std::string strReadyState = l_pDeviceObj->GetReadyState();
					l_oData.m_strAgent = l_pDeviceObj->GetAgentId();
					l_oData.m_strACD = l_pDeviceObj->GetACDGrp();
					l_oData.m_strTime = l_pDeviceObj->GetStateTime();

					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}
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
void CHytProtocol::CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif)
{
	if (p_pSwitchNotif)
	{
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			PROTOCOL::CGetAgentListRespond l_oRespondObj;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetAgentListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());

			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetAgentListSize());

			for each (auto l_pDeviceObj in p_pSwitchNotif->GetAgentList())
			{
				if (l_pDeviceObj)
				{
					PROTOCOL::CGetAgentListRespond::CBody::CData l_oData;

					std::string strReadyState = l_pDeviceObj->GetReadyState();
					l_oData.m_strAgent = l_pDeviceObj->GetAgentId();
					l_oData.m_strACD = l_pDeviceObj->GetACDGrp();
					l_oData.m_strLoginMode = l_pDeviceObj->GetLoginMode();
					l_oData.m_strReadyState = strReadyState.compare(ReadyState_Ready) == 0 ? ReadyState_Idle : ReadyState_Busy;
					l_oData.m_strTime = l_pDeviceObj->GetStateTime();

					l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
				}
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
		long l_lRequestId = p_pSwitchNotif->GetRequestId();
		boost::shared_ptr<CRequestMemory> l_pRequestMemory = CRequestMemoryManager::Instance()->GetRequestMemory(l_lRequestId);
		if (l_pRequestMemory)
		{
			std::string strMessage = l_pRequestMemory->GetNotifiRequest()->GetMessages();
			PROTOCOL::CGetCallListRequest l_oRequestObj;
			if (!l_oRequestObj.ParseString(strMessage, m_JsonFactoryPtr->CreateJson()))
			{
				ICC_LOG_ERROR(m_LogPtr, "Invalid GetCallList request protocol: [%s]", strMessage.c_str());

				return;
			}

			PROTOCOL::CGetCallListRespond l_oRespondObj;
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			//l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());
			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetCallListRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());
			l_oRespondObj.m_oBody.m_strCount = std::to_string(p_pSwitchNotif->GetCallListSize());
			for each (auto l_pCallObj in p_pSwitchNotif->m_CallList)
			{
				PROTOCOL::CGetCallListRespond::CBody::CData l_oData;

				l_oData.m_strCallRefId = l_pCallObj.m_strCTICallRefId;
				l_oData.m_strAgentId = l_pCallObj.m_strCalledParty;
				l_oData.m_strACDGrp = l_pCallObj.m_strCalledId;
				l_oData.m_strCallerId = l_pCallObj.m_strCallerId;
				l_oData.m_strCalledId = l_pCallObj.m_strCalledParty;
				l_oData.m_strState = l_pCallObj.m_strCallState;
				l_oData.m_strStateTime = l_pCallObj.m_strStateTime;

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
							PROTOCOL::CSeatInfo cSeatInfo;
							if (cSeatInfo.Parse(l_strSeatInfo, m_JsonFactoryPtr->CreateJson()))
							{
								l_strDeptCode = cSeatInfo.m_oBody.m_strDeptCode;
							}
						}
					}

					if (l_oRequestObj.m_oBody.calling_dept == l_strDeptCode)
					{
						l_oRespondObj.m_oBody.m_vecData.push_back(l_oData);
					}
				}
			}

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
			l_oRespondObj.m_oHeader.m_strMsgId = l_pRequestMemory->GetMsgId();
			l_oRespondObj.m_oHeader.m_strResult = p_pSwitchNotif->GetResult() ? Result_Success : ErrorCodeToString(p_pSwitchNotif->GetErrorCode());

			//l_oRespondObj.m_oHeader = CreateProtocolHeader(Cmd_GetReadyAgentRespond, "", SendType_Queue, l_pRequestMemory->GetMsgId());;
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

std::string CHytProtocol::_QueryAcdDept(const std::string& strAcd)
{
	/*DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_acd_dept_from_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = strAcd;
	if (m_pDBConnPtr)
	{
		DataBase::IResultSetPtr l_Result = m_pDBConnPtr->Exec(l_SQLRequest);
		ICC_LOG_DEBUG(m_LogPtr, "sql:[%s]", l_Result->GetSQL().c_str());

		if (!l_Result->IsValid())
		{
			ICC_LOG_ERROR(m_LogPtr, "query_acd_dept_from_icc_t_acd_dept failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
			return "";
		}

		if (l_Result->Next())
		{
			std::string strTmp = l_Result->GetValue("dept_code");
			ICC_LOG_DEBUG(m_LogPtr, "_QueryAcdDept dept_code:[%s],acd:[%s]", strTmp.c_str(), strAcd.c_str());
			return strTmp;
		}
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
