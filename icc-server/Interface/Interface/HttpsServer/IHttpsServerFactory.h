#pragma once
#include <IResource.h>
#include <HttpsServer/IHttpsServer.h>

namespace ICC
{
	class IHttpsServerFactory :
		public IResource
	{
	public:
		//************************************
		// Method:    CreateHttpsServer
		// FullName:  ACS::IHttpsServerFactory::CreateHttpsServer
		// Access:    virtual public 
		// Returns:   ACS::IHttpsServerPtr
		// Qualifier:
		//************************************
		virtual IHttpsServerPtr CreateHttpsServer() = 0;
	};

	typedef boost::shared_ptr<IHttpsServerFactory> IHttpsServerFactoryPtr;
}

#define ICCIHttpsServerFactoryResourceName "ICC.Com.IHttpsServerFactory.ResourceName"

#define ICCSetIHttpsServerFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIHttpsServerFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIHttpsServerFactoryNull \
this->GetResourceManager()->SetResource(ICCIHttpsServerFactoryResourceName, nullptr) \

#define ICCGetIHttpsServerFactory() \
boost::dynamic_pointer_cast<ICC::IHttpsServerFactory>(this->GetResourceManager()->GetResource(ICCIHttpsServerFactoryResourceName))