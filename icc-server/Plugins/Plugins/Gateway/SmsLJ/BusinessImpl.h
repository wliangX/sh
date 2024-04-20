#pragma once

#include "TcpHelper.h"

namespace ICC
{
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
		void ReadConfig();
		void ConnectServer();
		bool SelectDeptNameByDeptCode(std::string p_strDeptCode, std::string& p_strDeptName);

		// ThirdAlarm 2 SmsLJ
		void OnNotifiSetReadSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSendSmsSync(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		std::string							m_strIsUsing;

		std::map<std::string, boost::shared_ptr<CTcpHelper>> m_mapTcpHelper;

	public:
		SmsBuffer							m_SmsBuffer;

	public:
		Log::ILogPtr						m_pLog;
		Config::IConfigPtr					m_pConfig;
		DataBase::IDBConnPtr				m_pDBConn;
		DateTime::IDateTimePtr				m_pDateTime;
		StringUtil::IStringUtilPtr			m_pString;
		Timer::ITimerManagerPtr				m_pTimerMgr;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Lock::ILockPtr						m_Mutex;
	};
}