#ifndef __StatisticsProcessAndFeedBackByArea_H__
#define __StatisticsProcessAndFeedBackByArea_H__

#include "IAlarmStatistics.h"
#include "DbBase.h"

namespace ICC
{

class CStatisticsProcessAndFeedbackByArea : public IAlarmStatistics
{
public:
	virtual int Statistics();

public:
	CStatisticsProcessAndFeedbackByArea(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
	~CStatisticsProcessAndFeedbackByArea();

protected:
	std::string         _StatisticsDataByDeptCode(const std::string& strType, const std::string& strDate, const std::string& strDeptCode);
	void                _StatisticsData(const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName);

	int                 _PrcessPreDateStatistics();
	int                 _ProcessCurrDateStatistics();
	int                 _ProcessStatusUpdateStatistics();

	bool                _InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName);
	std::string         _QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strDeptCode);
	bool                _UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode);	


protected:
	DataBase::IDBConnPtr       m_pDBConn;
	Log::ILogPtr               m_pLog;
	StringUtil::IStringUtilPtr m_pStringUtil;
	DateTime::IDateTimePtr     m_pDateTime;

	std::string                m_strInsertSqlName;
	std::string                m_strUpdateSqlName;
	std::string                m_strQueryResultSqlName;

	std::string                m_strQueryDeptInfoSqlId;
	std::string                m_strDeptCodeFiled;
	std::string                m_strDeptNameFiled;

	std::vector<std::string>   m_vecStatisticsTypes;
	std::map<std::string, std::string> m_mapTypeAndSqlIds;

	StatisticsDbPtr            m_pStatisticsDb;
	
	std::string                m_strQueryUpdateDatesSqlId;
};

}
#endif