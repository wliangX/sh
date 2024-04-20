#include "Boost.h"
#include "HttpServerFactoryImpl.h"
#include "HttpServerImpl.h"

ICCFactoryCppInitialize(CHttpServerFactoryImpl)

IHttpServerPtr CHttpServerFactoryImpl::CreateHttpServer()
{
	try
	{
		return boost::make_shared<CHttpServerImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
