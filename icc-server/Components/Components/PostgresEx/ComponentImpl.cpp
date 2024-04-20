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

	ICCSetIPGClientExFactory(CPGClientExFactoryImpl);
	ICCSetIResultSetFactory(CResultSetFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIResultSetFactoryNull;
	ICCSetIPGClientExFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
