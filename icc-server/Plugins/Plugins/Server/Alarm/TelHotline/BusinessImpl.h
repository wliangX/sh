#pragma once

namespace ICC
{
	typedef struct MsgCenterListNode
	{
		DateTime::CDateTime lastExecTime;
		std::string		    sendMsg;
		std::string		    sendTar;
		int					sendCount;
	}MsgCenterListNode;

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
		//110ת��12345�����ط���Ϣ
		void OnCNotifiTHLVisitRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiTHLConfirmRequest(ObserverPattern::INotificationPtr p_pNotify);

		//110���;�����12345
		void OnPushAlarmTo12345(ObserverPattern::INotificationPtr p_pNotify);
		void OnRepulseAlarmTo12345(ObserverPattern::INotificationPtr p_pNotify);//����12345ת110�������˻���Ϣ
		void OnPushRevisitTo12345(ObserverPattern::INotificationPtr p_pNotify);

		//ҵ��֧�Žӿ�
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnMsgCenterRetransmission(ObserverPattern::INotificationPtr p_pNotify);
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
	private:
		//ͬ����Ϣ����
		void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
		void SyncLinkedTableState(std::string l_alarmId);
		void SyncAlarmInfo(const PROTOCOL::CAlarmInfo& p_roAlarmSync);

		//���ݿ����
		bool InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid = "");
		bool SelectDictNameOnDictTable(std::string l_alarmId, PROTOCOL::CPushAlarmRequest& l_oRequest);
		bool UpdateLinkedTableState(std::string l_alarmId, std::string state);
		bool UpdateLinkedTableState(PROTOCOL::CTelHotlineVisitRequest::CBody p_alarmInfo);
		void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value);
		bool UpdateAlarmState(PROTOCOL::CAlarmInfo& p_roAlarmSync, std::string l_alarmId, std::string state);
		bool UpdateAlarmStateEx(PROTOCOL::CAlarmInfo& p_roAlarmSync, std::string l_alarmId, std::string state, std::string l_lhlx);

		// ���������ֶ�
		std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
		bool Build12345Request(PROTOCOL::CPushAlarmRequest& p_sRequest, PROTOCOL::CPushAlarmTo12345Request& p_dRequest);

		//�жϲ���
		bool IsSingle12345Alarm(std::string l_strAlarmId);

		//��ȡ12345Ip��˿�
		bool Get12345Service(std::string& p_strIp, std::string& p_strPort);
		bool Get12345WorkId(PROTOCOL::CPushRevisitRequest& l_oRequest);
		bool Get12345WorkId(std::string p_strAlarmId);
		bool GetAlarmerName(std::string l_alarmerId, std::string& l_alarmName);
		std::vector<std::pair<std::string, std::string>> GetSonAlarm(std::string l_alarmId, std::string l_createTime);
		bool Get12345WorkId(std::string l_alarmId, std::string& l_workId);
		bool IsPushTo12345(std::string l_strAlarmId);

		//ʧ���ش�
		void AddWorkToRetransmission(std::string l_strTar, std::string l_strMsg, std::string l_strKey);

		std::string ChangeTime(std::string l_utcTime);

	private:
		//���ݱ�������
		////110ת��12345�������̰���
		//void OnCNotifiTHLAddRequest(ObserverPattern::INotificationPtr p_pNotify);
		////110ת��12345������������
		//void OnCNotifilTHLFpideRequest(ObserverPattern::INotificationPtr p_pNotify);
		////110ת��12345��������״̬
		//void OnCNotifiTHLHandleStatRequest(ObserverPattern::INotificationPtr p_pNotify);
		////110ת��12345�����ط���Ϣ
		//void OnCNotifiTHLVisitRequest(ObserverPattern::INotificationPtr p_pNotify);
		////110ת��12345����������
		//void OnCNotifiTHLRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		Timer::ITimerManagerPtr m_pTimerMgr;

		HelpTool::IHelpToolPtr m_pHelpTool;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		Config::IConfigPtr m_pConfig;
		Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		IHttpClientPtr m_pHttpClient;
		MsgCenter::IMessageCenterPtr m_pMsgCenter;
		Redis::IRedisClientPtr m_pRedisClient;

		std::string m_str12345ServerIp;
		std::string m_str12345ServerPort;
		std::string m_strLang_code;
		std::string m_strIdGenRule;

		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex     m_mutexNacosParams;

		std::mutex	m_violationCheckMutex;
		std::map<std::string, int> m_mAlarmViolationCheckList;

		std::mutex	m_msgCenterMutex;
		std::map<std::string, MsgCenterListNode> m_mMsgCenterRetransmissionList;
		int m_nMsgCenterExecTime;
		int m_nMsgCenterExecCount;

		std::string m_sendMsgOverTime;
	};
}