#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "JsonFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_JSONPARESER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIJsonFactory(CJsonFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIJsonFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}