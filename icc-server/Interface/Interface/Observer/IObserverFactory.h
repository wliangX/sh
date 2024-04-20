#pragma once

#include <IResource.h>
#include <Observer/IObserver.h>

#define BUSINESS_MESSAGE "business_message"

namespace ICC
{
	namespace ObserverPattern
	{
		/*
		* class   观察者工厂类
		* author  w16314
		* purpose
		* note    内部开线程处理，支持跨组件调用。
		*/
		class IObserverFactory : public IResource
		{
		public:
			//************************************
			// Method:    GetObserverCenter
			// FullName:  ICC::IObserverFactory::GetObserverCenter
			// Access:    virtual public 
			// Returns:   ICC::IObserverCenterPtr
			// Qualifier:
			// Parameter: std::string p_strObserverName
			//************************************
			virtual IObserverCenterPtr GetObserverCenter(std::string p_strObserverName) = 0;
			//************************************
			// Method:    GetObserverCenterList
			// FullName:  ICC::IObserverFactory::GetObserverCenterList
			// Access:    virtual public 
			// Returns:   ICC::IObserverCenterList
			// Qualifier:
			// Parameter: std::string p_strObserverName
			virtual IObserverCenterList GetObserverCenterList() = 0;
			//************************************
			// Method:    DestroyObserverCenter
			// FullName:  ICC::IObserverFactory::DestroyObserverCenter
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strObserverName
			//************************************
			virtual void DestroyObserverCenter(std::string p_strObserverName) = 0;
		};

		typedef boost::shared_ptr<IObserverFactory> IObserverFactoryPtr;
	}
}

#define ICCIObserverFactoryResourceName "ICC.Com.IObserverFactory.ResourceName"

#define ICCSetIObserverFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIObserverFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIObserverFactoryNull \
this->GetResourceManager()->SetResource(ICCIObserverFactoryResourceName, nullptr) \

#define ICCGetIObserverFactory() \
boost::dynamic_pointer_cast<ObserverPattern::IObserverFactory>(this->GetResourceManager()->GetResource(ICCIObserverFactoryResourceName))
