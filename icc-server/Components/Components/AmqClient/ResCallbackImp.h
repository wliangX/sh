#pragma once
#include "Boost.h"
#include "AsyncConsumerImpl.h"

namespace ICC
{
	typedef struct _tagResponse
	{
		ConsumerMsg msg;							//���ѵ���Ϣ�ṹ
		Session* pSession;							//�Ự�߳�
		MessageProducer* pProducer;					//��Ϣ������
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
