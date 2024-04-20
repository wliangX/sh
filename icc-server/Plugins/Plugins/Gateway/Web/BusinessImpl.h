#pragma once

namespace ICC
{
	//////////////////////////////////////////////////////////////////////////
	// Web对接服务业务处理类
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
		void AddObserverInfo();

	public:
		void OnNotifiWebCheckLinkRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebLoginRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebLogoutRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebServiceCloseNotify(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebGetDictRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebGetAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		
		// 根据警单id获取接警和处警信息 [8/13/2018 t26150]
		void OnNotifiWebGetAlarmDetailRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebUpdateAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebSignRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebAddAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebGetAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebAddAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebSearchBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiBackAlarmResponse(ObserverPattern::INotificationPtr p_pNotify);
		//获取单位信息
		void OnNotifiWebGetDeptRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebDeptSyncRequest(ObserverPattern::INotificationPtr p_pNotify);
		//获取警员信息
		void OnNotifiWebGetStaffRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiWebStaffSyncRequest(ObserverPattern::INotificationPtr p_pNotify);

	
		void OnNotifiAlarmSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAlarmProcessSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAlarmRemarkSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAlarmFeedbackSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAlarmLogSync(ObserverPattern::INotificationPtr p_pNotify);

		void OnNotifiAddNoticeRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetNoticeRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAddUrgesRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
	
	public:
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		std::string GenToken(std::string p_strUserCode);
		std::string Token2UserCode(std::string p_strToken);
		bool CkeckIsNotLogin(std::string p_strUserCode,std::string p_strIPList);
		bool CheckToken(std::string p_strToken);
		bool CheckUserCode(const std::string& p_strUserCode, std::string& p_strToken);
		bool CheckIP(std::string p_strCode, std::string p_strInputIP);
		bool CheckIP(std::vector<std::string> p_vecInputIP, std::string p_strIPRange);
		bool ReadLoginRespondInfo(std::string p_strToken, PROTOCOL::CWebLoginRespond& l_oRespond);

		void SendRequestMsg(std::string p_strSendMsg);
		
		void BuildRequestHeader(PROTOCOL::CHeader& p_pRequestHeader, std::string p_strCmd, std::string p_strQueueName);	// Web对接服务到ICC服务_请求
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);			// Web对接服务到Web客户端_响应
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd);								// Web对接服务到主题_转发
		bool BuildRespond(PROTOCOL::CHeader& p_pRequestHeader, std::string& p_strResult, 
			const PROTOCOL::CHeader& p_oRequestHeader,std::string p_strToken);											// Web对接服务到Web客户端_响应
		bool BuildRespondEx(PROTOCOL::CHeader& p_pRequestHeader, std::string& p_strResult,
			const PROTOCOL::CHeader& p_oRequestHeader, std::string& p_strToken, std::string p_strUserCode);

		bool GetAlarmInfo(std::string p_strAlarmID, PROTOCOL::CWebGetAlarmRespond::CAlarm &p_oAlarm);
		bool GetProcessInfo(std::string p_strAlarmID, std::string p_strDeptCode, PROTOCOL::CWebGetAlarmRespond::CAlarmProcess &p_oProcess);
		bool GetFeedbackInfo(std::string p_strProcessID, PROTOCOL::CWebGetAlarmFeedBackRespond::CFeedback &p_oFeedback);

		bool BuildWebLoginRespond(PROTOCOL::CWebLoginRespond& p_oRespond, const PROTOCOL::CWebLoginRequest& p_oRequest);
		void BuildSetClientRegisterRequest(PROTOCOL::CSetClientRegisterRequest& p_oRequest, const PROTOCOL::CWebLoginRequest& p_oWebLoginRequest);
		void BuildDeleteClientRegisterRequest(PROTOCOL::CDeleteClientRegisterRequest& p_oRequest, const PROTOCOL::CWebLogoutRequest& p_oWebLogoutRequest);
		bool BuildWebGetDictRespond(PROTOCOL::CWebGetDictRespond& p_oRespond, const PROTOCOL::CWebGetDictRequest& p_oRequest);
		void BuildWebGetAlarmRespond(PROTOCOL::CWebGetAlarm& p_oRespond, const PROTOCOL::CWebGetAlarmRequest& p_oRequest);
		bool BuildWebGetAlarmDetailRespond(PROTOCOL::CWebGetAlarmDetailRespond& p_oRespond, const PROTOCOL::CWebGetAlarmDetailRequest& p_oRequest);
		void BuildWebAddAlarmRequest(PROTOCOL::CWebAddAlarmRequest& p_oRequest);
		void BuildWebUpdateAlarmRequest(PROTOCOL::CWebAddAlarmRequest& p_oRequest);
		void BuildWebAddAlarmRemarkRequest(PROTOCOL::CWebAddAlarmRemarkRequest& p_oRequest);
		void BuildWebGetAlarmFeedbackRespond(PROTOCOL::CWebGetAlarmFeedBackRespond& p_oRespond, const PROTOCOL::CWebGetAlarmFeedBackRequest& p_oRequest);
		void BuildWebAddAlarmFeedBackRequest(PROTOCOL::CWebAddAlarmFeedBackRequest& p_oRequest);
		bool BuildWebSignRequest(PROTOCOL::CWebSignRequest& p_oRequest);
		bool BuildWebProcessDept(std::string m_strAlarmID, std::vector<PROTOCOL::CProcessDept>& p_vecProcessDept);
		void BuildWebBackAlarmSync(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, PROTOCOL::CWebBackAlarmSync& p_oSync);
		bool SelectAlarmReciptInfo(const std::string& p_strAlarmId, std::string& p_strReciptId, std::string& p_strReciptName);

		bool BackAlarmHandle(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool IsStateBeforAccepted(std::string p_strProcessID, const std::string& strTransGuid);
		bool UpdateProcessState(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool _InsertBackReason(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool _UpdateAlarmState(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid);

		bool AddAlarmLogInfo(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
		bool InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid);
		void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);

		bool GetDeptGuid(const std::string& p_strInDeptCode, std::string& p_strOutDeptGuid, std::string& p_strOutParentDeptGuid);
		//bool LoadAllDept(std::map<std::string, std::string>& p_mapDeptInfo);

	private:
		bool m_bIsLinkAlive;
		std::string m_strServerGuid;
	
	private:
		std::map<std::string, std::string> m_mapCmdName;
		std::map<std::string, std::string> m_mapFilterDicts;

	public:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		DataBase::IDBConnPtr m_pDBConn;
		Redis::IRedisClientPtr	m_pRedisClient;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		HelpTool::IHelpToolPtr m_pHelpTool;

		Timer::ITimerManagerPtr m_pTimerMgr;

		bool m_bStartedTimerFlag;
	};
}