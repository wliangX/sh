#include "Boost.h"
#include "LockFactoryImpl.h"
#include "LockImpl.h"

ICCFactoryCppInitialize(CLockFactoryImpl)

Lock::ILockPtr CLockFactoryImpl::CreateLock(LockType p_elockType)
{
	Lock::ILockPtr lockPtr = nullptr;
	switch (p_elockType)
	{
	case TypeMutex:
		lockPtr = boost::make_shared<Mutex>();
		break;
	case TypeTimeMutex:
		lockPtr = boost::make_shared<TimeMutex>();
		break;
	case TypeRecursiveMutex:
		lockPtr = boost::make_shared<RecursiveMutex>();
		break;
	case TypeRecursiveTimeMutex:
		lockPtr = boost::make_shared<RecursiveTimeMutex>();
		break;
	default:
		break;
	}

	return lockPtr;
}

