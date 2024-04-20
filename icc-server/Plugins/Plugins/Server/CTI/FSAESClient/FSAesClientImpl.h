#pragma once

namespace ICC
{
	class CFSAesClientImpl :
		public ISwitchClient
	{
	public:
		CFSAesClientImpl(IResourceManagerPtr p_pResourceManager);
		virtual ~CFSAesClientImpl();

	public:
		//************************************
		// Method:    {InitSwitch}	��ʼ�������� Client
		// Parameter: {ISwitchEventCallbackPtr p_pCallback}	�������¼��ص�����
		// Returns:   {void}	
		//************************************
		virtual void InitSwitch(ISwitchEventCallbackPtr p_pCallback);

		//************************************
		// Method:    {StartSwitch}	���������� Client
		// Returns:   {void}	
		//************************************
		virtual void StartSwitch();

		//************************************
		// Method:    {StopSwitch}	ֹͣ������ Client
		// Returns:   {void}	
		//************************************
		virtual void StopSwitch();

		//************************************
		// Method:    {DestroySwitch}	���ٽ����� Client
		// Returns:   {void}	
		//************************************
		virtual void DestroySwitch();

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//************************************
		// Method:    {AgentLogin}	Agent Login
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Returns:   {long}	������
		//************************************
		virtual long AgentLogin(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {AgentLogout}	Agent Logout
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Returns:   {long}	������
		//************************************
		virtual long AgentLogout(const std::string& p_strAgent, const std::string& p_strACD);

		//************************************
		// Method:    {SetAgentState}	���� Agent Ready/NotReady ״̬
		// Parameter: {const std::string & p_strAgent}	��ϯ��
		// Parameter: {const std::string & p_strACD}	ACD ��
		// Parameter: {const std::string & p_strReadyState}	Ready/NotReady
		// Returns:   {long}	������
		//************************************
		virtual long SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState);

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
		virtual long MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId);

		//************************************
		// Method:    {AnswerCall}	����Ӧ��
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID	
		// Parameter: {const std::string & p_strDevice}	����Ӧ������ϯ��
		// Returns:   {long}	������
		//************************************
		virtual long AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {RefuseAnswer}	�ܽ�
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID	
		// Parameter: {const std::string & p_strDevice}	����ܽӵ���ϯ��
		// Returns:   {long}	������
		//************************************
		virtual long RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice);

		//************************************
		// Method:    {Hangup}	�Ҷ�
		// Parameter: {const std::string& p_strCTICallRefId}	Ӧ�ò㻰��ID
		// Parameter: {const std::string & p_strSponsor}	����Ҷϵ���ϯ��
		// Parameter: {const std::string & p_strDevice}	���ҶϵĻ����Ա
		// Returns:   {long}	������
		//************************************
		virtual long Hangup(const std::string& p_strCTICallRefId, const std::string & p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ClearCall		�Ҷ���������
		// FullName:  ICC::SwitchClient::ISwitchClient::ClearCall
		// Access:    virtual public 
		// Qualifier:
		// Parameter: const std::string & p_strCTICallRefId	Ӧ�ò㻰��ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		// Parameter: {const std::string & p_strTarget}	��ǿ�����ϯ��
		// Returns:   long	������
		//************************************
		virtual long ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    {ListenCall}	����
		// Parameter: {const std::string& p_strCTICallRefId}	����������ID
		// Parameter: {const std::string & p_strSponsor}	�����������ϯ��
		// Parameter: {const std::string & p_strTarget}	����������ϯ��
		// Returns:   {long}	������
		//************************************
		virtual long ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    {PickupCall}	����
		// Parameter: {const std::string& p_strCTICallRefId}	������Ļ���ID
		// Parameter: {const std::string & p_strSponsor}	����������ϯ��
		// Parameter: {const std::string & p_strTarget}	���������ϯ��
		// Returns:   {long}	������
		//************************************
		virtual long PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    BargeInCall	ǿ��
		// FullName:  ACS::ISwitchClient::BargeInCall
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǿ��Ļ��� ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		// Parameter: const std::string & p_strTarget	��ǿ�����ϯ��
		//************************************
		virtual long BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ForcePopCall	ǿ��
		// FullName:  ACS::ISwitchClient::ForcePopCall
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǿ��Ļ��� ID
		// Parameter: const std::string & p_strSponsor	����ǿ�����ϯ��
		// Parameter: const std::string & p_strTarget	����ǿ�����ϯ��
		//************************************
		virtual long ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    ConsultationCall	��ѯ
		// FullName:  ACS::ISwitchClient::ConsultationCall
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ѯ����ID
		// Parameter: const std::string & p_strSponsor	������ѯ����ϯ��
		// Parameter: const std::string & p_strTarget	��ѯĿ�����
		// Parameter: const std::string & p_strTargetDeviceType	��ѯĿ����������
		// Parameter: bool p_bIsTransferCall	�Ƿ�Ϊת�Ʋ�������ѯ���ǣ�true����false
		//************************************
		virtual long ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall);

		//************************************
		// Method:    ConsultationCallEx	��ѯ
		// FullName:  ACS::ISwitchClient::TransferCall
		// Access:    virtual public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId		��ǰ���� ID
		// Parameter: const std::string & p_strSponsor	����ת�Ƶ���ϯ��
		// Parameter: const std::string & p_strTarget	ת�Ƶ�Ŀ�� ACD
		//************************************
		//virtual long ConsultationCallEx(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    TransferCall	ת��
		// FullName:  ACS::ISwitchClient::TransferCall
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strActiveCallRefId	��ǰ��Ļ���ID
		// Parameter: const std::string& p_strHeldCallRefId		�������Ļ���ID
		// Parameter: const std::string & p_strSponsor	����ת�Ƶ���ϯ��
		// Parameter: const std::string & p_strTarget	ת�Ƶ�Ŀ����ϯ��
		//************************************
		virtual long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    DeflectCall	ƫת
		// FullName:  ACS::CSwitchClientImpl::DeflectCall
		// Access:    virtual public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ƫת���� ID
		// Parameter: const std::string & p_strSponsor	����ƫת����ϯ��
		// Parameter: const std::string & p_strTarget	ƫתĿ����ϯ��
		//************************************
		virtual long DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//************************************
		// Method:    HoldCall	����
		// FullName:  ACS::CSwitchClientImpl::HoldCall
		// Access:    virtual public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	���������� ID
		// Parameter: const std::string & p_strSponsor	����������ϯ��
		// Parameter: const std::string & p_strDevice	�������ĺ���
		//************************************
		virtual long HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    RetrieveCall	��ʰ
		// FullName:  ACS::ISwitchClient::RetrieveCall
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	��ǰ���� ID
		// Parameter: const std::string & p_strSponsor	������ʰ����ϯ��
		// Parameter: const std::string & p_strDevice	��ʰ����ϯ��
		//************************************
		virtual long RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    ConferenceCall	ͨ���������
		// FullName:  ACS::ISwitchClient::ConferenceCall
		// Access:    virtual public 
		// Returns:   long	������	
		// Qualifier:
		// Parameter: const std::string & p_strTarget	�������ĳ�Ա
		// Parameter: const std::string& p_strHeldCallRefId		�������Ļ��� ID
		// Parameter: const std::string& p_strActiveCallRefId	��ǰ����� ID
		//************************************
		virtual long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId);

		//************************************
		// @Method:    ReconnectCall	����
		// @Parameter: {const std::string& p_strActiveCallRefId}	��ǰ���� ID
		// @Parameter: {const std::string& p_strHeldCallRefId}		�������� ID
		// @Parameter: {const std::string & p_strSponsor}	������������ϯ��
		// @Parameter: {const std::string & p_strDevice}	��������ϯ��
		// @Returns:   {long}	������
		//************************************
		virtual long ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice);

		//************************************
		// Method:    TakeOverCall	�ӹܻ���
		// FullName:  ACS::CSwitchClientImpl::TakeOverCall
		// Access:    virtual public 
		// Returns:   long
		// Qualifier:
		// Parameter: const std::string& p_strCTICallRefId	���ӹܻ��� ID
		// Parameter: const std::string & p_strSponsor	����ӹܵ���ϯ��
		// Parameter: const std::string & p_strTarget	���ӹܵ�Ŀ����ϯ��
		//************************************
		virtual long TakeOverCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// Method:    {GetCTIConnectState}	��ȡ CTI �� ��������������״̬
		// Returns:   {long}	������
		//************************************
		virtual long GetCTIConnectState();

		//************************************
		// Method:    {GetDeviceList}	��ȡ���л����豸״̬
		// Returns:   {long}	������
		//************************************
		virtual long GetDeviceList();

		//************************************
		// Method:    {GetACDList}	��ȡ���� ACD Agent
		// @Parameter: 
		// Returns:   {long}	������
		//************************************
		virtual long GetACDList();

		//************************************
		// Method:    {GetAgentList}	��ȡ���� Agent ��״̬
		// @Parameter: {const std::string & p_strACDGrp}	ACD ��ţ�Ϊ��������е� ACD ��
		// Returns:   {long}	������
		//************************************
		virtual long GetAgentList(const std::string & p_strACDGrp);

		//************************************
		// Method:    {GetCallList}	��ȡ���е�ǰ����
		// Returns:   {long}	������
		//************************************
		virtual long GetCallList();

		//************************************
		// Method:    GetReadyAgent	��ȡ ACD ��һ�����е� Agent
		// FullName:  ACS::ISwitchClient::GetReadyAgent
		// Access:    virtual public 
		// Returns:   long	������
		// Qualifier:
		// Parameter: const std::string & p_strACDGrp	ACD ��
		//************************************
		virtual long GetReadyAgent(const std::string& p_strACDGrp);

		///////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//************************************
		// Method:    {SetBlackList}	���ú�����
		// Parameter: {const std::string & p_strLimitNum}	���޺���
		// Parameter: {const std::string & p_strLimitType}	�������ͣ���ͷ��������...��
		// Returns:   {long}	������
		//************************************
		virtual long SetBlackList(const std::string& p_strLimitNum, const std::string& p_strLimitType = "");

		//************************************
		// Method:    {DeleteBlackList}	ɾ��������
		// Parameter: {const std::string & p_strLimitNum}	���޺���
		// Returns:   {long}	������
		//************************************
		virtual long DeleteBlackList(const std::string& p_strLimitNum);

		//************************************
		// Method:    {DeleteAllBlackList}	ɾ�����к�����
		// Returns:   {long}	
		//************************************
		virtual long DeleteAllBlackList();
		//////////////////////////////////////////////////////////////////////////

		virtual std::string GetDeviceState(const std::string& strTarget);
		virtual std::string GetReadyAgentEx(const std::string& strTarget);

		virtual std::string GetCallerID(const std::string& p_strInCTICallRefId, std::string& p_strCalled);
		virtual bool GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime);

		virtual long GetFreeAgentList(const std::string& p_strACDGrp, const std::string& p_strDeptCode);

		//Freeswitch Aes�첽������չ�ӿ�
		virtual long FSAesAsyncRequestEx(const std::string p_strCmdName, const std::string& p_strResquestData);

		virtual long CreateConference(const std::string& p_strCompere, std::string& p_strConferenceId);
		virtual std::string QueryConferenceByCallid(const std::string& p_strCallRefId,std::string &p_strCompere, std::string& p_strBargeInSponsor);
		virtual long AddConferencePartyEx(const std::string& p_strCallRefId, const std::string& p_strTarget, const std::string& p_strTargetType, std::string& p_strConferenceId);
		virtual long AddConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType);

		virtual long DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strTarget, const std::string& p_strTargetType);
		virtual long DeleteConferenceParty(const std::string& p_strConferenceId, std::string& p_strTarget, const std::string& p_strTargetType);
		virtual long GetConferenceParty(const std::string& p_strConferenceId, std::string& p_strPartyData);
	private:
		bool _CheckMinOnlineAgentNum(int p_nMinOnLineCount, const std::string& p_strAgent, std::string& p_strACDList);
	private:
		IResourceManagerPtr m_pResourceManager;
		Log::ILogPtr		m_pLog;
		StringUtil::IStringUtilPtr	m_pStringPtr;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
	};
}