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
		//获取所有单位下的所有可调派单位信息
		void OnNotifiGetAllCanAdminDept(ObserverPattern::INotificationPtr p_pNotify);
		//设置某一单位下某一个可调派单位信息
		void OnNotifiSetAdmin4anDept(ObserverPattern::INotificationPtr p_pNotify);
		//删除某一单位下某一个可调派单位信息
		void OnNotifiDelDeptCanAdmin(ObserverPattern::INotificationPtr p_pNotify);
		//获取某一单位下的所有可调派单位信息
		void OnNotifiGetCanAdminDept(ObserverPattern::INotificationPtr p_pNotify);
    public:        
		//加载可调派单位数据
		bool LoadAdminInfo();
		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
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