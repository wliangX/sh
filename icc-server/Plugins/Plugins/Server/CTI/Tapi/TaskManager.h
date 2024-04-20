#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		TaskManage.h
 @created	2018/01/19
 @brief		CTaskManager负责管理所有任务
 CTaskManager管理两个队列：
 1，正在等待switchManage执行的任务队列
 2，已经执行，等待返回结果的任务队列

 @author	psy
 */

#include "Task.h"

namespace ICC
{
#define STATR_TASKID		1
#define END_TASKID			0xFFFFF

	class CTaskManager
	{
	public:
		CTaskManager(void);
		virtual ~CTaskManager(void);

		static boost::shared_ptr<CTaskManager> Instance();
		void ExitInstance();

		void SetLogPtr(Log::ILogPtr	p_pLog){ m_pLog = p_pLog; }

	private:
		long CreateTaskId();

	public:
		boost::shared_ptr<CTask> CreateNewTask();
		boost::shared_ptr<CTask> CreateNewTask(E_TASK_NAME p_nInTaskName, ISwitchNotifPtr p_pInSwitchNotif, long& p_lOutTaskId);

		void AddCmdTask(boost::shared_ptr<CTask> p_pTask);
		long AddCmdTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif);
		boost::shared_ptr<CTask> GetCmdTaskHeader();
		bool DeleteCmdTask(long p_lTaskId);
		int GetCmdTaskCount();
		void ClearCmdTaskQueue();

		void AddExcutedTask(boost::shared_ptr<CTask> p_pTask);
		boost::shared_ptr<CTask> GetExcutedTask(long p_lTaskId);
		boost::shared_ptr<CTask> GetExcutedTask(const std::string& p_strTaskId);
		bool DeleteExcusedTask(const std::string& p_strTaskId);
		bool DeleteExcusedTask(long p_lTaskId);
		void ClearExcutedTaskQueue();
		bool ExcuseTaskTimeout(E_TASK_NAME& p_nTaskName, long& p_lTaskId);
		void SetInvokeId(long p_lTaskId, long p_lInvokeId);
		long GetTaskIdByInvokeId(long p_lInvokeId);
		E_TASK_NAME GetTaskNameByInvokeId(long p_lInvokeId);

		void AddSwitchEventTask(boost::shared_ptr<CTask> p_pTask);
		long AddSwitchEventTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif);
		boost::shared_ptr<CTask> GetSwitchEventTaskHeader();
		bool DeleteSwitchEventTask(long p_lTaskId);
		int GetSwitchEventTaskCount();
		void ClearSwitchEventTaskQueue();

		//////////////////////////////////////////////////////////////////////////
		//************************************
		// Method:    {AgentLogin}	Agent Login
		// Parameter: {const std::string & p_strAgent}	坐席号
		// Parameter: {const std::string & p_strACD}	ACD 组
		// Returns:   {long}	任务编号
		//************************************
		long AgentLogin(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {AgentLogout}	Agent Logout
		// Parameter: {const std::string & p_strAgent}	坐席号
		// Parameter: {const std::string & p_strACD}	ACD 组
		// Returns:   {long}	任务编号
		//************************************
		long AgentLogout(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {SetAgentState}	设置 Agent Ready/NotReady 状态
		// Parameter: {const std::string & p_strAgent}	坐席号
		// Parameter: {const std::string & p_strACD}	ACD 组
		// Parameter: {const std::string & p_strReadyState}	Ready/NotReady
		// Returns:   {long}	任务编号
		//************************************
		long SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState, const std::string& p_strLogoutReason);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//************************************
		// Method:    {MakeCall}	单呼
		// Parameter: {const std::string & p_strCallerId}	主叫
		// Parameter: {const std::string & p_strCalledId}	被叫
		// Parameter: {const std::string & p_strTargetType}	被叫类型
		// Parameter: {const std::string & p_strCaseId}	警情 ID
		// Returns:   {long}	任务编号
		//************************************
		long MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId);

		//************************************
		// Method:    {AnswerCall}	话务应答
		// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID	
		// Parameter: {const std::string & p_strDevice}	发起应发的坐席号
		// Returns:   {long}	任务编号
		//************************************
		long AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {RefuseAnswer}	拒接
		// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID	
		// Parameter: {const std::string & p_strDevice}	发起拒接的坐席号
		// Returns:   {long}	任务编号
		//************************************
		long RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {Hangup}	挂断
		// Parameter: {const std::string& p_strCTICallRefId}	应用层话务ID
		// Parameter: {const std::string & p_strSponsor}	发起挂断的坐席号
		// Parameter: {const std::string & p_strDevice}	被挂断的话务成员
		// Returns:   {long}	任务编号
		//************************************
		long Hangup(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);
		long Hangup(long p_strCallRefId, const std::string & p_strSponsor, const std::string& p_strDevice);
		long ForceHangup(long p_strCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ClearCall	强拆整个话务
		// Parameter: const std::string & p_strCTICallRefId	应用层话务ID
		// Parameter: const std::string & p_strSponsor	发起强拆的坐席号
		//************************************
		long ClearCall(const std::string& p_strCTICallRefId, const std::string & p_strSponsor);

		//************************************
		// Method:    {ListenCall}	监听
		// Parameter: {const std::string& p_strCTICallRefId}	被监听话务ID
		// Parameter: {const std::string & p_strSponsor}	发起监听的坐席号
		// Parameter: {const std::string & p_strTarget}	被监听的坐席号
		// Returns:   {long}	任务编号
		//************************************
		long ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    {PickupCall}	代答
		// Parameter: {const std::string& p_strCTICallRefId}	被代答的话务ID
		// Parameter: {const std::string & p_strSponsor}	发起代答的坐席号
		// Parameter: {const std::string & p_strTarget}	被代答的坐席号
		// Returns:   {long}	任务编号
		//************************************
		long PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    BargeInCall	强插
		// FullName: ICC::CTaskManager::BargeInCall
		// Access:    public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	被强插的话务 ID
		// Parameter: const std::string & p_strSponsor	发起强插的坐席号
		// Parameter: const std::string & p_strTarget	被强插的坐席号
		//************************************
		long BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ForcePopCall	强拆一个话务成员
		// FullName: ICC::CTaskManager::ForcePopCall
		// Access:    public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	被强拆的话务 ID
		// Parameter: const std::string & p_strSponsor	发起强拆的坐席号
		// Parameter: const std::string & p_strTarget	被强拆的目标坐席号
		//************************************
		long ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ConsultationCall	咨询
		// FullName: ICC::CTaskManager::ConsultationCall
		// Access:    public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	咨询话务ID
		// Parameter: const std::string & p_strSponsor	发起咨询的坐席号
		// Parameter: const std::string & p_strTarget	咨询目标号码
		// Parameter: const std::string & p_strTargetDeviceType	咨询目标号码的类型
		// Parameter: bool p_bIsTransferCall	是否为转移产生的咨询，是：true，否：false
		//************************************
		long ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall = false);

		//************************************
		// Method:    TransferCall	转移
		// FullName: ICC::CTaskManager::TransferCall
		// Access:    public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string& p_strActiveCallRefId	当前活动的话务ID
		// Parameter: const std::string& p_strHeldCallRefId		被保留的话务ID
		// Parameter: const std::string & p_strSponsor	发起转移的坐席号
		// Parameter: const std::string & p_strTarget	转移的目标坐席号
		//************************************
		long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    DeflectCall	偏转
		// FullName:  ACS::CSwitchClientImpl::DeflectCall
		// Access:    public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	被偏转话务 ID
		// Parameter: const std::string & p_strSponsor	发起偏转的坐席号
		// Parameter: const std::string & p_strTarget	偏转目标坐席号
		//************************************
		long DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    HoldCall	保留
		// FullName:  ACS::CSwitchClientImpl::HoldCall
		// Access:    public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	被保留话务 ID
		// Parameter: const std::string & p_strSponsor	发起保留的坐席号
		// Parameter: const std::string & p_strDevice	被保留的号码
		//************************************
		long HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    RetrieveCall	重拾
		// FullName: ICC::CTaskManager::RetrieveCall
		// Access:    public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	当前话务 ID
		// Parameter: const std::string & p_strSponsor	发起重拾的坐席号
		// Parameter: const std::string & p_strDevice	重拾的坐席号
		//************************************
		long RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ConferenceCall	通话加入会议
		// FullName: ICC::CTaskManager::ConferenceCall
		// Access:    public 
		// Returns:   long	任务编号	
		// Qualifier:
		// Parameter: const std::string & p_strTarget	加入会议的成员
		// Parameter: const std::string& p_strHeldCallRefId		被保留的话务 ID
		// Parameter: const std::string& p_strActiveCallRefId	当前话务的 ID
		//************************************
		long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId);

		//************************************
		// @Method:    ReconnectCall	重连
		// @Parameter: {const std::string& p_strActiveCallRefId}	当前话务 ID
		// @Parameter: {const std::string& p_strHeldCallRefId}		保留话务 ID
		// @Parameter: {const std::string & p_strSponsor}	发起重连的坐席号
		// @Parameter: {const std::string & p_strDevice}	重连的坐席号
		// @Returns:   {long}	任务编号
		//************************************
		long ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		long RemoveFromConference(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//////////////////////////////////////////////////////////////////////////
		// Method:    {GetCTIConnectState}	获取 CTI 与 交换机网络连接状态
		// Returns:   {long}	任务编号
		//************************************
		long GetCTIConnectState();

		//************************************
		// Method:    {GetDeviceList}	获取所有话机设备状态
		// Returns:   {long}	任务编号
		//************************************
		long GetDeviceList();

		//************************************
		// Method:    {GetACDList}	获取所有 ACD Agent 
		// @Parameter: 
		// Returns:   {long}	任务编号
		//************************************
		long GetACDList();

		//************************************
		// Method:    {GetAgentList}	获取所有 Agent 的状态
		// @Parameter: {const std::string & p_strACDGrp}	ACD 组号，为空则查所有的 ACD 组
		// Returns:   {long}	任务编号
		//************************************
		long GetAgentList(const std::string & p_strACDGrp);

		//************************************
		// Method:    {GetCallList}	获取所有当前话务
		// Returns:   {long}	任务编号
		//************************************
		long GetCallList();

		//************************************
		// Method:    GetReadyAgent	获取 ACD 组一个空闲的 Agent
		// FullName:  ACS::ISwitchClient::GetReadyAgent
		// Access:    virtual public 
		// Returns:   long	任务编号
		// Qualifier:
		// Parameter: const std::string & p_strACDGrp	ACD 组
		//************************************
		long GetReadyAgent(const std::string& p_strACDGrp);

	private:
		static boost::shared_ptr<CTaskManager> m_pInstance;
		
		Log::ILogPtr	m_pLog;

		static long		m_slTaskId;

		std::mutex		m_cmdTaskLock;
		std::mutex		m_excuteTaskLock;
		std::mutex		m_switchEventTaskLock;

		//维护正在等待执行的任务
		std::map<long, boost::shared_ptr<CTask>>		m_cmdTaskQueue;
		//维护已经执行的所有任务
		std::map<long, boost::shared_ptr<CTask>>		m_excutedTaskQueue;
		// Switch 主动推送的事件或返回的执行结果
		std::map<long, boost::shared_ptr<CTask>>		m_switchEventTaskQueue;
	};
}	// end namespace

