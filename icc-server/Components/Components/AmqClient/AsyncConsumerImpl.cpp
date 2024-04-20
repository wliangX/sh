#include "Boost.h"
#include "AsyncConsumerImpl.h"
#include "ResCallbackImp.h"

namespace ICC 
{
	CAsyncConsumerImpl::CAsyncConsumerImpl(IResourceManagerPtr pResourceManager) 
		: m_pConnection(nullptr)
		, m_pAsyncsession(nullptr)
		, m_pProducer(nullptr)
		, m_bexitThread(false)
	{
		// TODO Auto-generated constructor stub
		m_pJsonFty = ICCGetResourceEx(JsonParser::IJsonFactory, ICCIJsonFactoryResourceName, pResourceManager);
		m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, pResourceManager)->GetLogger(MODULE_NAME);
		m_pConfig = ICCGetResourceEx(Config::IConfigFactory, ICCIConfigFactoryResourceName, pResourceManager)->CreateConfig();
		m_pString = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, pResourceManager)->CreateString();
		m_nCMSPriority = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/AmqClient/CMSPriority", "4"));
		m_lCMSExpiration = m_pString->ToInt64(m_pConfig->GetValue("ICC/Component/AmqClient/CMSExpiration", "60000"));
		m_bConsumerExpiryCheckEnabled = m_pString->ToUShort(m_pConfig->GetValue("ICC/Component/AmqClient/IsConsumerExpiryCheckEnabled", "0"));
		m_intTimedWait = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/AmqClient/TimedWait", "1"));
		if (m_intTimedWait < 0)
		{
			m_intTimedWait = 0;
		}
		m_iMsgTotalCount = 0;

		this->Initialize();
		this->Start();

		ICC_LOG_INFO(m_pLog, "CAsyncConsumerImpl constructor");
	}

	CAsyncConsumerImpl::~CAsyncConsumerImpl() 
	{
		// TODO Auto-generated destructor stub
		m_bexitThread = true;
		m_cvaMsg.notify_all();
		m_callbackThread[0]->join();
		m_callbackThread[1]->join();
		this->Cleanup();

		ICC_LOG_INFO(m_pLog, "CAsyncConsumerImpl destructor");
	}

	bool CAsyncConsumerImpl::Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName /*= std::string()*/, std::string strPassword /*= std::string()*/)
	{
		try
		{
			shared_ptr<ConnectionFactory> connectionFactory(ConnectionFactory::createCMSConnectionFactory(strBrokerURI));
			// Create a Connection
			if (nullptr == m_pConnection && connectionFactory)
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
					strClientID.append(".Consumer");
					m_pConnection->setClientID(strClientID);
				}
			}
			if (m_pConnection)
			{
				m_pConnection->start();
				m_pConnection->setExceptionListener(this);
				m_pAsyncsession = m_pConnection->createSession(Session::AUTO_ACKNOWLEDGE);
				if (m_pAsyncsession)
				{
					m_pProducer = m_pAsyncsession->createProducer();
				}
				strClientID = m_pConnection->getClientID();
				//设置消费者到期检查启用开关
				activemq::core::ActiveMQConnection* pAMQConnection = dynamic_cast<activemq::core::ActiveMQConnection*>(m_pConnection);
				if (pAMQConnection)
				{
					pAMQConnection->setConsumerExpiryCheckEnabled(m_bConsumerExpiryCheckEnabled);
				}
				if (m_pAsyncsession)
				{
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

	bool CAsyncConsumerImpl::Subscribe(std::string strDest, int iMode, IAsyncCallbackPtr pCallback, const std::string& strSelector /*= string()*/)
	{
		try
		{
			itConsumer it = m_mapConsumer[iMode].find(strDest);
			if (it == m_mapConsumer[iMode].end())
			{
				if (m_pAsyncsession)
				{
					PConsumer pConsumer = new Consumer;
					if (pConsumer)
					{
						//主题
						if (Destination::TOPIC == iMode)
						{
							pConsumer->pDestination = m_pAsyncsession->createTopic(strDest);
						}
						else if (Destination::QUEUE == iMode)//队列
						{
							pConsumer->pDestination = m_pAsyncsession->createQueue(strDest);
						}
						//过滤
						if (!strSelector.empty())
						{
							pConsumer->pConsumer = m_pAsyncsession->createConsumer(pConsumer->pDestination, strSelector);
						}
						else//不过滤
						{
							pConsumer->pConsumer = m_pAsyncsession->createConsumer(pConsumer->pDestination);
						}
						//设置监听器
						if (pConsumer->pConsumer)
						{
							pConsumer->pConsumer->setMessageListener(this);
						}
						pConsumer->pCallback = pCallback;
						m_mapConsumer[iMode].insert(make_pair(strDest, pConsumer));

						return true;
					}
				}
			}
			else
			{
				string strDestType = (iMode == Destination::TOPIC ? "Topic" : "Queue");
				ICC_LOG_INFO(m_pLog, "CMS[%s %s has subscribed by this CMS client.]", strDestType.c_str(), strDest.c_str());
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

	bool CAsyncConsumerImpl::Unsubscribe(std::string strDest, int iMode)
	{
		try
		{
			itConsumer iter = m_mapConsumer[iMode].find(strDest);
			if (iter != m_mapConsumer[iMode].end())
			{
				//删除目的
				delete iter->second->pDestination;
				iter->second->pDestination = NULL;
				//删除消费者
				delete iter->second->pConsumer;
				iter->second->pConsumer = NULL;
				//删除消费者结构体
				delete iter->second;
				iter->second = NULL;
				//删除map中成员
				m_mapConsumer[iMode].erase(strDest);

				return true;
			}
			else
			{
				ICC_LOG_INFO(m_pLog, "CMS[Unsubscribe a not exist Destination %s]", strDest.c_str());
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

	bool CAsyncConsumerImpl::DurableSubscribe(std::string strTopic, std::string strName, std::string strSelector, IAsyncCallbackPtr pCallback, bool bNoLocal /*= false*/)
	{
		try
		{
			itConsumer it = m_mapConsumer[Destination::TOPIC].find(strTopic);
			if (it == m_mapConsumer[Destination::TOPIC].end())
			{
				if (m_pAsyncsession)
				{
					PConsumer pConsumer = new Consumer;
					if (pConsumer)
					{
						pConsumer->pDestination = m_pAsyncsession->createTopic(strTopic);
						pConsumer->pConsumer = m_pAsyncsession->createDurableConsumer((Topic*)pConsumer->pDestination, strName, strSelector, bNoLocal);
						if (pConsumer->pConsumer)
						{
							pConsumer->pConsumer->setMessageListener(this);
						}
						pConsumer->pCallback = pCallback;
						m_mapConsumer[Destination::TOPIC].insert(make_pair(strTopic, pConsumer));
						m_mapDurable.insert(make_pair(strName, strTopic));

						return true;
					}
				}
			}
			else
			{
				ICC_LOG_INFO(m_pLog, "CMS[Durable Topic %s has subscribed by this CMS client.]", strTopic.c_str());
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

	bool CAsyncConsumerImpl::Unsubscribe(std::string strName)
	{
		try
		{
			itDurable it = m_mapDurable.find(strName);
			if (it != m_mapDurable.end())
			{
				//取消订阅
				if (m_pAsyncsession)
				{
					m_pAsyncsession->unsubscribe(strName);
				}
				//清理资源
				if (!Unsubscribe(it->second, Destination::TOPIC))
				{
					return false;
				}
				m_mapDurable.erase(it);

				return true;
			}
			else
			{
				ICC_LOG_INFO(m_pLog, "CMS[Unsubscribe a not exist name %s.]", strName.c_str());
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

	void CAsyncConsumerImpl::onMessage(const cms::Message* message)
	{
		try
		{
			string strDestName;
			string strMsg;
			const Destination* pDest = message->getCMSDestination();
			Destination::DestinationType type = pDest->getDestinationType();
			const cms::Destination* pCMSReplyTo = message->getCMSReplyTo();
			string strCMSCorrelationID = message->getCMSCorrelationID();
			if (cms::Destination::TOPIC == type)//topic
			{
				strDestName = ((cms::Topic*)pDest)->getTopicName();
			}
			else if (cms::Destination::QUEUE == type)//queue
			{
				strDestName = ((cms::Queue*)pDest)->getQueueName();
			}

			//连接/断开公告主题
			if (!strDestName.compare(Activemq_Advisory_Connection))
			{
				const activemq::commands::ActiveMQMessage* pAmqMsg = (activemq::commands::ActiveMQMessage*)(message);
				if (pAmqMsg)
				{
					//decaf::lang::Pointer<activemq::commands::DataStructure> dataStruct = pAmqMsg->getDataStructure();
					decaf::lang::Pointer<activemq::commands::Command> cmd = (pAmqMsg->getDataStructure().dynamicCast<activemq::commands::Command>());
					if (cmd->isConnectionInfo())//连接通知
					{
						decaf::lang::Pointer<activemq::commands::ConnectionInfo> info = cmd.dynamicCast<activemq::commands::ConnectionInfo>();
						strMsg = ConnectionInfo2Json(info);
					}
					else if (cmd->isRemoveInfo())//断开通知
					{
						decaf::lang::Pointer<activemq::commands::RemoveInfo> info = cmd.dynamicCast<activemq::commands::RemoveInfo>();
						strMsg = RemoveInfo2Json(info);
					}
				}
			}
			//其他广告主题用于定位问题,消息广告默认不启用，如需启用需要配置
			//比如ActiveMQ.Advisory.Expired.Queue、ActiveMQ.Advisory.MessageConsumed、ActiveMQ.Advisory.MessageDelivered等主题
			else if (string::npos != strDestName.find(Activemq_Advisory))
			{
				const activemq::commands::ActiveMQMessage* pAmqMsg = (activemq::commands::ActiveMQMessage*)(message);
				if (nullptr == pAmqMsg) return;
				strMsg = pAmqMsg->toString();
			}
			else
			{
				const BytesMessage* bytesMessage = dynamic_cast<const BytesMessage*>(message);
				if (bytesMessage)
				{
					unsigned int len = bytesMessage->getBodyLength();
					unsigned char* pRecvBytes = new unsigned char[len + 1]();
					bytesMessage->readBytes(pRecvBytes, len);
					strMsg = (char*)pRecvBytes;
					delete [] pRecvBytes;
				}
			}

			//ICC_LOG_DEBUG(m_pLog, "CMS receive msg [%s]", strMsg.c_str());

			ConsumerMsg  msg;
			msg.iMode = type;
			msg.strDest = strDestName;
			msg.strMsg = strMsg;	

			message->acknowledge();

			if (nullptr == pCMSReplyTo)
			{	
				m_mutexMsg.lock();
				m_lMsg[0].push_back(msg);
				m_mutexMsg.unlock();
				m_cvaMsg.notify_all();
			}
			else
			{
				msg.pCMSReplyTo = pCMSReplyTo->clone();
				msg.strCMSCorrelationID = strCMSCorrelationID;
				m_mutexMsg.lock();
				m_lMsg[1].push_back(msg);
				m_mutexMsg.unlock();
				m_cvaMsg.notify_all();			
			}

			if (m_iMsgTotalCount >= 0x7fffffff)
			{
				m_iMsgTotalCount = 0;
			}
			m_iMsgTotalCount++;
			ICC_LOG_DEBUG(m_pLog, "CMS receive msg total count [%d]", m_iMsgTotalCount);
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

	void CAsyncConsumerImpl::onException(const CMSException& ex AMQCPP_UNUSED)
	{
		ICC_LOG_ERROR(m_pLog, "CMS[%s]", ex.getStackTraceString().c_str());
	}

	void* CAsyncConsumerImpl::ThreadFunc(void *arg)
	{
		CAsyncConsumerImpl* pThis = (CAsyncConsumerImpl*)arg;
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
			
			ConsumerMsg msg;
			std::list<ConsumerMsg>::iterator itMsg = pThis->m_lMsg[0].begin();
			while (itMsg != pThis->m_lMsg[0].end())
			{
				msg = *itMsg;
				itConsumer it = pThis->m_mapConsumer[msg.iMode].find(msg.strDest);
				if (it != pThis->m_mapConsumer[msg.iMode].end())
				{
					if (it->second->pCallback)
					{
						it->second->pCallback->OnMessage(msg.strMsg);
					}
				}

				itMsg++;
			}
			pThis->m_lMsg[0].clear();
			try
			{
				pThis->m_mutexMsg.unlock();
			}			
			catch (...)
			{
			}
			
		}

		return (void*)0;
	}

	void* CAsyncConsumerImpl::ThreadFuncEx(void *arg)
	{
		CAsyncConsumerImpl* pThis = (CAsyncConsumerImpl*)arg;
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
			
			
			ConsumerMsg msg;
			std::list<ConsumerMsg>::iterator itMsg = pThis->m_lMsg[1].begin();
			while (itMsg != pThis->m_lMsg[1].end())
			{
				msg = *itMsg;
				itConsumer it = pThis->m_mapConsumer[msg.iMode].find(msg.strDest);
				if (it != pThis->m_mapConsumer[msg.iMode].end())
				{
					if (it->second->pCallback)
					{
						string strTempQueueName;
						cms::TemporaryQueue *pTempQueue = dynamic_cast<cms::TemporaryQueue*>(msg.pCMSReplyTo);
						if (pTempQueue)
						{
							strTempQueueName = pTempQueue->getQueueName();
						}
						
						//ICC_LOG_INFO(pThis->m_pLog, "CMS[Recv msg:%s\nneed reply to tempQueue:%s\nset CMSCorrelationID:%s]", msg.strMsg.c_str(), strTempQueueName.c_str(), msg.strCMSCorrelationID.c_str());

						ICC::Response Res;
						Res.msg = *itMsg;
						Res.pSession = pThis->m_pAsyncsession;
						Res.pProducer = pThis->m_pProducer;
						Res.nCMSPriority = pThis->m_nCMSPriority;
						Res.lCMSExpiration = pThis->m_lCMSExpiration;
						it->second->pCallback->OnMessage(msg.strMsg, boost::make_shared<CResCallbackImp>(Res, pThis->m_pLog));
					}
				}

				itMsg++;
			}
			pThis->m_lMsg[1].clear();

			try
			{
				pThis->m_mutexMsg.unlock();
			}			
			catch (...)
			{
			}

			
		}

		return (void*)0;
	}

	void CAsyncConsumerImpl::Initialize()
	{
		for (int idx = 0; idx < ICC_DEST_TYPE; idx++)
		{
			m_mapConsumer[idx].clear();
		}
		m_mapDurable.clear();
	}

	void CAsyncConsumerImpl::Cleanup()
	{
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
				itConsumer it = m_mapConsumer[idx].begin();
				while (it != m_mapConsumer[idx].end()) 
				{
					delete it->second->pDestination;
					it->second->pDestination = nullptr;
					delete it->second->pConsumer;
					it->second->pConsumer = nullptr;
					delete it->second;
					it++;
				}
			}
			if (m_pProducer)
			{
				delete m_pProducer;
				m_pProducer = nullptr;
			}
			if (m_pAsyncsession)
			{
				delete m_pAsyncsession;
				m_pAsyncsession = nullptr;
			}
			if (m_pConnection)
			{
				delete m_pConnection;
				m_pConnection = nullptr;
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
	}

	void CAsyncConsumerImpl::Start()
	{
		m_callbackThread[0].reset(new boost::thread(boost::bind(&ThreadFunc, this)));
		m_callbackThread[1].reset(new boost::thread(boost::bind(&ThreadFuncEx, this)));
	}

	string CAsyncConsumerImpl::ConnectionInfo2Json(decaf::lang::Pointer<activemq::commands::ConnectionInfo> &info)
	{
		string strJson;
		if (info != nullptr)
		{
			string strConnectionId;
			Pointer<activemq::commands::ConnectionId> pConnection = info->getConnectionId();
			if (pConnection != nullptr)
			{
				strConnectionId = pConnection->toString();
			}
			string strClientId = info->getClientId();
			string strClientIp = info->getClientIp();

			JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
			if (pJson)
			{
				pJson->SetNodeValue("/header/cmd", "ConnectionInfo");
				pJson->SetNodeValue("/header/ConnectionId", strConnectionId);
				pJson->SetNodeValue("/header/ClientId", strClientId);
				pJson->SetNodeValue("/header/ClientIp", strClientIp);

				strJson = pJson->ToString();
			}
		}

		return strJson;
	}

	string CAsyncConsumerImpl::RemoveInfo2Json(decaf::lang::Pointer<activemq::commands::RemoveInfo> &info)
	{
		string strJson;
		if (info != nullptr)
		{
			string strConnectionId;
			Pointer<activemq::commands::DataStructure> pObjId = info->getObjectId();
			if (pObjId != nullptr)
			{
				strConnectionId = pObjId->toString();
			}

			JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
			if (pJson)
			{
				pJson->SetNodeValue("/header/cmd", "RemoveInfo");
				pJson->SetNodeValue("/header/ConnectionId", strConnectionId);

				strJson = pJson->ToString();
			}
		}

		return strJson;
	}

} /*namespace ICC*/