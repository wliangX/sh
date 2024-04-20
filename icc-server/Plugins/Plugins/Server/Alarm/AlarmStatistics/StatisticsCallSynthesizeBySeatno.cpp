#include "Boost.h"
#include "StatisticsCallSynthesizeBySeatno.h"
#include "StatisticsCommonDef.h"
#include "StatisticsCommonTime.h"

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/






	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CStatisticsCallSynthesizeBySeatno::CStatisticsCallSynthesizeBySeatno(Log::ILogPtr pLog, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStringUtil, DateTime::IDateTimePtr pDateTime)
		: CAbstractStatisticsAlarmByField(pLog, pDBConn, pStringUtil, pDateTime)
	{
		m_strStatisticsType = STATISTICS_TYPE_SYNTHESIZE_SEATNO;
		m_strQueryFieldInfoSqlId = "statistics_alarm_seatnoinfo";
		m_strFiledInfoName = "receipt_seatno";		
	}

	CStatisticsCallSynthesizeBySeatno::~CStatisticsCallSynthesizeBySeatno()
	{

	}

	std::string CStatisticsCallSynthesizeBySeatno::_StatisticsDataByField(const std::string& strType, const std::string& strDate, const std::string& strFiled)
	{
		std::string strBeginTime = CStatisticsCommonTime::StatisticsBeginString(strDate, m_pStringUtil);
		std::string strEndTime = CStatisticsCommonTime::StatisticsEndString(strDate, m_pStringUtil);

		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "statistics_count_by_seatno";
		l_SqlRequest.param["caller_id"] = strFiled;
		l_SqlRequest.param["called_id"] = strFiled;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_BEGIN_TIME] = strBeginTime;
		l_SqlRequest.param[STATISTICS_PARAM_NAME_END_TIME] = strEndTime;

		std::string strTmpLog;
		std::string strResultFiledName = "count";
		std::string strResult = _QueryData(l_SqlRequest, strResultFiledName);
		if (strResult.empty())
		{
			strTmpLog = m_pStringUtil->Format("statistics count failed! id = %s", l_SqlRequest.sql_id.c_str());
			ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
			strResult = "0";
		}
		else
		{
			strTmpLog = m_pStringUtil->Format("statistics count success! count = %s, id = %s", strResult.c_str(), l_SqlRequest.sql_id.c_str());
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}

		return strResult;
	}
}