#pragma once

#include <HttpServer/IHttpServerFactory.h>

namespace ICC
{
	class CHttpServerFactoryImpl :
		public IHttpServerFactory
	{
		ICCFactoryHeaderInitialize(CHttpServerFactoryImpl)
	public:
		virtual IHttpServerPtr CreateHttpServer();
	};
}