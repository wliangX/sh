#pragma once
#include <TcpClient/ITcpClientFactory.h>

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   Tcp客户端工厂实现
		* author  w16314
		* purpose
		* note
		*/
		class CTcpClientFactoryImpl :
			public ITcpClientFactory
		{
			ICCFactoryHeaderInitialize(CTcpClientFactoryImpl)
		public:
			virtual ITcpClientPtr CreateTcpClient();
		};
	}
}