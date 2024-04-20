#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "SwitchClientFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_TSAPICLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetISwitchClientFactory(CSwitchClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetISwitchClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}