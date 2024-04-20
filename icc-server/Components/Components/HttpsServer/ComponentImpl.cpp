#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "HttpsServerFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_HTTPSSERVER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIHttpsServerFactory(CHttpsServerFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIHttpsServerFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}