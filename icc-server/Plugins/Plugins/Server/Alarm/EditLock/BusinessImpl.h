#pragma once

//消息主题名
#define TOPIC_NAME "topic_alarm"

namespace ICC
{    
	typedef struct AlarmEditLock
	{
		std::string m_strCode;			    //用户名
		std::string m_strLockTime;			//锁定时间
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
		//警情编辑加锁请求
		void OnNotifiAlarmEditLockRequest(ObserverPattern::INotificationPtr p_pNotify);
		//警情编辑解锁请求
		void OnNotifiAlarmEditUnlockRequest(ObserverPattern::INotificationPtr p_pNotify);
		//获取警情锁状态
		void OnNotifiGetAlarmEditlockStatusRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:        

		void OnTimer(ObserverPattern::INotificationPtr p_pNotify);
		//发送同步处理函数
		void AlarmEditLockSync(PROTOCOL::CAlarmEditLockSync& p_pAlarmEditLockSync, 
			PROTOCOL::CAlarmEditLock& p_pRequest, std::string p_strSyncType);
		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
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




