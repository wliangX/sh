#pragma once
#include "ManagerPhoneMark.h"

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
        //���õ绰�����Ϣ
		void OnCNotifiSetPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify);

        //��ȡ���е绰�����Ϣ
		void OnCNotifiGetAllPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡָ���绰�����Ϣ
		void OnCNotifiGetPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify);

		//ɾ���绰�����Ϣ
		void OnNotifiDeletePhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify);

		//ͬ���绰���
		void SendSyncPhoneMark(CPhoneMark p_tPhoneMark,eSyncType p_eSyncType);

	private:
		//�����ݿ��м��ص绰�������
		bool LoadPhoneMarkFromDB();

		bool ExistInCache(std::string p_strGuid);		
		bool DeletePhoneMark(std::string p_strGuid);
		bool GetAllPhoneMarkInfo(std::map<std::string, std::string>& p_mapPhoneMark);
		bool GetPhoneMarkByGuid(std::string p_strGuid, CPhoneMark &p_tPhoneMark);
		void AddPhoneMark(std::string p_strGuid, const CPhoneMark& p_tPhoneMark);

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		bool ExecSql(DataBase::SQLRequest p_oSQLReq, const std::string& strTransGuid);

		std::string _GetPhoneMarkTimes(const std::string& p_strPhone, const std::string& p_strType);
	private:

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr		m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;

		// Redis�ͻ���
		Redis::IRedisClientPtr m_pRedisClient;
		// �绰��ǻ������ [3/23/2018 w26326]
		//ManagerPhoneMarkPtr m_pManagerPhoneMark;
	};
}