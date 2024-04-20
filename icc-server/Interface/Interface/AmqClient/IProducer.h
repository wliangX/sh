#pragma once
#include <string>
#include <map>
#include <IObject.h>

/************************************************************************/
/* C#封装的接口，AMQ头部属性都不开放设置(放到业务协议体中，业务模块处理)            */
/* 此处开放AMQ的头部属性，业务若无需求可不用设置                                */
/************************************************************************/
typedef struct _tagCMSHeader 
{
	std::map<std::string, std::string> msProperties;		//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, long long> mlProperties;			//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, int> mnProperties;				//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, float> mfProperties;				//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, double> mdProperties;				//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, unsigned char> mcProperties;		//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	std::map<std::string, bool> mbProperties;				//消息属性，设置AMQ消息属性，常用在消息过滤的场景
	_tagCMSHeader()
	{
		msProperties.clear();
		mlProperties.clear();
		mnProperties.clear();
		mfProperties.clear();
		mdProperties.clear();
		mcProperties.clear();
		mbProperties.clear();
	}
}CMSHeader, *PCMSHeader;

namespace ICC 
{
	class IProducer :
		public IObject
	{
	public:
		/*
		* 初始化连接到activemq服务器且创建一个连接会话线程，消息接收后自动确认
		* @参数 strBrokerURI
		*      连接到broker的URI.
		*      消息接收后确认模式:默认自动确认AUTO_ACKNOWLEDGE
		* @参数 strClientID
		*      客户端唯一标识.
		* @参数 strUserName
		*      连接MQ代理身份验证的用户名.
		* @参数 strPassword
		*      连接MQ代理身份验证的密码.
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string()) = 0;
		/*
		* 同步发送消息到主题或者队列，主题或者队列不存在时会自动创建一个主题/队列
		* @参数 strMsg
		*      消息体，Json/Xml格式.
		* @参数 strDest
		*      主题或者队列名称.
		* @参数 iMode
		*      消息模式:0->主题   1->队列
		* @参数 pHeader
		*      CMS消息头：xml/Json格式文本串，包括标准消息头参数及附加属性，如优先级，过期时间，消息选择器附加属性等
		*	   对CMS消息头没有需求时，不需要传参
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Send(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = NULL) = 0;
		/*
		* 异步发送消息到主题或者队列，主题或者队列不存在时会自动创建一个主题/队列
		* @参数 strMsg
		*      消息体，Json/Xml格式.
		* @参数 strDest
		*      主题或者队列名称.
		* @参数 p_iMode
		*      消息模式:0->主题   1->队列
		* @参数 pHeader
		*      CMS消息头：xml/Json格式文本串，包括标准消息头参数及附加属性，如优先级，过期时间，消息选择器附加属性等
		*	   对CMS消息头没有需求时，不需要传参
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Post(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = NULL) = 0;
		/*
		* 请求应答模式发送请求，注册接收回调接收消息
		* @参数 strMsg
		*      消息体，Json/Xml格式.
		* @参数 strDest
		*      队列名称.
		* @参数 pResCallback
		*      应答端消费消息后，响应消息回调
		* @参数 pHeader
		*      CMS消息头：xml/Json格式文本串，包括标准消息头参数及附加属性，如优先级，过期时间，消息选择器附加属性等
		*	   对CMS消息头没有需求时，不需要传参
		* @返回 true:	成功
		*	   false:	失败，失败原因会打印日志
		*/
		virtual bool Request(std::string strMsg, std::string strDest, IResCallbackPtr pResCallback, PCMSHeader pHeader = NULL) = 0;
	};

	typedef boost::shared_ptr<IProducer> IProducerPtr;

} /*namespace ICC*/