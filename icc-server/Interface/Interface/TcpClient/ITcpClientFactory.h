#pragma once
#include <IResource.h>
#include <TcpClient/ITcpClient.h>

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   Tcp客户端工厂
		* author  w16314
		* purpose
		* note
		*/
		class ITcpClientFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateTcpClient
			// FullName:  ACS::ITcpClientFactory::CreateTcpClient
			// Access:    virtual public 
			// Returns:   ACS::ITcpClientPtr
			// Qualifier:
			//************************************
			virtual ITcpClientPtr CreateTcpClient() = 0;
		};

		typedef boost::shared_ptr<ITcpClientFactory> ITcpClientFactoryPtr;
	}
}

#define ICCITcpClientFactoryResourceName "ICC.Com.ITcpClientFactory.ResourceName"

#define ICCSetITcpClientFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCITcpClientFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetITcpClientFactoryNull \
this->GetResourceManager()->SetResource(ICCITcpClientFactoryResourceName, nullptr) \

#define ICCGetITcpClientFactory() \
boost::dynamic_pointer_cast<Tcp::ITcpClientFactory>(this->GetResourceManager()->GetResource(ICCITcpClientFactoryResourceName))