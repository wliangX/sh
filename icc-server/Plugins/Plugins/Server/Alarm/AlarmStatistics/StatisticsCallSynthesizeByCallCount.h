#ifndef __StatisticsCallSynthesizeByCallCount_H__
#define __StatisticsCallSynthesizeByCallCount_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsCallSynthesizeByCallCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallSynthesizeByCallCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallSynthesizeByCallCount();

	private:
		CStatisticsCallSynthesizeByCallCount();
	};
}

#endif