#include "Boost.h"
#include "UdpFactoryImpl.h"
#include "UdpImpl.h"

ICCFactoryCppInitialize(CUdpFactoryImpl)

IUdpPtr CUdpFactoryImpl::CreateUdp()
{
	try
	{
		return boost::make_shared<CUdpImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
