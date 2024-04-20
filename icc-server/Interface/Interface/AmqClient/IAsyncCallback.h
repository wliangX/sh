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
		* 应答消息回调
		* @参数 strMsg
		*      用户消费消息完成后，给请求端应答的消息内容
		* @返回 无
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
		* 异步消费者消息回调
		* @参数 strMsg
		*      消费者接收到的消息内容
		* @参数 callback
		*      非请求应答模式，该参数为NULL
		*      请求应答模式时,AmqClient组件注册该参数供用户消费消息后回调该指针的OnResponse方法做应答
		* @返回 无
		*/
		virtual void OnMessage(std::string strMsg, IResCallbackPtr callback = NULL) = 0;
	};

	typedef boost::shared_ptr<IAsyncCallback> IAsyncCallbackPtr;

} /*namespace ICC*/