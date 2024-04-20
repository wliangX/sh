#ifndef __AbstractAlarmStatisticsRate_H__
#define __AbstractAlarmStatisticsRate_H__

#include "AbstractAlarmStatistics.h"

namespace ICC
{
	class CAbstractStatisticsRate : public CAbstractAlarmStatistics
	{
	public:
		CAbstractStatisticsRate(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CAbstractStatisticsRate();

	private:
		virtual std::string _StatisticsData(const std::string& strDate);

		std::string _StatisticsMolecularValue(const std::string& strDate);
		std::string _StatisticsDenominatorValue(const std::string& strDate);
		std::string _CalculateRate(const std::string& strMolecularValue, const std::string& strDenominatorValue);

	private:
		CAbstractStatisticsRate();

	protected:
		std::string m_strStatisticsMolecularValueSqlId;
		std::string m_strStatisticsDenominatorValueSqlId;
	};
}

#endif