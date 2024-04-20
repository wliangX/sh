#ifndef __StatisticsCallSynthesizeByCallTime_H__
#define __StatisticsCallSynthesizeByCallTime_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsCallSynthesizeByCallTime : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallSynthesizeByCallTime(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallSynthesizeByCallTime();

	private:
		CStatisticsCallSynthesizeByCallTime();
	};
}

#endif