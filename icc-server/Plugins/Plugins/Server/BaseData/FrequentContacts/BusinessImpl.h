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
        //增加常用联系人
		void OnCNotifiAddRequest(ObserverPattern::INotificationPtr p_pNotify);		

        //查询常用联系人
		void OnCNotifiQueryRequest(ObserverPattern::INotificationPtr p_pNotify);		

		//删除常用联系人
		void OnNotifiDeleteRequest(ObserverPattern::INotificationPtr p_pNotify);			

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq);

	private:
					


	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr		m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		//Redis::IRedisClientPtr m_pRedisClient;
	};
}