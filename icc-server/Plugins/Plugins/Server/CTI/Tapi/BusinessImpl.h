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
		*  @brief    :  UDP服务回调函数,处理所有收到的消息
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
		// Method:    {LoadBlackList}	从数据库加载所有黑名单
		// Returns:   {void}	
		//************************************
		void LoadBlackList();

		//	双机服务控制
		void StartSwitchService();
		void StopSwitchServeice();

	public:
		// =========== 交换机主动推送的事件 ===================

		//	CTI 与交换机网络链接状态事件
		void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
		//	Agent 状态事件
		void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
		//	话务状态事件
		void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
		//	话务结束事件
		void Event_CallOverEx(ICallOverNotifPtr p_pSwitchNotif);
		//	设备状态事件
		void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif);
		//	会议成员挂机事件
		void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif);
		//	话务拒接事件
		void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
		//	失败事件
		void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);

		// ============== 具体命令执行结果反馈( CR-- Command Result)============
		//	登录应答
		void CR_AgentLogin(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	退出应答
		void CR_AgentLogout(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	置闲、置忙应答
		void CR_SetAgentState(ISetAgentStateResultNotifPtr p_pSwitchNotif);
		//	单呼应答
		void CR_MakeCall(IMakeCallResultNotifPtr p_pSwitchNotif);
		//	接听应答
		void CR_AnswerCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	拒接应答
		void CR_RefuseAnswer(ISwitchResultNotifPtr p_pSwitchNotif);
		//	挂机应答
		void CR_Hangup(ISwitchResultNotifPtr p_pSwitchNotif);
		//	强拆整个话务应答
		void CR_ClearCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	监听应答
		void CR_ListenCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	代答应答
		void CR_PickupCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	强插应答
		void CR_BargeInCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	强拆应答
		void CR_ForcePopCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	咨询应答
		void CR_ConsultationCall(IConsultationCallResultNotifPtr p_pSwitchNotif);
		//	转移应答
		void CR_TransferCall(ITransferCallResultNotifPtr p_pSwitchNotif);
		//	偏转应答
		void CR_DeflectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	保留应答
		void CR_HoldCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	重拾应答
		void CR_RetrieveCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	重连应答
		void CR_ReconnectCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	会议应答
		void CR_ConferenceCall(ISwitchResultNotifPtr p_pSwitchNotif);
		//	添加会议成员应答
	//	void CR_AddConferenceParty(IAddConferencePartyNotifPtr p_pSwitchNotif);
		//	删除会议成员应答
	//	void CR_DeleteConferenceParty(IDeleteConferencePartyNotifPtr p_pSwitchNotif);

		//	获取 CTI 与交换机网络链接状态应答
		void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
		//	获取所有设备状态应答
		void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
		//	获取所有 Agent 成员状态应答
		void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
		//	获取所有当前话务应答
		void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
		//	获取空闲 Agent 应答
		void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);

		//////////////////////////////////////////////////////////////////////////
		//	双机服务角色控制
		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	加载所有黑名单应答(DBProcess -> CTI)
		void OnNotifiLoadAllBlackListRespond(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	加载所有黑名单同步(DBProcess -> CTI)
		void OnNotifiLoadAllBlackListSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// ============== 应用层操作交换机的请求 ===========================
		void OnNotifiClientRegisterSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	登录请求
		void OnNotifiAgentLoginRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	退出请求
		void OnNotifiAgentLogoutRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	置闲、置忙请求
		void OnNotifiSetAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	单呼请求
		void OnNotifiMakeCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	挂机请求
		void OnNotifiHangupRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	应答请求
		void OnNotifiAnswerCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	拒接请求
		void OnNotifiRefuseAnswerRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	强拆整个话务请求
		void OnNotifiClearCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	监听请求
		void OnNotifiListenCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	代答请求
		void OnNotifiPickupCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	强插请求
		void OnNotifiBargeInCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	强拆请求
		void OnNotifiForcePopCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	咨询请求
		void OnNotifiConsultationCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	咨询请求
		void OnNotifiConsultationCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	转移请求
		void OnNotifiTransferCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	转警请求
		void OnNotifiTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	取消转警请求
		void OnNotifiCancelTransferCallExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	偏转请求
		void OnNotifiDeflectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	保留请求
		void OnNotifiHoldCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	重拾请求
		void OnNotifiRetrieveCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	重连话务请求
		void OnNotifiReconnectCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	合并会议请求
		void OnConferenceCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	新建会议请求
		void OnMakeConferenceRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	添加会议成员请求
		void OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	获取会议成员请求
		void OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		void OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	删除会议成员请求
		void OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// ============== 应用层获取交换机相关状态的请求 ======================
		//	获取 CTI 与交换机网络链接状态请求
		void OnNotifiGetCTIConnStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有设备状态请求
		void OnNotifiGetDeviceListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有 ACD Agent 请求
		void OnNotifiGetACDListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有 Agent 状态请求
		void OnNotifiGetAgentListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取当前话务请求
		void OnNotifiGetCallListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	话务查询请求
		//void OnNotifiQueryCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	黑名单查询请求
		//void OnNotifiQueryBlackCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest); 
		//	获取空闲坐席请求
		void OnNotifiGetReadyAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//================ 黑名单操作 =================================
		//	添加、修改一个黑名单请求
		void OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除一个黑名单请求
		void OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除所有请求
		void OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有黑名单请求
		void OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//================	话务操作	================================
		//	移除早释话务请求
	//	void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//////////////////////////////////////////////////////////////////////////
		void OnInitOverTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// 黑名单管理定时器
		void OnBlackListManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// 话务管理定时器
		void OnCallManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// 会议管理定时器
		void OnConferenceManagerTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		// 转警管理定时器
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
		std::string							m_strServiceID;				//服务ID

		DataBase::IDBConnPtr		        m_pDBConnPtr;
		HelpTool::IHelpToolPtr              m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;
		CTITestUdpHelperPtr					m_UdpHelper;
	};
}