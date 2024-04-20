#ifndef __SimpleHttpClient_H__
#define __SimpleHttpClient_H__

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CMyHttpClientImp : public std::enable_shared_from_this<CMyHttpClientImp>
{
	tcp::resolver m_resolver;
	beast::tcp_stream m_stream;
	beast::flat_buffer m_buffer; // (Must persist between reads)
	http::request<http::string_body> m_req;
	http::response<http::string_body> m_res;

	std::string m_strContent;
	std::string m_strLocalIp;
	std::string m_strWhat;
	beast::error_code m_ec;

	unsigned int m_uTimeout;

	Log::ILogPtr m_Log;
public:
	// Objects are constructed with a strand to
	// ensure that handlers do not execute concurrently.

	explicit CMyHttpClientImp(net::io_context& ioc) : m_resolver(net::make_strand(ioc)), m_stream(net::make_strand(ioc))
	{
		m_uTimeout = 30;
	}

	std::string content()
	{
		return m_strContent;
	}

	std::string localip()
	{
		return m_strLocalIp;
	}

	beast::error_code ErrorCode(std::string& strWhat)
	{
		strWhat = m_strWhat;
		return m_ec;
	}

	// Start the asynchronous operation
	void run(const std::string& strHost, const std::string& strPort, int version, int mothod, const std::string& strTarget,
		std::map<std::string, std::string>& mapHeaders, const std::string& strContent, unsigned int uTimeout, Log::ILogPtr m_pLog)
	{
		m_Log = m_pLog;

		ICC_LOG_LOWDEBUG(m_Log, "run enter!");

		m_uTimeout = uTimeout;

		// Set up an HTTP GET request message
		m_req.version(version);
		m_req.method(http::verb(mothod));
		m_req.target(strTarget.c_str());
		m_req.set(http::field::host, strHost.c_str());
		m_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		m_req.set(http::field::content_type, "application/json");

		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = mapHeaders.begin(); itr_const != mapHeaders.end(); ++itr_const)
		{
			m_req.set(itr_const->first, itr_const->second);
		}

		if (!strContent.empty())
		{
			m_req.body() = strContent;
			m_req.prepare_payload();
		}

		ICC_LOG_LOWDEBUG(m_Log, "run bind_front_handler enter!");
		//m_req.body() = strContent;
		//m_req.prepare_payload();

		// Look up the domain name
		m_resolver.async_resolve(strHost, strPort, beast::bind_front_handler(&CMyHttpClientImp::on_resolve, shared_from_this()));
	}

	void on_resolve(beast::error_code ec, tcp::resolver::results_type results)
	{
		if (ec)
		{
			ICC_LOG_FATAL(m_Log, "on_resolve fail! [%d] [%s]", ec.value(), ec.message().c_str());
			return fail(ec, "resolve");
		}
		ICC_LOG_LOWDEBUG(m_Log, "on_resolve enter!");
		// Set a timeout on the operation
		//m_stream.expires_after(std::chrono::seconds(30));
		m_stream.expires_after(std::chrono::seconds(m_uTimeout));

		// Make the connection on the IP address we get from a lookup
		m_stream.async_connect(results, beast::bind_front_handler(&CMyHttpClientImp::on_connect, shared_from_this()));
	}

	void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
	{
		if (ec)
		{
			ICC_LOG_FATAL(m_Log, "on_connect fail! [%d] [%s]", ec.value(), ec.message().c_str());
			return fail(ec, "connect");
		}
		ICC_LOG_LOWDEBUG(m_Log, "on_connect enter!");
		// Set a timeout on the operation
		//m_stream.expires_after(std::chrono::seconds(30));
		m_stream.expires_after(std::chrono::seconds(m_uTimeout));

		// Send the HTTP request to the remote host
		http::async_write(m_stream, m_req, beast::bind_front_handler(&CMyHttpClientImp::on_write, shared_from_this()));
	}

	void on_write(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			ICC_LOG_FATAL(m_Log, "on_write fail! [%d] [%s]", ec.value(), ec.message().c_str());
			return fail(ec, "write");
		}
		ICC_LOG_LOWDEBUG(m_Log, "on_write enter!");
		// Receive the HTTP response
		http::async_read(m_stream, m_buffer, m_res, beast::bind_front_handler(&CMyHttpClientImp::on_read, shared_from_this()));
	}

	void on_read(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			ICC_LOG_FATAL(m_Log, "on_read fail! [%d] [%s]", ec.value(), ec.message().c_str());
			return fail(ec, "read");
		}
		ICC_LOG_LOWDEBUG(m_Log, "on_read enter!");
		// Write the message to standard out
		//std::cout << m_res << std::endl;
		m_strContent = m_res.body();

		m_strLocalIp = m_stream.socket().local_endpoint().address().to_string();
		// Gracefully close the socket
		m_stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes so don't bother reporting it.
		if (ec && ec != beast::errc::not_connected)
		{
			ICC_LOG_FATAL(m_Log, "on_read shutdown!");
			return fail(ec, "shutdown");
		}
		// If we get here then the connection is closed gracefully
	}

	void fail(beast::error_code ec, char const* what)
	{
		m_ec = ec;
		m_strWhat = what;
		std::cerr << what << ": " << ec.message() << "\n";
	}
};

#endif
