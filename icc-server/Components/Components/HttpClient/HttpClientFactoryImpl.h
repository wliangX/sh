#pragma once

#include <HttpClient/IHttpClientFactory.h>

namespace ICC
{
	namespace Http
	{
		class CHttpClientFactoryImpl :
			public IHttpClientFactory
		{
			ICCFactoryHeaderInitialize(CHttpClientFactoryImpl)
		public:
			virtual IHttpClientPtr CreateHttpClient();
		};
	}
}