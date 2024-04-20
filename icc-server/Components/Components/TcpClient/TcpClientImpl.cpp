#include "Boost.h"
//Boost
#include <boost/lexical_cast.hpp>

//Project
#include "TcpClientImpl.h"

CTcpClientImpl::CTcpClientImpl(IResourceManagerPtr p_ResourceManagerPtr) : m_Resolver(m_IoService), m_ClientSock(m_IoService), m_pResourceManager(p_ResourceManagerPtr)
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);

	m_bReconnect = true;
 	m_pBuffer = new char[TCP_RECV_BUF_SIZE];
	m_BufferSize = TCP_RECV_BUF_SIZE;
	memset(m_pBuffer, 0, TCP_RECV_BUF_SIZE);

	m_ushPort = 0;
}

CTcpClientImpl::~CTcpClientImpl()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}
	m_IoService.stop();
}

bool CTcpClientImpl::Connect(std::string p_strIp, unsigned short p_ushPort, ITcpClientCallBackPtr p_UserCallbackPtr)
{
	m_strIp = p_strIp;
	m_ushPort = p_ushPort;
	m_ITcpClientCallbackPtr = p_UserCallbackPtr;
	try
	{
		m_ThreadPtr = ThreadPtr(new boost::thread(boost::bind(&CTcpClientImpl::S_RunEventLoop, &m_IoService)));
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "create thread exception");
		return false;
	}

	try
	{

		boost::system::error_code ec;
		boost::asio::ip::tcp::resolver::query query(p_strIp, boost::lexical_cast<std::string, unsigned short>(p_ushPort));
		boost::asio::ip::tcp::resolver::iterator iter = m_Resolver.resolve(query, ec);
		boost::asio::ip::tcp::resolver::iterator end;

		if (iter != end)
		{		
			boost::asio::ip::tcp::endpoint endpoint = *iter;
				//异步连接
			m_ClientSock.async_connect(endpoint, boost::bind(&CTcpClientImpl::OnConnected, this, boost::asio::placeholders::error));
			return true;
		}

	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "async connect exception");
		return false;
	}
	
	return false;
}

/* ---------- Error Code ------------
0	Open Success

/// Already open.
already_open = 1,

/// End of file or stream.
eof = 2,

/// Element not found.
not_found = 3,

/// The descriptor cannot fit into the select system call's fd_set.
fd_set_failure = 4
*/

unsigned int CTcpClientImpl::Send(const char* p_Msg,unsigned int p_size)
{
	if (!m_ClientSock.is_open())
	{
		ICC_LOG_ERROR(m_pLog, "socket is close, invalid socket");
		return 0;
	}

	std::size_t bytes = 0;
	try
	{
		boost::system::error_code err;
		bytes = boost::asio::write(m_ClientSock, boost::asio::buffer(p_Msg, p_size), err);
		if (err)
		{
			ICC_LOG_ERROR(m_pLog, "send failed, error_code:[%d]", err.value());
			return 0;
		}
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "async write exception");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "async send message, length[%d]", p_size);
	return bytes;
}

void CTcpClientImpl::OnConnected(const boost::system::error_code& error)
{
	if (!error.value())
	{
		int l_nErrorCode = error.value();
		m_ITcpClientCallbackPtr->OnConnected(std::to_string(l_nErrorCode));
		ICC_LOG_DEBUG(m_pLog, "connected success, code:[%d]", l_nErrorCode);
	}
	try
	{
		m_ClientSock.async_read_some(boost::asio::buffer(m_pBuffer, m_BufferSize),
			boost::bind(&CTcpClientImpl::OnReceive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch (...)
	{
		std::string l_strMsg = error.message();
		ICC_LOG_ERROR(m_pLog, "async_read_some exception, error:%s", l_strMsg.c_str());
	}
}

void CTcpClientImpl::OnReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error.value())
	{
		m_ITcpClientCallbackPtr->OnReceived(m_pBuffer, bytes_transferred);
		memset(m_pBuffer, 0, TCP_RECV_BUF_SIZE);
		try
		{
			m_ClientSock.async_read_some(boost::asio::buffer(m_pBuffer, TCP_RECV_BUF_SIZE),
				boost::bind(&CTcpClientImpl::OnReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "async_read_some exception, error_code:%d", error.value());
		}
	}
	else
	{
		int l_nErrorCode = error.value();
		std::string l_strErrorMsg = error.message();
		ICC_LOG_ERROR(m_pLog, "operator exception, error_code[%d], error_msg[%s]", l_nErrorCode, l_strErrorMsg.c_str());
		m_ITcpClientCallbackPtr->OnDisconnected(std::to_string(l_nErrorCode));

		if (m_bReconnect)
		{
			InnerClose();

			try
			{
				boost::system::error_code ec;
				boost::asio::ip::tcp::resolver::query query(m_strIp, boost::lexical_cast<std::string, unsigned short>(m_ushPort));
				boost::asio::ip::tcp::resolver::iterator iter = m_Resolver.resolve(query, ec);
				boost::asio::ip::tcp::resolver::iterator end;

				if (iter != end)
				{				
					boost::asio::ip::tcp::endpoint endpoint = *iter;
					//异步连接
					m_ClientSock.async_connect(endpoint, boost::bind(&CTcpClientImpl::OnConnected, this, boost::asio::placeholders::error));
					ICC_LOG_DEBUG(m_pLog, "start reconnected [%s:%d]", m_strIp.c_str(), m_ushPort);				
				}
			}
			catch (...)
			{
				ICC_LOG_ERROR(m_pLog, "async reconnect [%s:%d] exception", m_strIp.c_str(), m_ushPort);
			}
		}
	}
}

void CTcpClientImpl::Close()
{
	try
	{
		m_bReconnect = false;

		m_ClientSock.shutdown(boost::asio::socket_base::shutdown_both);
		m_ClientSock.close();
		ICC_LOG_DEBUG(m_pLog, "acceptor close");
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "acceptor close exception");
	}
}
void CTcpClientImpl::InnerClose()
{
	//this->Close();

	m_bReconnect = true;
	boost::this_thread::sleep(boost::posix_time::millisec(3000));
}

void CTcpClientImpl::S_RunEventLoop(boost::asio::io_service* p_Engine)
{
	boost::asio::io_service::work permanence(*p_Engine);
	p_Engine->run();
}

bool CTcpClientImpl::Named()
{
	boost::system::error_code err;
	boost::asio::ip::tcp::endpoint localEndPoint = m_ClientSock.local_endpoint(err);
	if (err)
	{
		ICC_LOG_ERROR(m_pLog, "get local endpoint error, error_code[%d]", err.value());
		return false;
	}
	
	boost::asio::ip::tcp::endpoint remoteEndpoint = m_ClientSock.remote_endpoint(err);
	if (err)
	{
		ICC_LOG_ERROR(m_pLog, "Get Remote Endpoint error, error_code[%d]", err.value());
		return false;
	}

	return true;
}

IResourceManagerPtr CTcpClientImpl::GetResourceManager()
{
	return m_pResourceManager;
}