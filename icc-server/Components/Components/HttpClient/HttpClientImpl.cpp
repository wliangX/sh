#include "Boost.h"
#include "HttpClientImpl.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include "SimpHttpClient.h"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

#define HTTP_SUCCESS  0
#define HTTP_FAILED   -1

#define HTTP_ERROR_UNKNOWN  "http unknown error"
#define HTTP_ERROR_HEAD  "http header error"
#define HTTP_ERROR_VERSION  "http version error"

CHttpClientImpl::CHttpClientImpl(IResourceManagerPtr p_ResourceManagerPtr) :m_resolver(m_IoService), m_socket(m_IoService), m_iHttpErrorCode(HTTP_SUCCESS)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_ResourceManagerPtr)->GetLogger(MODULE_NAME);
}

CHttpClientImpl::~CHttpClientImpl()
{
	m_multiHeaders.clear();

	ClearActiveUrlList();
}

std::string CHttpClientImpl::GetEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage)
{
	ICC_LOG_DEBUG(m_pLog, "GetEx begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());

	std::string strResult;

	try
	{
		int version = 11;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());

		//设置30s后超时
		stream.expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		stream.connect(results);


		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::get, strTarget.c_str(), version };
		req.set(http::field::host, strServerPort.c_str());
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			req.set(itr_const->first, itr_const->second);
		}

		if (!strContent.empty())
		{
			req.body() = strContent;
		    req.prepare_payload();
		}

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		strResult = res.body().c_str();

		//设置超时结束
		stream.expires_never();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{			
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "GetEx end ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());

	return strResult;
}

bool CHttpClientImpl::GetLocalIP(const std::string& strServerIp, const std::string& strServerPort, std::string& strErrorMessage, std::string& strLocalIp)
{
	ICC_LOG_DEBUG(m_pLog, "GetLocalIP begin ip[%s:%s]\n",strServerIp.c_str(), strServerPort.c_str());

	bool bResult = false;
	try
	{
		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());

		//设置30s后超时
		stream.expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		stream.connect(results);

		strLocalIp = stream.socket().local_endpoint().address().to_string();

		bResult = true;

		//设置超时结束
		stream.expires_never();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "GetLocalIP end ip[%s:%s]\n", strServerIp.c_str(), strServerPort.c_str());
	return bResult;
}

std::string CHttpClientImpl::GetExEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp)
{
	ICC_LOG_DEBUG(m_pLog, "GetExEx begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	std::string strResult;

	try
	{
		int version = 11;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());

		//设置30s后超时
		stream.expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		stream.connect(results);


		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::get, strTarget.c_str(), version };
		req.set(http::field::host, strServerPort.c_str());
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			req.set(itr_const->first, itr_const->second);
		}

		if (!strContent.empty())
		{
			req.body() = strContent;
			req.prepare_payload();
		}

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		strResult = res.body().c_str();

		strLocalIp = stream.socket().local_endpoint().address().to_string();

		//设置超时结束
		stream.expires_never();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}

	ICC_LOG_DEBUG(m_pLog, "GetExEx end ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());

	return strResult;
}

std::string CHttpClientImpl::PostEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage)
{
	ICC_LOG_DEBUG(m_pLog, "PostEx begin ip[%s:%s]\nstrTarget[%s]\n", 
		strServerIp.c_str(),strServerPort.c_str(), strTarget.c_str());

	std::string strResult;

	try
	{
		int version = 11;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());

		//设置30s后超时
		stream.expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		stream.connect(results);
		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::post, strTarget.c_str(), version };
		req.set(http::field::host, strServerIp.c_str());
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(http::field::content_type, "application/json");

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			req.set(itr_const->first, itr_const->second);
		}	

		if (!strContent.empty())
		{
			req.body() = strContent;
			req.prepare_payload();
		}

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		strResult = res.body().c_str();

		stream.expires_never();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "PostEx end ip[%s:%s]\nstrTarget[%s]\nstrResult[%s]", 
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str(),  strResult.c_str());
	return strResult;
}

std::string CHttpClientImpl::GetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout)
{
	ICC_LOG_DEBUG(m_pLog, "GetWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());

	std::string strResult;

	try
	{
		net::io_context ioc;
		std::shared_ptr<CMyHttpClientImp> pClient = std::make_shared<CMyHttpClientImp>(ioc);
		if (!pClient)
		{
			return "";
		}

		pClient->run(strServerIp, strServerPort, 11, (int)http::verb::get, strTarget, mapHeaders, strContent, uTimeout, m_pLog);
		ioc.run();

		strResult = pClient->content();
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "GetWithTimeout end ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	return strResult;
}

std::string CHttpClientImpl::PostWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout)
{
	ICC_LOG_DEBUG(m_pLog, "PostWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	std::string strResult;

	try
	{
		net::io_context ioc;
		std::shared_ptr<CMyHttpClientImp> pClient = std::make_shared<CMyHttpClientImp>(ioc);
		if (!pClient)
		{
			return "";
		}

		pClient->run(strServerIp, strServerPort, 11, (int)http::verb::post, strTarget, mapHeaders, strContent, uTimeout, m_pLog);
		ioc.run();

		strResult = pClient->content();
	}	
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "PostWithTimeout end ip[%s:%s]\nstrTarget[%s]\nstrResult[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str(), strResult.c_str());
	return strResult;
}

std::string CHttpClientImpl::DeletetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout)
{
	ICC_LOG_DEBUG(m_pLog, "DeletetWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	std::string strResult;

	try
	{
		net::io_context ioc;
		std::shared_ptr<CMyHttpClientImp> pClient = std::make_shared<CMyHttpClientImp>(ioc);
		if (!pClient)
		{
			return "";
		}

		pClient->run(strServerIp, strServerPort, 11, (int)http::verb::delete_, strTarget, mapHeaders, strContent, uTimeout, m_pLog);
		ioc.run();

		strResult = pClient->content();
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "DeletetWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str(), strContent.c_str());
	return strResult;
}

std::string CHttpClientImpl::GetExWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp, unsigned int uTimeout)
{
	ICC_LOG_DEBUG(m_pLog, "GetExWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());

	std::string strResult;


	try
	{
		net::io_context ioc;
		std::shared_ptr<CMyHttpClientImp> pClient = std::make_shared<CMyHttpClientImp>(ioc);
		if (!pClient)
		{
			return "";
		}

		pClient->run(strServerIp, strServerPort, 11, (int)http::verb::get, strTarget, mapHeaders, strContent, uTimeout, m_pLog);
		ioc.run();

		strResult = pClient->content();
		strLocalIp = pClient->localip();
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "GetExWithTimeout end ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	return strResult;
}

std::string CHttpClientImpl::PutWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout)
{
	ICC_LOG_DEBUG(m_pLog, "PutWithTimeout begin ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	std::string strResult;

	try
	{
		net::io_context ioc;
		std::shared_ptr<CMyHttpClientImp> pClient = std::make_shared<CMyHttpClientImp>(ioc);
		if (!pClient)
		{
			return "";
		}

		pClient->run(strServerIp, strServerPort, 11, (int)http::verb::put, strTarget, mapHeaders, strContent, uTimeout, m_pLog);
		ioc.run();

		strResult = pClient->content();
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	ICC_LOG_DEBUG(m_pLog, "PutWithTimeout end ip[%s:%s]\nstrTarget[%s]",
		strServerIp.c_str(), strServerPort.c_str(), strTarget.c_str());
	return strResult;
}

std::string CHttpClientImpl::PutEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage)
{
	std::string strResult;

	try
	{
		int version = 11;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());
	
	    //设置30s后超时
		stream.expires_after(std::chrono::seconds(30));
		// Make the connection on the IP address we get from a lookup
	   //stream.connect(results);
		//get_lowest_layer(stream).async_connect(results, [&strErrorMessage](beast::error_code ec, net::ip::tcp::endpoint ep)
		//	{
		//		if (ec == beast::error::timeout)
		//		{
		//			strErrorMessage = "connect timeout";
		//			//return false;
		//		}
		//	});
		//
		stream.connect(results);

		//设置socket接收超时
#if defined(_WIN32) || defined(_WIN64)
		//const int timeout = 200;
		//::setsockopt(stream.socket().native_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);//SO_SNDTIMEO for send ops
#else
		struct timeval to;
		struct timeval timeout={30,0};
		to.tv_sec = 30;
		to.tv_usec = 0;
		if (::setsockopt(stream.socket().native_handle(), SOL_SOCKET, SO_RCVTIMEO, (char *)&to, sizeof(to)) < 0)
		{
			printf("CHttpClientImpl::PutEx, setsockopt error");
		}
#endif
		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::put, strTarget.c_str(), version };
		req.set(http::field::host, strServerIp.c_str());
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(http::field::content_type, "application/json");

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			req.set(itr_const->first, itr_const->second);
		}

		if (!strContent.empty())
		{
			req.body() = strContent;
			req.prepare_payload();
		}

		// Send the HTTP request to the remote host
		http::write(stream, req);
	
		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		//设置超时结束
		stream.expires_never();

		strResult = res.body().c_str();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	return strResult;
}

std::string CHttpClientImpl::DeleteEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage)
{
	std::string strResult;

	try
	{
		int version = 11;

		// The io_context is required for all I/O
		net::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		// Look up the domain name
		auto const results = resolver.resolve(strServerIp.c_str(), strServerPort.c_str());

		//设置30s后超时
		stream.expires_after(std::chrono::seconds(30));

		// Make the connection on the IP address we get from a lookup
		stream.connect(results);


		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::delete_, strTarget.c_str(), version };
		req.set(http::field::host, strServerIp.c_str());
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		//req.set(http::field::content_type, "application/json");

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			req.set(itr_const->first, itr_const->second);
		}

		if (!strContent.empty())
		{
			req.body() = strContent;
			req.prepare_payload();
		}

		// Send the HTTP request to the remote host
		http::write(stream, req);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::string_body> res;

		// Receive the HTTP response
		http::read(stream, buffer, res);

		strResult = res.body().c_str();


		//设置超时结束
		stream.expires_never();

		// Gracefully close the socket
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != beast::errc::not_connected)
			throw beast::system_error{ ec };

		// If we get here then the connection is closed gracefully
	}
	catch (std::exception const& e)
	{
		strErrorMessage = e.what();
	}
	return strResult;
}

void CHttpClientImpl::SetHeaders(std::map<std::string, std::string> p_ParamsMap)
{
	m_paramsMap = p_ParamsMap;
}
void CHttpClientImpl::SetHeadersEx(std::vector< std::map<std::string, std::string> > p_multiHeaders)
{
	m_multiHeaders = p_multiHeaders;
}

std::string CHttpClientImpl::GetLastError()
{
	return m_strError;
}

int CHttpClientImpl::GetLastErrorCode()
{
	return m_iHttpErrorCode;
}

void CHttpClientImpl::BuildPostRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request)
{
	out_request << "POST " << p_strUrl.c_str() << " HTTP/1.1\r\n";
	out_request << "Host: " << p_strIp.c_str() << ":" << p_strPort.c_str() << "\r\n";

	//设置http头参数
	if (!m_paramsMap.empty())
	{
		out_request << "Content-Length: " << p_strBody.length() << "\r\n";

		std::string l_strHeader;
		std::map<std::string, std::string>::iterator it = m_paramsMap.begin();
		for (; it != m_paramsMap.end(); ++it)
		{
			l_strHeader = it->first + ": " + it->second + "\r\n";
			out_request << l_strHeader;
		}		
	}
	else
	{
		out_request << "Content-Length: " << p_strBody.length() << "\r\n";
		out_request << "Content-Type: application/x-www-form-urlencoded\r\n";
		out_request << "Accept: */*\r\n";
		out_request << "Connection: close\r\n";		
	}

	out_request << "\r\n";
	out_request << p_strBody.c_str();
}

void CHttpClientImpl::BuildGetRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request)
{
	out_request << "GET " << p_strUrl.c_str() << " HTTP/1.1\r\n";
	out_request << "Host: " << p_strIp.c_str() << ":" << p_strPort.c_str() << "\r\n";

	//设置http头参数
	if (!m_paramsMap.empty())
	{
		out_request << "Content-Length: " << p_strBody.length() << "\r\n";

		std::string l_strHeader;
		std::map<std::string, std::string>::iterator it = m_paramsMap.begin();
		for (; it != m_paramsMap.end(); ++it)
		{
			l_strHeader = it->first + ": " + it->second + "\r\n";
			out_request << l_strHeader;
		}
	}
	else
	{
		out_request << "Accept: */*\r\n";
		out_request << "Connection: close\r\n";
	}
	out_request << "\r\n";
	out_request << p_strBody.c_str();
}

std::string CHttpClientImpl::Get(std::string p_strServerUrl, std::string p_strContent)
{
	std::list<UrlStruct> l_UrlList;
	PareUrlEx(p_strServerUrl, l_UrlList);

	for (auto l_UrlObj : l_UrlList)
	{
		if (m_IoService.stopped())
		{
			m_IoService.reset();
		}

		std::string l_strUrl = l_UrlObj.m_strUrl;
		std::map<std::string, std::string> l_headerMap = l_UrlObj.m_HeaderMap;
		SetHeaders(l_headerMap);

		try
		{
			HandleRequestResolve(ICC::Http::Get, l_strUrl, p_strContent);
			m_IoService.run();
		}		
		catch (...)
		{
			
		}		

		if (!m_strResponseContent.empty())
		{
			if (!FindActiveUrl(l_strUrl))
			{
				AddActiveUrl(l_strUrl);
			}

			break;
		}
		else
		{
			DelActiveUrl(l_strUrl);
		}
	}

	
	return m_strResponseContent;
}

std::string CHttpClientImpl::Post(std::string p_strServerUrl, std::string p_strContent)
{
	std::list<UrlStruct> l_UrlList;
	PareUrlEx(p_strServerUrl, l_UrlList);

	for (auto l_UrlObj : l_UrlList)                                                                                                           
	{
		if (m_IoService.stopped())
		{
			m_IoService.reset();
		}

		std::string l_strUrl = l_UrlObj.m_strUrl;
		std::map<std::string, std::string> l_headerMap = l_UrlObj.m_HeaderMap;
		SetHeaders(l_headerMap);

		try
		{
			HandleRequestResolve(ICC::Http::Post, l_strUrl, p_strContent);
			m_IoService.run();
		}		
		catch (...)
		{
		}

		

		if (!m_strResponseContent.empty())
		{
			if (!FindActiveUrl(l_strUrl))
			{
				AddActiveUrl(l_strUrl);
			}

			break;
		}
		else
		{
			DelActiveUrl(l_strUrl);
		}
	}

	l_UrlList.clear();

	return m_strResponseContent;
}

void CHttpClientImpl::HandleRequestResolve(HttpRequestType p_httpRequestType, std::string p_strUrl, std::string p_strContent /* = "" */)
{
	try
	{
		m_strError.clear();
		m_strResponseContent.clear();
		if (m_response.size() > 0)
		{
			m_response.consume(m_response.size());
		}
		
		// 解析URL
		std::string l_strIP, l_strPort, l_strPath;

		if (HTTP_SUCCESS != ParseUrl(p_strUrl, l_strIP, l_strPort, l_strPath))
		{
			return;
		}

		std::ostream l_requestStream(&m_request);

		// 构造请求
		switch (p_httpRequestType)
		{
		case ICC::Http::Post:
			BuildPostRequest(l_strPath, l_strIP, l_strPort, p_strContent, l_requestStream);
			break;
		case ICC::Http::Get:
			BuildGetRequest(l_strPath, l_strIP, l_strPort, p_strContent, l_requestStream);
			break;
			
		default:
			return;
		}

		
		
		// 解析服务地址\端口
		boost::asio::ip::tcp::resolver::query l_query(l_strIP, l_strPort);
		m_resolver.async_resolve(l_query,
			boost::bind(&CHttpClientImpl::HandleResolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
	}
	catch (std::exception& e)
	{
		//m_socket.close();
		_CloseSocket();
		m_strError = e.what();
	}
	return;
}

void CHttpClientImpl::HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (err)
	{
		m_strError = err.message();
		return;
	}

	try
	{
		// 尝试连接
		boost::asio::async_connect(m_socket, endpoint_iterator,
			boost::bind(&CHttpClientImpl::HandleConnect, this,
			boost::asio::placeholders::error));
	}
	catch (std::exception& e)
	{
		_CloseSocket();
		//m_socket.close();
		m_strError = e.what();
	}
}

// 连接后
void CHttpClientImpl::HandleConnect(const boost::system::error_code& err)
{
	if (err)
	{
		m_strError = err.message();
		return;
	}

	try
	{
		// 发送request请求
		boost::asio::async_write(m_socket, m_request,
			boost::bind(&CHttpClientImpl::HandleWriteRequest, this,
			boost::asio::placeholders::error));
	}
	catch (std::exception& e)
	{
		_CloseSocket();
		//m_socket.close();
		m_strError = e.what();
	}
}

// 发送请求后
void CHttpClientImpl::HandleWriteRequest(const boost::system::error_code& err)
{
	if (err)
	{
		m_strError = err.message();
		return;
	}

	try
	{
		// 异步持续读数据到m_response，直到接收协议符号 \r\n 为止
		boost::asio::async_read_until(m_socket, m_response, "\r\n",
			boost::bind(&CHttpClientImpl::HandleReadStatusLine, this,
			boost::asio::placeholders::error));
	}
	catch (std::exception& e)
	{
		_CloseSocket();
		//m_socket.close();
		m_strError = e.what();
	}
}

// 读取响应后
void CHttpClientImpl::HandleReadStatusLine(const boost::system::error_code& err)
{
	if (err)
	{
		m_strError = err.message();
		return;
	}

	// 解析buff
	std::istream l_responseStream(&m_response);

	unsigned int l_iStatusCode;
	std::string l_strHttpVersion, l_strStatusMessage;

	l_responseStream >> l_strHttpVersion;
	l_responseStream >> l_iStatusCode;   //状态码

	try
	{
		std::getline(l_responseStream, l_strStatusMessage);
	}	
	catch (...)
	{
	}

	

	// 核对是否是正确返回
	if (!l_responseStream || l_strHttpVersion.substr(0, 5) != "HTTP/")
	{
		m_iHttpErrorCode = l_iStatusCode;
		m_strError = HTTP_ERROR_VERSION;
		return;
	}

	if (l_iStatusCode != 200 && l_iStatusCode != 202)
	{
		m_iHttpErrorCode = l_iStatusCode;
		m_strError = HTTP_ERROR_UNKNOWN;
		return;
	}

	try
	{
		// 读取响应头,直到接收协议符号 \r\n\r\n 为止
		boost::asio::async_read_until(m_socket, m_response, "\r\n\r\n",
			boost::bind(&CHttpClientImpl::HandleReadHeaders, this,
			boost::asio::placeholders::error));
	}
	catch (std::exception& e)
	{
		_CloseSocket();
		//m_socket.close();
		m_strError = e.what();
	}
}

// 读取响应头后
void CHttpClientImpl::HandleReadHeaders(const boost::system::error_code& err)
{
	if (err)
	{
		m_strError = err.message();
		return;
	}

	// 输出响应头
	std::istream l_responseStream(&m_response);
	std::string l_strHeader;

	try
	{
		while (std::getline(l_responseStream, l_strHeader) && l_strHeader != "\r")
		{
			m_strResponseContent += l_strHeader;
#ifdef _DEBUG
			std::cout << l_strHeader << std::endl;
#endif 
		}
#ifdef _DEBUG
		std::cout << "\n";
#endif 
	}	
	catch (...)
	{
	}	

	if (m_response.size() > 0)
	{
		boost::asio::streambuf::const_buffers_type cbt = m_response.data();
		std::string strData = std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
		m_strResponseContent += strData;
		m_response.consume(strData.length());
		try
		{
			// 开始读取剩余所有内容
			boost::asio::async_read(m_socket, m_response,
				boost::asio::transfer_at_least(1),
				boost::bind(&CHttpClientImpl::HandleReadContent, this,
				boost::asio::placeholders::error));
		}
		catch (std::exception& e)
		{
			_CloseSocket();
			//m_socket.close();
			m_strError = e.what();
		}
	}
}

// 读取正文数据后
void CHttpClientImpl::HandleReadContent(const boost::system::error_code& err)
{
	if (!err)
	{
		try
		{
			if (m_response.size() > 0)
			{
				boost::asio::streambuf::const_buffers_type cbt = m_response.data();
				std::string strData = std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
				m_strResponseContent += strData;
				//std::cout << strData << std::endl;
				int iSize = m_strResponseContent.size();
				//std::cout << iSize << std::endl;
				m_response.consume(strData.length());
				// 继续读取剩余内容，直到读到EOF
				boost::asio::async_read(m_socket, m_response,
					boost::asio::transfer_at_least(1),
					boost::bind(&CHttpClientImpl::HandleReadContent, this,
					boost::asio::placeholders::error));
			}
			else
			{
				int i = 0;
			}
		}
		catch (std::exception& e)
		{
			_CloseSocket();
			//m_socket.close();
			m_strError = e.what();
		}
	}
	else if (err != boost::asio::error::eof)
	{
		std::cout << "err value:" << err.value() << "err message:" << err.message() << std::endl;
		m_strError = err.message();
	}
	else
	{
		_CloseSocket();
		//m_socket.close();
		m_resolver.cancel();
		m_strError = err.message();
	}
}

//解析URL
int CHttpClientImpl::ParseUrl(std::string p_strUrl, std::string& p_strIP, std::string& p_strPort, std::string& p_strPath)
{
	std::string l_strHttpTag = "http://";
	std::string l_strHttpsTag = "https://";
	std::string l_strTemp = p_strUrl;

	// 截断http协议头
	if (l_strTemp.find(l_strHttpTag) == 0)
	{
		l_strTemp = l_strTemp.substr(l_strHttpTag.length());
	}
	else if (l_strTemp.find(l_strHttpsTag) == 0)
	{
		l_strTemp = l_strTemp.substr(l_strHttpsTag.length());
	}
	else
	{
		return HTTP_FAILED;
	}

	// 解析域名
	std::size_t l_idex = l_strTemp.find('/');
	// 解析 域名后的page
	if (std::string::npos == l_idex)
	{
		p_strPath = "/";
		l_idex = l_strTemp.size();
	}
	else
	{
		p_strPath = l_strTemp.substr(l_idex);
	}

	// 解析域名
	p_strIP = l_strTemp.substr(0, l_idex);

	// 解析端口
	l_idex = p_strIP.find(':');
	if (std::string::npos == l_idex)
	{
		p_strPort = "80";
	}
	else
	{
		p_strPort = p_strIP.substr(l_idex + 1);
		p_strIP = p_strIP.substr(0, l_idex);
	}

	return HTTP_SUCCESS;
}
void CHttpClientImpl::PareUrlEx(std::string p_strUrls, std::list<std::string>& p_UrlList)
{
	std::string l_strStringList = p_strUrls;
	p_UrlList.clear();

	int l_nIndex = l_strStringList.find(";");
	while (l_nIndex != std::string::npos)
	{
		std::string l_strSubString = l_strStringList.substr(0, l_nIndex);
		if (FindActiveUrl(l_strSubString))
		{
			p_UrlList.push_front(l_strSubString);
		} 
		else
		{
			p_UrlList.push_back(l_strSubString);
		}

		l_strStringList = l_strStringList.substr(l_nIndex + 1, l_strStringList.length() - l_nIndex - 1);
		l_nIndex = l_strStringList.find(";");
	}

	if (!l_strStringList.empty())
	{
		if (FindActiveUrl(l_strStringList))
		{
			p_UrlList.push_front(l_strStringList);
		}
		else
		{
			p_UrlList.push_back(l_strStringList);
		}
	}
}
void CHttpClientImpl::PareUrlEx(std::string p_strUrls, std::list<UrlStruct>& p_UrlList)
{
	// 如果有多个 Header，上层配置的 URL 需与 Header 一一对应
	std::string l_strStringList = p_strUrls;
	p_UrlList.clear();

	int l_nHeaderIndex = 0;
	int l_nFindPos = l_strStringList.find(";");
	while (l_nFindPos != std::string::npos)
	{
		std::string l_strSubString = l_strStringList.substr(0, l_nFindPos);

		std::map<std::string, std::string> l_headerMap = m_paramsMap;
		if (m_multiHeaders.size() > l_nHeaderIndex)
		{
			l_headerMap = m_multiHeaders[l_nHeaderIndex++];
		}
		
		UrlStruct l_UrlStruct;
		l_UrlStruct.m_strUrl = l_strSubString;
		l_UrlStruct.m_HeaderMap = l_headerMap;

		if (FindActiveUrl(l_strSubString))
		{
			p_UrlList.push_front(l_UrlStruct);
		}
		else
		{
			p_UrlList.push_back(l_UrlStruct);
		}

		l_strStringList = l_strStringList.substr(l_nFindPos + 1, l_strStringList.length() - l_nFindPos - 1);
		l_nFindPos = l_strStringList.find(";");
	}

	if (!l_strStringList.empty())
	{
		std::map<std::string, std::string> l_headerMap = m_paramsMap;
		if (m_multiHeaders.size() > l_nHeaderIndex)
		{
			l_headerMap = m_multiHeaders[l_nHeaderIndex++];
		}

		UrlStruct l_UrlStruct;
		l_UrlStruct.m_strUrl = l_strStringList;
		l_UrlStruct.m_HeaderMap = l_headerMap;

		if (FindActiveUrl(l_strStringList))
		{
			p_UrlList.push_front(l_UrlStruct);
		}
		else
		{
			p_UrlList.push_back(l_UrlStruct);
		}
	}
}
void CHttpClientImpl::AddActiveUrl(const std::string& p_strUrl)
{
	std::lock_guard<std::mutex> guard(m_UrlListMutex);
	m_ActiveUrlList.push_back(p_strUrl);
}
void CHttpClientImpl::DelActiveUrl(const std::string& p_strUrl)
{
	std::lock_guard<std::mutex> guard(m_UrlListMutex);
	m_ActiveUrlList.remove(p_strUrl);
}
bool CHttpClientImpl::FindActiveUrl(const std::string& p_strUrl)
{
	std::lock_guard<std::mutex> guard(m_UrlListMutex);
	for (auto l_strActiveUrl : m_ActiveUrlList)
	{
		if (p_strUrl.find(l_strActiveUrl) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}
void CHttpClientImpl::ClearActiveUrlList()
{
	std::lock_guard<std::mutex> guard(m_UrlListMutex);
	m_ActiveUrlList.clear();
}

void CHttpClientImpl::_CloseSocket()
{
	try
	{
		m_socket.close();
	}	
	catch (...)
	{
	}
}