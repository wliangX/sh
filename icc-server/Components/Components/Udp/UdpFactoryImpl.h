#pragma once

namespace ICC
{
	namespace Udp
	{
		class CUdpFactoryImpl :
			public IUdpFactory
		{
			ICCFactoryHeaderInitialize(CUdpFactoryImpl)
		public:
			virtual IUdpPtr CreateUdp();
		};
	}
}