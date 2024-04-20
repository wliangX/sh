#include "Boost.h"
#include "RedisClientImpl.h"
#include "RedisClientFactoryImpl.h"

ICCFactoryCppInitialize(CRedisClientFactoryImpl)
IRedisClientPtr CRedisClientFactoryImpl::CreateRedisClient()
{
	if (!m_pRedisClient)
	{
		m_pRedisClient = boost::make_shared<CRedisClientImpl>(m_IResourceManagerPtr);
	}
	return m_pRedisClient;
}
