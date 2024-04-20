#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "TcpClientFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_TCPCLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetITcpClientFactory(CTcpClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetITcpClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}