#pragma once

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <ostream>
#include <fstream>


using boost::asio::ip::tcp;
using namespace boost::placeholders;

namespace ICC
{
	namespace Http
	{
		enum HttpRequestType
		{
			Post,
			Get,
			Put
		};

		/*
		* class   HttpClient实现类
		* author  w16314
		* purpose
		* note
		*/
		class CHttpClientImpl
			: public IHttpClient
		{
		public:
			CHttpClientImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CHttpClientImpl();
		public:
			virtual void SetHeaders(std::map<std::string, std::string> p_ParamsMap);
			virtual void SetHeadersEx(std::vector< std::map<std::string, std::string> > p_multiHeaders);

			virtual std::string Get(std::string p_strServerUrl, std::string p_strContent);
			virtual std::string Post(std::string p_strServerUrl, std::string p_strContent);

			virtual std::string GetEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage);
			virtual std::string GetExEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp);
			virtual std::string PostEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage);
			virtual std::string DeleteEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage);

			virtual std::string GetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout);
			virtual std::string PostWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout);
			virtual std::string DeletetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout);
			virtual std::string GetExWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp, unsigned int uTimeout);


			virtual std::string GetLastError();
			virtual int GetLastErrorCode();

			virtual bool GetLocalIP(const std::string& strServerIp, const std::string& strServerPort, std::string& strErrorMessage, std::string& strLocalIp);

			/*发送put消息*/
			virtual std::string PutEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage);
			virtual std::string PutWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout);
		private:
			//构建Post请求
			void BuildPostRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request);

			//构建Get请求
			void BuildGetRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request);

		private:
			// 建立请求
			void HandleRequestResolve(HttpRequestType p_httpRequestType, std::string p_strUrl, std::string p_strContent = "");

			// 解析后
			void HandleResolve(const boost::system::error_code& err,
				boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

			// 连接后
			void HandleConnect(const boost::system::error_code& err);

			// 发送请求后
			void HandleWriteRequest(const boost::system::error_code& err);

			// 读取响应后
			void HandleReadStatusLine(const boost::system::error_code& err);

			// 读取响应头后
			void HandleReadHeaders(const boost::system::error_code& err);

			// 读取正文数据后
			void HandleReadContent(const boost::system::error_code& err);

		private:
			// 如果有多个 Header，上层配置的 URL 需与 Header 一一对应
			typedef struct _tUrlStruct
			{
				std::string m_strUrl;
				std::map<std::string, std::string> m_HeaderMap;
			}UrlStruct, * PUrlStruct;

			// Url解析
			int ParseUrl(std::string p_strUrl, std::string& p_strIP, std::string& p_strPort, std::string& p_strPath);
			void PareUrlEx(std::string p_strUrls, std::list<std::string>& p_UrlList);
			void PareUrlEx(std::string p_strUrls, std::list<UrlStruct>& p_UrlList);

			void AddActiveUrl(const std::string& p_strUrl);
			void DelActiveUrl(const std::string& p_strUrl);
			bool FindActiveUrl(const std::string& p_strUrl);
			void ClearActiveUrlList();

			void _CloseSocket();

		private:
			//io服务，用作事件驱动引擎
			boost::asio::io_service m_IoService;
			// 解析器
			boost::asio::ip::tcp::resolver m_resolver;
			// 套接字
			boost::asio::ip::tcp::socket m_socket;
			// 请求缓冲区
			boost::asio::streambuf m_request;
			// 响应缓冲区
			boost::asio::streambuf m_response;
			// 响应数据
			std::string m_strResponseContent;
			// http错误码
			int m_iHttpErrorCode;
			// 错误信息
			std::string m_strError;
		private:
			std::map<std::string, std::string> m_paramsMap;
			std::vector< std::map<std::string, std::string> > m_multiHeaders;
			// 当前 URL 服务地址
			std::list<std::string> m_ActiveUrlList;
			std::mutex	m_UrlListMutex;

			Log::ILogPtr m_pLog;
		};
	}
}