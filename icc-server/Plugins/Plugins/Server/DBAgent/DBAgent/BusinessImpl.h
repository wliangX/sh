#pragma once

#include "CommonWorkThread.h"

#define THREADCOUNT 4

#define CALL_QUERY_COMMON 1
#define CALL_QUERY_WITH_SEAT 2
#define CALL_QUERY_WITH_ALARM 3

namespace ICC
{		
	class CBusinessImpl;
	typedef void (CBusinessImpl::*ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);

	class CBusinessImpl : public CBusinessBase, CCommonThreadCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:	
		void OnNotifiGetAlarmAllRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//接警信息查询响应函数
		void OnNotifiSearchAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// 涉案人员信息查询响应函数
		void OnNotifiSearchCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// 添加或更新涉案人员信息响应函数
		void OnNotifiAddOrUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest); 
		// 添加或更新涉案人员信息响应函数
		void OnNotifiUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// 发送 涉案人员信息 消息函数
		void SendAlarmCallerInfoSync(const PROTOCOL::CSearchCallerRequest p_CAlarmCarsInfoEx, const std::string p_strSyncType);
		//处警信息查询响应函数
		void OnNotifiSearchAlarmProcessRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//反馈信息查询响应函数
		void OnNotifiSearchAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//话务信息查询响应函数
		void OnNotifiGetCalleventInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//历史话务查询响应函数[录音查询]
		void OnNotifiCallEventSearchRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCallEventSearchCallorRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCallEventSearchCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//历史话务查询响应函数-位置、归属地
		void OnNotifiCallEventSearchExRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计分析信息查询响应函数
		void OnNotifiSearchStatisticInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//警情数量查询响应函数
		void OnNotifiSearchAlarmCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//话务数量查询响应函数
		void OnNotifiSearchCallrefCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//早逝电话查询响应函数
		void OnNotifiSearchReleaseCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//在线时长查询响应函数
		void OnNotifiSearchOnlineCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//离席时长查询响应函数
		void OnNotifiSearchLogoutCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//接警席置闲、置忙查询
		void OnNotifiSearchClientInOutRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		//呼入记录分类统计
		void OnNotifiSearchStatisticCallInRecordRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//接警统计
		void OnNotifiSearchStatisticReceiveAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//警情统计
		void OnNotifiSearchStatisticAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//排队查询
		void OnNotifiSearchCallWaitingRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//退单查询
		void OnNotifiSearchBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计告警
		void OnNotifStatistcsAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计坐席告警
		void OnNotifStatistcsSeatAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计话务
		void OnNotifStatistcsCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计违规
		void OnNotifStatistcsViolationRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计黑名单
		void OnNotifStatistcsBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//统计电话标记
		void OnNotifStatistcsPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCommonDeptStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void OnNotifiCommonTypeStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiNumberBlackCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		bool SearchCommonDept(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond);
		bool SearchCommonDeptInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond);
		bool SearchCommonType(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond);
		bool SearchCommonTypeInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond);

	public:
		//定时器响应函数
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//异步查询search_alarm_request请求中满足条件的警情数量
		void QueryAlarmCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		void QueryAlarmAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		void QueryCallAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		// 更新涉案人员数据
		bool UpdateCallerData(PROTOCOL::CSearchCallerRequest::CBody p_strCallerRequest);
		// 插入涉案人员信息数据
		bool InsertCallerData(PROTOCOL::CSearchCallerRequest::CBody synCallerData);
		// 查询 涉案人员信息
		bool SearchCallerData(const std::string l_strCallerID);
	private:	
		bool GetAlarmProcessByAlarmID(const std::string& p_strAlarmID, std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& p_vecProcess);
		//配置文件读取函数
		bool ReadConfig();
		//从连接池获取一个数据库连接
		DataBase::IDBConnPtr GetDBClient(std::string p_strSenderID, std::string p_strSql);
		//归还数据库连接至连接池
		void ReleaseDBClient(DataBase::IDBConnPtr p_pDBClient, std::string p_strSenderID, std::string p_strSql);
		//响应消息头生成函数
		void GenRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader &p_pRequestHeader, PROTOCOL::CHeader &p_pRespHeader);
		void GenRespondHeaderEx(const std::string& p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		//接警信息查询条件构造函数
		void BuildQueryCondition(PROTOCOL::CSearchAlarmRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//处警信息查询条件构造函数
		void BuildQueryCondition(PROTOCOL::CSearchAlarmProcessRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//反馈信息查询条件构造函数
		void BuildQueryCondition(PROTOCOL::CSearchAlarmFeedbackRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//历史话务查询条件构造函数
		void BuildQueryCondition(PROTOCOL::CCallEventSearchRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//多值查询条件构造函数
		std::string BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt);
		//SQL Request请求查询结果响应函数
		void ResponseSQLRequest(const DataBase::IResultSetPtr &p_pRSet, ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		//SQL Request请求查询结果响应函数
		void RespondSqlRequestBak(const DataBase::IResultSetPtr &p_pRSet, ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		//响应消息发送函数
		void SendRespondMsg(PROTOCOL::IRespond &p_oRespond, ObserverPattern::INotificationPtr p_pNotifiReqeust);	

		int BuildQuerySqlId(ObserverPattern::INotificationPtr p_pNotifiReqeust, DataBase::SQLRequest& l_oSqlRequest, PROTOCOL::CCallEventSearchRequest::CBody l_oRequestBody);

		//2023.4.19新增根据警情类别可以多个查询
		std::string BuildTypeConditions(std::string p_strConditionToSpilt);
		//多值查询条件构造函数
		std::string BuildMultiConditionsEx(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt);

		void _QueryCallWaitingDataCount(const PROTOCOL::CSearchCallWaitingRequest& callInRequest, std::string& strCount);


		DataBase::IResultSetPtr _QueryReceiveAlarm(const PROTOCOL::CSearchAlarmRequest::CBody& alarmRequest);
		DataBase::IResultSetPtr _QueryProcessAlarm();
		DataBase::IResultSetPtr _QueryFeedBack();

		void                    _RespondReceiveAlarmQueryRequest(DataBase::IResultSetPtr);
		void                    _RespondProcessAlarmQueryRequest(DataBase::IResultSetPtr);
		void                    _RespondFeedBackQueryRequest(DataBase::IResultSetPtr);

		void                    _StandardizationPageParam(const std::string& strPageIndex, const std::string& strPageSize, int& iPageIndex, int& iPageSize);
		std::string             _StatisticsResultCount(DataBase::SQLRequest sqlRequest);

		void                    _StatisticsCallInData(const PROTOCOL::CSearchStatisticsCallInRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchStatisticsCallInRespond::CData>& vecDatas);
		void                    _StatisticsReceiveAlarmData(const PROTOCOL::CSearchStatisticsReceiveAlarmRequest::CBody& receiveAlarmRequest, std::vector<PROTOCOL::CSearchStatisticsReceiveAlarmRespond::CData>& vecDatas);
		void                    _StatisticsAlarmInfoData(const PROTOCOL::CSearchStatisticsAlarmRequest::CBody& receiveAlarmRequest, std::vector<PROTOCOL::CSearchStatisticsAlarmRespond::CData>& vecDatas);

		void                    _QueryCallWaitingData(const PROTOCOL::CSearchCallWaitingRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchCallWaitingRespond::CData>& vecDatas, std::string& strCount);
		void                    _QueryBackAlarmData(const PROTOCOL::CSearchBackAlarmRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchBackAlarmRespond::CData>& vecDatas);


		bool _StatisticsSeatAlarmData(const PROTOCOL::CGetSeatAlarmStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetSeatAlarmStatisticsRespond&);

		bool _StatisticsAlarmData(const PROTOCOL::CGetAlarmStatisticsRequest &in_oStatAlarmRequest, PROTOCOL::CGetAlarmStatisticsRespond &);

		bool _StatisticsCalleventData(const PROTOCOL::CGetCalleventStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetCalleventStatisticsRespond&);

		bool _StatisticsViolationData(const PROTOCOL::CGetViolationStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetViolationStatisticsRespond&);

		bool _StatisticsBlackListData(const PROTOCOL::CGetBlacklistStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetBlacklistStatisticsRespond&);

		bool _StatisticsPhoneMarkData(const PROTOCOL::CGetPhoneMarkStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetPhoneMarkStatisticsRespond&);

		void        _QueryMajorAlarmTypeNames(std::vector<std::string>& vecTypeNames);

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
		//数据库连接信息
		std::string m_strDBHost;
		std::string m_strDBPort;
		std::string m_strDBUser;
		std::string m_strDBPwd;
		std::string m_strDBName;

		//数据库连接状态: true=connect,false disconnect
		bool m_bDBConnStatus;		
		//数据库连接有效性检测的SQL语句
		std::string m_strCheckSQL;
		//数据库连接状态检测时间间隔
		unsigned int m_uiDBCheckDuration;
		//单页最大记录数
		int m_iMaxPageSize;
		//统计方式
		int m_iStatisticMethod;

		DataBase::IDBConnPtr		m_pDBConn;
		DataBase::IDBConnPtr		m_pDBAlarmCount;
		DataBase::IDBConnFactoryPtr m_pDBConnFty;
		DataBase::IResultSetPtr		m_pAlarmCount;
		DataBase::IResultSetPtr		m_pAlarmAllCount;
		//logger
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr	m_pString;		
		DateTime::IDateTimePtr		m_pDateTime;
		JsonParser::IJsonPtr		m_pJson;
		JsonParser::IJsonFactoryPtr	m_pJsonFty;
		Thread::IThreadPoolPtr		m_pThreadPool;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
		
		unsigned int m_uiCurDBClientNum = 0;
		unsigned int m_uiInitDBClientNum = 0;
		unsigned int m_uiMaxDBClientNum = 0;
		boost::mutex m_mtxDBClientLock;
		boost::condition_variable_any m_cvHasIdleDBClient;
		std::list<DataBase::IDBConnPtr> m_lstIdleDBClient;
		std::list<DataBase::IDBConnPtr> m_lstBusyDBClient;
		std::map<std::string, DataBase::IDBConnPtr> m_mapTransactionInfo;//<transaction id, PGClient WITH Transaction begin,unused>

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		int m_iIntervalTime;	//常用单位以及常用警情类型Redis中数据更新的频率，单位为分钟
		int m_iCommonAlarmDeptQueryDays;	//常用单位查询最近多少天的
		int m_iCommonAlarmTypeQueryDays;	//常用类型查询最近多少天的

		std::mutex m_mutexSelectAlarmCount;
		std::mutex m_mutexSelectAllAlarmCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectAlarmCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectAllAlarmCount;

		std::mutex m_mutexSelectCallCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectCallCount;
	};
}