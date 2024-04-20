#include "Boost.h"
#include "HttpsServerFactoryImpl.h"
#include "HttpsServerImpl.h"

ICCFactoryCppInitialize(CHttpsServerFactoryImpl)

IHttpsServerPtr CHttpsServerFactoryImpl::CreateHttpsServer()
{
	try
	{
		return boost::make_shared<CHttpsServerImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
