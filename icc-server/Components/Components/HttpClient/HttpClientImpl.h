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
		* class   HttpClientʵ����
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

			/*����put��Ϣ*/
			virtual std::string PutEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage);
			virtual std::string PutWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout);
		private:
			//����Post����
			void BuildPostRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request);

			//����Get����
			void BuildGetRequest(std::string p_strUrl, std::string p_strIp, std::string p_strPort, std::string p_strBody, std::ostream& out_request);

		private:
			// ��������
			void HandleRequestResolve(HttpRequestType p_httpRequestType, std::string p_strUrl, std::string p_strContent = "");

			// ������
			void HandleResolve(const boost::system::error_code& err,
				boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

			// ���Ӻ�
			void HandleConnect(const boost::system::error_code& err);

			// ���������
			void HandleWriteRequest(const boost::system::error_code& err);

			// ��ȡ��Ӧ��
			void HandleReadStatusLine(const boost::system::error_code& err);

			// ��ȡ��Ӧͷ��
			void HandleReadHeaders(const boost::system::error_code& err);

			// ��ȡ�������ݺ�
			void HandleReadContent(const boost::system::error_code& err);

		private:
			// ����ж�� Header���ϲ����õ� URL ���� Header һһ��Ӧ
			typedef struct _tUrlStruct
			{
				std::string m_strUrl;
				std::map<std::string, std::string> m_HeaderMap;
			}UrlStruct, * PUrlStruct;

			// Url����
			int ParseUrl(std::string p_strUrl, std::string& p_strIP, std::string& p_strPort, std::string& p_strPath);
			void PareUrlEx(std::string p_strUrls, std::list<std::string>& p_UrlList);
			void PareUrlEx(std::string p_strUrls, std::list<UrlStruct>& p_UrlList);

			void AddActiveUrl(const std::string& p_strUrl);
			void DelActiveUrl(const std::string& p_strUrl);
			bool FindActiveUrl(const std::string& p_strUrl);
			void ClearActiveUrlList();

			void _CloseSocket();

		private:
			//io���������¼���������
			boost::asio::io_service m_IoService;
			// ������
			boost::asio::ip::tcp::resolver m_resolver;
			// �׽���
			boost::asio::ip::tcp::socket m_socket;
			// ���󻺳���
			boost::asio::streambuf m_request;
			// ��Ӧ������
			boost::asio::streambuf m_response;
			// ��Ӧ����
			std::string m_strResponseContent;
			// http������
			int m_iHttpErrorCode;
			// ������Ϣ
			std::string m_strError;
		private:
			std::map<std::string, std::string> m_paramsMap;
			std::vector< std::map<std::string, std::string> > m_multiHeaders;
			// ��ǰ URL �����ַ
			std::list<std::string> m_ActiveUrlList;
			std::mutex	m_UrlListMutex;

			Log::ILogPtr m_pLog;
		};
	}
}