#pragma once
#include <string>
#include <IObject.h>
#include <AmqClient/IAsyncCallback.h>

namespace ICC 
{
	class IConsumer :
		public IObject
	{
	public:
		/*
		* ��ʼ�����ӵ�activemq�������Ҵ���һ�����ӣ�һ�����Ӵ���һ���Ự�߳�
		* @���� strBrokerURI
		*      ���ӵ�broker��URI.
		* @���� strClientID
		*      �ͻ���Ψһ��ʶ.
		* @���� strUserName
		*      ����MQ���������֤���û���.
		* @���� strPassword
		*      ����MQ���������֤������.
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*      
		*/
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string()) = 0;
		/*
		* ��������/����
		* @���� strTopic
		*      ��������.
		* @���� iMode
		*      ��Ϣģʽ:0->����   1->����.
		* @���� pCallback
		*      �첽������Ϣ�ص���
		* @���� strSelector
		*      Ҫʹ�õ���Ϣѡ����,ȱʡ������.
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Subscribe(std::string strDest, int iMode, IAsyncCallbackPtr pCallback, const std::string& strSelector = std::string()) = 0;
		/*
		*ȡ�����Ŀͻ��˴�����һ������/����
		* @���� strDest
		*      ����/��������.
		* @���� iMode
		*      ��Ϣģʽ:0->����   1->����.
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Unsubscribe(std::string strDest, int iMode) = 0;
		/*
		* �־û���������
		* @���� strTopic
		*      ��������.
		* @���� strName
		* 		���ڱ�ʶ���ĵ�����
		* @����strSelector
		* 		Ҫʹ�õ���Ϣѡ����
		* @���� pCallback
		*      �첽������Ϣ�ص���
		*  @����bNoLocal
		*  		bNoLocal��־Ĭ��Ϊfalse��������Ϊtrueʱ����������ֻ�ܽ��պ��Լ���ͬ����������������Ϣ���˱�־ֻ���������⣬�������ڶ���
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool DurableSubscribe(std::string strTopic, std::string strName, std::string strSelector, IAsyncCallbackPtr pCallback, bool bNoLocal = false) = 0;
		/*
		*ȡ���ͻ��˴�����һ���־û�����
		* @���� name
		*      ���ڱ�ʶ�־û����ĵ�����
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Unsubscribe(std::string strName) = 0;
	};

	typedef boost::shared_ptr<IConsumer> IConsumerPtr;

} /*namespace ICC*/