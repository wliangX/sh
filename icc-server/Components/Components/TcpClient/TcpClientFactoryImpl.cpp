#include "Boost.h"
#include "TcpClientFactoryImpl.h"
#include "TcpClientImpl.h"

ICCFactoryCppInitialize(CTcpClientFactoryImpl)

ITcpClientPtr CTcpClientFactoryImpl::CreateTcpClient()
{
	try
	{
		return boost::make_shared<CTcpClientImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
