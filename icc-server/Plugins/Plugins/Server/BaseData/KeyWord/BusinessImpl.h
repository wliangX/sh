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
        //设置关键字信息
		void OnCNotifiSetKeyWordRequest(ObserverPattern::INotificationPtr p_pNotify);		

        //获取所有关键字信息
		void OnCNotifiQueryKeyWordByPageRequest(ObserverPattern::INotificationPtr p_pNotify);		

		//删除关键字信息
		void OnNotifiDeleteKeyWordRequest(ObserverPattern::INotificationPtr p_pNotify);			

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq);

	private:
		void _SendSyncKeyWord(const ICC::PROTOCOL::CKeyWordInfo &in_oKeyWordInfo, int in_iSynType);

		bool _QueryKeyWordCount(int& iCount);
		bool _QueryKeyWordByPage(std::vector<ICC::PROTOCOL::CKeyWordInfo>& vecAllKeyWords, int iPageSize, int iBeginIndex);
		bool _QueryAllKeyWord(std::vector<ICC::PROTOCOL::CKeyWordInfo>& vecAllKeyWords);
		/*bool _LoadKeyWordFromDB();
		bool _AddKeyWord(const std::string& strguid, std::string& strkeyword);
		bool _DeleteKeyWord(const std::string& strguid);	*/		


	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		//Redis::IRedisClientPtr m_pRedisClient;
	};
}