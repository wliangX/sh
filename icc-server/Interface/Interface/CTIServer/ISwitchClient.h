#pragma once
//STL
#include <string>
#include <vector>

#include <IResource.h>
#include <CTIServer/ISwitchEventCallback.h>
#include <CTIServer/ISwitchNotifaction.h>

namespace ICC
{
	namespace SwitchClient
	{
		class ISwitchClient :
			public IResource
		{
		public:
			//************************************
			// Method:    {InitSwitch}	初始化交换机 Client
			// Parameter: {ISwitchEventCallbackPtr p_pCallback}	交换机事件回调函数
			// Returns:   {void}	
			//************************************
			virtual void InitSwitch(ISwitchEventCallbackPtr p_pCallback) = 0;

			//************************************
			// Method:    {StartSwitch}	初始化交换机 Client
			// Returns:   {void}	
			//************************************
			virtual void StartSwitch() = 0;

			//************************************
			// Method:    {StopSwitch}	停止交换机 Client
			// Returns:   {void}	
			//************************************
			virtual void StopSwitch() = 0;

			//************************************
			// Method:    {DestroySwitch}	销毁交换机 Client
			// Returns:   {void}	
			//************************************
			virtual void DestroySwitch() = 0;

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//************************************
			// Method:    {AgentLogin}	Agent Login
			// Parameter: {const std::string & p_strAgent}	坐席号
			// Parameter: {const std::string & p_strACD}	ACD 组
			// Returns:   {long}	任务编号
			//************************************
			virtual long AgentLogin(const std::string& p_strAgent, const std::string& p_strACD) = 0;

			//************************************
			// Method:    {AgentLogout}	Agent Logout
			// Parameter: {const std::string & p_strAgent}	坐席号
			// Parameter: {const std::string & p_strACD}	ACD 组
			// Returns:   {long}	任务编号
			//************************************
			virtual long AgentLogout(const std::string& p_strAgent, const std::string& p_strACD) = 0;

			//************************************
			// Method:    {SetAgentState}	设置 Agent Ready/NotReady 状态
			// Parameter: {const std::string & p_strAgent}	坐席号
			// Parameter: {const std::string & p_strACD}	ACD 组
			// Parameter: {const std::string & p_strReadyState}	Ready/NotReady
			// Returns:   {long}	任务编号
			//************************************
			virtual long SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState) = 0;

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//************************************
			// Method:    {MakeCall}	当呼
			// Parameter: {const std::string & p_strCallerId}	主叫
			// Parameter: {const std::string & p_strCalledId}	被叫
			// Parameter: {const std::string & p_strTargetType}	被叫类型
			// Parameter: {const std::string & p_strCaseId}	警情 ID
			// Returns:   {long}	任务编号
			//************************************
			virtual long MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId) = 0;

			//************************************
			// Method:    {AnswerCall}	话务应答
			// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID	
			// Parameter: {const std::string & p_strDevice}	发起应发的坐席号
			// Returns:   {long}	任务编号
			//************************************
			virtual long AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    {RefuseAnswer}	拒接
			// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID	
			// Parameter: {const std::string & p_strDevice}	发起拒接的坐席号
			// Returns:   {long}	任务编号
			//************************************
			virtual long RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    ClearCall		挂断整个话务
			// FullName:  ICC::SwitchClient::ISwitchClient::ClearCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string & p_strCTICallRefId	应用层话务ID
			// Parameter: const std::string & p_strSponsor	发起强拆的坐席号
			// Parameter: {const std::string & p_strTarget}	被强拆的坐席号
			//************************************
			virtual long ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    {Hangup}	挂断
			// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID
			// Parameter: {const std::string & p_strSponsor}	发起挂断的坐席号
			// Parameter: {const std::string & p_strDevice}	被挂断的话务成员
			// Returns:   {long}	任务编号
			//************************************
			virtual long Hangup(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    {ListenCall}	监听
			// Parameter: {const std::string& p_strCTICallRefId}	被监听话务ID
			// Parameter: {const std::string & p_strSponsor}	发起监听的坐席号
			// Parameter: {const std::string & p_strTarget}	被监听的坐席号
			// Returns:   {long}	任务编号
			//************************************
			virtual long ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    {PickupCall}	代答
			// Parameter: {const std::string& p_strCTICallRefId}	被代答的话务ID
			// Parameter: {const std::string & p_strSponsor}	发起代答的坐席号
			// Parameter: {const std::string & p_strTarget}	被代答的坐席号
			// Returns:   {long}	任务编号
			//************************************
			virtual long PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    BargeInCall	强插
			// FullName:  ACS::ISwitchClient::BargeInCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	被强插的话务 ID
			// Parameter: const std::string & p_strSponsor	发起强插的坐席号
			// Parameter: const std::string & p_strTarget	被强插的坐席号
			//************************************
			virtual long BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    ForcePopCall	强拆
			// FullName:  ACS::ISwitchClient::ForcePopCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	被强拆的话务 ID
			// Parameter: const std::string & p_strSponsor	发起强拆的坐席号
			// Parameter: const std::string & p_strTarget	发起强拆的坐席号
			//************************************
			virtual long ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    ConsultationCall	咨询
			// FullName:  ACS::ISwitchClient::ConsultationCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	咨询话务ID
			// Parameter: const std::string & p_strSponsor	发起咨询的坐席号
			// Parameter: const std::string & p_strTarget	咨询目标号码
			// Parameter: const std::string & p_strTargetDeviceType	咨询目标号码的类型
			// Parameter: bool p_bIsTransferCall	是否为转移产生的咨询，是：true，否：false
			//************************************
			virtual long ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall = false) = 0;

			//************************************
			// Method:    ConsultationCallEx	咨询
			// FullName:  ACS::ISwitchClient::TransferCall
			// Access:    virtual public 
			// Returns:   long
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId		当前话务 ID
			// Parameter: const std::string & p_strSponsor	发起转移的坐席号
			// Parameter: const std::string & p_strTarget	转移的目标 ACD
			//************************************
			//virtual long ConsultationCallEx(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    TransferCall	转移
			// FullName:  ACS::ISwitchClient::TransferCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string& p_strActiveCallRefId	当前活动的话务ID
			// Parameter: const std::string& p_strHeldCallRefId		被保留的话务ID
			// Parameter: const std::string & p_strSponsor	发起转移的坐席号
			// Parameter: const std::string & p_strTarget	转移的目标坐席号
			//************************************
			virtual long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    DeflectCall	偏转
			// FullName:  ACS::CSwitchClientImpl::DeflectCall
			// Access:    virtual public 
			// Returns:   long
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	被偏转话务 ID
			// Parameter: const std::string & p_strSponsor	发起偏转的坐席号
			// Parameter: const std::string & p_strTarget	偏转目标坐席号
			//************************************
			virtual long DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//************************************
			// Method:    HoldCall	保留
			// FullName:  ACS::CSwitchClientImpl::HoldCall
			// Access:    virtual public 
			// Returns:   long
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	被保留话务 ID
			// Parameter: const std::string & p_strSponsor	发起保留的坐席号
			// Parameter: const std::string & p_strDevice	被保留的号码
			//************************************
			virtual long HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    RetrieveCall	重拾
			// FullName:  ACS::ISwitchClient::RetrieveCall
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	当前话务 ID
			// Parameter: const std::string & p_strSponsor	发起重拾的坐席号
			// Parameter: const std::string & p_strDevice	重拾的坐席号
			//************************************
			virtual long RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    ConferenceCall	通话加入会议
			// FullName:  ACS::ISwitchClient::ConferenceCall
			// Access:    virtual public 
			// Returns:   long	任务编号	
			// Qualifier:
			// Parameter: const std::string & p_strTarget	加入会议的成员
			// Parameter: const std::string& p_strHeldCallRefId		被保留的话务 ID
			// Parameter: const std::string& p_strActiveCallRefId	当前话务的 ID
			//************************************
			virtual long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId) = 0;

			//************************************
			// @Method:    ReconnectCall	重连
			// @Parameter: {const std::string& p_strActiveCallRefId}	当前话务 ID
			// @Parameter: {const std::string& p_strHeldCallRefId}		保留话务 ID
			// @Parameter: {const std::string & p_strSponsor}	发起重连的坐席号
			// @Parameter: {const std::string & p_strDevice}	重连的坐席号
			// @Returns:   {long}	任务编号
			//************************************
			virtual long ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice) = 0;

			//************************************
			// Method:    TakeOverCall	接管话务
			// FullName:  ACS::CSwitchClientImpl::TakeOverCall
			// Access:    virtual public 
			// Returns:   long
			// Qualifier:
			// Parameter: const std::string& p_strCTICallRefId	被接管话务 ID
			// Parameter: const std::string & p_strSponsor	发起接管的坐席号
			// Parameter: const std::string & p_strTarget	被接管的目标坐席号
			//************************************
			virtual long TakeOverCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget) = 0;

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//************************************
			// Method:    {GetCTIConnectState}	获取 CTI 与 交换机网络连接状态
			// Returns:   {long}	任务编号
			//************************************
			virtual long GetCTIConnectState() = 0;

			//************************************
			// Method:    {GetDeviceList}	获取所有话机设备状态
			// Returns:   {long}	任务编号
			//************************************
			virtual long GetDeviceList() = 0;

			//************************************
			// Method:    {GetACDList}	获取所有 ACD Agent
			// @Parameter: 
			// Returns:   {long}	任务编号
			//************************************
			virtual long GetACDList() = 0;

			//************************************
			// Method:    {GetAgentList}	获取所有 Agent 的状态
			// @Parameter: {const std::string & p_strACDGrp}	ACD 组号，为空则查所有的 ACD 组
			// Returns:   {long}	任务编号
			//************************************
			virtual long GetAgentList(const std::string& p_strACDGrp) = 0;

			//************************************
			// Method:    {GetCallList}	获取所有当前话务
			// Returns:   {long}	任务编号
			//************************************
			virtual long GetCallList() = 0;

			//************************************
			// Method:    GetReadyAgent	获取 ACD 组一个空闲的 Agent
			// FullName:  ACS::ISwitchClient::GetReadyAgent
			// Access:    virtual public 
			// Returns:   long	任务编号
			// Qualifier:
			// Parameter: const std::string & p_strACDGrp	ACD 组
			//************************************
			virtual long GetReadyAgent(const std::string& p_strACDGrp) = 0;

			///////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//************************************
			// Method:    {SetBlackList}	设置黑名单
			// Parameter: {const std::string & p_strLimitNum}	受限号码
			// Parameter: {const std::string & p_strLimitType}	受限类型（字头、黑名单...）
			// Returns:   {long}	任务编号
			//************************************
			virtual long SetBlackList(const std::string& p_strLimitNum, const std::string& p_strLimitType = "") = 0;

			//************************************
			// Method:    {DeleteBlackList}	删除黑名单
			// Parameter: {const std::string & p_strLimitNum}	受限号码
			// Returns:   {long}	任务编号
			//************************************
			virtual long DeleteBlackList(const std::string& p_strLimitNum) = 0;

			//************************************
			// Method:    {DeleteAllBlackList}	删除所有黑名单
			// Returns:   {long}	
			//************************************
			virtual long DeleteAllBlackList() = 0;

			virtual std::string GetDeviceState(const std::string& strTarget) = 0;
			//同步获取空闲坐席
			virtual std::string GetReadyAgentEx(const std::string& strTarget) = 0;

			virtual std::string GetCallerID(const std::string& p_strInCTICallRefId, std::string& p_strCalled) = 0;

			virtual long GetFreeAgentList(const std::string& p_strACDGrp,const std::string &p_strDeptCode) = 0;

			virtual bool GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime) = 0;
			///////////////////////////////////////////////////////////////
			//Freeswitch Aes异步请求扩展接口
			virtual long FSAesAsyncRequestEx(const std::string p_strCmdName, const std::string& p_strResquestData) = 0;

			//Freeswitch Aes对接会议类接口(先同步实现)
			virtual long CreateConference(const std::string& p_strCompere, std::string& p_strConferenceId) = 0;

			virtual std::string QueryConferenceByCallid(const std::string& p_strCallRefId, std::string& p_strCompere, std::string& p_strBargeInSponsor) = 0;
			//virtual long QueryConferenceByCallid(const std::string& p_strCallRefId, int& p_nCreateType, std::string& p_strConferenceId) = 0;
			virtual long AddConferencePartyEx(const std::string& p_strCallRefId, const std::string& p_strTarget, const std::string& p_strTargetType, std::string& p_strConferenceId) = 0;
			virtual long AddConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType) = 0;
			
			virtual long DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType) = 0;
			virtual long DeleteConferenceParty(const std::string& p_strConferenceId, std::string& p_strTarget, const std::string& p_strTargetType) = 0;
			virtual long GetConferenceParty(const std::string& p_strConferenceId, std::string& p_strPartyData) = 0;
		};

		typedef boost::shared_ptr<ISwitchClient> ISwitchClientPtr;
	}
}

#define ICCSwitchClientResourceName "ICC.Com.SwitchClient.ResourceName"