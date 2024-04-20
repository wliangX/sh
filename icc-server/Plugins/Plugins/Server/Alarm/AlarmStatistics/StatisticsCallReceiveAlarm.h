#ifndef __StatisticsCallReceiveAlarm_H__
#define __StatisticsCallReceiveAlarm_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsCallReceiveAlarm : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallReceiveAlarm(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallReceiveAlarm();	

	private:
		CStatisticsCallReceiveAlarm();	
	};
}

#endif