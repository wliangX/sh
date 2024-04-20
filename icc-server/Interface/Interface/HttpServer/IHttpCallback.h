#pragma once
#include <IObject.h>

namespace ICC 
{
	class IHttpCallback :
		public IObject
	{
	public:
		IHttpCallback(){}
		virtual ~IHttpCallback(){};

		//************************************
		// Method:    OnMessage
		// FullName:  ICC::IHttpsCallback::OnMessage
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// // Parameter: const std::string & p_strSrcIP	����ԴIP
		// Parameter: const std::string & p_strGuid	GUID��ƥ��Ӧ����
		// Parameter: const std::string & p_strTarget	��������
		// Parameter: const std::string & p_strBody	��������
		//************************************
		virtual void OnMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody) = 0;
	};

	typedef boost::shared_ptr<IHttpCallback> IHttpCallbackPtr;

} /*namespace ICC*/