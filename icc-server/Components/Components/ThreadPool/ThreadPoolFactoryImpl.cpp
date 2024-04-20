#include "Boost.h"
#include "ThreadPoolFactoryImpl.h"
#include "ThreadPoolImpl.h"

ICCFactoryCppInitialize(CThreadPoolFactoryImpl)

IThreadPoolPtr CThreadPoolFactoryImpl::CreateThreadPool(unsigned int p_iThreadNum)
{
    return boost::make_shared<CThreadPoolImpl>(p_iThreadNum);
}
