#pragma once

namespace ICC
{
	enum ESyncType
	{
		SYNC_TYPE_ADD = 1,
		SYNC_TYPE_UPDATE,
		SYNC_TYPE_DELETE,
	};
	typedef std::map<std::string, PROTOCOL::CSetFuncRequest::CBody> mapFuncInfo;
    
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
		//获取权限信息
		void OnNotifiGetFuncRequest(ObserverPattern::INotificationPtr p_pNotify);

		//设置权限信息
		void OnNotifiSetFuncRequest(ObserverPattern::INotificationPtr p_pNotify);

		//删除权限信息
		void OnNotifiDeleteFuncRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:
		//加载权限
		bool LoadFuncInfo();

		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		//同步权限信息
		bool SyncFuncInfo(const CFuncInfo& p_tPrivilege, const ESyncType& p_roSyncType);
	
	private:
		DataBase::IDBConnPtr m_pDBConn;
        IAmqClientPtr m_pAmqClient;
		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		JsonParser::IJsonPtr m_pIJson;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
        
		JsonParser::IJsonFactoryPtr m_JsonFacPtr;

		Redis::IRedisClientPtr m_pRedisClient;
		//权限信息缓存
		mapFuncInfo m_mapFuncInfo;
	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Lock::ILockFactoryPtr m_LockFacPtr;
	};
}