#include "ResCallbackImp.h"
#include <iostream>
#include <Log/ILogFactory.h>

namespace ICC
{

	CResCallbackImp::CResCallbackImp(ICC::Response Msg, Log::ILogPtr pLog) : m_Response(Msg), m_pLog(pLog)
	{
	}

	CResCallbackImp::~CResCallbackImp()
	{
		//请求应答模式每一个应答的目的都是临时队列，临时队列在服务端是通过复制获得，在应答完成后需要释放，否则会内存泄露
		if (m_Response.msg.pCMSReplyTo)
		{
			delete m_Response.msg.pCMSReplyTo;
			m_Response.msg.pCMSReplyTo = nullptr;
		}
	}

	void CResCallbackImp::OnResponse(std::string strMsg)
	{
		if (m_Response.pSession && m_Response.pProducer)
		{
			try
			{
				shared_ptr<BytesMessage> pMsg(m_Response.pSession->createBytesMessage((const unsigned char*)strMsg.c_str(), strMsg.length()));
				if (pMsg)
				{
					pMsg->setCMSCorrelationID(m_Response.msg.strCMSCorrelationID);
					//应答往临时队列回复消息，设置默认消息级别，消息生存周期永不过期
					//如果对端连接断开临时队列销毁则无法再往临时队列回复消息，不会导致消息堆积
					m_Response.pProducer->send(m_Response.msg.pCMSReplyTo, pMsg.get(), DeliveryMode::NON_PERSISTENT, m_Response.nCMSPriority, m_Response.lCMSExpiration);
				}
				
				cms::TemporaryQueue *pTempQueue = dynamic_cast<cms::TemporaryQueue*>(m_Response.msg.pCMSReplyTo);
				if (pTempQueue)
				{
					//ICC_LOG_LOWDEBUG(m_pLog, "CMS[Reply msg(%s)\nto tempQueue %s\nCMSCorrelationID:%s]", strMsg.c_str(), pTempQueue->getQueueName().c_str(), m_Response.msg.strCMSCorrelationID.c_str());
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
	}

} /*namespace ICC*/