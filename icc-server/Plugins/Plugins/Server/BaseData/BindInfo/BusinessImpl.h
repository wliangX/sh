#pragma once
#include "BindInfo.h"

namespace ICC
{
	typedef std::map<std::string, PROTOCOL::CSetBindInfo::CBindInfo> mapBindInfo;
    
	class CBusinessImpl :
        public CBusinessBase
    {
		enum eSyncType
		{
			ICC_ADD = 1,
			ICC_MODIFY,
			ICC_DELETE
		};
    public:
        CBusinessImpl();
        ~CBusinessImpl();

	public:
		
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		//获取绑定信息
		void OnNotifiGetBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		//设置绑定信息
		void OnNotifiSetBindInfo(ObserverPattern::INotificationPtr p_pNotify);
		//删除绑定信息
		void OnNotifiDeleteBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:
		//加载绑定信息
        bool LoadBindInfo();
		//同步绑定信息
		bool SyncBindInfo(const CBindInfo& p_info, std::string p_strSyncType);

		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

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

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Lock::ILockFactoryPtr m_LockFacPtr;
	};
}