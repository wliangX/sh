#pragma once

#include "amqinc.h"
#include <boost/thread/thread.hpp>
#include <AmqClient/IConsumer.h>

namespace ICC 
{
	typedef struct _tagConsumer
	{
		Destination* pDestination;							//��Ϣ��Ŀ�ĵ�:һ����������������
		MessageConsumer* pConsumer;							//��Session���󴴽�������������Ϣ�Ķ���
		IAsyncCallbackPtr pCallback;						//����/������Ϣ�ص�
	}Consumer, *PConsumer;

	typedef struct _tagConsumerMsg
	{
		int iMode;   //0:TOPIC 1:QUEUE
		string strDest;										//��ϢĿ�ĵ�ַ����
		string strMsg;										//��Ϣ����
		cms::Destination* pCMSReplyTo;						//����Ϣ�ظ���Ϣ��Ŀ�ĵ�ַ
		string strCMSCorrelationID;							//�ظ���Ϣ�����ӵ�ԭ��Ϣ����Ϣ����

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
		static void* ThreadFunc(void *arg);						//��ͨ�����߳�
		static void* ThreadFuncEx(void *arg);					//����Ӧ�������߳�
	private:
		void Initialize();
		void Cleanup();
		void Start();
		string ConnectionInfo2Json(decaf::lang::Pointer<activemq::commands::ConnectionInfo> &info);
		string RemoveInfo2Json(decaf::lang::Pointer<activemq::commands::RemoveInfo> &info);
	private:
		Connection* m_pConnection;										    //���Ӷ���
		Session* m_pAsyncsession;											//һ���첽������Ϣ���߳�
		MessageProducer* m_pProducer;										//Ӧ��������
		mapConsumer m_mapConsumer[ICC_DEST_TYPE];							//�������/���й��� 0:QUEUE 1:TOPIC
		std::list<ConsumerMsg> m_lMsg[2];									//��Ϣ���У��첽���յ���Ϣ����������д��ص��������� 0:��ͨ��Ϣ 1:��ҪӦ�����Ϣ
		boost::mutex m_mutexMsg;											//������Ϣ����m_qMsg��
		boost::condition_variable_any m_cvaMsg;							    //��Ϣ��������
		shared_ptr<boost::thread> m_callbackThread[2];						//�ص��߳� 0:��ͨ��Ϣ 1:��ҪӦ�����Ϣ
		mapDurable m_mapDurable;											//�־û����Ĺ�ϵ<name,topic>
		bool m_bexitThread;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		StringUtil::IStringUtilPtr m_pString;
		int m_nCMSPriority;
		long long m_lCMSExpiration;
		bool m_bConsumerExpiryCheckEnabled;
		int  m_iMsgTotalCount;

		int m_intTimedWait;												//AmqClient�����ȴ�ʱ�� ��λ:��
	};

} /*namespace ICC*/
