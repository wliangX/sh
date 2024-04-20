#pragma once

namespace ICC
{
	namespace Separate
	{
		//警情队列
		class CAlarmQueue
		{
		public:
			//添加接警单
			void AddAlarm(Alarm& p_alarm);
			//获取接警单
			bool GetAlarm(std::string p_strAlarmID, Alarm& p_alarm);
			//删除接警单
			void DelAlarm(std::string p_strAlarmID);

		public:
			IResourceManagerPtr GetResourceManager();
			static CAlarmQueue* Instance();
			bool Init(IResourceManagerPtr p_pResourceManager);
		public:
			static CAlarmQueue* sm_instance;
			static boost::mutex sm_mutexInstance;
		private:
			void OnTimer(ObserverPattern::INotificationPtr p_pNotify);
		private:
			CAlarmQueue();
			~CAlarmQueue();
		private:
			Log::ILogPtr m_pLog;
			Config::IConfigPtr m_pConfig;
			StringUtil::IStringUtilPtr m_pString;
			HelpTool::IHelpToolPtr m_pHelpTool;
			Redis::IRedisClientPtr m_pRedisClient;
			DateTime::IDateTimePtr m_pDateTime;
			Timer::ITimerManagerPtr m_pTimerMgr;
			JsonParser::IJsonFactoryPtr m_pJsonFty;			
			ObserverPattern::IObserverCenterPtr m_pObserverCenter;
			IResourceManagerPtr m_pResourceManager;

		private:
			bool m_bIsInit; //是否已初始化	
		};
	}
}