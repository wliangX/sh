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
	ICCSetIDBConnFactory(CDBConnFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIDBConnFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
