#ifndef __StatisticsReceiveAlarmRate_H__
#define __StatisticsReceiveAlarmRate_H__

#include "AbstractAlarmStatisticsRate.h"
#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsReceiveAlarmRate : public CAbstractStatisticsRate
	{
	public:
		CStatisticsReceiveAlarmRate(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsReceiveAlarmRate();		

	private:
		CStatisticsReceiveAlarmRate();	
	};

	class CStatisticsReceiveAlarmRateReceiveCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsReceiveAlarmRateReceiveCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsReceiveAlarmRateReceiveCount();

	private:
		CStatisticsReceiveAlarmRateReceiveCount();
	};

	class CStatisticsReceiveAlarmRateCallInCount : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsReceiveAlarmRateCallInCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsReceiveAlarmRateCallInCount();

	private:
		CStatisticsReceiveAlarmRateCallInCount();
	};
}

#endif