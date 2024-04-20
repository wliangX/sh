#pragma once

#include <IResource.h>
#include <Udp/IUdp.h>

namespace ICC
{
	namespace Udp
	{
		class IUdpFactory :
			public IResource
		{
		public:

			//************************************
			// Method:    CreateUdp
			// FullName:  ACS::Udp::IUdpFactory::CreateUdp
			// Access:    virtual public 
			// Returns:   ACS::Udp::IUdpPtr
			// Qualifier:
			//************************************
			virtual IUdpPtr CreateUdp() = 0;
		};

		typedef boost::shared_ptr<IUdpFactory> IUdpFactoryPtr;
	}
}
#define ICCIUdpFactoryResourceName "ICC.Com.IUdpFactory.ResourceName"

#define ICCSetIUdpFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIUdpFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIUdpFactoryNull \
this->GetResourceManager()->SetResource(ICCIUdpFactoryResourceName, nullptr) \

#define ICCGetIUdpFactory() \
boost::dynamic_pointer_cast<Udp::IUdpFactory>(this->GetResourceManager()->GetResource(ICCIUdpFactoryResourceName))