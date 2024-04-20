#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "FSAesClientFactoryImpl.h"

#define ICCSetIHCPClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIHCPClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIHCPClientFactoryNull \
this->GetResourceManager()->SetResource(ICCIHCPClientFactoryResourceName, nullptr) \

unsigned int CComponentImpl::GetPriorityLevel()
{
	//return PRIORITY_LEVEL_TSAPICLIENT;
	return PRIORITY_LEVEL_TSAPICLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIHCPClientFactory(CFSAesClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIHCPClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}