#pragma once
#include <IResource.h>
#include <TcpServer/ITcpServer.h>

namespace ICC
{
	namespace Tcp
	{
		class ITcpServerFactory :
			public IResource
		{
		public:
			//************************************
			// Method:    CreateTcpServer
			// FullName:  ACS::ITcpServerFactory::CreateTcpServer
			// Access:    virtual public 
			// Returns:   ACS::ITcpServerPtr
			// Qualifier:
			//************************************
			virtual ITcpServerPtr CreateTcpServer() = 0;
		};

		typedef boost::shared_ptr<ITcpServerFactory> ITcpServerFactoryPtr;
	}
}
#define ICCITcpServerFactoryResourceName "ICC.Com.ITcpServerFactory.ResourceName"

#define ICCSetITcpServerFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCITcpServerFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetITcpServerFactoryNull \
this->GetResourceManager()->SetResource(ICCITcpServerFactoryResourceName, nullptr) \

#define ICCGetITcpServerFactory() \
boost::dynamic_pointer_cast<Tcp::ITcpServerFactory>(this->GetResourceManager()->GetResource(ICCITcpServerFactoryResourceName))