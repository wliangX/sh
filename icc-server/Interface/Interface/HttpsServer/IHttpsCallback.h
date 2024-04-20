#pragma once
#include <IObject.h>

namespace ICC 
{
	class IHttpsCallback :
		public IObject
	{
	public:
		IHttpsCallback(){}
		virtual ~IHttpsCallback(){};

		//************************************
		// Method:    OnMessage
		// FullName:  ICC::IHttpsCallback::OnMessage
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: const std::string & p_strGuid	GUID，匹配应答用
		// Parameter: const std::string & p_strTarget	请求类型
		// Parameter: const std::string & p_strBody	请求内容
		//************************************
		virtual void OnMessage(const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody) = 0;
	};

	typedef boost::shared_ptr<IHttpsCallback> IHttpsCallbackPtr;

} /*namespace ICC*/