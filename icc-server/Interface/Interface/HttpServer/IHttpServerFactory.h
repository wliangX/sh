#pragma once
#include <IResource.h>
#include <HttpServer/IHttpServer.h>

namespace ICC
{
	class IHttpServerFactory :
		public IResource
	{
	public:
		//************************************
		// Method:    CreateHttpServer
		// FullName:  ACS::IHttpServerFactory::CreateHttpServer
		// Access:    virtual public 
		// Returns:   ACS::IHttpServerPtr
		// Qualifier:
		//************************************
		virtual IHttpServerPtr CreateHttpServer() = 0;
	};

	typedef boost::shared_ptr<IHttpServerFactory> IHttpServerFactoryPtr;
}

#define ICCIHttpServerFactoryResourceName "ICC.Com.IHttpServerFactory.ResourceName"

#define ICCSetIHttpServerFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIHttpServerFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIHttpServerFactoryNull \
this->GetResourceManager()->SetResource(ICCIHttpServerFactoryResourceName, nullptr) \

#define ICCGetIHttpServerFactory() \
boost::dynamic_pointer_cast<ICC::IHttpServerFactory>(this->GetResourceManager()->GetResource(ICCIHttpServerFactoryResourceName))