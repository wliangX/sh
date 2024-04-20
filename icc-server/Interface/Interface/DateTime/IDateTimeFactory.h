#pragma once

#include <IResource.h>
#include <DateTime/IDateTime.h>

namespace ICC
{
	namespace DateTime
	{
		class IDateTimeFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateDateTime
			// FullName:  ACS::IDateTimeFactory::CreateDateTime
			// Access:    virtual public 
			// Returns:   ACS::IDateTimePtr
			// Qualifier:
			//************************************
			virtual IDateTimePtr CreateDateTime() = 0;
		};

		typedef boost::shared_ptr<IDateTimeFactory> IDateTimeFactoryPtr;
	}
}

#define ICCIDateTimeFactoryResourceName "ICC.Com.IDateTimeFactory.ResourceName"

#define ICCSetIDateTimeFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIDateTimeFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIDateTimeFactoryNull \
this->GetResourceManager()->SetResource(ICCIDateTimeFactoryResourceName, nullptr) \

#define ICCGetIDateTimeFactory() \
boost::dynamic_pointer_cast<DateTime::IDateTimeFactory>(this->GetResourceManager()->GetResource(ICCIDateTimeFactoryResourceName))
