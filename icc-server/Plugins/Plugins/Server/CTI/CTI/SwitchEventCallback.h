#pragma once

namespace ICC
{
	class CBusinessImpl;

	class CSwitchEventCallback : public ISwitchEventCallback
	{
	public:
		CSwitchEventCallback(CBusinessImpl* p_SwitchBusiness);
		virtual ~CSwitchEventCallback();

	public:
		//	交换机事件通知
		virtual void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
		virtual void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
		virtual void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
		virtual void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif);
		virtual void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif);
		virtual void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif);
		virtual void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
		virtual void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);
		virtual void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif);

		// 具体命令执行结果反馈( CR-- Command Result)
		virtual void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		virtual void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		virtual void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		virtual void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif);
		virtual void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pRespondNotif);
		virtual void CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif);
		virtual void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif);
		virtual void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif);
	//	virtual void CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif);
	//	virtual void CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif);
		
		virtual void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
		virtual void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
		virtual void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif);
		virtual void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
		virtual void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
		virtual void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);

		virtual void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif);

		virtual void CR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_pSwitchNotif);

	private:
		CBusinessImpl* m_pSwitchBusiness;
	};
}
