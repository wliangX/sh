#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "TcpServerFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_TCPSERVER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetITcpServerFactory(CTcpServerFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetITcpServerFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}