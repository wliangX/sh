#pragma once
#include <HelpTool/IHelpTool.h>
#include <IResource.h>
namespace ICC
{
	namespace HelpTool
	{
		class IHelpToolFactory :
			public IResource
		{
		public:
			virtual IHelpToolPtr CreateHelpTool() = 0;
		};

		typedef boost::shared_ptr<IHelpToolFactory> IHelpToolFactoryPtr;
	}
}

#define ICCHelpToolFactoryResourceName "ICC.Com.IHelpToolFactory.ResourceName"

#define ICCSetHelpToolFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCHelpToolFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetHelpToolFactoryNull \
this->GetResourceManager()->SetResource(ICCHelpToolFactoryResourceName, nullptr) \

#define ICCGetHelpToolFactory() \
boost::dynamic_pointer_cast<HelpTool::IHelpToolFactory>(this->GetResourceManager()->GetResource(ICCHelpToolFactoryResourceName))