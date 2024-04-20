#pragma once
#include "PhoneInfo.h"

namespace ICC
{
	enum ePhoneInfoStatus
	{
		PHONEINFO_ADD = 1,
		PHONEINFO_UPDATE = 2,
		PHONEINFO_DELETE = 3
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
        bool LoadPhoneInfo();
		void OnCNotifiGetAllPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiGetPhoneInfoByPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiSetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiDeletePhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify);

		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);
		void SyncClient(CPhoneInfo p_ClientRegisterInfo, ePhoneInfoStatus p_eState);
		void SetPhoneInfo(CPhoneInfo& p_pPhoneInfo, PROTOCOL::CSetPhoneInfoRespond& p_pSetPhoneInfoResp,ObserverPattern::INotificationPtr& p_pNotifiSetPhoneInfoRequest);
		void UpdatePhoneInfo(CPhoneInfo& p_pPhoneInfo, PROTOCOL::CSetPhoneInfoRespond& p_pPhoneInfoResp, ObserverPattern::INotificationPtr& p_pNotifiSetPhoneInfoRequest);
		bool IsRecoverPhoneInfo(CPhoneInfo& p_pPhoneInfo);
		void SendSetPhoneinfoResponse(PROTOCOL::CSetPhoneInfoRespond& p_pPhoneInfoResp,ObserverPattern::INotificationPtr p_pNotifiSetPhoneInfoRequest);
	private:
		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
        Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;

		Redis::IRedisClientPtr		m_pRedisClient;		//redis¿Í»§¶Ë
        Lock::ILockPtr m_mapPhoneInfoLock;

        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
        Lock::ILockFactoryPtr m_LockFacPtr;
		JsonParser::IJsonFactoryPtr	m_pJsonFty;
	};
}