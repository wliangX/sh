#pragma once

namespace ICC
{
#define PROTOCOL_QUEUE	"0"
#define PROTOCOL_TOPIC	"1"

	enum ESyncType
	{
		SYNC_TYPE_ADD = 1,
		SYNC_TYPE_MODIFY = 2,
		SYNC_TYPE_DELETE = 3
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
		//��ȡ��λ��Ϣ
		void OnNotifiGetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);
		//���õ�λ��Ϣ
		void OnNotifiSetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ����λ��Ϣ
		void OnNotifiDeleteBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡ��ϵ����Ϣ
		void OnNotifiGetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);
		//������ϵ����Ϣ
		void OnNotifiSetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ����ϵ����Ϣ
		void OnNotifiDeleteBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡ�������Ϣ
		void OnNotifiGetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		//�����������Ϣ
		void OnNotifiSetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ���������Ϣ
		void OnNotifiDeleteBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡ�绰��Ϣ
		void OnNotifiGetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);
		//���õ绰��Ϣ
		void OnNotifiSetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ���绰��Ϣ
		void OnNotifiDeleteBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		bool LoadBookOrg();
		bool LoadBookPhone();
		bool LoadBookGroup();
		bool LoadBookContact();
		
		bool DelContactByGrpId(const std::string& p_strGrpId);
		bool DelContactInfo(const std::string& p_strContactId);

		//ͬ����Ϣ
		void SyncOrgInfo(const PROTOCOL::CBookOrgInfo& p_oDept, ESyncType p_eSyncType);
		void SyncPhoneInfo(const PROTOCOL::CBookPhoneInfo& p_oStaff, ESyncType p_eSyncType);
		void SyncGroupInfo(const PROTOCOL::CContactGrpInfo& p_oGroup, ESyncType p_eSyncType);
		void SyncContactInfo(const PROTOCOL::CContactInfo& p_oContact, ESyncType p_eSyncType);

		void BuildProtocolHeader(PROTOCOL::CHeader& p_oOutHeader, const std::string& p_strCmd, const std::string& p_strRequest, 
			const std::string& p_strRequestType = PROTOCOL_QUEUE, const std::string& p_strRelateId = "");
	
	private:
		Log::ILogPtr			m_pLog;
		Config::IConfigPtr		m_pConfig;
		DataBase::IDBConnPtr	m_pDBConn;
		DateTime::IDateTimePtr	m_pDateTime;
        StringUtil::IStringUtilPtr	m_pString;
		JsonParser::IJsonFactoryPtr	m_pJsonFty;

		Redis::IRedisClientPtr		m_pRedisClient;		//redis�ͻ���
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}