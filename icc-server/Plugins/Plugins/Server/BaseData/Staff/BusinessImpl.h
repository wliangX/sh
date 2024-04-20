#pragma once
#include "StaffInfo.h"

#define STAFF_INFO_KEY "StaffInfo"

namespace ICC
{
	enum ESyncType
	{
		ADD = 1,
		UPDATE,
		DELETE
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
		//��ȡ��Ա��Ϣ
		void OnNotifiGetStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

		//���þ�Ա��Ϣ
		void OnNotifiSetStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

		//ɾ����Ա��Ϣ
		void OnNotifiDeleteStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:       

        //���ؾ�Ա����
		bool LoadStaffInfo();

		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
		void GeneralHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		//ͬ����Ա��Ϣ		
		bool SyncStaffInfo(const CStaffInfo &p_oStaff, ESyncType p_eSyncType);

    private:
        DataBase::IDBConnPtr m_pDBConn;		
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig; 
		JsonParser::IJsonPtr m_pIJson;
		DateTime::IDateTimePtr m_pDateTime; 
		StringUtil::IStringUtilPtr m_pString;
		JsonParser::IJsonFactoryPtr m_JsonFacPtr;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr	m_pRedisClient;	//redis�ͻ���
	};
}