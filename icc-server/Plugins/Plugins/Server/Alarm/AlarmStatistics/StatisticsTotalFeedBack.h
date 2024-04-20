#ifndef __StatisticsTotalFeedBack_H__
#define __StatisticsTotalFeedBack_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsTotalFeedBack : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsTotalFeedBack(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsTotalFeedBack();

	private:
		CStatisticsTotalFeedBack();
	};
}

#endif