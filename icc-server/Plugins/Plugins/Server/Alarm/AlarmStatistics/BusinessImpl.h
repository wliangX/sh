#pragma once

#include "StatisticsTimer.h"
#include "IAlarmStatistics.h"

namespace ICC
{ 

	class CBusinessImpl : public CBusinessBase, public CStatisticsTimerCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		

	private:
		virtual void OnStatisticsTimer(unsigned long uMsgId);
		void         _InitAlarmStatisticsProcessers();
		void         _StatisticsProcess();
		
	private:
        DataBase::IDBConnPtr m_pDBConn;

		Config::IConfigPtr m_pIConfig;
        Log::ILogPtr m_pILog;		

		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pIDateTime;

		int m_iStatisticsHour;
		int m_iStatisticsMinute;

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		CStatisticsTimer                    m_statisticsTimer;
		std::vector <std::shared_ptr<IAlarmStatistics>> m_vecAlarmStatisticsProcessers;
	};
}




