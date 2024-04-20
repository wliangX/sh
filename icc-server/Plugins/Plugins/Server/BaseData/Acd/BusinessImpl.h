#pragma once

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
		//����ACD���Ź�ϵ
		void OnCNotifiAcdAddRequest(ObserverPattern::INotificationPtr p_pNotify);
		//ȡ��ACD���Ź�ϵ
		void OnCNotifiAcdDeleteRequest(ObserverPattern::INotificationPtr p_pNotify);
		//��ѯACD���Ź�ϵ
		void OnCNotifiAcdQueryRequest(ObserverPattern::INotificationPtr p_pNotify);
		//����ACD���Ź�ϵ
		void OnCNotifiAcdUpdateRequest(ObserverPattern::INotificationPtr p_pNotify);

		//�����ݿ��������е�ACD���Ź�ϵ
		void SearchAcdRelationInDb(std::vector< std::map<std::string, std::string>>& m_vecAcdRelation);
		//�����ݿ���������ACD���Ź�ϵ����Redis��
		void InsertAcdRelationInRedis(std::vector< std::map<std::string, std::string>>& m_vecAcdRelation);
		//�ۺ�ǰ���������������ݿ��ȡACD���ݲ�����Redis
		void SaveAcdData();

	private:
		//����ACD���Ź�ϵ
		void _SyncUpdate(PROTOCOL::CAcdSync::CBody& data);

	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr		m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;
		Redis::IRedisClientPtr m_pRedisClient;
	};
}