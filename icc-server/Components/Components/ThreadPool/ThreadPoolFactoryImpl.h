#pragma once

#include <Threadpool/IThreadPoolFactory.h>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   �̹߳�����
		* author  w16314
		* purpose
		* note
		*/
		class CThreadPoolFactoryImpl : public IThreadPoolFactory
		{
			ICCFactoryHeaderInitialize(CThreadPoolFactoryImpl)
		public:
			virtual IThreadPoolPtr CreateThreadPool(unsigned int p_iThreadNum);

		};
	}
}