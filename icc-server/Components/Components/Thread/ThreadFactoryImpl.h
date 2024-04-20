#pragma once

#include <Thread/IThreadFactory.h>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   �̹߳���ʵ��
		* author  w16314
		* purpose
		* note
		*/
		class CThreadFactoryImpl : public IThreadFactory
		{
			ICCFactoryHeaderInitialize(CThreadFactoryImpl)
		public:
			//�����߳�
			virtual IThreadPtr CreateThread();
		};
	}
}