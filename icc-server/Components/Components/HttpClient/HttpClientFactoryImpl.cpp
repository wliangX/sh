#include "Boost.h"
#include "HttpClientFactoryImpl.h"
#include "HttpClientImpl.h"

ICCFactoryCppInitialize(CHttpClientFactoryImpl)

IHttpClientPtr CHttpClientFactoryImpl::CreateHttpClient()
{
	try
	{
		return boost::make_shared<CHttpClientImpl>(m_IResourceManagerPtr);
	}	
	catch (...)
	{
		return nullptr;
	}
	
}
