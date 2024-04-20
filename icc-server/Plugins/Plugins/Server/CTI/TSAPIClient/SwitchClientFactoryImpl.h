#pragma once

#include "Boost.h"

namespace ICC
{
	class CSwitchClientFactoryImpl :
		public ISwitchClientFactory
	{
		ICCFactoryHeaderInitialize(CSwitchClientFactoryImpl)

	public:
		virtual ISwitchClientPtr CreateSwitchClient(ClientType p_nClientType);
	};
}