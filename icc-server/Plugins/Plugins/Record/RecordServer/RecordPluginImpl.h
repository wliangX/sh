#pragma once
#include "PluginResource.h"


namespace ICC
{
	class RecordPluginImpl :
		public CPluginBase
	{
	public:
		virtual unsigned int GetPriorityLevel();
		virtual void OnStart();
		virtual void OnStop();

		virtual const char* GetPublicName() const
		{
			static const char plugName[] = { "RecordPluginImpl" };
			return plugName;
		}

	private:
		IBusinessPtr m_pBusiness;

		//IResourceManagerPtr m_pResourceManager;

	// above is shared same functions && members, 
	// use different class name instead of inherit it, to avoid name conflict
	public:
		RecordPluginImpl();
		virtual ~RecordPluginImpl();

		// std::string HandlerXMLProtocal(const std::string& xmlString);
		// std::string HandlerJSONProtocal(std::string& jsonString);

		boost::shared_ptr<PluginResource>	m_pRES;

		//Log::ILogPtr m_pLog;
		//JsonParser::IJsonPtr m_pJson;
		//std::recursive_mutex m_pJsonMutex;
		//JsonParser::IJsonFactoryPtr m_pJsonFty;

		//Redis::IRedisClientPtr m_pRedis;
		// ObserverPattern::IObserverCenterPtr m_pObserverCenter;

		//IHttpClientPtr m_pHttpClient;


	private:
		
		// std::map<std::string, IPluginPtr>* pPlugsMap;

		//void HandlerSubscribeRequest();
		//void HandlerUnsubscribeRequest();

		//void MakeTrainLocationNotify();


		
	};
}
