#include "Boost.h"

#include "SwitchClientFactoryImpl.h"
#include "SwitchClientImpl.h"


ICCFactoryCppInitialize(CSwitchClientFactoryImpl)

SwitchClient::ISwitchClientPtr CSwitchClientFactoryImpl::CreateSwitchClient(ClientType p_nClientType)
{
	SwitchClient::ISwitchClientPtr l_pSwitchClient = nullptr;

	switch (p_nClientType)
	{
	case ClientType_TSAPI:
		l_pSwitchClient = boost::make_shared<CSwitchClientImpl>(m_IResourceManagerPtr);
		break;
	case ClientType_TAPI:
		break;
	default:
		break;
	}

	return  l_pSwitchClient;
}
