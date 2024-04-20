#ifndef __StatisticsAlarmByArea_H__
#define __StatisticsAlarmByArea_H__

#include "IAlarmStatistics.h"
#include "DbBase.h"

namespace ICC
{
	struct StatisticsAreaData
	{
		std::string strFirstTypeData;
		std::string strSecondTypeData;
		std::string strThirdTypeData;
		std::string strFourthTypeData;

		StatisticsAreaData(){}
		StatisticsAreaData(const StatisticsAreaData& other)
		{
			strFirstTypeData = other.strFirstTypeData;
			strSecondTypeData = other.strSecondTypeData;
			strThirdTypeData = other.strThirdTypeData;
			strFourthTypeData = other.strFourthTypeData;
		}
		StatisticsAreaData& operator=(const StatisticsAreaData& other)
		{
			if (&other != this)
			{
				strFirstTypeData = other.strFirstTypeData;
				strSecondTypeData = other.strSecondTypeData;
				strThirdTypeData = other.strThirdTypeData;
				strFourthTypeData = other.strFourthTypeData;
			}
			return *this;
		}
	};

	class CStatisticsAlarmCountByArea : public IAlarmStatistics
	{
	public:
		virtual int Statistics();

	public:
		CStatisticsAlarmCountByArea(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CStatisticsAlarmCountByArea();

	protected:
		std::string         _StatisticsData(const std::string& strType, const std::string& strDate, const std::string& strDeptCode);
		void                _StatisticsDeptData(const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName);

		int                 _PrcessPreDateStatistics();
		int                 _ProcessCurrDateStatistics();		

		void                _StatisticsDeptInfo(const std::string& strDate, std::vector<StatisticsDeptInfo>& vecDeptInfos);

		bool                _InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName);
		std::string         _QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strDeptCode);
		bool                _UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode);

		bool                _ExecSql(DataBase::SQLRequest sqlRequest);
		std::string         _QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName);
				

	protected:
		DataBase::IDBConnPtr       m_pDBConn;
		Log::ILogPtr               m_pLog;
		StringUtil::IStringUtilPtr m_pStringUtil;
		DateTime::IDateTimePtr     m_pDateTime;

		std::string                m_strInsertSqlName;
		std::string                m_strUpdateSqlName;
		std::string                m_strQueryResultSqlName;				

		std::vector<std::string>   m_vecStatisticsTypes;
		std::map<std::string, std::string> m_mapTypeAndSqlIds;
	};
}

#endif