#pragma once

#include <IResource.h>
#include <Thread/IThread.h>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   线程工厂
		* author  w16314
		* purpose
		* note
		*/
		class IThreadFactory : public IResource
		{
		public:
			//************************************
			// Method:    CreateThread
			// FullName:  ACS::IThreadFactory::CreateThread
			// Access:    virtual public 
			// Returns:   ACS::IThreadPtr
			// Qualifier: 创建线程
			//************************************
			virtual IThreadPtr CreateThread() = 0;
		};

		typedef boost::shared_ptr<IThreadFactory> IThreadFactoryPtr;
	}
}

#define ICCIThreadFactoryResourceName "ICC.Com.IThreadFactory.ResourceName"

#define ICCSetIThreadFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIThreadFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIThreadFactoryNull \
this->GetResourceManager()->SetResource(ICCIThreadFactoryResourceName, nullptr) \

#define ICCGetIThreadFactory() \
boost::dynamic_pointer_cast<Thread::IThreadFactory>(this->GetResourceManager()->GetResource(ICCIThreadFactoryResourceName))