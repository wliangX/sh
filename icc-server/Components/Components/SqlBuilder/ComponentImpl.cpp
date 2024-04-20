#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "SqlBuilderFactoryImpl.h"
#include "SqlRequestFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_SQLBUILDER;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetISqlBuilderFactory(CSqlBuilderFactoryImpl);
	ICCSetISqlRequestFactory(CSqlRequestFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetISqlRequestFactoryNull;
	ICCSetISqlBuilderFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}
