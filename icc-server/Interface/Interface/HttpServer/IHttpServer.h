#pragma once

#include <IObject.h>
#include "IHttpCallback.h"

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

	class IHttpServer :
		public IObject
	{
	public:
		//virtual void SetCallbackPtr(IHttpsCallbackPtr p_pCallback) = 0;

		//************************************
		// Method:    StartHttp
		// FullName:  ICC::IHttpServer::ListenHttps
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: const std::string & p_strLocalIP	绑定的 IP
		// Parameter: unsigned short p_uLocalPort		监听的端口
		// Parameter: const std::string & p_strDocRoot	根目录
		//************************************
		virtual bool StartHttp(IHttpCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
			const std::string& p_strDocRoot = std::string("/")) = 0;

		virtual bool StopHttp() = 0;

		//************************************
		// Method:    OnResponse
		// FullName:  ICC::IHttpServer::OnResponse
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: const std::string & p_strGuid	请求对应的 GUID
		// Parameter: const std::string & p_strResp	应答
		//************************************
		virtual bool OnResponse(const std::string& p_strGuid, const std::string& p_strResp) = 0;

		//************************************
		// Method:    CheckHttpIsStop HTTP是否停止服务
		// FullName:  ICC::IHttpServer::CheckHttpIsStop
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool CheckHttpIsStop() = 0;
	};

	typedef boost::shared_ptr<IHttpServer> IHttpServerPtr;
}
