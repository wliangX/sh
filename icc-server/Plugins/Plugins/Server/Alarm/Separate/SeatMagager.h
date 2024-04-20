#pragma once

namespace ICC
{
	//ϯλ����
	class CSeatManager
	{
	public:
		//��ȡ����ϯλ����Ϣ
		bool GetOnlineSeat(std::string p_strSeatNo, Separate::SeatInfo & p_SeatInfo);
		//��ȡһ���͵�ǰ�Ӿ�ϯͬһ���ſ��еĴ���ϯ
		std::string GetFreeProcessSeat(std::string p_strDeptCode);
		//ϯλ���һ�������ݴ浥
		bool AddCache(std::string p_strSeatNo, std::string p_strAlarmID);
		//ϯλɾ��һ�������ݴ浥
		bool DeleteCache(std::string p_strSeatNo, std::string p_strAlarmID);
		//��ȡϯλ�ݴ浥
		bool GetCacheList(std::string p_strSeatNo, std::list<std::string>& p_listCache);
		//��ȡϯλ�ݴ浥����
		unsigned int GetCacheCount(std::string p_strSeatNo);

		//��ӹ�������
		void AddPubCache(std::string p_strDeptCode, std::string p_strAlarmID);
		//ɾ����������
		void DelPubCache(std::string p_strDeptCode, std::string p_strAlarmID);
		//��ȡָ����λ�µĹ�������
		bool GetPubCache(std::string p_strDeptCode, PubAlarmList & p_pubList);
		//��ȡ���й�������
		bool GetAllPubCache(PubAlarmList & p_pubList);
		//��ȡ������λ����
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
		bool m_bIsInit; //�Ƿ��ѳ�ʼ��
	};
}