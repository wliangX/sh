#pragma once
#include <IResource.h>
#include <PGClient/IPGClient.h>

namespace ICC
{
	namespace DataBase
	{
		class IPGClientFactory :
			public IResource
		{
		public:
			virtual IPGClientPtr CreatePGClient() = 0;
		};

		typedef boost::shared_ptr<IPGClientFactory> IPGClientFactoryPtr;
	}
}
//IPGClientFactory
#define ICCIPGClientFactoryResourceName "ICC.Com.IPGClientFactory.ResourceName"

#define ICCSetIPGClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIPGClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIPGClientFactoryNull \
this->GetResourceManager()->SetResource(ICCIPGClientFactoryResourceName, nullptr) \

#define ICCGetIPGClientFactory() \
boost::dynamic_pointer_cast<DataBase::IPGClientFactory>(this->GetResourceManager()->GetResource(ICCIPGClientFactoryResourceName))


//IPGClientExFactory
#define ICCIPGClientExFactoryResourceName "ICC.Com.IPGClientExFactory.ResourceName"

#define ICCSetIPGClientExFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIPGClientExFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIPGClientExFactoryNull \
this->GetResourceManager()->SetResource(ICCIPGClientExFactoryResourceName, nullptr) \

#define ICCGetIPGClientExFactory() \
boost::dynamic_pointer_cast<DataBase::IPGClientFactory>(this->GetResourceManager()->GetResource(ICCIPGClientExFactoryResourceName))