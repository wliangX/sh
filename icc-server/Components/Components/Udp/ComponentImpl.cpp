#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "UdpFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_UDP;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIUdpFactory(CUdpFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIUdpFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}