#pragma once

#include <IObject.h>
//#include <CTIServer/IProperty.h>
//#include <CTIServer/ICommonDef.h>
#include <CTIServer/ISwitchNotifaction.h>

namespace ICC 
{
	class ISwitchEventCallback :
		public IObject
	{
	public:
		ISwitchEventCallback(){}
		virtual ~ISwitchEventCallback(){};

	public:
		//************************************
		// Method:    {Event_CTIConnState}	CTI 与交换机网络连接状态事件通知
		// Parameter: {ICTIConnStateNotifPtr p_pSwitchNotif}	AES网络连接状态结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_AgentState}	Agent 状态（Login/Logout、Ready/NotReady）事件通知
		// Parameter: {IAgentStateNotifPtr p_pSwitchNotif}	Agent 状态结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_CallState}	话务状态（Incoming（呼入）、Waiting（排队）、Assign（分配）、Release（早释））事件通知
		// Parameter: {ICallStateNotifPtr p_pSwitchNotif}	话务状态结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_CallState(ICallStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_CallOver}	一个话务结束
		// Parameter: {ICallOverNotifPtr p_pSwitchNotif}	话务结束结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_DeviceState}	话机设备状态（振铃、回铃、通话...）事件通知
		// Parameter: {IDeviceStateNotifPtr p_pSwitchNotif}	话机状态结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_ConferenceeHangup}	会议成员挂机事件通知
		// Parameter: {IDeviceStateNotifPtr p_pSwitchNotif}	话机状态结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_RefuseCall}	拒接话务事件通知
		// Parameter: {IRefuseCallNotifPtr p_pSwitchNotif}	拒接话务结构体
		// Returns:   {void}	
		//************************************
		virtual void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    Event_Failed	失败事件通知
		// FullName:  ACS::ISwitchEventCallback::Event_Failed
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IFailedEventNotifPtr p_pSwitchNotif	失败事件结构体
		//************************************
		virtual void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif) = 0;

		virtual void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif) = 0;

		// --------- 具体命令执行结果反馈( CR-- Command Result) ------------------

		//************************************
		// Method:    {CR_AgentLogin}	Login 结果返回
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Login 结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AgentLogout}	Logout 结果返回
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Logout 结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_SetAgentState}	设置 Agent 状态（Ready/NotReady）结果返回
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Agent 状态（Ready/NotReady）结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_MakeCall}	单呼结果返回
		// Parameter: {IMakeCallResultNotifPtr p_pSwitchNotif}	单呼结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AnswerCall}	应答结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	应答结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_RefuseAnswer	拒接结果返回
		// FullName:  ACS::ISwitchEventCallback::CR_RefuseAnswer
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif	拒接结果结构体
		//************************************
		virtual void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_Hangup		挂机结果返回
		// FullName:  ACS::ISwitchEventCallback::CR_Hangup
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif
		//************************************
		virtual void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_ClearCall	挂断整个话务结果返回
		// FullName:  ICC::ISwitchEventCallback::CR_ClearCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif
		//************************************
		virtual void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ListenCall}	监听结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	监听结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_PickupCall}	代答结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	代答结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_BargeInCall} 强插结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	强插结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ForcePopCall}	强拆结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif} 强拆结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_ConsultationCall	咨询结果返回
		// FullName:  ACS::ISwitchEventCallback::CR_ConsultationCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IConsultationCallResultNotifPtr p_pResultNotif	咨询结果结构体
		//************************************
		virtual void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pResultNotif) = 0;

		//************************************
		// Method:    CR_TransferCall	转移结果返回
		// FullName:  ACS::ISwitchEventCallback::CR_TransferCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ITransferCallResultNotifPtr p_pSwitchNotif	转移结果结构体
		//************************************
		virtual void CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_DeflectCall}	偏转结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	偏转结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_HoldCall} 保留结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	保留结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_RetrieveCall} 重拾结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	重拾结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ReconnectCall}	重拾结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif} 重拾结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ConferenceCall} 合并会议结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	合并会议结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_TakeOverCall} 接管话务结果返回
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	合并会议结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AddConferenceParty} 添加会议成员结果返回
		// Parameter: {IAddConferencePartyNotifPtr p_pSwitchNotif}	添加会议成员结果结构体
		// Returns:   {void}	
		//************************************
//		virtual void CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_DeleteConferenceParty}	 删除会议成员结果返回
		// Parameter: {IDeleteConferencePartyNotifPtr p_pSwitchNotif} 删除会议成员结果结构体
		// Returns:   {void}	
		//************************************
//		virtual void CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetCTIConnState}	获取 CTI 网络状态结果返回
		// Parameter: {IGetCTIConnStateResultNotifPtr p_pSwitchNotif}	AES 网络连接状态结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetDeviceList}	获取话机设备状态结果返回	
		// Parameter: {IGetDeviceListResultNotifPtr p_pSwitchNotif}	话机状态结果结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetACDList}	获取 ACD Agent 结果返回
		// Parameter: {IGetACDListResultNotifPtr p_pSwitchNotif}	ACD Agent 结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetAgentList}	获取 Agent 状态（Login/Logout、Ready/NotReady）结果返回
		// Parameter: {IGetAgentListResultNotifPtr p_pSwitchNotif}	Agent 状态结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetCallList}	获取当前话务结果返回
		// Parameter: {IGetCallListResultNotifPtr p_pSwitchNotif}	当前话务结构体
		// Returns:   {void}	
		//************************************
		virtual void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_GetReadyAgent	获取空闲 ACD 组 Agent 结果
		// FullName:  ACS::ISwitchEventCallback::CR_GetReadyAgent
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IGetReadyAgentResultNotifPtr p_pSwitchNotif	空闲 ACD 组 Agent 结构体
		//************************************
		virtual void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif) = 0;

		//获取空闲坐席
		virtual void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif) = 0;

		//FS aes事件扩展接口
		virtual void CR_FSAesEventEx(const std::string &p_strCmdName,ISwitchNotifPtr p_pSwitchNotif) = 0;
	};

	typedef boost::shared_ptr<ISwitchEventCallback>	ISwitchEventCallbackPtr;

} /*namespace ICC*/
