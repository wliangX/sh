#pragma once

#include <thread>
#include <condition_variable>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/thread.hpp>
#include "CommonWorkThread.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace ICC
{
	// This is the C++11 equivalent of a generic lambda.
	// The function object is used to send an HTTP message.
	template<class Stream>
	struct send_lambda
	{
		Stream& m_stream;
		bool&   m_close;
		beast::error_code& m_ec;

		explicit send_lambda(Stream& stream, bool& bclose, beast::error_code& ec)
			: m_stream(stream)
			, m_close(bclose)
			, m_ec(ec)
		{
			//
		}

		template<bool isRequest, class Body, class Fields>
		void operator()(http::message<isRequest, Body, Fields>&& msg) const
		{
			// Determine if we should close the connection after
			m_close = msg.need_eof();

			// We need the serializer here because the serializer requires
			// a non-const file_body, and the message oriented version of
			// http::write only works with const messages.
			http::serializer<isRequest, Body, Fields> sr{ msg };
			http::write(m_stream, sr, m_ec);
		}
	};

	typedef struct _sRequestObj
	{
		bool					m_bIsResp;
		std::string				m_strSrcIP;
		std::string				m_strReqGuid;
		std::shared_ptr<http::request<http::string_body>> m_pHttpReq;
		std::shared_ptr<send_lambda<tcp::socket&>> m_pReqStream;		
		std::shared_ptr<std::condition_variable> m_pConditionWait;
		std::shared_ptr<std::mutex>              m_pMutexExec;

		_sRequestObj() :
			m_bIsResp(false),
			m_strSrcIP(""),
			m_strReqGuid(""),
			m_pHttpReq(nullptr),
			m_pReqStream(nullptr),			
			m_pConditionWait(nullptr),
			m_pMutexExec(nullptr)
		{
			//
		}
	}RequestObj, * PRequestObj;

	class CHttpServerImpl
		: public IHttpServer, public CCommonThreadCore
	{

		typedef std::map <std::string, std::shared_ptr< RequestObj>> MapRequest;

	public:
		CHttpServerImpl(IResourceManagerPtr p_resourceManager);
		virtual ~CHttpServerImpl();

	public:
		virtual bool StartHttp(IHttpCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
			const std::string& p_strDocRoot = std::string("/"));
		virtual bool StopHttp();
		virtual bool OnResponse(const std::string& p_strReqGuid, const std::string& p_strRespBody);

		virtual bool CheckHttpIsStop();
	private:
		void LoadParam();
		beast::string_view MimeType(beast::string_view p_strPath);
		std::string PathCat(beast::string_view p_strBase, beast::string_view p_strPath);

		template<class Body, class Send>
		bool AddReqList(const std::string& p_strSrcIP, const std::string& p_strReqGuid, Body&& p_httpReq, Send&& p_send, 
			std::shared_ptr<std::condition_variable> pCondition, std::shared_ptr<std::mutex> pMutexExec);
		void ClearReqList();

		bool IsResp(const std::string& p_strReqGuid);
		void AddWaitRespList(std::shared_ptr<RequestObj> p_pReqObj);
		void DelWaitRespList(const std::string& p_strReqGuid);
		bool ProcessRespTimeout(const std::string& p_strReqGuid);
		void ClearWaitRespList();

		template<class Body, class Send, class Result>
		void DoRespond(Body&& p_httpReq, Send&& p_send, Result p_result, const std::string& p_strRespBody, const std::string& strGuid);
		template<class Body, class Allocator, class Send>
		bool HandleRequest(const std::string& p_strSrcIP, const std::string& p_strReqGuid, 
			http::request<Body, http::basic_fields<Allocator>>&& p_httpReq, Send&& p_send, std::shared_ptr<std::condition_variable> pCondition,
			std::shared_ptr<std::mutex> pMutexExec);
		void DoSession(tcp::socket& p_socket);
		void Fail(beast::error_code p_errorCode, const std::string& p_strWhat);	

		void ListenHttp();
		void OnMessage();

		virtual void ProcMessage(CommonThread_data msg_data);
		
	private:
		std::atomic_bool m_bStopServer;
		std::atomic_bool m_bStopServerEx;
		int			m_nRespTimeout;		//应用层应答超时时长（单位：秒）
		std::string m_strLocalAddress;
		std::string m_strLocalPort;
		std::string m_strDocRoot;

		std::string m_strReleaseAfterRequestComplete;
			
		Log::ILogPtr							m_pLog;
		Config::IConfigPtr						m_pConfig;
		DateTime::IDateTimePtr					m_pDateTime;
		StringUtil::IStringUtilPtr				m_pString;
		JsonParser::IJsonFactoryPtr				m_pJsonFty;
		IHttpCallbackPtr						m_pCallback;

		std::mutex								m_mutexRequest;
		std::mutex								m_mutexWaitAdd;
		std::mutex								m_mutexWaitResp;
		std::mutex								m_mutexWaitDelete;
		std::condition_variable					m_conditionRequest;		
		//std::condition_variable					m_conditionWaitResp;	
		std::list<std::shared_ptr<RequestObj>>	m_listRequest;
		//std::list<std::shared_ptr<RequestObj>>	m_listWaitResp;

		std::map <std::string, std::shared_ptr< RequestObj>> m_mapRequest;
		std::map<std::string, std::shared_ptr< RequestObj>> m_mapWaitResp;

		std::shared_ptr<std::thread>			m_threadListen;
		std::shared_ptr<std::thread>			m_threadReqCallback;

		CCommonWorkThread m_threadProcListen;
		unsigned int                            m_uLimitRequestCount;
	};
}