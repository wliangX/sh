#ifndef __AbstractAlarmStatisticsCount_H__
#define __AbstractAlarmStatisticsCount_H__

#include "AbstractAlarmStatistics.h"

namespace ICC
{
	class CAbstractAlarmStatisticsCount : public CAbstractAlarmStatistics
	{
	public:
		CAbstractAlarmStatisticsCount(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CAbstractAlarmStatisticsCount();

	private:
		virtual std::string _StatisticsData(const std::string& strDate);

	private:
		CAbstractAlarmStatisticsCount();

	protected:
		std::string m_strStatisticCountSqlId;
	};
}

#endif