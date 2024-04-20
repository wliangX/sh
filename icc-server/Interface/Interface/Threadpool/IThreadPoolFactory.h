#pragma once

#include <IResource.h>
#include <Threadpool/IThreadpool.h>

namespace ICC
{
	namespace Thread
	{
		class IThreadPoolFactory : public IResource
		{
		public:
			//************************************
			// Method:    CreateThreadPool
			// FullName:  ACS::IThreadPoolFactory::CreateThreadPool
			// Access:    virtual public 
			// Returns:   ACS::IThreadPoolPtr
			// Qualifier:
			// Parameter: unsigned int p_iThreadNum 线程数量
			//************************************
			virtual IThreadPoolPtr CreateThreadPool(unsigned int p_iThreadNum) = 0;
		};

		typedef boost::shared_ptr<IThreadPoolFactory> IThreadPoolFactoryPtr;
	}
}

#define ICCIThreadPoolFactoryResourceName "ICC.Com.IThreadPoolFactory.ResourceName"

#define ICCSetIThreadPoolFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIThreadPoolFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIThreadPoolFactoryNull \
this->GetResourceManager()->SetResource(ICCIThreadPoolFactoryResourceName, nullptr) \

#define ICCGetIThreadPoolFactory() \
boost::dynamic_pointer_cast<Thread::IThreadPoolFactory>(this->GetResourceManager()->GetResource(ICCIThreadPoolFactoryResourceName))
