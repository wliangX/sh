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
		//播放录音地址请求
		void OnReceivePlayRecord(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//下载录音地址请求
		void OnReceiveDownloadRecord(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//强制弹单，检测一定时间内通话状态的话务没有创警，发umc消息通知前端
		void OnReceiveMandatoryList(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
	public:
		//根据录音号查询数据库里有没有接警单生成
		bool IsExitsAlarmId(std::string &strSourceId,std::string &strAlarmId); 
		//创建一个任务定时器id
		std::string CreateTimerId();
		//定时任务内容
		virtual void OnTimer(std::string uMsgId);
		void ExchangeData(PROTOCOL::CDeviceInfo &l_DeciceInfo,PROTOCOL::CDeviceStateEvent &l_DeviceStateEvent);
		//获取Nacos服务参数
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		//获取文件服务器的IP地址以及端口
		bool GetFileService(std::string& p_strIp, std::string& p_strPort,const std::string& strFileServiceName);

		void _CommonRecordRespond(ObserverPattern::INotificationPtr p_pNotifiRequest,PROTOCOL::CReceiveDownloadRecordRespond& l_DownloadRespond,const std::string& strCode, const std::string& strMsg,const std::string& strMsgId);
		bool _GetCallMsgFromDB(const std::string& strCallRefId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond,std::string& strRecordType);
		//录音是同一个录音服务录的，返回true，否则返回false
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

		std::string					m_strDetectionTime;		//警单是否存在的检测时间，超过检测时间给客户端发umc通知
		std::string					m_strIsDetectionACD;	//没有ACD是否检测强制弹单创警配置项
		std::string					m_strIsUsing;			//是否检测强制弹单
		std::string					m_strSendOverTime;
		std::string					m_strFpsServiceName;	//文件服务器fps服务名
		std::string					m_strFpsPalyTarget;		//播放录音接口
		std::string					m_strFpsDownloadTarget;	//下载录音接口
		std::string					m_strFmsServiceName;	//文件服务器fms服务名
		std::string					m_strFmsTarget;			//fms文件服务器接口
	};
}