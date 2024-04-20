#include "Boost.h"
#include "UdpImpl.h"
#include <boost/thread.hpp>

CUdpImpl::CUdpImpl(IResourceManagerPtr p_ResourceManagerPtr) : m_pSockUDP(nullptr), m_pResourceManager(p_ResourceManagerPtr)
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pBuffer = new char[UDP_RECV_BUF_SIZE];
	memset(m_pBuffer, 0, UDP_RECV_BUF_SIZE);
	m_BufferSize = UDP_RECV_BUF_SIZE;
}

CUdpImpl::~CUdpImpl()
{

	m_IoService.stop();

	if (m_pThread)
	{
		m_pThread->join();
	}
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}
}

void CUdpImpl::Bind(unsigned short p_ushPort, IUdpCallBackPtr p_IUdpCallBackPtr)
{	
	if (!p_IUdpCallBackPtr)
	{
		ICC_LOG_ERROR(m_pLog, "udp callbackptr is null!!!!!!");
		return;
	}

	try
	{
		m_pSockUDP = boost::make_shared<boost::asio::ip::udp::socket>(m_IoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), p_ushPort));

		boost::system::error_code err;
		m_pSockUDP->set_option(boost::asio::socket_base::reuse_address(true), err);
		if (err)
		{
			ICC_LOG_ERROR(m_pLog, "set reuse_address error in set_option, error message:[%s]", boost::system::system_error(err).what());
			p_IUdpCallBackPtr->OnReport(UDP_RESULT_UDPERROR, boost::system::system_error(err).what());
			return ;
		}

		m_pThread = boost::make_shared<boost::thread>(boost::bind(&CUdpImpl::S_RunEventLoop, &m_IoService));
	}
	catch (...)
	{
		try
		{
			ICC_LOG_ERROR(m_pLog, "bind port[%d] failed error:[%s]", p_ushPort, boost::current_exception_diagnostic_information().c_str());
			p_IUdpCallBackPtr->OnReport(UDP_RESULT_UDPERROR, boost::current_exception_diagnostic_information());
		}		
		catch (...)
		{
		}
		
		return;
	}

	m_IUdpCallBackPtr = p_IUdpCallBackPtr;
	m_IUdpCallBackPtr->OnReport(UDP_RESULT_SUCCESS, "success!!");

	try
	{
		m_pSockUDP->async_receive_from(
			boost::asio::buffer(m_pBuffer, m_BufferSize), m_endpointRemote,
			boost::bind(&CUdpImpl::OnReceive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch (...)
	{
		try
		{
			ICC_LOG_FATAL(m_pLog, "async receive failed error:[%s]", boost::current_exception_diagnostic_information().c_str());
		}		
		catch (...)
		{
		}
		
		return;
	}
}

void CUdpImpl::OnReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error || error == boost::asio::error::message_size)
    {		
		std::string l_strRemoteIP = m_endpointRemote.address().to_v4().to_string();
		size_t l_iPort = m_endpointRemote.port();
		m_IUdpCallBackPtr->OnReceived(m_pBuffer, bytes_transferred, l_strRemoteIP, l_iPort);

		ICC_LOG_DEBUG(m_pLog, "receive message from ip[%s] port[%d], size[%d]", l_strRemoteIP.c_str(), l_iPort, bytes_transferred);
    }
	else
	{
		ICC_LOG_ERROR(m_pLog, "receive failed error:[%s]", boost::system::system_error(error).what());

		if (error.value() == boost::system::errc::errc_t::io_error) //IO操作终止
		{
			return;
		}
	}

	//接收下一次的信息。
	memset(m_pBuffer, 0, UDP_RECV_BUF_SIZE);

	try
	{
		m_pSockUDP->async_receive_from(
			boost::asio::buffer(m_pBuffer, m_BufferSize), m_endpointRemote,
			boost::bind(&CUdpImpl::OnReceive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch (...)
	{
		try
		{
			ICC_LOG_FATAL(m_pLog, "async receive failed error:[%s]", boost::current_exception_diagnostic_information().c_str());
		}		
		catch (...)
		{
		}
		
		return;
	}
}

unsigned int CUdpImpl::Send(std::string p_strMsg, std::string p_strDestIp, unsigned short p_ushDestPort)
{
	if (nullptr == m_pSockUDP)
	{
		ICC_LOG_ERROR(m_pLog, "send error, invalid socket");
		return 0;
	}

	if (!m_pSockUDP->is_open())
	{
		ICC_LOG_ERROR(m_pLog, "socket is close, error message invalid socket");
		return 0;
	}

	unsigned int l_ibytes = 0;
	try
	{
		boost::asio::ip::udp::endpoint SendPoint(boost::asio::ip::address::from_string(p_strDestIp), p_ushDestPort);
		l_ibytes = m_pSockUDP->send_to(boost::asio::buffer(p_strMsg, p_strMsg.size()), SendPoint);
	}
	catch (...)
	{
		try
		{
			ICC_LOG_ERROR(m_pLog, "async receive failed error:[%s]", boost::current_exception_diagnostic_information().c_str());
		}		
		catch (...)
		{
		}
		
	}

	return l_ibytes;
}

unsigned int CUdpImpl::Send(char* p_pbuf, unsigned int p_ibuffLen, std::string p_strDestIp, unsigned short p_ushDestPort)
{
	if (nullptr == m_pSockUDP)
	{
		ICC_LOG_ERROR(m_pLog, "send error, invalid socket");
		return 0;
	}

	if (!m_pSockUDP->is_open())
	{
		ICC_LOG_ERROR(m_pLog, "socket is close, error message invalid socket");
		return 0;
	}

	unsigned int l_ibytes = 0;
	try
	{
		boost::asio::ip::udp::endpoint SendPoint(boost::asio::ip::address::from_string(p_strDestIp), p_ushDestPort);
		l_ibytes = m_pSockUDP->send_to(boost::asio::buffer(p_pbuf, p_ibuffLen), SendPoint);
	}
	catch (...)
	{
		try
		{
			ICC_LOG_ERROR(m_pLog, "async receive failed error:[%s]", boost::current_exception_diagnostic_information().c_str());
		}		
		catch (...)
		{
		}
		
	}

	return l_ibytes;
}

void CUdpImpl::Close()
{
    try
    {
		ICC_LOG_DEBUG(m_pLog, "close udp begin");
		if (m_pSockUDP && m_pSockUDP->is_open())
		{
			m_pSockUDP->shutdown(boost::asio::ip::udp::socket::shutdown_both);
			m_pSockUDP->close();
		}	
		ICC_LOG_DEBUG(m_pLog, "close udp end");
    }
    catch (...)
    {
		try
		{
			ICC_LOG_ERROR(m_pLog, "close exception, exception error:[%s]", boost::current_exception_diagnostic_information().c_str());
		}		
		catch (...)
		{
		}
		
    }
}

void CUdpImpl::S_RunEventLoop(boost::asio::io_service* p_Engine)
{
	boost::asio::io_service::work permanence(*p_Engine);
	p_Engine->run();
}

IResourceManagerPtr CUdpImpl::GetResourceManager()
{
	return m_pResourceManager;
}