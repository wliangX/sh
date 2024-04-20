#pragma once
#include <CapDataProc.h>
#include <CommonTimer.h>
#include <AESClient.h>
#include <RecordFileManage.h>
#include <FSFtpFileDownLoad.h>


#define CTI_INCOMING           "incoming"		     //ACD （呼入）
#define CTI_WAITING            "waiting"		     //ACD （排队）
#define CTI_ASSIGN             "assign"			     //ACD （分配）
#define CTI_RELEASE            "release"		     //ACD （早释）
#define CTI_FREESTATE          "freestate"	          //Device （空闲态）
#define CTI_RINGSTATE          "ringstate"	          //Device （振铃态）
#define CTI_RINGBACKSTATE      "ringbackstate"	      //Device （回铃态）
#define CTI_DIALSTATE          "dialstate"		      //Device （拨号态）
#define CTI_TALKSTATE          "talkstate"		      //Device （通话态）
#define CTI_CONFERENCESTATE    "conferencestate"      //Device （会议态）
#define CTI_SILENTMONITOR      "silentmonitorstate"	  //Device （监听态）
#define CTI_HOLDSTATE          "holdstate"			   //Device （保留态）
#define CTI_HANGUPSTATE        "hangupstate"		   //Device （挂机态）
#define CTI_WAITHANGUPSTATE    "waithangupstate"	   //Device (等待挂机态)

#define CMD_AesConnStateSync   "cti_connect_state_sync"    //AES连接CTI状态同步消息
#define CMD_AcdCallStateSync   "acd_call_state_sync"       //呼叫状态同步消息
#define CMD_DeviceStateSync    "device_state_sync"         //设备状态同步消息
#define CMD_CallOverSync       "call_over_sync"            //呼叫结束同步消息
#define CMD_AlarmSync          "alarm_sync"                //告警同步
#define CMD_FileUploadSync     "record_file_up_sync"       //向CTI发送文件同步消息
#define CMD_FileUploadSyncAck  "record_file_up_sync_ack"   //CTI回的文件同步消息响应
#define CMD_TopicName_FileUploadSync "topic_record_file_up_sync"
#define CMD_Sync_Server_Lock   "sync_server_lock"          //是否获取到了服务锁

#define CMD_System_Id_Record                     "record"
#define CMD_SubSystemId_RecordServer             "record_server"
#define CMD_SendType_Topic						("1")	// 主题
#define CMD_SendType_Queue						("0")	// 队列


namespace ICC
{
	class RecordBusinessImpl : public CBusinessBase, public CCommonTimerCore
	{	
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();	

	public:
		RecordBusinessImpl();
		~RecordBusinessImpl();
		void OnCtiAesConnNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCtiCallInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCtiDeviceInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCtiCallOverSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCtiAlarmSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnSyncServerLock(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCtiFileSyncMsgAck(ObserverPattern::INotificationPtr p_pNotifiReponse);
		//定时任务
		void OnTimer(unsigned long ulTimerID);

        void DoGetAESPhoneIP();

	private:
		//获取历史录音记录
		bool DoProcHistoryCallList();
		// 创建网络抓包业务
		int CreateNetDataCap();
		// 停止网络抓包业务
		int StopNetDataCap();
		//上传文件业务
		void DoFileUpload();
		int GetHistoryCallList(std::string& strUrl, int nPageSize, int nPageIndex, int& nAllCount);
		void GetFileRecordFromProtocol(const std::string& paraFSIp, const std::string& paraRecordFileList, const std::string& paraIncomingTime, const std::string& paraHangupTime,
			const std::string& paraAgentId, const std::string& paraCallRefId, const std::string& paraCallerId, const std::string& paraCalledId, bool bIsHistory = false);
		//开始连接FreeSwitch的Ftp服务
		void StartConnectFSFtp();
		//返回 0:表示上传数据库成功，其他值：表示上传数据库失败
		int UpdateDBRecord(CRecordFilePtr pRecordFile);
		void SendFileSyncMsg(CRecordFilePtr pRecordFile, int nResult);
		int UploadFileToServer(CRecordFilePtr pRecordFile);
		void ThreadGetHistoryCallList();

		Log::ILogPtr                        m_pLog;
		Config::IConfigPtr                  m_pConfig;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		DateTime::IDateTimePtr              m_pDateTime;
		JsonParser::IJsonFactoryPtr         m_pJsonFty;
		DataBase::IDBConnPtr				m_pInitDBConn;		//	主进程的数据库链接, 用于保存文件记录
		StringUtil::IStringUtilPtr		    m_pString;

		FtpDownLoadManagePtr  m_pFtpDownLoadMgr; //FS录音文件Ftp下载管理类

		unsigned long m_ulTimerSyncAvayaConfig;//同步AVAYA AES定时器
		unsigned long m_ulTimerUploadFiles; //上传文件定时器

		CAavayaAESClientPtr m_pAvayaAesClt;     //AES分机号码同步类
		CCapDataProcPtr     m_pAvayaCap;        //网络抓包处理 
		bool                m_bIsStartAvayaCap; //是否已经启动Avaya抓包
		int                 m_nEnableNetCap;    //是否开启网络抓包 
		std::string         m_strFSLocalRecordFileDir;  //FreeSwitch本地存储录音文件的路径

		//存储CallRefID和BeginTalkTime的关系
		std::string GetBeginTalkTimeByCallRefId(const std::string& strCallRefId, const std::string& strAgentID);
		void SetCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID, const std::string& strBeginTalkTime);
		void RmvCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID);
		void RmvCallBeginTalkTime(const std::string& strCallRefId);
		size_t CallBeginTalkTimeSize();
		std::mutex m_lkMap;
		std::map < std::string, std::map<std::string, std::string> > m_mapCallRefIdBeginTalkTime;

		//获取历史话务线程设计
		boost::shared_ptr<boost::thread> m_thrdGetHistoryCallList;
		bool GetHistoryCallState(bool bGetHistorySetState)
		{
			std::lock_guard<std::mutex> lk(m_lkGetHistoryCall);
			if (m_bIsGetHistoryCall && bGetHistorySetState)
			{
				return true;
			}
			m_bIsGetHistoryCall = bGetHistorySetState;
			return false;
		}
		void SetHistoryCallState(bool bGetHistory)
		{
			std::lock_guard<std::mutex> lk(m_lkGetHistoryCall);
			m_bIsGetHistoryCall = bGetHistory;
		}
		bool m_bIsGetHistoryCall;
		std::mutex m_lkGetHistoryCall;
	};
}