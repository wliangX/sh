#pragma once

#include <HttpsServer/IHttpsServerFactory.h>

namespace ICC
{
	class CHttpsServerFactoryImpl :
		public IHttpsServerFactory
	{
		ICCFactoryHeaderInitialize(CHttpsServerFactoryImpl)
	public:
		virtual IHttpsServerPtr CreateHttpsServer();
	};
}