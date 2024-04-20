#ifndef __StatisticsCountByCallInNumber_H__
#define __StatisticsCountByCallInNumber_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsCountBy110 : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCountBy110(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCountBy110();

	private:
		CStatisticsCountBy110();
	};

	class CStatisticsCountBy119 : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCountBy119(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCountBy119();

	private:
		CStatisticsCountBy119();
	};

	class CStatisticsCountBy122 : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCountBy122(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCountBy122();

	private:
		CStatisticsCountBy122();
	};
}




#endif