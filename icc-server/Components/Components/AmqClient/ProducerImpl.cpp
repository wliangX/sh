#include "Boost.h"
#include "ProducerImpl.h"
#include "ICCTransportListener.h"
#include <thread>

const unsigned long TIMERID_CHECKCALLBACKEXPIRED = 10000;
const unsigned long TIMERINTERVAL_CHECKCALLBACKEXPIRED = 5 * 1000;
const unsigned long DEFAULTEXPIRED_CALLBACK = 5 * 60;
const unsigned long MINEXPIRED_CALLBACK = 30;

namespace ICC 
{
	CProducerImpl::CProducerImpl(IResourceManagerPtr p_pResourceManager) 
		: m_pConnection(nullptr)
		, m_pSession(nullptr)
		, m_pProducer(nullptr)
		, m_pTmpQueue(nullptr)
		, m_pConsumer(nullptr)
		, m_bexitThread(false) 		
	{
		// TODO Auto-generated constructor stub
		m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
		m_pConfig = ICCGetResourceEx(Config::IConfigFactory, ICCIConfigFactoryResourceName, p_pResourceManager)->CreateConfig();
		m_pString = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
		m_nCMSPriority = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/AmqClient/CMSPriority", "4"));
		m_lCMSExpiration = m_pString->ToInt64(m_pConfig->GetValue("ICC/Component/AmqClient/CMSExpiration", "60000"));
		m_bConsumerExpiryCheckEnabled = m_pString->ToUShort(m_pConfig->GetValue("ICC/Component/AmqClient/IsConsumerExpiryCheckEnabled", "0"));
		m_uDefaultCallbackExpired = m_pString->ToUShort(m_pConfig->GetValue("ICC/Component/AmqClient/CallbackExpired", "0"));
		m_intTimedWait= m_pString->ToInt(m_pConfig->GetValue("ICC/Component/AmqClient/TimedWait", "1"));
		if (m_uDefaultCallbackExpired < MINEXPIRED_CALLBACK)
		{
			m_uDefaultCallbackExpired = DEFAULTEXPIRED_CALLBACK;
		}
		m_bIsStopCheckResCallbackExpired = false;

		ICC_LOG_DEBUG(m_pLog, "CProducerImpl CallbackExpired = %u", m_uDefaultCallbackExpired);

		Initialize();
		this->Start();

		ICC_LOG_INFO(m_pLog, "CProducerImpl constructor");
	}

	CProducerImpl::~CProducerImpl() 
	{
		// TODO Auto-generated destructor stub
		m_bexitThread = true;
		m_bIsStopCheckResCallbackExpired = true;
		m_cvaMsg.notify_all();
		m_cvaResMsg.notify_all();
		m_callbackThread[0]->join();
		m_callbackThread[1]->join();
		m_threadCheckResCallbackExpired->join();
		Cleanup();

		ICC_LOG_INFO(m_pLog, "CProducerImpl destructor");
	}

	bool CProducerImpl::Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName /*= std::string()*/, std::string strPassword /*= std::string()*/)
	{
		try
		{
			shared_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory(strBrokerURI));
			// Create a Connection
			if (nullptr == m_pConnection && nullptr != connectionFactory)
			{
				if (!strUserName.empty() && !strPassword.empty())
				{
					m_pConnection = connectionFactory->createConnection(strUserName, strPassword);
				}
				else
				{
					m_pConnection = connectionFactory->createConnection();
				}
				if (!strClientID.empty() && nullptr != m_pConnection)
				{
					strClientID.append(".Producer");
					m_pConnection->setClientID(strClientID);
				}
			}
			if (m_pConnection)
			{
				m_pConnection->start();
				m_pSession = m_pConnection->createSession(Session::AUTO_ACKNOWLEDGE);
				if (m_pSession)
				{
					m_pProducer = m_pSession->createProducer();

					strClientID = m_pConnection->getClientID();
					//设置消费者到期检查启用开关
					activemq::core::ActiveMQConnection* pAMQConnection = dynamic_cast<activemq::core::ActiveMQConnection*>(m_pConnection);
					if (pAMQConnection)
					{
						pAMQConnection->setConsumerExpiryCheckEnabled(m_bConsumerExpiryCheckEnabled);
					}
					//临时队列及监听
					ListenTempQueue();					
					return true;
				}
			}
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}

		return false;
	}

	bool CProducerImpl::Send(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader/* = nullptr*/)
	{
		if (m_mapDest[iMode].find(strDest) == m_mapDest[iMode].end())
		{
			this->CreateDestination(strDest, iMode);
		}

		try
		{
			if (nullptr != m_pSession && m_pProducer)
			{
				shared_ptr<BytesMessage> pMsg(m_pSession->createBytesMessage((const unsigned char*)strMsg.c_str(), strMsg.length()));
				if (pHeader)
				{
					SetProperty(pMsg.get(), pHeader);
				}
				m_pProducer->send(m_mapDest[iMode][strDest], pMsg.get(), DeliveryMode::PERSISTENT, m_nCMSPriority, m_lCMSExpiration);

				return true;
			}
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}

		return false;
	}

	bool CProducerImpl::Post(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader/* = nullptr*/)
	{
		try
		{
			if (m_mapDest[iMode].find(strDest) == m_mapDest[iMode].end())
			{
				this->CreateDestination(strDest, iMode);
			}

			ProducerMsg msg;
			msg.iMode = iMode;
			msg.strDest = strDest;
			msg.strMsg = strMsg;
			if (pHeader)
			{
				msg.pHeader = boost::make_shared<CMSHeader>(*pHeader);
			}

			m_mutexMsg.lock();
			m_lMsg.push_back(msg);
			m_mutexMsg.unlock();
			m_cvaMsg.notify_all();

			return true;
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}

		return false;
	}

	bool CProducerImpl::Request(std::string strMsg, std::string strDest, IResCallbackPtr pResCallback, PCMSHeader pHeader/* = nullptr*/)
	{
		try
		{
			if (m_mapDest[Destination::QUEUE].find(strDest) == m_mapDest[Destination::QUEUE].end())
			{
				this->CreateDestination(strDest, Destination::QUEUE);
			}

			ProducerMsg msg;
			msg.iMode = Destination::QUEUE;
			msg.strDest = strDest;
			msg.strMsg = strMsg;
			msg.pResCallback = pResCallback;
			if (pHeader)
			{
				msg.pHeader = boost::make_shared<CMSHeader>(*pHeader);
			}

			m_mutexMsg.lock();
			m_lMsg.push_back(msg);
			m_mutexMsg.unlock();
			m_cvaMsg.notify_all();

			return true;
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}

		return false;
	}

	void CProducerImpl::onMessage(const cms::Message* message)
	{
		try
		{
			string strMsg;
			string strCMSCorrelationID = message->getCMSCorrelationID();
			const BytesMessage* bytesMessage = dynamic_cast<const BytesMessage*>(message);
			if (bytesMessage)
			{
				unsigned int len = bytesMessage->getBodyLength();
				unsigned char* pRecvBytes = new unsigned char[len + 1]();
				bytesMessage->readBytes(pRecvBytes, len);
				strMsg = (char*)pRecvBytes;
				delete[] pRecvBytes;
			}

			ResMsg msg;
			msg.strCMSCorrelationID = strCMSCorrelationID;
			msg.strMsg = strMsg;

			m_mutexResMsg.lock();
			m_lResMsg.push_back(msg);
			m_mutexResMsg.unlock();
			m_cvaResMsg.notify_all();
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}
	}

	// Create the destination (Topic or Queue)
	bool CProducerImpl::CreateDestination(const string& strDest, int iMode)
	{
		try
		{
			itDest it = m_mapDest[iMode].find(strDest);
			if (it == m_mapDest[iMode].end())
			{
				if (m_pSession)
				{
					Destination* pDest = nullptr;
					if (Destination::QUEUE == iMode)
					{
						pDest = m_pSession->createQueue(strDest);
					}
					else if (Destination::TOPIC == iMode)
					{
						pDest = m_pSession->createTopic(strDest);
					}
					m_mapDest[iMode].insert(make_pair(strDest, pDest));

					return true;
				}
			}
		}
		catch (CMSException* e)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", e->getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}

		return false;
	}

	void CProducerImpl::Initialize()
	{
		for (int idx = 0; idx < ICC_DEST_TYPE; idx++)
		{
			m_mapDest[idx].clear();
		}

		//m_timerCheckResCallbackExpired.AddTimer(this, TIMERID_CHECKCALLBACKEXPIRED, TIMERINTERVAL_CHECKCALLBACKEXPIRED);
	}

	void CProducerImpl::Cleanup()
	{
		//m_timerCheckResCallbackExpired.RemoveTimer(TIMERID_CHECKCALLBACKEXPIRED);

		if (m_pConnection) 
		{
			try 
			{
				m_pConnection->close();
			}
			catch (cms::CMSException& ex) 
			{
				ICC_LOG_ERROR(m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
			}
			catch (...)
			{
				ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
			}
		}

		// Destroy resources.
		try 
		{
			for (int idx = 0; idx < ICC_DEST_TYPE; idx++) 
			{
				itDest it = m_mapDest[idx].begin();
				while (it != m_mapDest[idx].end())
				{
					delete it->second;
					it->second = nullptr;
					it++;
				}
			}

			if (m_pTmpQueue)
			{
				delete m_pTmpQueue; 
				m_pTmpQueue = nullptr;
			}

			if (m_pConsumer)
			{
				delete m_pConsumer;
				m_pConsumer = nullptr;
			}

			if (m_pSession)
			{
				m_pSession->close();
				delete m_pSession;
				m_pSession = nullptr;
			}
			if (m_pConnection)
			{
				delete m_pConnection;
				m_pConnection = nullptr;
			}
		}
		catch (cms::CMSException& ex)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}
	}

	void* CProducerImpl::SendThreadFunc(void *arg)
	{
		CProducerImpl* pThis = (CProducerImpl*)arg;
		while (true)
		{
			if (pThis->m_bexitThread)
				break;
			pThis->m_mutexMsg.lock();

			try
			{
				if (pThis->m_intTimedWait == 0)
				{
					pThis->m_cvaMsg.wait(pThis->m_mutexMsg);
				}
				else
				{
					pThis->m_cvaMsg.timed_wait(pThis->m_mutexMsg, boost::posix_time::seconds(pThis->m_intTimedWait));
				}
			}			
			catch (...)
			{
			}

			

			ProducerMsg msg;
			std::list<ProducerMsg>::iterator itMsg = pThis->m_lMsg.begin();
			while (itMsg != pThis->m_lMsg.end())
			{
				msg = *itMsg;
				itDest it = pThis->m_mapDest[msg.iMode].find(msg.strDest);
				if (it != pThis->m_mapDest[msg.iMode].end())
				{
					try
					{
						if (pThis->m_pSession && pThis->m_pProducer)
						{
							shared_ptr<BytesMessage> pMsg(pThis->m_pSession->createBytesMessage((const unsigned char*)msg.strMsg.c_str(), msg.strMsg.length()));
							if (msg.pHeader)
							{
								pThis->SetProperty(pMsg.get(), msg.pHeader.get());
							}
							if (msg.pResCallback && pMsg)
							{                       
                                std::string strCMSCorrelationID = pThis->m_pString->CreateGuid();
								pMsg->setCMSReplyTo(pThis->m_pTmpQueue);
								pMsg->setCMSCorrelationID(strCMSCorrelationID);

								ResCallBackInfo info;
								info.pResCallbackPtr = msg.pResCallback;
								info.strId = strCMSCorrelationID;
								//std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
								unsigned long long u64CurrentTime = std::time(NULL);//std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
								info.llBeginTime = u64CurrentTime;
								pThis->_InsertCallbackInfo(strCMSCorrelationID, info);
								//pThis->m_mapCallback.insert(make_pair(strCMSCorrelationID, msg.pResCallback));
							}
							pThis->m_pProducer->send(pThis->m_mapDest[msg.iMode][msg.strDest], pMsg.get(), DeliveryMode::NON_PERSISTENT, pThis->m_nCMSPriority, pThis->m_lCMSExpiration);
						}
					}
					catch (cms::CMSException& ex)
					{
						ICC_LOG_ERROR(pThis->m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
					}
					catch (...)
					{
						ICC_LOG_ERROR(pThis->m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
					}
				}

				itMsg++;
			}

			ICC_LOG_DEBUG(pThis->m_pLog, "count:[%d]", pThis->m_lMsg.size());
			pThis->m_lMsg.clear();

			try
			{
				pThis->m_mutexMsg.unlock();
			}			
			catch (...)
			{
				ICC_LOG_ERROR(pThis->m_pLog, "CMS[ unlock excepiton]");
			}

			
		}

		return (void*)0;
	}

	void* CProducerImpl::ResThreadFunc(void *arg)
	{
		CProducerImpl* pThis = (CProducerImpl*)arg;
		while (true)
		{
			try
			{
				if (pThis->m_bexitThread)
					break;
				pThis->m_mutexResMsg.lock();

				if (pThis->m_intTimedWait == 0)
				{
					pThis->m_cvaResMsg.wait(pThis->m_mutexResMsg);
				}
				else
				{
					pThis->m_cvaResMsg.timed_wait(pThis->m_mutexResMsg, boost::posix_time::seconds(pThis->m_intTimedWait));
				}

				ResMsg msg;
				std::list<ResMsg>::iterator itMsg = pThis->m_lResMsg.begin();
				while (itMsg != pThis->m_lResMsg.end())
				{
					msg = *itMsg;
					//itResCallback it = pThis->m_mapCallback.find(msg.strCMSCorrelationID);
					//if (it != pThis->m_mapCallback.end())
					{
						//IResCallbackPtr pCallback = it->second;
						IResCallbackPtr pCallback = pThis->_ResCallbackPtrById(msg.strCMSCorrelationID);
						if (pCallback)
						{
							pCallback->OnResponse(msg.strMsg);
						}						
					}

					itMsg++;
				}

				pThis->m_lResMsg.clear();
				pThis->m_mutexResMsg.unlock();
			}
			catch (cms::CMSException& ex)
			{
				ICC_LOG_ERROR(pThis->m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
			}
			catch (...)
			{
				ICC_LOG_ERROR(pThis->m_pLog, "CMS[unknown exception]");
			}
		}

		return (void*)0;
	}

	void CProducerImpl::Start()
	{
		m_callbackThread[0].reset(new boost::thread(boost::bind(&SendThreadFunc, this)));
		m_callbackThread[1].reset(new boost::thread(boost::bind(&ResThreadFunc, this)));
		m_threadCheckResCallbackExpired.reset(new boost::thread(boost::bind(&_CheckCallbackExpired, this)));
	}

	void CProducerImpl::SetProperty(Message *pMsg, PCMSHeader pHeader)
	{
		if (nullptr == pHeader || nullptr == pMsg)
		{
			return;
		}
		//setStringProperty
		map<string, string>::iterator iterss = pHeader->msProperties.begin();
		while (iterss != pHeader->msProperties.end())
		{
			pMsg->setStringProperty(iterss->first, iterss->second);
			iterss++;
		}
		//setLongProperty
		map<string, long long>::iterator itersl = pHeader->mlProperties.begin();
		while (itersl != pHeader->mlProperties.end())
		{
			pMsg->setLongProperty(itersl->first, itersl->second);
			itersl++;
		}
		//setIntProperty
		map<string, int>::iterator itersn = pHeader->mnProperties.begin();
		while (itersn != pHeader->mnProperties.end())
		{
			pMsg->setIntProperty(itersn->first, itersn->second);
			itersn++;
		}
		//setFloatProperty
		map<string, float>::iterator itersf = pHeader->mfProperties.begin();
		while (itersf != pHeader->mfProperties.end())
		{
			pMsg->setFloatProperty(itersf->first, itersf->second);
			itersf++;
		}
		//setDoubleProperty
		map<string, double>::iterator itersd = pHeader->mdProperties.begin();
		while (itersd != pHeader->mdProperties.end())
		{
			pMsg->setDoubleProperty(itersd->first, itersd->second);
			itersd++;
		}
		//setByteProperty
		map<string, unsigned char>::iterator itersc = pHeader->mcProperties.begin();
		while (itersc != pHeader->mcProperties.end())
		{
			pMsg->setByteProperty(itersc->first, itersc->second);
			itersc++;
		}
		//setBooleanProperty
		map<string, bool>::iterator itersb = pHeader->mbProperties.begin();
		while (itersb != pHeader->mbProperties.end())
		{
			pMsg->setBooleanProperty(itersb->first, itersb->second);
			itersb++;
		}
	}

	void CProducerImpl::ListenTempQueue()
	{
		try
		{
			if (m_pTmpQueue)
			{
				delete m_pTmpQueue;
				m_pTmpQueue = nullptr;
			}

			if (m_pConsumer)
			{
				delete m_pConsumer;
				m_pConsumer = nullptr;
			}

			m_pTmpQueue = m_pSession->createTemporaryQueue();
			m_pConsumer = m_pSession->createConsumer(m_pTmpQueue);
			if (m_pConsumer)
			{
				m_pConsumer->setMessageListener(this);
			}
		}
		catch (cms::CMSException& ex)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "CMS[%s]", ICCExceptionDefaultHandle.c_str());
		}
	}

	IResCallbackPtr CProducerImpl::_ResCallbackPtrById(const std::string& strId)
	{
		boost::lock_guard<boost::mutex> lock(m_mutexCallbackInfo);
		mapResCallbackInfo::iterator itr = m_mapCallbackInfo.find(strId);
		if (itr != m_mapCallbackInfo.end())
		{
			return itr->second.pResCallbackPtr;
		}
		return nullptr;
	}

	void CProducerImpl::_InsertCallbackInfo(const std::string& strId, const ResCallBackInfo& callbackInfo)
	{
		int iResMapSize = 0;
		{
			boost::lock_guard<boost::mutex> lock(m_mutexCallbackInfo);
			m_mapCallbackInfo.insert(std::make_pair(strId, callbackInfo));
			iResMapSize = m_mapCallbackInfo.size();
		}
		ICC_LOG_DEBUG(m_pLog, "_InsertCallbackInfo[%s] call back map size[%d]", strId.c_str(), iResMapSize);
	}

	void CProducerImpl::_RemoveCallbackInfo(const std::string& strId)
	{
		int iResMapSize = 0;
		{
			boost::lock_guard<boost::mutex> lock(m_mutexCallbackInfo);
			m_mapCallbackInfo.erase(strId);
			iResMapSize = m_mapCallbackInfo.size();
		}
		ICC_LOG_DEBUG(m_pLog, "_RemoveCallbackInfo[%s] call back map size[%d]", strId.c_str(), iResMapSize);
	}

	void CProducerImpl::_AllCallbackInfo(mapResCallbackInfo& mapResCallBackInfos)
	{
		boost::lock_guard<boost::mutex> lock(m_mutexCallbackInfo);
		mapResCallBackInfos = m_mapCallbackInfo;
	}

	void* CProducerImpl::_CheckCallbackExpired(void* parg)
	{
		CProducerImpl* pThis = (CProducerImpl*)parg;
		if (!pThis)
		{
			return (void*)0;
		}

		ICC_LOG_DEBUG(pThis->m_pLog, "_CheckCallbackExpired begin");

		
		while (!pThis->m_bIsStopCheckResCallbackExpired)
		{
			mapResCallbackInfo mapTmpResCallbackInfo;
			pThis->_AllCallbackInfo(mapTmpResCallbackInfo);
			
			unsigned long long u64CurrentTime = std::time(NULL);
			{				
				mapResCallbackInfo::iterator itr;
				for (itr = mapTmpResCallbackInfo.begin(); itr != mapTmpResCallbackInfo.end(); ++itr)
				{
					if (pThis->m_uDefaultCallbackExpired < (u64CurrentTime - itr->second.llBeginTime))
					{
						pThis->_RemoveCallbackInfo(itr->second.strId);						
					}
				}
			}
			
			boost::this_thread::sleep(boost::posix_time::millisec(1000));
		}	

		ICC_LOG_DEBUG(pThis->m_pLog, "_CheckCallbackExpired end");

		return (void*)0;
	}


	

} /*namespace ICC*/