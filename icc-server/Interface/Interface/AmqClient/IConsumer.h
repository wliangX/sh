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
		* 初始化连接到activemq服务器且创建一个连接，一个连接创建一个会话线程
		* @参数 strBrokerURI
		*      连接到broker的URI.
		* @参数 strClientID
		*      客户端唯一标识.
		* @参数 strUserName
		*      连接MQ代理身份验证的用户名.
		* @参数 strPassword
		*      连接MQ代理身份验证的密码.
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*      
		*/
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string()) = 0;
		/*
		* 订阅主题/队列
		* @参数 strTopic
		*      主题名称.
		* @参数 iMode
		*      消息模式:0->主题   1->队列.
		* @参数 pCallback
		*      异步接收消息回调类
		* @参数 strSelector
		*      要使用的消息选择器,缺省不过滤.
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Subscribe(std::string strDest, int iMode, IAsyncCallbackPtr pCallback, const std::string& strSelector = std::string()) = 0;
		/*
		*取消订阅客户端创建的一个主题/队列
		* @参数 strDest
		*      主题/队列名称.
		* @参数 iMode
		*      消息模式:0->主题   1->队列.
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Unsubscribe(std::string strDest, int iMode) = 0;
		/*
		* 持久化订阅主题
		* @参数 strTopic
		*      主题名称.
		* @参数 strName
		* 		用于标识订阅的名称
		* @参数strSelector
		* 		要使用的消息选择器
		* @参数 pCallback
		*      异步接收消息回调类
		*  @参数bNoLocal
		*  		bNoLocal标志默认为false，当设置为true时限制消费者只能接收和自己相同的连接所发布的消息，此标志只适用于主题，不适用于队列
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool DurableSubscribe(std::string strTopic, std::string strName, std::string strSelector, IAsyncCallbackPtr pCallback, bool bNoLocal = false) = 0;
		/*
		*取消客户端创建的一个持久化订阅
		* @参数 name
		*      用于标识持久化订阅的名称
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Unsubscribe(std::string strName) = 0;
	};

	typedef boost::shared_ptr<IConsumer> IConsumerPtr;

} /*namespace ICC*/