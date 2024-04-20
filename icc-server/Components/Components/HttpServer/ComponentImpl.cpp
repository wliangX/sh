#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "HttpServerFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_HTTPSSERVER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIHttpServerFactory(CHttpServerFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIHttpServerFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}