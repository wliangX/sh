#pragma once
#include <IResource.h>
#include <Log/ILog.h>

namespace ICC
{
	namespace Log
	{
		/*
		* class   日志工厂类
		* author  w16314
		* purpose
		* note
		*/
		class ILogFactory :
			public IResource
		{
		public:

			//************************************
			// Method:    GetLogger
			// FullName:  ACS::Log::ILogFactory::GetLogger
			// Access:    virtual public 
			// Returns:   ACS::Log::ILogPtr
			// Qualifier:
			// Parameter: const std::string & p_strModuleName
			//************************************
			virtual ILogPtr GetLogger(const std::string& p_strModuleName) = 0;
		};

		typedef boost::shared_ptr<ILogFactory> ILogFactoryPtr;
	}
}

#define ICCILogFactoryResourceName "ICC.Com.ILogFactory.ResourceName"

#define ICCSetILogFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCILogFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetILogFactoryNull \
this->GetResourceManager()->SetResource(ICCILogFactoryResourceName, nullptr) \

#define ICCGetILogFactory() \
boost::dynamic_pointer_cast<Log::ILogFactory>(this->GetResourceManager()->GetResource(ICCILogFactoryResourceName))
