#pragma once
#include <IResource.h>
#include <MessageCenter/IMessageCenter.h>

namespace ICC
{
	namespace MsgCenter
	{
		class IMessageCenterFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateMessageCenter
			// FullName:  ACS::IMessageCenterFactory::CreateMessageCenter
			// Access:    virtual public 
			// Returns:   ACS::IMessageCenterPtr
			// Qualifier:
			//************************************
			virtual IMessageCenterPtr CreateMessageCenter() = 0;
		};

		typedef boost::shared_ptr<IMessageCenterFactory> IMessageCenterFactoryPtr;
	}
}

#define ICCIMessageCenterFactoryResourceName "ICC.Com.IMessageCenterFactory.ResourceName"

#define ICCSetIMessageCenterFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIMessageCenterFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIMessageCenterFactoryNull \
this->GetResourceManager()->SetResource(ICCIMessageCenterFactoryResourceName, nullptr) \

#define ICCGetIMessageCenterFactory() \
boost::dynamic_pointer_cast<MsgCenter::IMessageCenterFactory>(this->GetResourceManager()->GetResource(ICCIMessageCenterFactoryResourceName))