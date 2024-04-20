#include "Boost.h"
#include "BusinessImpl.h"

#define SYSTEMID		"icc_server"
#define SUBSYSTEMID		"icc_server_dbagent"

#define TIMER_CMD_NAME	"db_conn_check_timer"
#define DBCONN_STATUS_OK	"1"
#define DBCONN_STATUS_BAD	"0"
#define STATISTIC_QTABLE	1	/*通过查icc_t_statistic表的方式获取统计结果*/
#define COMMON_DEPT		"CommonDept"
#define COMMON_TYPE		"CommonType"


const std::string STRING_RESULT_0 = "0";
const unsigned long THREADID_RECEIVENOTIFY = 10000;

const unsigned long long TIMOUT_INTERVAL = 500 * 1000;

enum {
	only_self = 1, //只返回与本坐席相关话务
	early_release_query = 2 //早释话务查询
};

using namespace std;

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_bDBConnStatus = false;
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(DBAGENT_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pJson = ICCGetIJsonFactory()->CreateJson();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pThreadPool = ICCGetIThreadPoolFactory()->CreateThreadPool(2);
	m_pDBConnFty = ICCGetIDBConnFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pDBAlarmCount = m_pDBConn;
	//m_pDBAlarmCount = m_pDBConnFty->CreateNewDBConn(DataBase::DBType::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_uProcThreadCount = 0;
	m_uCurrentThreadIndex = 0;

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::_InitProcNotifys()
{
	//m_mapFuncs.insert(std::make_pair(TIMER_CMD_NAME, &CBusinessImpl::OnTimer));//定时检测数据库连接
	m_mapFuncs.insert(std::make_pair("search_alarm_request", &CBusinessImpl::OnNotifiSearchAlarmRequest));//警单高级查询
	m_mapFuncs.insert(std::make_pair("search_jqdsr_request", &CBusinessImpl::OnNotifiSearchCallerRequest));// 涉案人员信息查询响应函数
	m_mapFuncs.insert(std::make_pair("add_or_update_caller_info_request", &CBusinessImpl::OnNotifiAddOrUpdateCallerRequest));// 涉案人员信息添加 ICC
	m_mapFuncs.insert(std::make_pair("update_caller_info_request", &CBusinessImpl::OnNotifiUpdateCallerRequest));// 涉案人员信息更新 vcs 过来的数据
	m_mapFuncs.insert(std::make_pair("get_alarm_all_request", &CBusinessImpl::OnNotifiGetAlarmAllRequest));//
	m_mapFuncs.insert(std::make_pair("search_alarm_process_request", &CBusinessImpl::OnNotifiSearchAlarmProcessRequest));//处警信息查询[未使用]
	m_mapFuncs.insert(std::make_pair("search_alarm_feedback_request", &CBusinessImpl::OnNotifiSearchAlarmFeedbackRequest));//反馈信息查询[未使用]
	m_mapFuncs.insert(std::make_pair("search_statistic_info_request", &CBusinessImpl::OnNotifiSearchStatisticInfoRequest));//统计分析
	m_mapFuncs.insert(std::make_pair("search_alarm_count_request", &CBusinessImpl::OnNotifiSearchAlarmCountRequest));//警情数量查询
	m_mapFuncs.insert(std::make_pair("search_callref_count_request", &CBusinessImpl::OnNotifiSearchCallrefCountRequest));//话务数量查询[席位监控]
	m_mapFuncs.insert(std::make_pair("get_callevent_info_request", &CBusinessImpl::OnNotifiGetCalleventInfoRequest));//话务信息查询[接警话务面板]
	m_mapFuncs.insert(std::make_pair("call_event_search_request", &CBusinessImpl::OnNotifiCallEventSearchRequest));//历史话务查询[录音查询]
	m_mapFuncs.insert(std::make_pair("call_event_search_callor_request", &CBusinessImpl::OnNotifiCallEventSearchCallorRequest));//历史话务查询[录音查询]

	m_mapFuncs.insert(std::make_pair("call_event_search_count_request", &CBusinessImpl::OnNotifiCallEventSearchCountRequest));//历史话务查询[录音查询]
	m_mapFuncs.insert(std::make_pair("call_event_search_ex_request", &CBusinessImpl::OnNotifiCallEventSearchExRequest));//带归属地和运营商信息的历史话务查询[早释电话面板]
	m_mapFuncs.insert(std::make_pair("search_release_count_request", &CBusinessImpl::OnNotifiSearchReleaseCountRequest));//早释电话数量查询[席位监控]
	m_mapFuncs.insert(std::make_pair("search_online_count_request", &CBusinessImpl::OnNotifiSearchOnlineCountRequest));//在线时长查询
	m_mapFuncs.insert(std::make_pair("search_logout_count_request", &CBusinessImpl::OnNotifiSearchLogoutCountRequest));//离席时长查询
	m_mapFuncs.insert(std::make_pair("search_client_inout_request", &CBusinessImpl::OnNotifiSearchClientInOutRequest));

	m_mapFuncs.insert(std::make_pair("get_called_bills_request", &CBusinessImpl::OnNotifiSearchStatisticCallInRecordRequest));//呼入记录分类统计
	m_mapFuncs.insert(std::make_pair("get_dept_bills_request", &CBusinessImpl::OnNotifiSearchStatisticReceiveAlarmRequest));//接警统计
	m_mapFuncs.insert(std::make_pair("get_type_bills_request", &CBusinessImpl::OnNotifiSearchStatisticAlarmRequest));//警情统计

	m_mapFuncs.insert(std::make_pair("get_callevent_waiting_request", &CBusinessImpl::OnNotifiSearchCallWaitingRequest));//排队查询
	m_mapFuncs.insert(std::make_pair("get_back_alarm_request", &CBusinessImpl::OnNotifiSearchBackAlarmRequest));//退单查询

	//增加统计类数据处理  2022/3/8 begin
	m_mapFuncs.insert(std::make_pair("get_alarm_statistics_request", &CBusinessImpl::OnNotifStatistcsAlarmRequest));//统计告警
	m_mapFuncs.insert(std::make_pair("get_callevent_statistics_request", &CBusinessImpl::OnNotifStatistcsCallEventRequest));//统计话务
	m_mapFuncs.insert(std::make_pair("get_violation_statistics_request", &CBusinessImpl::OnNotifStatistcsViolationRequest));//统计违规
	m_mapFuncs.insert(std::make_pair("get_blacklist_statistics_request", &CBusinessImpl::OnNotifStatistcsBlackListRequest));//统计黑名单
	m_mapFuncs.insert(std::make_pair("get_phonemark_statistics_request", &CBusinessImpl::OnNotifStatistcsPhoneMarkRequest));//统计电话标记

	m_mapFuncs.insert(std::make_pair("get_seat_alarm_statistics_request", &CBusinessImpl::OnNotifStatistcsSeatAlarmRequest));//统计告警
	//增加统计类数据处理  2022/3/8 end

	m_mapFuncs.insert(std::make_pair("get_common_dept_request", &CBusinessImpl::OnNotifiCommonDeptStaticRequest));//警情常用单位统计
	m_mapFuncs.insert(std::make_pair("get_common_type_request", &CBusinessImpl::OnNotifiCommonTypeStaticRequest));//警情常用类型统计

	m_mapFuncs.insert(std::make_pair("get_number_black_count_request", &CBusinessImpl::OnNotifiNumberBlackCountRequest));//警情常用类型统计
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	if (!ReadConfig())
	{
		ICC_LOG_ERROR(m_pLog, "failed to read dbagent configuration file");
		ICC_LOG_INFO(m_pLog, "failed to start DBAgent");
		return;
	}

	ICC_LOG_INFO(m_pLog, "database clients initialized");
	ICC_LOG_INFO(m_pLog, "dbagent started");
	//cout << "DBAgent started " << this_thread::get_id() << endl;

	//添加检测数据库连接有效性的定时器
	//m_pTimerMgr->AddTimer(TIMER_CMD_NAME, 5, 10);


	_InitProcNotifys();

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, _OnReceiveNotify);//定时检测数据库连接
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_request", _OnReceiveNotify);//警单高级查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_jqdsr_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_caller_info_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_caller_info_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_request", _OnReceiveNotify);//
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_process_request", _OnReceiveNotify);//处警信息查询[未使用]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_feedback_request", _OnReceiveNotify);//反馈信息查询[未使用]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_statistic_info_request", _OnReceiveNotify);//统计分析
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_count_request", _OnReceiveNotify);//警情数量查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_callref_count_request", _OnReceiveNotify);//话务数量查询[席位监控]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_info_request", _OnReceiveNotify);//话务信息查询[接警话务面板]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_request", _OnReceiveNotify);//历史话务查询[录音查询]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_callor_request", _OnReceiveNotify);//历史话务查询[录音查询]

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_count_request", _OnReceiveNotify);//历史话务查询[录音查询]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_ex_request", _OnReceiveNotify);//带归属地和运营商信息的历史话务查询[早释电话面板]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_release_count_request", _OnReceiveNotify);//早释电话数量查询[席位监控]
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_online_count_request", _OnReceiveNotify);//在线时长查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_logout_count_request", _OnReceiveNotify);//离席时长查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_client_inout_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_called_bills_request", _OnReceiveNotify);//呼入记录分类统计
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dept_bills_request", _OnReceiveNotify);//接警统计
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_type_bills_request", _OnReceiveNotify);//警情统计

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_waiting_request", _OnReceiveNotify);//排队查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_back_alarm_request", _OnReceiveNotify);//退单查询

	//增加统计类数据处理  2022/3/8 begin
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_statistics_request", _OnReceiveNotify);//统计告警
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_statistics_request", _OnReceiveNotify);//统计话务
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_violation_statistics_request", _OnReceiveNotify);//统计违规
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_blacklist_statistics_request", _OnReceiveNotify);//统计黑名单
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_phonemark_statistics_request", _OnReceiveNotify);//统计电话标记

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_seat_alarm_statistics_request", _OnReceiveNotify);//统计告警
	//增加统计类数据处理  2022/3/8 end

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_common_dept_request", _OnReceiveNotify);//警情常用单位统计
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_common_type_request", _OnReceiveNotify);//警情常用类型统计

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_number_black_count_request", _OnReceiveNotify);//警情常用类型统计


	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);//定时检测数据库连接
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_request", OnNotifiSearchAlarmRequest);//警单高级查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_request", OnNotifiGetAlarmAllRequest);//
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_process_request", OnNotifiSearchAlarmProcessRequest);//处警信息查询[未使用]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_feedback_request", OnNotifiSearchAlarmFeedbackRequest);//反馈信息查询[未使用]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_statistic_info_request", OnNotifiSearchStatisticInfoRequest);//统计分析
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_count_request", OnNotifiSearchAlarmCountRequest);//警情数量查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_callref_count_request", OnNotifiSearchCallrefCountRequest);//话务数量查询[席位监控]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_info_request", OnNotifiGetCalleventInfoRequest);//话务信息查询[接警话务面板]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_request", OnNotifiCallEventSearchRequest);//历史话务查询[录音查询]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_callor_request", OnNotifiCallEventSearchCallorRequest);//历史话务查询[录音查询]
	//
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_count_request", OnNotifiCallEventSearchCountRequest);//历史话务查询[录音查询]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_event_search_ex_request", OnNotifiCallEventSearchExRequest);//带归属地和运营商信息的历史话务查询[早释电话面板]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_release_count_request", OnNotifiSearchReleaseCountRequest);//早释电话数量查询[席位监控]
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_online_count_request", OnNotifiSearchOnlineCountRequest);//在线时长查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_logout_count_request", OnNotifiSearchLogoutCountRequest);//离席时长查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_client_inout_request", OnNotifiSearchClientInOutRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_called_bills_request", OnNotifiSearchStatisticCallInRecordRequest);//呼入记录分类统计
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dept_bills_request", OnNotifiSearchStatisticReceiveAlarmRequest);//接警统计
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_type_bills_request", OnNotifiSearchStatisticAlarmRequest);//警情统计

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_waiting_request", OnNotifiSearchCallWaitingRequest);//排队查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_back_alarm_request", OnNotifiSearchBackAlarmRequest);//退单查询

	////增加统计类数据处理  2022/3/8 begin
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_statistics_request", OnNotifStatistcsAlarmRequest);//统计告警
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_callevent_statistics_request", OnNotifStatistcsCallEventRequest);//统计话务
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_violation_statistics_request", OnNotifStatistcsViolationRequest);//统计违规
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_blacklist_statistics_request", OnNotifStatistcsBlackListRequest);//统计黑名单
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_phonemark_statistics_request", OnNotifStatistcsPhoneMarkRequest);//统计电话标记

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_seat_alarm_statistics_request", OnNotifStatistcsSeatAlarmRequest);//统计告警
	////增加统计类数据处理  2022/3/8 end

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_common_dept_request", OnNotifiCommonDeptStaticRequest);//警情常用单位统计
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_common_type_request", OnNotifiCommonTypeStaticRequest);//警情常用类型统计

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_number_black_count_request", OnNotifiNumberBlackCountRequest);//警情常用类型统计

	ICC_LOG_DEBUG(m_pLog, "dbagent start success.");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	_DestoryThreads();

	ICC_LOG_DEBUG(m_pLog, "dbagent stop success.");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiNumberBlackCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv get number black count msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetNumberBlackCountRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetNumberBlackCountRespond l_oRespond;
	GenRespondHeaderEx("get_number_black_count_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strNumber = l_oRequest.m_oBody.m_strNumber;

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_number_black_count_by_number";
	l_SqlRequest.param["limit_num"] = l_oRequest.m_oBody.m_strNumber;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "query error,error msg:[%s],sql=%s", l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
	}
	else
	{
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "exec sql ok sql=%s", l_pResult->GetSQL().c_str());
		if (l_pResult->Next())
		{
			l_oRespond.m_oBody.m_strCount = l_pResult->GetValue("rcount");
		}
		else
		{
			l_oRespond.m_oBody.m_strCount = "0";
		}
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiCommonDeptStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv CommonDeptStatic msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetAlarmCommonInfoRequest l_oGetCommonDeptRequest;
	if (!l_oGetCommonDeptRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetAlarmCommonDeptRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.clear();
	GenRespondHeader("get_common_dept_respond", l_oGetCommonDeptRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_oGetCommonDeptRequest.m_oHeader.m_strMsgid;

	if (l_oGetCommonDeptRequest.m_oBody.m_strDeptCode.empty())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "dept code is empty!!!";
		string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotifiReqeust->Response(l_strMessage);
		return;
	}

	if (!SearchCommonDept(l_oGetCommonDeptRequest, l_oRespond))
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "search common dept failed!!!";
		string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotifiReqeust->Response(l_strMessage);
		return;
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiCommonTypeStaticRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv CommonTypeStatic alarm msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetAlarmCommonInfoRequest l_oGetCommonTypeRequest;
	if (!l_oGetCommonTypeRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetAlarmCommonTypeRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.clear();
	GenRespondHeader("get_common_type_respond", l_oGetCommonTypeRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_oGetCommonTypeRequest.m_oHeader.m_strMsgid;

	if (l_oGetCommonTypeRequest.m_oBody.m_strDeptCode.empty())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "dept code is empty!!!";
		string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotifiReqeust->Response(l_strMessage);
		return;
	}

	if (!SearchCommonType(l_oGetCommonTypeRequest, l_oRespond))
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "search common type failed!!!";
		string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotifiReqeust->Response(l_strMessage);
		return;
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_QueryBackAlarmData(const PROTOCOL::CSearchBackAlarmRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchBackAlarmRespond::CData>& vecDatas)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_back_alarm_info";
	l_SqlRequest.param["page_index"] = callInRequest.m_strPageIndex;
	l_SqlRequest.param["page_size"] = callInRequest.m_strPageSize;
	l_SqlRequest.param["begin_time"] = callInRequest.m_strBeginTime;
	l_SqlRequest.param["end_time"] = callInRequest.m_strEndTime;
	l_SqlRequest.param["dept_code"] = callInRequest.m_strDeptCode;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchBackAlarmRespond::CData data;
		data.m_strAlarmId = l_pResult->GetValue("alarm_id");
		data.m_strProcessId = l_pResult->GetValue("process_id");
		data.m_strBackUserCode = l_pResult->GetValue("update_user");
		data.m_strBackUserName = l_pResult->GetValue("update_user_name");
		data.m_strBackDeptCode = l_pResult->GetValue("back_dept_code");
		data.m_strBackDeptName = l_pResult->GetValue("back_dept_name");
		data.m_strBackTime = l_pResult->GetValue("update_time");
		data.m_strBackReason = l_pResult->GetValue("back_reason");

		vecDatas.push_back(data);
	}
}

void CBusinessImpl::OnNotifiSearchBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv search back alarm msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchBackAlarmRequest l_oSearchRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchBackAlarmRequest::CBody& l_oRequestBody = l_oSearchRequest.m_oBody;
	int iPageIndex = 0;
	int iPageSize = 0;
	_StandardizationPageParam(l_oRequestBody.m_strPageIndex, l_oRequestBody.m_strPageSize, iPageIndex, iPageSize);


	PROTOCOL::CSearchBackAlarmRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.resize(iPageSize);
	l_oRespond.m_oBody.m_vecData.clear();

	_QueryBackAlarmData(l_oRequestBody, l_oRespond.m_oBody.m_vecData);

	unsigned int iRealCount = std::min(iPageSize, (int)l_oRespond.m_oBody.m_vecData.size());
	GenRespondHeader("get_back_alarm_respond", l_oSearchRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iRealCount);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_back_alarm_count";
	l_SqlRequest.param["begin_time"] = l_oRequestBody.m_strBeginTime;
	l_SqlRequest.param["end_time"] = l_oRequestBody.m_strEndTime;
	l_SqlRequest.param["dept_code"] = l_oRequestBody.m_strDeptCode;
	l_oRespond.m_oBody.m_strAllCount = _StatisticsResultCount(l_SqlRequest);

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::_StatisticsSeatAlarmData(const PROTOCOL::CGetSeatAlarmStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetSeatAlarmStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;


	l_SqlRequest.sql_id = "statistics_icc_t_jjdb_count_groupby_seatno";
	//l_SqlRequest.param["receipt_dept_code"] = in_oStatAlarmRequest.m_oBody.m_strDeptCode;

	if (!in_oStatAlarmRequest.m_oBody.m_strBeginTime.empty())
	{
		l_SqlRequest.param["begin_time"] = in_oStatAlarmRequest.m_oBody.m_strBeginTime;

		if (!in_oStatAlarmRequest.m_oBody.m_strEndTime.empty())
		{
			l_SqlRequest.param["end_time"] = in_oStatAlarmRequest.m_oBody.m_strEndTime;
		}
	}

	//l_SqlRequest.param["groupby"] = "level";
	l_SqlRequest.param["groupby"] = "jjxwh";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	unsigned int tmp_uiCount = 0;
	while (l_pResult->Next())
	{
		PROTOCOL::CGetSeatAlarmStatisticsRespond::CSeatCount data;
		data.m_strSeatNo = l_pResult->GetValue("receipt_seatno");
		data.m_strCount = l_pResult->GetValue("num");

		tmp_uiCount += m_pString->ToUInt(data.m_strCount);
		out_oRespond.m_oBody.m_vecSeatCount.push_back(data);
	}

	out_oRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiCount);
	return true;
}

bool CBusinessImpl::_StatisticsAlarmData(const PROTOCOL::CGetAlarmStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetAlarmStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;

	//如果为空则根据人查，否则根据部门查
	if (in_oStatAlarmRequest.m_oBody.m_strDeptCode.empty())
	{
		l_SqlRequest.sql_id = "statistics_icc_t_jjdb_count_by_seatcode";

		//如果body中的坐席为空，才查本坐席的
		if (in_oStatAlarmRequest.m_oBody.m_strSeatNo.empty())
		{
			l_SqlRequest.param["receipt_seatno"] = in_oStatAlarmRequest.m_oHeader.m_strSeatNo;
		}
		else
		{
			l_SqlRequest.param["receipt_seatno"] = in_oStatAlarmRequest.m_oBody.m_strSeatNo;
		}
	}
	else
	{
		l_SqlRequest.sql_id = "statistics_icc_t_jjdb_count_by_deptcode";
		l_SqlRequest.param["receipt_dept_code"] = in_oStatAlarmRequest.m_oBody.m_strDeptCode;
	}
	if (!in_oStatAlarmRequest.m_oBody.m_strBeginTime.empty())
	{
		l_SqlRequest.param["begin_time"] = in_oStatAlarmRequest.m_oBody.m_strBeginTime;

		if (!in_oStatAlarmRequest.m_oBody.m_strEndTime.empty())
		{
			l_SqlRequest.param["end_time"] = in_oStatAlarmRequest.m_oBody.m_strEndTime;
		}
	}

	//l_SqlRequest.param["groupby"] = "level";
	l_SqlRequest.param["groupby"] = "jqdjdm";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	unsigned int tmp_uiCount = 0;
	while (l_pResult->Next())
	{
		PROTOCOL::CGetAlarmStatisticsRespond::CLevelCount data;
		data.m_strLevel = l_pResult->GetValue("level");
		data.m_strCount = l_pResult->GetValue("num");

		tmp_uiCount += m_pString->ToUInt(data.m_strCount);
		out_oRespond.m_oBody.m_vecLevelCount.push_back(data);
	}

	out_oRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiCount);
	return true;
}

void CBusinessImpl::OnNotifStatistcsSeatAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc alarm msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetSeatAlarmStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetSeatAlarmStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//只用较验开始时间
		if (l_oStatistsRequest.m_oBody.m_strBeginTime.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "begin time is empty";
			break;
		}

		if (!_StatisticsSeatAlarmData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

//统计告警
void CBusinessImpl::OnNotifStatistcsAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc alarm msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetAlarmStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetAlarmStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//只用较验开始时间
		if (l_oStatistsRequest.m_oBody.m_strBeginTime.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "begin time is empty";
			break;
		}

		if (!_StatisticsAlarmData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::_StatisticsCalleventData(const PROTOCOL::CGetCalleventStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetCalleventStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;

	l_SqlRequest.sql_id = "statistics_icc_t_callevent_count_by_seatcode";
	l_SqlRequest.param["seat_no"] = in_oStatAlarmRequest.m_oHeader.m_strSeatNo;

	if (!in_oStatAlarmRequest.m_oBody.m_strBeginTime.empty())
	{
		l_SqlRequest.param["begin_time"] = in_oStatAlarmRequest.m_oBody.m_strBeginTime;
	}

	if (!in_oStatAlarmRequest.m_oBody.m_strEndTime.empty())
	{
		l_SqlRequest.param["end_time"] = in_oStatAlarmRequest.m_oBody.m_strEndTime;
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		out_oRespond.m_oBody.m_strAllCount = l_pResult->GetValue("num");
	}

	return true;
}

//统计话务
void CBusinessImpl::OnNotifStatistcsCallEventRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc callevent msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetCalleventStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetCalleventStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//只用较验开始时间
		if (l_oStatistsRequest.m_oBody.m_strBeginTime.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "begin time is empty";
			break;
		}

		if (!_StatisticsCalleventData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::_StatisticsViolationData(const PROTOCOL::CGetViolationStatisticsRequest& in_oStatViolationRequest, PROTOCOL::CGetViolationStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;

	l_SqlRequest.sql_id = "statistics_icc_t_violation_count_by_seatcode";
	l_SqlRequest.param["seat_no"] = in_oStatViolationRequest.m_oHeader.m_strSeatNo;


	if (!in_oStatViolationRequest.m_oBody.m_strBeginTime.empty())
	{
		l_SqlRequest.param["begin_time"] = in_oStatViolationRequest.m_oBody.m_strBeginTime;

		if (!in_oStatViolationRequest.m_oBody.m_strEndTime.empty())
		{
			l_SqlRequest.param["end_time"] = in_oStatViolationRequest.m_oBody.m_strEndTime;
		}
	}

	l_SqlRequest.param["groupby"] = "violation_type";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	unsigned int tmp_uiCount = 0;
	while (l_pResult->Next())
	{
		PROTOCOL::CGetViolationStatisticsRespond::CData data;
		data.m_strViolationType = l_pResult->GetValue("violation_type");
		data.m_strCount = l_pResult->GetValue("num");

		tmp_uiCount += m_pString->ToUInt(data.m_strCount);
		out_oRespond.m_oBody.m_vecData.push_back(data);
	}

	out_oRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiCount);

	return true;
}

//统计违规
void CBusinessImpl::OnNotifStatistcsViolationRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc violation msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetViolationStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetViolationStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//只用较验开始时间
		if (l_oStatistsRequest.m_oBody.m_strBeginTime.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "begin time is empty";
			break;
		}

		if (!_StatisticsViolationData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::_StatisticsBlackListData(const PROTOCOL::CGetBlacklistStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetBlacklistStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;

	l_SqlRequest.sql_id = "statistics_icc_t_blacklist_count_by_phone";
	l_SqlRequest.param["limit_num"] = in_oStatAlarmRequest.m_oBody.m_strPhoneNum;

	if (!in_oStatAlarmRequest.m_oBody.m_strBeginTime.empty())
	{
		l_SqlRequest.param["begin_time"] = in_oStatAlarmRequest.m_oBody.m_strBeginTime;

		if (!in_oStatAlarmRequest.m_oBody.m_strEndTime.empty())
		{
			l_SqlRequest.param["end_time"] = in_oStatAlarmRequest.m_oBody.m_strEndTime;
		}

	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		out_oRespond.m_oBody.m_strCount = l_pResult->GetValue("num");
	}

	return true;
}

//统计黑名单
void CBusinessImpl::OnNotifStatistcsBlackListRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc blacklist msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetBlacklistStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetBlacklistStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//较验是否带了号码
		if (l_oStatistsRequest.m_oBody.m_strPhoneNum.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "phone num is empty";
			break;
		}

		if (!_StatisticsBlackListData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::_StatisticsPhoneMarkData(const PROTOCOL::CGetPhoneMarkStatisticsRequest& in_oStatAlarmRequest, PROTOCOL::CGetPhoneMarkStatisticsRespond& out_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;

	l_SqlRequest.sql_id = "statistics_icc_t_phonemark_by_phone";
	l_SqlRequest.param["phone"] = in_oStatAlarmRequest.m_oBody.m_strPhone;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pResult->GetSQL().c_str());

	if (l_pResult == nullptr)
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		out_oRespond.m_oHeader.m_strMsg = "exec sql error, sql = " + l_pResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "exec sql error: %s", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	unsigned int tmp_uiCount = 0;
	while (l_pResult->Next())
	{
		PROTOCOL::CGetPhoneMarkStatisticsRespond::CData data;
		data.m_strType = l_pResult->GetValue("type");
		data.m_strCount = l_pResult->GetValue("num");

		tmp_uiCount += m_pString->ToUInt(data.m_strCount);
		out_oRespond.m_oBody.m_vecData.push_back(data);
	}

	out_oRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiCount);

	return true;
}

//统计电话标记
void CBusinessImpl::OnNotifStatistcsPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv statistc phone mark msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CGetPhoneMarkStatisticsRequest l_oStatistsRequest;

	if (!l_oStatistsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "msg parse failed");
		return;
	}

	PROTOCOL::CGetPhoneMarkStatisticsRespond l_oStatistsRespond;
	l_oStatistsRespond.m_oHeader = l_oStatistsRequest.m_oHeader;
	do
	{
		//较验是否带了号码
		if (l_oStatistsRequest.m_oBody.m_strPhone.empty())
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "1";
			l_oStatistsRespond.m_oHeader.m_strMsg = "phone num is empty";
			break;
		}

		if (!_StatisticsPhoneMarkData(l_oStatistsRequest, l_oStatistsRespond))
		{
			l_oStatistsRespond.m_oHeader.m_strResult = "2";
			break;
		}

	} while (0);

	string l_strMessage = l_oStatistsRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_QueryCallWaitingData(const PROTOCOL::CSearchCallWaitingRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchCallWaitingRespond::CData>& vecDatas, std::string& strCount)
{
	int l_nPageSize = m_pString->ToInt(callInRequest.m_strPageSize.c_str());
	int l_nPageIndex = m_pString->ToInt(callInRequest.m_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;
	l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_call_waiting_info";
	l_SqlRequest.param["begin_time"] = callInRequest.m_strBeginTime;
	l_SqlRequest.param["end_time"] = callInRequest.m_strEndTime;
	l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(callInRequest.m_strBeginTime);
	l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(callInRequest.m_strEndTime);

	if (!callInRequest.m_strCallerId.empty())
	{
		l_SqlRequest.param["caller_id"] = callInRequest.m_strCallerId;
	}

	if (!callInRequest.m_strCalledId.empty())
	{
		l_SqlRequest.param["called_id"] = callInRequest.m_strCalledId;
	}

	if (!callInRequest.m_strHangupType.empty())
	{
		l_SqlRequest.param["hangup_type"] = callInRequest.m_strHangupType;
	}

	if (!callInRequest.m_strWaitlen.empty())
	{
		l_SqlRequest.param["wait_len"] = callInRequest.m_strWaitlen;
	}
	std::string strTmpLog;
	
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}
	
	strCount = m_pString->Format("%d", l_pResult->RecordSize());
	l_SqlRequest.param["limit"] = std::to_string(l_nPageSize);
	l_SqlRequest.param["offset"] = std::to_string(l_nPageIndex);
	l_pResult = m_pDBConn->Exec(l_SqlRequest);
	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchCallWaitingRespond::CData data;
		data.m_strCallRefId = l_pResult->GetValue("callref_id");
		data.m_strAcd = l_pResult->GetValue("acd");
		data.m_strOriginalAcd = l_pResult->GetValue("original_acd");
		data.m_strAcdDept = l_pResult->GetValue("acd_dept");
		data.m_strCallerNumber = l_pResult->GetValue("callernumber");
		data.m_strWaitStartTime = l_pResult->GetValue("state_time");
		data.m_strWaitLen = l_pResult->GetValue("waittime");
		data.m_strHangupType = l_pResult->GetValue("hangup_type");

		vecDatas.push_back(data);
	}
}

void CBusinessImpl::OnNotifiSearchCallWaitingRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv search call waiting msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchCallWaitingRequest l_oSearchRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchCallWaitingRequest::CBody& l_oRequestBody = l_oSearchRequest.m_oBody;
	int iPageIndex = 0;
	int iPageSize = 0;
	_StandardizationPageParam(l_oRequestBody.m_strPageIndex, l_oRequestBody.m_strPageSize, iPageIndex, iPageSize);


	PROTOCOL::CSearchCallWaitingRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.resize(iPageSize);
	l_oRespond.m_oBody.m_vecData.clear();
	std::string strCount;
	_QueryCallWaitingData(l_oRequestBody, l_oRespond.m_oBody.m_vecData, strCount);
	
	unsigned int iRealCount = std::min(iPageSize, (int)l_oRespond.m_oBody.m_vecData.size());
	GenRespondHeaderEx("get_callevent_waiting_respond", l_oSearchRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iRealCount);
	/*
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_call_waiting_count";

	l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(l_oRequestBody.m_strBeginTime);
	l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(l_oRequestBody.m_strEndTime);


	if (!l_oRequestBody.m_strCallerId.empty())
	{
		l_SqlRequest.param["caller_id"] = l_oRequestBody.m_strCallerId;

		std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequestBody.m_strCallerId);
		if (strTime != "")
		{
			l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	
	_QueryCallWaitingDataCount(l_oSearchRequest, strCount);
	*/
	l_oRespond.m_oBody.m_strAllCount = strCount;

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_StandardizationPageParam(const std::string& strPageIndex, const std::string& strPageSize, int& iPageIndex, int& iPageSize)
{
	iPageSize = m_pString->ToInt(strPageSize);
	iPageIndex = m_pString->ToInt(strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	iPageSize = std::min(iPageSize, m_iMaxPageSize);

	if (iPageSize <= 0)
	{//检查客户端传入的page_size参数
		iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", strPageSize.c_str(), iPageSize);
	}
	if (iPageIndex < 1)
	{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", strPageIndex.c_str(), iPageIndex);
	}
}

std::string CBusinessImpl::_StatisticsResultCount(DataBase::SQLRequest sqlRequest)
{
	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return STRING_RESULT_0;
	}


	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return STRING_RESULT_0;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s", l_pResult->GetSQL().c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	if (l_pResult->Next())
	{
		std::string strCount = l_pResult->GetValue("rcount");
		strTmpLog = m_pString->Format("statistics success! count = %s", strCount.c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		return strCount;
	}

	return STRING_RESULT_0;
}

void CBusinessImpl::_StatisticsCallInData(const PROTOCOL::CSearchStatisticsCallInRequest::CBody& callInRequest, std::vector<PROTOCOL::CSearchStatisticsCallInRespond::CData>& vecDatas)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_statistics_call_in_info";
	l_SqlRequest.param["dept_code"] = callInRequest.m_strStaffDeptCode;
	l_SqlRequest.param["page_index"] = callInRequest.m_strPageIndex;
	l_SqlRequest.param["page_size"] = callInRequest.m_strPageSize;
	l_SqlRequest.param["begin_time"] = callInRequest.m_strBeginTime;
	l_SqlRequest.param["end_time"] = callInRequest.m_strEndTime;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchStatisticsCallInRespond::CData data;
		data.m_strSeatNo = l_pResult->GetValue("no");
		data.m_strCalledCount = l_pResult->GetValue("counttotalcall");
		data.m_strCount110 = l_pResult->GetValue("count110");
		data.m_strCount119 = l_pResult->GetValue("count119");
		data.m_strCount122 = l_pResult->GetValue("count122");
		data.m_strCountHangupByCaller = l_pResult->GetValue("counthangupbycaller");
		data.m_strCountHangupByCalled = l_pResult->GetValue("counthangupbycalled");
		data.m_strCountHangupByBlack = l_pResult->GetValue("counthangupbyblack");
		data.m_strCountHangupByTimeout = STRING_RESULT_0;
		data.m_strCountHangupByRelease = l_pResult->GetValue("counthangupbyrelease");
		data.m_strCountTransfer = l_pResult->GetValue("counthangupbytransfer");
		data.m_strTotalTalkTime = l_pResult->GetValue("talktime");

		vecDatas.push_back(data);
	}
}

//呼入记录分类统计
void CBusinessImpl::OnNotifiSearchStatisticCallInRecordRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv search statistic call in msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchStatisticsCallInRequest l_oSearchStatisticsRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchStatisticsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchStatisticsCallInRequest::CBody& l_oRequestBody = l_oSearchStatisticsRequest.m_oBody;
	int iPageIndex = 0;
	int iPageSize = 0;
	_StandardizationPageParam(l_oRequestBody.m_strPageIndex, l_oRequestBody.m_strPageSize, iPageIndex, iPageSize);


	PROTOCOL::CSearchStatisticsCallInRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.resize(iPageSize);
	l_oRespond.m_oBody.m_vecData.clear();

	_StatisticsCallInData(l_oRequestBody, l_oRespond.m_oBody.m_vecData);

	unsigned int iRealCount = std::min(iPageSize, (int)l_oRespond.m_oBody.m_vecData.size());
	GenRespondHeader("get_called_bills_respond", l_oSearchStatisticsRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iRealCount);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_statistics_call_in_info_count";
	l_SqlRequest.param["dept_code"] = l_oRequestBody.m_strStaffDeptCode;
	l_oRespond.m_oBody.m_strAllCount = _StatisticsResultCount(l_SqlRequest);

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_StatisticsReceiveAlarmData(const PROTOCOL::CSearchStatisticsReceiveAlarmRequest::CBody& receiveAlarmRequest, std::vector<PROTOCOL::CSearchStatisticsReceiveAlarmRespond::CData>& vecDatas)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_statistics_receive_alarm_info";
	l_SqlRequest.param["dept_code"] = receiveAlarmRequest.m_strReceiptDeptCode.empty() ? receiveAlarmRequest.m_strStaffDeptCode : receiveAlarmRequest.m_strReceiptDeptCode;
	l_SqlRequest.param["page_index"] = receiveAlarmRequest.m_strPageIndex;
	l_SqlRequest.param["page_size"] = receiveAlarmRequest.m_strPageSize;
	l_SqlRequest.param["begin_time"] = receiveAlarmRequest.m_strBeginTime;
	l_SqlRequest.param["end_time"] = receiveAlarmRequest.m_strEndTime;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchStatisticsReceiveAlarmRespond::CData data;
		data.m_strDeptCode = l_pResult->GetValue("admin_dept_code");
		data.m_strDeptName = l_pResult->GetValue("dept_name");
		data.m_strReceiveAlarmCount = l_pResult->GetValue("alarmcount");
		data.m_strProcessCount = l_pResult->GetValue("processcount");
		data.m_strFeedbackCount = l_pResult->GetValue("feedbackcount");
		data.m_strRealAlarmCount = l_pResult->GetValue("realalarmcount");
		data.m_strCallAlarmCount = l_pResult->GetValue("callcount");

		vecDatas.push_back(data);
	}
}

//接警统计
void CBusinessImpl::OnNotifiSearchStatisticReceiveAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv search statistic receive alarm msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchStatisticsReceiveAlarmRequest l_oSearchStatisticsRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchStatisticsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchStatisticsReceiveAlarmRequest::CBody& l_oRequestBody = l_oSearchStatisticsRequest.m_oBody;
	int iPageIndex = 0;
	int iPageSize = 0;
	_StandardizationPageParam(l_oRequestBody.m_strPageIndex, l_oRequestBody.m_strPageSize, iPageIndex, iPageSize);

	PROTOCOL::CSearchStatisticsReceiveAlarmRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.resize(iPageSize);
	l_oRespond.m_oBody.m_vecData.clear();

	_StatisticsReceiveAlarmData(l_oRequestBody, l_oRespond.m_oBody.m_vecData);

	unsigned int iRealCount = std::min(iPageSize, (int)l_oRespond.m_oBody.m_vecData.size());
	GenRespondHeader("get_dept_bills _respond", l_oSearchStatisticsRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iRealCount);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_statistics_receive_alarm_info_count";
	l_SqlRequest.param["dept_code"] = l_oRequestBody.m_strStaffDeptCode;
	l_SqlRequest.param["begin_time"] = l_oRequestBody.m_strBeginTime;
	l_SqlRequest.param["end_time"] = l_oRequestBody.m_strEndTime;
	l_oRespond.m_oBody.m_strAllCount = _StatisticsResultCount(l_SqlRequest);

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_StatisticsAlarmInfoData(const PROTOCOL::CSearchStatisticsAlarmRequest::CBody& receiveAlarmRequest, std::vector<PROTOCOL::CSearchStatisticsAlarmRespond::CData>& vecDatas)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = receiveAlarmRequest.m_strFirstType.empty() ? "query_statistics_alarm_info" : "query_statistics_alarm_info_by_first_type";
	l_SqlRequest.param["dept_code"] = receiveAlarmRequest.m_strAdminDeptCode.empty() ? receiveAlarmRequest.m_strStaffDeptCode : receiveAlarmRequest.m_strAdminDeptCode;
	l_SqlRequest.param["page_index"] = receiveAlarmRequest.m_strPageIndex;
	l_SqlRequest.param["page_size"] = receiveAlarmRequest.m_strPageSize;
	l_SqlRequest.param["begin_time"] = receiveAlarmRequest.m_strBeginTime;
	l_SqlRequest.param["end_time"] = receiveAlarmRequest.m_strEndTime;
	if (!receiveAlarmRequest.m_strFirstType.empty())
	{
		l_SqlRequest.param["first_type"] = receiveAlarmRequest.m_strFirstType;
	}

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchStatisticsAlarmRespond::CData data;
		data.m_strTypeCode = l_pResult->GetValue("first_type");
		data.m_strTypeName = l_pResult->GetValue("first_type_name");
		data.m_strAdminDeptCode = l_pResult->GetValue("admin_dept_code");
		data.m_strAdminDeptName = l_pResult->GetValue("dept_name");
		data.m_strAlarmCount = l_pResult->GetValue("alarmcount");
		data.m_strProcessCount = l_pResult->GetValue("processcount");
		data.m_strFeedbackCount = l_pResult->GetValue("feedbackcount");

		vecDatas.push_back(data);
	}
}

//警情统计
void CBusinessImpl::OnNotifiSearchStatisticAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv search statistic alarm info msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchStatisticsAlarmRequest l_oSearchStatisticsRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchStatisticsRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchStatisticsAlarmRequest::CBody& l_oRequestBody = l_oSearchStatisticsRequest.m_oBody;
	int iPageIndex = 0;
	int iPageSize = 0;
	_StandardizationPageParam(l_oRequestBody.m_strPageIndex, l_oRequestBody.m_strPageSize, iPageIndex, iPageSize);

	PROTOCOL::CSearchStatisticsAlarmRespond l_oRespond;
	l_oRespond.m_oBody.m_vecData.resize(iPageSize);
	l_oRespond.m_oBody.m_vecData.clear();

	_StatisticsAlarmInfoData(l_oRequestBody, l_oRespond.m_oBody.m_vecData);

	unsigned int iRealCount = std::min(iPageSize, (int)l_oRespond.m_oBody.m_vecData.size());
	GenRespondHeader("get_type_bills _respond", l_oSearchStatisticsRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", iRealCount);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = l_oRequestBody.m_strFirstType.empty() ? "query_statistics_alarm_info_count" : "query_statistics_alarm_info_count_by_first_type";
	l_SqlRequest.param["dept_code"] = l_oRequestBody.m_strStaffDeptCode;
	l_SqlRequest.param["begin_time"] = l_oRequestBody.m_strBeginTime;
	l_SqlRequest.param["end_time"] = l_oRequestBody.m_strEndTime;

	if (!l_oSearchStatisticsRequest.m_oBody.m_strFirstType.empty())
	{
		l_SqlRequest.param["first_type"] = l_oRequestBody.m_strFirstType;
	}
	l_oRespond.m_oBody.m_strAllCount = _StatisticsResultCount(l_SqlRequest);

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotifiReqeust->Response(l_strMessage);

	ICC_LOG_INFO(m_pLog, "response complete:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::GetAlarmProcessByAlarmID(const std::string& p_strAlarmID, std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& p_vecProcess)
{
	DataBase::SQLRequest l_oSeleteProcessSQLReq;
	l_oSeleteProcessSQLReq.sql_id = "select_icc_t_pjdb";
	l_oSeleteProcessSQLReq.param["alarm_id"] = p_strAlarmID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_oSeleteProcessSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_oSeleteProcessSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteProcessSQLReq, true);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get process by alarm failed :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_LOWDEBUG(m_pLog, "get process by alarm success, [%s]", p_strAlarmID.c_str());

	while (l_pResult->Next())
	{
		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_oProcess;
		l_oProcess.m_bIsNewProcess = false;
		l_oProcess.m_strDispatchDeptDistrictCode = l_pResult->GetValue("dispatch_dept_district_code");
		l_oProcess.m_strID = l_pResult->GetValue("id");
		l_oProcess.m_strAlarmID = l_pResult->GetValue("alarm_id");
		l_oProcess.m_strDispatchDeptCode = l_pResult->GetValue("dispatch_dept_code");
		l_oProcess.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
		l_oProcess.m_strDispatchName = l_pResult->GetValue("dispatch_name");
		l_oProcess.m_strRecordID = l_pResult->GetValue("record_id");
		l_oProcess.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		l_oProcess.m_strProcessDeptCode = l_pResult->GetValue("process_dept_code");
		l_oProcess.m_strTimeSubmit = l_pResult->GetValue("time_submit");
		l_oProcess.m_strTimeArrived = l_pResult->GetValue("time_arrived");
		l_oProcess.m_strTimeSigned = l_pResult->GetValue("time_signed");
		l_oProcess.m_strProcessName = l_pResult->GetValue("process_name");
		l_oProcess.m_strProcessCode = l_pResult->GetValue("process_code");
		l_oProcess.m_strDispatchPersonnel = l_pResult->GetValue("dispatch_personnel");
		l_oProcess.m_strDispatchVehicles = l_pResult->GetValue("dispatch_vehicles");
		l_oProcess.m_strDispatchBoats = l_pResult->GetValue("dispatch_boats");
		l_oProcess.m_strState = l_pResult->GetValue("state");
		l_oProcess.m_strCreateTime = l_pResult->GetValue("create_time");
		l_oProcess.m_strUpdateTime = l_pResult->GetValue("update_time");
		l_oProcess.m_strDispatchDeptName = l_pResult->GetValue("dispatch_dept_name");
		l_oProcess.m_strDispatchDeptOrgCode = l_pResult->GetValue("dispatch_dept_org_code");
		l_oProcess.m_strProcessDeptName = l_pResult->GetValue("process_dept_name");
		l_oProcess.m_strProcessDeptOrgCode = l_pResult->GetValue("process_dept_org_code");
		l_oProcess.m_strProcessObjectType = l_pResult->GetValue("process_object_type");
		l_oProcess.m_strProcessObjectName = l_pResult->GetValue("process_object_name");
		l_oProcess.m_strProcessObjectCode = l_pResult->GetValue("process_object_code");
		l_oProcess.m_strBusinessStatus = l_pResult->GetValue("business_status");
		l_oProcess.m_strSeatCode = l_pResult->GetValue("seat_code");
		l_oProcess.m_strCancelTime = l_pResult->GetValue("cancel_time");
		l_oProcess.m_strCancelReason = l_pResult->GetValue("cancel_reason");
		l_oProcess.m_strIsAutoAssignJob = l_pResult->GetValue("is_auto_assign_job", "0");
		l_oProcess.m_strCreateUser = l_pResult->GetValue("create_user");
		l_oProcess.m_strUpdateUser = l_pResult->GetValue("update_user");
		l_oProcess.m_strOvertimeState = l_pResult->GetValue("overtime_state");

		l_oProcess.m_strProcessObjectID = l_pResult->GetValue("process_object_id");
		l_oProcess.m_strTransfDeptOrjCode = l_pResult->GetValue("transfers_dept_org_code");
		l_oProcess.m_strIsOver = l_pResult->GetValue("is_over", "0");
		l_oProcess.m_strOverRemark = l_pResult->GetValue("over_remark");
		l_oProcess.m_strParentID = l_pResult->GetValue("parent_id");
		l_oProcess.m_strGZLDM = l_pResult->GetValue("flow_code");

		l_oProcess.m_strCentreProcessDeptCode = l_pResult->GetValue("centre_process_dept_code");
		l_oProcess.m_strCentreAlarmDeptCode = l_pResult->GetValue("centre_alarm_dept_code");
		l_oProcess.m_strDispatchDeptDistrictName = l_pResult->GetValue("dispatch_dept_district_name");
		l_oProcess.m_strLinkedDispatchCode = l_pResult->GetValue("linked_dispatch_code");
		l_oProcess.m_strOverTime = l_pResult->GetValue("over_time");
		l_oProcess.m_strFinishedTimeoutState = l_pResult->GetValue("finished_timeout_state");
		l_oProcess.m_strPoliceType = l_pResult->GetValue("police_type");
		l_oProcess.m_strProcessDeptShortName = l_pResult->GetValue("process_dept_short_name");
		l_oProcess.m_strDispatchDeptShortName = l_pResult->GetValue("dispatch_dept_short_name");
		l_oProcess.m_strReceivedTime = l_pResult->GetValue("received_time");
		l_oProcess.m_strCreateTeminal = l_pResult->GetValue("createteminal");
		l_oProcess.m_strUpdateTeminal = l_pResult->GetValue("updateteminal");
		//if (l_oProcess.m_strState != PROCESS_STATUS_CANCEL)
		{
			p_vecProcess.push_back(l_oProcess);
		}
	}
	return true;
}


//警情查询
void CBusinessImpl::OnNotifiGetAlarmAllRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchAlarmRequest l_oSearchAlarmRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchAlarmRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	std::string l_strCondition;
	PROTOCOL::CSearchAlarmRequest::CBody& l_oRequestBody = l_oSearchAlarmRequest.m_oBody;
	//BuildQueryCondition(l_oRequestBody, l_strCondition);

	int l_iPageSize = m_pString->ToInt(l_oRequestBody.m_strPageSize);
	int l_iPageIndex = m_pString->ToInt(l_oRequestBody.m_strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	l_iPageSize = std::min(l_iPageSize, m_iMaxPageSize);

	if (l_iPageSize <= 0)
	{
		//检查客户端传入的page_size参数
		l_iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oRequestBody.m_strPageSize.c_str(), l_iPageSize);
	}

	if (l_iPageSize >= 100)
	{
		//检查客户端传入的page_size参数
		l_iPageSize = 100;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oRequestBody.m_strPageSize.c_str(), l_iPageSize);
	}

	if (l_iPageIndex < 1)
	{
		//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		l_iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_oRequestBody.m_strPageSize.c_str(), l_iPageIndex);
	}

	//根据请求参数构造查询条件	
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "seach_alarm_count";

	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_oRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strBeginTime.empty())
	{
		l_strBeginTime = m_pDateTime->ToString(m_pDateTime->AddDays(m_pDateTime->CurrentDateTime(), -1));//如果时间为空，默认只查一天 [10/24/2019 151116314]
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}

	tmp_oSQLRequst.param["gxbegin_time"] = l_strBeginTime;
	tmp_oSQLRequst.param["gxend_time"] = l_strEndTime;
	tmp_oSQLRequst.param["jjsj_end"] = l_strEndTime;
	std::string l_strAlarmID = m_pString->Trim(l_oRequestBody.m_strID);
	if (!l_strAlarmID.empty())
	{
		tmp_oSQLRequst.param["id"] = l_strAlarmID;
	}

	//另用一个线程查询警情数量
	boost::thread l_oThreadQueryAlarmCount(boost::bind(&CBusinessImpl::QueryAlarmAllCount, this, tmp_oSQLRequst, l_oSearchAlarmRequest.m_oHeader.m_strMsgId));
	PROTOCOL::CSearchAlarmAllRespond l_oRespond;
	GenRespondHeader("search_alarm_respond", l_oSearchAlarmRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = "0";
	l_oRespond.m_oBody.m_strTotalAlarmCount = "0";
	l_oRespond.m_oHeader.m_strMsgId = l_oSearchAlarmRequest.m_oHeader.m_strMsgId;
	tmp_oSQLRequst.sql_id = "seach_alarm_data";
	tmp_oSQLRequst.param["orderby"] = "alarm.gxsj";
	tmp_oSQLRequst.param["limit"] = std::to_string(l_iPageSize);
	tmp_oSQLRequst.param["offset"] = std::to_string(l_iRequestIdxBegin);

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(tmp_oSQLRequst, true);
	ICC_LOG_INFO(m_pLog, "query sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "query alarm exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
	}
	else if (l_pRSet->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_pRSet->RecordSize();
		//l_oRespond.m_oBody.m_strCount = l_pRSet->GetValue(0, "count"); //分包总数
		l_oRespond.m_oBody.m_strCount = std::to_string(l_iRecordSize); //结果集警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm searched,begin to build respond msg,[%s] alarm in total,this msg contains [%d] alarm(s)", l_oRespond.m_oBody.m_strCount.c_str(), l_pRSet->RecordSize());

		for (size_t index = 0; index < l_iRecordSize;)
		{
			PROTOCOL::CSearchAlarmAllRespond::CData l_oAlarmData;

			l_oAlarmData.m_oAlarm.m_strID = l_pRSet->GetValue(index, "id");
			l_oAlarmData.m_oAlarm.m_strMergeID = l_pRSet->GetValue(index, "merge_id");
			l_oAlarmData.m_oAlarm.m_strSeatNo = l_pRSet->GetValue(index, "receipt_seatno");
			l_oAlarmData.m_oAlarm.m_strTitle = l_pRSet->GetValue(index, "label");
			l_oAlarmData.m_oAlarm.m_strContent = l_pRSet->GetValue(index, "content");
			l_oAlarmData.m_oAlarm.m_strTime = l_pRSet->GetValue(index, "receiving_time");
			l_oAlarmData.m_oAlarm.m_strReceivedTime = l_pRSet->GetValue(index, "received_time");
			l_oAlarmData.m_oAlarm.m_strAddr = l_pRSet->GetValue(index, "addr");
			l_oAlarmData.m_oAlarm.m_strLongitude = l_pRSet->GetValue(index, "longitude");
			l_oAlarmData.m_oAlarm.m_strLatitude = l_pRSet->GetValue(index, "latitude");
			l_oAlarmData.m_oAlarm.m_strState = l_pRSet->GetValue(index, "state");
			l_oAlarmData.m_oAlarm.m_strLevel = l_pRSet->GetValue(index, "level");
			l_oAlarmData.m_oAlarm.m_strSourceType = l_pRSet->GetValue(index, "source_type");
			l_oAlarmData.m_oAlarm.m_strSourceID = l_pRSet->GetValue(index, "source_id");
			l_oAlarmData.m_oAlarm.m_strHandleType = l_pRSet->GetValue(index, "handle_type");
			l_oAlarmData.m_oAlarm.m_strFirstType = l_pRSet->GetValue(index, "first_type");
			l_oAlarmData.m_oAlarm.m_strSecondType = l_pRSet->GetValue(index, "second_type");
			l_oAlarmData.m_oAlarm.m_strThirdType = l_pRSet->GetValue(index, "third_type");
			l_oAlarmData.m_oAlarm.m_strFourthType = l_pRSet->GetValue(index, "fourth_type");
			l_oAlarmData.m_oAlarm.m_strVehicleNo = l_pRSet->GetValue(index, "vehicle_no");
			l_oAlarmData.m_oAlarm.m_strVehicleType = l_pRSet->GetValue(index, "vehicle_type");
			l_oAlarmData.m_oAlarm.m_strSymbolCode = l_pRSet->GetValue(index, "symbol_code");
			l_oAlarmData.m_oAlarm.m_strCalledNoType = l_pRSet->GetValue(index, "called_no_type");
			l_oAlarmData.m_oAlarm.m_strCallerNo = l_pRSet->GetValue(index, "caller_no");
			l_oAlarmData.m_oAlarm.m_strCallerName = l_pRSet->GetValue(index, "caller_name");
			l_oAlarmData.m_oAlarm.m_strCallerAddr = l_pRSet->GetValue(index, "caller_addr");
			l_oAlarmData.m_oAlarm.m_strCallerID = l_pRSet->GetValue(index, "caller_id");
			l_oAlarmData.m_oAlarm.m_strCallerIDType = l_pRSet->GetValue(index, "caller_id_type");
			l_oAlarmData.m_oAlarm.m_strCallerGender = l_pRSet->GetValue(index, "caller_gender");
			l_oAlarmData.m_oAlarm.m_strContactNo = l_pRSet->GetValue(index, "contact_no");
			l_oAlarmData.m_oAlarm.m_strAdminDeptCode = l_pRSet->GetValue(index, "admin_dept_code");
			l_oAlarmData.m_oAlarm.m_strAdminDeptName = l_pRSet->GetValue(index, "admin_dept_name");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptDistrictCode = l_pRSet->GetValue(index, "receipt_dept_district_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptCode = l_pRSet->GetValue(index, "receipt_dept_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptName = l_pRSet->GetValue(index, "receipt_dept_name");
			l_oAlarmData.m_oAlarm.m_strReceiptCode = l_pRSet->GetValue(index, "receipt_code");
			l_oAlarmData.m_oAlarm.m_strReceiptName = l_pRSet->GetValue(index, "receipt_name");
			l_oAlarmData.m_oAlarm.m_strCreateTime = l_pRSet->GetValue(index, "create_time");
			l_oAlarmData.m_oAlarm.m_strCreateUser = l_pRSet->GetValue(index, "create_user");
			l_oAlarmData.m_oAlarm.m_strUpdateTime = l_pRSet->GetValue(index, "update_time");
			l_oAlarmData.m_oAlarm.m_strUpdateUser = l_pRSet->GetValue(index, "update_user");
			l_oAlarmData.m_oAlarm.m_strPrivacy = l_pRSet->GetValue(index, "is_privacy");
			l_oAlarmData.m_oAlarm.m_strRemark = l_pRSet->GetValue(index, "remark");
			l_oAlarmData.m_oAlarm.m_strIsVisitor = l_pRSet->GetValue(index, "is_visitor");
			l_oAlarmData.m_oAlarm.m_strIsFeedBack = l_pRSet->GetValue(index, "is_feedback");
			//新增
			l_oAlarmData.m_oAlarm.m_strAlarmAddr = l_pRSet->GetValue(index, "alarm_addr");
			l_oAlarmData.m_oAlarm.m_strCallerUserName = l_pRSet->GetValue(index, "caller_user_name");
			l_oAlarmData.m_oAlarm.m_strErpetratorsNumber = l_pRSet->GetValue(index, "erpetrators_number");

			l_oAlarmData.m_oAlarm.m_strIsArmed = l_pRSet->GetValue(index, "is_armed");
			l_oAlarmData.m_oAlarm.m_strIsHazardousSubstances = l_pRSet->GetValue(index, "is_hazardous_substances");
			l_oAlarmData.m_oAlarm.m_strIsExplosionOrLeakage = l_pRSet->GetValue(index, "is_explosion_or_leakage");

			l_oAlarmData.m_oAlarm.m_strDescOfTrapped = l_pRSet->GetValue(index, "desc_of_trapped");
			l_oAlarmData.m_oAlarm.m_strDescOfInjured = l_pRSet->GetValue(index, "desc_of_injured");
			l_oAlarmData.m_oAlarm.m_strDescOfDead = l_pRSet->GetValue(index, "desc_of_dead");
			l_oAlarmData.m_oAlarm.m_strIsForeignLanguage = l_pRSet->GetValue(index, "is_foreign_language");
			l_oAlarmData.m_oAlarm.m_strManualLongitude = l_pRSet->GetValue(index, "manual_longitude");
			l_oAlarmData.m_oAlarm.m_strManualLatitude = l_pRSet->GetValue(index, "manual_latitude");
			l_oAlarmData.m_oAlarm.m_strEmergencyRescueLevel = l_pRSet->GetValue(index, "emergency_rescue_level");
			l_oAlarmData.m_oAlarm.m_strIsHazardousVehicle = l_pRSet->GetValue(index, "is_hazardous_vehicle");
			l_oAlarmData.m_oAlarm.m_strReceiptSrvName = l_pRSet->GetValue(index, "receipt_srv_name");
			l_oAlarmData.m_oAlarm.m_strAdminDeptOrgCode = l_pRSet->GetValue(index, "admin_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptOrgCode = l_pRSet->GetValue(index, "receipt_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strIsInvalid = l_pRSet->GetValue(index, "is_invalid");
			l_oAlarmData.m_oAlarm.m_strBusinessState = l_pRSet->GetValue(index, "business_status");
			l_oAlarmData.m_oAlarm.m_strFirstSubmitTime = l_pRSet->GetValue(index, "first_submit_time");

			l_oAlarmData.m_oAlarm.m_strFirstSubmitTime = l_pRSet->GetValue(index, "first_submit_time");

			l_oAlarmData.m_oAlarm.m_strIsOver = l_pRSet->GetValue(index, "is_over");
			l_oAlarmData.m_oAlarm.m_strHadPush = l_pRSet->GetValue(index, "had_push");

			l_oAlarmData.m_oAlarm.m_strIsSameForBackground = l_pRSet->GetValue(index, "is_sameforbackground");

			l_oAlarmData.m_oAlarm.m_strIsProcessFlagSynchronized = l_pRSet->GetValue(index, "is_processflagsynchronized");

			l_oAlarmData.m_oAlarm.m_strDeleteFlag = l_pRSet->GetValue(index, "is_delete");
			std::string l_strIsMerge(l_pRSet->GetValue(index, "is_merge"));
			l_oAlarmData.m_oAlarm.m_strIsMerge = l_strIsMerge == "1" || m_pString->Lower(l_strIsMerge) == "true" ? "1" : "0";

			l_oAlarmData.m_oAlarm.m_strJurisdictionalOrgcode = l_pRSet->GetValue(index, "jurisdictional_orgcode");
			l_oAlarmData.m_oAlarm.m_strJurisdictionalOrgname = l_pRSet->GetValue(index, "jurisdictional_orgname");
			l_oAlarmData.m_oAlarm.m_strJurisdictionalOrgidentifier = l_pRSet->GetValue(index, "jurisdictional_orgidentifier");
			l_oAlarmData.m_oAlarm.m_strHadPush = l_pRSet->GetValue(index, "had_push");
			l_oAlarmData.m_oAlarm.m_strIsOver = l_pRSet->GetValue(index, "is_over");
			
			l_oAlarmData.m_oAlarm.m_strCreateTeminal = l_pRSet->GetValue(index, "createteminal");
			l_oAlarmData.m_oAlarm.m_strUpdateTeminal = l_pRSet->GetValue(index, "updateteminal");
			l_oAlarmData.m_oAlarm.m_strAlarmSourceType = l_pRSet->GetValue(index, "alarm_source_type");
			l_oAlarmData.m_oAlarm.m_strAlarmSourceId = l_pRSet->GetValue(index, "alarm_source_id");
			l_oAlarmData.m_oAlarm.m_strCentreDeptCode = l_pRSet->GetValue(index, "centre_dept_code");
			l_oAlarmData.m_oAlarm.m_strCentreRelationDeptCode = l_pRSet->GetValue(index, "centre_relation_dept_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptDistrictName = l_pRSet->GetValue(index, "receipt_dept_district_name");
			l_oAlarmData.m_oAlarm.m_strAlarmSystemReceiptDeptCode = l_pRSet->GetValue(index, "develop_dept_code");
			l_oAlarmData.m_oAlarm.m_strAlarmSystemReceiptDeptName = l_pRSet->GetValue(index, "develop_dept_name");
			l_oAlarmData.m_oAlarm.m_strLinkedDispatchCode = l_pRSet->GetValue(index, "linked_dispatch_code");
			l_oAlarmData.m_oAlarm.m_strIsSigned = l_pRSet->GetValue(index, "is_signed");
			l_oAlarmData.m_oAlarm.m_strOverTime = l_pRSet->GetValue(index, "over_time");
			l_oAlarmData.m_oAlarm.m_strInitialFirstType = l_pRSet->GetValue(index, "initial_first_type");
			l_oAlarmData.m_oAlarm.m_strInitialSecondType = l_pRSet->GetValue(index, "initial_second_type");
			l_oAlarmData.m_oAlarm.m_strInitialThirdType = l_pRSet->GetValue(index, "initial_third_type");
			l_oAlarmData.m_oAlarm.m_strInitialFourthType = l_pRSet->GetValue(index, "initial_fourth_type");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptCode = l_pRSet->GetValue(index, "initial_admin_dept_code");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptName = l_pRSet->GetValue(index, "initial_admin_dept_name");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptOrgCode = l_pRSet->GetValue(index, "initial_admin_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strInitialAddr = l_pRSet->GetValue(index, "initial_addr");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptShortName = l_pRSet->GetValue(index, "receipt_dept_short_name");
			l_oAlarmData.m_oAlarm.m_strAdminDeptShortName = l_pRSet->GetValue(index, "admin_dept_short_name");
			GetAlarmProcessByAlarmID(l_oAlarmData.m_oAlarm.m_strID, l_oAlarmData.m_vecProcessData);
			l_oRespond.m_oBody.m_vecData.push_back(l_oAlarmData);
			++index;
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "no alarm respond to client, alarm count in query result=[%d],request pagesize=[%d],request pageindex=[%d],request index[%d-%d]",
			l_pRSet->RecordSize(), l_iPageSize, l_iPageIndex, l_iRequestIdxBegin + 1, l_iRequestIdxEnd);
	}

	ICC_LOG_DEBUG(m_pLog, "wait for the returning of alarm_count_thread");
	l_oThreadQueryAlarmCount.join();//等警情数量查询线程返回
	ICC_LOG_DEBUG(m_pLog, "alarm_count_thread returned");
	DataBase::IResultSetPtr l_pAlarmAllCount;
	l_pAlarmAllCount = m_mapSelectAllAlarmCount[l_oSearchAlarmRequest.m_oHeader.m_strMsgId];

	//设置响应消息的警情总数
	if (!l_pAlarmAllCount || !l_pAlarmAllCount->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "query alarm count error:[%s]\nsql[%s],", l_pAlarmAllCount->GetErrorMsg().c_str(), l_pAlarmAllCount->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "set alarm count = RecordSize[%d]", l_pRSet->RecordSize());
		l_oRespond.m_oBody.m_strTotalAlarmCount = l_pRSet->RecordSize();
	}
	else
	{
		l_oRespond.m_oBody.m_strTotalAlarmCount = l_pAlarmAllCount->GetValue(0, "count"); //数据库中满足查询条件的警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm count [%s]", l_oRespond.m_oBody.m_strCount.c_str());
	}

	{
		std::lock_guard<std::mutex> lock(m_mutexSelectAllAlarmCount);
		m_mapSelectAllAlarmCount.erase(l_oSearchAlarmRequest.m_oHeader.m_strMsgId);
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response,msgID:[%s],count;[%s], data:[%s]", l_oSearchAlarmRequest.m_oHeader.m_strMsgId.c_str(), l_oRespond.m_oBody.m_strTotalAlarmCount.c_str(), l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);
}


void CBusinessImpl::OnNotifiAddOrUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSearchCallerRequest l_oSetCallerRequest;
	if (!l_oSetCallerRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	PROTOCOL::CSetCallerRespond l_oCallerRespond;
	l_oCallerRespond.m_oHeader.m_strMsgId = l_oSetCallerRequest.m_oHeader.m_strMsgId;
	l_oCallerRespond.m_oHeader.m_strMsg = "success";
	l_oCallerRespond.m_oHeader.m_strCode = "200";
	std::string p_strSyncType = "";
	std::string l_strMessage = "";
	if (l_oSetCallerRequest.m_oBody.m_str_id.empty())
	{
		l_oCallerRespond.m_oHeader.m_strMsg = "id is empty";
		l_oCallerRespond.m_oHeader.m_strCode = "0";
		l_strMessage = l_oCallerRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		return;
	}
	if (SearchCallerData(l_oSetCallerRequest.m_oBody.m_str_id))
	{
		if (!UpdateCallerData(l_oSetCallerRequest.m_oBody)) {
			l_oCallerRespond.m_oHeader.m_strMsg = "Update sql failed";
			l_oCallerRespond.m_oHeader.m_strCode = "1";
		}
		p_strSyncType = "2";
	}
	else
	{
		if (!InsertCallerData(l_oSetCallerRequest.m_oBody))
		{
			l_oCallerRespond.m_oHeader.m_strMsg = "Insert sql failed";
			l_oCallerRespond.m_oHeader.m_strCode = "2";
		}
		p_strSyncType = "1";
	}

	l_strMessage = l_oCallerRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	SendAlarmCallerInfoSync(l_oSetCallerRequest, p_strSyncType);
}


void CBusinessImpl::OnNotifiUpdateCallerRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSearchCallerRequest l_oSetCallerRequest;
	if (!l_oSetCallerRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	std::string p_strSyncType = "";
	std::string l_strMessage = "";
	if (l_oSetCallerRequest.m_oBody.m_str_id.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "Request Caller info is empty :[%s]", l_oSetCallerRequest.ToString(m_pJsonFty->CreateJson()).c_str());
		return;
	}
	if (SearchCallerData(l_oSetCallerRequest.m_oBody.m_str_id))
	{
		if (!UpdateCallerData(l_oSetCallerRequest.m_oBody)) {
			ICC_LOG_ERROR(m_pLog, "Request Update sql failed:");
		}
	}
	else
	{
		if (!InsertCallerData(l_oSetCallerRequest.m_oBody))
		{
			ICC_LOG_ERROR(m_pLog, "Request Insert sql failed:");
		}
	}
}

void CBusinessImpl::SendAlarmCallerInfoSync(const PROTOCOL::CSearchCallerRequest p_CAlarmcallerInfoEx, const std::string p_strSyncType)
{
	PROTOCOL::CSearchCallerRequest l_CAlarmRelatedCarsChangeSync;
	l_CAlarmRelatedCarsChangeSync = p_CAlarmcallerInfoEx;
	l_CAlarmRelatedCarsChangeSync.m_oBody.m_strOperateType = p_strSyncType;

	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRelatedID = "";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strCmd = "caller_info_sync_request";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequest = "topic_alarm_notice_sync";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestType = "1";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponse = p_CAlarmcallerInfoEx.m_oHeader.m_strResponse;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponseType = "";

	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";

	//同步消息
	std::string l_strMsg(l_CAlarmRelatedCarsChangeSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

// 更新涉案人员数据
bool CBusinessImpl::UpdateCallerData(PROTOCOL::CSearchCallerRequest::CBody synCallerData)
{
	DataBase::SQLRequest l_tSQLReqInsertCaller;
	l_tSQLReqInsertCaller.sql_id = "update_icc_t_involved_party";
	l_tSQLReqInsertCaller.param["id"] = synCallerData.m_str_id;
	if (!synCallerData.m_str_alarm_id.empty())
	{
		l_tSQLReqInsertCaller.set["alarm_id"] = synCallerData.m_str_alarm_id;
	}
	if (!synCallerData.m_str_process_id.empty())
	{
		l_tSQLReqInsertCaller.set["process_id"] = synCallerData.m_str_process_id;
	}
	if (!synCallerData.m_str_feedback_id.empty())
	{
		l_tSQLReqInsertCaller.set["feedback_id"] = synCallerData.m_str_feedback_id;
	}
	if (!synCallerData.m_str_centre_process_dept_code.empty())
	{
		l_tSQLReqInsertCaller.set["centre_process_dept_code"] = synCallerData.m_str_centre_process_dept_code;
	}
	if (!synCallerData.m_str_centre_alarm_dept_code.empty())
	{
		l_tSQLReqInsertCaller.set["centre_alarm_dept_code"] = synCallerData.m_str_centre_alarm_dept_code;
	}
	if (!synCallerData.m_str_centre_process_id.empty())
	{
		l_tSQLReqInsertCaller.set["centre_process_id"] = synCallerData.m_str_centre_process_id;
	}
	if (!synCallerData.m_str_centre_feedback_id.empty())
	{
		l_tSQLReqInsertCaller.set["centre_feedback_id"] = synCallerData.m_str_centre_feedback_id;
	}
	if (!synCallerData.m_str_district.empty())
	{
		l_tSQLReqInsertCaller.set["district"] = synCallerData.m_str_district;
	}
	if (!synCallerData.m_str_district_name.empty())
	{
		l_tSQLReqInsertCaller.set["district_name"] = synCallerData.m_str_district_name;
	}
	if (!synCallerData.m_str_name.empty())
	{
		l_tSQLReqInsertCaller.set["name"] = synCallerData.m_str_name;
	}
	if (!synCallerData.m_str_sex.empty())
	{
		l_tSQLReqInsertCaller.set["sex"] = synCallerData.m_str_sex;
	}
	if (!synCallerData.m_str_identification_type.empty())
	{
		l_tSQLReqInsertCaller.set["identification_type"] = synCallerData.m_str_identification_type;
	}
	if (!synCallerData.m_str_identification_id.empty())
	{
		l_tSQLReqInsertCaller.set["identification_id"] = synCallerData.m_str_identification_id;
	}
	if (!synCallerData.m_str_identity.empty())
	{
		l_tSQLReqInsertCaller.set["identity"] = synCallerData.m_str_identity;
	}
	if (!synCallerData.m_str_is_focus_pesron.empty())
	{
		l_tSQLReqInsertCaller.set["is_focus_pesron"] = synCallerData.m_str_is_focus_pesron;
	}
	if (!synCallerData.m_str_focus_pesron_info.empty())
	{
		l_tSQLReqInsertCaller.set["focus_pesron_info"] = synCallerData.m_str_focus_pesron_info;
	}
	if (!synCallerData.m_str_registered_address_division.empty())
	{
		l_tSQLReqInsertCaller.set["registered_address_division"] = synCallerData.m_str_registered_address_division;
	}
	if (!synCallerData.m_str_registered_address.empty())
	{
		l_tSQLReqInsertCaller.set["registered_address"] = synCallerData.m_str_registered_address;
	}
	if (!synCallerData.m_str_current_address_division.empty())
	{
		l_tSQLReqInsertCaller.set["current_address_division"] = synCallerData.m_str_current_address_division;
	}
	if (!synCallerData.m_str_current_address.empty())
	{
		l_tSQLReqInsertCaller.set["current_address"] = synCallerData.m_str_current_address;
	}
	if (!synCallerData.m_str_work_unit.empty())
	{
		l_tSQLReqInsertCaller.set["work_unit"] = synCallerData.m_str_work_unit;
	}
	if (!synCallerData.m_str_occupation.empty())
	{
		l_tSQLReqInsertCaller.set["occupation"] = synCallerData.m_str_occupation;
	}
	if (!synCallerData.m_str_contact_number.empty())
	{
		l_tSQLReqInsertCaller.set["contact_number"] = synCallerData.m_str_contact_number;
	}
	if (!synCallerData.m_str_other_certificates.empty())
	{
		l_tSQLReqInsertCaller.set["other_certificates"] = synCallerData.m_str_other_certificates;
	}
	if (!synCallerData.m_str_lost_item_information.empty())
	{
		l_tSQLReqInsertCaller.set["lost_item_information"] = synCallerData.m_str_lost_item_information;
	}
	if (!synCallerData.m_str_create_time.empty())
	{
		l_tSQLReqInsertCaller.set["create_time"] = synCallerData.m_str_create_time;
	} 
	else {
		l_tSQLReqInsertCaller.set["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	if (!synCallerData.m_str_update_time.empty())
	{
		l_tSQLReqInsertCaller.set["update_time"] = synCallerData.m_str_update_time;
	}
	else
	{
		l_tSQLReqInsertCaller.set["update_time"] =  m_pDateTime->CurrentDateTimeStr();
	}
	if (!synCallerData.m_str_date_of_birth.empty())
	{
		l_tSQLReqInsertCaller.set["date_of_birth"] = synCallerData.m_str_date_of_birth;
	}
	if (!synCallerData.m_str_digital_signature.empty())
	{
		l_tSQLReqInsertCaller.set["digital_signature"] = synCallerData.m_str_digital_signature;
	}
	if (!synCallerData.m_str_is_delete.empty())
	{
		l_tSQLReqInsertCaller.set["is_delete"] = synCallerData.m_str_is_delete;
	}
	else {
		l_tSQLReqInsertCaller.set["is_delete"] = "0";
	}
	if (!synCallerData.m_str_createTeminal.empty())
	{
		l_tSQLReqInsertCaller.set["createTeminal"] = synCallerData.m_str_createTeminal;
	}
	if (!synCallerData.m_str_updateTeminal.empty())
	{
		l_tSQLReqInsertCaller.set["updateTeminal"] = synCallerData.m_str_updateTeminal;
	}
	if (!synCallerData.m_str_nationality.empty())
	{
		l_tSQLReqInsertCaller.set["nationality"] = synCallerData.m_str_nationality;
	}
	if (!synCallerData.m_str_nation.empty())
	{
		l_tSQLReqInsertCaller.set["nation"] = synCallerData.m_str_nation;
	}
	if (!synCallerData.m_str_educationLevel.empty())
	{
		l_tSQLReqInsertCaller.set["educationLevel"] = synCallerData.m_str_educationLevel;
	}
	if (!synCallerData.m_str_marriageStatus.empty())
	{
		l_tSQLReqInsertCaller.set["marriageStatus"] = synCallerData.m_str_marriageStatus;
	}
	if (!synCallerData.m_str_politicalOutlook.empty())
	{
		l_tSQLReqInsertCaller.set["politicalOutlook"] = synCallerData.m_str_politicalOutlook;
	}
	if (!synCallerData.m_str_is_NPC_deputy.empty())
	{
		l_tSQLReqInsertCaller.set["is_NPC_deputy"] = synCallerData.m_str_is_NPC_deputy;
	}
	if (!synCallerData.m_str_is_national_staff.empty())
	{
		l_tSQLReqInsertCaller.set["is_national_staff"] = synCallerData.m_str_is_national_staff;
	}
	if (!synCallerData.m_str_createUserId.empty())
	{
		l_tSQLReqInsertCaller.set["createUserId"] = synCallerData.m_str_createUserId;
	}
	if (!synCallerData.m_str_createUserName.empty())
	{
		l_tSQLReqInsertCaller.set["createUserName"] = synCallerData.m_str_createUserName;
	}
	if (!synCallerData.m_str_createUserOrgCode.empty())
	{
		l_tSQLReqInsertCaller.set["createUserOrgCode"] = synCallerData.m_str_createUserOrgCode;
	}
	if (!synCallerData.m_str_createUserOrgName.empty())
	{
		l_tSQLReqInsertCaller.set["createUserOrgName"] = synCallerData.m_str_createUserOrgName;
	}
	if (!synCallerData.m_str_updateUserId.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserId"] = synCallerData.m_str_updateUserId;
	}
	if (!synCallerData.m_str_updateUserName.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserName"] = synCallerData.m_str_updateUserName;
	}
	if (!synCallerData.m_str_updateUserOrgCode.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserOrgCode"] = synCallerData.m_str_updateUserOrgCode;
	}
	if (!synCallerData.m_str_updateUserOrgName.empty())
	{
		l_tSQLReqInsertCaller.set["updateUserOrgName"] = synCallerData.m_str_updateUserOrgName;
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertCaller);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "RecvCallerData insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "RecvCallerData,%s Exec sql success! sql:[%s]", l_tSQLReqInsertCaller.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}
	return true;
}
// 插入涉案人员信息数据
bool CBusinessImpl::InsertCallerData(PROTOCOL::CSearchCallerRequest::CBody synCallerData)
{
	DataBase::SQLRequest l_tSQLReqInsertCaller;
	l_tSQLReqInsertCaller.sql_id = "insert_icc_t_involved_party";
	l_tSQLReqInsertCaller.param["id"] = synCallerData.m_str_id;
	l_tSQLReqInsertCaller.param["alarm_id"] = synCallerData.m_str_alarm_id;
	l_tSQLReqInsertCaller.param["process_id"] = synCallerData.m_str_process_id;
	l_tSQLReqInsertCaller.param["feedback_id"] = synCallerData.m_str_feedback_id;
	l_tSQLReqInsertCaller.param["centre_process_dept_code"] = synCallerData.m_str_centre_process_dept_code;
	l_tSQLReqInsertCaller.param["centre_alarm_dept_code"] = synCallerData.m_str_centre_alarm_dept_code;
	l_tSQLReqInsertCaller.param["centre_process_id"] = synCallerData.m_str_centre_process_id;
	l_tSQLReqInsertCaller.param["centre_feedback_id"] = synCallerData.m_str_centre_feedback_id;
	l_tSQLReqInsertCaller.param["district"] = synCallerData.m_str_district;
	l_tSQLReqInsertCaller.param["district_name"] = synCallerData.m_str_district_name;
	l_tSQLReqInsertCaller.param["name"] = synCallerData.m_str_name;
	l_tSQLReqInsertCaller.param["sex"] = synCallerData.m_str_sex;
	l_tSQLReqInsertCaller.param["identification_type"] = synCallerData.m_str_identification_type;
	l_tSQLReqInsertCaller.param["identification_id"] = synCallerData.m_str_identification_id;
	l_tSQLReqInsertCaller.param["identity"] = synCallerData.m_str_identity;
	l_tSQLReqInsertCaller.param["is_focus_pesron"] = synCallerData.m_str_is_focus_pesron;
	l_tSQLReqInsertCaller.param["focus_pesron_info"] = synCallerData.m_str_focus_pesron_info;
	l_tSQLReqInsertCaller.param["registered_address_division"] = synCallerData.m_str_registered_address_division;
	l_tSQLReqInsertCaller.param["registered_address"] = synCallerData.m_str_registered_address;
	l_tSQLReqInsertCaller.param["current_address_division"] = synCallerData.m_str_current_address_division;
	l_tSQLReqInsertCaller.param["current_address"] = synCallerData.m_str_current_address;
	l_tSQLReqInsertCaller.param["work_unit"] = synCallerData.m_str_work_unit;
	l_tSQLReqInsertCaller.param["occupation"] = synCallerData.m_str_occupation;
	l_tSQLReqInsertCaller.param["contact_number"] = synCallerData.m_str_contact_number;
	l_tSQLReqInsertCaller.param["other_certificates"] = synCallerData.m_str_other_certificates;
	l_tSQLReqInsertCaller.param["lost_item_information"] = synCallerData.m_str_lost_item_information;
	l_tSQLReqInsertCaller.param["create_time"] = synCallerData.m_str_create_time;
	l_tSQLReqInsertCaller.param["update_time"] = synCallerData.m_str_update_time;
	l_tSQLReqInsertCaller.param["date_of_birth"] = synCallerData.m_str_date_of_birth;
	l_tSQLReqInsertCaller.param["digital_signature"] = synCallerData.m_str_digital_signature;
	l_tSQLReqInsertCaller.param["is_delete"] = synCallerData.m_str_is_delete;
	l_tSQLReqInsertCaller.param["createTeminal"] = synCallerData.m_str_createTeminal;
	l_tSQLReqInsertCaller.param["updateTeminal"] = synCallerData.m_str_updateTeminal;
	l_tSQLReqInsertCaller.param["nationality"] = synCallerData.m_str_nationality;
	l_tSQLReqInsertCaller.param["nation"] = synCallerData.m_str_nation;
	l_tSQLReqInsertCaller.param["educationLevel"] = synCallerData.m_str_educationLevel;
	l_tSQLReqInsertCaller.param["marriageStatus"] = synCallerData.m_str_marriageStatus;
	l_tSQLReqInsertCaller.param["politicalOutlook"] = synCallerData.m_str_politicalOutlook;
	l_tSQLReqInsertCaller.param["is_NPC_deputy"] = synCallerData.m_str_is_NPC_deputy;
	l_tSQLReqInsertCaller.param["is_national_staff"] = synCallerData.m_str_is_national_staff;
	l_tSQLReqInsertCaller.param["createUserId"] = synCallerData.m_str_createUserId;
	l_tSQLReqInsertCaller.param["createUserName"] = synCallerData.m_str_createUserName;
	l_tSQLReqInsertCaller.param["createUserOrgCode"] = synCallerData.m_str_createUserOrgCode;
	l_tSQLReqInsertCaller.param["createUserOrgName"] = synCallerData.m_str_createUserOrgName;
	l_tSQLReqInsertCaller.param["updateUserId"] = synCallerData.m_str_updateUserId;
	l_tSQLReqInsertCaller.param["updateUserName"] = synCallerData.m_str_updateUserName;
	l_tSQLReqInsertCaller.param["updateUserOrgCode"] = synCallerData.m_str_updateUserOrgCode;
	l_tSQLReqInsertCaller.param["updateUserOrgName"] = synCallerData.m_str_updateUserOrgName;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertCaller);
	// ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "RecvCallerData insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "RecvCallerData,%s Exec sql success! sql:[%s]", l_tSQLReqInsertCaller.sql_id.c_str(), l_pRSet->GetSQL().c_str());
	}
	return true;
}
// 查询 涉案人员信息
bool CBusinessImpl::SearchCallerData(const std::string l_strCallerID)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_involved_party";
	l_oSeleteAlarmSQLReq.param["id"] = l_strCallerID;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		return true;
	}
	return false;
}


//涉案人员信息查询响应函数
void CBusinessImpl::OnNotifiSearchCallerRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchCallerRequest l_oSearchCallerRequest;
	if (!l_oSearchCallerRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}
	int l_iPageSize = m_pString->ToInt(l_oSearchCallerRequest.m_oBody.m_strPageSize);
	int l_iPageIndex = m_pString->ToInt(l_oSearchCallerRequest.m_oBody.m_strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	l_iPageSize = std::min(l_iPageSize, m_iMaxPageSize);

	if (l_iPageSize <= 0)
	{//检查客户端传入的page_size参数
		l_iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oSearchCallerRequest.m_oBody.m_strPageSize.c_str(), l_iPageSize);
	}
	if (l_iPageIndex < 1)
	{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		l_iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_oSearchCallerRequest.m_oBody.m_strPageSize.c_str(), l_iPageIndex);
	}

	//根据请求参数构造查询条件	
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "select_icc_t_involved_party";

	if (!l_oSearchCallerRequest.m_oBody.m_strBeginTime.empty())
	{
		tmp_oSQLRequst.param["jjsj_begin"] = l_oSearchCallerRequest.m_oBody.m_strBeginTime;
	}
	if (!l_oSearchCallerRequest.m_oBody.m_strEndTime.empty())
	{
		tmp_oSQLRequst.param["jjsj_end"] = l_oSearchCallerRequest.m_oBody.m_strEndTime;
	}
	
	if (!l_oSearchCallerRequest.m_oBody.m_str_id.empty())
	{
		tmp_oSQLRequst.param["id"] = l_oSearchCallerRequest.m_oBody.m_str_id;
	}

	if (!l_oSearchCallerRequest.m_oBody.m_str_alarm_id.empty())
	{
		tmp_oSQLRequst.param["alarm_id"] = l_oSearchCallerRequest.m_oBody.m_str_alarm_id;
	}
	if (!l_oSearchCallerRequest.m_oBody.m_str_process_id.empty())
	{
		tmp_oSQLRequst.param["process_id"] = l_oSearchCallerRequest.m_oBody.m_str_process_id;
	}
	if (!l_oSearchCallerRequest.m_oBody.m_str_identification_id.empty())
	{
		tmp_oSQLRequst.param["identification_id"] = l_oSearchCallerRequest.m_oBody.m_str_identification_id;
	}
	if (!l_oSearchCallerRequest.m_oBody.m_str_contact_number.empty())
	{
		tmp_oSQLRequst.param["contact_number"] = l_oSearchCallerRequest.m_oBody.m_str_contact_number;
	}
	if (!l_oSearchCallerRequest.m_oBody.m_str_district_name.empty())
	{
		tmp_oSQLRequst.param["district_name"] = l_oSearchCallerRequest.m_oBody.m_str_district_name;
	}

	if (!l_oSearchCallerRequest.m_oBody.m_str_registered_address_division.empty())
	{
		tmp_oSQLRequst.param["registered_address_division"] = l_oSearchCallerRequest.m_oBody.m_str_registered_address_division;
	}
	PROTOCOL::CVcsAlarmSynDataSar l_oRespond;
	GenRespondHeaderEx("search_jqdsr_request", l_oSearchCallerRequest.m_oHeader, l_oRespond.m_oHeader);
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(tmp_oSQLRequst, true);
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_strCode = "0";
		l_oRespond.m_strMessage = "query alarm exec sql failed!!!";
		l_oRespond.m_oHeader.m_strResult = "query alarm exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
	}
	l_oRespond.m_strAllCount = std::to_string(l_pRSet->RecordSize());
	
	l_oRespond.m_strCode = "200";
	l_oRespond.m_strMessage = "success";
	tmp_oSQLRequst.param["limit"] = std::to_string(l_iPageSize);
	tmp_oSQLRequst.param["offset"] = std::to_string(l_iRequestIdxBegin);
	l_oRespond.m_strPageIndex = l_oSearchCallerRequest.m_oBody.m_strPageIndex;
	l_pRSet = m_pDBConn->Exec(tmp_oSQLRequst, true);
	ICC_LOG_INFO(m_pLog, "query sql:[%s],RecordSize:[%d]", l_pRSet->GetSQL().c_str(), l_pRSet->RecordSize());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_strCode = "0";
		l_oRespond.m_strMessage = "query alarm exec sql failed!!!";
		l_oRespond.m_oHeader.m_strResult = "query alarm exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
	}
	else if (l_pRSet->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_pRSet->RecordSize();
		l_oRespond.m_strCount = std::to_string(l_iRecordSize); //结果集警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm searched,begin to build respond msg,[%s] alarm in total,this msg contains [%d] alarm(s)",
			l_oRespond.m_strCount.c_str(), l_pRSet->RecordSize());

		while (l_pRSet->Next())
		{
			PROTOCOL::CVcsAlarmSynDataSar::CData l_oData;
			l_oData.m_str_id = l_pRSet->GetValue("id");
			l_oData.m_str_alarm_id = l_pRSet->GetValue("alarm_id");
			l_oData.m_str_process_id = l_pRSet->GetValue("process_id");
			l_oData.m_str_feedback_id = l_pRSet->GetValue("feedback_id");
			l_oData.m_str_centre_process_dept_code = l_pRSet->GetValue("centre_process_dept_code");
			l_oData.m_str_centre_alarm_dept_code = l_pRSet->GetValue("centre_alarm_dept_code");
			l_oData.m_str_centre_process_id = l_pRSet->GetValue("centre_process_id");
			l_oData.m_str_centre_feedback_id = l_pRSet->GetValue("centre_feedback_id");
			l_oData.m_str_district = l_pRSet->GetValue("district");
			l_oData.m_str_district_name = l_pRSet->GetValue("district_name");
			l_oData.m_str_name = l_pRSet->GetValue("name");
			l_oData.m_str_sex = l_pRSet->GetValue("sex");
			l_oData.m_str_identification_type = l_pRSet->GetValue("identification_type");
			l_oData.m_str_identification_id = l_pRSet->GetValue("identification_id");
			l_oData.m_str_identity = l_pRSet->GetValue("identity");
			l_oData.m_str_is_focus_pesron = l_pRSet->GetValue("is_focus_pesron");
			l_oData.m_str_focus_pesron_info = l_pRSet->GetValue("focus_pesron_info");
			l_oData.m_str_registered_address_division = l_pRSet->GetValue("registered_address_division");
			l_oData.m_str_registered_address = l_pRSet->GetValue("registered_address");
			l_oData.m_str_current_address_division = l_pRSet->GetValue("current_address_division");
			l_oData.m_str_current_address = l_pRSet->GetValue("current_address");
			l_oData.m_str_work_unit = l_pRSet->GetValue("work_unit");
			l_oData.m_str_occupation = l_pRSet->GetValue("occupation");
			l_oData.m_str_contact_number = l_pRSet->GetValue("contact_number");
			l_oData.m_str_other_certificates = l_pRSet->GetValue("other_certificates");
			l_oData.m_str_lost_item_information = l_pRSet->GetValue("lost_item_information");
			l_oData.m_str_create_time = l_pRSet->GetValue("create_time");
			l_oData.m_str_update_time = l_pRSet->GetValue("update_time");
			l_oData.m_str_date_of_birth = l_pRSet->GetValue("date_of_birth");
			l_oData.m_str_digital_signature = l_pRSet->GetValue("digital_signature");
			l_oData.m_str_is_delete = l_pRSet->GetValue("is_delete");
			l_oData.m_str_createTeminal = l_pRSet->GetValue("createteminal");
			l_oData.m_str_updateTeminal = l_pRSet->GetValue("updateteminal");
			l_oData.m_str_nationality = l_pRSet->GetValue("nationality");
			l_oData.m_str_nation = l_pRSet->GetValue("nation");
			l_oData.m_str_educationLevel = l_pRSet->GetValue("educationlevel");
			l_oData.m_str_marriageStatus = l_pRSet->GetValue("marriagestatus");
			l_oData.m_str_politicalOutlook = l_pRSet->GetValue("politicaloutlook");
			l_oData.m_str_is_NPC_deputy = l_pRSet->GetValue("is_npc_deputy");
			l_oData.m_str_is_national_staff = l_pRSet->GetValue("is_national_staff");
			l_oData.m_str_createUserId = l_pRSet->GetValue("createuserid");
			l_oData.m_str_createUserName = l_pRSet->GetValue("createusername");
			l_oData.m_str_createUserOrgCode = l_pRSet->GetValue("createuserorgcode");
			l_oData.m_str_createUserOrgName = l_pRSet->GetValue("createuserorgname");
			l_oData.m_str_updateUserId = l_pRSet->GetValue("updateuserid");
			l_oData.m_str_updateUserName = l_pRSet->GetValue("updateusername");
			l_oData.m_str_updateUserOrgCode = l_pRSet->GetValue("updateuserorgcode");
			l_oData.m_str_updateUserOrgName = l_pRSet->GetValue("updateuserorgname");
			l_oRespond.m_vecData.push_back(l_oData);
			// ICC_LOG_ERROR(m_pLog, "query alarm  msg id [%s]", l_oData.m_str_id.c_str());
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "no alarm respond to client, alarm count in query result=[%d],request pagesize=[%d],request pageindex=[%d],request index[%d-%d]",
			l_pRSet->RecordSize(), l_iPageSize, l_iPageIndex, l_iRequestIdxBegin + 1, l_iRequestIdxEnd);
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response,count;[%s], msg:[%s]", l_oRespond.m_strCount.c_str(), l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);
}


//警情查询
void CBusinessImpl::OnNotifiSearchAlarmRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CSearchAlarmRequest l_oSearchAlarmRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchAlarmRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	std::string l_strCondition;
	PROTOCOL::CSearchAlarmRequest::CBody& l_oRequestBody = l_oSearchAlarmRequest.m_oBody;
	//BuildQueryCondition(l_oRequestBody, l_strCondition);

	int l_iPageSize = m_pString->ToInt(l_oRequestBody.m_strPageSize);
	int l_iPageIndex = m_pString->ToInt(l_oRequestBody.m_strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	l_iPageSize = std::min(l_iPageSize, m_iMaxPageSize);

	if (l_iPageSize <= 0)
	{//检查客户端传入的page_size参数
		l_iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oRequestBody.m_strPageSize.c_str(), l_iPageSize);
	}
	if (l_iPageIndex < 1)
	{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		l_iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_oRequestBody.m_strPageSize.c_str(), l_iPageIndex);
	}

	//根据请求参数构造查询条件	
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始
	//std::string l_strLimitOffset = str(boost::format(" limit %1% offset %2%") % l_iPageSize %l_iRequestIdxBegin);

	//查询警情表所有字段，并返回查询结果总数
	//分两次查询，一次查count，一次查实际内容，在两个线程中同时查询
	//std::string l_strQueryAlarmSQL("SELECT alarm.*,callevent.acd FROM public.icc_t_alarm alarm left join icc_t_callevent as callevent on alarm.source_id=callevent.callref_id where " + l_strCondition);
	//std::string l_strQueryCountSQL("SELECT count_estimate('" + m_pString->ReplaceAll(l_strQueryAlarmSQL, "'", "''") + "') as count;");
	//std::string l_strQueryCountSQL("SELECT count(1) FROM public.icc_t_alarm alarm where " + l_strCondition);
	//l_strQueryAlarmSQL.append(" order by alarm.time desc").append(l_strLimitOffset);
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "seach_alarm_count";

	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_oRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strBeginTime.empty())
	{
		l_strBeginTime = m_pDateTime->ToString(m_pDateTime->AddDays(m_pDateTime->CurrentDateTime(), -1));//如果时间为空，默认只查一天 [10/24/2019 151116314]
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}

	//以alarm.id代替时间，可以利用索引，并减少数据库排序和计数时间，提高查询效率
	//std::string l_strConditionTime("alarm.time between '" + l_strBeginTime + "' and '" + l_strEndTime + "'");
	//l_strCondResult.append("(").append(l_strConditionTime).append(")");
	if (l_oRequestBody.m_strVcsSyncFlag == "1")
	{
		tmp_oSQLRequst.param["gxbegin_time"] = l_strBeginTime;
		tmp_oSQLRequst.param["gxend_time"] = l_strEndTime;

		tmp_oSQLRequst.param["jjsj_end"] = l_strEndTime;
	}
	else
	{
		tmp_oSQLRequst.param["jjsj_begin"] = l_strBeginTime;
		tmp_oSQLRequst.param["jjsj_end"] = l_strEndTime;
	}


	std::string l_strAlarmID = m_pString->Trim(l_oRequestBody.m_strID);
	if (!l_strAlarmID.empty())
	{
		tmp_oSQLRequst.param["id"] = l_strAlarmID;
		//l_strCondResult.append("and ").append("(alarm.id like '%" + l_strAlarmID + "%')");
	}

	//{
		//状态是 受理中的（未提交的）警情不显示----应兵爷要求注释放开查询
		//std::string l_strCondAlarmContent("alarm.state <> '00'"); //DIC019010
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmContent).append(")");
	//}

	if (!l_oRequestBody.m_strTitle.empty())
	{
		tmp_oSQLRequst.param["label"] = l_oRequestBody.m_strTitle;
		//std::string l_strCondTitle = "alarm.title like '%" + l_oRequestBody.m_strTitle + "%'";
		//l_strCondResult.append(" and ").append("(").append(l_strCondTitle).append(")");
	}

	if (!l_oRequestBody.m_strContent.empty())
	{	//警情描述，模糊匹配
		tmp_oSQLRequst.param["content"] = l_oRequestBody.m_strContent;
		//std::string l_strCondAlarmContent("alarm.content like '%" + l_oRequestBody.m_strContent + "%'");
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmContent).append(")");
	}

	if (!l_oRequestBody.m_strAddr.empty())
	{	//警情详细发生地址，模糊匹配
		//std::string l_strCondAlarmAddr("alarm.addr like '%" + l_oRequestBody.m_strAddr + "%'");
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmAddr).append(")");
		tmp_oSQLRequst.param["addr"] = l_oRequestBody.m_strAddr;
	}

	//if (!l_oRequestBody.m_strCityCode.empty())
	//{	//警情详细发生地址，模糊匹配
	//	std::string l_strCondAlarmCityCode("alarm.city_code like '%" + l_oRequestBody.m_strCityCode + "%'");
	//	l_strCondResult.append(" and ").append("(").append(l_strCondAlarmCityCode).append(")");
	//}

	if (!l_oRequestBody.m_strState.empty())
	{	//警情状态：未处置、未反馈、未结案、已结案
		//std::string l_strCondAlarmState(BuildMultiConditions("alarm.state", "=", l_oRequestBody.m_strState));
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmState).append(")");

		//支持多值查询 
		//tmp_oSQLRequst.param["state"] = l_oRequestBody.m_strState;

		tmp_oSQLRequst.param["state"] = BuildMultiConditions("alarm.jqclztdm", "=", l_oRequestBody.m_strState);
	}

	if (!l_oRequestBody.m_strLevel.empty())
	{	//警情级别：一级、二级、三级
		//std::string l_strCondAlarmLevel(BuildMultiConditions("alarm.level", "=",
		//	l_oRequestBody.m_strLevel));
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmLevel).append(")");
		tmp_oSQLRequst.param["level"] = BuildMultiConditions("alarm.jqdjdm", "=", l_oRequestBody.m_strLevel);
	}

	if (!l_oRequestBody.m_strSourceType.empty())
	{	//警情报警来源类型：110，119，122，短信，微信
		//std::string l_strCondSourceType(BuildMultiConditions("alarm.source_type", "=",
		//	l_oRequestBody.m_strSourceType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondSourceType).append(")");
		tmp_oSQLRequst.param["source_type"] = l_oRequestBody.m_strSourceType;
	}

	if (!l_oRequestBody.m_strHandleType.empty())
	{
		//警情处理类型（下拉选择）
		//std::string l_strCondHandleType(BuildMultiConditions("alarm.handle_type", "=",
		//	l_oRequestBody.m_strHandleType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondHandleType).append(")");
		tmp_oSQLRequst.param["handle_type"] = l_oRequestBody.m_strHandleType;
	}

	//支持客户端只显示有效警情
	if (l_oRequestBody.m_strIsVerity == "1")
	{
		//有效警情
		//无效警情编码==DIC003011
		//std::string l_strCondIsVerity("alarm.handle_type = '01'");// and alarm.first_type != 'DIC003011'");
		//l_strCondResult.append(" and ").append("(").append(l_strCondIsVerity).append(")");
		tmp_oSQLRequst.param["handle_type_valid"] = "01";
	}
	else if (l_oRequestBody.m_strIsVerity == "0")
	{
		//无效警情
		//std::string l_strCondIsVerity("alarm.handle_type != '01'");// or alarm.first_type = 'DIC003011'");
		//l_strCondResult.append(" and ").append("(").append(l_strCondIsVerity).append(")");
		tmp_oSQLRequst.param["handle_type_invalid"] = "01";
	}
	else
	{
		//所有
	}

	/*if (!l_oRequestBody.m_strFirstType.empty())
	{	//警情一级类型（下拉选择）
		//std::string l_strCondFirstType(BuildMultiConditions("alarm.first_type", "=",
		//	l_oRequestBody.m_strFirstType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondFirstType).append(")");
		tmp_oSQLRequst.param["first_type"] = l_oRequestBody.m_strFirstType;
	}

	if (!l_oRequestBody.m_strSecondType.empty())
	{	//警情二级类型（下拉选择）
		//std::string l_strCondSecondType(BuildMultiConditions("alarm.second_type", "=",
			//l_oRequestBody.m_strSecondType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondSecondType).append(")");
		tmp_oSQLRequst.param["second_type"] = l_oRequestBody.m_strSecondType;
	}

	if (!l_oRequestBody.m_strThirdType.empty())
	{	//警情三级类型（下拉选择）
		//std::string l_strCondThirdType(BuildMultiConditions("alarm.third_type", "=",
		//	l_oRequestBody.m_strThirdType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondThirdType).append(")");
		tmp_oSQLRequst.param["third_type"] = l_oRequestBody.m_strThirdType;
	}
	if (!l_oRequestBody.m_strFourthType.empty())
	{	//警情四级类型（下拉选择）
		//std::string l_strCondFourthType(BuildMultiConditions("alarm.fourth_type", "=",
		//	l_oRequestBody.m_strFourthType));
		//l_strCondResult.append(" and ").append("(").append(l_strCondFourthType).append(")");
		tmp_oSQLRequst.param["fourth_type"] = l_oRequestBody.m_strFourthType;
	}
	*/

	//修改警情类型查询为多个查询，前端传多个警情类型，以",;"分割
	if (!l_oRequestBody.m_strAlarmType.empty())
	{
		tmp_oSQLRequst.param["alarm_type"] = BuildTypeConditions(l_oRequestBody.m_strAlarmType);
	}


	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		//l_strCondResult.append(m_pString->Format(" and alarm.receipt_seatno = '%s' ", l_oRequestBody.m_strSeatNo.c_str()));
		tmp_oSQLRequst.param["receipt_seatno"] = l_oRequestBody.m_strSeatNo;
	}

	//if (!l_oRequestBody.m_strEventType.empty())
	//{	//事件类型（编码以逗号分隔）
	//	std::string l_strCondTmp(BuildMultiConditions("alarm.event_type", "=",
	//		l_oRequestBody.m_strEventType));
	//	l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	//}

	//警情报警号码字典类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strCalledNoType.empty())
	{
		/*std::string l_strCondCalledNoType(BuildMultiConditions("alarm.called_no_type", "=",
			l_oRequestBody.m_strCalledNoType));
		l_strCondResult.append(" and ").append("(").append(l_strCondCalledNoType).append(")");*/

		//tmp_oSQLRequst.param["called_no_type"] = l_oRequestBody.m_strCalledNoType;
		tmp_oSQLRequst.param["called_no_type"] = BuildMultiConditions("alarm.jjlx", "=", l_oRequestBody.m_strCalledNoType);

	}

	//警情报警人号码（模糊匹配）
	std::string l_strCallerNumber = m_pString->Trim(l_oRequestBody.m_strCallerNo);
	if (!l_strCallerNumber.empty())
	{
		//std::string l_strCondCallerNo(str(boost::format("alarm.caller_no like '%%%1%%%'")
		//	% l_strCallerNumber));
		//l_strCondResult.append(" and ").append("(").append(l_strCondCallerNo).append(")");

		tmp_oSQLRequst.param["caller_no"] = l_strCallerNumber;
	}

	//警情报警人姓名（模糊匹配）
	if (!l_oRequestBody.m_strCallerName.empty())
	{
		/*std::string l_strCondCallerName("alarm.caller_name like '%"
			+ l_oRequestBody.m_strCallerName + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondCallerName).append(")");*/
		tmp_oSQLRequst.param["caller_name"] = l_oRequestBody.m_strCallerName;
	}

	//警情联系人号码（模糊查询）
	if (!l_oRequestBody.m_strContactNo.empty())
	{
		/*std::string l_strCondTmp("alarm.contact_no like '%"
			+ l_oRequestBody.m_strContactNo + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
		tmp_oSQLRequst.param["contact_no"] = l_oRequestBody.m_strContactNo;
	}

	//警情联系人姓名（模糊查询）
	/*if (!l_oRequestBody.m_strContactName.empty())
	{
		std::string l_strCondTmp("alarm.contact_name like '%"
			+ l_oRequestBody.m_strCallerName + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");

	}*/

	//警情管辖单位编码（多值查询，逗号分隔）[admin_dept_code]
	if (!l_oRequestBody.m_strAdminDeptCode.empty())
	{
		if (l_oRequestBody.m_strAdminDeptCodeRecursion == "0")
		{//不递归
			/*std::string l_strCondTmp(BuildMultiConditions("alarm.admin_dept_code", "=",
				l_oRequestBody.m_strAdminDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
			//tmp_oSQLRequst.param["admin_dept_code"] = l_oRequestBody.m_strAdminDeptCode;
			tmp_oSQLRequst.param["admin_dept_code"] = BuildMultiConditionsEx("alarm.gxdwdm", "like", l_oRequestBody.m_strAdminDeptCode);

		}
		else
		{//递归查询所有下级单位单位编码
			/*std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strAdminDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("alarm.admin_dept_code = any(%1%)") % l_strQueryChildDept));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
			//tmp_oSQLRequst.param["recursive_admin_dept_code"] = l_oRequestBody.m_strAdminDeptCode;
			tmp_oSQLRequst.param["recursive_admin_dept_code"] = l_oRequestBody.m_strAdminDeptCode;
			std::string strCondition = "and (alarm.gxdwdm =any(select cdept.code from icc_view_dept_recursive cdept where ";
			std::string strBuildCondition = BuildMultiConditionsEx("cdept.path", "like", l_oRequestBody.m_strAdminDeptCode);
			std::string l_strCondResult;
			l_strCondResult = strCondition.append(strBuildCondition).append("))");
			tmp_oSQLRequst.param["recursive_admin_dept_code"] = l_strCondResult;
		}
	}

	//警情接警单位编码（递归查询）[receipt_dept_code]
	if (!l_oRequestBody.m_strReceiptDeptCode.empty())
	{
		if (l_oRequestBody.m_strReceiptDeptCodeRecursion == "0")
		{//不递归查询
			/*std::string l_strCondTmp(BuildMultiConditions("alarm.receipt_dept_code", "=",
				l_oRequestBody.m_strReceiptDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
			tmp_oSQLRequst.param["receipt_dept_code"] = l_oRequestBody.m_strReceiptDeptCode;
		}
		else
		{//递归查询下级单位
			/*std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strReceiptDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("alarm.receipt_dept_code = any(%1%)") % l_strQueryChildDept));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
			tmp_oSQLRequst.param["recursive_receipt_dept_code"] = l_oRequestBody.m_strReceiptDeptCode;
		}
	}

	//警情值班领导警号（多值查询，逗号分隔）
	/*if (!l_oRequestBody.m_strLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.leader_code", "=",
			l_oRequestBody.m_strLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}*/

	//接警员（多值查询，下拉选择）
	if (!l_oRequestBody.m_strReceiptCode.empty())
	{
		/*std::string l_strCondTmp(BuildMultiConditions("alarm.receipt_code", "=",
			l_oRequestBody.m_strReceiptCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");*/
		tmp_oSQLRequst.param["receipt_code"] = l_oRequestBody.m_strReceiptCode;
	}

	if (l_oRequestBody.m_strTelHotLine == "1")
	{
		tmp_oSQLRequst.param["is_tel_hotline"] = "right join public.icc_t_linked_dispatch linked on alarm.jjdbh = linked.alarm_id";
	}

	//当前用户部门编码，限制用户只可查询自己部门及下属部门信息
	if (!l_oRequestBody.m_strCurUserDeptCode.empty())
	{
		std::string l_strCurUserDept("cdept.path like '%" + l_oRequestBody.m_strCurUserDeptCode + "%'");
		std::string l_strCondCurUserDept("select cdept.code from icc_view_dept_recursive cdept where ");
		l_strCondCurUserDept.append(l_strCurUserDept);

		std::string l_strChildDept;
		DataBase::IResultSetPtr l_pRSetChildDept = m_pDBConn->Exec(l_strCondCurUserDept);
		while (l_pRSetChildDept->Next())
		{
			l_strChildDept += l_strChildDept.empty() ? l_pRSetChildDept->GetValue("code") : "," + l_pRSetChildDept->GetValue("code");
		}

		std::string l_strCondTmp(BuildMultiConditions("alarm.jjdwdm", "=", l_strChildDept));
		//l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");

		tmp_oSQLRequst.param["receipt_dept_code_condition"] = l_strChildDept;
	}

	std::string l_strFeedBackCondition;
	//反馈警情类型
	if (!l_oRequestBody.m_strFeedbackFirstType.empty())
	{	//反馈警情一级类型（下拉选择）
		std::string l_strCondFirstType(BuildMultiConditions("feedback.alarm_first_type", "=",
			l_oRequestBody.m_strFeedbackFirstType));
		l_strFeedBackCondition.append("(").append(l_strCondFirstType).append(")");
	}

	if (!l_oRequestBody.m_strFeedbackSecondType.empty())
	{	//反馈警情二级类型（下拉选择）
		std::string l_strCondSecondType(BuildMultiConditions("feedback.alarm_second_type", "=",
			l_oRequestBody.m_strFeedbackSecondType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondSecondType).append(")");
	}

	if (!l_oRequestBody.m_strFeedbackThirdType.empty())
	{
		// 反馈警情三级类型（下拉选择）
		std::string l_strCondThirdType(BuildMultiConditions("feedback.alarm_third_type", "=",
			l_oRequestBody.m_strFeedbackThirdType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondThirdType).append(")");
	}
	if (!l_oRequestBody.m_strFeedbackFourthType.empty())
	{	//反馈警情四级类型（下拉选择）
		std::string l_strCondFourthType(BuildMultiConditions("feedback.alarm_fourth_type", "=",
			l_oRequestBody.m_strFeedbackFourthType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondFourthType).append(")");
	}

	if (!l_strFeedBackCondition.empty())
	{
		tmp_oSQLRequst.param["feedback_condition"] = l_strFeedBackCondition;
		//l_strCondResult.append(" and ").append("alarm.id = any(SELECT alarm_id FROM icc_t_alarm_feedback feedback WHERE " + l_strFeedBackCondition + ")");
	}

	if (!l_oRequestBody.m_strIsFeedBack.empty()/* && l_oRequestBody.m_strIsFeedBack == "1"*/)
	{
		tmp_oSQLRequst.param["is_feedback"] = l_oRequestBody.m_strIsFeedBack;
		//l_strCondResult.append(" and ").append("(alarm.is_feedback = '" + l_oRequestBody.m_strIsFeedBack + "')");
	}

	if (!l_oRequestBody.m_strIsVisitor.empty())
	{
		//l_strCondResult.append(" and ").append("(alarm.is_visitor = '" + l_oRequestBody.m_strIsVisitor + "')");
		tmp_oSQLRequst.param["is_visitor"] = l_oRequestBody.m_strIsVisitor;
	}

	if (!l_oRequestBody.m_strIsClosure.empty())
	{
		tmp_oSQLRequst.param["is_over"] = l_oRequestBody.m_strIsClosure;
	}

	/** change by tt 2022-4-26
	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		tmp_oSQLRequst.param["is_visitor"] = l_oRequestBody.m_strSeatNo;
	}
	***/
	//只做报警人姓名，报警人电话号码，报警人证件号精确匹配
	if (!l_oRequestBody.m_strCallerKey.empty())
	{
		tmp_oSQLRequst.param["caller_key"] = l_oRequestBody.m_strCallerKey;
	}

	if (!l_oRequestBody.m_strIsInvalid.empty())
	{
		//is_invalid
		tmp_oSQLRequst.param["is_invalid"] = l_oRequestBody.m_strIsInvalid;
	}

	if (!l_oRequestBody.m_strQueryKey.empty())
	{
		//is_invalid
		tmp_oSQLRequst.param["query_key"] = l_oRequestBody.m_strQueryKey;
	}

	if (l_oRequestBody.m_strMajorAlarmFlag == "1")
	{
		std::vector<std::string> vecTypeNames;
		_QueryMajorAlarmTypeNames(vecTypeNames);
		for (int i = 0; i < vecTypeNames.size(); ++i)
		{
			tmp_oSQLRequst.param[vecTypeNames[i]] = vecTypeNames[i];
		}
	}

	if (!l_oRequestBody.m_strReceiverCode.empty())
	{
		//is_invalid
		tmp_oSQLRequst.param["receiver_code_condition"] = m_pString->Format("alarm.jjdbh in (select alarm_id from icc_t_shift_relation as a left join icc_t_shift as b on a.shift_id=b.guid left join icc_t_jjdb as c on c.jjdbh = a.alarm_id where c.jjsj >= '%s' and c.jjsj <= '%s' and receiver_code = '%s')",
			l_oRequestBody.m_strBeginTime.c_str(), l_oRequestBody.m_strEndTime.c_str(), l_oRequestBody.m_strReceiverCode.c_str());
	}

	if (!l_oRequestBody.m_strCodeWhenQueryAll.empty())
	{
		//is_invalid
		tmp_oSQLRequst.param["receiver_code_condition"] = m_pString->Format("alarm.jjybh = '%s' or alarm.jjdbh in (select alarm_id from icc_t_shift_relation as a left join icc_t_shift as b on a.shift_id=b.guid left join icc_t_jjdb as c on c.jjdbh = a.alarm_id where c.jjsj >= '%s' and c.jjsj <= '%s' and receiver_code = '%s')",
			l_oRequestBody.m_strCodeWhenQueryAll.c_str(), l_oRequestBody.m_strBeginTime.c_str(), l_oRequestBody.m_strEndTime.c_str(), l_oRequestBody.m_strCodeWhenQueryAll.c_str());
	}

	std::string str12345condition = "";
	if (l_oRequestBody.m_str110Transfer12345 == "1") 
	{
		str12345condition = m_pString->Format("exists (select 1 from icc_t_linked_dispatch as lnk where lnk.linked_org_type = '01' and lnk.alarm_id = alarm.jjdbh)");
	}
	if (l_oRequestBody.m_str110Transfer12345 == "1" && l_oRequestBody.m_str12345Transfer110 == "1") 
	{
		str12345condition += " or ";
	}

	if (l_oRequestBody.m_str12345Transfer110 == "1")
	{
		str12345condition += "alarm.jjlylx = '03'";
	}
	if (!str12345condition.empty()) 
	{
		tmp_oSQLRequst.param["12345To110_condition"] = str12345condition;
	}

	if (!l_oRequestBody.m_strIsSigned.empty())
	{
		tmp_oSQLRequst.param["is_signed"] =  m_pString->Format(" and ( sfyqs = '%s' and jqclztdm = '02' )", l_oRequestBody.m_strIsSigned.c_str());
	}

	//另用一个线程查询警情数量
	boost::thread l_oThreadQueryAlarmCount(boost::bind(&CBusinessImpl::QueryAlarmCount, this, tmp_oSQLRequst, l_oSearchAlarmRequest.m_oHeader.m_strMsgId));


	PROTOCOL::CSearchAlarmRespond l_oRespond;
	GenRespondHeaderEx("search_alarm_respond", l_oSearchAlarmRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = "0";
	l_oRespond.m_oBody.m_strTotalAlarmCount = "0";

	tmp_oSQLRequst.sql_id = "seach_alarm_data";
	if (l_oRequestBody.m_strVcsSyncFlag == "1")
	{
		tmp_oSQLRequst.param["orderby"] = "alarm.gxsj";
	}
	else
	{
		tmp_oSQLRequst.param["orderby"] = "alarm.jjsj desc";
	}

	tmp_oSQLRequst.param["limit"] = std::to_string(l_iPageSize);
	tmp_oSQLRequst.param["offset"] = std::to_string(l_iRequestIdxBegin);

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(tmp_oSQLRequst, true);
	ICC_LOG_INFO(m_pLog, "query sql:[%s],RecordSize:[%d]", l_pRSet->GetSQL().c_str(), l_pRSet->RecordSize());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "query alarm exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
	}
	else if (l_pRSet->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_pRSet->RecordSize();
		//l_oRespond.m_oBody.m_strCount = l_pRSet->GetValue(0, "count"); //分包总数
		l_oRespond.m_oBody.m_strCount = std::to_string(l_iRecordSize); //结果集警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm searched,begin to build respond msg,[%s] alarm in total,this msg contains [%d] alarm(s)", l_oRespond.m_oBody.m_strCount.c_str(), l_pRSet->RecordSize());

		for (size_t index = 0; index < l_iRecordSize;)
		{
			PROTOCOL::CSearchAlarmRespond::CData l_oAlarmData;
			l_oAlarmData.m_oAlarm.m_strID = l_pRSet->GetValue(index, "id");
			l_oAlarmData.m_oAlarm.m_strMergeID = l_pRSet->GetValue(index, "merge_id");
			l_oAlarmData.m_oAlarm.m_strSeatNo = l_pRSet->GetValue(index, "receipt_seatno");
			l_oAlarmData.m_oAlarm.m_strTitle = l_pRSet->GetValue(index, "label");
			l_oAlarmData.m_oAlarm.m_strContent = l_pRSet->GetValue(index, "content");
			l_oAlarmData.m_oAlarm.m_strTime = l_pRSet->GetValue(index, "receiving_time");
			l_oAlarmData.m_oAlarm.m_strReceivedTime = l_pRSet->GetValue(index, "received_time");
			l_oAlarmData.m_oAlarm.m_strAddr = l_pRSet->GetValue(index, "addr");
			l_oAlarmData.m_oAlarm.m_strLongitude = l_pRSet->GetValue(index, "longitude");
			l_oAlarmData.m_oAlarm.m_strLatitude = l_pRSet->GetValue(index, "latitude");
			l_oAlarmData.m_oAlarm.m_strState = l_pRSet->GetValue(index, "state");
			l_oAlarmData.m_oAlarm.m_strLevel = l_pRSet->GetValue(index, "level");
			l_oAlarmData.m_oAlarm.m_strSourceType = l_pRSet->GetValue(index, "source_type");
			l_oAlarmData.m_oAlarm.m_strSourceID = l_pRSet->GetValue(index, "source_id");
			l_oAlarmData.m_oAlarm.m_strHandleType = l_pRSet->GetValue(index, "handle_type");
			l_oAlarmData.m_oAlarm.m_strFirstType = l_pRSet->GetValue(index, "first_type");
			l_oAlarmData.m_oAlarm.m_strSecondType = l_pRSet->GetValue(index, "second_type");
			l_oAlarmData.m_oAlarm.m_strThirdType = l_pRSet->GetValue(index, "third_type");
			l_oAlarmData.m_oAlarm.m_strFourthType = l_pRSet->GetValue(index, "fourth_type");
			l_oAlarmData.m_oAlarm.m_strVehicleNo = l_pRSet->GetValue(index, "vehicle_no");
			l_oAlarmData.m_oAlarm.m_strVehicleType = l_pRSet->GetValue(index, "vehicle_type");
			l_oAlarmData.m_oAlarm.m_strSymbolCode = l_pRSet->GetValue(index, "symbol_code");
			l_oAlarmData.m_oAlarm.m_strCalledNoType = l_pRSet->GetValue(index, "called_no_type");
			l_oAlarmData.m_oAlarm.m_strCallerNo = l_pRSet->GetValue(index, "caller_no");
			l_oAlarmData.m_oAlarm.m_strCallerName = l_pRSet->GetValue(index, "caller_name");
			l_oAlarmData.m_oAlarm.m_strCallerAddr = l_pRSet->GetValue(index, "caller_addr");
			l_oAlarmData.m_oAlarm.m_strCallerID = l_pRSet->GetValue(index, "caller_id");
			l_oAlarmData.m_oAlarm.m_strCallerIDType = l_pRSet->GetValue(index, "caller_id_type");
			l_oAlarmData.m_oAlarm.m_strCallerGender = l_pRSet->GetValue(index, "caller_gender");
			l_oAlarmData.m_oAlarm.m_strContactNo = l_pRSet->GetValue(index, "contact_no");
			l_oAlarmData.m_oAlarm.m_strAdminDeptCode = l_pRSet->GetValue(index, "admin_dept_code");
			l_oAlarmData.m_oAlarm.m_strAdminDeptName = l_pRSet->GetValue(index, "admin_dept_name");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptDistrictCode = l_pRSet->GetValue(index, "receipt_dept_district_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptCode = l_pRSet->GetValue(index, "receipt_dept_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptName = l_pRSet->GetValue(index, "receipt_dept_name");
			l_oAlarmData.m_oAlarm.m_strReceiptCode = l_pRSet->GetValue(index, "receipt_code");
			l_oAlarmData.m_oAlarm.m_strReceiptName = l_pRSet->GetValue(index, "receipt_name");
			l_oAlarmData.m_oAlarm.m_strCreateTime = l_pRSet->GetValue(index, "create_time");
			l_oAlarmData.m_oAlarm.m_strCreateUser = l_pRSet->GetValue(index, "create_user");
			l_oAlarmData.m_oAlarm.m_strUpdateTime = l_pRSet->GetValue(index, "update_time");
			l_oAlarmData.m_oAlarm.m_strUpdateUser = l_pRSet->GetValue(index, "update_user");
			l_oAlarmData.m_oAlarm.m_strPrivacy = l_pRSet->GetValue(index, "is_privacy");
			l_oAlarmData.m_oAlarm.m_strRemark = l_pRSet->GetValue(index, "remark");
			l_oAlarmData.m_oAlarm.m_strIsVisitor = l_pRSet->GetValue(index, "is_visitor");
			l_oAlarmData.m_oAlarm.m_strIsFeedBack = l_pRSet->GetValue(index, "is_feedback");
			//新增
			l_oAlarmData.m_oAlarm.m_strAlarmAddr = l_pRSet->GetValue(index, "alarm_addr");
			l_oAlarmData.m_oAlarm.m_strCallerUserName = l_pRSet->GetValue(index, "caller_user_name");
			l_oAlarmData.m_oAlarm.m_strErpetratorsNumber = l_pRSet->GetValue(index, "erpetrators_number");

			l_oAlarmData.m_oAlarm.m_strIsArmed = l_pRSet->GetValue(index, "is_armed");

			l_oAlarmData.m_oAlarm.m_strIsHazardousSubstances = l_pRSet->GetValue(index, "is_hazardous_substances");

			l_oAlarmData.m_oAlarm.m_strIsExplosionOrLeakage = l_pRSet->GetValue(index, "is_explosion_or_leakage");

			l_oAlarmData.m_oAlarm.m_strDescOfTrapped = l_pRSet->GetValue(index, "desc_of_trapped");
			l_oAlarmData.m_oAlarm.m_strDescOfInjured = l_pRSet->GetValue(index, "desc_of_injured");
			l_oAlarmData.m_oAlarm.m_strDescOfDead = l_pRSet->GetValue(index, "desc_of_dead");
			l_oAlarmData.m_oAlarm.m_strIsForeignLanguage = l_pRSet->GetValue(index, "is_foreign_language");
			l_oAlarmData.m_oAlarm.m_strManualLongitude = l_pRSet->GetValue(index, "manual_longitude");
			l_oAlarmData.m_oAlarm.m_strManualLatitude = l_pRSet->GetValue(index, "manual_latitude");
			l_oAlarmData.m_oAlarm.m_strEmergencyRescueLevel = l_pRSet->GetValue(index, "emergency_rescue_level");
			l_oAlarmData.m_oAlarm.m_strIsHazardousVehicle = l_pRSet->GetValue(index, "is_hazardous_vehicle");
			l_oAlarmData.m_oAlarm.m_strReceiptSrvName = l_pRSet->GetValue(index, "receipt_srv_name");
			l_oAlarmData.m_oAlarm.m_strAdminDeptOrgCode = l_pRSet->GetValue(index, "admin_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptOrgCode = l_pRSet->GetValue(index, "receipt_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strIsInvalid = l_pRSet->GetValue(index, "is_invalid");
			l_oAlarmData.m_oAlarm.m_strBusinessState = l_pRSet->GetValue(index, "business_status");
			l_oAlarmData.m_oAlarm.m_strFirstSubmitTime = l_pRSet->GetValue(index, "first_submit_time");

			l_oAlarmData.m_oAlarm.m_strIsOver = l_pRSet->GetValue(index, "is_over");
			l_oAlarmData.m_oAlarm.m_strHadPush = l_pRSet->GetValue(index, "had_push");

			l_oAlarmData.m_oAlarm.m_strIsSameForBackground = l_pRSet->GetValue(index, "is_sameforbackground");

			l_oAlarmData.m_oAlarm.m_strIsProcessFlagSynchronized = l_pRSet->GetValue(index, "is_processflagsynchronized");
			l_oAlarmData.m_oAlarm.m_strAlarmSourceType = l_pRSet->GetValue(index, "alarm_source_type");
			l_oAlarmData.m_oAlarm.m_strAlarmSourceId = l_pRSet->GetValue(index, "alarm_source_id");
			l_oAlarmData.m_oAlarm.m_strCentreDeptCode = l_pRSet->GetValue(index, "centre_dept_code");
			l_oAlarmData.m_oAlarm.m_strCentreRelationDeptCode = l_pRSet->GetValue(index, "centre_relation_dept_code");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptDistrictName = l_pRSet->GetValue(index, "receipt_dept_district_name");
			l_oAlarmData.m_oAlarm.m_strAlarmSystemReceiptDeptCode = l_pRSet->GetValue(index, "develop_dept_code");
			l_oAlarmData.m_oAlarm.m_strAlarmSystemReceiptDeptName = l_pRSet->GetValue(index, "develop_dept_name");
			l_oAlarmData.m_oAlarm.m_strLinkedDispatchCode = l_pRSet->GetValue(index, "linked_dispatch_code");
			l_oAlarmData.m_oAlarm.m_strIsSigned = l_pRSet->GetValue(index, "is_signed");
			l_oAlarmData.m_oAlarm.m_strOverTime = l_pRSet->GetValue(index, "over_time");
			l_oAlarmData.m_oAlarm.m_strInitialFirstType = l_pRSet->GetValue(index, "initial_first_type");
			l_oAlarmData.m_oAlarm.m_strInitialSecondType = l_pRSet->GetValue(index, "initial_second_type");
			l_oAlarmData.m_oAlarm.m_strInitialThirdType = l_pRSet->GetValue(index, "initial_third_type");
			l_oAlarmData.m_oAlarm.m_strInitialFourthType = l_pRSet->GetValue(index, "initial_fourth_type");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptCode = l_pRSet->GetValue(index, "initial_admin_dept_code");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptName = l_pRSet->GetValue(index, "initial_admin_dept_name");
			l_oAlarmData.m_oAlarm.m_strInitialAdminDeptOrgCode = l_pRSet->GetValue(index, "initial_admin_dept_org_code");
			l_oAlarmData.m_oAlarm.m_strInitialAddr = l_pRSet->GetValue(index, "initial_addr");
			l_oAlarmData.m_oAlarm.m_strReceiptDeptShortName = l_pRSet->GetValue(index, "receipt_dept_short_name");
			l_oAlarmData.m_oAlarm.m_strAdminDeptShortName = l_pRSet->GetValue(index, "admin_dept_short_name");

			std::string l_strIsMerge(l_pRSet->GetValue(index, "is_merge"));
			l_oAlarmData.m_oAlarm.m_strIsMerge = l_strIsMerge == "1" || m_pString->Lower(l_strIsMerge) == "true" ? "1" : "0";

			l_oRespond.m_oBody.m_vecData.push_back(l_oAlarmData);
			++index;
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "no alarm respond to client, alarm count in query result=[%d],request pagesize=[%d],request pageindex=[%d],request index[%d-%d]",
			l_pRSet->RecordSize(), l_iPageSize, l_iPageIndex, l_iRequestIdxBegin + 1, l_iRequestIdxEnd);
	}

	ICC_LOG_DEBUG(m_pLog, "wait for the returning of alarm_count_thread");
	l_oThreadQueryAlarmCount.join();//等警情数量查询线程返回
	ICC_LOG_DEBUG(m_pLog, "alarm_count_thread returned");
	DataBase::IResultSetPtr l_pAlarmCount;
	l_pAlarmCount = m_mapSelectAlarmCount[l_oSearchAlarmRequest.m_oHeader.m_strMsgId];
	//设置响应消息的警情总数
	if (!l_pAlarmCount || !l_pAlarmCount->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "query alarm count error:[%s]\nsql[%s],", l_pAlarmCount->GetErrorMsg().c_str(), l_pAlarmCount->GetSQL().c_str());
		ICC_LOG_DEBUG(m_pLog, "set alarm count = RecordSize[%d]", l_pRSet->RecordSize());
		l_oRespond.m_oBody.m_strTotalAlarmCount = l_pRSet->RecordSize();
	}
	else
	{
		l_oRespond.m_oBody.m_strTotalAlarmCount = l_pAlarmCount->GetValue(0, "count"); //数据库中满足查询条件的警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm count [%s]", l_oRespond.m_oBody.m_strCount.c_str());
	}

	{
		std::lock_guard<std::mutex> lock(m_mutexSelectAlarmCount);
		m_mapSelectAlarmCount.erase(l_oSearchAlarmRequest.m_oHeader.m_strMsgId);
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response,msgID:[%s],count;[%s], msg:[%s]", l_oSearchAlarmRequest.m_oHeader.m_strMsgId.c_str(), l_oRespond.m_oBody.m_strTotalAlarmCount.c_str(), l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);
}

//处警信息查询，客户端暂未使用，未联调
void CBusinessImpl::OnNotifiSearchAlarmProcessRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "reqeust msg is empty");
		return;
	}

	PROTOCOL::CSearchAlarmProcessRequest l_oSearchProcessRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchProcessRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed");
		return;
	}

	PROTOCOL::CSearchAlarmProcessRequest::CBody& l_rRequestBody = l_oSearchProcessRequest.m_oBody;
	int l_iPageSize = m_pString->ToInt(l_rRequestBody.m_strPageSize);
	int l_iPageIndex = m_pString->ToInt(l_rRequestBody.m_strPageIndex);
	//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
	l_iPageSize = std::min(l_iPageSize, m_iMaxPageSize);
	if (l_iPageSize <= 0)
	{//检查客户端传入的page_size参数
		l_iPageSize = 10;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_rRequestBody.m_strPageSize.c_str(), l_iPageSize);
	}
	if (l_iPageIndex < 1)
	{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
		l_iPageIndex = 1;
		ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_rRequestBody.m_strPageSize.c_str(), l_iPageIndex);
	}
	//根据请求参数构造查询条件	
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始

	PROTOCOL::CSearchAlarmProcessRespond l_oRespond;
	GenRespondHeaderEx("search_alarm_process_respond", l_oSearchProcessRequest.m_oHeader, l_oRespond.m_oHeader);
	DataBase::SQLRequest tmp_oSQLRequst;
	//查询记录总数
	tmp_oSQLRequst.sql_id = "seach_pjdb_count";
	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_rRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_rRequestBody.m_strEndTime);
	if (l_rRequestBody.m_strBeginTime.empty())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "the request /body/begin_time is empty";
		std::string l_strMessage = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send Msg [%s]", l_strMessage.c_str());
		return;
	}

	if (l_rRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request /body/end_time is empty");
	}

	//tmp_oSQLRequst.param["begin_time"] = l_strBeginTime;
	//tmp_oSQLRequst.param["end_time"] = l_strEndTime;
	if (!l_rRequestBody.m_strAlarmID.empty())
	{
		tmp_oSQLRequst.param["alarm_id"] = l_rRequestBody.m_strAlarmID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(l_rRequestBody.m_strAlarmID);
		if (strTime != "")
		{
			tmp_oSQLRequst.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
			tmp_oSQLRequst.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
		}

		if (l_rRequestBody.m_strBeginTime.empty() && l_rRequestBody.m_strEndTime.empty())
		{
			//
		}
		else
		{
			tmp_oSQLRequst.param["begin_time"] = l_strBeginTime;
			tmp_oSQLRequst.param["end_time"] = l_strEndTime;
		}
	}
	else
	{
		tmp_oSQLRequst.param["begin_time"] = l_strBeginTime;
		tmp_oSQLRequst.param["end_time"] = l_strEndTime;
	}

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequst, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "ExecQuery Failed";

		std::string l_strMessage = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send Msg [%s]", l_strMessage.c_str());
		return;
	}
	l_oRespond.m_oBody.m_strAllCount = l_result->GetValue(0, "num");
	//查询当前页记录
	tmp_oSQLRequst.sql_id = "select_icc_t_pjdb";
	tmp_oSQLRequst.param["orderby"] = "gxsj desc";

	tmp_oSQLRequst.param["limit"] = std::to_string(l_iPageSize);
	tmp_oSQLRequst.param["offset"] = std::to_string(l_iRequestIdxBegin);

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(tmp_oSQLRequst, true);

	ICC_LOG_INFO(m_pLog, "query sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query alarm error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
		return;
	}
	else if (l_pRSet->RecordSize() > 0)
	{
		size_t l_iRecordSize = l_pRSet->RecordSize();
		l_oRespond.m_oBody.m_strCount = std::to_string(l_iRecordSize); //结果集警情总数
		ICC_LOG_DEBUG(m_pLog, "alarm process searched,begin to build respond msg,[%s] alarm process in total,this msg contains [%d] alarm(s)", l_oRespond.m_oBody.m_strCount.c_str(), l_pRSet->RecordSize());

		for (size_t index = 0; index < l_iRecordSize;)
		{
			PROTOCOL::CSearchAlarmProcessRespond::CData l_oProcess;

			l_oProcess.m_ProcessData.m_strDispatchDeptDistrictCode = l_pRSet->GetValue(index, "dispatch_dept_district_code");
			l_oProcess.m_ProcessData.m_strID = l_pRSet->GetValue(index, "id");
			l_oProcess.m_ProcessData.m_strAlarmID = l_pRSet->GetValue(index, "alarm_id");
			l_oProcess.m_ProcessData.m_strDispatchDeptCode = l_pRSet->GetValue(index, "dispatch_dept_code");
			l_oProcess.m_ProcessData.m_strDispatchCode = l_pRSet->GetValue(index, "dispatch_code");
			l_oProcess.m_ProcessData.m_strDispatchName = l_pRSet->GetValue(index, "dispatch_name");
			l_oProcess.m_ProcessData.m_strRecordID = l_pRSet->GetValue(index, "record_id");
			l_oProcess.m_ProcessData.m_strDispatchSuggestion = l_pRSet->GetValue(index, "dispatch_suggestion");
			l_oProcess.m_ProcessData.m_strProcessDeptCode = l_pRSet->GetValue(index, "process_dept_code");
			l_oProcess.m_ProcessData.m_strTimeSubmit = l_pRSet->GetValue(index, "time_submit");
			l_oProcess.m_ProcessData.m_strTimeArrived = l_pRSet->GetValue(index, "time_arrived");
			l_oProcess.m_ProcessData.m_strTimeSigned = l_pRSet->GetValue(index, "time_signed");
			l_oProcess.m_ProcessData.m_strProcessName = l_pRSet->GetValue(index, "process_name");
			l_oProcess.m_ProcessData.m_strProcessCode = l_pRSet->GetValue(index, "process_code");
			l_oProcess.m_ProcessData.m_strDispatchPersonnel = l_pRSet->GetValue(index, "dispatch_personnel");
			l_oProcess.m_ProcessData.m_strDispatchVehicles = l_pRSet->GetValue(index, "dispatch_vehicles");
			l_oProcess.m_ProcessData.m_strDispatchBoats = l_pRSet->GetValue(index, "dispatch_boats");
			l_oProcess.m_ProcessData.m_strState = l_pRSet->GetValue(index, "state");
			l_oProcess.m_ProcessData.m_strCreateTime = l_pRSet->GetValue(index, "create_time");
			l_oProcess.m_ProcessData.m_strUpdateTime = l_pRSet->GetValue(index, "update_time");
			l_oProcess.m_ProcessData.m_strDispatchDeptName = l_pRSet->GetValue(index, "dispatch_dept_name");
			l_oProcess.m_ProcessData.m_strDispatchDeptOrgCode = l_pRSet->GetValue(index, "dispatch_dept_org_code");
			l_oProcess.m_ProcessData.m_strProcessDeptName = l_pRSet->GetValue(index, "process_dept_name");
			l_oProcess.m_ProcessData.m_strProcessDeptOrgCode = l_pRSet->GetValue(index, "process_dept_org_code");
			l_oProcess.m_ProcessData.m_strProcessObjectType = l_pRSet->GetValue(index, "process_object_type");
			l_oProcess.m_ProcessData.m_strProcessObjectName = l_pRSet->GetValue(index, "process_object_name");
			l_oProcess.m_ProcessData.m_strProcessObjectCode = l_pRSet->GetValue(index, "process_object_code");
			l_oProcess.m_ProcessData.m_strBusinessStatus = l_pRSet->GetValue(index, "business_status");
			l_oProcess.m_ProcessData.m_strSeatCode = l_pRSet->GetValue(index, "seat_code");
			l_oProcess.m_ProcessData.m_strCancelTime = l_pRSet->GetValue(index, "cancel_time");
			l_oProcess.m_ProcessData.m_strCancelReason = l_pRSet->GetValue(index, "cancel_reason");
			l_oProcess.m_ProcessData.m_strIsAutoAssignJob = l_pRSet->GetValue(index, "is_auto_assign_job", "0");
			l_oProcess.m_ProcessData.m_strCreateUser = l_pRSet->GetValue(index, "create_user");
			l_oProcess.m_ProcessData.m_strUpdateUser = l_pRSet->GetValue(index, "update_user");
			l_oProcess.m_ProcessData.m_strOvertimeState = l_pRSet->GetValue(index, "overtime_state");

			l_oProcess.m_ProcessData.m_strTransfDeptOrjCode = l_pRSet->GetValue(index, "transfers_dept_org_code");
			l_oProcess.m_ProcessData.m_strIsOver = l_pRSet->GetValue(index, "is_over", "0");
			l_oProcess.m_ProcessData.m_strOverRemark = l_pRSet->GetValue(index, "over_remark");
			l_oProcess.m_ProcessData.m_strParentID = l_pRSet->GetValue(index, "parent_id");
			l_oProcess.m_ProcessData.m_strGZLDM = l_pRSet->GetValue(index, "flow_code");

			l_oProcess.m_ProcessData.m_strCentreProcessDeptCode = l_pRSet->GetValue(index, "centre_process_dept_code");
			l_oProcess.m_ProcessData.m_strCentreAlarmDeptCode = l_pRSet->GetValue(index, "centre_alarm_dept_code");
			l_oProcess.m_ProcessData.m_strDispatchDeptDistrictName = l_pRSet->GetValue(index, "dispatch_dept_district_name");
			l_oProcess.m_ProcessData.m_strLinkedDispatchCode = l_pRSet->GetValue(index, "linked_dispatch_code");
			l_oProcess.m_ProcessData.m_strOverTime = l_pRSet->GetValue(index, "over_time");
			l_oProcess.m_ProcessData.m_strFinishedTimeoutState = l_pRSet->GetValue(index, "finished_timeout_state");
			l_oProcess.m_ProcessData.m_strPoliceType = l_pRSet->GetValue(index, "police_type");
			l_oProcess.m_ProcessData.m_strProcessDeptShortName = l_pRSet->GetValue(index, "process_dept_short_name");
			l_oProcess.m_ProcessData.m_strDispatchDeptShortName = l_pRSet->GetValue(index, "dispatch_dept_short_name");
			l_oProcess.m_ProcessData.m_strReceivedTime = l_pRSet->GetValue(index, "received_time");
			l_oProcess.m_ProcessData.m_strCreateTeminal = l_pRSet->GetValue("createteminal");
			l_oProcess.m_ProcessData.m_strUpdateTeminal = l_pRSet->GetValue("updateteminal");

			l_oRespond.m_oBody.m_vData.push_back(l_oProcess);

			index++;
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "no alarm process respond to client, process count in query result=[%d],request pagesize=[%d],request pageindex=[%d],request index[%d-%d]",
			l_pRSet->RecordSize(), l_iPageSize, l_iPageIndex, l_iRequestIdxBegin + 1, l_iRequestIdxEnd);
	}

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response:[%s]", l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);
}

//反馈信息查询，客户端暂未使用，未联调
void CBusinessImpl::OnNotifiSearchAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "reqeust msg is empty");
		return;
	}

	PROTOCOL::CSearchAlarmFeedbackRequest l_oSearchFeedbackRequest;
	l_strRecvMsg = m_pString->ReplaceAll(l_strRecvMsg, "'", "''");
	if (!l_oSearchFeedbackRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	PROTOCOL::CSearchAlarmFeedbackRequest::CBody& l_rRequestBody = l_oSearchFeedbackRequest.m_oBody;
	//根据请求参数构造查询条件
	std::string l_strCondition;
	BuildQueryCondition(l_rRequestBody, l_strCondition);

	std::string l_strQueryStatement("SELECT * FROM public.icc_t_alarm_feedback feedback");

	l_strQueryStatement.append(" where ").append(l_strCondition);

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DBConn is not initialized properly.");
		return;
	}

	PROTOCOL::CSearchAlarmFeedbackRespond l_oRespond;
	GenRespondHeaderEx("search_alarm_feedback_respond", l_oSearchFeedbackRequest.m_oHeader, l_oRespond.m_oHeader);

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_strQueryStatement, true);

	ICC_LOG_INFO(m_pLog, "query sql:[%s]", l_strQueryStatement.c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query feedback info error,error msg[\n%s\n]", l_pRSet->GetErrorMsg().c_str());
		return;
	}
	else
	{
		l_oRespond.m_oBody.m_strCount = std::to_string(l_pRSet->RecordSize());//查询结果总数，而非返回的分包数		

		unsigned int l_iPageSize = std::atoi(l_rRequestBody.m_strPageSize.c_str());
		unsigned int l_iPageIndex = std::atoi(l_rRequestBody.m_strPageIndex.c_str());
		unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
		unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//索引从0开始

		ICC_LOG_INFO(m_pLog, "query result count:[%s], pagesize:[%u], pageindex:[%u], requestidend[%u], requestidxbegin[%u]",
			l_oRespond.m_oBody.m_strCount.c_str(), l_iPageSize, l_iPageIndex, l_iRequestIdxEnd, l_iRequestIdxBegin);

		if (l_pRSet->RecordSize() > l_iRequestIdxBegin)
		{
			unsigned int l_iActualIdxEnd = std::min(l_iRequestIdxEnd, l_pRSet->RecordSize());
			ICC_LOG_INFO(m_pLog, "actual indexend:[%u]", l_iActualIdxEnd);
			for (unsigned int index = l_iRequestIdxBegin; index < l_iActualIdxEnd; ++index)
			{
				/*ICC_LOG_INFO(m_pLog, "actual indexend:[%u], index:[%u] begin", l_iActualIdxEnd, index);

				std::vector<std::string> vecFields = l_pRSet->GetFieldNames();
				unsigned int iFiledSize = l_pRSet->FieldSize();
				for (int i = 0; i < iFiledSize; ++i)
				{
					std::string strTmp = vecFields[i];
					ICC_LOG_INFO(m_pLog, "actual fileds:[%s], filed count:[%u], index:[%d], value:[%s]", strTmp.c_str(), iFiledSize, i, l_pRSet->GetValue(index, strTmp).c_str());
				}
				int kkk = 0;*/

				PROTOCOL::CSearchAlarmFeedbackRespond::CFeedbackData l_oData;

				l_oData.m_strID = l_pRSet->GetValue(index, "id");
				l_oData.m_strAlarmID = l_pRSet->GetValue(index, "alarm_id");
				l_oData.m_strProcessID = l_pRSet->GetValue(index, "process_id");
				l_oData.m_strResultType = l_pRSet->GetValue(index, "result_type");
				l_oData.m_strResultContent = l_pRSet->GetValue(index, "result_content");
				l_oData.m_strLeaderInstruction = l_pRSet->GetValue(index, "leader_instruction");
				l_oData.m_strState = l_pRSet->GetValue(index, "state");
				l_oData.m_strTimeEdit = l_pRSet->GetValue(index, "time_edit");
				l_oData.m_strTimeSubmit = l_pRSet->GetValue(index, "time_submit");
				l_oData.m_strTimeArrived = l_pRSet->GetValue(index, "time_arrived");
				l_oData.m_strTimeSigned = l_pRSet->GetValue(index, "time_signed");
				l_oData.m_strTimePoliceDispatch = l_pRSet->GetValue(index, "time_police_dispatch");
				l_oData.m_strTimePoliceArrived = l_pRSet->GetValue(index, "time_police_arrived");
				l_oData.m_strActualOccurTime = l_pRSet->GetValue(index, "actual_occur_time");
				l_oData.m_strActualOccurAddr = l_pRSet->GetValue(index, "actual_occur_addr");
				l_oData.m_strFeedbackDeptDistrictCode = l_pRSet->GetValue(index, "feedback_dept_district_code");
				l_oData.m_strFeedbackDeptCode = l_pRSet->GetValue(index, "feedback_dept_code");
				l_oData.m_strFeedbackDeptName = l_pRSet->GetValue(index, "feedback_dept_name");
				l_oData.m_strFeedbackCode = l_pRSet->GetValue(index, "feedback_code");
				l_oData.m_strFeedbackName = l_pRSet->GetValue(index, "feedback_name");
				l_oData.m_strFeedbackLeaderCode = l_pRSet->GetValue(index, "feedback_leader_code");
				l_oData.m_strFeedbackLeaderName = l_pRSet->GetValue(index, "feedback_leader_name");
				l_oData.m_strProcessDeptDistrictCode = l_pRSet->GetValue(index, "process_dept_district_code");
				l_oData.m_strProcessDeptCode = l_pRSet->GetValue(index, "process_dept_code");
				l_oData.m_strProcessDeptName = l_pRSet->GetValue(index, "process_dept_name");
				l_oData.m_strProcessCode = l_pRSet->GetValue(index, "process_code");
				l_oData.m_strProcessName = l_pRSet->GetValue(index, "process_name");
				l_oData.m_strProcessLeaderCode = l_pRSet->GetValue(index, "process_leader_code");
				l_oData.m_strProcessLeaderName = l_pRSet->GetValue(index, "process_leader_name");
				l_oData.m_strDispatchDeptDistrictCode = l_pRSet->GetValue(index, "dispatch_dept_district_code");
				l_oData.m_strDispatchDeptCode = l_pRSet->GetValue(index, "dispatch_dept_code");
				l_oData.m_strDispatchDeptName = l_pRSet->GetValue(index, "dispatch_dept_name");
				l_oData.m_strDispatchCode = l_pRSet->GetValue(index, "dispatch_code");
				l_oData.m_strDispatchName = l_pRSet->GetValue(index, "dispatch_name");
				l_oData.m_strDispatchLeaderCode = l_pRSet->GetValue(index, "dispatch_leader_code");
				l_oData.m_strDispatchLeaderName = l_pRSet->GetValue(index, "dispatch_leader_name");
				l_oData.m_strPersonId = l_pRSet->GetValue(index, "person_id");
				l_oData.m_strPersonIdType = l_pRSet->GetValue(index, "person_id_type");
				l_oData.m_strPersonNationality = l_pRSet->GetValue(index, "person_nationality");
				l_oData.m_strPersonName = l_pRSet->GetValue(index, "person_name");
				l_oData.m_strPersonSlaveId = l_pRSet->GetValue(index, "person_slave_id");
				l_oData.m_strPersonSlaveIdType = l_pRSet->GetValue(index, "person_slave_id_type");
				l_oData.m_strPersonSlaveNationality = l_pRSet->GetValue(index, "person_slave_nationality");
				l_oData.m_strPersonSlaveName = l_pRSet->GetValue(index, "person_slave_name");
				l_oData.m_strAlarmFirstType = l_pRSet->GetValue(index, "alarm_first_type");
				l_oData.m_strAlarmSecondType = l_pRSet->GetValue(index, "alarm_second_type");
				l_oData.m_strAlarmThirdType = l_pRSet->GetValue(index, "alarm_third_type");
				l_oData.m_strAlarmFourthType = l_pRSet->GetValue(index, "alarm_fourth_type");
				l_oData.m_strAlarmAddrDeptName = l_pRSet->GetValue(index, "alarm_addr_dept_name");
				l_oData.m_strAlarmAddrFirstType = l_pRSet->GetValue(index, "alarm_addr_first_type");
				l_oData.m_strAlarmAddrSecondType = l_pRSet->GetValue(index, "alarm_addr_second_type");
				l_oData.m_strAlarmAddrThirdType = l_pRSet->GetValue(index, "alarm_addr_third_type");
				l_oData.m_strAlarmLongitude = l_pRSet->GetValue(index, "alarm_longitude");
				l_oData.m_strAlarmLatitude = l_pRSet->GetValue(index, "alarm_latitude");
				l_oData.m_strAlarmRegionType = l_pRSet->GetValue(index, "alarm_region_type");
				l_oData.m_strAlarmLocationType = l_pRSet->GetValue(index, "alarm_location_type");
				l_oData.m_strPeopleNumCapture = l_pRSet->GetValue(index, "people_num_capture");
				l_oData.m_strPeopleNumRescue = l_pRSet->GetValue(index, "people_num_rescue");
				l_oData.m_strPeopleNumSlightInjury = l_pRSet->GetValue(index, "people_num_slight_injury");
				l_oData.m_strPeopleNumSeriousInjury = l_pRSet->GetValue(index, "people_num_serious_injury");
				l_oData.m_strPeopleNumDeath = l_pRSet->GetValue(index, "people_num_death");
				l_oData.m_strPoliceNumDispatch = l_pRSet->GetValue(index, "police_num_dispatch");
				l_oData.m_strPoliceCarNumDispatch = l_pRSet->GetValue(index, "police_car_num_dispatch");
				l_oData.m_strEconomyLoss = l_pRSet->GetValue(index, "economy_loss");
				l_oData.m_strRetrieveEconomyLoss = l_pRSet->GetValue(index, "retrieve_economy_loss");
				l_oData.m_strFirePutOutTime = l_pRSet->GetValue(index, "fire_put_out_time");
				l_oData.m_strFireBuildingFirstType = l_pRSet->GetValue(index, "fire_building_first_type");
				l_oData.m_strFireBuildingSecondType = l_pRSet->GetValue(index, "fire_building_second_type");
				l_oData.m_strFireBuildingThirdType = l_pRSet->GetValue(index, "fire_building_third_type");
				l_oData.m_strFireSourceType = l_pRSet->GetValue(index, "fire_source_type");
				l_oData.m_strFireRegionType = l_pRSet->GetValue(index, "fire_region_type");
				l_oData.m_strFireCauseFirstType = l_pRSet->GetValue(index, "fire_cause_first_type");
				l_oData.m_strFireCauseSecondType = l_pRSet->GetValue(index, "fire_cause_second_type");
				l_oData.m_strFireCauseThirdType = l_pRSet->GetValue(index, "fire_cause_third_type");
				l_oData.m_strFireArea = l_pRSet->GetValue(index, "fire_area");
				l_oData.m_strTrafficRoadLevel = l_pRSet->GetValue(index, "traffic_road_level");
				l_oData.m_strTrafficAccidentLevel = l_pRSet->GetValue(index, "traffic_accident_level");
				l_oData.m_strTrafficVehicleNo = l_pRSet->GetValue(index, "traffic_vehicle_no");
				l_oData.m_strTrafficVehicleType = l_pRSet->GetValue(index, "traffic_vehicle_type");
				l_oData.m_strTrafficSlaveVehicleNo = l_pRSet->GetValue(index, "traffic_slave_vehicle_no");
				l_oData.m_strTrafficSlaveVehicleType = l_pRSet->GetValue(index, "traffic_slave_vehicle_type");
				/*l_oData.m_strIsSolveCrown = l_pRSet->GetValue(index, "is_solve_crown");
				l_oData.m_strIsSolvePublicSecurity = l_pRSet->GetValue(index, "is_solve_public_security");
				l_oData.m_strIsInvolveForeign = l_pRSet->GetValue(index, "is_involve_foreign");
				l_oData.m_strIsInvolvePolice = l_pRSet->GetValue(index, "is_involve_police");
				l_oData.m_strIsInvolveGun = l_pRSet->GetValue(index, "is_involve_gun");
				l_oData.m_strIsInvolveKnife = l_pRSet->GetValue(index, "is_involve_knife");
				l_oData.m_strIsInvolveTerror = l_pRSet->GetValue(index, "is_involve_terror");
				l_oData.m_strIsInvolvePornography = l_pRSet->GetValue(index, "is_involve_pornography");
				l_oData.m_strIsInvolveGamble = l_pRSet->GetValue(index, "is_involve_gamble");
				l_oData.m_strIsInvolvePoison = l_pRSet->GetValue(index, "is_involve_poison");
				l_oData.m_strIsInvolveExt1 = l_pRSet->GetValue(index, "is_involve_ext1");
				l_oData.m_strIsInvolveExt2 = l_pRSet->GetValue(index, "is_involve_ext2");
				l_oData.m_strIsInvolveExt3 = l_pRSet->GetValue(index, "is_involve_ext3");*/
				l_oData.m_strAlarmCalledNoType = l_pRSet->GetValue(index, "alarm_called_no_type");
				l_oData.m_strEventType = l_pRSet->GetValue(index, "event_type");
				l_oData.m_strCreateUser = l_pRSet->GetValue(index, "create_user");
				l_oData.m_strCreateTime = l_pRSet->GetValue(index, "create_time");
				l_oData.m_strUpdateUser = l_pRSet->GetValue(index, "update_user");
				l_oData.m_strUpdateTime = l_pRSet->GetValue(index, "update_time");


				l_oRespond.m_oBody.m_vecData.push_back(l_oData);

				ICC_LOG_INFO(m_pLog, "actual indexend:[%u], index:[%u] end", l_iActualIdxEnd, index);
			}
		}
		else
		{
			ICC_LOG_WARNING(m_pLog, "reqeuest parameter error,result size=[%d],request pagesize=[%d],request pageindex=[%d],request index[%d-%d]",
				l_pRSet->RecordSize(), l_iPageSize, l_iPageIndex, l_iRequestIdxBegin + 1, l_iRequestIdxEnd);
		}
	}

	if (!m_pJsonFty)
	{
		ICC_LOG_FATAL(m_pLog, "dbagent json fty is null,can not generate response msg");
		return;
	}

	ICC_LOG_INFO(m_pLog, "begin response");

	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response:[%s]", l_strMessage.c_str());
	p_pNotifiRequest->Response(l_strMessage);
}

//统计分析信息查询，只能查询用户所在部门及其下级部门的统计信息
//在警情数量极大时查询效率降低明显，后期可考虑将维护实时统计信息，并存入数据库，
//届时可直接查库,避免SQL关联太多表而影响查询速度
void ICC::CBusinessImpl::OnNotifiSearchStatisticInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "search_statistic_info_request msg is empty");
		return;
	}

	PROTOCOL::CSearchStatisticInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse search_statistic_info_request msg failed.");
		return;
	}

	unsigned int l_iPageSize = std::atoi(l_oRequest.m_oBody.m_strPageSize.c_str());
	unsigned int l_iPageIndex = std::atoi(l_oRequest.m_oBody.m_strPageIndex.c_str());
	l_iPageIndex = l_iPageIndex > 0 ? l_iPageIndex : 1;
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//l_iPageSize最小为1;索引从0开始

	//构造SQL查询请求
	/*DataBase::SQLRequest l_tQueryAlarmCount;
	l_tQueryAlarmCount.sql_id = "search_statistic_analysis_info";
	l_tQueryAlarmCount.param["begin_time"] = l_oRequest.m_oBody.m_strBeginTime;
	l_tQueryAlarmCount.param["end_time"] = l_oRequest.m_oBody.m_strEndTime;
	if (!l_oRequest.m_oBody.m_strCurDeptCode.empty())
	{//限制用户只能查询其所在部门及下级部门相关统计信息
		l_tQueryAlarmCount.param["cur_dept_code"] = l_oRequest.m_oBody.m_strCurDeptCode;
	}
	if (!l_oRequest.m_oBody.m_strClientID.empty())
	{
		l_tQueryAlarmCount.param["client_id"] = l_oRequest.m_oBody.m_strClientID;
	}
	if (!l_oRequest.m_oBody.m_strClientName.empty())
	{
		l_tQueryAlarmCount.param["client_name"] = l_oRequest.m_oBody.m_strClientName;
	}
	if (l_iPageSize != 0) //page_size为0时不分页
	{
		l_tQueryAlarmCount.param["limit"] = l_oRequest.m_oBody.m_strPageSize;	//只取一页
		l_tQueryAlarmCount.param["offset"] = std::to_string(l_iRequestIdxBegin);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tQueryAlarmCount);*/

	//使用数据库函数查询统计信息，该方法在警情数量较少时比上面的方法慢，但在警情数量极大时更快，需要进一步评估
	std::string l_strQueryStatisticInfo;
	if (m_iStatisticMethod == STATISTIC_QTABLE)
	{	//query_statistic_info ：直接查询icc_t_statistic表中的数据，icc_t_statistic表中数据由各种触发器维护
		l_strQueryStatisticInfo = "select * from query_statistic_info";
	}
	else
	{	//get_statistic_info: 每次请求时各统计出单页的上下线时间，话务数量等数据，然后再分别count每次记录的接警数
		l_strQueryStatisticInfo = "select * from get_statistic_info";
	}

	l_strQueryStatisticInfo = l_strQueryStatisticInfo + str(boost::format("('%1%','%2%',%3%,%4%,'%5%','%6%','%7%')")
		% l_oRequest.m_oBody.m_strBeginTime
		% l_oRequest.m_oBody.m_strEndTime
		% l_oRequest.m_oBody.m_strPageSize
		% l_iRequestIdxBegin
		% l_oRequest.m_oBody.m_strCurDeptCode
		% l_oRequest.m_oBody.m_strClientName
		% l_oRequest.m_oBody.m_strClientID);

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_strQueryStatisticInfo, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	PROTOCOL::CSearchStatisticInfoRespond l_oRespond;
	GenRespondHeader("search_statistic_info_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query statistic error:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pRSet->RecordSize() > 0)
		{
			while (l_pRSet->Next())
			{
				PROTOCOL::CSearchStatisticInfoRespond::CData l_oData;
				l_oData.m_strClientName = l_pRSet->GetValue("client_name");
				l_oData.m_strClientID = l_pRSet->GetValue("client_id");
				l_oData.m_strDeptName = l_pRSet->GetValue("dept_name");
				l_oData.m_strOnlineTime = l_pRSet->GetValue("online_time");
				l_oData.m_strOfflineTime = l_pRSet->GetValue("offline_time");
				std::string l_strOnlineTimeLen = l_pRSet->GetValue("online_time_len");
				l_oData.m_strOnlineTimeLength = l_strOnlineTimeLen.empty() ? "0" : l_strOnlineTimeLen;
				std::string l_strOffSeatTimeLen = l_pRSet->GetValue("offseat_time_len");
				int64 l_iOffsetLen = m_pString->ToInt64(l_strOffSeatTimeLen);
				l_oData.m_strOffSeatTimeLength = m_pString->ToInt64(l_strOffSeatTimeLen) <= 0 ? "0" : l_strOffSeatTimeLen;
				l_oData.m_strReleaseCallRefCount = l_pRSet->GetValue("release_call_num");
				l_oData.m_strCallRefCount = l_pRSet->GetValue("total_call_num");
				l_oData.m_strAlarmCount = l_pRSet->GetValue("alarm_num");
				l_oRespond.m_oBody.m_vecData.push_back(l_oData);
			}
			l_oRespond.m_oBody.m_strCount = l_pRSet->GetValue(0, "count");
			ICC_LOG_DEBUG(m_pLog, "[SearchAlarmCount]Found [%s] In Request Range [%d-%d]",
				l_oRespond.m_oBody.m_strCount.c_str(), l_iRequestIdxBegin, l_iRequestIdxEnd);
		}
		else
		{
			l_oRespond.m_oBody.m_strCount = "0";
			ICC_LOG_WARNING(m_pLog, "[SearchAlarmCount]Found [0] In Request Range:[%d-%d]", l_iRequestIdxBegin, l_iRequestIdxEnd);
		}
	}

	SendRespondMsg(l_oRespond, p_pNotifiReqeust);
}

//警情数查询
void CBusinessImpl::OnNotifiSearchAlarmCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "search_alarm_count_request msg is empty");
		return;
	}

	PROTOCOL::CSearchAlarmCountRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse search_alarm_count_request msg failed.");
		return;
	}

	unsigned int l_iPageSize = std::atoi(l_oRequest.m_oBody.m_strPageSize.c_str());
	unsigned int l_iPageIndex = std::atoi(l_oRequest.m_oBody.m_strPageIndex.c_str());
	l_iPageIndex = l_iPageIndex > 0 ? l_iPageIndex : 1;
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//l_iPageSize最小为1;索引从0开始
	//构造SQL查询请求
	DataBase::SQLRequest l_tQueryAlarmCount;
	l_tQueryAlarmCount.sql_id = "search_alarm_count_statistics";
	l_tQueryAlarmCount.param["begin_time"] = l_oRequest.m_oBody.m_strBeginTime;
	l_tQueryAlarmCount.param["end_time"] = l_oRequest.m_oBody.m_strEndTime;
	l_tQueryAlarmCount.param["groupby"] = "receipt_code";

	if (l_iPageSize != 0) //page_size为0时不分页
	{
		l_tQueryAlarmCount.param["limit"] = l_oRequest.m_oBody.m_strPageSize;	//只取一页
		l_tQueryAlarmCount.param["offset"] = std::to_string(l_iRequestIdxBegin);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tQueryAlarmCount, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	PROTOCOL::CSearchAlarmCountRespond l_oRespond;
	GenRespondHeader("search_alarm_count_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query alarm count error:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_INFO(m_pLog, "[%s]", l_pRSet->GetErrorMsg().c_str());
		if (l_pRSet->RecordSize() > 0)
		{
			while (l_pRSet->Next())
			{
				PROTOCOL::CSearchAlarmCountRespond::CData l_oData;
				l_oData.m_strUserCode = l_pRSet->GetValue("receipt_code");
				l_oData.m_strAlarmCount = l_pRSet->GetValue("alarm_count");
				l_oData.m_strInvalidAlarmCount = l_pRSet->GetValue("invalid_alarm_count");
				l_oRespond.m_oBody.m_vecData.push_back(l_oData);
			}

			l_oRespond.m_oBody.m_strCount = l_pRSet->GetValue(0, "count");
			l_oRespond.m_oBody.m_strTotalAlarmCount = l_pRSet->GetValue(0, "total_alarm_count");//total_alarm_count
			l_oRespond.m_oBody.m_strTotalInvalidAlarmCount = l_pRSet->GetValue(0, "total_invalid_alarm_count");//total_invalid_alarm_count
			ICC_LOG_DEBUG(m_pLog, "[SearchAlarmCount]Found [%s] In Request Range [%d-%d]",
				l_oRespond.m_oBody.m_strCount.c_str(), l_iRequestIdxBegin, l_iRequestIdxEnd);
		}
		else
		{
			l_oRespond.m_oBody.m_strCount = "0";
			l_oRespond.m_oBody.m_strTotalAlarmCount = "0";
			l_oRespond.m_oBody.m_strTotalInvalidAlarmCount = "0";
			ICC_LOG_WARNING(m_pLog, "[SearchAlarmCount]Found [0] In Request Range:[%d-%d]", l_iRequestIdxBegin, l_iRequestIdxEnd);
		}
	}

	SendRespondMsg(l_oRespond, p_pNotifiReqeust);
}

//话务数查询-席位监控左侧蓝柱数据
//需将座席信息注册到系统（设置-添加座席,icc_t_seat表），否则无法查询该座席的信息
void CBusinessImpl::OnNotifiSearchCallrefCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_LOWDEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "search_alarm_count_request msg is empty");
		return;
	}

	PROTOCOL::CSearchCallrefCountRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse search_alarm_count_request msg failed.");
		return;
	}

	unsigned int l_iPageSize = std::atoi(l_oRequest.m_oBody.m_strPageSize.c_str());
	unsigned int l_iPageIndex = std::atoi(l_oRequest.m_oBody.m_strPageIndex.c_str());
	l_iPageIndex = l_iPageIndex > 0 ? l_iPageIndex : 1;
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//索引从0开始

	//构造SQL查询请求
	DataBase::SQLRequest l_tQueryCallrefCount;
	l_tQueryCallrefCount.sql_id = "search_callref_count_statistics";
	l_tQueryCallrefCount.param["call_direction"] = "in";//接听电话
	l_tQueryCallrefCount.param["begin_time"] = l_oRequest.m_oBody.m_strBeginTime;
	l_tQueryCallrefCount.param["end_time"] = l_oRequest.m_oBody.m_strEndTime;
	l_tQueryCallrefCount.param["dept_code"] = l_oRequest.m_oBody.m_strDeptCode;

	if (l_iPageSize != 0) //page_size为0时不分页
	{
		l_tQueryCallrefCount.param["limit"] = l_oRequest.m_oBody.m_strPageSize;	//只取一页
		l_tQueryCallrefCount.param["offset"] = std::to_string(l_iRequestIdxBegin);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tQueryCallrefCount, true);
	ICC_LOG_LOWDEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	PROTOCOL::CSearchCallrefCountRespond l_oRespond;
	GenRespondHeader("search_callref_count_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query callref count error:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else if (l_pRSet->RecordSize() > 0)
	{
		std::map<std::string, int> l_mapCallRef;
		while (l_pRSet->Next())
		{
			std::string l_strUserCode = l_pRSet->GetValue("client_name");
			if (l_strUserCode.empty())
			{
				l_strUserCode = l_pRSet->GetValue("called_id");
			}
			if (l_mapCallRef.find(l_strUserCode) == l_mapCallRef.end())
			{
				l_mapCallRef[l_strUserCode] = std::atoi(l_pRSet->GetValue("callref_count").c_str());
			}
			else
			{
				l_mapCallRef[l_strUserCode] = l_mapCallRef[l_strUserCode] + std::atoi(l_pRSet->GetValue("callref_count").c_str());
			}
		}

		PROTOCOL::CSearchCallrefCountRespond::CData l_oData;
		for (auto it = l_mapCallRef.begin(); it != l_mapCallRef.end(); ++it)
		{
			l_oData.m_strUserCode = it->first;
			l_oData.m_strCallrefCount = std::to_string(it->second);
			l_oRespond.m_oBody.m_vecData.push_back(l_oData);
		}

		l_oRespond.m_oBody.m_strCount = l_pRSet->GetValue(0, "count");//分包总数
		l_oRespond.m_oBody.m_strTotalCallrefCount = l_pRSet->GetValue(0, "total_callref_count");//total_callref_count
		ICC_LOG_LOWDEBUG(m_pLog, "[SearchCallrefCount]Found [%s] In Request Range [%d-%d]",
			l_oRespond.m_oBody.m_strCount.c_str(), l_iRequestIdxBegin, l_iRequestIdxEnd);
	}
	else
	{
		l_oRespond.m_oBody.m_strCount = "0";
		l_oRespond.m_oBody.m_strTotalCallrefCount = "0";
		ICC_LOG_LOWDEBUG(m_pLog, "[SearchCallrefCount]Found [0] In Request Range:[%d-%d]", l_iRequestIdxBegin, l_iRequestIdxEnd);
	}

	SendRespondMsg(l_oRespond, p_pNotifiReqeust);
}

//话务信息查询
void CBusinessImpl::OnNotifiGetCalleventInfoRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "request msg is empty");
		return;
	}

	PROTOCOL::CGetCallEventInfoRequest l_pRequest;
	if (!l_pRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	PROTOCOL::CGetCallEventInfoRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_pRequest.m_oHeader.m_strMsgId;
	//GenRespondHeader("get_callevent_info_respond", l_pRequest.m_oHeader, l_oRespond.m_oHeader);

	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_callevent";
	l_tSQLRequest.param["callref_id"] = l_pRequest.m_oBody.m_strCallrefID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(l_pRequest.m_oBody.m_strCallrefID);
	if (strTime != "")
	{
		l_tSQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "query callevent failed";
		ICC_LOG_ERROR(m_pLog, "query callevent info failed,error msg[%s]", l_pRSet->GetErrorMsg().c_str());
	}

	//话务查询结果：查询成功则返回话务信息，失败则返回空值	
	ICC_LOG_INFO(m_pLog, "[%s]", l_pRSet->GetErrorMsg().c_str());
	if (l_pRSet->RecordSize() > 0)
	{
		l_oRespond.m_oBody.m_strCallrefID = l_pRSet->GetValue(0, "callref_id");
		l_oRespond.m_oBody.m_strACD = l_pRSet->GetValue(0, "acd");
		l_oRespond.m_oBody.m_strCallerID = l_pRSet->GetValue(0, "caller_id");
		l_oRespond.m_oBody.m_strCalledID = l_pRSet->GetValue(0, "called_id");
		l_oRespond.m_oBody.m_strCallDirection = l_pRSet->GetValue(0, "call_direction");
		l_oRespond.m_oBody.m_strTalkTime = l_pRSet->GetValue(0, "talk_time");
		l_oRespond.m_oBody.m_strHangupTime = l_pRSet->GetValue(0, "hangup_time");
		l_oRespond.m_oBody.m_strHangupType = l_pRSet->GetValue(0, "hangup_type");
	}

	if (!m_pJsonFty)
	{
		ICC_LOG_FATAL(m_pLog, "dbagent json fty is null,can not generate response msg");
		return;
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_INFO(m_pLog, "response:[%s]", l_strMessage.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}

void CBusinessImpl::OnNotifiCallEventSearchCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "request msg is empty");
		return;
	}

	PROTOCOL::CCallEventSearchRequest l_pRequest;
	if (!l_pRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	std::string l_strCallEventQuerySQL;
	PROTOCOL::CCallEventSearchRequest::CBody& l_oRequestBody = l_pRequest.m_oBody;
	DataBase::SQLRequest l_oSqlRequest;

	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		l_oSqlRequest.sql_id = "seach_callevent_with_seatno";
		l_oSqlRequest.param["seat_no"] = l_oRequestBody.m_strSeatNo;
	}
	else
	{
		l_oSqlRequest.sql_id = "seach_callevent_without_seatno_allcount";
	}

	if (!l_oRequestBody.m_strDeptCode.empty() && l_oRequestBody.m_strHangupType != "blackcall")
	{
		l_oSqlRequest.param["dept_code"] = l_oRequestBody.m_strDeptCode;
	}

	std::string l_strCallerNumber = m_pString->Trim(l_oRequestBody.m_strCallerID);
	if (!l_strCallerNumber.empty())
	{
		l_oSqlRequest.param["caller_id"] = l_strCallerNumber;
	}

	/*if (!l_oRequestBody.m_strCallerID.empty())
	{
		l_oSqlRequest.param["caller_id"] = l_oRequestBody.m_strCallerID;
	}*/

	std::string l_strCalledNumber = m_pString->Trim(l_oRequestBody.m_strCalledID);
	if (!l_strCalledNumber.empty())
	{
		l_oSqlRequest.param["called_id"] = l_strCalledNumber;
	}

	/*if (!l_oRequestBody.m_strCalledID.empty())
	{
		l_oSqlRequest.param["called_id"] = l_oRequestBody.m_strCalledID;
	}*/

	std::string l_strBeginTime(l_oRequestBody.m_strStartTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strStartTime.empty())
	{
		l_strBeginTime = "1900-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	l_oSqlRequest.param["create_time_begin"] = l_strBeginTime;

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}
	l_oSqlRequest.param["create_time_end"] = l_strEndTime;
	if (!l_oRequestBody.m_strHangupType.empty())
	{

		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append("(" + BuildMultiConditions("c.hangup_type", "=", l_oRequestBody.m_strHangupType) + ")");
		l_oSqlRequest.param["hangup_type"] = BuildMultiConditions("c.hangup_type", " = ", l_oRequestBody.m_strHangupType);
	}
	if (!l_oRequestBody.m_strReceiptCodeOrName.empty())
	{
		/*l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(a.receipt_code like '%%%1%%%' or "
			"a.receipt_name like '%%%1%%%' or cb.receipt_name like '%%%1%%%')") % l_oRequestBody.m_strReceiptCodeOrName));*/
		l_oSqlRequest.param["receipt_code_or_name"] = l_oRequestBody.m_strReceiptCodeOrName;
	}
	std::string l_strAlarmId;
	if (!l_oRequestBody.m_strCaseID.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(a.id like '%%%1%%%' or cb.alarm_id like '%%%2%%%')") % l_oRequestBody.m_strCaseID % l_oRequestBody.m_strCaseID));
		l_strAlarmId = l_oRequestBody.m_strCaseID;
		l_oSqlRequest.param["alarm_id"] = l_oRequestBody.m_strCaseID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strCaseID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	if (!l_oRequestBody.m_strCallrefID.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.callref_id = '%1%')") % l_oRequestBody.m_strCallrefID));
		l_oSqlRequest.param["callref_id"] = l_oRequestBody.m_strCallrefID;

		std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequestBody.m_strCallrefID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	if (!l_oRequestBody.m_strCallDirection.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.call_direction = '%1%')") % l_oRequestBody.m_strCallDirection));
		l_oSqlRequest.param["call_direction"] = l_oRequestBody.m_strCallDirection;
	}
	std::string l_stRelAlarmId;
	if (!l_oRequestBody.m_strRelAlarmID.empty())
	{
		l_stRelAlarmId = l_oRequestBody.m_strRelAlarmID;
		l_oSqlRequest.param["rel_alarm_id"] = l_oRequestBody.m_strRelAlarmID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strRelAlarmID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}



	if (!l_strAlarmId.empty() && !l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s' or c.rel_alarm_id = '%s'", l_strAlarmId.c_str(), l_stRelAlarmId.c_str());
	}
	else if (!l_strAlarmId.empty() && l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s'", l_strAlarmId.c_str());
	}
	else if (l_strAlarmId.empty() && !l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s'", l_stRelAlarmId.c_str());
	}


	if (l_pRequest.m_oBody.m_strIsInnerCall.compare("1") == 0)
	{
		l_oSqlRequest.param["is_innercall"] = l_pRequest.m_oHeader.m_strSeatNo;
	}

	std::string l_autoUrgeCallerNum;
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum))
	{
		l_oSqlRequest.param["autourge_caller"] = l_autoUrgeCallerNum;
	}

	//std::string l_strQueryCondition;
	//BuildQueryCondition(l_oRequestBody, l_strQueryCondition);
	/*if (!l_strQueryCondition.empty())
	{
		l_strCallEventQuerySQL.append(" where ").append(l_strQueryCondition).append(" order by c.callref_id desc ");
	}*/
	//l_oSqlRequest.param["orderby"] = "c.callref_id desc";

	PROTOCOL::CCallEventSearchRespond l_oRespond;
	//	GenRespondHeader("callevent_search_respond", l_pRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_pRequest.m_oHeader.m_strMsgId;
	//l_oRespond.m_oBody.m_strPageIndex = l_pRequest.m_oBody.m_strPageIndex;
	l_oRespond.m_oBody.m_strAllCount = "0";
	l_oRespond.m_oBody.m_strCount = "0";

	DataBase::IResultSetPtr	l_pRSet = m_pDBConn->Exec(l_oSqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql [%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "query callevent failed";

		ICC_LOG_ERROR(m_pLog, "query callevent info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	int nCount = 0;
	if (l_pRSet->RecordSize() > 0)
	{
		l_oRespond.m_oBody.m_strAllCount = l_pRSet->GetValue(0, "count");
		l_oRespond.m_oBody.m_strCount = l_oRespond.m_oBody.m_strAllCount;
		ICC_LOG_DEBUG(m_pLog, "[%s] callevent info searched,begin to build the respond msg body.", l_oRespond.m_oBody.m_strAllCount.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "[0] callevent info searched.");
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	//ICC_LOG_DEBUG(m_pLog, "response:[%s]", l_strMessage.c_str());
	ICC_LOG_DEBUG(m_pLog, "response,msgID:[%s],allCount:%s", l_oRespond.m_oHeader.m_strMsgId.c_str(), l_oRespond.m_oBody.m_strAllCount.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}

int ICC::CBusinessImpl::BuildQuerySqlId(ObserverPattern::INotificationPtr p_pNotifiReqeust, DataBase::SQLRequest& l_oSqlRequest, PROTOCOL::CCallEventSearchRequest::CBody l_oRequestBody)
{
	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		l_oSqlRequest.sql_id = "seach_callevent_with_seatno";
		l_oSqlRequest.param["seat_no"] = l_oRequestBody.m_strSeatNo;

		return CALL_QUERY_WITH_SEAT;
	}
	else if (l_oRequestBody.m_strSearchTag == "1" || (l_oRequestBody.m_strIsVcsSearch == "1" && !l_oRequestBody.m_strRelAlarmID.empty()))
	{
		l_oSqlRequest.sql_id = "seach_callevent_with_relalarmid";

		return CALL_QUERY_WITH_ALARM;
	}
	else
	{
		l_oSqlRequest.sql_id = "seach_callevent_without_seatno";

		return CALL_QUERY_COMMON;
	}
}

//历史话务查询请求响应函数
//1.录音查询		2.早释电话查询-带运营商和归属地信息
//需将座席信息注册到系统（设置-添加座席,icc_t_seat表），否则无法查询该座席的信息
void ICC::CBusinessImpl::OnNotifiCallEventSearchRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "request msg is empty");
		return;
	}

	PROTOCOL::CCallEventSearchRequest l_pRequest;
	if (!l_pRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	std::string l_strCallEventQuerySQL;
	PROTOCOL::CCallEventSearchRequest::CBody& l_oRequestBody = l_pRequest.m_oBody;

	DataBase::SQLRequest l_oSqlRequest;
	std::map<std::string, std::string> l_mapAgentPhone;

	int cur_type = BuildQuerySqlId(p_pNotifiReqeust, l_oSqlRequest, l_oRequestBody);

	if (!l_oRequestBody.m_strExtraBusinessScene.empty())
	{
		if (m_pString->ToInt(l_oRequestBody.m_strExtraBusinessScene) == only_self)
		{
			l_oSqlRequest.param["extra_business_scene"] = m_pString->Format("(c.caller_id = '%s' or c.called_id = '%s') and c.caller_id != c.called_id", l_pRequest.m_oHeader.m_strSeatNo.c_str(), l_pRequest.m_oHeader.m_strSeatNo.c_str());
		}
		else if (m_pString->ToInt(l_oRequestBody.m_strExtraBusinessScene) == early_release_query)
		{
			l_oSqlRequest.param["extra_business_scene"] = m_pString->Format("c.is_callback is Null and '%s' = c.acd_relation_dept", l_oRequestBody.m_strReleaseSearchDept.c_str());
		}
		else
		{
			l_oSqlRequest.param["extra_business_scene"]= m_pString->Format("c.rel_alarm_id != ''");
		}
	}

	if (!l_oRequestBody.m_strDeptCode.empty() && l_oRequestBody.m_strHangupType != "blackcall")
	{
		l_oSqlRequest.param["dept_code"] = l_oRequestBody.m_strDeptCode;
	}

	std::string l_strCallerNumber = m_pString->Trim(l_oRequestBody.m_strCallerID);
	if (!l_strCallerNumber.empty())
	{
		l_oSqlRequest.param["caller_id"] = l_strCallerNumber;
	}

	/*if (!l_oRequestBody.m_strCallerID.empty())
	{
		l_oSqlRequest.param["caller_id"] = l_oRequestBody.m_strCallerID;
	}*/

	std::string l_strCalledNumber = m_pString->Trim(l_oRequestBody.m_strCalledID);
	if (!l_strCalledNumber.empty())
	{
		l_oSqlRequest.param["called_id"] = l_strCalledNumber;
	}

	/*if (!l_oRequestBody.m_strCalledID.empty())
	{
		l_oSqlRequest.param["called_id"] = l_oRequestBody.m_strCalledID;
	}*/

	std::string l_strBeginTime(l_oRequestBody.m_strStartTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strStartTime.empty())
	{
		l_strBeginTime = "1900-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	l_oSqlRequest.param["create_time_begin"] = l_strBeginTime;

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}
	l_oSqlRequest.param["create_time_end"] = l_strEndTime;

	/*std::string l_strTimeCondition = str(boost::format(
		"((c.incoming_time BETWEEN '%1%' and '%2%' AND c.call_direction = 'in') "
		"OR (c.ringback_time BETWEEN '%1%' and '%2%' AND c.call_direction = 'out'))")
		% l_strBeginTime%l_strEndTime);*/
		// 有的话务记录可以没有incoming_time和ringback_time [7/19/2018 t26150]
	//std::string l_strTimeCondition = str(boost::format("(c.callref_id BETWEEN to_char('%1%'::TIMESTAMP, 'YYYYMMDDHH24MISS')"
	//	"and to_char('%2%'::TIMESTAMP+interval'1 second', 'YYYYMMDDHH24MISS'))") % l_strBeginTime % l_strEndTime);

	//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
	//l_strCondition.append(l_strTimeCondition);

	if (!l_oRequestBody.m_strHangupType.empty())
	{

		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append("(" + BuildMultiConditions("c.hangup_type", "=", l_oRequestBody.m_strHangupType) + ")");
		l_oSqlRequest.param["hangup_type"] = BuildMultiConditions("c.hangup_type", " = ", l_oRequestBody.m_strHangupType);
	}
	if (!l_oRequestBody.m_strReceiptCodeOrName.empty())
	{
		/*l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(a.receipt_code like '%%%1%%%' or "
			"a.receipt_name like '%%%1%%%' or cb.receipt_name like '%%%1%%%')") % l_oRequestBody.m_strReceiptCodeOrName));*/
		l_oSqlRequest.param["receipt_code_or_name"] = l_oRequestBody.m_strReceiptCodeOrName;
	}
	std::string l_strAlarmId;
	//不支持警单号模糊查询
	if (!l_oRequestBody.m_strCaseID.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(a.id like '%%%1%%%' or cb.alarm_id like '%%%2%%%')") % l_oRequestBody.m_strCaseID % l_oRequestBody.m_strCaseID));
		l_strAlarmId = l_oRequestBody.m_strCaseID;
		l_oSqlRequest.param["alarm_id"] = l_oRequestBody.m_strCaseID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strCaseID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	

	if (!l_oRequestBody.m_strCallrefID.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.callref_id = '%1%')") % l_oRequestBody.m_strCallrefID));
		l_oSqlRequest.param["callref_id"] = l_oRequestBody.m_strCallrefID;

		std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequestBody.m_strCallrefID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	if (!l_oRequestBody.m_strCallDirection.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.call_direction = '%1%')") % l_oRequestBody.m_strCallDirection));
		l_oSqlRequest.param["call_direction"] = l_oRequestBody.m_strCallDirection;
	}
	std::string l_stRelAlarmId;
	if (!l_oRequestBody.m_strRelAlarmID.empty())
	{
		l_stRelAlarmId = l_oRequestBody.m_strRelAlarmID;
		l_oSqlRequest.param["rel_alarm_id"] = l_oRequestBody.m_strRelAlarmID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strRelAlarmID);
		if (strTime != "" && (l_oRequestBody.m_strStartTime.empty() && l_oRequestBody.m_strEndTime.empty()))
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}

	if (!l_strAlarmId.empty() && !l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s' or c.rel_alarm_id = '%s'", l_strAlarmId.c_str(), l_stRelAlarmId.c_str());
	}
	else if (!l_strAlarmId.empty() && l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s'", l_strAlarmId.c_str());
	}
	else if (l_strAlarmId.empty() && !l_stRelAlarmId.empty())
	{
		l_oSqlRequest.param["new_alarm_id"] = m_pString->Format("c.rel_alarm_id = '%s'", l_stRelAlarmId.c_str());
	}
	std::string l_autoUrgeCallerNum;
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum))
	{
		l_oSqlRequest.param["autourge_caller"] = l_autoUrgeCallerNum;
	}

	//std::string l_strQueryCondition;
	//BuildQueryCondition(l_oRequestBody, l_strQueryCondition);
	/*if (!l_strQueryCondition.empty())
	{
		l_strCallEventQuerySQL.append(" where ").append(l_strQueryCondition).append(" order by c.callref_id desc ");
	}*/
	l_oSqlRequest.param["orderby"] = "c.create_time desc";

	PROTOCOL::CCallEventSearchRespond l_oRespond;
	//	GenRespondHeader("callevent_search_respond", l_pRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_pRequest.m_oHeader.m_strMsgId;
	l_oRespond.m_oBody.m_strPageIndex = l_pRequest.m_oBody.m_strPageIndex;
	l_oRespond.m_oBody.m_strAllCount = "0";
	l_oRespond.m_oBody.m_strCount = "0";

	if (l_pRequest.m_oBody.m_strPageSize != "0")
	{
		int iPageSize = std::atoi(l_pRequest.m_oBody.m_strPageSize.c_str());
		int iIndex = std::atoi(l_pRequest.m_oBody.m_strPageIndex.c_str());

		if (iIndex < 1) iIndex = 1;
		iIndex = (iIndex - 1) * iPageSize;
		/*std::string l_strPageLimit = str(boost::format(" limit %1% offset %2%") % iPageSize % (iPageSize * (iIndex - 1)));
		l_strCallEventQuerySQL.append(l_strPageLimit);*/
		l_oSqlRequest.param["limit"] = std::to_string(iPageSize);
		l_oSqlRequest.param["offset"] = std::to_string(iIndex);
	}
	if (l_pRequest.m_oBody.m_strIsInnerCall.compare("1") == 0)
	{
		l_oSqlRequest.param["is_innercall"] = l_pRequest.m_oHeader.m_strSeatNo;
	}

	boost::thread *l_oThreadQueryAlarmCount;
	if(cur_type == CALL_QUERY_COMMON || cur_type == CALL_QUERY_WITH_ALARM)
		l_oThreadQueryAlarmCount = new boost::thread(boost::bind(&CBusinessImpl::QueryCallAllCount, this, l_oSqlRequest, l_pRequest.m_oHeader.m_strMsgId));

	DataBase::IResultSetPtr	l_pRSet = m_pDBConn->Exec(l_oSqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql [%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "query callevent failed";

		ICC_LOG_ERROR(m_pLog, "query callevent info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	int nCount = 0;
	if (l_pRSet->RecordSize() > 0)
	{
		ICC_LOG_DEBUG(m_pLog, "[%s] callevent info searched,begin to build the respond msg body.", l_oRespond.m_oBody.m_strAllCount.c_str());

		while (l_pRSet->Next())
		{
			nCount++;
			PROTOCOL::CCallEventSearchRespond::CData l_oCallEventInfo;
			l_oCallEventInfo.m_strCallrefID = l_pRSet->GetValue("callref_id");
			l_oCallEventInfo.m_strAcd = l_pRSet->GetValue("acd");
			l_oCallEventInfo.m_strCallerID = l_pRSet->GetValue("caller_id");
			l_oCallEventInfo.m_strCalledID = l_pRSet->GetValue("called_id");
			l_oCallEventInfo.m_strCallDirection = l_pRSet->GetValue("call_direction");
			l_oCallEventInfo.m_strCallTime = l_pRSet->GetValue("call_time");
			l_oCallEventInfo.m_strTalkTime = l_pRSet->GetValue("talk_time");
			l_oCallEventInfo.m_strTalkTimeLen = l_pRSet->GetValue("talk_time_len");
			l_oCallEventInfo.m_strRingTimeLen = l_pRSet->GetValue("ring_time_len");
			l_oCallEventInfo.m_strHangupType = l_pRSet->GetValue("hangup_type");
			l_oCallEventInfo.m_strSwitchType = l_pRSet->GetValue("switch_type");
			l_oCallEventInfo.m_strCaseID = l_pRSet->GetValue("case_id");
			l_oCallEventInfo.m_strReceiptCode = l_pRSet->GetValue("receipt_code");
			l_oCallEventInfo.m_strReceiptName = l_pRSet->GetValue("receipt_name");
			l_oCallEventInfo.m_strRelAlarmID = l_pRSet->GetValue("rel_alarm_id");
			l_oCallEventInfo.m_strIsCallBack = l_pRSet->GetValue("is_callback") == "true" ? "0" : "1";
			l_oCallEventInfo.m_strRecordFileId = l_pRSet->GetValue("record_id");

			//if (l_oCallEventInfo.m_strCallDirection == "out" && l_oCallEventInfo.m_strTalkTimeLen.empty() && l_oCallEventInfo.m_strRingTimeLen.empty() && l_oCallEventInfo.m_strCalledID.empty())
			//{
			//	continue;
			//}

			if (!l_oRequestBody.m_strSeatNo.empty())
			{//m_strSeatNo非空，早释电话，需要返回运营商信息
				l_oCallEventInfo.m_strName = l_pRSet->GetValue("name");
				l_oCallEventInfo.m_strAddress = l_pRSet->GetValue("address");
				l_oCallEventInfo.m_strProvince = l_pRSet->GetValue("province");
				l_oCallEventInfo.m_strCity = l_pRSet->GetValue("city");
				l_oCallEventInfo.m_strOperator = l_pRSet->GetValue("operator");
			}

			if (m_pString->ToInt(l_oRequestBody.m_strExtraBusinessScene) == early_release_query)
			{
				if (l_mapAgentPhone.find(l_oCallEventInfo.m_strCallerID) != l_mapAgentPhone.end())
					continue;
			}

			if (l_oCallEventInfo.m_strCallTime.empty())
			{
				l_oCallEventInfo.m_strCallTime = str(boost::format("%1%-%2%-%3% %4%:%5%:%6%")
					% l_oCallEventInfo.m_strCallrefID.substr(0, 4)
					% l_oCallEventInfo.m_strCallrefID.substr(4, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(6, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(8, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(10, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(12, 2));
			}
			if (l_oCallEventInfo.m_strHangupType == "release")
			{
				if (l_oCallEventInfo.m_strRingTimeLen.empty())
				{
					l_oCallEventInfo.m_strReleaseType = "wait";
				}
				else {
					l_oCallEventInfo.m_strReleaseType = "ring";
				}
			}
 			l_oRespond.m_oBody.m_vecData.push_back(l_oCallEventInfo);
		}

		l_oRespond.m_oBody.m_strCount = std::to_string(l_oRespond.m_oBody.m_vecData.size());
		ICC_LOG_DEBUG(m_pLog, "respond msg body has built.");
	}
	else
	{
		//l_oRespond.m_oBody.m_strCount = "0";
		ICC_LOG_DEBUG(m_pLog, "[0] callevent info searched.");
	}

	if (cur_type == CALL_QUERY_COMMON||cur_type==CALL_QUERY_WITH_ALARM)
	{
		ICC_LOG_DEBUG(m_pLog, "wait for the returning of call_count_thread");
		l_oThreadQueryAlarmCount->join();//等话务数量查询线程返回
		ICC_LOG_DEBUG(m_pLog, "call_count_thread returned");
		DataBase::IResultSetPtr l_pAlarmAllCount;
		l_pAlarmAllCount = m_mapSelectCallCount[l_pRequest.m_oHeader.m_strMsgId];

		//设置响应消息的警情总数
		if (!l_pAlarmAllCount || !l_pAlarmAllCount->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "query call count error:[%s]\nsql[%s],", l_pAlarmAllCount->GetErrorMsg().c_str(), l_pAlarmAllCount->GetSQL().c_str());
			ICC_LOG_DEBUG(m_pLog, "set call count = RecordSize[%d]", l_pRSet->RecordSize());
			l_oRespond.m_oBody.m_strAllCount = l_pRSet->RecordSize();
		}
		if(l_pAlarmAllCount->Next())
		{
			l_oRespond.m_oBody.m_strAllCount = l_pAlarmAllCount->GetValue("count"); //数据库中满足查询条件的警情总数
			ICC_LOG_DEBUG(m_pLog, "call all count [%s]", l_oRespond.m_oBody.m_strAllCount.c_str());
		}

		{
			std::lock_guard<std::mutex> lock(m_mutexSelectCallCount);
			m_mapSelectCallCount.erase(l_pRequest.m_oHeader.m_strMsgId);
		}

		delete l_oThreadQueryAlarmCount;
	}

	//SendRespondMsg(l_oRespond, p_pNotifiReqeust);
	if (nCount == std::atoi(l_oRespond.m_oBody.m_strAllCount.c_str()))
	{
		l_oRespond.m_oBody.m_strAllCount = l_oRespond.m_oBody.m_strCount;
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_LOWDEBUG(m_pLog, "response:[%s]", l_strMessage.c_str());
	ICC_LOG_DEBUG(m_pLog, "response,msgID:[%s],allCount:%s", l_oRespond.m_oHeader.m_strMsgId.c_str(), l_oRespond.m_oBody.m_strAllCount.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}


//历史话务查询请求响应函数
//1.录音查询		2.早释电话查询-带运营商和归属地信息
//需将座席信息注册到系统（设置-添加座席,icc_t_seat表），否则无法查询该座席的信息
void ICC::CBusinessImpl::OnNotifiCallEventSearchCallorRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "request msg is empty");
		return;
	}

	PROTOCOL::CCallEventSearchRequest l_pRequest;
	if (!l_pRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	bool l_bPagingFlag = true;
	std::string l_strCallEventQuerySQL;
	PROTOCOL::CCallEventSearchRequest::CBody& l_oRequestBody = l_pRequest.m_oBody;
	DataBase::SQLRequest l_oSqlRequest;

	l_oSqlRequest.sql_id = "seach_callevent_callor_filter";

	if (!l_oRequestBody.m_strDeptCode.empty() && l_oRequestBody.m_strHangupType != "blackcall")
	{
		l_oSqlRequest.param["dept_code"] = l_oRequestBody.m_strDeptCode;
	}

	std::string l_strCallerNumber = m_pString->Trim(l_oRequestBody.m_strCallerID);
	if (!l_strCallerNumber.empty())
	{
		l_oSqlRequest.param["caller_id"] = l_strCallerNumber;
	}

	std::string l_strCalledNumber = m_pString->Trim(l_oRequestBody.m_strCalledID);
	if (!l_strCalledNumber.empty())
	{
		l_oSqlRequest.param["called_id"] = l_strCalledNumber;
	}


	std::string l_strPhoneNumber = m_pString->Trim(l_oRequestBody.m_strPhoneNo);
	if (!l_strPhoneNumber.empty())
	{
		l_oSqlRequest.param["phone_no"] = l_strPhoneNumber;
	}
	std::string l_strBeginTime(l_oRequestBody.m_strStartTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strStartTime.empty())
	{
		l_strBeginTime = "1900-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	l_oSqlRequest.param["create_time_begin"] = l_strBeginTime;

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}
	l_oSqlRequest.param["create_time_end"] = l_strEndTime;


	if (!l_oRequestBody.m_strHangupType.empty())
	{

		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append("(" + BuildMultiConditions("c.hangup_type", "=", l_oRequestBody.m_strHangupType) + ")");
		l_oSqlRequest.param["hangup_type"] = BuildMultiConditions("c.hangup_type", " = ", l_oRequestBody.m_strHangupType);
	}
	if (!l_oRequestBody.m_strReceiptCodeOrName.empty())
	{
		/*l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(a.receipt_code like '%%%1%%%' or "
			"a.receipt_name like '%%%1%%%' or cb.receipt_name like '%%%1%%%')") % l_oRequestBody.m_strReceiptCodeOrName));*/
		l_oSqlRequest.param["receipt_code_or_name"] = l_oRequestBody.m_strReceiptCodeOrName;
	}
	if (!l_oRequestBody.m_strCaseID.empty())
	{
		l_oSqlRequest.param["alarm_id"] = l_oRequestBody.m_strCaseID;

		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strCaseID);
		if (strTime != "")
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}

		l_bPagingFlag = false;
	}

	if (!l_oRequestBody.m_strCallrefID.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.callref_id = '%1%')") % l_oRequestBody.m_strCallrefID));
		l_oSqlRequest.param["callref_id"] = l_oRequestBody.m_strCallrefID;

		std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequestBody.m_strCallrefID);
		if (strTime != "")
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
		l_bPagingFlag = false;
	}
	if (!l_oRequestBody.m_strCallDirection.empty())
	{
		//l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		//l_strCondition.append(str(boost::format("(c.call_direction = '%1%')") % l_oRequestBody.m_strCallDirection));
		l_oSqlRequest.param["call_direction"] = l_oRequestBody.m_strCallDirection;
	}

	if (!l_oRequestBody.m_strRelAlarmID.empty())
	{
		l_oSqlRequest.param["rel_alarm_id"] = l_oRequestBody.m_strRelAlarmID;

		std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequestBody.m_strRelAlarmID);
		if (strTime != "")
		{
			l_oSqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_oSqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}
	//std::string l_strQueryCondition;
	//BuildQueryCondition(l_oRequestBody, l_strQueryCondition);
	/*if (!l_strQueryCondition.empty())
	{
		l_strCallEventQuerySQL.append(" where ").append(l_strQueryCondition).append(" order by c.callref_id desc ");
	}*/
	l_oSqlRequest.param["orderby"] = "c.callref_id desc";

	PROTOCOL::CCallEventSearchRespond l_oRespond;
	//	GenRespondHeader("callevent_search_respond", l_pRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_pRequest.m_oHeader.m_strMsgId;
	l_oRespond.m_oBody.m_strPageIndex = l_pRequest.m_oBody.m_strPageIndex;
	l_oRespond.m_oBody.m_strAllCount = "0";
	l_oRespond.m_oBody.m_strCount = "0";

	if (l_bPagingFlag)
	{
		if (l_pRequest.m_oBody.m_strPageSize != "0")
		{
			int iPageSize = std::atoi(l_pRequest.m_oBody.m_strPageSize.c_str());
			int iIndex = std::atoi(l_pRequest.m_oBody.m_strPageIndex.c_str());

			if (iIndex < 1) iIndex = 1;
			iIndex = (iIndex - 1) * iPageSize;
			/*std::string l_strPageLimit = str(boost::format(" limit %1% offset %2%") % iPageSize % (iPageSize * (iIndex - 1)));
			l_strCallEventQuerySQL.append(l_strPageLimit);*/
			l_oSqlRequest.param["limit"] = std::to_string(iPageSize);
			l_oSqlRequest.param["offset"] = std::to_string(iIndex);
		}
	}

	DataBase::IResultSetPtr	l_pRSet = m_pDBConn->Exec(l_oSqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql [%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "query callevent failed";

		ICC_LOG_ERROR(m_pLog, "query callevent info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	int nCount = 0;
	if (l_pRSet->RecordSize() > 0)
	{
		l_oRespond.m_oBody.m_strAllCount = l_pRSet->GetValue(0, "count");
		ICC_LOG_DEBUG(m_pLog, "[%s] callevent info searched,begin to build the respond msg body.", l_oRespond.m_oBody.m_strAllCount.c_str());

		while (l_pRSet->Next())
		{
			nCount++;
			PROTOCOL::CCallEventSearchRespond::CData l_oCallEventInfo;
			l_oCallEventInfo.m_strCallrefID = l_pRSet->GetValue("callref_id");
			l_oCallEventInfo.m_strAcd = l_pRSet->GetValue("acd");
			l_oCallEventInfo.m_strCallerID = l_pRSet->GetValue("caller_id");
			l_oCallEventInfo.m_strCalledID = l_pRSet->GetValue("called_id");
			l_oCallEventInfo.m_strCallDirection = l_pRSet->GetValue("call_direction");
			l_oCallEventInfo.m_strCallTime = l_pRSet->GetValue("call_time");
			l_oCallEventInfo.m_strTalkTimeLen = l_pRSet->GetValue("talk_time_len");
			l_oCallEventInfo.m_strRingTimeLen = l_pRSet->GetValue("ring_time_len");
			l_oCallEventInfo.m_strHangupType = l_pRSet->GetValue("hangup_type");
			l_oCallEventInfo.m_strSwitchType = l_pRSet->GetValue("switch_type");
			l_oCallEventInfo.m_strCaseID = l_pRSet->GetValue("case_id");
			l_oCallEventInfo.m_strReceiptCode = l_pRSet->GetValue("receipt_code");
			l_oCallEventInfo.m_strReceiptName = l_pRSet->GetValue("receipt_name");
			l_oCallEventInfo.m_strRelAlarmID = l_pRSet->GetValue("rel_alarm_id");
			l_oCallEventInfo.m_strIsCallBack = l_pRSet->GetValue("is_callback") == "true" ? "0" : "1";

			if (!l_oRequestBody.m_strSeatNo.empty())
			{//m_strSeatNo非空，早释电话，需要返回运营商信息
				l_oCallEventInfo.m_strName = l_pRSet->GetValue("name");
				l_oCallEventInfo.m_strAddress = l_pRSet->GetValue("address");
				l_oCallEventInfo.m_strProvince = l_pRSet->GetValue("province");
				l_oCallEventInfo.m_strCity = l_pRSet->GetValue("city");
				l_oCallEventInfo.m_strOperator = l_pRSet->GetValue("operator");
			}

			if (l_oCallEventInfo.m_strCallTime.empty())
			{
				l_oCallEventInfo.m_strCallTime = str(boost::format("%1%-%2%-%3% %4%:%5%:%6%")
					% l_oCallEventInfo.m_strCallrefID.substr(0, 4)
					% l_oCallEventInfo.m_strCallrefID.substr(4, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(6, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(8, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(10, 2)
					% l_oCallEventInfo.m_strCallrefID.substr(12, 2));
			}
			l_oRespond.m_oBody.m_vecData.push_back(l_oCallEventInfo);
		}

		l_oRespond.m_oBody.m_strCount = std::to_string(l_oRespond.m_oBody.m_vecData.size());
		ICC_LOG_DEBUG(m_pLog, "respond msg body has built.");
	}
	else
	{
		//l_oRespond.m_oBody.m_strCount = "0";
		ICC_LOG_DEBUG(m_pLog, "[0] callevent info searched.");
	}

	//SendRespondMsg(l_oRespond, p_pNotifiReqeust);
	if (nCount == std::atoi(l_oRespond.m_oBody.m_strAllCount.c_str()))
	{
		l_oRespond.m_oBody.m_strAllCount = l_oRespond.m_oBody.m_strCount;
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	//ICC_LOG_DEBUG(m_pLog, "response:[%s]", l_strMessage.c_str());
	ICC_LOG_DEBUG(m_pLog, "response,msgID:[%s],allCount:%s", l_oRespond.m_oHeader.m_strMsgId.c_str(), l_oRespond.m_oBody.m_strAllCount.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}

//带归属地信息的历史话务查询，主要用于客户端早释电话窗口展示来电归属地等信息
void ICC::CBusinessImpl::OnNotifiCallEventSearchExRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	std::string l_strRecvMsg(p_pNotifiReqeust->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv msg:[%s]", l_strRecvMsg.c_str());
	if (l_strRecvMsg.empty())
	{
		ICC_LOG_WARNING(m_pLog, "request msg is empty");
		return;
	}

	PROTOCOL::CCallEventSearchExRequest l_pRequest;
	if (!l_pRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg failed.");
		return;
	}

	PROTOCOL::CCallEventSearchExRespond l_oRespond;
	//GenRespondHeader("callevent_search_ex_respond", l_pRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oHeader.m_strMsgId = l_pRequest.m_oHeader.m_strMsgId;
	l_oRespond.m_oBody.m_strPageIndex = l_pRequest.m_oBody.m_strPageIndex;
	l_oRespond.m_oBody.m_strAllCount = "0";
	l_oRespond.m_oBody.m_strCount = "0";

	DataBase::SQLRequest l_oCalleventSearchExSQL;
	l_oCalleventSearchExSQL.sql_id = "call_event_search_ex_request";

	if (l_pRequest.m_oBody.m_strPageSize != "0")
	{
		int iPageSize = std::atoi(l_pRequest.m_oBody.m_strPageSize.c_str());
		int iIndex = std::atoi(l_pRequest.m_oBody.m_strPageIndex.c_str());

		if (iIndex < 1) iIndex = 1;

		l_oCalleventSearchExSQL.param["limit"] = std::to_string(iPageSize);
		l_oCalleventSearchExSQL.param["offset"] = std::to_string(iPageSize * (iIndex - 1));
	}
	if (!l_pRequest.m_oBody.m_strAcd.empty())
	{
		l_oCalleventSearchExSQL.param["acd"] = l_pRequest.m_oBody.m_strAcd;
	}
	if (!l_pRequest.m_oBody.m_strSeatNo.empty())
	{
		l_oCalleventSearchExSQL.param["seat_no"] = l_pRequest.m_oBody.m_strSeatNo;
	}

	DataBase::IResultSetPtr	l_pRSet = m_pDBConn->Exec(l_oCalleventSearchExSQL, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "query callevent failed";

		ICC_LOG_ERROR(m_pLog, "query callevent info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
	}

	if (l_pRSet->RecordSize() > 0)
	{
		l_oRespond.m_oBody.m_strAllCount = l_pRSet->GetValue(0, "count");
		while (l_pRSet->Next())
		{
			PROTOCOL::CCallEventSearchExRespond::CData l_oCallEventInfo;
			l_oCallEventInfo.m_strCallrefID = l_pRSet->GetValue("callref_id");
			l_oCallEventInfo.m_strAcd = l_pRSet->GetValue("acd");
			l_oCallEventInfo.m_strCallerID = l_pRSet->GetValue("caller_id");
			l_oCallEventInfo.m_strCalledID = l_pRSet->GetValue("called_id");
			l_oCallEventInfo.m_strCallDirection = l_pRSet->GetValue("call_direction");
			l_oCallEventInfo.m_strInComingTime = l_pRSet->GetValue("incoming_time");
			l_oCallEventInfo.m_strDialTime = l_pRSet->GetValue("dial_time");
			l_oCallEventInfo.m_strTalkTime = l_pRSet->GetValue("talk_time");
			l_oCallEventInfo.m_strIsCallBack = l_pRSet->GetValue("is_callback") == "true" ? "0" : "1";
			l_oCallEventInfo.m_strHangupTime = l_pRSet->GetValue("hangup_time");
			l_oCallEventInfo.m_strHangupType = l_pRSet->GetValue("hangup_type");
			l_oCallEventInfo.m_strName = l_pRSet->GetValue("name");
			l_oCallEventInfo.m_strAddress = l_pRSet->GetValue("address");
			l_oCallEventInfo.m_strProvince = l_pRSet->GetValue("province");
			l_oCallEventInfo.m_strCity = l_pRSet->GetValue("city");
			l_oCallEventInfo.m_strOperator = l_pRSet->GetValue("operator");
			l_oRespond.m_oBody.m_vecData.push_back(l_oCallEventInfo);
		}

		l_oRespond.m_oBody.m_strCount = std::to_string(l_oRespond.m_oBody.m_vecData.size());
	}
	else
	{
		//l_oRespond.m_oBody.m_strCount = "0";
		ICC_LOG_DEBUG(m_pLog, "[0] callevent info searched.");
	}
	SendRespondMsg(l_oRespond, p_pNotifiReqeust);
}

//早逝电话查询响应函数
void CBusinessImpl::OnNotifiSearchReleaseCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	ICC_LOG_LOWDEBUG(m_pLog, "receive message:[%s]", p_pNotifiReqeust->GetMessages().c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSearchReleaseCount l_oSearchReleaseRequest;
	PROTOCOL::CSearchReleaseCount l_oSearchReleaseRespond;
	l_oSearchReleaseRespond.m_oBody.m_strSum = "0";
	if (!l_oSearchReleaseRequest.ParseString(p_pNotifiReqeust->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "SearchReleaseRequest is Invilid [%s]", p_pNotifiReqeust->GetMessages().c_str());
		return;
	}

	GenRespondHeader("search_release_count_respond", l_oSearchReleaseRequest.m_oHeader, l_oSearchReleaseRespond.m_oHeader);

	unsigned int l_iPageSize = std::atoi(l_oSearchReleaseRequest.m_oBody.m_oData.m_strPageSize.c_str());
	unsigned int l_iPageIndex = std::atoi(l_oSearchReleaseRequest.m_oBody.m_oData.m_strPageIndex.c_str());
	l_iPageIndex = l_iPageIndex > 0 ? l_iPageIndex : 1;
	unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
	unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//索引从0开始

	//早逝电话数查询
	DataBase::SQLRequest l_oQueryCount;
	int l_iReleaseSum = 0;
	int l_iRecordSum = 0;
	l_oQueryCount.sql_id = "select_icc_t_callevent_num";
	l_oQueryCount.param.insert(std::pair<std::string, std::string>("start_time", l_oSearchReleaseRequest.m_oBody.m_oData.m_strBeginTime));
	l_oQueryCount.param.insert(std::pair<std::string, std::string>("end_time", l_oSearchReleaseRequest.m_oBody.m_oData.m_strEndTime));
	l_oQueryCount.param.insert(std::pair<std::string, std::string>("dept_code", l_oSearchReleaseRequest.m_oBody.m_oData.m_strDeptCode));
	l_oQueryCount.param.insert(std::pair<std::string, std::string>("status", "release"));

	if (l_iPageSize != 0) //page_size为0时不分页
	{
		l_oQueryCount.param["limit"] = l_oSearchReleaseRequest.m_oBody.m_oData.m_strPageSize;	//只取一页
		l_oQueryCount.param["offset"] = std::to_string(l_iRequestIdxBegin);
	}

	DataBase::IResultSetPtr l_pQuerySumPtr = m_pDBConn->Exec(l_oQueryCount, true);
	ICC_LOG_LOWDEBUG(m_pLog, "sql:[%s]", l_pQuerySumPtr->GetSQL().c_str());
	if (!l_pQuerySumPtr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select_icc_t_callevent_num operation failed [%s]", l_pQuerySumPtr->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pQuerySumPtr->RecordSize() > 0)
		{
			l_oSearchReleaseRespond.m_oBody.m_strSum = l_pQuerySumPtr->GetValue(0, "call_sum");
			l_oSearchReleaseRespond.m_oBody.m_strCount = l_pQuerySumPtr->GetValue(0, "record_num");
			std::map<std::string, int> l_mapCallRef;
			while (l_pQuerySumPtr->Next())
			{
				std::string l_strUserCode = l_pQuerySumPtr->GetValue("client_name");
				if (l_strUserCode.empty())
				{
					l_strUserCode = l_pQuerySumPtr->GetValue("called_id");
				}
				if (l_mapCallRef.find(l_strUserCode) == l_mapCallRef.end())
				{
					l_mapCallRef[l_strUserCode] = std::atoi(l_pQuerySumPtr->GetValue("call_num").c_str());
				}
				else
				{
					l_mapCallRef[l_strUserCode] = l_mapCallRef[l_strUserCode] + std::atoi(l_pQuerySumPtr->GetValue("call_num").c_str());
				}
			}
			PROTOCOL::CSearchReleaseCount::CBody::CData l_oTempData;
			for (auto it = l_mapCallRef.begin(); it != l_mapCallRef.end(); ++it)
			{
				l_oTempData.m_strUserCode = it->first;
				l_oTempData.m_strReleaseCount = std::to_string(it->second);
				l_oSearchReleaseRespond.m_oBody.m_vecData.push_back(l_oTempData);
			}
		}
		else
		{
			l_oSearchReleaseRespond.m_oBody.m_strSum = "0";
			l_oSearchReleaseRespond.m_oBody.m_strCount = "0";
		}
	}
	std::string l_strMessage = l_oSearchReleaseRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	ICC_LOG_LOWDEBUG(m_pLog, "response:[%s]", l_strMessage.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}
//在线时长查询响应函数
void CBusinessImpl::OnNotifiSearchOnlineCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", p_pNotifiReqeust->GetMessages().c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSearchOnlineCount l_oSearchOnLineRequest;
	PROTOCOL::CSearchOnlineCount l_oSearchOnLineRespond;
	if (!l_oSearchOnLineRequest.ParseString(p_pNotifiReqeust->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "SearchOnlineCountRequest is Invilid [%s]", p_pNotifiReqeust->GetMessages().c_str());
		return;
	}
	GenRespondHeader("search_onlie_count_respond", l_oSearchOnLineRequest.m_oHeader, l_oSearchOnLineRespond.m_oHeader);
	//上下线总时长查询
	DataBase::SQLRequest l_oQuerySum;
	int l_iRecord = 0;
	l_oQuerySum.sql_id = "select_icc_t_client_on_off_sum";
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("on_time", l_oSearchOnLineRequest.m_oBody.m_oData.m_strBeginTime));
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("off_time", l_oSearchOnLineRequest.m_oBody.m_oData.m_strEndTime));
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("groupby", "client_name"));

	DataBase::IResultSetPtr l_pQuerySumPtr = m_pDBConn->Exec(l_oQuerySum, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pQuerySumPtr->GetSQL().c_str());
	if (!l_pQuerySumPtr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select OnLine operation failed");
	}
	else
	{
		float l_dDurationSum = (float)m_pString->ToInt(l_pQuerySumPtr->GetValue(0, "duration_sum")) / 60;
		l_oSearchOnLineRespond.m_oBody.m_strSum = str(boost::format("%.1f") % l_dDurationSum);
		l_oSearchOnLineRespond.m_oBody.m_strCount = l_pQuerySumPtr->GetValue(0, "record_num");
		l_iRecord = m_pString->ToInt(l_pQuerySumPtr->GetValue(0, "record_num"));
	}
	//进行上下线时长查询
	DataBase::SQLRequest l_oQuery;
	l_oQuery.sql_id = "select_icc_t_client_on_off_duration_sum";
	l_oQuery.param.insert(std::pair<std::string, std::string>("on_time", l_oSearchOnLineRequest.m_oBody.m_oData.m_strBeginTime));
	l_oQuery.param.insert(std::pair<std::string, std::string>("off_time", l_oSearchOnLineRequest.m_oBody.m_oData.m_strEndTime));
	l_oQuery.param.insert(std::pair<std::string, std::string>("groupby", "client_name"));
	DataBase::IResultSetPtr l_pQueryPtr = m_pDBConn->Exec(l_oQuery, true);
	if (!l_pQueryPtr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select OnLine operation failed");
	}
	else
	{
		//分页处理
		unsigned int  l_PageSize = m_pString->ToInt(l_oSearchOnLineRequest.m_oBody.m_oData.m_strPageSize);
		unsigned int  l_PageIndex = m_pString->ToInt(l_oSearchOnLineRequest.m_oBody.m_oData.m_strPageIndex);
		unsigned int  l_PageNum = 0;
		unsigned int  l_CurrentPage = 0;
		if (l_PageSize == 0 && l_PageIndex == 0)
		{
			for (unsigned int i = 0; i < l_pQueryPtr->RecordSize(); i++)
			{
				PROTOCOL::CSearchOnlineCount::CBody::CData l_oTempData;
				float l_iOnlineSum = (float)m_pString->ToInt(l_pQueryPtr->GetValue(i, "online_sum")) / 60;
				l_oTempData.m_strOnlineSum = str(boost::format("%.1f") % l_iOnlineSum);
				l_oTempData.m_strUserCode = l_pQueryPtr->GetValue(i, "client_name");
				l_oSearchOnLineRespond.m_oBody.m_vecData.push_back(l_oTempData);
			}
		}
		else
		{
			if (l_PageSize > 0)
			{
				if (l_iRecord % l_PageSize == 0)
				{
					l_PageNum = l_iRecord / l_PageSize;
				}
				else
				{
					l_PageNum = (l_iRecord / l_PageSize) + 1;
				}
			}

			for (unsigned int i = 0; i < l_PageNum; i++)
			{
				l_CurrentPage++;
				if (l_CurrentPage == l_PageIndex)
				{
					unsigned int  l_CuttentRow = (l_CurrentPage - 1) * l_PageSize;
					l_pQueryPtr->SetCurRow(l_CuttentRow);
					unsigned int  l_ResultCount = l_iRecord - (l_CurrentPage - 1) * l_PageSize;
					unsigned int  l_Loop = 0;
					l_Loop = (l_ResultCount <= l_PageSize) ? l_ResultCount : l_PageSize;
					for (unsigned int i = l_CuttentRow; i < l_CuttentRow + l_Loop; i++)
					{
						PROTOCOL::CSearchOnlineCount::CBody::CData l_oTempData;
						if (l_pQueryPtr->GetValue(i, "client_name").empty())
						{
							continue;
						}
						else
						{
							float l_iOnlineSum = (float)m_pString->ToInt(l_pQueryPtr->GetValue(i, "online_sum")) / 60;
							l_oTempData.m_strOnlineSum = str(boost::format("%.1f") % l_iOnlineSum);
							l_oTempData.m_strUserCode = l_pQueryPtr->GetValue(i, "client_name");
							l_oSearchOnLineRespond.m_oBody.m_vecData.push_back(l_oTempData);
						}
					}
					break;
				}
			}
		}
		std::string l_strMessage = l_oSearchOnLineRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		ICC_LOG_INFO(m_pLog, "response:[%s]", l_strMessage.c_str());
		p_pNotifiReqeust->Response(l_strMessage);
	}
}
//离席时长查询响应函数
void CBusinessImpl::OnNotifiSearchLogoutCountRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	ICC_LOG_DEBUG(m_pLog, "recv msg[%s]", p_pNotifiReqeust->GetMessages().c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSearchLogoutCount l_oSearchLogoutRequest;
	PROTOCOL::CSearchLogoutCount l_oSearchLogoutRespond;
	if (!l_oSearchLogoutRequest.ParseString(p_pNotifiReqeust->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "SearchLogoutRequest is Invilid [%s]", p_pNotifiReqeust->GetMessages().c_str());
		return;
	}
	GenRespondHeader("search_logout_count_respond", l_oSearchLogoutRequest.m_oHeader, l_oSearchLogoutRespond.m_oHeader);
	//离席总时长查询
	DataBase::SQLRequest l_oQuerySum;
	int l_iRecord = 0;
	l_oQuerySum.sql_id = "select_icc_t_client_in_out_sum";
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("login_time", l_oSearchLogoutRequest.m_oBody.m_oData.m_strBeginTime));
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("logout_time", l_oSearchLogoutRequest.m_oBody.m_oData.m_strEndTime));
	l_oQuerySum.param.insert(std::pair<std::string, std::string>("groupby", "client_name"));

	DataBase::IResultSetPtr l_pQuerySumPtr = m_pDBConn->Exec(l_oQuerySum, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pQuerySumPtr->GetSQL().c_str());
	if (!l_pQuerySumPtr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select in_out operation failed[%s]", l_pQuerySumPtr->GetErrorMsg().c_str());
	}
	else
	{
		float l_fDurationSum = (float)m_pString->ToInt(l_pQuerySumPtr->GetValue(0, "duration_sum")) / 60;
		l_oSearchLogoutRespond.m_oBody.m_strSum = str(boost::format("%.1f") % l_fDurationSum);
		l_oSearchLogoutRespond.m_oBody.m_strCount = l_pQuerySumPtr->GetValue(0, "record_num");
		l_iRecord = m_pString->ToInt(l_pQuerySumPtr->GetValue(0, "record_num"));
	}

	//进行离席时长查询
	DataBase::SQLRequest l_oQuery;
	l_oQuery.sql_id = "select_icc_t_client_in_out_count";
	l_oQuery.param.insert(std::pair<std::string, std::string>("login_time", l_oSearchLogoutRequest.m_oBody.m_oData.m_strBeginTime));
	l_oQuery.param.insert(std::pair<std::string, std::string>("logout_time", l_oSearchLogoutRequest.m_oBody.m_oData.m_strEndTime));
	l_oQuery.param.insert(std::pair<std::string, std::string>("groupby", "client_name"));

	DataBase::IResultSetPtr l_pQueryPtr = m_pDBConn->Exec(l_oQuery, true);
	int l_count = 0;
	if (!l_pQueryPtr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select OnLine operation failed");
	}
	else
	{
		//分页处理
		int l_PageSize = m_pString->ToInt(l_oSearchLogoutRequest.m_oBody.m_oData.m_strPageSize);
		int l_PageIndex = m_pString->ToInt(l_oSearchLogoutRequest.m_oBody.m_oData.m_strPageIndex);
		int l_PageNum = 0;
		int l_CurrentPage = 0;
		if (l_PageSize == 0 && l_PageIndex == 0)
		{
			for (unsigned int i = 0; i < l_pQueryPtr->RecordSize(); i++)
			{
				PROTOCOL::CSearchLogoutCount::CBody::CData l_oTempData;
				float l_iOnlineSum = (float)m_pString->ToInt(l_pQueryPtr->GetValue(i, "logout_count")) / 60;
				l_oTempData.m_strLogoutSum = str(boost::format("%.1f") % l_iOnlineSum);
				l_oTempData.m_strUserCode = l_pQueryPtr->GetValue(i, "client_name");
				l_oSearchLogoutRespond.m_oBody.m_vecData.push_back(l_oTempData);
			}
		}
		else
		{
			if (l_PageSize > 0)
			{
				if (l_iRecord % l_PageSize == 0)
				{
					l_PageNum = l_iRecord / l_PageSize;
				}
				else
				{
					l_PageNum = (l_iRecord / l_PageSize) + 1;
				}
			}

			for (int i = 0; i < l_PageNum; i++)
			{
				l_CurrentPage++;
				if (l_CurrentPage == l_PageIndex)
				{
					unsigned int  l_CuttentRow = (l_CurrentPage - 1) * l_PageSize;
					l_pQueryPtr->SetCurRow(l_CuttentRow);
					int  l_ResultCount = l_iRecord - (l_CurrentPage - 1) * l_PageSize;
					unsigned int  l_Loop = 0;
					if (l_ResultCount <= l_PageSize)
					{
						l_Loop = l_ResultCount;
					}
					else
					{
						l_Loop = l_PageSize;
					}
					for (unsigned int i = l_CuttentRow; i < l_CuttentRow + l_Loop; i++)
					{
						PROTOCOL::CSearchLogoutCount::CBody::CData l_oTempData;
						if (l_pQueryPtr->GetValue(i, "client_name").empty())
						{
							continue;
						}
						else
						{
							float l_fLogoutSum = (float)m_pString->ToInt(l_pQueryPtr->GetValue(i, "logout_count")) / 60;
							l_oTempData.m_strLogoutSum = str(boost::format("%.1f") % l_fLogoutSum);
							l_oTempData.m_strUserCode = l_pQueryPtr->GetValue(i, "client_name");
							l_oSearchLogoutRespond.m_oBody.m_vecData.push_back(l_oTempData);
						}

					}
					break;
				}
			}
		}
		std::string l_strMessage = l_oSearchLogoutRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		ICC_LOG_INFO(m_pLog, "response:[%s]", l_strMessage.c_str());
		p_pNotifiReqeust->Response(l_strMessage);
	}
}

void CBusinessImpl::OnNotifiSearchClientInOutRequest(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	if (p_pNotifiReqeust)
	{
		std::string l_strMessage = p_pNotifiReqeust->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Recv SearchInOutRequest msg[%s]", l_strMessage.c_str());

		PROTOCOL::CGetInOutInfoRequest l_oClientInOutRequest;
		PROTOCOL::CGetInOutInfoRespond l_oClientInOutRespond;
		if (!l_oClientInOutRequest.ParseString(l_strMessage, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Error ParseString :[%s]", l_strMessage.c_str());
			return;
		}

		GenRespondHeader("search_client_inout_respond", l_oClientInOutRequest.m_oHeader, l_oClientInOutRespond.m_oHeader);

		int l_iPageSize = m_pString->ToInt(l_oClientInOutRequest.m_oBody.m_strPageSize);
		int l_iPageIndex = m_pString->ToInt(l_oClientInOutRequest.m_oBody.m_strPageIndex);
		//限制单页最大记录数，避免条消息过大引起JsonParser组件崩溃，m_iMaxPageSize从配置文件读取
		l_iPageSize = std::min(l_iPageSize, m_iMaxPageSize);

		if (l_iPageSize <= 0)
		{//检查客户端传入的page_size参数
			l_iPageSize = 10;
			ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_size=[%s],set to default value[%d]", l_oClientInOutRequest.m_oBody.m_strPageSize.c_str(), l_iPageSize);
		}
		if (l_iPageIndex < 1)
		{//检查客户端传入的page_index参数,此参数代表客户端界面上的第X页，从1开始
			l_iPageIndex = 1;
			ICC_LOG_WARNING(m_pLog, "Error Request Parameter page_index=[%s],set to default value[%d]", l_oClientInOutRequest.m_oBody.m_strPageSize.c_str(), l_iPageIndex);
		}

		//根据请求参数构造查询条件	
		unsigned int l_iRequestIdxEnd = l_iPageSize * l_iPageIndex;
		unsigned int l_iRequestIdxBegin = l_iRequestIdxEnd - l_iPageSize;//数据库结果集索引从0开始

		DataBase::SQLRequest l_oSqlCount;
		l_oSqlCount.sql_id = "select_icc_t_client_in_out_count_ex";
		l_oSqlCount.param["login_time"] = l_oClientInOutRequest.m_oBody.m_strStartTime;
		l_oSqlCount.param["login_time_ex"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
		l_oSqlCount.param["logout_time"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
		if (!l_oClientInOutRequest.m_oBody.m_strClientId.empty())
		{
			l_oSqlCount.param["client_id"] = l_oClientInOutRequest.m_oBody.m_strClientId;
		}
		if (!l_oClientInOutRequest.m_oBody.m_strClientName.empty())
		{
			l_oSqlCount.param["client_name"] = l_oClientInOutRequest.m_oBody.m_strClientName;
		}

		unsigned int l_nSumCount = 0;
		DataBase::IResultSetPtr l_oResultCount = m_pDBConn->Exec(l_oSqlCount);
		if (l_oResultCount->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "sql success:[%s]", l_oResultCount->GetSQL().c_str());

			if (l_oResultCount->Next())
			{
				l_nSumCount = m_pString->ToInt(l_oResultCount->GetValue("num"));
			}

			DataBase::SQLRequest l_oSql;
			l_oSql.sql_id = "select_icc_t_client_in_out_during";
			l_oSql.param["login_time"] = l_oClientInOutRequest.m_oBody.m_strStartTime;
			l_oSql.param["login_time_ex"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
			l_oSql.param["logout_time"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
			l_oSql.param["orderby"] = "create_time desc";
			l_oSql.param["limit"] = std::to_string(l_iPageSize);
			l_oSql.param["offset"] = std::to_string(l_iRequestIdxBegin);
			if (!l_oClientInOutRequest.m_oBody.m_strClientId.empty())
			{
				l_oSql.param["client_id"] = l_oClientInOutRequest.m_oBody.m_strClientId;
			}
			if (!l_oClientInOutRequest.m_oBody.m_strClientName.empty())
			{
				l_oSql.param["client_name"] = l_oClientInOutRequest.m_oBody.m_strClientName;
			}

			DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSql);
			if (l_oResult->IsValid())
			{
				ICC_LOG_DEBUG(m_pLog, "sql success:[%s]", l_oResult->GetSQL().c_str());

				while (l_oResult->Next())
				{
					PROTOCOL::CGetInOutInfoRespond::CBody::CData l_oData;
					l_oData.m_strClientID = l_oResult->GetValue("client_id");
					l_oData.m_strClientName = l_oResult->GetValue("client_name");
					l_oData.m_strDuration = l_oResult->GetValue("duration");
					l_oData.m_strInTime = l_oResult->GetValue("login_time");
					l_oData.m_strOutTime = l_oResult->GetValue("logout_time");
					l_oClientInOutRespond.m_oBody.m_vData.push_back(l_oData);
				}
			}
			else
			{
				l_nSumCount = 0;
				ICC_LOG_ERROR(m_pLog, "sql failed, sql:[%s], error msg:[%s]",
					l_oResult->GetSQL().c_str(), l_oResult->GetErrorMsg().c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "sql failed, sql:[%s], error msg:[%s]",
				l_oResultCount->GetSQL().c_str(), l_oResultCount->GetErrorMsg().c_str());
		}

		l_oClientInOutRespond.m_oBody.m_strCount = m_pString->Number(l_nSumCount);
		l_strMessage = l_oClientInOutRespond.ToString(ICCGetIJsonFactory()->CreateJson());
		p_pNotifiReqeust->Response(l_strMessage);

		ICC_LOG_DEBUG(m_pLog, "Send SearchInOutInfoRespond [%s]", l_strMessage.c_str());
	}
}

void ICC::CBusinessImpl::BuildQueryCondition(PROTOCOL::CCallEventSearchRequest::CBody& l_oRequestBody, std::string& l_strCondition)
{
	if (!l_oRequestBody.m_strDeptCode.empty() && l_oRequestBody.m_strHangupType != "blackcall")
	{
		//限制只能查询用户所在部门及其下属部门的话务
		l_strCondition = "(c.called_id = ANY(SELECT seat.no FROM icc_t_seat seat WHERE seat.is_delete = 'false' AND "
			"seat.dept_code = ANY(SELECT code FROM icc_view_dept_recursive WHERE path like '%" + l_oRequestBody.m_strDeptCode + "%')) OR "
			"c.caller_id = ANY(SELECT seat.no FROM icc_t_seat seat WHERE seat.is_delete = 'false' AND "
			"seat.dept_code = ANY(SELECT code FROM icc_view_dept_recursive WHERE path like '%" + l_oRequestBody.m_strDeptCode + "%')))";
	}

	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition += str(boost::format("(c.called_id ='%1%')") % l_oRequestBody.m_strSeatNo);
	}

	if (!l_oRequestBody.m_strCallerID.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition += str(boost::format("(c.caller_id like '%%%1%%%')") % l_oRequestBody.m_strCallerID);
	}
	if (!l_oRequestBody.m_strCalledID.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(c.called_id like '%%%1%%%')") % l_oRequestBody.m_strCalledID));
	}

	std::string l_strBeginTime(l_oRequestBody.m_strStartTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strStartTime.empty())
	{
		l_strBeginTime = "1900-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}

	/*std::string l_strTimeCondition = str(boost::format(
		"((c.incoming_time BETWEEN '%1%' and '%2%' AND c.call_direction = 'in') "
		"OR (c.ringback_time BETWEEN '%1%' and '%2%' AND c.call_direction = 'out'))")
		% l_strBeginTime%l_strEndTime);*/
		// 有的话务记录可以没有incoming_time和ringback_time [7/19/2018 t26150]
	std::string l_strTimeCondition = str(boost::format("(c.callref_id BETWEEN to_char('%1%'::TIMESTAMP, 'YYYYMMDDHH24MISS')"
		"and to_char('%2%'::TIMESTAMP+interval'1 second', 'YYYYMMDDHH24MISS'))") % l_strBeginTime % l_strEndTime);

	l_strCondition.append(l_strCondition.empty() ? "" : " and ");
	l_strCondition.append(l_strTimeCondition);

	if (!l_oRequestBody.m_strHangupType.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append("(" + BuildMultiConditions("c.hangup_type", "=", l_oRequestBody.m_strHangupType) + ")");
	}
	if (!l_oRequestBody.m_strReceiptCodeOrName.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(a.receipt_code like '%%%1%%%' or "
			"a.receipt_name like '%%%1%%%' or cb.receipt_name like '%%%1%%%')") % l_oRequestBody.m_strReceiptCodeOrName));
	}
	if (!l_oRequestBody.m_strCaseID.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(a.id like '%%%1%%%' or cb.alarm_id like '%%%2%%%')") % l_oRequestBody.m_strCaseID % l_oRequestBody.m_strCaseID));
	}
	if (!l_oRequestBody.m_strCallrefID.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(c.callref_id = '%1%')") % l_oRequestBody.m_strCallrefID));
	}
	if (!l_oRequestBody.m_strCallDirection.empty())
	{
		l_strCondition.append(l_strCondition.empty() ? "" : " and ");
		l_strCondition.append(str(boost::format("(c.call_direction = '%1%')") % l_oRequestBody.m_strCallDirection));
	}
}

void CBusinessImpl::BuildQueryCondition(PROTOCOL::CSearchAlarmRequest::CBody& l_oRequestBody, std::string& l_strCondResult)
{
	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_oRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strBeginTime.empty())
	{
		l_strBeginTime = m_pDateTime->ToString(m_pDateTime->AddDays(m_pDateTime->CurrentDateTime(), -1));//如果时间为空，默认只查一天 [10/24/2019 151116314]
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}
	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}

	//以alarm.id代替时间，可以利用索引，并减少数据库排序和计数时间，提高查询效率
	std::string l_strConditionTime("alarm.time between '" + l_strBeginTime + "' and '" + l_strEndTime + "'");
	l_strCondResult.append("(").append(l_strConditionTime).append(")");

	std::string l_strAlarmID = m_pString->Trim(l_oRequestBody.m_strID);
	if (!l_strAlarmID.empty())
	{
		l_strCondResult.append("and ").append("(alarm.id like '%" + l_strAlarmID + "%')");
	}

	{
		//状态是 受理中的（未提交的）警情不显示----应兵爷要求注释放开查询
		std::string l_strCondAlarmContent("alarm.state <> '00'"); //DIC019010
		//l_strCondResult.append(" and ").append("(").append(l_strCondAlarmContent).append(")");
	}

	if (!l_oRequestBody.m_strTitle.empty())
	{
		std::string l_strCondTitle = "alarm.title like '%" + l_oRequestBody.m_strTitle + "%'";
		l_strCondResult.append(" and ").append("(").append(l_strCondTitle).append(")");
	}

	if (!l_oRequestBody.m_strContent.empty())
	{	//警情描述，模糊匹配
		std::string l_strCondAlarmContent("alarm.content like '%" + l_oRequestBody.m_strContent + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondAlarmContent).append(")");
	}

	if (!l_oRequestBody.m_strAddr.empty())
	{	//警情详细发生地址，模糊匹配
		std::string l_strCondAlarmAddr("alarm.addr like '%" + l_oRequestBody.m_strAddr + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondAlarmAddr).append(")");
	}

	//if (!l_oRequestBody.m_strCityCode.empty())
	//{	//警情详细发生地址，模糊匹配
	//	std::string l_strCondAlarmCityCode("alarm.city_code like '%" + l_oRequestBody.m_strCityCode + "%'");
	//	l_strCondResult.append(" and ").append("(").append(l_strCondAlarmCityCode).append(")");
	//}

	if (!l_oRequestBody.m_strState.empty())
	{	//警情状态：未处置、未反馈、未结案、已结案
		std::string l_strCondAlarmState(BuildMultiConditions("alarm.state", "=", l_oRequestBody.m_strState));
		l_strCondResult.append(" and ").append("(").append(l_strCondAlarmState).append(")");
	}

	if (!l_oRequestBody.m_strLevel.empty())
	{	//警情级别：一级、二级、三级
		std::string l_strCondAlarmLevel(BuildMultiConditions("alarm.level", "=",
			l_oRequestBody.m_strLevel));
		l_strCondResult.append(" and ").append("(").append(l_strCondAlarmLevel).append(")");
	}

	if (!l_oRequestBody.m_strSourceType.empty())
	{	//警情报警来源类型：110，119，122，短信，微信
		std::string l_strCondSourceType(BuildMultiConditions("alarm.source_type", "=",
			l_oRequestBody.m_strSourceType));
		l_strCondResult.append(" and ").append("(").append(l_strCondSourceType).append(")");
	}

	if (!l_oRequestBody.m_strHandleType.empty())
	{
		//警情处理类型（下拉选择）
		std::string l_strCondHandleType(BuildMultiConditions("alarm.handle_type", "=",
			l_oRequestBody.m_strHandleType));
		l_strCondResult.append(" and ").append("(").append(l_strCondHandleType).append(")");
	}

	//支持客户端只显示有效警情
	if (l_oRequestBody.m_strIsVerity == "1")
	{
		//有效警情
		//无效警情编码==DIC003011
		std::string l_strCondIsVerity("alarm.handle_type = '01'");// and alarm.first_type != 'DIC003011'");
		l_strCondResult.append(" and ").append("(").append(l_strCondIsVerity).append(")");
	}
	else if (l_oRequestBody.m_strIsVerity == "0")
	{
		//无效警情
		std::string l_strCondIsVerity("alarm.handle_type != '01'");// or alarm.first_type = 'DIC003011'");
		l_strCondResult.append(" and ").append("(").append(l_strCondIsVerity).append(")");
	}
	else
	{
		//所有
	}

	if (!l_oRequestBody.m_strFirstType.empty())
	{	//警情一级类型（下拉选择）
		std::string l_strCondFirstType(BuildMultiConditions("alarm.first_type", "=",
			l_oRequestBody.m_strFirstType));
		l_strCondResult.append(" and ").append("(").append(l_strCondFirstType).append(")");
	}

	if (!l_oRequestBody.m_strSecondType.empty())
	{	//警情二级类型（下拉选择）
		std::string l_strCondSecondType(BuildMultiConditions("alarm.second_type", "=",
			l_oRequestBody.m_strSecondType));
		l_strCondResult.append(" and ").append("(").append(l_strCondSecondType).append(")");
	}

	if (!l_oRequestBody.m_strThirdType.empty())
	{	//警情三级类型（下拉选择）
		std::string l_strCondThirdType(BuildMultiConditions("alarm.third_type", "=",
			l_oRequestBody.m_strThirdType));
		l_strCondResult.append(" and ").append("(").append(l_strCondThirdType).append(")");
	}
	if (!l_oRequestBody.m_strFourthType.empty())
	{	//警情四级类型（下拉选择）
		std::string l_strCondFourthType(BuildMultiConditions("alarm.fourth_type", "=",
			l_oRequestBody.m_strFourthType));
		l_strCondResult.append(" and ").append("(").append(l_strCondFourthType).append(")");
	}

	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		l_strCondResult.append(m_pString->Format(" and alarm.receipt_seatno = '%s' ", l_oRequestBody.m_strSeatNo.c_str()));
	}

	if (!l_oRequestBody.m_strEventType.empty())
	{	//事件类型（编码以逗号分隔）
		std::string l_strCondTmp(BuildMultiConditions("alarm.event_type", "=",
			l_oRequestBody.m_strEventType));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//警情报警号码字典类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strCalledNoType.empty())
	{
		std::string l_strCondCalledNoType(BuildMultiConditions("alarm.called_no_type", "=",
			l_oRequestBody.m_strCalledNoType));
		l_strCondResult.append(" and ").append("(").append(l_strCondCalledNoType).append(")");
	}

	//警情报警人号码（模糊匹配）
	std::string l_strCallerNumber = m_pString->Trim(l_oRequestBody.m_strCallerNo);
	if (!l_strCallerNumber.empty())
	{
		std::string l_strCondCallerNo(str(boost::format("alarm.caller_no like '%%%1%%%'")
			% l_strCallerNumber));
		l_strCondResult.append(" and ").append("(").append(l_strCondCallerNo).append(")");
	}

	//警情报警人姓名（模糊匹配）
	if (!l_oRequestBody.m_strCallerName.empty())
	{
		std::string l_strCondCallerName("alarm.caller_name like '%"
			+ l_oRequestBody.m_strCallerName + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondCallerName).append(")");
	}

	//警情联系人号码（模糊查询）
	if (!l_oRequestBody.m_strContactNo.empty())
	{
		std::string l_strCondTmp("alarm.contact_no like '%"
			+ l_oRequestBody.m_strContactNo + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//警情联系人姓名（模糊查询）
	if (!l_oRequestBody.m_strContactName.empty())
	{
		std::string l_strCondTmp("alarm.contact_name like '%"
			+ l_oRequestBody.m_strCallerName + "%'");
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//警情管辖单位编码（多值查询，逗号分隔）[admin_dept_code]
	if (!l_oRequestBody.m_strAdminDeptCode.empty())
	{
		if (l_oRequestBody.m_strAdminDeptCodeRecursion == "0")
		{//不递归
			std::string l_strCondTmp(BuildMultiConditions("alarm.admin_dept_code", "=",
				l_oRequestBody.m_strAdminDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询所有下级单位单位编码
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strAdminDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("alarm.admin_dept_code = any(%1%)") % l_strQueryChildDept));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//警情接警单位编码（递归查询）[receipt_dept_code]
	if (!l_oRequestBody.m_strReceiptDeptCode.empty())
	{
		if (l_oRequestBody.m_strReceiptDeptCodeRecursion == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("alarm.receipt_dept_code", "=",
				l_oRequestBody.m_strReceiptDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strReceiptDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("alarm.receipt_dept_code = any(%1%)") % l_strQueryChildDept));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//警情值班领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.leader_code", "=",
			l_oRequestBody.m_strLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//接警员（多值查询，下拉选择）
	if (!l_oRequestBody.m_strReceiptCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.receipt_code", "=",
			l_oRequestBody.m_strReceiptCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//当前用户部门编码，限制用户只可查询自己部门及下属部门信息
	if (!l_oRequestBody.m_strCurUserDeptCode.empty())
	{
		std::string l_strCurUserDept("cdept.path like '%" + l_oRequestBody.m_strCurUserDeptCode + "%'");
		std::string l_strCondCurUserDept("select cdept.code from icc_view_dept_recursive cdept where ");
		l_strCondCurUserDept.append(l_strCurUserDept);

		std::string l_strChildDept;
		DataBase::IResultSetPtr l_pRSetChildDept = m_pDBConn->Exec(l_strCondCurUserDept);
		while (l_pRSetChildDept->Next())
		{
			l_strChildDept += l_strChildDept.empty() ? l_pRSetChildDept->GetValue("code") : "," + l_pRSetChildDept->GetValue("code");
		}

		std::string l_strCondTmp(BuildMultiConditions("alarm.receipt_dept_code", "=", l_strChildDept));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	std::string l_strFeedBackCondition;
	//反馈警情类型
	if (!l_oRequestBody.m_strFeedbackFirstType.empty())
	{	//反馈警情一级类型（下拉选择）
		std::string l_strCondFirstType(BuildMultiConditions("feedback.alarm_first_type", "=",
			l_oRequestBody.m_strFeedbackFirstType));
		l_strFeedBackCondition.append("(").append(l_strCondFirstType).append(")");
	}

	if (!l_oRequestBody.m_strFeedbackSecondType.empty())
	{	//反馈警情二级类型（下拉选择）
		std::string l_strCondSecondType(BuildMultiConditions("feedback.alarm_second_type", "=",
			l_oRequestBody.m_strFeedbackSecondType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondSecondType).append(")");
	}

	if (!l_oRequestBody.m_strFeedbackThirdType.empty())
	{
		// 反馈警情三级类型（下拉选择）
		std::string l_strCondThirdType(BuildMultiConditions("feedback.alarm_third_type", "=",
			l_oRequestBody.m_strFeedbackThirdType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondThirdType).append(")");
	}
	if (!l_oRequestBody.m_strFeedbackFourthType.empty())
	{	//反馈警情四级类型（下拉选择）
		std::string l_strCondFourthType(BuildMultiConditions("feedback.alarm_fourth_type", "=",
			l_oRequestBody.m_strFeedbackFourthType));
		l_strFeedBackCondition.append(" and ").append("(").append(l_strCondFourthType).append(")");
	}

	if (!l_strFeedBackCondition.empty())
	{
		l_strCondResult.append(" and ").append("alarm.id = any(SELECT alarm_id FROM icc_t_alarm_feedback feedback WHERE " + l_strFeedBackCondition + ")");
	}

	if (!l_oRequestBody.m_strIsFeedBack.empty()/* && l_oRequestBody.m_strIsFeedBack == "1"*/)
	{
		l_strCondResult.append(" and ").append("(alarm.is_feedback = '" + l_oRequestBody.m_strIsFeedBack + "')");
	}

	if (!l_oRequestBody.m_strIsVisitor.empty())
	{
		l_strCondResult.append(" and ").append("(alarm.is_visitor = '" + l_oRequestBody.m_strIsVisitor + "')");
	}

	if (!l_oRequestBody.m_strSeatNo.empty())
	{
		l_strCondResult.append(" and ").append("(alarm.receipt_seatno = '" + l_oRequestBody.m_strSeatNo + "')");
	}

	l_strCondResult.append(" and ").append("(alarm.is_delete is null or alarm.is_delete != 'true')");
}

//构造处警查询条件
void CBusinessImpl::BuildQueryCondition(PROTOCOL::CSearchAlarmProcessRequest::CBody& l_oRequestBody, std::string& l_strCondResult)
{
	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_oRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strBeginTime.empty())
	{
		l_strBeginTime = "1979-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}
	std::string l_strCondTime = str(boost::format("(process.create_time between '%1%' and '%2%')")
		% l_strBeginTime % l_strEndTime);

	l_strCondResult.append(l_strCondTime);
	//处警ID（模糊查询）
	std::string l_strProcessID = m_pString->Trim(l_oRequestBody.m_strID);
	if (!l_strProcessID.empty())
	{
		l_strCondResult.append("and ").append(str(boost::format("(process.id like '%1%')")
			% l_strProcessID));
	}

	//警情ID（模糊查询）
	std::string l_strAlarmID = m_pString->Trim(l_oRequestBody.m_strAlarmID);
	if (!l_strAlarmID.empty())
	{
		std::string l_strCondTmp = str(boost::format("(process.alarm_id like '%1%')")
			% l_strAlarmID);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//处警单状态（模糊查询）
	if (!l_oRequestBody.m_strState.empty())
	{
		std::string l_strCondTmp = str(boost::format("(process.state like '%1%')")
			% l_oRequestBody.m_strState);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//派警单位代码（递归查询）
	if (!l_oRequestBody.m_strDispatchDeptCode.empty())
	{
		if (l_oRequestBody.m_strDispatchDeptCodeRecursive == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("process.dispatch_dept_code", "=",
				l_oRequestBody.m_strDispatchDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strDispatchDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("process.dispatch_dept_code = any(%1%)") % l_strQueryChildDept));

			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//派警人警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strDispatchCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("process.dispatch_code", "=",
			l_oRequestBody.m_strDispatchCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//派警单位值班领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strDispatchLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("process.dispatch_leader_code", "=",
			l_oRequestBody.m_strDispatchLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//派警单位派警意见（模糊查询）
	if (!l_oRequestBody.m_strDispatchSuggestion.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("process.dispatch_suggestion", "=",
			l_oRequestBody.m_strDispatchSuggestion));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//派警单位领导指示（模糊查询）
	if (!l_oRequestBody.m_strDispatchLeaderInstruction.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("process.dispatch_leader_instruction", "=",
			l_oRequestBody.m_strDispatchLeaderInstruction));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//处警单位代码（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessDeptCode.empty())
	{
		if (l_oRequestBody.m_strDispatchDeptCodeRecursive == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("process.process_dept_code", "=",
				l_oRequestBody.m_strProcessDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strProcessDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("process.process_dept_code = any(%1%)") % l_strQueryChildDept));

			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//处警人警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessCode.empty())
	{	//是否涉外（空值不查）
		std::string l_strCondTmp(BuildMultiConditions("process.process_code", "=",
			l_oRequestBody.m_strProcessCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//处警单位值班领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("process.process_leader_code", "=",
			l_oRequestBody.m_strProcessLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//处警反馈（模糊查询）
	if (!l_oRequestBody.m_strProcessFeedback.empty())
	{
		std::string l_strCondTmp = str(boost::format("(process.process_feedback like '%1%')")
			% l_oRequestBody.m_strProcessFeedback);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//处警单位领导指示（模糊查询）
	if (!l_oRequestBody.m_strProcessLeaderInstruction.empty())
	{
		std::string l_strCondTmp = str(boost::format("(process.process_leader_instruction like '%1%')")
			% l_oRequestBody.m_strProcessLeaderInstruction);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//警情一级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strFirstType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.first_type", "=",
			l_oRequestBody.m_strFirstType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//警情二级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strSecondType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.second_type", "=",
			l_oRequestBody.m_strSecondType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}
	//警情三级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strThirdType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("alarm.third_type", "=",
			l_oRequestBody.m_strThirdType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}
}

////构造反馈查询条件，未联调，未使用
void CBusinessImpl::BuildQueryCondition(PROTOCOL::CSearchAlarmFeedbackRequest::CBody& l_oRequestBody, std::string& l_strCondResult)
{
	//查询开始时间（不能为空）;查询结束时间（不能为空）
	std::string l_strBeginTime(l_oRequestBody.m_strBeginTime);
	std::string l_strEndTime(l_oRequestBody.m_strEndTime);
	if (l_oRequestBody.m_strBeginTime.empty())
	{
		l_strBeginTime = "1979-1-1 0:0:0";
		ICC_LOG_WARNING(m_pLog, "the request begein_time is empty");
	}

	if (l_oRequestBody.m_strEndTime.empty())
	{
		l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		ICC_LOG_WARNING(m_pLog, "the request end_time is empty");
	}

	std::string l_strCondTime = str(boost::format("(feedback.create_time between '%1%' and '%2%')")
		% l_strBeginTime % l_strEndTime);

	l_strCondResult.append(l_strCondTime);

	//反馈ID（模糊查询）
	std::string l_strFeedbackID = m_pString->Trim(l_oRequestBody.m_strID);
	if (!l_strFeedbackID.empty())
	{
		l_strCondResult.append("and ").append(str(boost::format("(feedback.guid like '%1%')")
			% l_strFeedbackID));
	}

	//警情ID（模糊查询）
	std::string l_strAlarmID = m_pString->Trim(l_oRequestBody.m_strAlarmID);
	if (!l_strAlarmID.empty())
	{
		std::string l_strCondTmp = str(boost::format("(feedback.alarm_id like '%1%')")
			% l_strAlarmID);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//处警ID（模糊查询）
	std::string l_strProcessID = m_pString->Trim(l_oRequestBody.m_strProcessID);
	if (!l_strProcessID.empty())
	{
		std::string l_strCondTmp = str(boost::format("(feedback.process_id like '%1%')")
			% l_strProcessID);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//反馈类型（多值，模糊查询）
	if (!l_oRequestBody.m_strResultType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.result_type", "like",
			l_oRequestBody.m_strResultType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//处理结果内容（模糊查询）
	if (!l_oRequestBody.m_strResultContent.empty())
	{
		std::string l_strCondTmp = str(boost::format("(feedback.result_content like '%1%')")
			% l_oRequestBody.m_strResultContent);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//领导指示（模糊查询）
	if (!l_oRequestBody.m_strLeaderInstruction.empty())
	{
		std::string l_strCondTmp = str(boost::format("(feedback.leader_instruction like '%1%')")
			% l_oRequestBody.m_strLeaderInstruction);
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//反馈单状态（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strState.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.state", "=",
			l_oRequestBody.m_strState));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//反馈单位代码（递归查询）
	if (!l_oRequestBody.m_strFeedbackDeptCode.empty())
	{
		if (l_oRequestBody.m_strFeedbackDeptCodeRecursive == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("feedback.feedback_dept_code", "=",
				l_oRequestBody.m_strFeedbackDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strFeedbackDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("feedback.feedback_dept_code = any(%1%)") % l_strQueryChildDept));

			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//反馈人警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strFeedbackCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.feedback_code", "=",
			l_oRequestBody.m_strFeedbackCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//反馈单位领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strFeedbackLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.feedback_leader_code", "=",
			l_oRequestBody.m_strFeedbackLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//处警单位代码（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessDeptCode.empty())
	{
		if (l_oRequestBody.m_strProcessDeptCodeRecursive == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("feedback.process_dept_code", "=",
				l_oRequestBody.m_strProcessDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strProcessDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("feedback.process_dept_code = any(%1%)") % l_strQueryChildDept));

			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//处警人警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessCode.empty())
	{
		//是否涉外（空值不查）
		std::string l_strCondTmp(BuildMultiConditions("feedback.process_code", "=",
			l_oRequestBody.m_strProcessCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//处警单位值班领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strProcessLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.process_leader_code", "=",
			l_oRequestBody.m_strProcessLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//派警单位代码（递归查询）
	if (!l_oRequestBody.m_strDispatchDeptCode.empty())
	{
		if (l_oRequestBody.m_strDispatchDeptCodeRecursive == "0")
		{//不递归查询
			std::string l_strCondTmp(BuildMultiConditions("feedback.dispatch_dept_code", "=",
				l_oRequestBody.m_strDispatchDeptCode));
			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
		else
		{//递归查询下级单位
			std::string l_strCondQueryChildDept(BuildMultiConditions("cdept.path", "like",
				l_oRequestBody.m_strDispatchDeptCode));
			std::string l_strQueryChildDept("select cdept.code from icc_view_dept_recursive cdept where ");
			l_strQueryChildDept.append(l_strCondQueryChildDept);

			std::string l_strCondTmp(str(boost::format("feedback.dispatch_dept_code = any(%1%)") % l_strQueryChildDept));

			l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
		}
	}

	//派警人警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strDispatchCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.dispatch_code", "=",
			l_oRequestBody.m_strDispatchCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//派警单位值班领导警号（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strDispatchLeaderCode.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.dispatch_leader_code", "=",
			l_oRequestBody.m_strDispatchLeaderCode));
		l_strCondResult.append(" and ").append("(").append(l_strCondTmp).append(")");
	}

	//警情报警号码类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strAlarmCalledNoType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.alarm_called_no_type", "=",
			l_oRequestBody.m_strAlarmCalledNoType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//警情一级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strAlarmFirstType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.alarm_first_type", "=",
			l_oRequestBody.m_strAlarmFirstType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//警情二级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strAlarmSecondType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.alarm_second_type", "=",
			l_oRequestBody.m_strAlarmSecondType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}
	//警情三级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strAlarmThirdType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.alarm_third_type", "=",
			l_oRequestBody.m_strAlarmThirdType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}

	//警情四级类型（多值查询，逗号分隔）
	if (!l_oRequestBody.m_strAlarmFourthType.empty())
	{
		std::string l_strCondTmp(BuildMultiConditions("feedback.alarm_fourth_type", "=",
			l_oRequestBody.m_strAlarmFourthType));
		l_strCondResult.append(" and ").append(l_strCondTmp);
	}
}

std::string CBusinessImpl::BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt)
{
	std::string l_strResult;
	vector<std::string> l_vecCondition;
	m_pString->Split(p_strConditionToSpilt, ",;", l_vecCondition, false);
	for (std::string l_strCondition : l_vecCondition)
	{
		std::string l_strCondTemp;
		if (p_strOperator == "like")
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '%" + l_strCondition + "%'");
		}
		else
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '" + l_strCondition + "'");
		}
		if (!l_strResult.empty())
		{
			l_strResult.append(" or ").append(l_strCondTemp);
		}
		else
		{
			l_strResult.append(l_strCondTemp);
		}
	}
	return l_strResult;
}

void CBusinessImpl::QueryAlarmCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid)
{
	DataBase::IResultSetPtr l_pAlarmCount;
	l_pAlarmCount = m_pDBAlarmCount->Exec(p_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pAlarmCount->GetSQL().c_str());
	std::lock_guard<std::mutex> lock(m_mutexSelectAlarmCount);
	m_mapSelectAlarmCount[p_strGuid] = l_pAlarmCount;
}

void CBusinessImpl::QueryAlarmAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid)
{
	DataBase::IResultSetPtr l_pAlarmAllCount;
	l_pAlarmAllCount = m_pDBAlarmCount->Exec(p_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pAlarmAllCount->GetSQL().c_str());
	std::lock_guard<std::mutex> lock(m_mutexSelectAllAlarmCount);
	m_mapSelectAllAlarmCount[p_strGuid] = l_pAlarmAllCount;
}

void CBusinessImpl::QueryCallAllCount(DataBase::SQLRequest p_oSQLRequest, std::string p_strGuid)
{
	DataBase::IResultSetPtr l_pCallAllCount;
	p_oSQLRequest.sql_id = "seach_callevent_without_seatno_allcount";
	l_pCallAllCount = m_pDBAlarmCount->Exec(p_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pCallAllCount->GetSQL().c_str());
	std::lock_guard<std::mutex> lock(m_mutexSelectCallCount);
	m_mapSelectCallCount[p_strGuid] = l_pCallAllCount;
}


//*************************************************
//Method: ICC::CDBAgentBusiness::OnSqlResponseBak
//Function: 根据SQL执行结果构造响应消息，并发送至目的地
//Params: const IResultSetPtr  &p_pRSet
//Params: NotifiSqlRequestPtr p_pNotifiSqlRequest
//Return: void
//Author: [2/24/2018 t26150]
//*************************************************
void CBusinessImpl::ResponseSQLRequest(const DataBase::IResultSetPtr& p_pRSet, ObserverPattern::INotificationPtr p_pNotifiSqlRequest)
{
	std::string l_strRecvMsg(p_pNotifiSqlRequest->GetMessages());
	if (!m_pJsonFty)
	{
		ICC_LOG_ERROR(m_pLog, "json factory is null,cannot parse sql request");
		return;
	}

	JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
	if (!l_pJson || !l_pJson->LoadJson(l_strRecvMsg))
	{
		ICC_LOG_ERROR(m_pLog, "parse sql request error,not json syntax");
		return;
	}

	//消息头
	PROTOCOL::CSQLRespond l_oRespond;
	l_oRespond.m_oHeader.m_strSystemID = "icc_2.0";
	l_oRespond.m_oHeader.m_strSubsystemID = "icc_application_server_dbagent";
	l_oRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oRespond.m_oHeader.m_strRelatedID = l_pJson->GetNodeValue("/header/msgid", "");
	l_oRespond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oRespond.m_oHeader.m_strCmd = "sql_respond";
	l_oRespond.m_oHeader.m_strRequest = l_pJson->GetNodeValue("/header/reponse", "");
	l_oRespond.m_oHeader.m_strRequestType = "0";

	if (!p_pRSet->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "dbagent exec sql error:[%s]", p_pRSet->GetErrorMsg().c_str());
		l_oRespond.m_oBody.m_strCount = "-1";
	}
	else
	{
		l_oRespond.m_oBody.m_strCount = std::to_string(p_pRSet->RecordSize());
	}
	if (p_pRSet->FieldSize() > 0)
	{
		//SQL为SELECT时，返回消息中包含查询结果:count,data
		l_oRespond.m_oBody.m_vecData.push_back(p_pRSet->GetFieldNames());
		while (p_pRSet->Next())
		{
			std::vector<std::string> l_vecDataItem;
			for (string l_strFieldName : l_oRespond.m_oBody.m_vecData[0])
			{
				l_vecDataItem.push_back(p_pRSet->GetValue(l_strFieldName));
			}
			l_oRespond.m_oBody.m_vecData.push_back(std::move(l_vecDataItem));
		}
	}

	if (!m_pJsonFty)
	{
		ICC_LOG_FATAL(m_pLog, "dbagent json fty is null,can not create response msg", p_pRSet->GetErrorMsg().c_str());
		return;
	}
	string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());

	p_pNotifiSqlRequest->Response(l_strMessage);
}

/*
* 功能: 读取配置
* TODO: 使用Config组件读取配置
* 参数: string p_strConfigPath
* 返回: bool
* 作者: [2/12/2018 t26150]
*/
bool CBusinessImpl::ReadConfig()
{
	Config::IConfigPtr l_pCfgReader = ICCGetIConfigFactory()->CreateConfig();
	m_strCheckSQL = l_pCfgReader->GetValue("ICC/Plugin/DBAgent/check_sql", "SELECT 1;");
	m_iMaxPageSize = m_pString->ToInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/max_page_size", "1000").c_str());
	m_iMaxPageSize = m_iMaxPageSize < 0 ? 1000 : m_iMaxPageSize;
	m_uiDBCheckDuration = std::min(m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/check_duration", "5").c_str()), (unsigned int)5);

	if (m_uiDBCheckDuration <= 0)
	{
		m_uiDBCheckDuration = 5;
	}

	m_uiInitDBClientNum = std::min(m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/client_initnum", "5")), (unsigned int)5);
	m_uiMaxDBClientNum = std::min(m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/client_maxnum", "20")), (unsigned int)20);
	m_iStatisticMethod = m_pString->ToInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/statistic", "1").c_str());

	m_iCommonAlarmDeptQueryDays = m_pString->ToInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/CommonAlarmDeptQueryDays", "5"));
	m_iCommonAlarmTypeQueryDays = m_pString->ToInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/CommonAlarmTypeQueryDays", "5"));
	//常用单位以及常用警情类型Redis中数据更新的频率，单位为分钟
	m_iIntervalTime = m_pString->ToInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/IntervalTime", "10"));


	//20230207新增

	_CreateThreads();

	return true;
}

void ICC::CBusinessImpl::SendRespondMsg(PROTOCOL::IRespond& p_oRespond, ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	string l_strMessage = p_oRespond.ToString(m_pJsonFty->CreateJson());
	ICC_LOG_DEBUG(m_pLog, "response:[%s]", l_strMessage.c_str());
	p_pNotifiReqeust->Response(l_strMessage);
}

/*弃用
* 功能: 获取一个DBClient用以执行SQL命令；如果SQL请求属于先前已开始的一个事务,
*		 则返回对应事务的DBClient，否则返回任意一个空闲的DBClient.
* 参数: std::string p_strSenderID
* 参数: std::string p_strSql
* 返回: IPGClientPtr
* 作者: [2/12/2018 t26150]
*/
DataBase::IDBConnPtr CBusinessImpl::GetDBClient(std::string p_strSenderID, std::string p_strSql)
{
	m_mtxDBClientLock.lock();
	DataBase::IDBConnPtr l_pDBClient = nullptr;
	//在m_mapTransactionInfo查找SQL命令是否属于一个未完成的事务,若是，则返回开启对应
	//事务的DBClient，否则返回任一空闲的DBClient
	auto it = m_mapTransactionInfo.find(p_strSenderID);
	if (it != m_mapTransactionInfo.end())
	{
		l_pDBClient = it->second;
	}
	else
	{
		while (!l_pDBClient)
		{
			if (!m_lstIdleDBClient.empty())
			{
				l_pDBClient = m_lstIdleDBClient.back();
				m_lstIdleDBClient.pop_back();
			}
			else if (m_uiCurDBClientNum < m_uiMaxDBClientNum)
			{
				DataBase::IDBConnPtr l_pTmpDB = m_pDBConnFty->CreateDBConn(DataBase::DBType::PostgreSQL);
				if (0 == l_pTmpDB->Connect())
				{
					l_pDBClient = l_pTmpDB;
					l_pDBClient->Exec("set client_encoding = utf8");
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "DBClient failed to connect to dababase ");
				}
			}
			else
			{
				try
				{
					m_cvHasIdleDBClient.wait(m_mtxDBClientLock);
				}
				catch (...)
				{
				}
			}
		}
	}
	m_lstBusyDBClient.push_back(l_pDBClient);

	//如果SQL为begin(开始事务)，则将该DBClient保存到m_mapTransactionInfo
	if (p_strSql == "begin")
	{
		m_mapTransactionInfo[p_strSenderID] = l_pDBClient;
	}

	try
	{
		m_mtxDBClientLock.unlock();
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "m_mtxDBClientLock.unlock() Exception! ");
	}


	return l_pDBClient;
}

/*弃用
* 功能: 归还使用后的DBClient，以便其他线程可用。如果DBClient处于事务状态，
*		 则仍将其保存到事务map;如果一个DBClient结束事务，则将其从事务map里
*		 移除然后保存至空闲DBClient队列
* 参数: IPGClientPtr p_pDBClient
* 参数: std::string p_strSenderID，标识开启的事务，以后或用替换以事务ID
* 参数: std::string p_strSql，根据该参数确定SQL是否与事务相关
* 返回: void
* 作者: [2/12/2018 t26150]
*/
void CBusinessImpl::ReleaseDBClient(DataBase::IDBConnPtr p_pDBClient, std::string p_strSenderID, std::string p_strSql)
{
	if (!p_strSql.compare("begin"))
	{// SQL为“开始事务”，保持DB客户端信息不变
		return;
	}
	m_mtxDBClientLock.lock();
	auto it = m_mapTransactionInfo.find(p_strSenderID);
	if (it != m_mapTransactionInfo.end())
	{// DB客户端已开启事务，当执行的SQL为提交或回滚时，将其从事务表中删除，并加入idle队列
		if (p_strSql == "commit" || p_strSql == "rollback")
		{
			m_mapTransactionInfo.erase(it);
			m_lstIdleDBClient.push_back(p_pDBClient);
			m_cvHasIdleDBClient.notify_one();
		}
	}
	else
	{// 该客户端未开启事务，直接将DB客户端归还到idle队列		
		m_lstIdleDBClient.push_back(p_pDBClient);
		m_cvHasIdleDBClient.notify_one();
	}
	m_lstBusyDBClient.remove(p_pDBClient);

	try
	{
		m_mtxDBClientLock.unlock();
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "m_mtxDBClientLock.unlock() Exception! ");
	}

}

void CBusinessImpl::GenRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader = p_pRequestHeader;

	/*p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;*/
}

void CBusinessImpl::GenRespondHeaderEx(const std::string& p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader)
{
	p_pRespHeader = p_pRequestHeader;
}

//定时检测数据库连接的有效性，在状态变化时发送同步消息至Moniter
void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(m_strCheckSQL);
	bool l_bLastConnStatus = m_bDBConnStatus;
	m_bDBConnStatus = l_pRSet->IsValid();

	ICC_LOG_INFO(m_pLog, "DBConent check,res=%d", m_bDBConnStatus);
	if (l_bLastConnStatus != m_bDBConnStatus)
	{
		if (m_bDBConnStatus)
		{//重新连接上数据库
			ICC_LOG_INFO(m_pLog, "DB CONNECTED!");
		}
	}
}

void CBusinessImpl::_QueryMajorAlarmTypeNames(std::vector<std::string>& vecTypeNames)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = "query_icc_t_major_alarm_config_alltypename";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "get major alarm type names , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return;
	}

	while (l_result->Next())
	{
		std::string strTypeName = l_result->GetValue("type_name");
		if (!strTypeName.empty())
		{
			vecTypeNames.push_back(strTypeName);
		}
	}
}

/*****************************************************************
 *
 ****************************************************************/
void CBusinessImpl::_CreateThreads()
{
	Config::IConfigPtr l_pCfgReader = ICCGetIConfigFactory()->CreateConfig();
	if (nullptr == l_pCfgReader)
	{
		ICC_LOG_ERROR(m_pLog, "dbagent create config failed!!!");
		return;
	}
	m_strDispatchMode = l_pCfgReader->GetValue("ICC/Plugin/DBAgent/procthreaddispatchmode", "1");
	m_uProcThreadCount = m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/DBAgent/procthreadcount", "8"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (m_uProcThreadCount > uConCurrency)
	{
		m_uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "dbagent proc thread count:%d,mode:%s", m_uProcThreadCount, m_strDispatchMode.c_str());

	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			pThread->SetLogPtr(m_pLog);
			m_vecProcThreads.push_back(pThread);
		}
	}

	m_uProcThreadCount = m_vecProcThreads.size();

	ICC_LOG_INFO(m_pLog, "dbagent real proc thread count:%d", m_uProcThreadCount);
}

void CBusinessImpl::_DestoryThreads()
{
	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		m_vecProcThreads[i]->Stop(0);
	}
	m_vecProcThreads.clear();
}


CommonWorkThreadPtr CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	if (m_strDispatchMode == "1")
	{
		//std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uCurrentThreadIndex >= m_uProcThreadCount)
		{
			m_uCurrentThreadIndex = 0;
		}
		iIndex = m_uCurrentThreadIndex;
		m_uCurrentThreadIndex++;
	}
	else
	{
		//std::lock_guard<std::mutex> lock(m_mutexThread);
		int iMessageCount = 0;
		for (int i = 0; i < m_uProcThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}

	return m_vecProcThreads[iIndex];
}

void CBusinessImpl::_OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	ICC_LOG_LOWDEBUG(m_pLog, "_OnReceiveNotify enter! cmd:%s,guid:%s, ", p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str());

	CommonWorkThreadPtr pThread = _GetThread();
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVENOTIFY, 0, 0, 0, p_pNotifiReqeust->GetCmdName(), "", p_pNotifiReqeust);
		ICC_LOG_LOWDEBUG(m_pLog, "_OnReceiveNotify complete! cmd:%s,guid:%s, threadid:%u, message count:%u",
			p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str(), pThread->GetThreadId(), pThread->GetCWorkThreadListCount());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "_OnReceiveNotify failed! cmd:%s,guid:%s", p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str());
	}
}

void CBusinessImpl::_DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName)
{
	std::map<std::string, ProcNotify>::const_iterator itr;
	itr = m_mapFuncs.find(strCmdName);
	if (itr != m_mapFuncs.end())
	{
		(this->*itr->second)(p_pNotifiReqeust);
	}
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_RECEIVENOTIFY:
	{
		try
		{
			std::chrono::system_clock::time_point curBegin;
			curBegin = std::chrono::system_clock::now();

			ICC_LOG_LOWDEBUG(m_pLog, "dbagent THREADID_RECEIVENOTIFY %u begin! cmd:%s, guid:%s",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str());

			_DispatchNotify(msg_data.pNotifyPtr, msg_data.str_msg);

			std::chrono::system_clock::time_point curEnd;
			curEnd = std::chrono::system_clock::now();
			unsigned long long ullInterval = std::chrono::duration_cast<std::chrono::microseconds>(curEnd.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::microseconds>(curBegin.time_since_epoch()).count();


			ICC_LOG_LOWDEBUG(m_pLog, "dbagent THREADID_RECEIVENOTIFY %u end! cmd:%s, guid:%s, exec time:%lld",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str(), ullInterval);

			if (ullInterval > TIMOUT_INTERVAL)
			{
				ICC_LOG_WARNING(m_pLog, "exec slow! thread id:%u time:%llu, message:%s", msg_data.uThreadId, ullInterval, msg_data.pNotifyPtr->GetMessages().c_str());
			}
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception!");
		}
	}
	break;

	default:
		break;
	}
}

std::string CBusinessImpl::BuildTypeConditions(std::string p_strConditionToSpilt)
{
	std::string l_strResult;
	vector<std::string> l_vecCondition;
	m_pString->Split(p_strConditionToSpilt, ",;", l_vecCondition, false);
	for (std::string l_strCondition : l_vecCondition)
	{
		std::string l_strCondTemp;
		l_strCondTemp = m_pString->Format("(alarm.jqlbdm='%s' or alarm.jqlxdm='%s' or alarm.jqxldm='%s' or alarm.jqzldm ='%s')", l_strCondition.c_str(), l_strCondition.c_str(), l_strCondition.c_str(), l_strCondition.c_str());
		if (l_strResult.empty())
		{
			l_strResult.append(l_strCondTemp);
		}
		else
		{
			l_strResult.append(" or ").append(l_strCondTemp);
		}
	}
	return l_strResult;
}

std::string CBusinessImpl::BuildMultiConditionsEx(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt)
{
	std::string l_strResult;
	vector<std::string> l_vecCondition;
	m_pString->Split(p_strConditionToSpilt, ",;", l_vecCondition, false);
	for (std::string l_strCondition : l_vecCondition)
	{
		std::string l_strCondTemp;
		if (p_strOperator == "like")
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '" + "%" + l_strCondition + "%" + "'");
		}
		else
		{
			l_strCondTemp = (p_strColumnName + " " + p_strOperator + " '" + l_strCondition + "'");
		}
		if (!l_strResult.empty())
		{
			l_strResult.append(" or ").append(l_strCondTemp);
		}
		else
		{
			l_strResult.append(l_strCondTemp);
		}
	}
	return l_strResult;
}

bool CBusinessImpl::SearchCommonDept(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond)
{
	std::string strMessage = "";
	if (!m_pRedisClient->HGet(COMMON_DEPT, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strMessage))
	{
		//首次有部门请求该接口，Redis中还未建立节点，先判定是否建立，如果节点不存在，先查数据库并存Redis
		if (!m_pRedisClient->Exists(COMMON_DEPT))
		{
			if (!SearchCommonDeptInDB(l_oGetCommonDeptRequest, l_oRespond))
			{
				ICC_LOG_DEBUG(m_pLog, "Search Common Dept In DB Failed!!!");
				return false;
			}
			std::string strCommonDept = l_oRespond.ToExString(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(COMMON_DEPT, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strCommonDept))
			{
				ICC_LOG_DEBUG(m_pLog, "Hset Common Dept Failed!!!");
				return false;
			}
		}
		else
		{
			if (!SearchCommonDeptInDB(l_oGetCommonDeptRequest, l_oRespond))
			{
				ICC_LOG_DEBUG(m_pLog, "Search Common Dept In DB Failed!!!");
				return false;
			}
			std::string strCommonDept = l_oRespond.ToExString(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(COMMON_DEPT, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strCommonDept))
			{
				ICC_LOG_DEBUG(m_pLog, "Hset Common Dept Failed!!!");
				return false;
			}
		}
	}

	DateTime::CDateTime l_updateTime = m_pDateTime->FromString(l_oRespond.m_oBody.m_strUpdateTime);
	std::string  m_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DateTime::CDateTime l_currentTime = m_pDateTime->FromString(m_strCurrentTime);
	DateTime::CDateTime l_changeTime = m_pDateTime->AddMinutes(l_updateTime, m_iIntervalTime);
	//如果常用单位过期,更新Redis里面存储的常用单位
	if (l_changeTime <= l_currentTime)
	{
		if (!SearchCommonDeptInDB(l_oGetCommonDeptRequest, l_oRespond))
		{
			ICC_LOG_DEBUG(m_pLog, "Search Common Dept In DB Failed!!!");
			return false;
		}
		std::string m_strCommonDept = l_oRespond.ToExString(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(COMMON_DEPT, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, m_strCommonDept))
		{
			ICC_LOG_DEBUG(m_pLog, "Hset Common Dept Failed!!!");
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::SearchCommonType(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond)
{
	std::string strMessage = "";;
	if (!m_pRedisClient->HGet(COMMON_TYPE, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strMessage))
	{
		//首次有部门请求该接口，Redis中的节点还不存在，先查询数据库，再将数据存入Redis
		if (!m_pRedisClient->Exists(COMMON_TYPE))
		{
			if (!SearchCommonTypeInDB(l_oGetCommonDeptRequest, l_oRespond))
			{
				ICC_LOG_DEBUG(m_pLog, "Search Common Type In DB Failed!!!");
				return false;
			}

			//查数据库之后将数据库中的数据保存到Redis中，下一次又从Redis开始查
			std::string strCommonType = l_oRespond.ToExString(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(COMMON_TYPE, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strCommonType))
			{
				ICC_LOG_DEBUG(m_pLog, "Hset Common Type Failed!!!");
				return false;
			}
		}
		else
		{
			if (!SearchCommonTypeInDB(l_oGetCommonDeptRequest, l_oRespond))
			{
				ICC_LOG_DEBUG(m_pLog, "Search Common Type In DB Failed!!!");
				return false;
			}

			//查数据库之后将数据库中的数据保存到Redis中，下一次又从Redis开始查
			std::string strCommonType = l_oRespond.ToExString(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(COMMON_TYPE, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, strCommonType))
			{
				ICC_LOG_DEBUG(m_pLog, "Hset Common Type Failed!!!");
				return false;
			}
		}
	}

	//如果Redis中存储的的数据的超过更新时限，更新Redis中的数据
	DateTime::CDateTime l_updateTime = m_pDateTime->FromString(l_oRespond.m_oBody.m_strUpdateTime);
	std::string  m_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DateTime::CDateTime l_currentTime = m_pDateTime->FromString(m_strCurrentTime);
	DateTime::CDateTime l_changeTime = m_pDateTime->AddMinutes(l_updateTime, m_iIntervalTime);
	if (l_changeTime <= l_currentTime)
	{
		if (!SearchCommonTypeInDB(l_oGetCommonDeptRequest, l_oRespond))
		{
			ICC_LOG_DEBUG(m_pLog, "Search Common Type In DB Failed!!!");
			return false;
		}
		//查数据库之后将数据库中的数据保存到Redis中，下一次又从Redis开始查
		std::string m_strCommonType = l_oRespond.ToExString(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(COMMON_TYPE, l_oGetCommonDeptRequest.m_oBody.m_strDeptCode, m_strCommonType))
		{
			ICC_LOG_DEBUG(m_pLog, "HsetCommon Type Failed!!!");
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::SearchCommonDeptInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonDeptRespond& l_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_common_alarm_dept";
	//取前5天的数据
	DateTime::CDateTime datetime = m_pDateTime->CurrentDateTime();
	datetime = m_pDateTime->AddDays(datetime, -1 * m_iCommonAlarmDeptQueryDays);
	std::string strInterval = m_pDateTime->ToString(datetime);
	l_SqlRequest.param["jjsj_begin"] = strInterval;
	l_SqlRequest.param["jjsj_end"] = m_pDateTime->CurrentDateTimeStr();
	int l_iLimit = 15;
	l_SqlRequest.param["jjdwdm"] = l_oGetCommonDeptRequest.m_oBody.m_strDeptCode;
	l_SqlRequest.param["limit"] = std::to_string(l_iLimit);
	int l_tmpCount = 0;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "OnNotifiCommonDeptStaticRequest query error,error msg:[%s],sql=%s", l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
		return false;
	}
	else
	{
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "OnNotifiCommonDeptStaticRequest exec sql ok sql=%s", l_pResult->GetSQL().c_str());
		while (l_pResult->Next())
		{
			PROTOCOL::CGetAlarmCommonDeptRespond::CData data;
			data.m_strDeptCode = l_pResult->GetValue("dept_code");
			data.m_strDeptName = l_pResult->GetValue("dept_name");
			if (data.m_strDeptCode.empty())
			{
				continue;
			}
			l_oRespond.m_oBody.m_vecData.push_back(data);
			l_tmpCount++;
			if (l_tmpCount >= l_iLimit)
			{
				break;
			}
		}
	}
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", l_tmpCount);
	l_oRespond.m_oBody.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	return true;
}
bool CBusinessImpl::SearchCommonTypeInDB(PROTOCOL::CGetAlarmCommonInfoRequest& l_oGetCommonDeptRequest, PROTOCOL::CGetAlarmCommonTypeRespond& l_oRespond)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_common_alarm_type";
	//取前5天的数据
	DateTime::CDateTime datetime = m_pDateTime->CurrentDateTime();
	datetime = m_pDateTime->AddDays(datetime, -1 * m_iCommonAlarmDeptQueryDays);
	std::string strInterval = m_pDateTime->ToString(datetime);
	l_SqlRequest.param["jjsj_begin"] = strInterval;
	l_SqlRequest.param["jjsj_end"] = m_pDateTime->CurrentDateTimeStr();
	int l_iLimit = 15;
	l_SqlRequest.param["jjdwdm"] = l_oGetCommonDeptRequest.m_oBody.m_strDeptCode;
	l_SqlRequest.param["limit"] = std::to_string(l_iLimit);
	int l_tmpCount = 0;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "exec sql failed!!!";
		ICC_LOG_ERROR(m_pLog, "OnNotifiCommonTypeStaticRequest query error,error msg:[%s],sql=%s", l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
		return false;
	}
	else
	{
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "OnNotifiCommonTypeStaticRequest exec sql ok sql=%s", l_pResult->GetSQL().c_str());
		while (l_pResult->Next())
		{
			PROTOCOL::CGetAlarmCommonTypeRespond::CData data;
			std::vector<std::string> l_vecParam;
			std::string l_strParam = l_pResult->GetValue("jqjb");
			m_pString->Split(l_strParam, "_", l_vecParam, true);
			int l_nIndex = 0;
			for (auto it = l_vecParam.begin(); it != l_vecParam.end(); ++it)
			{
				if (l_nIndex == 0)
				{
					data.m_strFirstType = l_vecParam[l_nIndex];
				}
				else if (l_nIndex == 1)
				{
					data.m_strSecondType = l_vecParam[l_nIndex];
				}
				else if (l_nIndex == 2)
				{
					data.m_strThirdType = l_vecParam[l_nIndex];
				}
				else if (l_nIndex == 3)
				{
					data.m_strFourthType = l_vecParam[l_nIndex];
				}
				l_nIndex++;
			}
			l_oRespond.m_oBody.m_vecData.push_back(data);
			l_tmpCount++;
			if (l_tmpCount >= l_iLimit)
			{
				break;
			}
		}
	}
	l_oRespond.m_oBody.m_strCount = m_pString->Format("%d", l_tmpCount);
	l_oRespond.m_oBody.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	return true;
}

void CBusinessImpl::_QueryCallWaitingDataCount(const PROTOCOL::CSearchCallWaitingRequest &callInRequest, std::string &strCount)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "query_call_waiting_info_count";
	l_SqlRequest.param["begin_time"] = callInRequest.m_oBody.m_strBeginTime;
	l_SqlRequest.param["end_time"] = callInRequest.m_oBody.m_strEndTime;


	if (!callInRequest.m_oBody.m_strCallerId.empty())
	{
		l_SqlRequest.param["caller_id"] = callInRequest.m_oBody.m_strCallerId;

		std::string strTime = m_pDateTime->GetCallRefIdTime(callInRequest.m_oBody.m_strCallerId);
		if (strTime != "")
		{
			l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
	}

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = m_pString->Format("exec sql error, sql = %s", l_pResult->GetSQL().c_str());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return;
	}

	strTmpLog = m_pString->Format("exec sql success!, sql = %s, recordsize = %u", l_pResult->GetSQL().c_str(), l_pResult->RecordSize());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	if(l_pResult->Next())
	{
		strCount = l_pResult->GetValue("waiting_count");
	}
}