#include "Boost.h"
#include "TimerFactoryImpl.h"
#include "TimerImpl.h"

ICCFactoryCppInitialize(CTimerFactoryImpl)

ITimerManagerPtr CTimerFactoryImpl::CreateTimerManager()
{
	return boost::make_shared<CTimerManagerImpl>(m_IResourceManagerPtr);
}
