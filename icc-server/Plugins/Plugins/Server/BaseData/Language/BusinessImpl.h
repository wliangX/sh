#pragma once

//消息分包数

#define SET_LANGUAGE_RESPOND "set_language_respond"
#define GET_LANGUAGE_RESPOND "get_language_respond"
#define DELETE_LANGUAGE_RESPOND "delete_language_respond"
#define LANGUAGE_SYNC "language_sync"
#define SYNC_TYPE_ADD "1"
#define SYNC_TYPE_CHANGE "2"
#define SYNC_TYPE_DELETE "3"
#define LANGUAGE_KEY  "LanguageInfoKey"
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
        bool LoadLanguage();
        
        //获取语言信息
		void OnCNotifiGetLanguageRequest(ObserverPattern::INotificationPtr p_pNotify);

        //设置语言信息
		void OnCNotifiSetLanguageRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnCNotifiDeleteLanguageRequest(ObserverPattern::INotificationPtr p_pNotify);

        void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
        bool SyncLanguage(const CLanguageInfo &p_oLanguage, std::string p_strSyncType);
        bool SetRedisLanguageInfo(CLanguageInfo& p_oLanguageInfo);
	private:
		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
        Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
        Redis::IRedisClientPtr	m_pRedisClient;		        

    private:
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
        Lock::ILockFactoryPtr m_LockFacPtr;
	};
}