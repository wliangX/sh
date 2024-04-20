#ifndef __StatisticsValidReceiveAlarm_H__
#define __StatisticsValidReceiveAlarm_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsValidReceiveAlarm : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsValidReceiveAlarm(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsValidReceiveAlarm();	

	private:
		CStatisticsValidReceiveAlarm();
	};
}

#endif