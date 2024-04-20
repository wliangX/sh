#pragma once
#include <StringUtil/IStringUtil.h>

namespace ICC
{
	namespace StringUtil
	{
		class IStringFactory :
			public IResource
		{
		public:
			virtual IStringUtilPtr CreateString() = 0;
		};

		typedef boost::shared_ptr<IStringFactory> IStringFactoryPtr;
	}
}

#define ICCIStringFactoryResourceName "ICC.Com.IStringFactory.ResourceName"

#define ICCSetIStringFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIStringFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIStringFactoryNull \
this->GetResourceManager()->SetResource(ICCIStringFactoryResourceName, nullptr) \

#define ICCGetIStringFactory() \
boost::dynamic_pointer_cast<StringUtil::IStringFactory>(this->GetResourceManager()->GetResource(ICCIStringFactoryResourceName))