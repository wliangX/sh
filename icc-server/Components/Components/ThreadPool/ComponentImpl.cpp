#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "ThreadPoolFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_THREADPOOL;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIThreadPoolFactory(CThreadPoolFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIThreadPoolFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}