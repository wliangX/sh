#ifndef __StatisticsTotalCallIn_H__
#define __StatisticsTotalCallIn_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsTotalCallIn : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsTotalCallIn(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsTotalCallIn();

	private:
		CStatisticsTotalCallIn();
	};
}

#endif