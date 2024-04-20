#pragma once

#include <IResource.h>
#include <IObject.h>
#include <CTIServer/ISwitchClient.h>

namespace ICC
{
	namespace SwitchClient
	{
		enum ClientType
		{
			ClientType_TSAPI = 0,
			ClientType_TAPI = 1,
			ClientType_HCP = 2,
		};

		class ISwitchClientFactory :
			public IResource
		{
		public:
			virtual ISwitchClientPtr CreateSwitchClient(ClientType p_nClientType) = 0;
		};

		typedef boost::shared_ptr<ISwitchClientFactory> ISwitchClientFactoryPtr;
	}
	
}




#define ICCISwitchClientFactoryResourceName "ICC.Com.ISwitchClientFactory.ResourceName"

#define ICCIHCPClientFactoryResourceName "ICC.Com.IHCPClientFactory.ResourceName"

#define ICCSetISwitchClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCISwitchClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetISwitchClientFactoryNull \
this->GetResourceManager()->SetResource(ICCISwitchClientFactoryResourceName, nullptr) \

#define ICCGetISwitchClientFactory() \
boost::dynamic_pointer_cast<ICC::SwitchClient::ISwitchClientFactory>(this->GetResourceManager()->GetResource(ICCISwitchClientFactoryResourceName))

#define ICCGetIHCPClientFactory() \
boost::dynamic_pointer_cast<ICC::SwitchClient::ISwitchClientFactory>(this->GetResourceManager()->GetResource(ICCIHCPClientFactoryResourceName))