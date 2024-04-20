#ifndef __StatisticsRealAlarmRate_H__
#define __StatisticsRealAlarmRate_H__

#include "AbstractAlarmStatisticsRate.h"
#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsRealAlarmRate : public CAbstractStatisticsRate
	{
	public:
		CStatisticsRealAlarmRate(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsRealAlarmRate();

	private:
		CStatisticsRealAlarmRate();
	};

	class CStatisticsRealAlarmRateRealCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsRealAlarmRateRealCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsRealAlarmRateRealCount();

	private:
		CStatisticsRealAlarmRateRealCount();
	};

	class CStatisticsRealAlarmRateReceiveCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsRealAlarmRateReceiveCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsRealAlarmRateReceiveCount();

	private:
		CStatisticsRealAlarmRateReceiveCount();
	};
}


#endif