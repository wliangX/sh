#pragma once

#include "Boost.h"

namespace ICC
{
	class CFSAesClientFactoryImpl :
		public ISwitchClientFactory
	{
		ICCFactoryHeaderInitialize(CFSAesClientFactoryImpl)

	public:
		virtual ISwitchClientPtr CreateSwitchClient(ClientType p_nClientType);
	};
}