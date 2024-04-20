#ifndef __StatisticsTotalProcessAlarm_H__
#define __StatisticsTotalProcessAlarm_H__


#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsTotalProcessAlarm : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsTotalProcessAlarm(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsTotalProcessAlarm();

	private:
		CStatisticsTotalProcessAlarm();
	};
}

#endif