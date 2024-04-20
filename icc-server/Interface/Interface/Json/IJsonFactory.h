#pragma once
#include <IResource.h>
#include <Json/IJson.h>

namespace ICC
{
	namespace JsonParser
	{
		class IJsonFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateJson
			// FullName:  ACS::IJsonFactory::CreateJson
			// Access:    virtual public 
			// Returns:   ACS::IJsonPtr
			// Qualifier:
			//************************************
			virtual IJsonPtr CreateJson() = 0;
		};

		typedef boost::shared_ptr<IJsonFactory> IJsonFactoryPtr;
	}
}

#define ICCIJsonFactoryResourceName "ICC.Com.IJsonFactory.ResourceName"

#define ICCSetIJsonFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIJsonFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIJsonFactoryNull \
this->GetResourceManager()->SetResource(ICCIJsonFactoryResourceName, nullptr) \

#define ICCGetIJsonFactory() \
boost::dynamic_pointer_cast<JsonParser::IJsonFactory>(this->GetResourceManager()->GetResource(ICCIJsonFactoryResourceName))
