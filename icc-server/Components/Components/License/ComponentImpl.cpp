#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "LicenseImpl.h"
#include "LicenseFactoryImpl.h"


unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_LICENSE;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetLicenseFactory(CLicenseFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetLicenseFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}