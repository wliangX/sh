#pragma once


namespace ICC
{
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
        //增加重大警情
		void OnCNotifiMajorAlarmConfigAddRequest(ObserverPattern::INotificationPtr p_pNotify);		

        //查询重大警情
		void OnCNotifiMajorAlarmConfigQueryRequest(ObserverPattern::INotificationPtr p_pNotify);

		//删除重大警情
		void OnNotifiMajorAlarmConfigDeleteRequest(ObserverPattern::INotificationPtr p_pNotify);

		//推送消息至消息中心部分
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq);

		//推送消息至消息中心
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);

	private:
		void _SyncUpdate(PROTOCOL::CMajorAlarmConfigSync::CBody& data);


	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr		m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		MsgCenter::IMessageCenterPtr m_pMsgCenter;
		//Redis::IRedisClientPtr m_pRedisClient;

		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex     m_mutexNacosParams;
	};
}