#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "LockFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_LOCK;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetILockFactory(CLockFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetILockFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}