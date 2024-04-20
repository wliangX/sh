#pragma once

#include <IResource.h>
#ifdef WIN32
#include <config/IConfig.h>
#else
#include "IConfig.h"
#endif

namespace ICC
{
	namespace Config
	{
		class IConfigFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateConfig
			// FullName:  ACS::Log::IConfigFactory::CreateConfig
			// Access:    virtual public 
			// Returns:   ACS::IConfigPtr
			// Qualifier:
			//************************************
			virtual IConfigPtr CreateConfigConfig() = 0;
			virtual IConfigPtr CreateConfig() = 0;
			virtual IConfigPtr CreateStaticConfig() = 0;
			virtual IConfigPtr CreateSqlConfig() = 0;
		};

		typedef boost::shared_ptr<IConfigFactory> IConfigFactoryPtr;
	}
}

#define ICCIConfigFactoryResourceName "ICC.Com.IConfigFactory.ResourceName"

#define ICCSetIConfigFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIConfigFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIConfigFactoryNull \
this->GetResourceManager()->SetResource(ICCIConfigFactoryResourceName, nullptr) \

#define ICCGetIConfigFactory() \
boost::dynamic_pointer_cast<Config::IConfigFactory>(this->GetResourceManager()->GetResource(ICCIConfigFactoryResourceName))