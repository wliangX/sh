#include "Boost.h"
#include "PGClientImpl.h"
#include "PGClientFactoryImpl.h"

ICCFactoryCppInitialize(CPGClientFactoryImpl)
IPGClientPtr CPGClientFactoryImpl::CreatePGClient()
{
	return boost::make_shared<CPGClientImpl>(m_IResourceManagerPtr);
}
