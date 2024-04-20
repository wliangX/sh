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
		static void* SendThreadFunc(void *arg);						    //�첽������Ϣ�߳�
		static void* ResThreadFunc(void *arg);							//���ѶԶ�Ӧ����Ϣ�߳�							
		void Start();
		void SetProperty(Message *pMsg, PCMSHeader pHeader);
		void ListenTempQueue();										    //������ʱ����		
		
		static void* _CheckCallbackExpired(void *arg);
		void _InsertCallbackInfo(const std::string& strId, const ResCallBackInfo& callbackInfo);
		IResCallbackPtr _ResCallbackPtrById(const std::string& strId);
		void _RemoveCallbackInfo(const std::string& strId);
		void _AllCallbackInfo(mapResCallbackInfo& mapResCallBackInfos);

	private:
		Connection* m_pConnection;										//���Ӷ���
		Session* m_pSession;											//һ�����ͻ������Ϣ���߳�
		mapDestination m_mapDest[ICC_DEST_TYPE];						//�������/���й��� 0:TOPIC 1:QUEUE
		//mapResCallback m_mapCallback;									//������Ϣ����ID��ص�ӳ��	
		boost::mutex m_mutexCallbackInfo;
		mapResCallbackInfo m_mapCallbackInfo;
		MessageProducer* m_pProducer;									//������
		Destination* m_pTmpQueue;										//��ʱ���У���ʶ����Ӧ��ģʽ�лظ��ĵ�ַ
		MessageConsumer* m_pConsumer;									//���ѶԶ˻ظ�����ʱ������Ϣ
		shared_ptr<boost::thread> m_callbackThread[2];					//�ص��߳� 0:�����߳� 1:�����߳�
		boost::mutex m_mutexMsg;										//������Ϣ����m_qMsg��
		boost::mutex m_mutexResMsg;										//������Ϣ����m_lResMsg��
		boost::condition_variable_any m_cvaMsg;							//��Ϣ��������
		boost::condition_variable_any m_cvaResMsg;						//��Ϣ��������
		std::list<ProducerMsg> m_lMsg;									//��Ϣ���У��첽���͵���Ϣ����������д��ص���������
		std::list<ResMsg> m_lResMsg;									//��Ϣ���У����յĻظ���Ϣ����������д��ص���������
		bool m_bexitThread;												//�˳�
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

		int m_intTimedWait;												//AmqClient�����ȴ�ʱ�� ��λ:��
	};

} /*namespace ICC*/
