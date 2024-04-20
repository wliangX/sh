#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "DBConnFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_DBCONN;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIDBConnExFactory(CDBConnExFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIDBConnExFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
