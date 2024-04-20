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
		//�Ӿ���Ϣ��ѯ��Ӧ����
		void OnNotifiSearchAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// �永��Ա��Ϣ��ѯ��Ӧ����
		void OnNotifiSearchCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// ��ӻ�����永��Ա��Ϣ��Ӧ����
		void OnNotifiAddOrUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest); 
		// ��ӻ�����永��Ա��Ϣ��Ӧ����
		void OnNotifiUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		// ���� �永��Ա��Ϣ ��Ϣ����
		void SendAlarmCallerInfoSync(const PROTOCOL::CSearchCallerRequest p_CAlarmCarsInfoEx, const std::string p_strSyncType);
		//������Ϣ��ѯ��Ӧ����
		void OnNotifiSearchAlarmProcessRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//������Ϣ��ѯ��Ӧ����
		void OnNotifiSearchAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//������Ϣ��ѯ��Ӧ����
		void OnNotifiGetCalleventInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//��ʷ�����ѯ��Ӧ����[¼����ѯ]
		void OnNotifiCallEventSearchRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCallEventSearchCallorRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCallEventSearchCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//��ʷ�����ѯ��Ӧ����-λ�á�������
		void OnNotifiCallEventSearchExRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ�Ʒ�����Ϣ��ѯ��Ӧ����
		void OnNotifiSearchStatisticInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//����������ѯ��Ӧ����
		void OnNotifiSearchAlarmCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//����������ѯ��Ӧ����
		void OnNotifiSearchCallrefCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//���ŵ绰��ѯ��Ӧ����
		void OnNotifiSearchReleaseCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//����ʱ����ѯ��Ӧ����
		void OnNotifiSearchOnlineCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//��ϯʱ����ѯ��Ӧ����
		void OnNotifiSearchLogoutCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//�Ӿ�ϯ���С���æ��ѯ
		void OnNotifiSearchClientInOutRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		//�����¼����ͳ��
		void OnNotifiSearchStatisticCallInRecordRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//�Ӿ�ͳ��
		void OnNotifiSearchStatisticReceiveAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//����ͳ��
		void OnNotifiSearchStatisticAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//�ŶӲ�ѯ
		void OnNotifiSearchCallWaitingRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//�˵���ѯ
		void OnNotifiSearchBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ�Ƹ澯
		void OnNotifStatistcsAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ����ϯ�澯
		void OnNotifStatistcsSeatAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ�ƻ���
		void OnNotifStatistcsCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ��Υ��
		void OnNotifStatistcsViolationRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ�ƺ�����
		void OnNotifStatistcsBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		//ͳ�Ƶ绰���
		void OnNotifStatistcsPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiCommonDeptStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void OnNotifiCommonTypeStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		void OnNotifiNumberBlackCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust);

		bool SearchCommonDept(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond);
		bool SearchCommonDeptInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond);
		bool SearchCommonType(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond);
		bool SearchCommonTypeInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond);

	public:
		//��ʱ����Ӧ����
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//�첽��ѯsearch_alarm_request���������������ľ�������
		void QueryAlarmCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		void QueryAlarmAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		void QueryCallAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid);
		// �����永��Ա����
		bool UpdateCallerData(PROTOCOL::CSearchCallerRequest::CBody p_strCallerRequest);
		// �����永��Ա��Ϣ����
		bool InsertCallerData(PROTOCOL::CSearchCallerRequest::CBody synCallerData);
		// ��ѯ �永��Ա��Ϣ
		bool SearchCallerData(const std::string l_strCallerID);
	private:	
		bool GetAlarmProcessByAlarmID(const std::string& p_strAlarmID, std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& p_vecProcess);
		//�����ļ���ȡ����
		bool ReadConfig();
		//�����ӳػ�ȡһ�����ݿ�����
		DataBase::IDBConnPtr GetDBClient(std::string p_strSenderID, std::string p_strSql);
		//�黹���ݿ����������ӳ�
		void ReleaseDBClient(DataBase::IDBConnPtr p_pDBClient, std::string p_strSenderID, std::string p_strSql);
		//��Ӧ��Ϣͷ���ɺ���
		void GenRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader &p_pRequestHeader, PROTOCOL::CHeader &p_pRespHeader);
		void GenRespondHeaderEx(const std::string& p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);

		//�Ӿ���Ϣ��ѯ�������캯��
		void BuildQueryCondition(PROTOCOL::CSearchAlarmRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//������Ϣ��ѯ�������캯��
		void BuildQueryCondition(PROTOCOL::CSearchAlarmProcessRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//������Ϣ��ѯ�������캯��
		void BuildQueryCondition(PROTOCOL::CSearchAlarmFeedbackRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//��ʷ�����ѯ�������캯��
		void BuildQueryCondition(PROTOCOL::CCallEventSearchRequest::CBody &l_oRequestBody, std::string &l_strCondition);
		//��ֵ��ѯ�������캯��
		std::string BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt);
		//SQL Request�����ѯ�����Ӧ����
		void ResponseSQLRequest(const DataBase::IResultSetPtr &p_pRSet, ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		//SQL Request�����ѯ�����Ӧ����
		void RespondSqlRequestBak(const DataBase::IResultSetPtr &p_pRSet, ObserverPattern::INotificationPtr p_pNotifiSqlRequest);
		//��Ӧ��Ϣ���ͺ���
		void SendRespondMsg(PROTOCOL::IRespond &p_oRespond, ObserverPattern::INotificationPtr p_pNotifiReqeust);	

		int BuildQuerySqlId(ObserverPattern::INotificationPtr p_pNotifiReqeust, DataBase::SQLRequest& l_oSqlRequest, PROTOCOL::CCallEventSearchRequest::CBody l_oRequestBody);

		//2023.4.19�������ݾ��������Զ����ѯ
		std::string BuildTypeConditions(std::string p_strConditionToSpilt);
		//��ֵ��ѯ�������캯��
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
		//���ݿ�������Ϣ
		std::string m_strDBHost;
		std::string m_strDBPort;
		std::string m_strDBUser;
		std::string m_strDBPwd;
		std::string m_strDBName;

		//���ݿ�����״̬: true=connect,false disconnect
		bool m_bDBConnStatus;		
		//���ݿ�������Ч�Լ���SQL���
		std::string m_strCheckSQL;
		//���ݿ�����״̬���ʱ����
		unsigned int m_uiDBCheckDuration;
		//��ҳ����¼��
		int m_iMaxPageSize;
		//ͳ�Ʒ�ʽ
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
		Redis::IRedisClientPtr		m_pRedisClient;		//redis�ͻ���
		
		unsigned int m_uiCurDBClientNum = 0;
		unsigned int m_uiInitDBClientNum = 0;
		unsigned int m_uiMaxDBClientNum = 0;
		boost::mutex m_mtxDBClientLock;
		boost::condition_variable_any m_cvHasIdleDBClient;
		std::list<DataBase::IDBConnPtr> m_lstIdleDBClient;
		std::list<DataBase::IDBConnPtr> m_lstBusyDBClient;
		std::map<std::string, DataBase::IDBConnPtr> m_mapTransactionInfo;//<transaction id, PGClient WITH Transaction begin,unused>

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		int m_iIntervalTime;	//���õ�λ�Լ����þ�������Redis�����ݸ��µ�Ƶ�ʣ���λΪ����
		int m_iCommonAlarmDeptQueryDays;	//���õ�λ��ѯ����������
		int m_iCommonAlarmTypeQueryDays;	//�������Ͳ�ѯ����������

		std::mutex m_mutexSelectAlarmCount;
		std::mutex m_mutexSelectAllAlarmCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectAlarmCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectAllAlarmCount;

		std::mutex m_mutexSelectCallCount;
		std::map<std::string, DataBase::IResultSetPtr> m_mapSelectCallCount;
	};
}