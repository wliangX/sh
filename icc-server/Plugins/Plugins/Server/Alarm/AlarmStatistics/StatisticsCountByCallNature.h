#ifndef __StatisticsCountByCallNature_H__
#define __StatisticsCountByCallNature_H__

#include "AbstractAlarmStatisticsCount.h"

namespace ICC
{
	class CStatisticsCallNatureByAccept : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallNatureByAccept(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallNatureByAccept();

	private:
		CStatisticsCallNatureByAccept();
	};

	class CStatisticsCallNatureByRelease : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallNatureByRelease(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallNatureByRelease();

	private:
		CStatisticsCallNatureByRelease();
	};

	class CStatisticsCallNatureByDisturbance : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallNatureByDisturbance(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallNatureByDisturbance();

	private:
		CStatisticsCallNatureByDisturbance();
	};

	class CStatisticsCallNatureByInside : public CAbstractAlarmStatisticsCount
	{
	public:
		CStatisticsCallNatureByInside(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallNatureByInside();

	private:
		CStatisticsCallNatureByInside();
	};
}

#endif