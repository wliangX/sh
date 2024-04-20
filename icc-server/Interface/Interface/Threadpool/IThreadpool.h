#pragma once
#include <string>
#include <IObject.h>
#include <Thread/IThread.h>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   线程池接口类
		* author  w16314
		* purpose
		* note
		*/
		class IThreadPool : public IObject
		{
		public:
			//************************************
			// Method:    Post
			// FullName:  ACS::IThreadPool::Post
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier:
			// Parameter: IMethodRequestPtr p_methodPtr
			//************************************
			virtual bool Post(IMethodRequestPtr p_methodPtr) = 0;
		};

		typedef boost::shared_ptr<IThreadPool> IThreadPoolPtr;
	}
}