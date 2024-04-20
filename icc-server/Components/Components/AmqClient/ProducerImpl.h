#pragma once

#include <boost/thread/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <AmqClient/IProducer.h>
#include "amqinc.h"
#include <mutex>


namespace ICC 
{
	typedef struct _tagProducerMsg
	{
		boost::shared_ptr<CMSHeader> pHeader;
		int iMode;   //0:TOPIC 1:QUEUE
		string strDest;
		string strMsg;
		IResCallbackPtr pResCallback;
		_tagProducerMsg()
		{
			iMode = -1;
			pResCallback = nullptr;
		}
	}ProducerMsg, *PProducerMsg;

	typedef struct _tagResMsg
	{
		string strCMSCorrelationID;
		string strMsg;
	}ResMsg, *PResMsg;

	struct ResCallBackInfo
	{
		IResCallbackPtr pResCallbackPtr;
		long long       llBeginTime;
		std::string     strId;

		ResCallBackInfo(){ llBeginTime = 0; pResCallbackPtr == nullptr; }
		ResCallBackInfo(const ResCallBackInfo& other)
		{
			_Assign(other);
		}
		ResCallBackInfo& operator=(const ResCallBackInfo& other)
		{
			if (&other != this)
			{
				_Assign(other);
			}
			return *this;
		}
		void _Assign(const ResCallBackInfo& other)
		{
			this->pResCallbackPtr = other.pResCallbackPtr;
			this->llBeginTime = other.llBeginTime;
			this->strId = other.strId;
		}
	};
	
	typedef map<string, Destination*> mapDestination;
	typedef map<string, Destination*>::iterator itDest;
	//typedef map<string, IResCallbackPtr> mapResCallback;
	typedef map<string, IResCallbackPtr>::iterator itResCallback;
	typedef map<string, ResCallBackInfo> mapResCallbackInfo;
	

	class CProducerImpl : public IProducer, public MessageListener
	{
	public:
		CProducerImpl(IResourceManagerPtr p_pResourceManager);
		virtual ~CProducerImpl();
	public:
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string());
		virtual bool Send(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = nullptr);
		virtual bool Post(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = nullptr);
		virtual bool Request(std::string strMsg, std::string strDest, IResCallbackPtr pResCallback, PCMSHeader pHeader = nullptr);
	public:
		virtual void onMessage(const cms::Message* message);		
	private:
		bool CreateDestination(const string& strDest, int iMode);
		void Initialize();
		void Cleanup();
		static void* SendThreadFunc(void *arg);						    //异步发送消息线程
		static void* ResThreadFunc(void *arg);							//消费对端应答消息线程							
		void Start();
		void SetProperty(Message *pMsg, PCMSHeader pHeader);
		void ListenTempQueue();										    //监听临时队列		
		
		static void* _CheckCallbackExpired(void *arg);
		void _InsertCallbackInfo(const std::string& strId, const ResCallBackInfo& callbackInfo);
		IResCallbackPtr _ResCallbackPtrById(const std::string& strId);
		void _RemoveCallbackInfo(const std::string& strId);
		void _AllCallbackInfo(mapResCallbackInfo& mapResCallBackInfos);

	private:
		Connection* m_pConnection;										//连接对象
		Session* m_pSession;											//一个发送或接收消息的线程
		mapDestination m_mapDest[ICC_DEST_TYPE];						//主题管理/队列管理 0:TOPIC 1:QUEUE
		//mapResCallback m_mapCallback;									//请求消息关联ID与回调映射	
		boost::mutex m_mutexCallbackInfo;
		mapResCallbackInfo m_mapCallbackInfo;
		MessageProducer* m_pProducer;									//生产者
		Destination* m_pTmpQueue;										//临时队列，标识请求应答模式中回复的地址
		MessageConsumer* m_pConsumer;									//消费对端回复的临时队列消息
		shared_ptr<boost::thread> m_callbackThread[2];					//回调线程 0:发送线程 1:接收线程
		boost::mutex m_mutexMsg;										//保护消息队列m_qMsg锁
		boost::mutex m_mutexResMsg;										//保护消息队列m_lResMsg锁
		boost::condition_variable_any m_cvaMsg;							//消息条件变量
		boost::condition_variable_any m_cvaResMsg;						//消息条件变量
		std::list<ProducerMsg> m_lMsg;									//消息队列，异步发送的消息都加入队列中待回调函数调用
		std::list<ResMsg> m_lResMsg;									//消息队列，接收的回复消息都加入队列中待回调函数调用
		bool m_bexitThread;												//退出
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		StringUtil::IStringUtilPtr m_pString;
		int m_nCMSPriority;
		long long m_lCMSExpiration;
		bool m_bConsumerExpiryCheckEnabled;
		//CCommonTimer m_timerCheckResCallbackExpired;
		shared_ptr<boost::thread> m_threadCheckResCallbackExpired;
		boost::atomic_bool m_bIsStopCheckResCallbackExpired;
		unsigned int m_uDefaultCallbackExpired;

		int m_intTimedWait;												//AmqClient抢锁等待时间 单位:秒
	};

} /*namespace ICC*/
