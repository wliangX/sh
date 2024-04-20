#ifndef __StatisticsCallSynthesizeBySeatno_H__
#define __StatisticsCallSynthesizeBySeatno_H__

#include "AbstractAlarmStatisticsByField.h"

namespace ICC
{
	class CStatisticsCallSynthesizeBySeatno : public CAbstractStatisticsAlarmByField
	{
	public:
		CStatisticsCallSynthesizeBySeatno(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsCallSynthesizeBySeatno();

	private:
		CStatisticsCallSynthesizeBySeatno();

	private:
		virtual std::string _StatisticsDataByField(const std::string& strType, const std::string& strDate, const std::string& strFiled);
	};
}

#endif