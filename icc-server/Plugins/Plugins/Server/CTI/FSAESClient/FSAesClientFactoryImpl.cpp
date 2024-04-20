#include "Boost.h"

#include "FSAesClientFactoryImpl.h"
#include "FSAesClientImpl.h"


ICCFactoryCppInitialize(CFSAesClientFactoryImpl)

SwitchClient::ISwitchClientPtr CFSAesClientFactoryImpl::CreateSwitchClient(ClientType p_nClientType)
{
	SwitchClient::ISwitchClientPtr l_pSwitchClient = nullptr;

	l_pSwitchClient = boost::make_shared<CFSAesClientImpl>(m_IResourceManagerPtr);

	return  l_pSwitchClient;
}
