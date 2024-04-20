#pragma once


namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase, public CCommonTimerCore
    {
    public:
        CBusinessImpl();
        ~CBusinessImpl();

	public:
		
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
        // 根据消息同步坐席号数据信息
		void OnNotifiSeatDataSync(ObserverPattern::INotificationPtr p_pNotify);
		// 坐席号话机状态数据同步
		void OnNotifiSeatDeviceStateSync(ObserverPattern::INotificationPtr p_pNotify);
		// 坐席号登陆状态数据同步
		void OnNotifiSeatLoginStateSync(ObserverPattern::INotificationPtr p_pNotify);
		// 坐席号今日警情数据同步
		void OnNotifiAlarmCountSync(ObserverPattern::INotificationPtr p_pNotify);
		//获取坐席数据信息
		void OnCNotifGetSeatInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 同步坐席信息管理
		void OnNotifiSeatInfoManagerSync(ObserverPattern::INotificationPtr p_pNotify);
		// 同步nacos 服务的IP
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotify);
		// CTI 连接更新数据
		void OnNotifiCtiConnectStateSync(ObserverPattern::INotificationPtr p_pNotify);

	private:
		// 发送给前端同步消息
		void SendSeatDataInfoSync(PROTOCOL::CSeatDataInfo SeatData);
		// 发送给前端同步消息
		void SyncSeatDataInfo(PROTOCOL::CSeatDataInfo SeatData, PROTOCOL::CSeatInfoManagerSync l_oSyncObj);
		// 修改 坐席号的警情数量
		bool SetSeatAlarmCountBySeatNo(const std::string strSeatNo, const std::string strAlarmCount);

		bool SetStaffAlarmCountByStaff(const std::string strStaffCode, const std::string strAlarmCount);

		//查所有的坐席地址
		bool _QueryTodayAlarmCount(std::vector<ICC::PROTOCOL::CSeatDataInfo>& vecAllSeatAddress);
		// 加载数据
		bool _LoadSeatDataFromDB();
		// 加载凌晨整点刷新的定时器
		void _LoadSeatDataToTimer();
		// 获取今天凌晨整点时间
		std::string GetYestertodayTime();
		// 统计今天凌晨整点16点到今天16点的接警数量，并更新redis
		void BuildAlarmCountByConfig();
		// 构建Redis数据
		void BuildSeatDataFromClientRegister(PROTOCOL::CClientRegisterSync l_oSyncObj, ICC::PROTOCOL::CSeatDataInfo& SeatData);
		// 初始化设备状态信息
		bool InitClientDevicesStateInfo();
		// 初始化 客户端注册信息
		bool InitClientRegisterInfo();
		// 班长注册信息
		bool InitSeatInfoFromRedis();
		// 初始化今日接警数量
		bool InitSeatAlarmCount();
		// 初始化设备状态
		bool InitSeatDeviceState(std::string& strSeatInfoReceive);
		// 初始化席位状态和设备状态
		bool InitSeatAgentState(std::string& strSeatInfoReceive);
		// 获取nacos服务器
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);
		// 检测设备状态
		void CheckDeviceState(std::string& strSeatDeptCode);
		// 获取 nacos params
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		// 从数据库中获取中文字符
		void GetSeatParamsFromConfig(const std::string strParamsKey, std::string& strParamsValue);
		// 过滤部门信息数据
		bool CheckDeptCodeData(const std::string strParamsKey, PROTOCOL::CSeatDataInfo data);

		bool _QueryStaffTodayAlarmCount(std::vector<ICC::PROTOCOL::CStaffDataInfo::CStaffInfo>& tmp_vecStaffCount);

	public:
		//定时任务内容
		virtual void OnTimer(std::string uMsgId);
	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		Redis::IRedisClientPtr m_pRedisClient;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		CCommonTimer				m_pTimer;
		IHttpClientPtr m_pHttpClient;
		std::string m_strAlarmCountConfig;
		std::string m_strCheckDeviceState;
		std::string m_strMinFreeCount;
		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex m_mutexNacosParams;

		std::string m_strCodeMode;                                   //警员号码模式：1、警员身份证号码 2、警员编号
	};
}