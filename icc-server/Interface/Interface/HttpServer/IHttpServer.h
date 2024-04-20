#pragma once

#include <IObject.h>
#include "IHttpCallback.h"

namespace ICC
{

	/*
	* http������Э���ʽ
	*****************************************
	* ���� url �汾\r\n
	* ͷ�������� �� ͷ����ֵ\r\n
	* ͷ�������� �� ͷ����ֵ\r\n
	* ͷ�������� �� ͷ����ֵ\r\n
	* ......
	* ͷ�������� �� ͷ����ֵ\r\n\
	* \r\n
	* Э����
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
		// Parameter: const std::string & p_strLocalIP	�󶨵� IP
		// Parameter: unsigned short p_uLocalPort		�����Ķ˿�
		// Parameter: const std::string & p_strDocRoot	��Ŀ¼
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
		// Parameter: const std::string & p_strGuid	�����Ӧ�� GUID
		// Parameter: const std::string & p_strResp	Ӧ��
		//************************************
		virtual bool OnResponse(const std::string& p_strGuid, const std::string& p_strResp) = 0;

		//************************************
		// Method:    CheckHttpIsStop HTTP�Ƿ�ֹͣ����
		// FullName:  ICC::IHttpServer::CheckHttpIsStop
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool CheckHttpIsStop() = 0;
	};

	typedef boost::shared_ptr<IHttpServer> IHttpServerPtr;
}
