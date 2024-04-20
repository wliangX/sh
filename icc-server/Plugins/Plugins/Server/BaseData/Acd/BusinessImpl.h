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
		//增加ACD部门关系
		void OnCNotifiAcdAddRequest(ObserverPattern::INotificationPtr p_pNotify);
		//取消ACD部门关系
		void OnCNotifiAcdDeleteRequest(ObserverPattern::INotificationPtr p_pNotify);
		//查询ACD部门关系
		void OnCNotifiAcdQueryRequest(ObserverPattern::INotificationPtr p_pNotify);
		//更新ACD部门关系
		void OnCNotifiAcdUpdateRequest(ObserverPattern::INotificationPtr p_pNotify);

		//从数据库里查出所有的ACD部门关系
		void SearchAcdRelationInDb(std::vector< std::map<std::string, std::string>>& m_vecAcdRelation);
		//将数据库里查出来的ACD部门关系存入Redis中
		void InsertAcdRelationInRedis(std::vector< std::map<std::string, std::string>>& m_vecAcdRelation);
		//综合前两步操作，从数据库抽取ACD数据并存入Redis
		void SaveAcdData();

	private:
		//更新ACD部门关系
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