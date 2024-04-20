#include "Boost.h"
#include "PGClientImpl.h"
#include "PGClientFactoryImpl.h"

ICCFactoryCppInitialize(CPGClientExFactoryImpl)
IPGClientPtr CPGClientExFactoryImpl::CreatePGClient()
{
	return boost::make_shared<CPGClientExImpl>(m_IResourceManagerPtr);
}
