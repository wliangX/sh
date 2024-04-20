#include "Boost.h"
#include "ThreadFactoryImpl.h"
#include "ThreadImpl.h"

ICCFactoryCppInitialize(CThreadFactoryImpl)

IThreadPtr CThreadFactoryImpl::CreateThread()
{
	try
	{
		return boost::make_shared<CThreadImpl>();
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
