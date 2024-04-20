#pragma once

namespace ICC
{
	//席位管理
	class CSeatManager
	{
	public:
		//获取在线席位的信息
		bool GetOnlineSeat(std::string p_strSeatNo, Separate::SeatInfo & p_SeatInfo);
		//获取一个和当前接警席同一部门空闲的处警席
		std::string GetFreeProcessSeat(std::string p_strDeptCode);
		//席位添加一条处警暂存单
		bool AddCache(std::string p_strSeatNo, std::string p_strAlarmID);
		//席位删除一条处警暂存单
		bool DeleteCache(std::string p_strSeatNo, std::string p_strAlarmID);
		//获取席位暂存单
		bool GetCacheList(std::string p_strSeatNo, std::list<std::string>& p_listCache);
		//获取席位暂存单数量
		unsigned int GetCacheCount(std::string p_strSeatNo);

		//添加公共警单
		void AddPubCache(std::string p_strDeptCode, std::string p_strAlarmID);
		//删除公共警单
		void DelPubCache(std::string p_strDeptCode, std::string p_strAlarmID);
		//获取指定单位下的公共警单
		bool GetPubCache(std::string p_strDeptCode, PubAlarmList & p_pubList);
		//获取所有公共警单
		bool GetAllPubCache(PubAlarmList & p_pubList);
		//获取处警单位代码
		std::string GetPrcoessDeptCode(std::string p_strAlarmID, std::string p_strProcessID);

	public:
		IResourceManagerPtr GetResourceManager();
		static CSeatManager* Instance();
		bool Init(IResourceManagerPtr p_pResourceManager);
	public:
		static CSeatManager* sm_instance;
		static boost::mutex sm_mutexInstance;

	private:
		void OnTimer(ObserverPattern::INotificationPtr p_pNotify);

	private:
		CSeatManager();
		~CSeatManager();
	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		StringUtil::IStringUtilPtr m_pString;
		HelpTool::IHelpToolPtr m_pHelpTool;
		Redis::IRedisClientPtr m_pRedisClient;
		DateTime::IDateTimePtr m_pDateTime;
		Timer::ITimerManagerPtr m_pTimerMgr;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Lock::ILockFactoryPtr m_pLockFty;		
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		DataBase::IDBConnPtr m_pDBConn;
		IResourceManagerPtr m_pResourceManager;
	private:
		bool m_bIsInit; //是否已初始化
	};
}