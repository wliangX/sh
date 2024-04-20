#pragma once
#include "Boost.h"
#include "HttpClient/IHttpClientFactory.h"

namespace ICC
{
	class PluginResource
	{
	public:
		Log::ILogPtr m_pLog;

		Config::IConfigPtr m_pConfig;
		//JsonParser::IJsonPtr m_pJson;
		//std::recursive_mutex m_pJsonMutex;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		StringUtil::IStringUtilPtr	m_pString;
		// Redis::IRedisClientPtr m_pRedis;
		// ObserverPattern::IObserverCenterPtr m_pObserverCenter;

		IHttpClientPtr m_pHttpClient;

	};
}