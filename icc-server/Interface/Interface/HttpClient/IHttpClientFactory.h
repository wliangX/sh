#pragma once
#include <IResource.h>
#include <HttpClient/IHttpClient.h>

namespace ICC
{
	namespace Http
	{
		class IHttpClientFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateHttpClient
			// FullName:  ACS::IHttpClientFactory::CreateHttpClient
			// Access:    virtual public 
			// Returns:   ACS::IHttpClientPtr
			// Qualifier:
			//************************************
			virtual IHttpClientPtr CreateHttpClient() = 0;
		};

		typedef boost::shared_ptr<IHttpClientFactory> IHttpClientFactoryPtr;
	}
}

#define ICCIHttpClientFactoryResourceName "ICC.Com.IHttpClientFactory.ResourceName"

#define ICCSetIHttpClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIHttpClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIHttpClientFactoryNull \
this->GetResourceManager()->SetResource(ICCIHttpClientFactoryResourceName, nullptr) \

#define ICCGetIHttpClientFactory() \
boost::dynamic_pointer_cast<Http::IHttpClientFactory>(this->GetResourceManager()->GetResource(ICCIHttpClientFactoryResourceName))