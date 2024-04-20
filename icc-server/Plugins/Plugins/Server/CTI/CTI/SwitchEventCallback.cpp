#include "Boost.h"

#include "SwitchEventCallback.h"
#include "BusinessImpl.h"


CSwitchEventCallback::CSwitchEventCallback(CBusinessImpl* pSwitchBusiness) : m_pSwitchBusiness(pSwitchBusiness)
{
	//
}

CSwitchEventCallback::~CSwitchEventCallback()
{
	//
}


//////////////////////////////////////////////////////////////////////////
//	交换机事件
void CSwitchEventCallback::Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_CTIConnState(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_AgentState(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_CallState(ICallStateNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_CallState(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_CallOver(ICallOverNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_CallOver(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_DeviceState(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_ConferenceHangup(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_RefuseCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::Event_Failed(IFailedEventNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_Failed(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->Event_BlackTransfer(p_pSwitchNotif);
	}
}
//////////////////////////////////////////////////////////////////////////

// 具体命令执行结果反馈( CR-- Command Result)
void CSwitchEventCallback::CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_AgentLogin(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_AgentLogout(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_SetAgentState(p_pSwitchNotif);;
	}
}
void CSwitchEventCallback::CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_MakeCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_AnswerCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_RefuseAnswer(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_Hangup(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ClearCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ListenCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_PickupCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_BargeInCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ForcePopCall(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_ConsultationCall(IConsultationCallResultNotifPtr p_pRespondNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ConsultationCall(p_pRespondNotif);
	}
}
void CSwitchEventCallback::CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_TransferCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_DeflectCall(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_HoldCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_RetrieveCall(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ReconnectCall(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_ConferenceCall(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_TakeOverCall(p_pSwitchNotif);
	}
}

/*
void CSwitchEventCallback::CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_AddConferenceParty(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_DeleteConferenceParty(p_pSwitchNotif);
	}
}*/

void CSwitchEventCallback::CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetCTIConnState(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetDeviceList(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetACDList(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetAgentList(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetFreeAgentList(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetCallList(p_pSwitchNotif);
	}
}
void CSwitchEventCallback::CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_GetReadyAgent(p_pSwitchNotif);
	}
}

void CSwitchEventCallback::CR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_pSwitchNotif)
{
	if (m_pSwitchBusiness)
	{
		m_pSwitchBusiness->CR_FSAesEventEx(p_strCmdName, p_pSwitchNotif);
	}
}
