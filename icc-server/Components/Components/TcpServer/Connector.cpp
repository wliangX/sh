#include "Boost.h"
#include <sstream>
//Boost
#include <boost/format.hpp>
#include <boost/bind/bind.hpp>
#include <boost/locale.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

//Project
#include "Connector.h"

Connector::Connector(IConnectionPtr p_IConnectionPtr, AsioEnginePtr p_AsioEngine) : m_IConnectionPtr(p_IConnectionPtr), m_AsioEngine(p_AsioEngine)
{
	m_pBuffer = new char[MAX_BUFFER_SIZE];
	m_BufferSize = MAX_BUFFER_SIZE;
	memset(m_pBuffer, 0, m_BufferSize);
	m_pClientSocket = boost::make_shared<boost::asio::ip::tcp::socket>(*m_AsioEngine);
}

Connector::~Connector()
{
	delete[] m_pBuffer;
	m_pBuffer = nullptr;
	m_BufferSize = 0;
}

std::string Connector::GetLocalAddress()
{
    return m_LocalAddress;
}

std::string Connector::GetRemoteAddress()
{
    return m_RemoteAddress;
}

char* Connector::GetBuffer(unsigned int& p_iSize)
{
	p_iSize = m_BufferSize;
    return m_pBuffer;
}

void Connector::SetBuffer(char* p_pBuffer, unsigned int p_iSize)
{
    delete[] m_pBuffer;
    m_pBuffer = nullptr;
    m_BufferSize = 0;
	m_pBuffer = p_pBuffer;
	m_BufferSize = p_iSize;
}

unsigned int Connector::Send(const char* p_pData, unsigned int p_ilength)
{
    if (nullptr == m_pClientSocket)
    {
        std::cerr << "send error, error message : "
            << " invalid socket..." << std::endl;
        return 0;
    }

	if (!m_pClientSocket->is_open())
	{
		std::cerr << "socket is close, error message : " << " invalid socket..." << std::endl;
		return 0;
	}

    boost::system::error_code err;
	std::size_t bytes = boost::asio::write(*m_pClientSocket, boost::asio::buffer(p_pData, p_ilength), err);
    if (err)
    {
        std::cerr << "send error, error message : "
            << boost::system::system_error(err).what() << std::endl;
        return 0;
    }
    return bytes;
}


void Connector::OnAsyncReceive(ConnectorPtr p_ConnectorPtr, const boost::system::error_code& p_err, size_t p_ibytes_transferred)
{
	unsigned int bufferSize = 0;
	if (p_err)
    {
		p_ConnectorPtr->Close();
		p_ConnectorPtr->GetIConnection()->OnDisconnected(p_ConnectorPtr->GetRemoteAddress(),
			boost::locale::conv::to_utf<char>(boost::system::system_error(p_err).what(), boost::locale::generator().generate("utf8")));
        return;
    }

	char* pBuffer = p_ConnectorPtr->GetBuffer(bufferSize);

	p_ConnectorPtr->GetIConnection()->OnReceived(pBuffer, p_ibytes_transferred);
	memset(pBuffer, 0, bufferSize);

	p_ConnectorPtr->GetSocket()->async_read_some(
        boost::asio::buffer(pBuffer, bufferSize),
		boost::bind(Connector::OnAsyncReceive,
		p_ConnectorPtr,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));

}

bool Connector::Close()
{
    if (!m_pClientSocket->is_open())
    {
        return true;
    }
    boost::system::error_code err;
    m_pClientSocket->close(err);
    if (err)
    {
        std::cerr << "close error, error message : "
            << boost::system::system_error(err).what() << std::endl;
        return false;
    }
    return true;
}

bool Connector::Named()
{
    boost::system::error_code err;
    boost::asio::ip::tcp::endpoint localEndPoint = m_pClientSocket->local_endpoint(err);
    if (err)
    {
        std::cerr << "Get Local Endpoint error, error message : " << boost::system::system_error(err).what() << std::endl;
        return false;
    }
    m_LocalAddress = AddressFromEndpoint(localEndPoint);

    boost::asio::ip::tcp::endpoint remoteEndpoint = m_pClientSocket->remote_endpoint(err);
    if (err)
    {
        std::cerr << "Get Remote Endpoint error, error message : " << boost::system::system_error(err).what() << std::endl;
        return false;
    }
    m_RemoteAddress = AddressFromEndpoint(remoteEndpoint);
    return true;
}

IConnectionPtr Connector::GetIConnection()
{
	return m_IConnectionPtr;
}

SocketPtr Connector::GetSocket()
{
    return m_pClientSocket;
}

std::string Connector::AddressFromEndpoint(const boost::asio::ip::tcp::endpoint& p_Endpoint)
{
    std::string l_addressName;
    boost::system::error_code l_err;
	boost::asio::ip::address l_addr = p_Endpoint.address();
    //std::string ip = boost::locale::conv::to_utf<char>(addr.to_string(err), boost::locale::generator().generate("utf8"));
	std::string l_strIP = l_addr.to_string(l_err);
	unsigned short l_iport = p_Endpoint.port();
	if (l_err)
    {
		std::cerr << "Get ip error, error message : " << boost::system::system_error(l_err).what() << std::endl;
		return l_addressName;
    }

	std::stringstream ss;
	ss << l_strIP << ":" << l_iport;
	return ss.str();
}