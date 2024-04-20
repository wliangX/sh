#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "HttpClientFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_HTTPCLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIHttpClientFactory(CHttpClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIHttpClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}