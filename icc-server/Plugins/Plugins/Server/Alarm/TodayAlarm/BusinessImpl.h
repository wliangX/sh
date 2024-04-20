#pragma once
#include <algorithm> 
#include <Timer/ITimerFactory.h> 
#include "CommonWorkThread.h"

using namespace std;

//消息分包数

#define SELECT_TODAY_ALARM					"select_today_jjdb"
#define SELECT_TODAY_ALARM_CNT				"select_today_jjdb_count"
#define SELECT_TODAY_ALARM_UP_ATTENTION					"select_today_jjdb_up_attention"
#define SELECT_TODAY_ALARM_CNT_UP_ATTENTION				"select_today_jjdb_count_up_attention"
#define SELECT_SIMILAR_ALARM				"select_similar_alarm"
#define GET_TODAY_ALARM_RESPOND				"get_today_alarm_respond"
#define TODAY_ALARM_SYNC					"today_alarm_sync"
#define TOPIC_ALARM							"topic_alarm"

namespace ICC
{
	class CBusinessImpl;
	typedef void (CBusinessImpl::* ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);

	class CBusinessImpl : public CBusinessBase, CCommonThreadCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		//获取今日警情(24小时警情，从当前往前推24小时)
		void OnCNotifiGetTodayAlarm(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetSimilarAlarm(ObserverPattern::INotificationPtr p_pNotify);

		void OnNotifiGetCallCountStatistics(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetCallInCountByPeriod(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetAlarmCountByReceivedType(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetAlarmCountByFirstType(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetAlarmInfoByFirstType(ObserverPattern::INotificationPtr p_pNotify);

	private:

		std::string _BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt);
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		std::string _QueryCallInCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId);
		std::string _QueryCallOutCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId);
		std::string _QueryCallReleaseCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId);
		std::string _QueryCallAverageTalkTime(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId);

		void        _AsignedPeriod(const std::string& strStartTime, const std::string& strEndTime, const std::string& strIncrementTime, std::map<std::string, std::string>& mapPeriods, PROTOCOL::CTodayCallStatisticsRespond& response);
		void        _QueryCallInByPeriod(const PROTOCOL::CTodayCallInStatisticsRequest& request, PROTOCOL::CTodayCallStatisticsRespond& response);
		void		CalrQueryCallInCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId, std::map<std::string, std::string>& mapData);
		void		_AanalyzeTypesByFirstType(const std::string& strTypes, PROTOCOL::CTodayStatisticsByFirstTypeRespond& response);
		void        _AanalyzeTypesByByReceive(const std::string& strTypes, PROTOCOL::CTodayStatisticsByReceiveTypeRespond& response);
		void        _QueryAlarmByReceiveType(const PROTOCOL::CTodayStatisticsByReceiveTypeRequest& request, PROTOCOL::CTodayStatisticsByReceiveTypeRespond& response);
		void        _QueryAlarmByFirstType(const PROTOCOL::CTodayStatisticsByFirstTypeRequest& request, PROTOCOL::CTodayStatisticsByFirstTypeRespond& response);
		void        _QueryAlarmInfoByFirstType(const PROTOCOL::CTodayAlarmInfoByFirstTypeRequest& request, PROTOCOL::CTodayAlarmInfoByFirstTypeRespond& response);

		void        _QueryMajorAlarmTypeNames(std::vector<std::string>& vecTypeNames);
		std::string _QueryMajorAlarmTypeNames(std::map<std::string, std::string>& vecTypeNames);

		bool        _QueryTodayAlarmNotUpAttention(PROTOCOL::CTodayAlarm& request, std::string& strInterval, DataBase::IResultSetPtr& pResult, std::string& strTotalCount);
		bool        _QueryTodayAlarmUpAttention(PROTOCOL::CTodayAlarm& request, std::string& strInterval, DataBase::IResultSetPtr& pResult, std::string& strTotalCount);

		std::string _ReadFile();

	private:
		void    _OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void    _DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName);
		virtual void ProcMessage(CommonThread_data msg_data);
		CommonWorkThreadPtr _GetThread();
		void    _InitProcNotifys();
		void    _CreateThreads();
		void    _DestoryThreads();

		unsigned int                        m_uCurrentThreadIndex;
		unsigned int                        m_uProcThreadCount;
		std::string                         m_strDispatchMode;

		std::mutex                          m_mutexThread;
		std::vector<CommonWorkThreadPtr>    m_vecProcThreads;
		std::map<std::string, ProcNotify>   m_mapFuncs;

	private:
		Lock::ILockPtr m_pAlarmLock;
		Lock::ILockPtr m_pTimeLock;

		Log::ILogPtr			m_pLog;
		Config::IConfigPtr		m_pConfig;
		DataBase::IDBConnPtr		m_pDBConn;
		StringUtil::IStringUtilPtr	m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		Lock::ILockFactoryPtr m_pLockFty;
		DateTime::IDateTimePtr	m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Timer::ITimerManagerPtr m_pTimerMgr;
		static string m_strCurTime;
		int64 m_IntervalTime;
		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
	};
}