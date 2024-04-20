#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "StringFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_STRINGUTIL;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIStringFactory(CStringFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIStringFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}