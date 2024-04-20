#pragma once

#include <boost/thread.hpp>
#include <atomic>
#include "CommonWorkThread.h"
#include "CMetadata.h"

#include "CommonTimer.h"
namespace ICC
{
#define MSGSEND_THREAD_COUNT 4

	class CBusinessImpl :
		public CBusinessBase, public CCommonThreadCore, public CCommonTimerCore
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
		void OnNotifiSendRequest(ObserverPattern::INotificationPtr p_pNotifiSendRequest);
		void OnNotifiHealthRequest(ObserverPattern::INotificationPtr p_pNotifiSendRequest);
		void OnNotifiHealthActivemqRequest(ObserverPattern::INotificationPtr p_pNotifiSendRequest);
		
		
		void OnReceiveReloadConfigNotify(ObserverPattern::INotificationPtr p_pNotifiSendRequest);
		void OnCreateShardTable();

		void OnPostActiveMQCheck();
		
		virtual void ProcMessage(CommonThread_data msg_data);
	public:
		void OnMessage(std::string p_strMsg, IResCallbackPtr p_callbackPtr);
		void OnResponse(std::string p_strMessage);
		void Response(std::string p_strGuid, std::string p_strMessage, bool p_IsSubPackage);

		//Https
		//void RegistNacos();
		bool StartHttpServer();
		void OnHttpMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody);
		void HttpRespond(const std::string& p_strGuid, const std::string& p_strContent);
	
	private:
		bool ConnectMQ();
		bool Subscribe();
		bool UnSubscribe();
		void ParseCmsProperty(std::string p_strJsonProperty, boost::shared_ptr<CMSHeader> pHeader);
	
	private:
		virtual void OnTimer(unsigned long uMsgId);

		bool ServiceLock(std::string p_strApplicationID, std::string& p_strLockValue, std::string& p_strError);
		//void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest,const std::string &p_strCmd="");
		bool CheckIsSyncCmd(const std::string& p_strCmd);
		void InitSyncCmdList();

		void _ProcessNacosCheck();
		bool _ExistNacosService();
		bool _RegistNacosService();
		bool _UnregistNacosService();

		std::string _GetUCMSAddrFromEnv();
		void        _ResetNacosParam(const std::string& strUcmsAddr);
		std::string _GetNacosParams(const std::string& strUcmsAddr);
		bool        _AnalyzeNacosParams(const std::string& strNacosParam);
		void        _NotifyNacosParams();
		void        _WriteNacosParams(const std::string& strValue);
		std::string _GetNacosParams();

		bool _HeartBeatNacosService();
		bool _AnalyzeNacosQueryResponse(const std::string& strReceive, std::map<std::string, std::string>& mapHosts);

		bool _AnalyzeNacosHeartBeatResponse(const std::string& strReceive, std::string &strHeartBeat, std::string &strCode);
		//获取nacos本机IP
		std::string _GetNacosLocalIP();
		bool _GetLocalAllIp(std::vector<std::string>& vecTmps);
		bool _IsSameSegment(const std::string& strSourceIp, const std::string& strDestIp);
		std::string _GetRealIp(const std::string& strTmpIp);

		void _HttpRespond(const std::string& p_strGuid, const std::string& p_strContent);
		CCommonWorkThread& _GetThread();

		void DoTimerEvent();

		// 服务状态检测
		bool healthPostgreSQL();
		bool healthRedis();
		void healthActivemqFlag();
		void healthActivemqRequest();
	private:
		boost::shared_ptr<boost::thread>   m_threadProcessNacos;
		std::atomic_bool m_bIsMaster;
		std::atomic_bool m_bStopNacos;
		
		HelpTool::IHelpToolPtr m_pHelpTool;
		IHttpClientPtr m_pHttpClient;
		long long m_lCheckInterval;
		std::string m_strNacosSuccessFlag;
	    
		//是否需要连接Nacos，如果需要连接，则需要取到本地地址才正常运行
		std::string m_strNeedNacos;

		bool m_bHealthActivemq;            

		std::map<std::string, ObserverPattern::INotificationPtr> m_mapHealthActivemq;
		
		std::mutex m_mutexHealthActivemq;

		std::string m_strRequestTime;     // 记录请求时间  

		std::string m_strStatus;
		std::string m_strActivemqStatus;
		std::string m_strpostgreSqlStatus;
		std::string m_strRedisStatus;
	private:
		Config::IConfigPtr m_pConfig;
		Config::IConfigPtr m_pStaticConfig;
		Log::ILogPtr m_pLog;
		IProducerPtr m_pProducer;
		IConsumerPtr m_pConsumer;
		int m_nActiveCheckCount;
		int m_nActiveCheckMaxValue;

		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		ObserverPattern::IObserverCenterList m_pObserverCenterList;

		ObserverPattern::IObserverCenterPtr m_pObserverCenteHealthActivemq;

		Lock::ILockFactoryPtr m_pLockFty;
		StringUtil::IStringUtilPtr m_pString;
		Lock::ILockPtr m_callbackMutex;
		Lock::ILockPtr m_notificationMutex;
		Redis::IRedisClientPtr m_pRedisClient;
		Timer::ITimerManagerPtr m_pTimerMgr;
		DateTime::IDateTimePtr m_pDateTime;
		DataBase::IDBConnPtr m_pDBConn;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		License::ILicensePtr m_pLicense;
		IHttpServerPtr m_pHttpServer;

		CCommonTimer m_timerServiceLock;
	private:
		std::string m_strStartTime;

		std::string m_strbrokerURI;
		std::string m_strClientID;
		std::string m_strApplicationID;
		std::string m_strUserName;
		std::string m_strPassword;
		std::string m_strTopics;
		std::string m_strSyncCmdList;
		std::string m_strQueues;

		std::string m_strMQCheckQueueName;

		std::map<std::string, IResCallbackPtr> m_callbackMap;//请求应答回调缓存
		std::map<std::string, ObserverPattern::INotificationPtr> m_notificationMap;//

		std::map<std::string, int> m_SyncCmdList;//请求应答回调缓存

		std::string m_strHttpLocalAddress;
		std::string m_strHttpLocalPort;
		std::string m_strDocRoot;

		int m_strCheckNum;
		std::string m_strShardCheckTime;
		std::string m_strShardUpdateTime;
		/*std::string m_strCrtFile;
		std::string m_strKeyFile;
		std::string m_strDhFile;*/

		//Nacos 参数
		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosNamespace;
		std::string m_strNacosServiceName;
		std::string m_strNacosGroupName;
		std::string m_strNacosLocalIP;
		std::string m_strNacosQueryUrl;
		std::string m_strNacosRegistUrl;

		ICC::CMetadata m_oNacosMetaData;

		boost::mutex m_responseCountMutex;

		//服务自身IP
		std::string m_strServerLocalIP; 

		unsigned int m_uiHeartBeatTime;

		unsigned int m_uiCountTimes;

		unsigned int m_uiHeartBeatCount;

		int   m_nTimeOut;
		bool  m_bNacosWorkCheck;
	private:
		bool m_bServiceState;

		bool m_bDisposeHeartMsg;  //是否处理心跳消息

		CCommonWorkThread m_oWorkThread; //另起线程处理心跳消息

		CCommonWorkThread m_msgSendThread[MSGSEND_THREAD_COUNT]; //另起线程处理心跳消息
		unsigned int m_respondHttpCount;
		std::mutex m_mutexRespondHttpCount;

		unsigned int                        m_uIndex;
		std::mutex                          m_mutexThread;

	private:

		class CNotifiReceivePrivate : public ObserverPattern::INotification
		{
		public:
			CNotifiReceivePrivate(CBusinessImpl* p_pBusinessImpl, std::string p_str, std::string p_strGuid)
				: m_pBusinessImpl(p_pBusinessImpl), m_str(p_str), m_strGuid(p_strGuid)
			{
				SetCmdGuid(p_strGuid);
			}
			virtual std::string GetMessages()
			{
				return m_str;
			}

			virtual void Response(std::string p_strResponse, bool p_IsSubPackage = false)
			{
				m_pBusinessImpl->Response(m_strGuid, p_strResponse, p_IsSubPackage);
			}

			virtual ObserverPattern::EBehavior GetBehavior()
			{
				return ObserverPattern::EBehavior::Received;
			}
			virtual ObserverPattern::ERequestMode GetRequestMode()
			{
				return ObserverPattern::ERequestMode::Invalid;
			}
			virtual ObserverPattern::INotificationPtr GetNotification()
			{
				return nullptr;
			}
			virtual std::string GetRequestGuid()
			{
				return m_strGuid;
			}
		private:
			std::string m_str;
			
		protected:
			std::string m_strGuid;
			CBusinessImpl* m_pBusinessImpl;
		};
		class CNotifiReceiveHttpPrivate : public CNotifiReceivePrivate
		{
		public:
			CNotifiReceiveHttpPrivate(CBusinessImpl* p_pBusinessImpl, std::string p_str, std::string p_strGuid, std::string p_strTarget)
				: CNotifiReceivePrivate(p_pBusinessImpl, p_str, p_strGuid), m_strTarget(p_strTarget)
			{
				//
				SetCmdName(p_strTarget);				
			}

			virtual void Response(std::string p_strResponse, bool p_IsSubPackage = false)
			{
				m_pBusinessImpl->HttpRespond(m_strGuid, p_strResponse);
			}

		private:
			std::string m_strTarget;
		};
	};

	class CResCallImpl : public IResCallback
	{
	public:
		CResCallImpl(CBusinessImpl* p_pBusinessImpl)
			: m_pBusinessImpl(p_pBusinessImpl){}
		~CResCallImpl(){}
	public:
		virtual void OnResponse(std::string p_strMessage);
	private:
		CBusinessImpl* m_pBusinessImpl;
	};
}

