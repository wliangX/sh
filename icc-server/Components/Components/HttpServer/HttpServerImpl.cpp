#include "Boost.h"
#include "HttpServerImpl.h"
#include <vector>

const unsigned long THREADID_LISTEN = 10000;

const std::string TIMEOUT_RESP =
"{ \
		\"header\":{ \
			\"result\":\"999\", \
			\"msg\":\"timeout\" \
		} \
	}";

CHttpServerImpl::CHttpServerImpl(IResourceManagerPtr p_resourceManager) :
	m_strLocalAddress("0.0.0.0"),
	m_strLocalPort("29090"),
	m_strDocRoot("/"),
	m_bStopServer(false),
	m_nRespTimeout(5),
	m_pCallback(nullptr)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_resourceManager)->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetResourceEx(Config::IConfigFactory, ICCIConfigFactoryResourceName, p_resourceManager)->CreateConfig();
	m_pString = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_resourceManager)->CreateString();
	m_pDateTime = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_resourceManager)->CreateDateTime();

	m_uLimitRequestCount = 200;
	m_bStopServer = false;
}

CHttpServerImpl::~CHttpServerImpl()
{
	m_bStopServer = true;
	m_bStopServerEx = true;
	m_threadReqCallback->join();
	m_threadListen->join();

	ClearReqList();
	ClearWaitRespList();
}

void CHttpServerImpl::LoadParam()
{
	std::string l_strRespTimeout = m_pConfig->GetValue("ICC/Component/HttpServer/RespTimeout", "5");
	m_nRespTimeout = std::atoi(l_strRespTimeout.c_str());

	std::string l_strRequestLimitCount = m_pConfig->GetValue("ICC/Component/HttpServer/RequestLimitCount", "200");
	if (!l_strRequestLimitCount.empty())
	{
		m_uLimitRequestCount = std::stoi(l_strRequestLimitCount);
		if (m_uLimitRequestCount > 1000)
		{
			m_uLimitRequestCount = 200;
		}
	}
	
	m_strReleaseAfterRequestComplete = m_pConfig->GetValue("ICC/Component/HttpServer/ReleaseFlagAfterRequestComplete", "1");

	ICC_LOG_INFO(m_pLog, "httpsever LoadParam complete!! request limit:[%u], ReleaseFlagAfterRequestComplete:[%s]", m_uLimitRequestCount, m_strReleaseAfterRequestComplete.c_str());
}
//////////////////////////////////////////////////////////////////////////

bool CHttpServerImpl::StartHttp(IHttpCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
	const std::string& p_strDocRoot/* = std::string("/")*/)
{
	LoadParam();

	m_strLocalAddress = p_strLocalIP;
	m_strLocalPort = std::to_string(p_uLocalPort);
	m_strDocRoot = p_strDocRoot;
	m_pCallback = p_pCallback;

	m_bStopServer = false;
	m_bStopServerEx = false;

	m_threadListen = std::make_shared<std::thread>(std::bind(&CHttpServerImpl::ListenHttp, this));
	m_threadReqCallback = std::make_shared<std::thread>(std::bind(&CHttpServerImpl::OnMessage, this));
	
	return true;
}
bool CHttpServerImpl::StopHttp()
{
	m_bStopServer = true;
	m_bStopServerEx = true;


	m_conditionRequest.notify_all();

	MapRequest::const_iterator itr;
	for (itr = m_mapWaitResp.begin(); itr != m_mapWaitResp.end(); ++itr)
	{
		itr->second->m_pConditionWait->notify_one();
	}

	boost::this_thread::sleep(boost::posix_time::millisec(100));
	
	//m_conditionWaitResp.notify_all();
	m_threadListen->join();
	m_threadReqCallback->join();
	ClearReqList();
	ClearWaitRespList();
	
	return true;
}

bool CHttpServerImpl::OnResponse(const std::string& p_strReqGuid, const std::string& p_strRespBody)
{
	if (m_bStopServer)
	{
		ICC_LOG_WARNING(m_pLog, "sever is stop!! not send, Guid:[%s]", p_strReqGuid.c_str());
		return false;
	}

	int iSize = 0;
	std::shared_ptr<RequestObj> pTmpObj = nullptr;
	{
		MapRequest::iterator itr;
		std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);
		iSize = m_mapWaitResp.size();
		itr = m_mapWaitResp.find(p_strReqGuid);
		if (itr != m_mapWaitResp.end())
		{
			pTmpObj = itr->second;
		}		
	}

	if (pTmpObj)
	{
		{
			std::lock_guard<std::mutex> lock(*pTmpObj->m_pMutexExec);
			DoRespond(pTmpObj->m_pHttpReq, pTmpObj->m_pReqStream, http::status::ok, p_strRespBody, p_strReqGuid);
		}		

		pTmpObj->m_bIsResp = true;
		//m_conditionWaitResp.notify_all();
		pTmpObj->m_pConditionWait->notify_one();

		ICC_LOG_DEBUG(m_pLog, "CHttpServerImpl::OnResponse complete, Guid:[%s], wait size: %d", p_strReqGuid.c_str(), iSize);

		return true;
	}
	

	//std::cout << "Not found req, Guid: " << p_strReqGuid.c_str() << std::endl;
	ICC_LOG_ERROR(m_pLog, "Not found req, Guid:[%s]", p_strReqGuid.c_str());

	return false;
}

bool CHttpServerImpl::CheckHttpIsStop()
{
	if (m_bStopServerEx)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////

template<class Body, class Send>
bool CHttpServerImpl::AddReqList(const std::string& p_strSrcIP, const std::string& p_strReqGuid, Body&& p_httpReq, Send&& p_send, 
	std::shared_ptr<std::condition_variable> pCondition, std::shared_ptr<std::mutex> pMutexExec)
{
	std::string strContent;
	std::shared_ptr<http::request<http::string_body>> pHttpReq = p_httpReq;
	if (pHttpReq)
	{
		strContent = pHttpReq->body();
	}
	
	ICC_LOG_DEBUG(m_pLog, "Add to Req List enter, Guid:[%s], src:[%s]", p_strReqGuid.c_str(), p_strSrcIP.c_str());
	ICC_LOG_LOWDEBUG(m_pLog, "Add to Req List, Guid:[%s], src:[%s], content:[%s]", p_strReqGuid.c_str(), p_strSrcIP.c_str(), strContent.c_str());

	{
		std::lock_guard<std::mutex> l_lock(m_mutexRequest);

		std::shared_ptr<RequestObj> l_reqObj = std::make_shared<RequestObj>();
		if (l_reqObj)
		{
			l_reqObj->m_strSrcIP = p_strSrcIP;
			l_reqObj->m_strReqGuid = p_strReqGuid;
			l_reqObj->m_pHttpReq = p_httpReq;
			l_reqObj->m_pReqStream = p_send;
			l_reqObj->m_pConditionWait = pCondition;
			l_reqObj->m_pMutexExec = pMutexExec;
			m_listRequest.push_back(l_reqObj);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Add to Req List alloc memory failed, Guid:[%s], src:[%s]", p_strReqGuid.c_str(), p_strSrcIP.c_str());
		}
	}		

	m_conditionRequest.notify_one();

	ICC_LOG_LOWDEBUG(m_pLog, "Add to Req List complete, Guid:[%s], src:[%s]", p_strReqGuid.c_str(), p_strSrcIP.c_str());

	return true;
}
void CHttpServerImpl::ClearReqList()
{
	std::lock_guard<std::mutex> l_lock(m_mutexRequest);

	m_listRequest.clear();
}

bool CHttpServerImpl::IsResp(const std::string& p_strReqGuid)
{	
	MapRequest::iterator itr;
	std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);	
	itr = m_mapWaitResp.find(p_strReqGuid);
	if (itr != m_mapWaitResp.end())
	{
		return itr->second->m_bIsResp;
	}	

	return false;
}

void CHttpServerImpl::AddWaitRespList(std::shared_ptr<RequestObj> p_pReqObj)
{
	std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);
	m_mapWaitResp.insert(std::make_pair(p_pReqObj->m_strReqGuid, p_pReqObj));	
}

void CHttpServerImpl::DelWaitRespList(const std::string& p_strReqGuid)
{
	ICC_LOG_DEBUG(m_pLog, "DelWaitRespList delete wait enter guid: %s", p_strReqGuid.c_str());

	int iSize = 0;
	{
		std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);
		m_mapWaitResp.erase(p_strReqGuid);
		iSize = m_mapWaitResp.size();
	}
	
	ICC_LOG_DEBUG(m_pLog, "delete wait guid: %s, WaitRespList Size:[%d]", p_strReqGuid.c_str(), iSize);	
}

bool CHttpServerImpl::ProcessRespTimeout(const std::string& p_strReqGuid)
{
	ICC_LOG_DEBUG(m_pLog, "Request timeout process enter guid: %s", p_strReqGuid.c_str());

	std::shared_ptr<RequestObj> pTmpObj = nullptr;
	{
		MapRequest::iterator itr;
		std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);
		ICC_LOG_LOWDEBUG(m_pLog, "Request timeout process enter 11 guid: %s", p_strReqGuid.c_str());
		itr = m_mapWaitResp.find(p_strReqGuid);
		if (itr != m_mapWaitResp.end())
		{
			pTmpObj = itr->second;
			m_mapWaitResp.erase(p_strReqGuid);
			ICC_LOG_DEBUG(m_pLog, "Request timeout process and delete guid: %s", p_strReqGuid.c_str());
		}

		ICC_LOG_LOWDEBUG(m_pLog, "Request timeout process enter 33 guid: %s", p_strReqGuid.c_str());
	}	

	if (pTmpObj)
	{
		ICC_LOG_WARNING(m_pLog, "Request timeout, Guid:[%s]", p_strReqGuid.c_str());

		{
			std::lock_guard<std::mutex> lock(*pTmpObj->m_pMutexExec);
			DoRespond(pTmpObj->m_pHttpReq, pTmpObj->m_pReqStream, http::status::request_timeout, TIMEOUT_RESP, p_strReqGuid);
		}		

		ICC_LOG_WARNING(m_pLog, "Request timeout process complete! Guid:[%s]", p_strReqGuid.c_str());

		pTmpObj->m_bIsResp = true;		

		return true;
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Request timeout not find obj!!! Guid:[%s]", p_strReqGuid.c_str());
	}

	return true;
}
void CHttpServerImpl::ClearWaitRespList()
{
	std::unique_lock<std::mutex> l_lock(m_mutexWaitResp);
	m_mapWaitResp.clear();	
}

// Return a reasonable mime type based on the extension of a file.
beast::string_view CHttpServerImpl::MimeType(beast::string_view p_strPath)
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
std::string CHttpServerImpl::PathCat(beast::string_view p_strBase, beast::string_view p_strPath)
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
void CHttpServerImpl::DoRespond(Body&& p_httpReq, Send&& p_send, Result p_result, const std::string& p_strRespBody, const std::string& strGuid)
{
	try
	{
		http::response<http::string_body> l_httpRes{ p_result/*http::status::ok*/, p_httpReq->version() };
		l_httpRes.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		l_httpRes.set(http::field::content_type, "text/html");
		l_httpRes.body() = p_strRespBody;
		l_httpRes.content_length(p_strRespBody.size());
		l_httpRes.keep_alive(p_httpReq->keep_alive());

		//std::cout << "Send Resp: " << p_strRespBody.c_str() << std::endl;
		ICC_LOG_DEBUG(m_pLog, "Send Resp begin, guid:[%s]", strGuid.c_str());

		//return p_send->operator()(std::move(l_httpRes));
		p_send->operator()(std::move(l_httpRes));

		ICC_LOG_DEBUG(m_pLog, "Send Resp complete, guid:[%s]", strGuid.c_str());
		ICC_LOG_LOWDEBUG(m_pLog, "Send Resp complete, guid:[%s], content:[%s]", strGuid.c_str(), p_strRespBody.c_str());
	}	
	catch (std::exception const& e)
	{
		ICC_LOG_ERROR(m_pLog, "DoRespond exception!!!. guid:%s err:%s", strGuid.c_str(),e.what());
	}	
}

// Report a failure
void CHttpServerImpl::Fail(beast::error_code p_errorCode, const std::string& p_strWhat)
{
	std::cerr << p_strWhat << ": " << p_errorCode.message() << std::endl;
	ICC_LOG_ERROR(m_pLog, "Fail, Error [%s] Code[%s]", p_strWhat.c_str(), p_errorCode.message().c_str());
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<class Body, class Allocator, class Send>
bool CHttpServerImpl::HandleRequest(const std::string& p_strSrcIP, const std::string& p_strReqGuid,
	http::request<Body, http::basic_fields<Allocator>>&& p_httpReq, Send&& p_send,
	std::shared_ptr<std::condition_variable> pCondition, std::shared_ptr<std::mutex> pMutexExec)
{
	//std::cout << "Req target: " << p_httpReq.target() << " Body: " << p_httpReq.body() << std::endl;
	ICC_LOG_DEBUG(m_pLog, "Req Target: [%s]", std::string(p_httpReq.target()).c_str());
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
		int iReqSize = 0;
		{
			std::lock_guard<std::mutex> l_lock(m_mutexWaitResp);
			//iReqSize = m_listWaitResp.size();
			iReqSize = m_mapWaitResp.size();
		}		

		if (iReqSize > m_uLimitRequestCount)
		{
			p_send->operator()(l_serverError("The number of requests exceeds the limit!!"));
			ICC_LOG_WARNING(m_pLog, "The number of requests exceeds the limit!! request:%d", iReqSize);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "The number of requests is:[%d]", iReqSize);

		std::shared_ptr<http::request<http::string_body>> l_pHttpReq = std::make_shared<http::request<http::string_body>>(p_httpReq);
		if (l_pHttpReq == nullptr)
		{
			p_send->operator()(l_serverError("alloc memory failed!!"));
			return false;
		}

		bool bAdd = AddReqList(p_strSrcIP, p_strReqGuid, l_pHttpReq, p_send, pCondition,pMutexExec);

		return bAdd;
	} 
	else
	{
		p_send->operator()(l_notFound(p_httpReq.target()));

		return false;
	}

	return false;
}

// Handles an HTTP server connection
void CHttpServerImpl::DoSession(tcp::socket& p_socket)
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

	// This buffer is required to persist across reads
	beast::flat_buffer l_buffer;

	// This lambda is used to send messages
	std::shared_ptr<send_lambda<tcp::socket&>> l_pLambda = std::make_shared<send_lambda<tcp::socket&>>(p_socket, l_bClose, l_errorCode);

	std::mutex mutexWait;
	std::shared_ptr<std::condition_variable> pConditionWait = std::make_shared<std::condition_variable>();
	std::shared_ptr<std::mutex> pMutexExec = std::make_shared<std::mutex>();

	for (;;)
	{
		if (m_bStopServer) break;

		// Read a request
		http::request<http::string_body> l_reqMsg;
		http::read(p_socket, l_buffer, l_reqMsg, l_errorCode);
		if (l_errorCode == http::error::end_of_stream)
		{
			//std::cout << "=========== End of stream. ==========" << std::endl;
			ICC_LOG_DEBUG(m_pLog, "Remote conn IP[%s] port[%d], End of stream", l_strRemoteIP.c_str(), l_uRemoteport);

			break;
		}
		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], read", l_strRemoteIP.c_str(), l_uRemoteport);
			Fail(l_errorCode, l_strErr);
			break;
		}		

		

		// Send the response
		std::string l_strReqGuid = m_pString->CreateGuid();
		bool l_bHand =  HandleRequest(l_strRemoteIP, l_strReqGuid, std::move(l_reqMsg), l_pLambda,pConditionWait, pMutexExec);
		if (l_bHand)
		{
			bool l_bIsResp = false;
			bool bTimeout = false;
			std::chrono::system_clock::time_point curBegin;
			curBegin = std::chrono::system_clock::now();
			while (!l_bIsResp)
			{
				bool l_bTimeout = false;
				{
					std::unique_lock<std::mutex> l_lock(mutexWait);
					if (pConditionWait->wait_for(l_lock, std::chrono::seconds(m_nRespTimeout)) == std::cv_status::timeout)
						l_bTimeout = true;
				}

				if (m_bStopServer) break;

				std::chrono::system_clock::time_point curEnd;
				curEnd = std::chrono::system_clock::now();
				unsigned long long ullInterval = std::chrono::duration_cast<std::chrono::seconds>(curEnd.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::seconds>(curBegin.time_since_epoch()).count();

				if (ullInterval >= m_nRespTimeout)
				{
					bTimeout = true;
				}

				if (l_bTimeout || bTimeout)
				{					
					l_bIsResp = ProcessRespTimeout(l_strReqGuid);
					ICC_LOG_DEBUG(m_pLog, "Request timeout send complete, Guid:[%s],Remote conn IP[%s] Port[%d]", l_strReqGuid.c_str(),
						l_strRemoteIP.c_str(), l_uRemoteport);
					break;
				}
				else
				{
					l_bIsResp = IsResp(l_strReqGuid);
				}
			}			

			DelWaitRespList(l_strReqGuid);

			if (m_strReleaseAfterRequestComplete == "1")
			{
				ICC_LOG_DEBUG(m_pLog, "Request complete will disconnect, Guid:[%s],Remote conn IP[%s] Port[%d]", l_strReqGuid.c_str(),
					l_strRemoteIP.c_str(), l_uRemoteport);
				break;
			}
		}
		else
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], request is error", l_strRemoteIP.c_str(), l_uRemoteport);
			Fail(l_errorCode, l_strErr);
			break;
		}

		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], write", l_strRemoteIP.c_str(), l_uRemoteport);
			Fail(l_errorCode, l_strErr);
			break;
		}

		if (l_bClose)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			std::cout << "Connection: close." << std::endl;
			ICC_LOG_DEBUG(m_pLog, "Remote conn IP[%s] Port[%d] l_bClose.", l_strRemoteIP.c_str(), l_uRemoteport);
			break;
		}
		
	}

	// Send a TCP shutdown
	//p_socket.shutdown(tcp::socket::shutdown_send, l_errorCode);	
	try
	{
		p_socket.shutdown(tcp::socket::shutdown_both, l_errorCode);
		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], shutdown", l_strRemoteIP.c_str(), l_uRemoteport);
			return Fail(l_errorCode, l_strErr);
		}

		p_socket.close(l_errorCode);
		if (l_errorCode)
		{
			std::string l_strErr = m_pString->Format("Remote conn IP[%s] Port[%d], close", l_strRemoteIP.c_str(), l_uRemoteport);
			return Fail(l_errorCode, l_strErr);
		}

		ICC_LOG_DEBUG(m_pLog, "Remote conn close success IP[%s] Port[%d] .", l_strRemoteIP.c_str(), l_uRemoteport);
	}	
	catch (std::exception const& e)
	{
		ICC_LOG_ERROR(m_pLog, "shutdown exception!!!. Remote conn IP[%s] Port[%d] err:%s", l_strRemoteIP.c_str(), l_uRemoteport, e.what());
	}
	

	// At this point the connection is closed gracefully
}

void CHttpServerImpl::ListenHttp()
{
	ICC_LOG_DEBUG(m_pLog, "ListenHttp enter, IP[%s] port[%s]", m_strLocalAddress.c_str(), m_strLocalPort.c_str());

	std::shared_ptr<tcp::acceptor> pAcceptor = nullptr;
	net::io_context l_ioContext{ 1 };

	try
	{
		auto const l_address = net::ip::make_address(m_strLocalAddress);
		auto const l_port = static_cast<unsigned short>(std::stoi(m_strLocalPort));
		auto const l_docRoot = std::make_shared<std::string>(m_strDocRoot);

		// The io_context is required for all I/O
		//pAcceptor = std::make_shared<tcp::acceptor>(l_ioContext, tcp::endpoint(l_address, l_port));

		boost::system::error_code err;		

		// The acceptor receives incoming connections
		tcp::acceptor l_acceptor{ l_ioContext, { l_address, l_port } };
		std::cout << "Http Listen, IP " << m_strLocalAddress << " port " << l_port << std::endl;
		ICC_LOG_DEBUG(m_pLog, "Http Listen, IP[%s] port[%d]", m_strLocalAddress.c_str(), l_port);

		for (;;)
		{
			if (m_bStopServer) break;

			// This will receive the new connection
			tcp::socket l_socket{ l_ioContext };
			// Block until we get a connection
			l_acceptor.accept(l_socket);
			//pAcceptor->accept(l_socket);

			ICC_LOG_DEBUG(m_pLog, "local accept ip: [%s]", l_socket.local_endpoint().address().to_string().c_str());
			l_socket.local_endpoint().address().to_string();			

			// Launch the session, transferring ownership of the socket
			std::thread{ std::bind(
				&CHttpServerImpl::DoSession,
				this,
				std::move(l_socket)) }.detach();
		}
	}
	catch (const std::exception& e)
	{		
		std::cerr << "Listen Error: " << e.what() << std::endl;
		ICC_LOG_ERROR(m_pLog, "Listen Error [%s]", e.what());

		ICC_LOG_ERROR(m_pLog, "system error!! will exit application!!");

		exit(0);

		/*m_bStopServer = true;
		m_conditionRequest.notify_all();
		
		MapRequest::const_iterator itr;
		for (itr = m_mapWaitResp.begin(); itr != m_mapWaitResp.end(); ++itr)
		{
			itr->second->m_pConditionWait->notify_one();
		}

		boost::this_thread::sleep(boost::posix_time::millisec(100));
		
		ClearReqList();
		ClearWaitRespList();*/

		//ICC_LOG_INFO(m_pLog, "free resource complete!!");
	}
	

	/*if (!m_bStopServerEx)
	{
		ICC_LOG_WARNING(m_pLog, "http server exception! will listen again!!! ");

		m_threadProcListen.AddMessage(this, THREADID_LISTEN);

		ICC_LOG_INFO(m_pLog, "http server exception! add lister message complete!!! ");
	}
	else
	{
		ICC_LOG_INFO(m_pLog, "http server listen stopped!!");
	}*/
}

void CHttpServerImpl::OnMessage()
{
	for (;;)
	{
		if (m_bStopServerEx) break;

		try
		{
			{
				std::unique_lock<std::mutex> l_lock(m_mutexWaitAdd);
				//m_conditionRequest.wait(l_lock);
				m_conditionRequest.wait_for(l_lock, std::chrono::seconds(1));
			}

			if (m_bStopServerEx) break;

			ICC_LOG_LOWDEBUG(m_pLog, "Request Callback, List size [%d]", m_listRequest.size());
			std::list<std::shared_ptr<RequestObj>> tmpRequests;
			{				
				std::lock_guard<std::mutex> l_lock(m_mutexRequest);			
				if (m_listRequest.size() == 0)
				{
					continue;
				}
				tmpRequests = m_listRequest;
				m_listRequest.clear();
			}		


			for (auto l_reqObj : tmpRequests)
			{
				if (m_pCallback)
				{
					AddWaitRespList(l_reqObj);

					std::string l_strTemp = std::string(l_reqObj->m_pHttpReq->target());
					size_t l_pos = l_strTemp.rfind("/");
					std::string l_strCmd = l_strTemp.substr(l_pos + 1, l_strTemp.size() - l_pos - 1);

					std::string l_strSrcIP= l_reqObj->m_strSrcIP;
					std::string l_strGuid = l_reqObj->m_strReqGuid;
					std::string l_strTarget = l_strCmd;
					std::string l_strBody = l_reqObj->m_pHttpReq->body();

					m_pCallback->OnMessage(l_strSrcIP, l_strGuid, l_strTarget, l_strBody);
				}
			}
			
		}
		catch (const std::exception& e)
		{
			ICC_LOG_ERROR(m_pLog, "Callback Error [%s]", e.what());
		}
	}
}

void CHttpServerImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{	
	case THREADID_LISTEN:
	{
		try
		{
			ICC_LOG_WARNING(m_pLog, "http server exception! will listen begin!!!");
			boost::this_thread::sleep(boost::posix_time::millisec(1000));
			m_threadListen->join();
			m_threadListen = nullptr;
			m_bStopServer = false;
			m_threadListen = std::make_shared<std::thread>(std::bind(&CHttpServerImpl::ListenHttp, this));

			ICC_LOG_INFO(m_pLog, "http server relisten complete!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "ProcMessage relisten exception!");
		}
	}
	break;
	default:
		break;
	}
}