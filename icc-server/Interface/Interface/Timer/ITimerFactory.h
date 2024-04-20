#pragma once

#include <IResource.h>
#include <Timer/ITimer.h>

namespace ICC
{
	namespace Timer
	{
		/*
		* class   ��ʱ������
		* author  w16314
		* purpose
		* note
		*/
		class ITimerFactory : public IResource
		{
		public:
			//************************************
			// Method:    CreateTimerManager
			// FullName:  ACS::ITimerFactory::CreateTimerManager
			// Access:    virtual public 
			// Returns:   ACS::ITimerManagerPtr
			// Qualifier: ������ʱ��������
			//************************************
			virtual ITimerManagerPtr CreateTimerManager() = 0;
		};

		typedef boost::shared_ptr<ITimerFactory> ITimerFactoryPtr;
	}
}

#define ICCITimerFactoryResourceName "ICC.Com.ITimerFactory.ResourceName"

#define ICCSetITimerFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCITimerFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetITimerFactoryNull \
this->GetResourceManager()->SetResource(ICCITimerFactoryResourceName, nullptr) \

#define ICCGetITimerFactory() \
boost::dynamic_pointer_cast<Timer::ITimerFactory>(this->GetResourceManager()->GetResource(ICCITimerFactoryResourceName))

