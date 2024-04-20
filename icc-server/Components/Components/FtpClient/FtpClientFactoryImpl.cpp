#include "Boost.h"
#include "FtpClientFactoryImpl.h"
#include "FtpClientImpl.h"

ICCFactoryCppInitialize(CFtpClientFactoryImpl)

IFtpClientPtr CFtpClientFactoryImpl::CreateFtpClient()
{
	return boost::make_shared<CFtpClientImpl>(m_IResourceManagerPtr);
}
