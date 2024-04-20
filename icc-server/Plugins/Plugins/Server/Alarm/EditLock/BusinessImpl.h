#pragma once

//��Ϣ������
#define TOPIC_NAME "topic_alarm"

namespace ICC
{    
	typedef struct AlarmEditLock
	{
		std::string m_strCode;			    //�û���
		std::string m_strLockTime;			//����ʱ��
		std::string m_strCreateTime;
	}AlarmEditLockNode;
	typedef std::map<std::string, AlarmEditLock> mapAlarmEditLock;


	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		//����༭��������
		void OnNotifiAlarmEditLockRequest(ObserverPattern::INotificationPtr p_pNotify);
		//����༭��������
		void OnNotifiAlarmEditUnlockRequest(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡ������״̬
		void OnNotifiGetAlarmEditlockStatusRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:        

		void OnTimer(ObserverPattern::INotificationPtr p_pNotify);
		//����ͬ��������
		void AlarmEditLockSync(PROTOCOL::CAlarmEditLockSync& p_pAlarmEditLockSync, 
			PROTOCOL::CAlarmEditLock& p_pRequest, std::string p_strSyncType);
		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
		void LoadAlarmEditLock();
	private:
        DataBase::IDBConnPtr m_pDBConn;

		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		Lock::ILockFactoryPtr m_pLockFty;
		Lock::ILockPtr m_pAlarmLock;

		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		Timer::ITimerManagerPtr m_pTimerMgr;

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}




