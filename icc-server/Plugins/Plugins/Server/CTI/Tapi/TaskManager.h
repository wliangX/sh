#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		TaskManage.h
 @created	2018/01/19
 @brief		CTaskManager���������������
 CTaskManager�����������У�
 1�����ڵȴ�switchManageִ�е��������
 2���Ѿ�ִ�У��ȴ����ؽ�����������

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
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Returns:   {long}	������
		//************************************
		long AgentLogin(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {AgentLogout}	Agent Logout
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Returns:   {long}	������
		//************************************
		long AgentLogout(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {SetAgentState}	���� Agent Ready/NotReady ״̬
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Parameter: {const std::string & p_strReadyState}	Ready/NotReady
		// Returns:   {long}	������
		//************************************
		long SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState, const std::string& p_strLogoutReason);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//************************************
		// Method:    {MakeCall}	����
		// Parameter: {const std::string & p_strCallerId}	����
		// Parameter: {const std::string & p_strCalledId}	����
		// Parameter: {const std::string & p_strTargetType}	��������
		// Parameter: {const std::string & p_strCaseId}	���� ID
		// Returns:   {long}	������
		//************************************
		long MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId);

		//************************************
		// Method:    {AnswerCall}	����Ӧ��
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID	
		// Parameter: {const std::string & p_strDevice}	����Ӧ������ϯ��
		// Returns:   {long}	������
		//************************************
		long AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {RefuseAnswer}	�ܽ�
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID	
		// Parameter: {const std::string & p_strDevice}	����ܽӵ���ϯ��
		// Returns:   {long}	������
		//************************************
		long RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {Hangup}	�Ҷ�
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID
		// Parameter: {const std::string & p_strSponsor}	����Ҷϵ���ϯ��
		// Parameter: {const std::string & p_strDevice}	���ҶϵĻ����Ա
		// Returns:   {long}	������
		//************************************
		long Hangup(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);
		long Hangup(long p_strCallRefId, const std::string & p_strSponsor, const std::string& p_strDevice);
		long ForceHangup(long p_strCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ClearCall	ǿ����������
		// Parameter: const std::string & p_strCTICallRefId	Ӧ�ò㻰��ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		//************************************
		long ClearCall(const std::string& p_strCTICallRefId, const std::string & p_strSponsor);

		//************************************
		// Method:    {ListenCall}	����
		// Parameter: {const std::string& p_strCTICallRefId}	����������ID
		// Parameter: {const std::string & p_strSponsor}	�����������ϯ��
		// Parameter: {const std::string & p_strTarget}	����������ϯ��
		// Returns:   {long}	������
		//************************************
		long ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    {PickupCall}	����
		// Parameter: {const std::string& p_strCTICallRefId}	������Ļ���ID
		// Parameter: {const std::string & p_strSponsor}	����������ϯ��
		// Parameter: {const std::string & p_strTarget}	���������ϯ��
		// Returns:   {long}	������
		//************************************
		long PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    BargeInCall	ǿ��
		// FullName: ICC::CTaskManager::BargeInCall
		// Access:    public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǿ��Ļ��� ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		// Parameter: const std::string & p_strTarget	��ǿ�����ϯ��
		//************************************
		long BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ForcePopCall	ǿ��һ�������Ա
		// FullName: ICC::CTaskManager::ForcePopCall
		// Access:    public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǿ��Ļ��� ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		// Parameter: const std::string & p_strTarget	��ǿ���Ŀ����ϯ��
		//************************************
		long ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ConsultationCall	��ѯ
		// FullName: ICC::CTaskManager::ConsultationCall
		// Access:    public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ѯ����ID
		// Parameter: const std::string & p_strSponsor	������ѯ����ϯ��
		// Parameter: const std::string & p_strTarget	��ѯĿ�����
		// Parameter: const std::string & p_strTargetDeviceType	��ѯĿ����������
		// Parameter: bool p_bIsTransferCall	�Ƿ�Ϊת�Ʋ�������ѯ���ǣ�true����false
		//************************************
		long ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall = false);

		//************************************
		// Method:    TransferCall	ת��
		// FullName: ICC::CTaskManager::TransferCall
		// Access:    public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strActiveCallRefId	��ǰ��Ļ���ID
		// Parameter: const std::string& p_strHeldCallRefId		�������Ļ���ID
		// Parameter: const std::string & p_strSponsor	����ת�Ƶ���ϯ��
		// Parameter: const std::string & p_strTarget	ת�Ƶ�Ŀ����ϯ��
		//************************************
		long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    DeflectCall	ƫת
		// FullName:  ACS::CSwitchClientImpl::DeflectCall
		// Access:    public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ƫת���� ID
		// Parameter: const std::string & p_strSponsor	����ƫת����ϯ��
		// Parameter: const std::string & p_strTarget	ƫתĿ����ϯ��
		//************************************
		long DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    HoldCall	����
		// FullName:  ACS::CSwitchClientImpl::HoldCall
		// Access:    public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	���������� ID
		// Parameter: const std::string & p_strSponsor	����������ϯ��
		// Parameter: const std::string & p_strDevice	�������ĺ���
		//************************************
		long HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    RetrieveCall	��ʰ
		// FullName: ICC::CTaskManager::RetrieveCall
		// Access:    public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǰ���� ID
		// Parameter: const std::string & p_strSponsor	������ʰ����ϯ��
		// Parameter: const std::string & p_strDevice	��ʰ����ϯ��
		//************************************
		long RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ConferenceCall	ͨ���������
		// FullName: ICC::CTaskManager::ConferenceCall
		// Access:    public 
		// Returns:   long	������	
		// Qualifier:
		// Parameter: const std::string & p_strTarget	�������ĳ�Ա
		// Parameter: const std::string& p_strHeldCallRefId		�������Ļ��� ID
		// Parameter: const std::string& p_strActiveCallRefId	��ǰ����� ID
		//************************************
		long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId);

		//************************************
		// @Method:    ReconnectCall	����
		// @Parameter: {const std::string& p_strActiveCallRefId}	��ǰ���� ID
		// @Parameter: {const std::string& p_strHeldCallRefId}		�������� ID
		// @Parameter: {const std::string & p_strSponsor}	������������ϯ��
		// @Parameter: {const std::string & p_strDevice}	��������ϯ��
		// @Returns:   {long}	������
		//************************************
		long ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		long RemoveFromConference(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//////////////////////////////////////////////////////////////////////////
		// Method:    {GetCTIConnectState}	��ȡ CTI �� ��������������״̬
		// Returns:   {long}	������
		//************************************
		long GetCTIConnectState();

		//************************************
		// Method:    {GetDeviceList}	��ȡ���л����豸״̬
		// Returns:   {long}	������
		//************************************
		long GetDeviceList();

		//************************************
		// Method:    {GetACDList}	��ȡ���� ACD Agent 
		// @Parameter: 
		// Returns:   {long}	������
		//************************************
		long GetACDList();

		//************************************
		// Method:    {GetAgentList}	��ȡ���� Agent ��״̬
		// @Parameter: {const std::string & p_strACDGrp}	ACD ��ţ�Ϊ��������е� ACD ��
		// Returns:   {long}	������
		//************************************
		long GetAgentList(const std::string & p_strACDGrp);

		//************************************
		// Method:    {GetCallList}	��ȡ���е�ǰ����
		// Returns:   {long}	������
		//************************************
		long GetCallList();

		//************************************
		// Method:    GetReadyAgent	��ȡ ACD ��һ�����е� Agent
		// FullName:  ACS::ISwitchClient::GetReadyAgent
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string & p_strACDGrp	ACD ��
		//************************************
		long GetReadyAgent(const std::string& p_strACDGrp);

	private:
		static boost::shared_ptr<CTaskManager> m_pInstance;
		
		Log::ILogPtr	m_pLog;

		static long		m_slTaskId;

		std::mutex		m_cmdTaskLock;
		std::mutex		m_excuteTaskLock;
		std::mutex		m_switchEventTaskLock;

		//ά�����ڵȴ�ִ�е�����
		std::map<long, boost::shared_ptr<CTask>>		m_cmdTaskQueue;
		//ά���Ѿ�ִ�е���������
		std::map<long, boost::shared_ptr<CTask>>		m_excutedTaskQueue;
		// Switch �������͵��¼��򷵻ص�ִ�н��
		std::map<long, boost::shared_ptr<CTask>>		m_switchEventTaskQueue;
	};
}	// end namespace

