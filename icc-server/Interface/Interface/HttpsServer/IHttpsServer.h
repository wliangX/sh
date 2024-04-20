#pragma once

#include <IObject.h>
#include "IHttpsCallback.h"

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

	class IHttpsServer :
		public IObject
	{
	public:
		//virtual void SetCallbackPtr(IHttpsCallbackPtr p_pCallback) = 0;

		//************************************
		// Method:    StartHttps
		// FullName:  ICC::IHttpsServer::ListenHttps
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: const std::string & p_strLocalIP	�󶨵� IP
		// Parameter: unsigned short p_uLocalPort		�����Ķ˿�
		// Parameter: const std::string & p_strCrtFile	���� Crt �ļ�
		// Parameter: const std::string & p_strKeyFile	���� Key �ļ�
		// Parameter: const std::string & p_strDhFile	���� DH	�ļ�
		// Parameter: const std::string & p_strDocRoot	��Ŀ¼
		//************************************
		virtual bool StartHttps(IHttpsCallbackPtr p_pCallback, const std::string& p_strLocalIP, unsigned short p_uLocalPort,
			const std::string& p_strCrtFile, const std::string& p_strKeyFile, const std::string& p_strDhFile,
			const std::string& p_strDocRoot = std::string("/")) = 0;

		virtual bool StopHttps() = 0;

		//************************************
		// Method:    OnResponse
		// FullName:  ICC::IHttpsServer::OnResponse
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: const std::string & p_strGuid	�����Ӧ�� GUID
		// Parameter: const std::string & p_strResp	Ӧ��
		//************************************
		virtual bool OnResponse(const std::string& p_strGuid, const std::string& p_strResp) = 0;
	};

	typedef boost::shared_ptr<IHttpsServer> IHttpsServerPtr;
}
