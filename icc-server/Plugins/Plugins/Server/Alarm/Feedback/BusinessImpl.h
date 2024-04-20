#pragma once

#define DIC019100 "06"	//已终结
#define DIC021020 "DIC021020"	//已到达
#define BS001003005 "BS001003005"
#define BS001003006 "BS001003006"
#define PROCESS_STATUS_CANCEL "DIC020071"    //处警单已取消
#define PROCESS_STATUS_RETURN "DIC020073"    //处警单已退单

#define MSG_SOURCE_VCS	"{\"string\":[{\"key\":\"msg_source\",\"value\":\"vcs\"}]}"
#define MSG_SOURCE		"{\"string\":[{\"key\":\"msg_source\",\"value\":\"\"}]}"
#define SELECT_FEEDBACK_COUNT "select_feedback_count"
namespace ICC
{
    typedef std::map<std::string, PROTOCOL::CGetFeedBackRespond::CFeedbackData> mapFeedback;
    //typedef PROTOCOL::CGetFeedBackRespond::CProcessData ProcessData;
	typedef PROTOCOL::CAddOrUpdateProcessRequest::CProcessData ProcessData;
	typedef PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData LinkedData;
    typedef PROTOCOL::CAlarmInfo AlarmData;
    typedef PROTOCOL::CGetAlarmLogFeedBackResourceRespond::CData FeedBackBook;
    typedef PROTOCOL::CGetFeedBackRespond::CFeedbackData FeedbackData; 

	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:

		void OnCNotifiGetFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiGetFeedbackByProcessRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiSetFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCGetAlarmLogFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify);

		void QueryFeedbackByAlarmID(std::string p_strAlarmID, std::vector<FeedbackData>& p_vecFeedBack);
        void QueryFeedbackByProcessID(std::string p_strProcessID, std::vector<FeedbackData>& p_oFeedBack);
        bool QueryFeedbackByFeedBackID(std::string p_strID, FeedbackData& p_oFeedBack);

		bool CheckIsEdit(const PROTOCOL::CSetFeedBackRequest::CBody& p_oFeedBackRequest, const FeedbackData& p_oFeedBack);
	private:
        bool AddFeedbackBook(FeedbackData& p_oFeedBack, std::string p_strGuid, std::string p_strFlag);
        bool GetAlarmProcess(std::string l_strID, ProcessData& l_oProcess);
		bool GetAlarmProcessByAlarmID(std::string p_strAlarmID, std::vector<ProcessData>& p_vecProcess);
		bool GetLinkedDispatchByAlarmID(std::string p_strAlarmID, std::vector<LinkedData>& p_vecProcess); // 通过警单获取联动单位信息

        bool GetAlarm(std::string l_strID, AlarmData& l_oAlarm,std::string l_strCallRefID="");
        bool GetFeedBackBook(std::string l_strID, std::vector<FeedBackBook>& p_oFeedBackBook);
		void SyncAlarmInfo(const AlarmData& p_roAlarmSync, std::string l_strType, std::string p_strReleatedID = "");
		void SyncProcessInfo(const ProcessData& p_rProcessToSync, std::string l_strType, std::string p_strReleatedID = "");
		void SyncFeedBackInfo(const FeedbackData& p_rFeedBackToSync, std::string l_strType, std::string p_strReleatedID = "");
        void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
        std::string GenAlarmLogContent(std::vector<std::string> p_vecParamList);	
		std::string GenAlarmLogAttach(std::string& type, std::string& id);
		bool SearchRecordFileID(AlarmData& l_oAlarm);
		bool		_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName);
	private:
		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
        Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
		JsonParser::IJsonFactoryPtr m_pJsonFty;

    private:
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
        Lock::ILockFactoryPtr m_LockFacPtr;

		//config
		std::string m_strAssistantPolice;	//配置项是否为辅警
		std::string m_strPolice;			//配置项是否为民警
		std::string m_strCodeMode;			//警员号码模式：1、警员身份证号码 2、警员编号
	};
}