#pragma once

#include <thread>
#include <condition_variable>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/thread/thread.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace ICC
{
	// This is the C++11 equivalent of a generic lambda.
	// The function object is used to send an HTTP message.
	template<class Stream>
	struct send_lambda
	{
		Stream& stream_;
		bool& close_;
		beast::error_code& ec_;

		explicit
			send_lambda(
				Stream& stream,
				bool& close,
				beast::error_code& ec)
			: stream_(stream)
			, close_(close)
			, ec_(ec)
		{
			//
		}

		template<bool isRequest, class Body, class Fields>
		void operator()(http::message<isRequest, Body, Fields>&& msg) const
		{
			// Determine if we should close the connection after
			close_ = msg.need_eof();

			// We need the serializer here because the serializer requires
			// a non-const file_body, and the message oriented version of
			// http::write only works with const messages.
			http::serializer<isRequest, Body, Fields> sr{ msg };
			http::write(stream_, sr, ec_);
		}
	};

	typedef struct _sRequestObj
	{
		bool					m_bIsResp;
		std::string				m_strReqGuid;
		std::shared_ptr<http::request<http::string_body>> m_pHttpReq;
		std::shared_ptr<send_lambda<beast::ssl_stream<tcp::socket&>>> m_pReqStream;

		_sRequestObj() :
			m_bIsResp(false),
			m_strReqGuid(""),
			m_pHttpReq(nullptr),
			m_pReqStream(nullptr)
		{
			//
		}
	}RequestObj, * PRequestObj;

	class CHttpsServerImpl
		: public IHttpsServer
	{
	public:
		CHttpsServerImpl(IResourceManagerPtr p_resourceManager);
		virtual ~CHttpsServerImpl();

	public:
		virtual bool StartHttps(IHttpsCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
			const std::string& p_strCrtFile, const std::string& p_strKeyFile, const std::string& p_strDhFile,
			const std::string& p_strDocRoot = std::string("/"));
		virtual bool StopHttps();
		virtual bool OnResponse(const std::string& p_strReqGuid, const std::string& p_strRespBody);

	private:
		void LoadParam();
		beast::string_view MimeType(beast::string_view p_strPath);
		std::string PathCat(beast::string_view p_strBase, beast::string_view p_strPath);
		std::string GetPassword() const { return m_strCrtPassword; };

		template<class Body, class Send>
		void AddReqList(const std::string& p_strReqGuid, Body&& p_httpReq, Send&& p_send);
		void ClearReqList();

		bool IsResp(const std::string& p_strReqGuid);
		void AddWaitRespList(std::shared_ptr<RequestObj> p_pReqObj);
		void DelWaitRespList(const std::string& p_strReqGuid);
		bool ProcessRespTimeout(const std::string& p_strReqGuid);
		void ClearWaitRespList();

		template<class Body, class Send, class Result>
		void DoRespond(Body&& p_httpReq, Send&& p_send, Result p_result, const std::string& p_strRespBody);
		template<class Body, class Allocator, class Send>
		bool HandleRequest(const std::string& p_strReqGuid, http::request<Body, http::basic_fields<Allocator>>&& p_httpReq, Send&& p_send);
		void DoSession(tcp::socket& p_socket, ssl::context& p_ctx);
		void Fail(beast::error_code p_errorCode, const std::string& p_strWhat);	

		void ListenHttps();
		void OnMessage();
		
	private:
		bool		m_bStopServer;
		int			m_nRespTimeout;		//应用层应答超时时长（单位：秒）
		std::string m_strLocalAddress;
		std::string m_strLocalPort;
		std::string m_strDocRoot;
		std::string m_strCrtFile;
		std::string m_strKeyFile;
		std::string m_strDhFile;
		std::string m_strCrtPassword;
			
		Log::ILogPtr							m_pLog;
		Config::IConfigPtr						m_pConfig;
		DateTime::IDateTimePtr					m_pDateTime;
		StringUtil::IStringUtilPtr				m_pString;
		JsonParser::IJsonFactoryPtr				m_pJsonFty;
		IHttpsCallbackPtr						m_pCallback;

		std::mutex								m_mutexRequest;
		std::mutex								m_mutexWaitResp;
		std::condition_variable					m_conditionRequest;		
		std::condition_variable					m_conditionWaitResp;	
		std::list<std::shared_ptr<RequestObj>>	m_listRequest;
		std::list<std::shared_ptr<RequestObj>>	m_listWaitResp;

		std::shared_ptr<std::thread>			m_threadListen;
		std::shared_ptr<std::thread>			m_threadReqCallback;
	};
}