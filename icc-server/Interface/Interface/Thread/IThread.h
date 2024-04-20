#pragma once

#include <string>
#include <IObject.h>

namespace ICC
{ 
	namespace Thread
	{
		/*
		* class   异步调用方法类
		* author  w16314
		* purpose
		* note
		*/
		class IMethodRequest
		{
		public:
			//************************************
			// Method:    AsyncCall
			// FullName:  ACS::IMethodRequest::AsyncCall
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: 用户实现的异步调用接口
			//************************************
			virtual void AsyncCall() = 0;
		};

		typedef boost::shared_ptr<IMethodRequest> IMethodRequestPtr;

		/*
		* class   线程类
		* author  w16314
		* purpose
		* note
		*/
		class IThread : public IObject
		{
		public:

			//************************************
			// Method:    Start
			// FullName:  ACS::IThread::Start
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 启动线程
			//************************************
			virtual bool Start() = 0;

			//************************************
			// Method:    Stop
			// FullName:  ACS::IThread::Stop
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: 停止线程
			//************************************
			virtual void Stop() = 0;

			//************************************
			// Method:    IsRunning
			// FullName:  ACS::IThread::IsRunning
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 是否运行
			//************************************
			virtual bool IsRunning() = 0;

			//************************************
			// Method:    Post
			// FullName:  ACS::IThread::Post
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 跨线程异步调用方式
			// Parameter: IMethodRequestPtr p_methodPtr
			//************************************
			virtual bool Post(IMethodRequestPtr p_methodPtr) = 0;
		};

		typedef boost::shared_ptr<IThread> IThreadPtr;
	}
}
