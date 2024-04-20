#pragma once

#include <Timer/ITimerFactory.h>

namespace ICC
{
	namespace Timer
	{
		/*
		* class   定时器工厂实现
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