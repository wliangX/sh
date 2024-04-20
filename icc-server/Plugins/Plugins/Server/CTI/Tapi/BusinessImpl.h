#pragma once

namespace ICC
{
	class CBusinessImpl;

	class CTestCallInfo
	{
	public:
		long			m_lCSTACallRefId;
		std::string		m_strCTICallRefId;
		std::string		m_strCalled;
		std::string		m_strCaller;
		std::string		m_strAgentNo;
	};

	class CCTITestUdpHelper : public Udp::IUdpCallBack
	{
	public:
		CCTITestUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool, Udp::IUdpPtr pUdpServer, CBusinessImpl *pBusiness);
		~CCTITestUdpHelper();

		void OnStart();
		void OnStop();

		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_stringUtilPtr) { m_pStringUtilPtr = p_stringUtilPtr; }
		/********************************************************
		*  @function :  CUdpHelper::OnReceived
		*  @brief    :  UDP����ص�����,���������յ�����Ϣ
		*  @input    :  p_pData, p_iLength, p_strRemoteIP, p_iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:08
		********************************************************/
		virtual void OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort);

		void SetTestCallInfo(const string& strCaller, const string& strCalled, const string& strAgentNo, long nCallRefId);
		bool GetTestCallInfoByCaller(const string& strCaller, CTestCallInfo& pCallInfo);
		bool GetTestCallInfoByRefId(long nCallRefId, CTestCallInfo& pCallInfo);

		bool CTITestCallOut(const string& strCaller, const string& strCalled, string& strCallRefId);
	private:
		Log::ILogPtr                        m_pLog;
		HelpTool::IHelpToolPtr				m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;

		CBusinessImpl*						m_pBusiness;
		StringUtil::IStringUtilPtr			m_pStringUtilPtr;

		std::map<std::string, CTestCallInfo> m_mapCallInfo;

		int									m_nIntCallRefId;
	};
	typedef boost::shared_ptr<CCTITestUdpHelper> CTITestUdpHelperPtr;

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
		void SetComponentsPtr();

		//************************************
		// Method:    {LoadBlackList}	�����ݿ�������к�����
		// Returns:   {void}	
		//************************************
		void LoadBlackList();

		//	˫���������
		void StartSwitchService();
		void StopSwitchServeice();

	public:
		// =========== �������������͵��¼� ===================

		//	CTI �뽻������������״̬�¼�
		void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
		//	Agent ״̬�¼�
		void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
		//	����״̬�¼�
		void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
		//	��������¼�
		void Event_CallOverEx(ICallOverNotifPtr p_pSwitchNotif);
		//	�豸״̬�¼�
		void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif);
		//	�����Ա�һ��¼�
		void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif);
		//	����ܽ��¼�
		void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
		//	ʧ���¼�
		void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);

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

		//	��ȡ CTI �뽻������������״̬Ӧ��
		void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
		//	��ȡ�����豸״̬Ӧ��
		void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���� Agent ��Ա״̬Ӧ��
		void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���е�ǰ����Ӧ��
		void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
		//	��ȡ���� Agent Ӧ��
		void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);

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
		//	��ѯ����
		void OnNotifiConsultationCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
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
		//	�ϲ���������
		void OnConferenceCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	�½���������
		void OnMakeConferenceRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ӻ����Ա����
		void OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	��ȡ�����Ա����
		void OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		void OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	ɾ�������Ա����
		void OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

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

		//================ ���������� =================================
		//	��ӡ��޸�һ������������
		void OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ��һ������������
		void OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	ɾ����������
		void OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	��ȡ���к���������
		void OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//================	�������	================================
		//	�Ƴ����ͻ�������
	//	void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		void OnInitOverTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// ����������ʱ��
		void OnBlackListManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// �������ʱ��
		void OnCallManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// �������ʱ��
		void OnConferenceManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// ת������ʱ��
		void OnTransferCAllManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		void OnAgentHeartBeatTimeoutSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

	private:
		bool								m_bLoadPlugins;
		bool								m_bStartService;

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		StringUtil::IStringUtilPtr			m_pStringUtil;
		JsonParser::IJsonFactoryPtr			m_pJsonFactory;
		Redis::IRedisClientPtr				 m_pRedisClient;
		DateTime::IDateTimePtr				m_pDateTime;
		Config::IConfigPtr					m_pConfig;
		Config::IConfigPtr					m_pStaticConfig;
		Log::ILogPtr						m_pLog;
		Timer::ITimerManagerPtr				m_pTimerManager;
		std::string							m_strServiceID;				//����ID

		DataBase::IDBConnPtr		        m_pDBConnPtr;
		HelpTool::IHelpToolPtr              m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;
		CTITestUdpHelperPtr					m_UdpHelper;
	};
}