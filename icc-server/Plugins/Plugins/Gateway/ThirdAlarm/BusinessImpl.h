#pragma once

namespace ICC
{
	enum SyncType
	{
		ADD = 1,
		UPDATE,
		DELETE
	};
	enum IDType
	{
		THIRDALARM = 1,
		CHANGEINFO
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
		void OnNotifiTAGetAlarmByGuidRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTAGetAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTAAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTAGetNotReadCountRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTASetStateRequest(ObserverPattern::INotificationPtr p_pNotify); 
		void OnNotifiTASetChangeInfoStateRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTAGetChangeInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiTASetChangeInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify);

		void SendTAAlarmSync(std::string p_strGuid, SyncType p_SyncType);
		void SendTANotReadCountSync(std::string p_strGuid);
		void SendTAStateSync(std::string p_strGuid,std::string p_strState);
		void SendTAChangeInfoSync(std::string p_strGuid, SyncType p_SyncType);
		void SendTAChangeInfoNotReadCountSync(std::string p_strGuid, IDType p_eIDTYpe);
		void SendTAChangeInfoStateSync(std::string p_strGuid, std::string p_strState);

		bool TransmitLocationRequest(PROTOCOL::CTAGetLocationRequest p_oRequest);
		bool IsBlackAlarmTTL(std::string p_strPhone);

	private:
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);
		void BuildRespondHeader(PROTOCOL::CHeader& p_oSyncHeader, std::string p_strCmd);

		bool BuildTAGetAlarmByGuidRespond(PROTOCOL::CTAGetAlarmByGuidRespond& p_oRespond, const PROTOCOL::CTAGetAlarmByGuidRequest& p_oRequest);
		bool BuildTAGetAlarmRespond(PROTOCOL::CTAGetAlarmRespond& p_oRespond, const PROTOCOL::CTAGetAlarmRequest& p_oRequest);
		bool BuildTAAddAlarmRespond(PROTOCOL::CTAAddAlarmRespond& p_oRespond, const PROTOCOL::CTAAddAlarmRequest& p_oRequest);
		bool BuildTAGetNotReadCountRespond(PROTOCOL::CTAGetNotReadCountRespond& p_oRespond, const PROTOCOL::CTAGetNotReadCountRequest& p_oRequest);
		bool BuildTASetStateRespond(PROTOCOL::CTASetStateRespond& p_oRespond, const PROTOCOL::CTASetStateRequest& p_oRequest);
		bool BuildTASetChangeInfoStateRespond(PROTOCOL::CTASetChangeInfoStateRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoStateRequest& p_oRequest);
		bool BuildTAGetChangeInfoRespond(PROTOCOL::CTAGetChangeInfoRespond& p_oRespond, const PROTOCOL::CTAGetChangeInfoRequest& p_oRequest);
		bool BuildTASetChangeInfoRespond(PROTOCOL::CTASetChangeInfoRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoRequest& p_oRequest);

		bool BuildTAAlarmSync(PROTOCOL::CTAAlarmSync& p_oSync, const std::string p_strGuid);
		bool BuildTANotReadCountSync(PROTOCOL::CTANotReadCountSync& p_oSync, const std::string p_strGuid);
		bool BuildTAStateSync(PROTOCOL::CTAStateSync& p_oSync,std::string p_strGuid, std::string p_strState);
		bool BuildTAChangeInfoSync(PROTOCOL::CTAChangeInfoSync& p_oSync, const std::string p_strGuid);
		bool BuildTAChangeInfoNotReadCountSync(PROTOCOL::CTAChangeInfoNotReadCountSync& p_oSync, const std::string p_strGuid, IDType p_eIDTYpe);
		bool BuildTAChangeInfoStateSync(PROTOCOL::CTAChangeInfoStateSync& p_oSync, std::string p_strGuid, std::string p_strState);

	private:
		bool SelectThirdAlarm(PROTOCOL::CThirdAlarm& p_oThirdAlarm, const std::string p_strGuid);
		bool SelectThirdAlarmEx(PROTOCOL::CThirdAlarm& p_oThirdAlarm, const std::string p_strGuid);
		bool SelectThirdAlarmEx(PROTOCOL::CTAGetAlarmRespond& p_oRespond, const PROTOCOL::CTAGetAlarmRequest& p_oRequest);
		bool SelectThirdAlarmCount(std::string& p_strCount, const PROTOCOL::CTAGetAlarmRequest& p_oRequest);
		bool SelectThirdAlarmNCount(std::string& p_strCount, const std::string p_strCurrDeptCode);
		
		bool SelectThirdChangeinfo(PROTOCOL::CTAChangeInfoSync& l_oSync, const std::string p_strGuid);
		bool SelectThirdChangeinfoEx(PROTOCOL::CTAGetChangeInfoRespond& p_oRespond, const std::string p_strThirdAlarmGuid);
		bool SelectThirdChangeinfoEx(std::vector<PROTOCOL::CMaterial>& p_vecMaterial, const std::string p_strThirdAlarmGuid);		
		bool SelectThirdChangeinfoNCount(std::string& p_strNotReadChangeCount, const std::string p_strThirdAlarmGuid);
		bool SelectThirdChangeinfoTAGuid(std::string& p_strTAGuid, const std::string p_strGuid);
		
		bool UpdateThirdAlarm(PROTOCOL::CTASetStateRespond& p_oRespond, const PROTOCOL::CTASetStateRequest& p_oRequest);
		bool UpdateThirdChangeInfo(PROTOCOL::CTASetChangeInfoStateRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoStateRequest& p_oRequest);
		bool UpdateThirdChangeInfo(PROTOCOL::CTASetStateRespond& p_oRespond, std::string l_strThirdAlarmID);

		bool InsertThirdChangeInfo(PROTOCOL::CTASetChangeInfoRespond& p_oRespond, const PROTOCOL::CTASetChangeInfoRequest& p_oRequest);
		bool InsertThirdChangeInfoEx(PROTOCOL::CTAAddAlarmRespond& p_oRespond, const PROTOCOL::CTAAddAlarmRequest& p_oRequest);
		bool InsertThirdAlarm(PROTOCOL::CTAAddAlarmRespond& p_oRespond, const PROTOCOL::CTAAddAlarmRequest& p_oRequest);

	public:
		Log::ILogPtr						m_pLog;
		Config::IConfigPtr					m_pConfig;
		DataBase::IDBConnPtr				m_pDBConn;
		DateTime::IDateTimePtr				m_pDateTime;
		StringUtil::IStringUtilPtr			m_pString;
		Timer::ITimerManagerPtr				m_pTimerMgr;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;

	private:
		std::map<std::string, std::string>	m_mapCmdName;
		std::string m_strChangeInfoGuid;
	};
}