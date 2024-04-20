#ifndef __DbBase_H__
#define __DbBase_H__

namespace ICC
{
	struct StatisticsDeptInfo
	{
		std::string strDeptCode;
		std::string strDeptName;

		StatisticsDeptInfo(){}
		StatisticsDeptInfo(const StatisticsDeptInfo& other)
		{
			strDeptCode = other.strDeptCode;
			strDeptName = other.strDeptName;
		}
		StatisticsDeptInfo& operator=(const StatisticsDeptInfo& other)
		{
			if (&other != this)
			{
				strDeptCode = other.strDeptCode;
				strDeptName = other.strDeptName;
			}
			return *this;
		}
	};

	class CDbBase
	{
	public:
		bool        ExecSql(DataBase::SQLRequest sqlRequest);
		std::string QueryData(DataBase::SQLRequest sqlRequest, const std::string& strResultFiledName);
		void        QueryDeptInfo(const std::string& strQuerySqlId, const std::string& strDeptCodeFiled, const std::string& strDeptNameFiled, const std::string& strDate, std::vector<StatisticsDeptInfo>& vecDeptInfos);
		void        QueryDateInfo(const std::string& strQuerySqlId, const std::string& strDate, std::vector<std::string>& vecDateInfos);

	public:
		CDbBase(Log::ILogPtr pILog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil);
		~CDbBase();

	private:
		CDbBase();

	private:
		DataBase::IDBConnPtr       m_pDBConn;
		Log::ILogPtr               m_pLog;
		StringUtil::IStringUtilPtr m_pStringUtil;
	};

	typedef boost::shared_ptr<CDbBase> StatisticsDbPtr;
}

#endif