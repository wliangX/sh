#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "DateTimeFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_DATETIME;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIDateTimeFactory(CDateTimeFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIDateTimeFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}