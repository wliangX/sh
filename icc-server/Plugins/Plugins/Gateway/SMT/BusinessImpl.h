#pragma once

namespace ICC
{
	//////////////////////////////////////////////////////////////////////////
	// ϵͳ���ƽ̨�Խ�
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		CBusinessImpl();
		~CBusinessImpl();
	
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCNotifiConnect(ObserverPattern::INotificationPtr p_pNotifiConnect);
		void OnCNotifiDisConnect(ObserverPattern::INotificationPtr p_pNotifiDisConnect);
	public:

		std::string BuildBody(const PROTOCOL::CConnect& p_connect, std::string p_strOpt);
		std::string BuildBody2(std::string p_strClientIP, std::string p_strSvcState, std::string p_strSvcID);

		// �������Ӵ�
		bool ParseUrl(std::string p_strUrl, std::string& p_strIP, std::string& p_strPort);

	public:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		Config::IConfigPtr m_pStaticConfig;
		HelpTool::IHelpToolPtr m_pHelpTool;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pString;
		Timer::ITimerManagerPtr	m_pTimerMgr;
		IHttpClientPtr m_pHttpClient;
		//Redis::IRedisClientPtr m_pRedisClient;
	private:
		std::string m_strUrl;  //ϵͳ���ƽ̨URL
		std::string m_strSystemCode;  //ϵͳ����
		std::string m_strSystemName;  //ϵͳ����
		std::string	m_strServiceID;	  //����ID

		std::map<std::string, std::string> m_mapMQConnectInfo;
		std::map<std::string, std::string> m_mapAlarmGuid;
	};
}