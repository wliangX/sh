#pragma once

#include "amqinc.h"
#include <boost/thread/thread.hpp>
#include <AmqClient/IConsumer.h>

namespace ICC 
{
	typedef struct _tagConsumer
	{
		Destination* pDestination;							//消息的目的地:一个队列名或主题名
		MessageConsumer* pConsumer;							//由Session对象创建的用来发送消息的对象
		IAsyncCallbackPtr pCallback;						//主题/队列消息回调
	}Consumer, *PConsumer;

	typedef struct _tagConsumerMsg
	{
		int iMode;   //0:TOPIC 1:QUEUE
		string strDest;										//消息目的地址名称
		string strMsg;										//消息内容
		cms::Destination* pCMSReplyTo;						//本消息回复消息的目的地址
		string strCMSCorrelationID;							//回复消息中连接到原消息的消息编码

		_tagConsumerMsg()
		{
			iMode = -1;
			pCMSReplyTo = nullptr;
			strCMSCorrelationID.clear();
		}
	}ConsumerMsg, *PConsumerMsg;

	typedef map<string, PConsumer> mapConsumer;
	typedef map<string, PConsumer>::iterator itConsumer;

	typedef map<string, string> mapDurable;
	typedef map<string, string>::iterator itDurable;

	class CAsyncConsumerImpl : public IConsumer, public cms::ExceptionListener, public MessageListener
	{
	public:
		CAsyncConsumerImpl(IResourceManagerPtr pResourceManager);
		virtual ~CAsyncConsumerImpl();
	public:
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string());
		virtual bool Subscribe(std::string strDest, int iMode, IAsyncCallbackPtr pCallback, const std::string& strSelector = std::string());
		virtual bool Unsubscribe(std::string strDest, int iMode);
		virtual bool DurableSubscribe(std::string strTopic, std::string strName, std::string strSelector, IAsyncCallbackPtr pCallback, bool bNoLocal = false);
		virtual bool Unsubscribe(std::string strName);
	public:
		virtual void onMessage(const cms::Message* message);
		void onException(const CMSException& ex AMQCPP_UNUSED);
		static void* ThreadFunc(void *arg);						//普通消费线程
		static void* ThreadFuncEx(void *arg);					//请求应答消费线程
	private:
		void Initialize();
		void Cleanup();
		void Start();
		string ConnectionInfo2Json(decaf::lang::Pointer<activemq::commands::ConnectionInfo> &info);
		string RemoveInfo2Json(decaf::lang::Pointer<activemq::commands::RemoveInfo> &info);
	private:
		Connection* m_pConnection;										    //连接对象
		Session* m_pAsyncsession;											//一个异步接收消息的线程
		MessageProducer* m_pProducer;										//应答生产者
		mapConsumer m_mapConsumer[ICC_DEST_TYPE];							//主题管理/队列管理 0:QUEUE 1:TOPIC
		std::list<ConsumerMsg> m_lMsg[2];									//消息队列，异步接收的消息都加入队列中待回调函数调用 0:普通消息 1:需要应答的消息
		boost::mutex m_mutexMsg;											//保护消息队列m_qMsg锁
		boost::condition_variable_any m_cvaMsg;							    //消息条件变量
		shared_ptr<boost::thread> m_callbackThread[2];						//回调线程 0:普通消息 1:需要应答的消息
		mapDurable m_mapDurable;											//持久化订阅关系<name,topic>
		bool m_bexitThread;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		StringUtil::IStringUtilPtr m_pString;
		int m_nCMSPriority;
		long long m_lCMSExpiration;
		bool m_bConsumerExpiryCheckEnabled;
		int  m_iMsgTotalCount;

		int m_intTimedWait;												//AmqClient抢锁等待时间 单位:秒
	};

} /*namespace ICC*/
