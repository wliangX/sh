#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "RedisClientImpl.h"
#include "RedisClientFactoryImpl.h"

unsigned int CComponentImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_REDISCLIENT;
}

void CComponentImpl::OnInit()
{

}

void CComponentImpl::OnStart()
{
	ICCSetIRedisClientFactory(CRedisClientFactoryImpl);
}

void CComponentImpl::OnStop()
{
	ICCSetIRedisClientFactoryNull;
}

void CComponentImpl::OnDestroy()
{

}