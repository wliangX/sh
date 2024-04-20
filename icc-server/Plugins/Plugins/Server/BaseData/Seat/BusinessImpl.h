#pragma once
#include "SeatInfo.h"

namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase
	{
		enum SyncType
		{
			ADD = 1,
			UPDATE,
			DELETE
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
		void OnCNotifiGetAllSeatRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiGetSeatByDeptRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiSetSeatRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiDeleteSeatRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		void SyncSeatInfo(const CSeatInfo& p_oSeat, SyncType p_eOperType);
		bool LoadSeat();		

	private:
		static const std::string SeatKey;

		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
        Config::IConfigPtr m_pConfig;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		Redis::IRedisClientPtr	m_pRedisClient;	//redis¿Í»§¶Ë
		
    private:
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
        Lock::ILockFactoryPtr m_LockFacPtr;
	};
}