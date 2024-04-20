#pragma once
#include <License/ILicense.h>
#include <IResource.h>
namespace ICC
{
	namespace License
	{
		class ILicenseFactory :
			public IResource
		{
		public:
			virtual ILicensePtr CreateLicense() = 0;
		};

		typedef boost::shared_ptr<ILicenseFactory> ILicenseFactoryPtr;
	}
}

#define ICCLicenseFactoryResourceName "ICC.Com.ILicenseFactory.ResourceName"

#define ICCSetLicenseFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCLicenseFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetLicenseFactoryNull \
this->GetResourceManager()->SetResource(ICCLicenseFactoryResourceName, nullptr) \

#define ICCGetLicenseFactory() \
boost::dynamic_pointer_cast<License::ILicenseFactory>(this->GetResourceManager()->GetResource(ICCLicenseFactoryResourceName))