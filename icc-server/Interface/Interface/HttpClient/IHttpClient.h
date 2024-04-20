#pragma once

#include <IObject.h>

namespace ICC
{

	/*
	* http请求报文协议格式
	*****************************************
	* 方法 url 版本\r\n
	* 头部域名称 ： 头部域值\r\n
	* 头部域名称 ： 头部域值\r\n
	* 头部域名称 ： 头部域值\r\n
	* ......
	* 头部域名称 ： 头部域值\r\n\
	* \r\n
	* 协议体
	* ***************************************
	*/

	/*
	* class   Http接口
	* author  w16314
	* purpose
	* note    
	*/
	class IHttpClient :
		public IObject
	{
	public:
		virtual void SetHeaders(std::map<std::string, std::string> p_ParamsMap) = 0;

		virtual void SetHeadersEx(std::vector<std::map<std::string, std::string>> p_multiHeaders) = 0;

		//************************************
		// Method:    Get
		// FullName:  ICC::IHttpClient::Get
		// Access:    virtual public 
		// Returns:   std::string
		// Qualifier:
		// Parameter: std::string p_strServerUrl 服务Url
		// Parameter: std::string p_strContent   报文附属体（body）
		//************************************
		virtual std::string Get(std::string p_strServerUrl, std::string p_strContent) = 0;

		virtual std::string GetEx(const std::string& strServerIp, const std::string& strServerPort,  std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage) = 0;
		virtual std::string GetExEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp) = 0;

		virtual std::string GetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout) = 0;
		virtual std::string GetExWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, std::string& strLocalIp, unsigned int uTimeout) = 0;

		//************************************
		// Method:    Post
		// FullName:  ICC::IHttpClient::Post
		// Access:    virtual public 
		// Returns:   std::string
		// Qualifier:
		// Parameter: std::string p_strServerUrl  服务Url
		// Parameter: std::string p_strContent    报文附属体（body）
		//************************************
		virtual std::string Post(std::string p_strServerUrl, std::string p_strContent) = 0;
		virtual std::string PostEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage) = 0;
		virtual std::string DeleteEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage) = 0;
		
		virtual std::string PostWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout) = 0;
		virtual std::string DeletetWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout) = 0;
		//************************************
		// Method:    GetLastError
		// FullName:  ICC::IHttpClient::GetLastError
		// Access:    virtual public 
		// Returns:   std::string
		// Qualifier:
		//************************************
		virtual std::string GetLastError() = 0;

		//************************************
		// Method:    GetLastErrorCode
		// FullName:  ICC::IHttpClient::GetLastErrorCode
		// Access:    virtual public 
		// Returns:   int
		// Qualifier:
		//************************************
		virtual int GetLastErrorCode() = 0;

		/*获取连接的本地IP*/
		virtual bool GetLocalIP(const std::string& strServerIp, const std::string& strServerPort, std::string& strErrorMessage, std::string& strLocalIp) = 0;

		/*发送put消息*/
		virtual std::string PutEx(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage) = 0;
		virtual std::string PutWithTimeout(const std::string& strServerIp, const std::string& strServerPort, std::string& strTarget, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, std::string& strErrorMessage, unsigned int uTimeout) = 0;
	};

	typedef boost::shared_ptr<IHttpClient> IHttpClientPtr;
}
