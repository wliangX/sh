#pragma once
#include "ManagerKeyDept.h"

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
        //�����ص㵥λ��Ϣ
		void OnCNotifiSetKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //��ȡ�����ص㵥λ��Ϣ
		void OnCNotifiGetAllKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//�ж�ĳ��λ�Ƿ�Ϊ�ص㵥λ
		void OnCNotifiCheckDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//ɾ���ص㵥λ��Ϣ
		void OnNotifiDeleteKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

		//ͬ���ص㵥λ
		void SendSyncKeyDept(CKeyDept& p_tKeyDept,eSyncType p_eSyncType);

		//�����ݿ��м����ص㵥λ����
		bool LoadKeyDeptFromDB();

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq, const std::string& strTransGuid);

	private:
		bool AddKeyDept(std::string p_strPhone, const CKeyDept& p_tKeyDept);
		bool DeleteKeyDept(std::string p_strPhone);
		bool GetAllKeyDept(std::map<std::string, std::string>& p_mapKeyDept);
		bool GetKeyDeptByPhone(std::string p_strPhone, CKeyDept& p_tKeyDept);
		std::string GetIDByPhone(std::string p_strPhone);
		bool ExistInCache(std::string p_strPhone);

	private:

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;

		Redis::IRedisClientPtr m_pRedisClient;
		// �ص㵥λ������� [3/23/2018 w26326]
		ManagerKeyDeptPtr m_pManagerKeyDept;
	};
}