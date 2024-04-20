#pragma once

namespace ICC
{
	namespace Separate
	{
		//�������
		class CAlarmQueue
		{
		public:
			//��ӽӾ���
			void AddAlarm(Alarm& p_alarm);
			//��ȡ�Ӿ���
			bool GetAlarm(std::string p_strAlarmID, Alarm& p_alarm);
			//ɾ���Ӿ���
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
			bool m_bIsInit; //�Ƿ��ѳ�ʼ��	
		};
	}
}