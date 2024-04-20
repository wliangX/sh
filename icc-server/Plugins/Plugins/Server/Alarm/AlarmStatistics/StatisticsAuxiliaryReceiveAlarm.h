#ifndef __StatisticsAuxiliaryReceiveAlarm_H__
#define __StatisticsAuxiliaryReceiveAlarm_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsAuxiliaryReceiveAlarm : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsAuxiliaryReceiveAlarm(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsAuxiliaryReceiveAlarm();

	private:
		CStatisticsAuxiliaryReceiveAlarm();
	};
}

#endif