#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "ObserverFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_OBSERVER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIObserverFactory(CObserverFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIObserverFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}