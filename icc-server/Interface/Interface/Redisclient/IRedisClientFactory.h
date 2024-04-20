#pragma once

#include <string>
#include <IResource.h>
#include <Redisclient/IRedisclient.h>

namespace ICC
{
	namespace Redis
	{
		class IRedisClientFactory :
			public IResource
		{
		public:
			virtual IRedisClientPtr CreateRedisClient() = 0;
		};

			
		typedef boost::shared_ptr<IRedisClientFactory> IRedisClientFactoryPtr;
	}
}

#define ICCIRedisClientFactoryResourceName "ICC.Com.IRedisClientFactory.ResourceName"

#define ICCSetIRedisClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIRedisClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIRedisClientFactoryNull \
this->GetResourceManager()->SetResource(ICCIRedisClientFactoryResourceName, nullptr) \

#define ICCGetIRedisClientFactory() \
boost::dynamic_pointer_cast<Redis::IRedisClientFactory>(this->GetResourceManager()->GetResource(ICCIRedisClientFactoryResourceName))