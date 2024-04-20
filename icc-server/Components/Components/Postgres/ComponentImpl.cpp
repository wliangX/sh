#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "PGClientFactoryImpl.h"
#include "ResultSetFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_POSTGRES;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{

	ICCSetIPGClientFactory(CPGClientFactoryImpl);
	ICCSetIResultSetFactory(CResultSetFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIResultSetFactoryNull;
	ICCSetIPGClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
