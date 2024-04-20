#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "MessageCenterFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_MESSAGECENTER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIMessageCenterFactory(CMessageCenterFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIMessageCenterFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}