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
        //������ϯĬ�ϵ�ַ��Ϣ
		void OnCNotifSetSeatAddressRequest(ObserverPattern::INotificationPtr p_pNotify);

        //��ȡ��ϯĬ�ϵ�ַ��Ϣ
		void OnCNotifGetSeatAddressRequest(ObserverPattern::INotificationPtr p_pNotify);
	private:

		//����������ϯĬ�ϵ�ַ����
		bool _ValidSetSeatAddress(const PROTOCOL::CSeatAddressSetRequest &in_oRequest, std::string &out_strErrMsg);

		bool _ExecSql(DataBase::SQLRequest in_oSQLReq, const std::string& in_strTransGuid);

		//�����е���ϯ��ַ
		bool _QueryAllSeatAddress(std::vector<ICC::PROTOCOL::CSeatAddressInfo>& vecAllSeatAddress);

		bool _LoadSeatAddressFromDB();

		bool _ExistSeatAddressBySeatno(const std::string& strSeatNo);
	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		Redis::IRedisClientPtr m_pRedisClient;
	};
}