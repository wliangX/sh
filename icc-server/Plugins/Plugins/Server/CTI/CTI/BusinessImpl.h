#pragma once

namespace ICC
{
	struct CACDAlias
	{
		std::string m_strACD;		//ԭʼ ACD
		std::string m_strAlias;		//ACD ����
		std::string m_strDept;		//��������
	};

	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		CBusinessImpl();
		virtual ~CBusinessImpl();

		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:

		//************************************
		// Method:    LoadParams	��������
		// FullName:  ICC::CBusinessImpl::LoadParams
		// Access:    public 
		// Returns:   void
		// Qualifier:
		//************************************
		void LoadParams();

		//************************************
		// Method:    {LoadBlackList}	�����ݿ�������к�����
		// Returns:   {void}	
		//************************************
		void LoadBlackList();

		//************************************
		// Method:    {InitSwitch}	��ʼ���������ͻ���
		// Returns:   {void}	
		//************************************
		void InitSwitch();

		//************************************
		// Method:    {StartSwitch}	�����������ͻ���
		// Returns:   {void}	
		//************************************
		void StartSwitch();

		//************************************
		// Method:    {StopSwitch}	�رս������ͻ���
		// Returns:   {void}	
		//************************************
		void StopSwitch();

		//************************************
		// Method:    {DestroySwitch}	���ٽ������ͻ���
		// Returns:   {void}	
		//************************************
		void DestroySwitch();

		//	˫���������
		void StartService();
		void StopServeice();

	//	void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

	public:
		bool GetACDGrpConver(std::string& p_strDestAlias, std::string& p_strDestDept, const std::string& p_strSrcACDGrp);

		// =========== �������������͵��¼� ===================

		//	CTI �뽻������������״̬�¼�
		void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
		//	Agent ״̬�¼�
		void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
		//	����״̬�¼�
		void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
		//	��������¼�
		void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif);
		//	�豸״̬�¼�
		void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif);
		//	�����Ա�һ��¼�
		void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif);
		//	����ܽ��¼�
		void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
		//	ʧ���¼�
		void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);
		//  ������ת���¼�
		void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif);

		// ============== ��������ִ�н������( CR-- Command Result)============
		//	��¼Ӧ��
		void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	�˳�Ӧ��
		void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	���С���æӦ��
		void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	�ܽ�Ӧ��
		void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif);
		//	�һ�Ӧ��
		void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif);
		//	ǿ����������Ӧ��
		void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	ǿ��Ӧ��
		void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	ǿ��Ӧ��
		void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	��ѯӦ��
		void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pSwitchNotif);
		//	ת��Ӧ��
		void CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif);
		//	ƫתӦ��
		void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	��ʰӦ��
		void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	����Ӧ��
		void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	��ӻ����ԱӦ��
	//	void CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif);
		//	ɾ�������ԱӦ��
	//	void CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif);
		//	�ӹ�Ӧ��
		void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif);

		//	��ȡ CTI �뽻������������״̬Ӧ��
		void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
		//	��ȡ�����豸״̬Ӧ��
		void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���� ACD Agent ��ԱӦ��
		void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���� Agent ��Ա״̬Ӧ��
		void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���е�ǰ����Ӧ��
		void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���� Agent Ӧ��
		void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);

		void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif);

		void CR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_pSwitchNotif);
		//////////////////////////////////////////////////////////////////////////
		//	˫�������ɫ����
		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	�������к�����Ӧ��(DBProcess -> CTI)
		void OnNotifiLoadAllBlackListRespond(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�������к�����ͬ��(DBProcess -> CTI)
		void OnNotifiLoadAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// ============== Ӧ�ò���������������� ===========================
		void OnNotifiClientRegisterSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��¼����
		void OnNotifiAgentLoginRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�˳�����
		void OnNotifiAgentLogoutRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	���С���æ����
		void OnNotifiSetAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��������
		void OnNotifiMakeCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�һ�����
		void OnNotifiHangupRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	Ӧ������
		void OnNotifiAnswerCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�ܽ�����
		void OnNotifiRefuseAnswerRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ǿ��������������
		void OnNotifiClearCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��������
		void OnNotifiListenCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��������
		void OnNotifiPickupCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ǿ������
		void OnNotifiBargeInCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ǿ������
		void OnNotifiForcePopCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ѯ����
		void OnNotifiConsultationCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ת������
		void OnNotifiTransferCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ת������
		void OnNotifiTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ȡ��ת������
		void OnNotifiCancelTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ƫת����
		void OnNotifiDeflectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��������
		void OnNotifiHoldCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ʰ����
		void OnNotifiRetrieveCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	������������
		void OnNotifiReconnectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�½���������
		void OnMakeConferenceRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnMakeConferenceExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��ӻ����Ա����
		void OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��ȡ�����Ա����
		void OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	ɾ�������Ա����
		void OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�ӹܻ�������
		void OnTakeOverCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// ============== Ӧ�ò��ȡ���������״̬������ ======================
		//	��ȡ CTI �뽻������������״̬����
		void OnNotifiGetCTIConnStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ�����豸״̬����
		void OnNotifiGetDeviceListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ���� ACD Agent ����
		void OnNotifiGetACDListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ���� Agent ״̬����
		void OnNotifiGetAgentListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ��ǰ��������
		void OnNotifiGetCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�����ѯ����
		//void OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��������ѯ����
		//void OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest); 
		//	��ȡ������ϯ����
		void OnNotifiGetReadyAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifiGetFreeAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//================ ���������� =================================
		//	��ӡ��޸�һ������������
		void OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ��һ������������
		void OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ����������
		void OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ���к���������
		void OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��ȡϵͳ��ǰʱ��
		void OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifiGetMinOnlineAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//��ϯ������ʱͬ��
		void OnAgentHeartBeatTimeoutSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//================	�������	================================
		//	�Ƴ����ͻ�������
	//	void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void ReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//��ȡ��ʷ��������
		void OnGetHistoryCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnReceiveCallOverRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//////////////////////////////////////////////////////////////////////////
	private:
		bool GetMergeHuntGrpConver(std::string& p_strDestAcd, const std::string& p_strSrcNum);
		void _LoadMergeHuntGrpConver();
		void _AssignAcdToCallNotify(ICallStateNotifPtr pCallOverNotify);		
		std::map<std::string, std::string> m_mapMergeHuntGrpConvers;
		std::string                         m_strEnableSkillMerge;

	private:
		bool								m_bLoadPlugins;
		bool								m_bStartService;
		SwitchClient::ISwitchClientPtr		m_SwitchClientPtr;

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		StringUtil::IStringUtilPtr			m_StringUtilPtr;
		JsonParser::IJsonFactoryPtr			m_JsonFactoryPtr;
		DateTime::IDateTimePtr				m_DateTimePtr;
		Config::IConfigPtr					m_ConfigPtr;
		Config::IConfigPtr					m_pStaticConfig;
		Log::ILogPtr						m_LogPtr;
		DataBase::IDBConnPtr		        m_pDBConnPtr;
		HelpTool::IHelpToolPtr				m_pHelpTool;
	//	Lock::ILockPtr						m_dbProcessMutex;
	//	Timer::ITimerManagerPtr				m_TimerManagerPtr;

		int									m_nTransferRingTimeout;		//	���峬ʱʱ������λ���룩
		int									m_nConferenceRingTimeout;	//	���峬ʱʱ������λ���룩
		std::map<std::string, CACDAlias>	m_mapACDGrpConversion;		//	ACD ת���б�
		std::string							m_strServiceID;				//����ID

		std::string							m_strMinOnlineAgentNum;

		Redis::IRedisClientPtr				m_pRedisClient;

	};
}