#pragma once
#include <IResource.h>
#include <FtpClient/IFtpClient.h>

namespace ICC
{
	namespace Ftp
	{
		class IFtpClientFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateFtpClient
			// FullName:  ACS::IFtpClientFactory::CreateFtpClient
			// Access:    virtual public 
			// Returns:   ACS::IFtpClientPtr
			// Qualifier:
			//************************************
			virtual IFtpClientPtr CreateFtpClient() = 0;
		};

		typedef boost::shared_ptr<IFtpClientFactory> IFtpClientFactoryPtr;
	}
}
#define ICCIFtpClientFactoryResourceName "ICC.Com.IFtpClientFactory.ResourceName"

#define ICCSetIFtpClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIFtpClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIFtpClientFactoryNull \
this->GetResourceManager()->SetResource(ICCIFtpClientFactoryResourceName, nullptr) \

#define ICCGetIFtpClientFactory() \
boost::dynamic_pointer_cast<Ftp::IFtpClientFactory>(this->GetResourceManager()->GetResource(ICCIFtpClientFactoryResourceName))