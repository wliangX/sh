#pragma once

#define SET_DICT_RESPOND "set_dict_respond"
#define GET_DICT_RESPOND "get_dict_respond"
#define DELETE_DICT_RESPOND "delete_dict_respond"
#define DICT_SYNC "dict_sync"
#define DICT_KEY "DictInfoKey"
#define DICT_LEVEL "DictLevel:"
#define DICT_LEVEL_ROOT "DictLevel:root"

namespace ICC
{
	enum ESyncType
	{
		SYNC_TYPE_ADD = 1,
		SYNC_TYPE_UPDATE,
		SYNC_TYPE_DELETE,
	};

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
        bool LoadDict();

        //获取字典信息
		void OnCNotifiGetDictRequest(ObserverPattern::INotificationPtr p_pNotify);

        //设置字典信息
		void OnCNotifiSetDictRequest(ObserverPattern::INotificationPtr p_pNotify);

        //删除字典信息
		void OnCNotifiDeleteDictRequest(ObserverPattern::INotificationPtr p_pNotify);

        void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
		void SyncDict(const CDictInfo& p_oDict, const ESyncType& p_roSyncType);

	public:
		void CreateDictLevel(std::unordered_map<std::string, std::string>& p_mapOutParent, const std::string& p_strInLevel,
			const std::string& p_strInParentGuid, const std::vector<PAIR>& p_vecInDict);
		void AddDictCache(const std::map<std::string, CDictInfo>& p_mapDict);

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