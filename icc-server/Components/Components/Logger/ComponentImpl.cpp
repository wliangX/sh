#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "LogFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_LOGGER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetILogFactory(CLogFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetILogFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}