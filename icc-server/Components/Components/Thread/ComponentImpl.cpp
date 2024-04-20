#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "ThreadFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_THREAD;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIThreadFactory(CThreadFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIThreadFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
