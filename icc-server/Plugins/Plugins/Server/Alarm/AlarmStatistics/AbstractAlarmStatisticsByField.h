#ifndef __StatisticsAlarmByField_H__
#define __StatisticsAlarmByField_H__

#include "IAlarmStatistics.h"
#include "DbBase.h"

namespace ICC
{
	struct BaseFieldInfo
	{
		std::string strField;		

		BaseFieldInfo(){}
		BaseFieldInfo(const BaseFieldInfo& other)
		{
			strField = other.strField;			
		}
		BaseFieldInfo& operator=(const BaseFieldInfo& other)
		{
			if (&other != this)
			{
				strField = other.strField;				
			}
			return *this;
		}
	};

	class CAbstractStatisticsAlarmByField : public IAlarmStatistics
	{
	public:
		virtual int Statistics();

	public:
		CAbstractStatisticsAlarmByField(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime);
		~CAbstractStatisticsAlarmByField();

	protected:
		virtual std::string _StatisticsDataByField(const std::string& strType, const std::string& strDate, const std::string& strFiled) = 0;
		void                _StatisticsFieldData(const std::string& strDate, std::shared_ptr<BaseFieldInfo> pFieldInfo);

		int                 _PrcessPreDateStatistics();
		int                 _ProcessCurrDateStatistics();		

		virtual void        _StatisticsFieldInfo(const std::string& strDate, std::vector<std::shared_ptr<BaseFieldInfo>>& vecFieldInfos);

		bool                _InsertStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strDeptCode, const std::string& strDeptName, std::string& strField);
		std::string         _QueryStatisticsData(const std::string& strStatisticsType, const std::string& strDate, const std::string& strField);
		bool                _UpdateStatisticsResult(const std::string& strStatisticsType, const std::string& strResult, const std::string& strDate, const std::string& strField);

		bool                _ExecSql(DataBase::SQLRequest sqlRequest);
		std::string         _QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName);
				

	protected:
		DataBase::IDBConnPtr       m_pDBConn;
		Log::ILogPtr               m_pLog;
		StringUtil::IStringUtilPtr m_pStringUtil;
		DateTime::IDateTimePtr     m_pDateTime;

		StatisticsDbPtr            m_pStatisticsDb;

		std::string                m_strInsertSqlName;
		std::string                m_strUpdateSqlName;
		std::string                m_strQueryResultSqlName;		

		std::string                m_strFiledInfoName;	
		std::string                m_strStatisticsType;
		std::string                m_strQueryFieldInfoSqlId;
	};
}

#endif