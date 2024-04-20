#include "Boost.h"
//Boost
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/locale.hpp>
//Project
#include "EffectiveAcceptor.h"
#include "Connector.h"
#include "EventLoop.h"

EffectiveAcceptor::EffectiveAcceptor(Log::ILogPtr p_pILog) : m_pLog(p_pILog),
m_AcceptionMsg("AcceptionEngine"),
m_iPort(0)
{

}

EffectiveAcceptor::~EffectiveAcceptor()
{
    Close();
}

bool EffectiveAcceptor::Listen(std::string p_strIp, unsigned short p_ushPort, unsigned int p_ThreadCount, IConnectionCreaterPtr p_ConnectionCreaterPtr)
{
	m_strIP = p_strIp;
	m_iPort = p_ushPort;
	m_ConnectionCreaterPtr = p_ConnectionCreaterPtr;

	try
	{
		m_AcceptorPtr = boost::make_shared<boost::asio::ip::tcp::acceptor>(m_AcceptionEngine);
		m_ConcurrencyPtr = boost::make_shared<Concurrency>(p_ThreadCount);
	}	
	catch (...)
	{
		return false;
	}	

    boost::system::error_code err;
	boost::asio::ip::address addr = boost::asio::ip::address::from_string(boost::locale::conv::from_utf(p_strIp, boost::locale::generator().generate("utf8")), err);
    if (err)
    {
		ICC_LOG_ERROR(m_pLog, "ip address error, error message:[%s]", boost::system::system_error(err).what());
        return false;
    }

	boost::asio::ip::tcp::endpoint endPoint(addr, m_iPort);
	m_AcceptorPtr->open(boost::asio::ip::tcp::v4(), err);
    if (err)
    {
		ICC_LOG_ERROR(m_pLog, "acceptor error in open, error message:[%s]", boost::system::system_error(err).what());
        return false;
    }

	m_AcceptorPtr->set_option(boost::asio::socket_base::reuse_address(true), err);
    if (err)
    {
		ICC_LOG_ERROR(m_pLog, "acceptor error in set_option, error message:[%s]", boost::system::system_error(err).what());
        return false;
    }

	m_AcceptorPtr->bind(endPoint, err);
    if (err)
    {
		ICC_LOG_ERROR(m_pLog, "acceptor error in bind, error message:[%s]", boost::system::system_error(err).what());
        return false;
    }

	if (!m_ConcurrencyPtr->Open())
    {
		ICC_LOG_ERROR(m_pLog, "acceptor error in concurrency opening");
        return false;
    }

    try
    {
        m_AcceptionThreadPtr = ThreadPtr(new boost::thread(
            boost::bind(&EventLoop::S_RunEventLoop, &m_AcceptionEngine, m_AcceptionMsg)));
    }
    catch (...)
    {
		ICC_LOG_ERROR(m_pLog, "effectiveacceptor create thread exception");
        return false;
    }

	m_AcceptorPtr->listen(boost::asio::socket_base::max_connections, err);
    if (err)
    {
		ICC_LOG_ERROR(m_pLog, "acceptor error in listen, error message:[%s]", boost::system::system_error(err).what());
        return false;
    }

    AsyncAccept();

    return true;
}

void EffectiveAcceptor::Close()
{
    m_AcceptionEngine.stop();
    if (m_AcceptionThreadPtr)
    {
		m_AcceptionThreadPtr->join();
		m_AcceptionThreadPtr = nullptr;
    }

    try
    {
        m_AcceptorPtr->close();
    }
    catch (...)
    {
		ICC_LOG_ERROR(m_pLog, "acceptor close exception");
    }

	m_ConcurrencyPtr->Close();
}

unsigned int EffectiveAcceptor::Send(std::string p_strClientTag, const char* p_pData, unsigned int p_ilength)
{
	return m_ConnectorMap[p_strClientTag]->Send(p_pData, p_ilength);
}

void EffectiveAcceptor::AsyncAccept()
{
	AsioEnginePtr l_asioEnginePtr = m_ConcurrencyPtr->GetBalanceAsioEnginePtr();
	IConnectionPtr l_ConnectionPtr = m_ConnectionCreaterPtr->CreateConnection();
	ConnectorPtr l_ConnectorPtr = boost::make_shared<Connector>(l_ConnectionPtr, l_asioEnginePtr);
	m_AcceptorPtr->async_accept(*l_ConnectorPtr->GetSocket(), boost::bind(&EffectiveAcceptor::S_AsyncAccept, this, l_ConnectorPtr, boost::asio::placeholders::error));
}

void EffectiveAcceptor::S_AsyncAccept(EffectiveAcceptor *p_pEffectiveAcceptor, ConnectorPtr p_ConnectorPtr, const boost::system::error_code& p_Err)
{
	try
	{
		p_pEffectiveAcceptor->AsyncAccept();
		if (p_Err)
		{
			return;
		}
		else
		{
			if (p_ConnectorPtr->Named())
			{
				std::string l_strClientTag = p_ConnectorPtr->GetRemoteAddress();
				if (p_pEffectiveAcceptor->m_ConnectorMap[l_strClientTag] == ConnectorPtr())
				{
					p_pEffectiveAcceptor->m_ConnectorMap[l_strClientTag] = p_ConnectorPtr;
				}

				p_pEffectiveAcceptor->m_ConcurrencyPtr->RegisterReceive(p_ConnectorPtr);
				p_ConnectorPtr->GetIConnection()->OnAccepted(l_strClientTag);
			}
			else
			{
				p_ConnectorPtr->Close();
			}
		}
	}	
	catch (...)
	{
	}
	
}
