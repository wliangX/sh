#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "HelpToolImpl.h"
#include "HelpToolFactoryImpl.h"


unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_HELPTOOL;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetHelpToolFactory(CHelpToolFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetHelpToolFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}