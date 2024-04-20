#ifndef __StatisticsProcessAndFeedBackRate_H__
#define __StatisticsProcessAndFeedBackRate_H__

#include "AbstractAlarmStatisticsRate.h"
#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsProcessAndFeedBackRate : public CAbstractStatisticsRate
	{
	public:
		CStatisticsProcessAndFeedBackRate(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsProcessAndFeedBackRate();

	private:
		CStatisticsProcessAndFeedBackRate();
	};

	class CStatisticsProcessAndFeedBackRateProcess : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsProcessAndFeedBackRateProcess(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsProcessAndFeedBackRateProcess();

	private:
		CStatisticsProcessAndFeedBackRateProcess();
	};

	class CStatisticsProcessAndFeedBackRateFeedBack : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsProcessAndFeedBackRateFeedBack(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsProcessAndFeedBackRateFeedBack();

	private:
		CStatisticsProcessAndFeedBackRateFeedBack();
	};
}

#endif