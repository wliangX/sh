#pragma once

#include <Lock/ILockFactory.h>

namespace ICC
{
	namespace Lock
	{
		/*
		* class   �̹߳�����
		* author  w16314
		* purpose
		* note
		*/
		class CLockFactoryImpl : public ILockFactory
		{
			ICCFactoryHeaderInitialize(CLockFactoryImpl)
		public:
			virtual Lock::ILockPtr CreateLock(LockType p_elockType);
		};
	}
}