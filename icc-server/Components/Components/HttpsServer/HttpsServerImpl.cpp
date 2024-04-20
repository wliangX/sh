#include "Boost.h"
#include "HttpsServerImpl.h"

#define SLEEP_TIME	2

CHttpsServerImpl::CHttpsServerImpl(IResourceManagerPtr p_resourceManager) :
	m_strLocalAddress("0.0.0.0"),
	m_strLocalPort("29090"),
	m_strDocRoot("/"),
	m_strCrtFile("server.crt"),
	m_strKeyFile("server.key"),
	m_strDhFile("dh1024.pem"),
	m_strCrtPassword("123456"),
	m_bStopServer(false),
	m_nRespTimeout(5),
	m_pCallback(nullptr)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_resourceManager)->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetResourceEx(Config::IConfigFactory, ICCIConfigFactoryResourceName, p_resourceManager)->CreateConfig();
	m_pString = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_resourceManager)->CreateString();
	m_pDateTime = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_resourceManager)->CreateDateTime();
}

CHttpsServerImpl::~CHttpsServerImpl()
{
	m_bStopServer = true;
	m_threadReqCallback->join();
	m_threadListen->join();

	ClearReqList();
	ClearWaitRespList();
}

void CHttpsServerImpl::LoadParam()
{
	/*m_strLocalAddress = m_pConfig->GetValue("ICC/Component/HttpsServer/LocalAddress", "0.0.0.0");
	m_strLocalPort = m_pConfig->GetValue("ICC/Component/HttpsServer/LocalPort", "9080");
	m_strDocRoot = m_pConfig->GetValue("ICC/Component/HttpsServer/DocRoot", "/");
	m_strCrtFile = m_pConfig->GetValue("ICC/Component/HttpsServer/CrtFile", "server.crt");
	m_strKeyFile = m_pConfig->GetValue("ICC/Component/HttpsServer/KeyFile", "server.key");
	m_strDhFile = m_pConfig->GetValue("ICC/Component/HttpsServer/DhFile", "dh1024.pem");
	m_strCrtPassword = m_pConfig->GetValue("ICC/Component/HttpsServer/CrtPassword", "123456");*/
	std::string l_strRespTimeout = m_pConfig->GetValue("ICC/Component/HttpsServer/RespTimeout", "5");
	m_nRespTimeout = std::atoi(l_strRespTimeout.c_str());
}
//////////////////////////////////////////////////////////////////////////

bool CHttpsServerImpl::StartHttps(IHttpsCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
	const std::string& p_strCrtFile, const std::string& p_strKeyFile, const std::string& p_strDhFile,
	const std::string& p_strDocRoot/* = std::string("/")*/)
{
	LoadParam();

	m_strLocalAddress = p_strLocalIP;
	m_strLocalPort = std::to_string(p_uLocalPort);
	m_strCrtFile = p_strCrtFile;
	m_strKeyFile = p_strKeyFile;
	m_strDhFile = p_strDhFile;
	m_strDocRoot = p_strDocRoot;
	m_pCallback = p_pCallback;

	m_threadListen = std::make_shared<std::thread>(std::bind(&CHttpsServerImpl::ListenHttps, this));
	m_threadReqCallback = std::make_shared<std::thread>(std::bind(&CHttpsServerImpl::OnMessage, this));

	return true;
}
bool CHttpsServerImpl::StopHttps()
{
	return true;
}

bool CHttpsServerImpl::OnResponse(const std::string& p_strReqGuid, const std::string& p_strRespBody)
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	for (auto l_respObj : m_listWaitResp)
	{
		if (p_strReqGuid.compare(l_respObj->m_strReqGuid) == 0)
		{/*
			http::response<http::string_body> l_httpRes{ http::status::ok, l_reqObj->m_uReqVersion };
			l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
			l_httpRes.set(http::field::content_type, "text/html");
			l_httpRes.body() = p_strResp;
			l_httpRes.content_length(p_strResp.size());
			l_httpRes.keep_alive(l_reqObj->m_bReqKeepAlive);

			l_reqObj->m_reqStream->operator()(std::move(l_httpRes));*/
			DoRespond(l_respObj->m_pHttpReq, l_respObj->m_pReqStream, http::status::ok, p_strRespBody);
			//m_listWaitResp.remove(l_respObj);

			l_respObj->m_bIsResp = true;
			m_conditionWaitResp.notify_all();

			return true;
		}
	}

	//std::cout << "Not found req, Guid: " << p_strReqGuid.c_str() << std::endl;
	ICC_LOG_ERROR(m_pLog, "Not found req, Guid:[%s]", p_strReqGuid.c_str());

	return false;
}
//////////////////////////////////////////////////////////////////////////

template<class Body, class Send>
void CHttpsServerImpl::AddReqList(const std::string& p_strReqGuid, Body&& p_httpReq, Send&& p_send)
{
	ICC_LOG_LOWDEBUG(m_pLog, "Add to Req List, Guid:[%s]", p_strReqGuid.c_str());

	std::unique_lock<std::mutex> l_lock(m_mutexRequest);
	
	std::shared_ptr<RequestObj> l_reqObj = std::make_shared<RequestObj>();
	l_reqObj->m_strReqGuid = p_strReqGuid;
	//l_reqObj->m_tReqTime = m_pDateTime->CurrentDateTime();
	l_reqObj->m_pHttpReq = p_httpReq;
	l_reqObj->m_pReqStream = p_send;
	m_listRequest.push_back(l_reqObj);

	m_conditionRequest.notify_all();
}
void CHttpsServerImpl::ClearReqList()
{
	std::unique_lock<std::mutex> l_lock(m_mutexRequest);

	m_listRequest.clear();
}

bool CHttpsServerImpl::IsResp(const std::string& p_strReqGuid)
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	for (auto l_reqObj : m_listWaitResp)
	{
		if (l_reqObj->m_bIsResp && p_strReqGuid.compare(l_reqObj->m_strReqGuid) == 0)
		{
			return true;
		}
	}

	return false;
}
void CHttpsServerImpl::AddWaitRespList(std::shared_ptr<RequestObj> p_pReqObj)
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	std::shared_ptr<RequestObj> l_pReqObj = p_pReqObj;
	m_listWaitResp.push_back(l_pReqObj);
}
void CHttpsServerImpl::DelWaitRespList(const std::string& p_strReqGuid)
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	for (auto l_reqObj : m_listWaitResp)
	{
		if (p_strReqGuid.compare(l_reqObj->m_strReqGuid) == 0)
		{
			m_listWaitResp.remove(l_reqObj);
			ICC_LOG_DEBUG(m_pLog, "WaitRespList Size:[%d]", m_listWaitResp.size());

			break;
		}
	}
}
bool CHttpsServerImpl::ProcessRespTimeout(const std::string& p_strReqGuid)
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	for (auto l_respObj : m_listWaitResp)
	{
		if (!l_respObj->m_bIsResp && p_strReqGuid.compare(l_respObj->m_strReqGuid) == 0)
		{
			ICC_LOG_ERROR(m_pLog, "Request timeout, Guid:[%s]", l_respObj->m_strReqGuid.c_str());

			DoRespond(l_respObj->m_pHttpReq, l_respObj->m_pReqStream, http::status::request_timeout, "request_timeout");

			l_respObj->m_bIsResp = true;
			m_conditionWaitResp.notify_all();

			return true;
		}
	}

	return false;
}
void CHttpsServerImpl::ClearWaitRespList()
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);

	m_listWaitResp.clear();
}

// Return a reasonable mime type based on the extension of a file.
beast::string_view CHttpsServerImpl::MimeType(beast::string_view p_strPath)
{
	using beast::iequals;
	auto const l_strFileExt = [&p_strPath]
	{
		auto const l_pos = p_strPath.rfind(".");
		if (l_pos == beast::string_view::npos)
			return beast::string_view{};
		return p_strPath.substr(l_pos);
	}();

	if (iequals(l_strFileExt, ".htm"))  return "text/html";
	if (iequals(l_strFileExt, ".html")) return "text/html";
	if (iequals(l_strFileExt, ".php"))  return "text/html";
	if (iequals(l_strFileExt, ".css"))  return "text/css";

	if (iequals(l_strFileExt, ".txt"))  return "text/plain";
	if (iequals(l_strFileExt, ".js"))   return "application/javascript";
	if (iequals(l_strFileExt, ".json")) return "application/json";
	if (iequals(l_strFileExt, ".xml"))  return "application/xml";
	if (iequals(l_strFileExt, ".swf"))  return "application/x-shockwave-flash";
	if (iequals(l_strFileExt, ".flv"))  return "video/x-flv";
	if (iequals(l_strFileExt, ".png"))  return "image/png";
	if (iequals(l_strFileExt, ".jpe"))  return "image/jpeg";
	if (iequals(l_strFileExt, ".jpeg")) return "image/jpeg";
	if (iequals(l_strFileExt, ".jpg"))  return "image/jpeg";
	if (iequals(l_strFileExt, ".gif"))  return "image/gif";
	if (iequals(l_strFileExt, ".bmp"))  return "image/bmp";
	if (iequals(l_strFileExt, ".ico"))  return "image/vnd.microsoft.icon";
	if (iequals(l_strFileExt, ".tiff")) return "image/tiff";
	if (iequals(l_strFileExt, ".tif"))  return "image/tiff";
	if (iequals(l_strFileExt, ".svg"))  return "image/svg+xml";
	if (iequals(l_strFileExt, ".svgz")) return "image/svg+xml";

	return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string CHttpsServerImpl::PathCat(beast::string_view p_strBase, beast::string_view p_strPath)
{
	if (p_strBase.empty())
		return std::string(p_strPath);

	std::string l_strResult(p_strBase);
#ifdef BOOST_MSVC
	char constexpr l_szPathSeparator = '\\';
	if (l_strResult.back() == l_szPathSeparator)
		l_strResult.resize(l_strResult.size() - 1);
	l_strResult.append(p_strPath.data(), p_strPath.size());
	for (auto& c : l_strResult)
		if (c == '/')
			c = l_szPathSeparator;
#else
	char constexpr l_szPathSeparator = '/';
	if (l_strResult.back() == l_szPathSeparator)
		l_strResult.resize(l_strResult.size() - 1);
	l_strResult.append(p_strPath.data(), p_strPath.size());
#endif

	return l_strResult;
}

template<class Body, class Send, class Result>
void CHttpsServerImpl::DoRespond(Body&& p_httpReq, Send&& p_send, Result p_result, const std::string& p_strRespBody)
{
	http::response<http::string_body> l_httpRes{ p_result/*http::status::ok*/, p_httpReq->version() };
	l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
	l_httpRes.set(http::field::content_type, "text/html");
	l_httpRes.body() = p_strRespBody;
	l_httpRes.content_length(p_strRespBody.size());
	l_httpRes.keep_alive(p_httpReq->keep_alive());

	//std::cout << "Send Resp: " << p_strRespBody.c_str() << std::endl;
	ICC_LOG_LOWDEBUG(m_pLog, "Send Resp:[%s]", p_strRespBody.c_str());

	return p_send->operator()(std::move(l_httpRes));
}

// Report a failure
void CHttpsServerImpl::Fail(beast::error_code p_errorCode, const std::string& p_strWhat)
{
	std::cerr << p_strWhat << ": " << p_errorCode.message() << std::endl;
	ICC_LOG_ERROR(m_pLog, "Fail, Error [%s] Code[%s]", p_strWhat.c_str(), p_errorCode.message().c_str());
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<class Body, class Allocator, class Send>
bool CHttpsServerImpl::HandleRequest(const std::string& p_strReqGuid, http::request<Body, http::basic_fields<Allocator>>&& p_httpReq, Send&& p_send)
{
	//std::cout << "Req target: " << p_httpReq.target() << " Body: " << p_httpReq.body() << std::endl;
	ICC_LOG_LOWDEBUG(m_pLog, "Req Target: [%s] Body: [%s]", std::string(p_httpReq.target()).c_str(), p_httpReq.body().c_str());

	// Returns a bad request response
	auto const l_badRequest =
		[&p_httpReq](beast::string_view why)
	{
		http::response<http::string_body> l_httpRes{ http::status::bad_request, p_httpReq.version() };
		l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		l_httpRes.set(http::field::content_type, "text/html");
		l_httpRes.keep_alive(p_httpReq.keep_alive());
		l_httpRes.body() = std::string(why);
		l_httpRes.prepare_payload();

		return l_httpRes;
	};

	// Returns a not found response
	auto const l_notFound =
		[&p_httpReq](beast::string_view target)
	{
		http::response<http::string_body> l_httpRes{ http::status::not_found, p_httpReq.version() };
		l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		l_httpRes.set(http::field::content_type, "text/html");
		l_httpRes.keep_alive(p_httpReq.keep_alive());
		l_httpRes.body() = "The resource '" + std::string(target) + "' was not found.";
		l_httpRes.prepare_payload();

		return l_httpRes;
	};

	// Returns a server error response
	auto const l_serverError =
		[&p_httpReq](beast::string_view what)
	{
		http::response<http::string_body> l_httpRes{ http::status::internal_server_error, p_httpReq.version() };
		l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		l_httpRes.set(http::field::content_type, "text/html");
		l_httpRes.keep_alive(p_httpReq.keep_alive());
		l_httpRes.body() = "An error occurred: '" + std::string(what) + "'";
		l_httpRes.prepare_payload();

		return l_httpRes;
	};

	// Make sure we can handle the method
	if (p_httpReq.method() != http::verb::get &&
		p_httpReq.method() != http::verb::post &&
		p_httpReq.method() != http::verb::head)
	{
		p_send->operator()(l_badRequest("Unknown HTTP-method"));
		return false;
	}

	// Request path must be absolute and not contain "..".
	if (p_httpReq.target().empty() ||
		p_httpReq.target()[0] != '/' ||
		p_httpReq.target().find("..") != beast::string_view::npos)
	{
		p_send->operator()(l_badRequest("Illegal request-target"));
		return false;
	}

	if (p_httpReq.target().find("/icc") != beast::string_view::npos || p_httpReq.target().find("/actuator") != beast::string_view::npos)
	{
		std::shared_ptr<http::request<http::string_body>> l_pHttpReq = std::make_shared<http::request<http::string_body>>(p_httpReq);
		AddReqList(p_strReqGuid, l_pHttpReq, p_send);

		return true;
	}
	else
	{
		p_send->operator()(l_notFound(p_httpReq.target()));

		return false;
	}

	return false;
}

// Handles an HTTP server connection
void CHttpsServerImpl::DoSession(tcp::socket& p_socket, ssl::context& p_ctx)
{
	std::string l_strRemoteIP = "";
	unsigned short l_uRemoteport = 0;
	boost::system::error_code l_sysErrCode;
	boost::asio::ip::tcp::endpoint remoteEndpoint = p_socket.remote_endpoint(l_sysErrCode);
	if (!l_sysErrCode)
	{
		boost::asio::ip::address l_remoteAddr = remoteEndpoint.address();
		l_strRemoteIP = l_remoteAddr.to_string();
		l_uRemoteport = remoteEndpoint.port();

		//std::cout << "Connection enter!! ip: " << l_strRemoteIP << " port: " << l_uRemoteport << std::endl;
		ICC_LOG_DEBUG(m_pLog, "Connection enter, Peer IP[%s] port[%d]", l_strRemoteIP.c_str(), l_uRemoteport);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Get remote socket failed, err:[%s]", l_sysErrCode.message().c_str());
	}

	bool l_bClose = false;
	beast::error_code l_errorCode;

	// Construct the stream around the socket
	beast::ssl_stream<tcp::socket&> l_sslStream{ p_socket, p_ctx };

	// Perform the SSL handshake
	l_sslStream.handshake(ssl::stream_base::server, l_errorCode);
	if (l_errorCode)
	{
		std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d] handshake", l_strRemoteIP.c_str(), l_uRemoteport);
		return Fail(l_errorCode, l_strErr);
	}

	// This buffer is required to persist across reads
	beast::flat_buffer l_buffer;

	// This lambda is used to send messages
	//send_lambda<beast::ssl_stream<tcp::socket&>> l_SendLambda{ l_sslStream, l_bClose, l_errorCode };
	std::shared_ptr<send_lambda<beast::ssl_stream<tcp::socket&>>> l_pLambda = std::make_shared<send_lambda<beast::ssl_stream<tcp::socket&>>>(l_sslStream, l_bClose, l_errorCode);

	for (;;)
	{
		if (m_bStopServer) break;

		// Read a request
		http::request<http::string_body> l_reqMsg;
		http::read(l_sslStream, l_buffer, l_reqMsg, l_errorCode);
		if (l_errorCode == http::error::end_of_stream)
		{
			//std::cout << "=========== End of stream. ==========" << std::endl;
			ICC_LOG_DEBUG(m_pLog, "Remote conn IP[%s] port[%d], End of stream", l_strRemoteIP.c_str(), l_uRemoteport);

			break;
		}
		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], read", l_strRemoteIP.c_str(), l_uRemoteport);
			return Fail(l_errorCode, l_strErr);
		}

		// Send the response
		std::string l_strReqGuid = m_pString->CreateGuid();
		bool l_bHand =  HandleRequest(l_strReqGuid, std::move(l_reqMsg), l_pLambda);
		if (l_bHand)
		{
			bool l_bIsResp = false;
			while (!l_bIsResp)
			{
				bool l_bTimeout = false;
				{
					std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);
					if (m_conditionWaitResp.wait_for(l_lock, std::chrono::seconds(m_nRespTimeout)) == std::cv_status::timeout) 
						l_bTimeout = true;
				}

				if (l_bTimeout)
				{
					l_bIsResp = ProcessRespTimeout(l_strReqGuid);
				}
				else
				{
					l_bIsResp = IsResp(l_strReqGuid);
				}
			}

			DelWaitRespList(l_strReqGuid);
		}

		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], write", l_strRemoteIP.c_str(), l_uRemoteport);
			return Fail(l_errorCode, l_strErr);
		}

		if (l_bClose)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			std::cout << "Connection: close." << std::endl;
			ICC_LOG_DEBUG(m_pLog, "Remote conn IP[%s] Port[%d] close.", l_strRemoteIP.c_str(), l_uRemoteport);
			break;
		}
		
	}

	// Perform the SSL shutdown
	l_sslStream.shutdown(l_errorCode);
	if (l_errorCode)
	{
		std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], shutdown", l_strRemoteIP.c_str(), l_uRemoteport);
		return Fail(l_errorCode, l_strErr);
	}

	// At this point the connection is closed gracefully
}

void CHttpsServerImpl::ListenHttps()
{
	try
	{
		auto const l_address = net::ip::make_address(m_strLocalAddress);
		auto const l_port = static_cast<unsigned short>(std::stoi(m_strLocalPort));
		auto const l_docRoot = std::make_shared<std::string>(m_strDocRoot);

		// The io_context is required for all I/O
		net::io_context l_ioContext{ 1 };

		// The SSL context is required, and holds certificates 
		ssl::context l_sslCtx{ ssl::context::tlsv12 };
		// This holds the self-signed certificate used by the server
		l_sslCtx.set_password_callback(boost::bind(&CHttpsServerImpl::GetPassword, this));
		l_sslCtx.use_certificate_chain_file(m_strCrtFile);
		l_sslCtx.use_private_key_file(m_strKeyFile, boost::asio::ssl::context::pem);
		l_sslCtx.use_tmp_dh_file(m_strDhFile);

		// The acceptor receives incoming connections
		tcp::acceptor l_acceptor{ l_ioContext, { l_address, l_port } };
		std::cout << "Https Listen, IP " << m_strLocalAddress << " port " << l_port << std::endl;
		ICC_LOG_DEBUG(m_pLog, "Https Listen, IP[%s] port[%d]", m_strLocalAddress.c_str(), l_port);

		for (;;)
		{
			if (m_bStopServer) break;

			// This will receive the new connection
			tcp::socket l_socket{ l_ioContext };
			// Block until we get a connection
			l_acceptor.accept(l_socket);

			// Launch the session, transferring ownership of the socket
			std::thread{ std::bind(
				&CHttpsServerImpl::DoSession,
				this,
				std::move(l_socket),
				std::ref(l_sslCtx)) }.detach();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Listen Error: " << e.what() << std::endl;
		ICC_LOG_ERROR(m_pLog, "Listen Error [%s]", e.what());
	}
}

void CHttpsServerImpl::OnMessage()
{
	for (;;)
	{
		if (m_bStopServer) break;

		try
		{
			std::unique_lock<std::mutex> l_lock(m_mutexRequest);
			m_conditionRequest.wait(l_lock);

			ICC_LOG_LOWDEBUG(m_pLog, "Request Callback, List size [%d]", m_listRequest.size());
			for (auto l_reqObj : m_listRequest)
			{
				if (m_pCallback)
				{
					AddWaitRespList(l_reqObj);

					std::string l_strTemp = std::string(l_reqObj->m_pHttpReq->target());
					size_t l_pos = l_strTemp.rfind("/");
					std::string l_strCmd = l_strTemp.substr(l_pos + 1, l_strTemp.size() - l_pos - 1);

					std::string l_strGuid = l_reqObj->m_strReqGuid;
					std::string l_strTarget = l_strCmd;
					std::string l_strBody = l_reqObj->m_pHttpReq->body();

					m_pCallback->OnMessage(l_strGuid, l_strTarget, l_strBody);
				}
			}

			m_listRequest.clear();
		}
		catch (const std::exception& e)
		{
			ICC_LOG_ERROR(m_pLog, "Callback Error [%s]", e.what());
		}
	}
}