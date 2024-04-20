#pragma once

namespace ICC
{
	struct CACDAlias
	{
		std::string m_strACD;		//原始 ACD
		std::string m_strAlias;		//ACD 别名
		std::string m_strDept;		//所属机构
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
		// Method:    LoadParams	参数加载
		// FullName:  ICC::CBusinessImpl::LoadParams
		// Access:    public 
		// Returns:   void
		// Qualifier:
		//************************************
		void LoadParams();

		//************************************
		// Method:    {LoadBlackList}	从数据库加载所有黑名单
		// Returns:   {void}	
		//************************************
		void LoadBlackList();

		//************************************
		// Method:    {InitSwitch}	初始化交换机客户端
		// Returns:   {void}	
		//************************************
		void InitSwitch();

		//************************************
		// Method:    {StartSwitch}	启动交换机客户端
		// Returns:   {void}	
		//************************************
		void StartSwitch();

		//************************************
		// Method:    {StopSwitch}	关闭交换机客户端
		// Returns:   {void}	
		//************************************
		void StopSwitch();

		//************************************
		// Method:    {DestroySwitch}	销毁交换机客户端
		// Returns:   {void}	
		//************************************
		void DestroySwitch();

		//	双机服务控制
		void StartService();
		void StopServeice();

	//	void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

	public:
		bool GetACDGrpConver(std::string& p_strDestAlias, std::string& p_strDestDept, const std::string& p_strSrcACDGrp);

		// =========== 交换机主动推送的事件 ===================

		//	CTI 与交换机网络链接状态事件
		void Event_CTIConnState(ICTIConnStateNotifPtr p_pSwitchNotif);
		//	Agent 状态事件
		void Event_AgentState(IAgentStateNotifPtr p_pSwitchNotif);
		//	话务状态事件
		void Event_CallState(ICallStateNotifPtr p_pSwitchNotif);
		//	话务结束事件
		void Event_CallOver(ICallOverNotifPtr p_pSwitchNotif);
		//	设备状态事件
		void Event_DeviceState(IDeviceStateNotifPtr p_pSwitchNotif);
		//	会议成员挂机事件
		void Event_ConferenceHangup(IDeviceStateNotifPtr p_pSwitchNotif);
		//	话务拒接事件
		void Event_RefuseCall(IRefuseCallNotifPtr p_pSwitchNotif);
		//	失败事件
		void Event_Failed(IFailedEventNotifPtr p_pSwitchNotif);
		//  黑名单转移事件
		void Event_BlackTransfer(IBlackTransferEventNotifPtr p_pSwitchNotif);

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
		//	接管应答
		void CR_TakeOverCall(ITakeOverCallResultNotifPtr p_pSwitchNotif);

		//	获取 CTI 与交换机网络链接状态应答
		void CR_GetCTIConnState(IGetCTIConnStateResultNotifPtr p_pSwitchNotif);
		//	获取所有设备状态应答
		void CR_GetDeviceList(IGetDeviceListResultNotifPtr p_pSwitchNotif);
		//	获取所有 ACD Agent 成员应答
		void CR_GetACDList(IGetACDListResultNotifPtr p_pSwitchNotif);
		//	获取所有 Agent 成员状态应答
		void CR_GetAgentList(IGetAgentListResultNotifPtr p_pSwitchNotif);
		//	获取所有当前话务应答
		void CR_GetCallList(IGetCallListResultNotifPtr p_pSwitchNotif);
		//	获取空闲 Agent 应答
		void CR_GetReadyAgent(IGetReadyAgentResultNotifPtr p_pSwitchNotif);

		void CR_GetFreeAgentList(IGetFreeAgentResultNotifPtr p_pSwitchNotif);

		void CR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_pSwitchNotif);
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
		//	新建会议请求
		void OnMakeConferenceRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnMakeConferenceExRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	添加会议成员请求
		void OnAddConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	获取会议成员请求
		void OnGetConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnQueryConferenceByCallidRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnDisenableConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	删除会议成员请求
		void OnDeleteConferencePartyRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	接管话务请求
		void OnTakeOverCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

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

		void OnNotifiGetFreeAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//================ 黑名单操作 =================================
		//	添加、修改一个黑名单请求
		void OnNotifiSetBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除一个黑名单请求
		void OnNotifiDeleteBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	删除所有请求
		void OnNotifiDeleteAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//	获取所有黑名单请求
		void OnNotifiGetAllBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//	获取系统当前时间
		void OnNotifiGetCurrentTimeRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifiGetMinOnlineAgentRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//坐席心跳超时同步
		void OnAgentHeartBeatTimeoutSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//================	话务操作	================================
		//	移除早释话务请求
	//	void OnNotifiRemoveReleaseCallRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void ReceiveRecordFileUpRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//获取历史话务请求
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

		int									m_nTransferRingTimeout;		//	振铃超时时长（单位：秒）
		int									m_nConferenceRingTimeout;	//	振铃超时时长（单位：秒）
		std::map<std::string, CACDAlias>	m_mapACDGrpConversion;		//	ACD 转换列表
		std::string							m_strServiceID;				//服务ID

		std::string							m_strMinOnlineAgentNum;

		Redis::IRedisClientPtr				m_pRedisClient;

	};
}