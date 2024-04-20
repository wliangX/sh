#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "FtpClientFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_FTPCLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIFtpClientFactory(CFtpClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIFtpClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}