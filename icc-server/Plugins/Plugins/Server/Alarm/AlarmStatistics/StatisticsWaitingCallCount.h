#ifndef __StatisticsWaitingCallCount_H__
#define __StatisticsWaitingCallCount_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsWaitingCallCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsWaitingCallCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsWaitingCallCount();

	private:
		CStatisticsWaitingCallCount();
	};	
}

#endif