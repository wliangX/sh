#pragma once
#include <TcpServer/ITcpServerFactory.h>

namespace ICC
{
	namespace Tcp
	{
		class CTcpServerFactoryImpl :
			public ITcpServerFactory
		{
			ICCFactoryHeaderInitialize(CTcpServerFactoryImpl)
		public:
			virtual ITcpServerPtr CreateTcpServer();
		};
	}
}