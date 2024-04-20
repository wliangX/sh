#pragma once

#include <Timer/ITimerFactory.h>

namespace ICC
{
	namespace Timer
	{
		/*
		* class   ��ʱ������ʵ��
		* author  w16314
		* purpose
		* note
		*/
		class CTimerFactoryImpl : public ITimerFactory
		{
			ICCFactoryHeaderInitialize(CTimerFactoryImpl)
		public:
			virtual ITimerManagerPtr CreateTimerManager();
		};
	}
}