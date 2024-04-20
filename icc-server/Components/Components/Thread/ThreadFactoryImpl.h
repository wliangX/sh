#pragma once

#include <Thread/IThreadFactory.h>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   线程工厂实现
		* author  w16314
		* purpose
		* note
		*/
		class CThreadFactoryImpl : public IThreadFactory
		{
			ICCFactoryHeaderInitialize(CThreadFactoryImpl)
		public:
			//创建线程
			virtual IThreadPtr CreateThread();
		};
	}
}