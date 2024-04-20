#ifndef __AbstractAlarmStatistics_H__
#define __AbstractAlarmStatistics_H__

#include "IAlarmStatistics.h"
#include "DbBase.h"

namespace ICC
{
	class CAbstractAlarmStatistics : public IAlarmStatistics
	{
	public:
		virtual int Statistics();

	public:		
		~CAbstractAlarmStatistics();

	protected:
		virtual std::string _StatisticsData(const std::string& strDate) = 0;

		int                 _PrcessPreDateStatistics();
		int                 _ProcessCurrDateStatistics();	
		int                 _ProcessStatusUpdateStatistics();		

		bool                _InsertStatisticsResult(const std::string& strResult, const std::string& strTmpDate);
		std::string         _QueryStatisticsData(const std::string& strDate);
		bool                _UpdateStatisticsResult(const std::string& strResult, const std::string& strDate);

		bool                _ExecSql(DataBase::SQLRequest sqlRequest);
		std::string         _QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName);		

	protected:
		CAbstractAlarmStatistics(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);

	protected:
		DataBase::IDBConnPtr       m_pDBConn;
		Log::ILogPtr               m_pLog;
		StringUtil::IStringUtilPtr m_pStringUtil;
		DateTime::IDateTimePtr     m_pDateTime;

		StatisticsDbPtr            m_pStatisticsDb;

		std::string                m_strInsertSqlName;
		std::string                m_strUpdateSqlName;
		std::string                m_strQueryResultSqlName;
		std::string                m_strStatisticsType;

		std::string                m_strRemark;

		std::string                m_strNeedScanUpdateTimeFlag;
		std::string                m_strQueryUpdateDatesSqlId;		
	};
}

#endif