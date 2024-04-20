#pragma once

#define SYSTEMID                    "ICC"
#define SUB_SYSTEMID                "ICC-VcsServer";

#define SYNTHETICAL_FEEDBACK_INFO				"synthetical_feedback_info"
#define ICC_UPDATE_FEEDBACK						"icc_update_feedback"
#define ADD_OR_UPDATE_FEEDBACK_REQUEST			"add_or_update_feedback_request"
#define QUEUE_ALARM								"queue_alarm"
#define APP_EDIT_FEEDBACK_RESPOND				"app_edit_feedback_respond"
#define GET_ALARM_ALL_INFO_REQUEST				"get_alarm_all_info_request"
#define APP_GET_FEEDBACK_RESPOND				"app_get_feedback_respond"

#define SELECT_ID_FROM_ICC_T_ALARM_PROCESS		"select_id_from_icc_t_alarm_process"

namespace ICC
{
	class CBusinessImpl : public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();            
    public:      
        void Connect();
	public:
		void AlarmFeedbackSync(ObserverPattern::INotificationPtr p_pRequest);
		void AppEditFeedback(ObserverPattern::INotificationPtr p_pRequest);
		void AppGetFeedback(ObserverPattern::INotificationPtr p_pRequest);
		void AppBackAlarmRequest(ObserverPattern::INotificationPtr p_pRequest);
		void AppGetAlarmIdRequest(ObserverPattern::INotificationPtr p_pRequest);
		void OnAlarmProcessSync(ObserverPattern::INotificationPtr p_pNotify);
		void AppGetAlarmIdFromICCRespond(ObserverPattern::INotificationPtr p_pRequest);
		void AppCommitAlarmInfoRequest(ObserverPattern::INotificationPtr p_pRequest);
		void OnCommitAlarmInfoResponse(ObserverPattern::INotificationPtr p_pRequest);

	public:
		void AddOrUpdateFeedbackRespond(ObserverPattern::INotificationPtr p_pRequest);
		void GetFeedbackRespond(ObserverPattern::INotificationPtr p_pRequest);
		void OnMpaEditAlarmRequest(ObserverPattern::INotificationPtr p_pRequest);
		void OnMpaEditAlarmRespond(ObserverPattern::INotificationPtr p_pRequest);


	private:
		bool        _BackAlarmHandle(PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		void        _BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader, const std::string& strResponseCmd);
		void        _BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd);

		std::string _QueryProcessIdByAlarmIdAndProcessDeptCode(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool        _IsStateBeforAccepted(std::string p_strProcessID, const std::string& strTransGuid);
		bool        _UpdateProcessState(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool        _UpdateAlarmState(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		bool        _InsertBackReason(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);

		std::string _BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
		bool        _InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid);
		void        _SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
		bool        _AddAlarmLogInfo(const PROTOCOL::CAppBackAlarmRequest& p_oRequest, const std::string& strTransGuid);
		void        _SynAlarmInfo(const std::string& strAlarmId);

		bool        _BuildWebProcessDept(std::string m_strAlarmID, std::vector<PROTOCOL::CAppProcessDept>& p_vecProcessDept);
		bool		_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName);
    private:    
		Config::IConfigPtr					m_pConfig;
        Log::ILogPtr						m_pLog;
		StringUtil::IStringUtilPtr			m_pString;
		DateTime::IDateTimePtr				m_pDateTime;    
        DataBase::IDBConnPtr				m_pDBConn;
		Redis::IRedisClientPtr				m_pRedisClient;
		std::map<uint64, std::string>		m_mCallRefId;
		uint64								m_uCallExpire;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
	private:
		Lock::ILockFactoryPtr m_LockFacPtr;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;

		std::map<std::string, std::string>   m_mapGuidAndCmds;

		//config
		std::string m_strAssistantPolice;	//配置项是否为辅警
		std::string m_strPolice;			//配置项是否为民警
		std::string m_strCodeMode;			//号码模式：1、警员身份证号码 2、警员编号
	};
}