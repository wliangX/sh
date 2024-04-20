#pragma once
#include <IObject.h>

namespace ICC 
{
	class IResCallback :
		public IObject
	{
	public:
		IResCallback(){}
		virtual ~IResCallback(){};
		/*
		* Ӧ����Ϣ�ص�
		* @���� strMsg
		*      �û�������Ϣ��ɺ󣬸������Ӧ�����Ϣ����
		* @���� ��
		*/
		virtual void OnResponse(std::string strMsg) = 0;
	};

	typedef boost::shared_ptr<IResCallback> IResCallbackPtr;

	class IAsyncCallback :
		public IObject
	{
	public:
		IAsyncCallback(){}
		virtual ~IAsyncCallback(){};
		/*
		* �첽��������Ϣ�ص�
		* @���� strMsg
		*      �����߽��յ�����Ϣ����
		* @���� callback
		*      ������Ӧ��ģʽ���ò���ΪNULL
		*      ����Ӧ��ģʽʱ,AmqClient���ע��ò������û�������Ϣ��ص���ָ���OnResponse������Ӧ��
		* @���� ��
		*/
		virtual void OnMessage(std::string strMsg, IResCallbackPtr callback = NULL) = 0;
	};

	typedef boost::shared_ptr<IAsyncCallback> IAsyncCallbackPtr;

} /*namespace ICC*/