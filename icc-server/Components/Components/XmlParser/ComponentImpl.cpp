#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "XmlFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_XMLPARSER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIXmlFactory(CXmlFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIXmlFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
