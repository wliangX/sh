#pragma once
#include <IResource.h>
#include <Xml/IXml.h>

namespace ICC
{
	namespace Xml
	{
		class IXmlFactory :
			public IResource
		{
		public:
			virtual IXmlPtr CreateXml() = 0;
		};

		typedef boost::shared_ptr<IXmlFactory> IXmlFactoryPtr;
	}
}

#define ICCIXmlFactoryResourceName "ICC.Com.IXmlFactory.ResourceName"

#define ICCSetIXmlFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIXmlFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIXmlFactoryNull \
this->GetResourceManager()->SetResource(ICCIXmlFactoryResourceName, nullptr) \

#define ICCGetIXmlFactory() \
boost::dynamic_pointer_cast<Xml::IXmlFactory>(this->GetResourceManager()->GetResource(ICCIXmlFactoryResourceName))

