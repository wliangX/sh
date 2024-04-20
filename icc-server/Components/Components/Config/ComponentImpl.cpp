#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "ConfigImpl.h"
#include "ConfigFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_CONFIG;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIConfigFactory(CConfigFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIConfigFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}