#include "Boost.h"
//boost
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
//Interface
#include "TcpServer/IConnection.h"

//Project
#include "Concurrency.h"
#include "Connector.h"
#include "EventLoop.h"

using namespace boost::placeholders;

bool ICC::Tcp::Concurrency::Open()
{
    try
    {
		for (unsigned int i = 0; i < m_SessionEngineCount; ++i)
        {
            ThreadPtr threadPtr(new boost::thread(
                boost::bind(&EventLoop::S_RunEventLoop, m_SessionEnginePtrs[i].get(), m_SessionMsg)));
            m_SessionThreadPtrs.push_back(threadPtr);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << " Concurrency create thread exception, exception message: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void ICC::Tcp::Concurrency::Close()
{
	for (unsigned int i = 0; i < m_SessionEngineCount; ++i)
    {
        m_SessionEnginePtrs[i]->stop();
		m_SessionThreadPtrs[i]->join();
    }
}

ICC::Tcp::Concurrency::Concurrency(unsigned int sessionEngineCount)
    : m_SessionEngineCount(sessionEngineCount), m_SessionMsg("sessionEngine"),
    m_SessionBalanceFactor(0)
{
	for (unsigned int i = 0; i < m_SessionEngineCount; ++i)
    {
		m_SessionEnginePtrs.push_back(boost::make_shared<AsioEngine>());
    }
}


ICC::Tcp::Concurrency::~Concurrency()
{

}

const AsioEnginePtr ICC::Tcp::Concurrency::GetBalanceAsioEnginePtr()
{
    return m_SessionEnginePtrs[(++m_SessionBalanceFactor) % m_SessionEngineCount];
}

void ICC::Tcp::Concurrency::RegisterReceive(ConnectorPtr p_ConnectorPtr)
{
	m_pSock = p_ConnectorPtr->GetSocket();
    unsigned int bufferSize = 0;
	m_pSock->async_receive(boost::asio::buffer(p_ConnectorPtr->GetBuffer(bufferSize), bufferSize),
        boost::bind(Connector::OnAsyncReceive,
		p_ConnectorPtr,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}
