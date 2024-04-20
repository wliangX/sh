#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "TimerFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_TIMER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetITimerFactory(CTimerFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetITimerFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}