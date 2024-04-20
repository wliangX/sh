#pragma once

#include "ACDCallState.h"

#define SYSTEMID                    "ICC"
#define SUB_SYSTEMID                "ICC-VcsServer";

#define SEND_TYPE_QUEUE             "0"
#define SEND_TYPE_TOPIC             "1"

#define QUEUE_ALARM_TO_VCS          "queue_alarm_to_vcs"
#define QUEUE_ALARM_TO_ICC          "queue_alarm_to_icc"
#define QUEUE_REQUEST_TO_ICC        "queue_request_to_icc"
#define QUEUE_REPONSE_TO_ICC        "queue_reponse_to_vcs"

#define CMD_UPDATE_CASE             "update_case"
#define CMD_UPDATE_DISPOSAL         "update_disposal"
#define CMD_LINKED_DISPATCH         "alarm_linked_dispatch_sync"
#define CMD_UPDATE_FEEDBACK         "update_feedback"
#define CMD_UPDATE_POSITION         "update_postion"
#define CMD_NOTIFY_INCOMINGCALL     "notify_incomingcall"

#define MSG_SOURCE_VCS              "vcs"
#define ACDCALLSTATEKEY		        "ACDCallState"
#define CASEINFO_CHANGED		    "CASEINFO_CHANGED"
#define CASEDISPOSAL_CHANGED		"CASEDISPOSAL_CHANGED"
#define CASEFEEDBACK_CHANGED		"CASEFEEDBACK_CHANGED"
#define TELINCOMING_CHANGED		    "TELINCOMING_CHANGED"
#define CASEINFO_VCSRESPONSE        "CASEINFO_VCSRESPONSE"
#define CASEDISPOSAL_VCSRESPONSE    "CASEDISPOSAL_VCSRESPONSE"
#define CASEFEEDBACK_VCSRESPONSE    "CASEFEEDBACK_VCSRESPONSE"
#define QUEUE_ALARM                 "queue_alarm"
#define ACCEPTE_CONFIRM             "BS001010001"
#define ARRIVE_DESTINATION          "BS001010002"
#define DISPOSAL_COMPLETION         "BS001010003"
#define RECORD_LOG_CODE             "BS001009001"
#define TEXTMSG_LOG_CODE            "BS001011001"
#define RECORD_RESOURSE_TYPE        "6"
#define SYNC_TYPE_ADD               "1"
#define SYNC_TYPE_UPDATE            "2"
#define SOURCE_TYPE_INPHONE         "01"//"DIC001001"

#define CASE_STATE_RECEIVED			"DIC019060"
#define CASE_STATE_ARRIVED			"DIC019080"
#define CASE_STATE_FINAL			"06"

#define POLICE_TYPE_110				"01"    //110
#define POLICE_TYPE_122				"02"    //122
#define POLICE_TYPE_MANU			"03"    //自接警
#define POLICE_TYPE_OTHER			"99"    //其他

#define POLICE_NO_110				"110"
#define POLICE_NO_119				"119"
#define POLICE_NO_122				"122"

#define VCS_TIMER					"vcstimer"
#define PARAM_INFO                  "ParamInfo"
namespace ICC
{
    typedef struct CallEventInfo
    {
        std::string m_strRingTime;			//呼入时间
        std::string m_strAnsweRingTime;		//应答时间
        std::string m_strSeatNo;            //接警坐席号
    }CallEventInfo;

    typedef PROTOCOL::CAlarmInfo Alarm;
    typedef PROTOCOL::CFeedBackSync::CBody FeedBack;
    typedef PROTOCOL::CAlarmProcessSync::CBody Process;

	class CBusinessImpl : public CBusinessBase, public IResCallback
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();            
    public:
        void SendAlarmChange(ObserverPattern::INotificationPtr p_pRequest);
        void SendProcessChange(ObserverPattern::INotificationPtr p_pRequest);
        void SendFeedBackChange(ObserverPattern::INotificationPtr p_pRequest);
        void SendCallInfoChange(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmCallerInfoSync(ObserverPattern::INotificationPtr p_pRequest);
        void OnRecvUpdateCallerInfo(ObserverPattern::INotificationPtr p_pRequest);
        void RecvAlarmUpdate(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmUpdateRequest(const PROTOCOL::CHeader& p_pRequestHeader, const Alarm& p_Alarm,const std::vector<Process>& p_vecProcess,const std::string p_strVcsFlag = "");
        void RecvFeedBackUpdate(ObserverPattern::INotificationPtr p_pRequest);
        void SendFeedBackUpdateRequest(const PROTOCOL::CHeader& p_pRequestHeader, const FeedBack& p_FeedBack);
		void RecvDeviceUpdate(ObserverPattern::INotificationPtr p_pRequest);
		void RecvTextMsgUpdate(ObserverPattern::INotificationPtr p_pRequest);
        void RecvRecordUpdate(ObserverPattern::INotificationPtr p_pRequest);

        void AlarmInfoRespond(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<Alarm>& p_vecAlarmInfo);
        void AlarmProcessInfoRespond(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmProcessInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<Process>& p_vecProcessInfo);
        void AlarmFeedBackInfoRespond(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmFeedBackInfoRespond(const PROTOCOL::CHeader& p_pRequestHeader, std::vector<FeedBack>& p_vecFeedBackInfo);

        void SaveCallInfo(ObserverPattern::INotificationPtr p_pRequest);

        void GenRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
        void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
        std::string GenAlarmLogContent(std::vector<std::string> p_vecParamList);
        std::string GenAlarmLogAttach(std::string& type, std::string& id);

        void SendRemarkToFeedBack(ObserverPattern::INotificationPtr p_pRequest);

        void GetGpsRespond(ObserverPattern::INotificationPtr p_pRequest);
        void SendAlarmPosition(ObserverPattern::INotificationPtr p_pRequest);
        void OnNotifiAlarmAttachSyn(ObserverPattern::INotificationPtr p_pRequest);
        void OnVcsAlarmAttachSync(ObserverPattern::INotificationPtr p_pRequest);

        void OnVcsNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

        void OnRecvVcsBllStatusSync(ObserverPattern::INotificationPtr p_pRequest);
        void RecvAlarmUpdateCase(ObserverPattern::INotificationPtr p_pRequest);
        void RecvUpdatePostion(ObserverPattern::INotificationPtr p_pRequest);
        void RecvUpdateMasterorg(ObserverPattern::INotificationPtr p_pRequest);
        void RecvUpdateDisposal(ObserverPattern::INotificationPtr p_pRequest);
        void RecvDeleteDisposal(ObserverPattern::INotificationPtr p_pRequest);
        void RecvUpdateFeedback(ObserverPattern::INotificationPtr p_pRequest);
        // 联合单位 VCS过来的消息
        void RecvUpdateLinkedDispatch(ObserverPattern::INotificationPtr p_pRequest);
        void RecvNotifyIncomingCall(ObserverPattern::INotificationPtr p_pRequest);

        void OnAdminDeptRequset(ObserverPattern::INotificationPtr p_pRequest);
        void OnSetTemporaryHoldRequest(ObserverPattern::INotificationPtr p_pRequest);
        void OnRecvUpdateLog(ObserverPattern::INotificationPtr p_pRequest);
        void RecvAlarmLogSync(ObserverPattern::INotificationPtr p_pRequest);

        void OnVcsAlarmRelatedCarsRequest(ObserverPattern::INotificationPtr p_pRequest);
        void OnVcsAlarmRelatedPersonsRequest(ObserverPattern::INotificationPtr p_pRequest);
        void OnRecvAlarmRelatedCarsSync(ObserverPattern::INotificationPtr p_pRequest);
        void OnRecvAlarmRelatedPersonsSync(ObserverPattern::INotificationPtr p_pRequest);

        void OnRecvSyncVcsDataRequest(ObserverPattern::INotificationPtr p_pRequest);
        void OnTimerVcsSyncRequestInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);

        void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);
        void OnReceiveSynLinkedDispatch(ObserverPattern::INotificationPtr p_pNotifiRequest);
        //消息中心部分
        void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value);
        void SendMessageToMsgCenterOnManualAccept(PROTOCOL::CAlarmLogSync l_oProcessData, std::string l_curMsg);

    public:
        bool QueryCallevent(std::string p_strID, CallEventInfo& p_tCallEventInfo);
        bool QueryAlarm(std::string p_strID, Alarm& p_tAlarmInfo);
        bool QueryAlarmProcessByID(std::string p_strID, Process& p_oProcess);
        bool QueryAlarmProcessByIDEx(std::string p_strID, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_pData);

        bool QueryAlarmProcessByAlarmID(std::string p_strID, std::vector<Process>& p_vecProcess);
        bool QueryFeedBack(std::string p_strID, FeedBack& p_oFeedBack);
        bool QueryAlarmUpdateTime(std::string p_strUpdateTime, std::vector<Alarm>& p_vecAlarmInfo);
        bool QueryProcessUpdateTime(std::string p_strUpdateTime, std::vector<Process>& p_vecProcessInfo);
        bool QueryFeedBackUpdateTime(std::string p_strUpdateTime, std::vector<FeedBack>& p_vecFeedBackInfo);
        bool InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo);

        void RequestVcsSyncFunc();
	public:
		virtual void OnResponse(std::string strMsg);
	public:
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
	private:
		void _BuildRequestHeader(PROTOCOL::CHeader& p_pRequestHeader, std::string p_strCmd, std::string p_strQueueName);
		std::string _QueryDeptCodeByProcesserCode(const std::string& strProcesserCode);
		std::string _QueryProcessIdByAlarmIdAndDeptCode(const std::string& strAlarmId, const std::string& strDeptCode);
		bool        _QueryProcessIdAndDeptCodeByAlarmId(const std::string& strAlarmId, std::string& strProcessId, std::string& strDeptCode, std::string& strDeptName);
		int         _Asigned(const std::string& strState);
		bool        _IsNeedSendAssigned(const std::string& strAlarmId, const std::string& strDeptCode);
	
        bool        _GetAlarmRelatedPersonsData(const std::string& p_strAlarmID, PROTOCOL::CAlarmRelatedPersonsQueryExRespond& p_AlarmRelatedPersonsRspd);
        bool        _GetAlarmRelatedCarsData(const std::string& p_strAlarmID, PROTOCOL::CAlarmRelatedCarsQueryExRespond& p_AlarmRelatedCarsRspd);

        bool        _DBChangeAlarmRelatedPersons(PROTOCOL::CAlarmRelatedPersonsInfoEx& p_PersonsInfo, const std::string& p_strFlag);
        bool        _DBChangeAlarmRelatedCars(PROTOCOL::CAlarmRelatedCarsInfoEx& p_RelatedCarsInfo, const std::string& p_strFlag);

        bool        _QueryProcess(const std::string& p_strCmd, const std::string& p_strValue, std::vector<Process>& p_vecProcessInfo);

        bool        _QueryAlarmRelatedCars(const std::string &p_strID);
        bool        _QueryAlarmRelatedPersons(const std::string& p_strID);

        std::string _GetDeptName(const std::string& p_strAdminDeptCode);
        void        _UpdateCaseSyncTime(const std::string& p_strMsgID,int nSyncType = 1);
        void        _UpdateCaseSyncTimeByMQ(const std::string& p_strMsgID);
        std::string _GetCaseSyncTime(int nSyncType);
    private:
        void        _GetVcsSyncConfig();
        bool        _QueryVcsService();
        bool        _LoginVcsService(std::string& p_strToken);
        void        _LogoutVcs(std::string& p_strToken);

        void        _VcsSynData();
        int         _ProcVcsSynData(const std::string & p_strStartTime = "");
        int         _ProcSynAlarmData(const std::string& p_strStartTime, const std::string& p_strToken);
        int         _ProcSynCallerInfoData(const std::string& p_strStartTime, const std::string& p_strToken);
        int         InsertDBCallerInfo(PROTOCOL::CVcsAlarmSynDataSar::CData l_tCallerInfo);
        int         QueryCallerData(const std::string& l_strCallerID);
        bool        UpdateCallerDataInfo(PROTOCOL::CVcsAlarmSynDataSar::CData synCallerData);
        int         _ProcSynProcessData(const std::string& p_strStartTime, const std::string& p_strToken);
        int         _ProcSynLogData(const std::string& p_strStartTime, const std::string& p_strToken);
        int         _ProcSynBllStatusData(const std::string& p_strStartTime, const std::string& p_strToken);
        bool        _InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
        bool        _UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
        bool        _SetSqlInsertProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, DataBase::SQLRequest& p_tSQLInsert);

        int        _QueryDBBllStatusInfo(const std::string& strID, PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data);
        bool        _InsertDBBllStatusInfo(const PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data);
        bool        _UpdateDBBllStatusInfo(const PROTOCOL::CVcsBllStatusSynDataRespond::CData& p_data);

        bool        _QueryAlarmLogByID(const std::string& p_strLogID, std::string p_strAlarmId);

		void _SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		void _GetNacosParams(std::string& strNacosIp, std::string& strNacosPort, std::string& strNameSpace, std::string& strGroupName);

        bool _QueryFeedBack(const std::string& p_strID, bool isUpdateFeedBackFlag);
        bool _InsertFeedBackData(const PROTOCOL::CFeekBackInfo& p_oFeekBackInfo);
        bool _UpdateFeedBackData(const PROTOCOL::CFeekBackInfo& p_oFeekBackInfo);
        bool GetAlarmReceiveByAlarmId(const std::string& p_strAlarmID, std::string& p_strReceiceTime);


        void SearchAlarmAttach(const PROTOCOL::SynAlarmAttachRequest& p_pRequest, PROTOCOL::SynAlarmAttachRequest& p_pResponse);
        bool SearchAttach(const PROTOCOL::SynAlarmAttachRequest& p_pRequest, std::string p_pSqlid, DataBase::SQLRequest sqlReqeust, DataBase::IResultSetPtr& p_pRSet);
    private:    
		Config::IConfigPtr					m_pConfig;
        Config::IConfigPtr					m_pStaticConfig;
        Log::ILogPtr						m_pLog;
		StringUtil::IStringUtilPtr			m_pString;
		DateTime::IDateTimePtr				m_pDateTime;    
		Redis::IRedisClientPtr				m_pRedisClient;
        DataBase::IDBConnPtr				m_pDBConn;
		Timer::ITimerManagerPtr				m_pTimerMgr;
		std::map<uint64, std::string>		m_mCallRefId;
		uint64								m_uCallExpire;
		std::string							m_queueToVCS;

        IHttpClientPtr                      m_pHttpClient;
        MsgCenter::IMessageCenterPtr        m_pMsgCenter;
        JsonParser::IJsonFactoryPtr         m_pJsonFty;
	private:
		Lock::ILockFactoryPtr m_LockFacPtr;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;

        ObserverPattern::IObserverCenterPtr m_pObserverCenterProcessVcsMessage;

    private:
        std::string	m_strServiceID;	  //服务ID
        std::string m_strNacosServerIp;
        std::string m_strNacosServerPort;

        std::string m_strNacosQueryUrl;
        std::string m_strNacosNamespace;
        std::string m_strNacosGroupName;
        std::string m_strVcsServiceName;

        boost::mutex     m_mutexNacosParams;

        std::string m_strVcsServiceHealthyFlag;
        std::string m_strVcsServerIp;
        std::string m_strVcsPort;

        int m_iPackageSize;
        std::string m_strIgnoreHttpError;
        int m_nFirstSyncDaysAgo;

        bool m_bEnbleVcsSyncFlag;

        int m_nVcsSyncUpdateFlag; //是否补偿更新数据

        bool m_bVcsSynSuccess;
        bool m_bFirstVcsSynFlag;
        boost::mutex     m_mutexSyning;
        bool m_bIsVcsSyning;
        boost::shared_ptr<boost::thread>   m_threadSynData;

        int m_nVcsSyncInterval;
        bool m_bStartedTimerFlag;
        std::string m_strVcsSyncStartTime;

        int m_nTimeOut;      //请求超时时间 

	};

    //监听主题回调类
    class CListenTopic : public IAsyncCallback
    {
    public:
        CListenTopic(CBusinessImpl* p_Vcs)
            : m_pVcs(p_Vcs){}
        ~CListenTopic(){}
    public:
		virtual void OnMessage(std::string p_strMsg, IResCallbackPtr p_callback = nullptr);
    private:
        CBusinessImpl*  m_pVcs;
    };


    //订阅队列回调类
    class CListenQueue : public IAsyncCallback
    {
    public:
        CListenQueue(CBusinessImpl* p_Vcs)
            : m_pVcs(p_Vcs){}
        ~CListenQueue(){}
    public:
        virtual void OnMessage(std::string p_strMsg, IResCallbackPtr p_callback = nullptr);
    private:
        CBusinessImpl*  m_pVcs;
    };
}