#pragma once
#include <Redisclient/IRedisClientFactory.h>

namespace ICC
{
	namespace Redis
	{
		class CRedisClientFactoryImpl:
			public IRedisClientFactory
		{
			ICCFactoryHeaderInitialize(CRedisClientFactoryImpl);
		public:
			virtual IRedisClientPtr CreateRedisClient();
		private:
			IRedisClientPtr m_pRedisClient;
		};
	}
}