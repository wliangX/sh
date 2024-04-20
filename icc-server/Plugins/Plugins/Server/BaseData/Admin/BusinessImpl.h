#pragma once


namespace ICC
{    
	typedef std::map<std::string, PROTOCOL::CGetAdminRespond::CAdmin> mapAdminInfo;
	//typedef std::map<std::string, PROTOCOL::CCanAdminDept::CAdmin> mapCanAdminDept;

	class CBusinessImpl :
        public CBusinessBase
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
		//��ȡ���е�λ�µ����пɵ��ɵ�λ��Ϣ
		void OnNotifiGetAllCanAdminDept(ObserverPattern::INotificationPtr p_pNotify);
		//����ĳһ��λ��ĳһ���ɵ��ɵ�λ��Ϣ
		void OnNotifiSetAdmin4anDept(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ��ĳһ��λ��ĳһ���ɵ��ɵ�λ��Ϣ
		void OnNotifiDelDeptCanAdmin(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡĳһ��λ�µ����пɵ��ɵ�λ��Ϣ
		void OnNotifiGetCanAdminDept(ObserverPattern::INotificationPtr p_pNotify);
    public:        
		//���ؿɵ��ɵ�λ����
		bool LoadAdminInfo();
		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
    private:
		DataBase::IDBConnPtr m_pDBConn;
		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;

		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		Redis::IRedisClientPtr m_pRedisClient;

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}