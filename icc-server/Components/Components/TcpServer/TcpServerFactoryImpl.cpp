#include "Boost.h"
#include "TcpServerFactoryImpl.h"
#include "TcpServerImpl.h"

ICCFactoryCppInitialize(CTcpServerFactoryImpl)

ITcpServerPtr CTcpServerFactoryImpl::CreateTcpServer()
{
	try
	{
		return boost::make_shared<CTcpServerImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
