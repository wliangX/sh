#pragma once
#include "Boost.h"
#include "AsyncConsumerImpl.h"

namespace ICC
{
	typedef struct _tagResponse
	{
		ConsumerMsg msg;							//消费的消息结构
		Session* pSession;							//会话线程
		MessageProducer* pProducer;					//消息生产者
		int nCMSPriority;
		long long lCMSExpiration;
		_tagResponse()
		{
			pSession = nullptr;
			pProducer = nullptr;
			nCMSPriority = 4;
			lCMSExpiration = 60000;
		}
	}Response, *PResponse;

	class CResCallbackImp : public IResCallback
	{
	public:
		CResCallbackImp(ICC::Response Msg, Log::ILogPtr pLog);
		~CResCallbackImp();
	public:
		virtual void OnResponse(std::string strMsg);
	private:
		Response m_Response;
		Log::ILogPtr m_pLog;
	};

} /*namespace ICC*/
