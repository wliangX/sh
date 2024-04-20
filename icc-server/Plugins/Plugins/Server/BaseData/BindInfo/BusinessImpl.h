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
		//��ȡ����Ϣ
		void OnNotifiGetBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		//���ð���Ϣ
		void OnNotifiSetBindInfo(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ������Ϣ
		void OnNotifiDeleteBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:
		//���ذ���Ϣ
        bool LoadBindInfo();
		//ͬ������Ϣ
		bool SyncBindInfo(const CBindInfo& p_info, std::string p_strSyncType);

		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
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