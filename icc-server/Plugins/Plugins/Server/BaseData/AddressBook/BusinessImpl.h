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
		//获取单位信息
		void OnNotifiGetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);
		//设置单位信息
		void OnNotifiSetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);
		//删除单位信息
		void OnNotifiDeleteBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify);

		//获取联系人信息
		void OnNotifiGetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);
		//设置联系人信息
		void OnNotifiSetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);
		//删除联系人信息
		void OnNotifiDeleteBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify);

		//获取面板组信息
		void OnNotifiGetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		//设置面板组信息
		void OnNotifiSetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		//删除面板组信息
		void OnNotifiDeleteBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify);

		//获取电话信息
		void OnNotifiGetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);
		//设置电话信息
		void OnNotifiSetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);
		//删除电话信息
		void OnNotifiDeleteBookContactRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		bool LoadBookOrg();
		bool LoadBookPhone();
		bool LoadBookGroup();
		bool LoadBookContact();
		
		bool DelContactByGrpId(const std::string& p_strGrpId);
		bool DelContactInfo(const std::string& p_strContactId);

		//同步信息
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

		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}