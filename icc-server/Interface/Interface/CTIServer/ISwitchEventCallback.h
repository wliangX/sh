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
		// Method:    {Event_CTIConnState}	CTI �뽻������������״̬�¼�֪ͨ
		// Parameter: {ICTIConnStateNotifPtr p_pSwitchNotif}	AES��������״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_AgentState}	Agent ״̬��Login/Logout��Ready/NotReady���¼�֪ͨ
		// Parameter: {IAgentStateNotifPtr p_pSwitchNotif}	Agent ״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_CallState}	����״̬��Incoming�����룩��Waiting���Ŷӣ���Assign�����䣩��Release�����ͣ����¼�֪ͨ
		// Parameter: {ICallStateNotifPtr p_pSwitchNotif}	����״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_CallState(ICallStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_CallOver}	һ���������
		// Parameter: {ICallOverNotifPtr p_pSwitchNotif}	��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_DeviceState}	�����豸״̬�����塢���塢ͨ��...���¼�֪ͨ
		// Parameter: {IDeviceStateNotifPtr p_pSwitchNotif}	����״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_ConferenceeHangup}	�����Ա�һ��¼�֪ͨ
		// Parameter: {IDeviceStateNotifPtr p_pSwitchNotif}	����״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {Event_RefuseCall}	�ܽӻ����¼�֪ͨ
		// Parameter: {IRefuseCallNotifPtr p_pSwitchNotif}	�ܽӻ���ṹ��
		// Returns:   {void}	
		//************************************
		virtual void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    Event_Failed	ʧ���¼�֪ͨ
		// FullName:  ACS::ISwitchEventCallback::Event_Failed
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IFailedEventNotifPtr p_pSwitchNotif	ʧ���¼��ṹ��
		//************************************
		virtual void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif) = 0;

		virtual void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif) = 0;

		// --------- ��������ִ�н������( CR-- Command Result) ------------------

		//************************************
		// Method:    {CR_AgentLogin}	Login �������
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Login ����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AgentLogout}	Logout �������
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Logout ����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_SetAgentState}	���� Agent ״̬��Ready/NotReady���������
		// Parameter: {ISetAgentStateResultNotifPtr p_pSwitchNotif}	Agent ״̬��Ready/NotReady������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_MakeCall}	�����������
		// Parameter: {IMakeCallResultNotifPtr p_pSwitchNotif}	��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AnswerCall}	Ӧ��������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	Ӧ�����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_RefuseAnswer	�ܽӽ������
		// FullName:  ACS::ISwitchEventCallback::CR_RefuseAnswer
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif	�ܽӽ���ṹ��
		//************************************
		virtual void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_Hangup		�һ��������
		// FullName:  ACS::ISwitchEventCallback::CR_Hangup
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif
		//************************************
		virtual void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_ClearCall	�Ҷ���������������
		// FullName:  ICC::ISwitchEventCallback::CR_ClearCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ISwitchResultNotifPtr p_pSwitchNotif
		//************************************
		virtual void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ListenCall}	�����������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_PickupCall}	����������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	�������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_BargeInCall} ǿ��������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	ǿ�����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ForcePopCall}	ǿ��������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif} ǿ�����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_ConsultationCall	��ѯ�������
		// FullName:  ACS::ISwitchEventCallback::CR_ConsultationCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IConsultationCallResultNotifPtr p_pResultNotif	��ѯ����ṹ��
		//************************************
		virtual void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pResultNotif) = 0;

		//************************************
		// Method:    CR_TransferCall	ת�ƽ������
		// FullName:  ACS::ISwitchEventCallback::CR_TransferCall
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: ITransferCallResultNotifPtr p_pSwitchNotif	ת�ƽ���ṹ��
		//************************************
		virtual void CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_DeflectCall}	ƫת�������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	ƫת����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_HoldCall} �����������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_RetrieveCall} ��ʰ�������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	��ʰ����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ReconnectCall}	��ʰ�������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif} ��ʰ����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_ConferenceCall} �ϲ�����������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	�ϲ��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_TakeOverCall} �ӹܻ���������
		// Parameter: {ISwitchResultNotifPtr p_pSwitchNotif}	�ϲ��������ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_AddConferenceParty} ��ӻ����Ա�������
		// Parameter: {IAddConferencePartyNotifPtr p_pSwitchNotif}	��ӻ����Ա����ṹ��
		// Returns:   {void}	
		//************************************
//		virtual void CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_DeleteConferenceParty}	 ɾ�������Ա�������
		// Parameter: {IDeleteConferencePartyNotifPtr p_pSwitchNotif} ɾ�������Ա����ṹ��
		// Returns:   {void}	
		//************************************
//		virtual void CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetCTIConnState}	��ȡ CTI ����״̬�������
		// Parameter: {IGetCTIConnStateResultNotifPtr p_pSwitchNotif}	AES ��������״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetDeviceList}	��ȡ�����豸״̬�������	
		// Parameter: {IGetDeviceListResultNotifPtr p_pSwitchNotif}	����״̬����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetACDList}	��ȡ ACD Agent �������
		// Parameter: {IGetACDListResultNotifPtr p_pSwitchNotif}	ACD Agent �ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetAgentList}	��ȡ Agent ״̬��Login/Logout��Ready/NotReady���������
		// Parameter: {IGetAgentListResultNotifPtr p_pSwitchNotif}	Agent ״̬�ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    {CR_GetCallList}	��ȡ��ǰ����������
		// Parameter: {IGetCallListResultNotifPtr p_pSwitchNotif}	��ǰ����ṹ��
		// Returns:   {void}	
		//************************************
		virtual void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif) = 0;

		//************************************
		// Method:    CR_GetReadyAgent	��ȡ���� ACD �� Agent ���
		// FullName:  ACS::ISwitchEventCallback::CR_GetReadyAgent
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IGetReadyAgentResultNotifPtr p_pSwitchNotif	���� ACD �� Agent �ṹ��
		//************************************
		virtual void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif) = 0;

		//��ȡ������ϯ
		virtual void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif) = 0;

		//FS aes�¼���չ�ӿ�
		virtual void CR_FSAesEventEx(const std::string &p_strCmdName,ISwitchNotifPtr p_pSwitchNotif) = 0;
	};

	typedef boost::shared_ptr<ISwitchEventCallback>	ISwitchEventCallbackPtr;

} /*namespace ICC*/
