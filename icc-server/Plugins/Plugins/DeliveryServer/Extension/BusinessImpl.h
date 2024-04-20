#pragma once
#define THREADCOUNT 4
#include "CommonWorkThread.h"

namespace ICC
{
	class CBusinessImpl;
	typedef void (CBusinessImpl::* ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);

	class CBusinessImpl : public CBusinessBase, CCommonThreadCore,public CCommonTimerCore
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
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//����¼����ַ����
		void OnReceivePlayRecord(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//����¼����ַ����
		void OnReceiveDownloadRecord(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//ǿ�Ƶ��������һ��ʱ����ͨ��״̬�Ļ���û�д�������umc��Ϣ֪ͨǰ��
		void OnReceiveMandatoryList(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
	public:
		//����¼���Ų�ѯ���ݿ�����û�нӾ�������
		bool IsExitsAlarmId(std::string &strSourceId,std::string &strAlarmId); 
		//����һ������ʱ��id
		std::string CreateTimerId();
		//��ʱ��������
		virtual void OnTimer(std::string uMsgId);
		void ExchangeData(PROTOCOL::CDeviceInfo &l_DeciceInfo,PROTOCOL::CDeviceStateEvent &l_DeviceStateEvent);
		//��ȡNacos�������
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		//��ȡ�ļ���������IP��ַ�Լ��˿�
		bool GetFileService(std::string& p_strIp, std::string& p_strPort,const std::string& strFileServiceName);

		void _CommonRecordRespond(ObserverPattern::INotificationPtr p_pNotifiRequest,PROTOCOL::CReceiveDownloadRecordRespond& l_DownloadRespond,const std::string& strCode, const std::string& strMsg,const std::string& strMsgId);
		bool _GetCallMsgFromDB(const std::string& strCallRefId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond,std::string& strRecordType);
		//¼����ͬһ��¼������¼�ģ�����true�����򷵻�false
		bool _JudgeIsSameRecordType(std::vector<std::string> m_vecTrafficId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, std::string& strSumRecordType, bool& bIsSame);
		bool _JudgeSingleRecordType(const std::string& strCallRefId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, std::string& strRecordType);
		void _BulidRecordContent(const PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest);
		void _GetFmsRecord(const PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, const std::string& strFmsIp, const std::string& strFmsPort, int i);
		void _GetSingleFpsRecordToLoad(const PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond,const std::string& strFpsIp, const std::string& strFpsPort, int i);
	private:
		void    _OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void    _DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName);
		virtual void ProcMessage(CommonThread_data msg_data);
		CommonWorkThreadPtr _GetThread();
		void    _InitProcNotifys();
		void    _CreateThreads();
		void    _DestoryThreads();

		unsigned int                        m_uCurrentThreadIndex;
		unsigned int                        m_uProcThreadCount;
		std::string                         m_strDispatchMode;

		std::mutex                          m_mutexThread;
		std::vector<CommonWorkThreadPtr>    m_vecProcThreads;
		std::map<std::string, ProcNotify>   m_mapFuncs;

	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr		m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;
		Redis::IRedisClientPtr		m_pRedisClient;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;
		IHttpClientPtr				m_pHttpClient;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		CCommonTimer				m_pTimer;

	private:
		unsigned long long			m_timerIdCount;
		std::string					m_strNacosServerIp;
		std::string					m_strNacosServerPort;
		std::string					m_strNacosServerNamespace;
		std::string					m_strNacosServerGroupName;
		std::string					m_strNacosQueryUrl;
		std::string					m_strServiceHealthyFlag;
		std:: map<std::string, PROTOCOL::CDeviceInfo> m_DeviceDataList;
		std::mutex					m_DeviceMutex;
		std::mutex					m_timerIdCountMutex;
		boost::mutex			    m_mutexNacosParams;

		std::string					m_strDetectionTime;		//�����Ƿ���ڵļ��ʱ�䣬�������ʱ����ͻ��˷�umc֪ͨ
		std::string					m_strIsDetectionACD;	//û��ACD�Ƿ���ǿ�Ƶ�������������
		std::string					m_strIsUsing;			//�Ƿ���ǿ�Ƶ���
		std::string					m_strSendOverTime;
		std::string					m_strFpsServiceName;	//�ļ�������fps������
		std::string					m_strFpsPalyTarget;		//����¼���ӿ�
		std::string					m_strFpsDownloadTarget;	//����¼���ӿ�
		std::string					m_strFmsServiceName;	//�ļ�������fms������
		std::string					m_strFmsTarget;			//fms�ļ��������ӿ�
	};
}