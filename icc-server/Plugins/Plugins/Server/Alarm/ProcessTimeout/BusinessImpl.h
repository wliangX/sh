#pragma once

#define MQTYPE_TOPIC						"1"	//mq���ջ��ͷ�ʽ��1�����⣬0������
#define MQTYPE_QUEUE						"0"

#define DELAYSTARTTIMER						10	 //��ʱ���ӳ�����ʱ��

#define TOPIC_ALARM							("topic_alarm")
#define DEPT_TYPE_CODE						("dept_type_code")
#define TIMER_CMD_NAME						("alarm_process_timer")
#define GET_ALARM_PROCESS_REQUEST			("get_alarm_process_timeout_request")
#define GET_ALARM_PROCESS_RESPOND			("get_alarm_process_timeout_respond")
#define ALARM_PROCESS_SYNC					("alarm_process_timeout_sync")
#define SELECT_ICC_T_ALARM_AND_PROCESS		("select_icc_t_alarm_and_process")
#define SELECT_ICC_T_ALARM_AND_PROCESS_EX	("select_icc_t_alarm_and_process_ex")

namespace ICC
{    
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		void OnGetAlarmProTimout(ObserverPattern::INotificationPtr p_pNotify);
		void OnTimer(ObserverPattern::INotificationPtr p_pNotify);
		void LoadAlarmProcessTimeout();

	private:
		DataBase::IDBConnPtr		m_pDBConn;
		Config::IConfigPtr			m_pConfig;
		Log::ILogPtr				m_pLog;
		Lock::ILockFactoryPtr		m_pLockFty;
		Lock::ILockPtr				m_pAlarmLock;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr	m_pString;
		DateTime::IDateTimePtr		m_pDateTime;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		IResCallbackPtr				m_pRespCallback;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr		m_pRedisClient;			//redis�ͻ���
		DateTime::CDateTime         m_timerInterval;		//��ʱ���������ʱ��
		DateTime::CDateTime			m_ProcessIntervalTime;	//��ʱʱ��
		DateTime::CDateTime			m_Duration;				//��ʱ������Чʱ��
	};
}




