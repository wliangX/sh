#pragma once
#include <CapDataProc.h>
#include <CommonTimer.h>
#include <AESClient.h>
#include <RecordFileManage.h>
#include <FSFtpFileDownLoad.h>


#define CTI_INCOMING           "incoming"		     //ACD �����룩
#define CTI_WAITING            "waiting"		     //ACD ���Ŷӣ�
#define CTI_ASSIGN             "assign"			     //ACD �����䣩
#define CTI_RELEASE            "release"		     //ACD �����ͣ�
#define CTI_FREESTATE          "freestate"	          //Device ������̬��
#define CTI_RINGSTATE          "ringstate"	          //Device ������̬��
#define CTI_RINGBACKSTATE      "ringbackstate"	      //Device ������̬��
#define CTI_DIALSTATE          "dialstate"		      //Device ������̬��
#define CTI_TALKSTATE          "talkstate"		      //Device ��ͨ��̬��
#define CTI_CONFERENCESTATE    "conferencestate"      //Device ������̬��
#define CTI_SILENTMONITOR      "silentmonitorstate"	  //Device ������̬��
#define CTI_HOLDSTATE          "holdstate"			   //Device ������̬��
#define CTI_HANGUPSTATE        "hangupstate"		   //Device ���һ�̬��
#define CTI_WAITHANGUPSTATE    "waithangupstate"	   //Device (�ȴ��һ�̬)

#define CMD_AesConnStateSync   "cti_connect_state_sync"    //AES����CTI״̬ͬ����Ϣ
#define CMD_AcdCallStateSync   "acd_call_state_sync"       //����״̬ͬ����Ϣ
#define CMD_DeviceStateSync    "device_state_sync"         //�豸״̬ͬ����Ϣ
#define CMD_CallOverSync       "call_over_sync"            //���н���ͬ����Ϣ
#define CMD_AlarmSync          "alarm_sync"                //�澯ͬ��
#define CMD_FileUploadSync     "record_file_up_sync"       //��CTI�����ļ�ͬ����Ϣ
#define CMD_FileUploadSyncAck  "record_file_up_sync_ack"   //CTI�ص��ļ�ͬ����Ϣ��Ӧ
#define CMD_TopicName_FileUploadSync "topic_record_file_up_sync"
#define CMD_Sync_Server_Lock   "sync_server_lock"          //�Ƿ��ȡ���˷�����

#define CMD_System_Id_Record                     "record"
#define CMD_SubSystemId_RecordServer             "record_server"
#define CMD_SendType_Topic						("1")	// ����
#define CMD_SendType_Queue						("0")	// ����


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
		//��ʱ����
		void OnTimer(unsigned long ulTimerID);

        void DoGetAESPhoneIP();

	private:
		//��ȡ��ʷ¼����¼
		bool DoProcHistoryCallList();
		// ��������ץ��ҵ��
		int CreateNetDataCap();
		// ֹͣ����ץ��ҵ��
		int StopNetDataCap();
		//�ϴ��ļ�ҵ��
		void DoFileUpload();
		int GetHistoryCallList(std::string& strUrl, int nPageSize, int nPageIndex, int& nAllCount);
		void GetFileRecordFromProtocol(const std::string& paraFSIp, const std::string& paraRecordFileList, const std::string& paraIncomingTime, const std::string& paraHangupTime,
			const std::string& paraAgentId, const std::string& paraCallRefId, const std::string& paraCallerId, const std::string& paraCalledId, bool bIsHistory = false);
		//��ʼ����FreeSwitch��Ftp����
		void StartConnectFSFtp();
		//���� 0:��ʾ�ϴ����ݿ�ɹ�������ֵ����ʾ�ϴ����ݿ�ʧ��
		int UpdateDBRecord(CRecordFilePtr pRecordFile);
		void SendFileSyncMsg(CRecordFilePtr pRecordFile, int nResult);
		int UploadFileToServer(CRecordFilePtr pRecordFile);
		void ThreadGetHistoryCallList();

		Log::ILogPtr                        m_pLog;
		Config::IConfigPtr                  m_pConfig;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		DateTime::IDateTimePtr              m_pDateTime;
		JsonParser::IJsonFactoryPtr         m_pJsonFty;
		DataBase::IDBConnPtr				m_pInitDBConn;		//	�����̵����ݿ�����, ���ڱ����ļ���¼
		StringUtil::IStringUtilPtr		    m_pString;

		FtpDownLoadManagePtr  m_pFtpDownLoadMgr; //FS¼���ļ�Ftp���ع�����

		unsigned long m_ulTimerSyncAvayaConfig;//ͬ��AVAYA AES��ʱ��
		unsigned long m_ulTimerUploadFiles; //�ϴ��ļ���ʱ��

		CAavayaAESClientPtr m_pAvayaAesClt;     //AES�ֻ�����ͬ����
		CCapDataProcPtr     m_pAvayaCap;        //����ץ������ 
		bool                m_bIsStartAvayaCap; //�Ƿ��Ѿ�����Avayaץ��
		int                 m_nEnableNetCap;    //�Ƿ�������ץ�� 
		std::string         m_strFSLocalRecordFileDir;  //FreeSwitch���ش洢¼���ļ���·��

		//�洢CallRefID��BeginTalkTime�Ĺ�ϵ
		std::string GetBeginTalkTimeByCallRefId(const std::string& strCallRefId, const std::string& strAgentID);
		void SetCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID, const std::string& strBeginTalkTime);
		void RmvCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID);
		void RmvCallBeginTalkTime(const std::string& strCallRefId);
		size_t CallBeginTalkTimeSize();
		std::mutex m_lkMap;
		std::map < std::string, std::map<std::string, std::string> > m_mapCallRefIdBeginTalkTime;

		//��ȡ��ʷ�����߳����
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