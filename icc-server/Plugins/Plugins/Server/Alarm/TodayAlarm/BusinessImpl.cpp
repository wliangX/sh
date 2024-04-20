#include "Boost.h"
#include "BusinessImpl.h"


#define SYSTEMID			("icc_server")
#define SUBSYSTEMID			("icc_server_alarm_process")
#define TIMER_CMD_NAME		"today_alarm_timer"
#define TODAYALARM_INFO		"TodayAlarmInfo"
using namespace std;

string CBusinessImpl::m_strCurTime = "";

const unsigned long THREADID_RECEIVENOTIFY = 10000;

const unsigned long long TIMOUT_INTERVAL = 500 * 1000;

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(OBSERVER_CENTER_ALARM_TODAYALARM);
	m_pLockFty = ICCGetILockFactory();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pAlarmLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
	m_pTimeLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_IntervalTime = 86400;

	m_uProcThreadCount = 0;
	m_uCurrentThreadIndex = 0;

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::_InitProcNotifys()
{
	m_mapFuncs.insert(std::make_pair("get_today_alarm_request", &CBusinessImpl::OnCNotifiGetTodayAlarm));
	m_mapFuncs.insert(std::make_pair("get_similar_alarm_request", &CBusinessImpl::OnNotifiGetSimilarAlarm));

	m_mapFuncs.insert(std::make_pair("get_call_count_today", &CBusinessImpl::OnNotifiGetCallCountStatistics));
	m_mapFuncs.insert(std::make_pair("get_call_in_count_by_period", &CBusinessImpl::OnNotifiGetCallInCountByPeriod));
	m_mapFuncs.insert(std::make_pair("get_alarm_count_by_receive_type", &CBusinessImpl::OnNotifiGetAlarmCountByReceivedType));
	m_mapFuncs.insert(std::make_pair("get_alarm_count_by_first_type", &CBusinessImpl::OnNotifiGetAlarmCountByFirstType));
	m_mapFuncs.insert(std::make_pair("get_alarm_info_by_first_type", &CBusinessImpl::OnNotifiGetAlarmInfoByFirstType));//先不分页，后面增加
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	//std::map<std::string, std::string> mapPeriods;
	//_AsignedPeriod("2022-08-30 08:30:00", "2022-08-30 11:35:00", "30", mapPeriods);

	//std::vector<std::string> vecTypes;
	//_AanalyzeTypes("00;01;02;", vecTypes);
	
	//OnCNotifiGetTodayAlarm(nullptr);

	_CreateThreads();

	_InitProcNotifys();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_today_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_similar_alarm_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_call_count_today", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_call_in_count_by_period", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_count_by_receive_type", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_count_by_first_type", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_info_by_first_type", _OnReceiveNotify);//先不分页，后面增加

	m_IntervalTime = m_pString->ToInt64(m_pConfig->GetValue("ICC/Plugin/TodayAlarm/interval", "86400"));	

	ICC_LOG_INFO(m_pLog, "todayalarm start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_today_alarm_request", OnCNotifiGetTodayAlarm);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_similar_alarm_request", OnNotifiGetSimilarAlarm);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_call_count_today", OnNotifiGetCallCountStatistics);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_call_in_count_by_period", OnNotifiGetCallInCountByPeriod);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_count_by_receive_type", OnNotifiGetAlarmCountByReceivedType);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_count_by_first_type", OnNotifiGetAlarmCountByFirstType);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_info_by_first_type", OnNotifiGetAlarmInfoByFirstType);//先不分页，后面增加
}

void CBusinessImpl::OnStop()
{
	_DestoryThreads();

	ICC_LOG_DEBUG(m_pLog, "todayalarm stop success");
}

void CBusinessImpl::OnDestroy()
{

}

bool CBusinessImpl::_QueryTodayAlarmNotUpAttention(PROTOCOL::CTodayAlarm& request, std::string& strInterval, DataBase::IResultSetPtr& pResult, std::string& strTotalCount)
{
	DataBase::SQLRequest l_tSQLReq;

	l_tSQLReq.sql_id = SELECT_TODAY_ALARM_CNT;

	l_tSQLReq.param["interval"] = strInterval;
	l_tSQLReq.param["jqgjz"] = "";

	//是否包含子集
	if (request.m_oBody.m_strReceiptDeptCodeRecursion == "1")
	{
		if (!request.m_oBody.m_strReceiptDeptCode.empty())
		{
			l_tSQLReq.param["in_receipt_dept_code"] = request.m_oBody.m_strReceiptDeptCode;
		}
	}
	else
	{
		if (!request.m_oBody.m_strReceiptDeptCode.empty())
		{
			l_tSQLReq.param["receipt_dept_code"] = request.m_oBody.m_strReceiptDeptCode;
		}
	}

	if (!request.m_oBody.m_strState.empty())
	{
		l_tSQLReq.param["state"] = request.m_oBody.m_strState;
	}
	if (!request.m_oBody.m_strCalledNoType.empty())
	{
		l_tSQLReq.param["called_no_type"] = request.m_oBody.m_strCalledNoType;
	}
	if (!request.m_oBody.m_strReceiptCode.empty())
	{
		l_tSQLReq.param["receipt_code"] = request.m_oBody.m_strReceiptCode;
	}

	if (!request.m_oBody.m_strIsSigned.empty())
	{
		l_tSQLReq.param["is_signed"] = m_pString->Format(" and ( sfyqs = '%s' and jqclztdm = '02' )", request.m_oBody.m_strIsSigned.c_str());
	}

	if (!request.m_oBody.m_strSeatNo.empty())
	{
		//l_strCondResult.append(m_pString->Format(" and alarm.receipt_seatno = '%s' ", l_oRequestBody.m_strSeatNo.c_str()));
		l_tSQLReq.param["receipt_seatno"] = request.m_oBody.m_strSeatNo;
	}

	if (!request.m_oBody.m_strIsInvalid.empty())
	{
		//is_invalid
		l_tSQLReq.param["is_invalid"] = request.m_oBody.m_strIsInvalid;
	}

	if (!request.m_oBody.m_strIsClosure.empty())
	{
		l_tSQLReq.param["is_over"] = request.m_oBody.m_strIsClosure;
	}

	if (!request.m_oBody.m_strCallerKey.empty())
	{
		//is_invalid
		l_tSQLReq.param["caller_key"] = request.m_oBody.m_strCallerKey;
	}

	/*
	if (request.m_oBody.m_strOnlyShowReal == "1")
	{
		l_tSQLReq.param["is_invalid"] = "1";
		//l_tSQLReqCnt.param["first_type"] = "DIC003011";
	}*/

	if (!request.m_oBody.m_strQueryKey.empty())
	{
		l_tSQLReq.param["query_key"] = request.m_oBody.m_strQueryKey;
	}
	if (!request.m_oBody.m_strLevel.empty())
	{
		l_tSQLReq.param["level"] = _BuildMultiConditions("jqdjdm", "=", request.m_oBody.m_strLevel);
	}

	if (request.m_oBody.m_strIsMajorAlarmFlag == "1")
	{
		int l_curCount = -1;
		DataBase::IResultSetPtr r_preResult = m_pDBConn->Exec("select count(1) num from public.icc_t_major_alarm_config where type_name = 'jqgjz'");
		if (r_preResult->IsValid())
		{
			l_curCount = m_pString->ToInt(r_preResult->GetValue(0, "num"));
		}

		//std::vector<std::string> vecTypeNames;
		//_QueryMajorAlarmTypeNames(vecTypeNames);
		//for (int i = 0; i < vecTypeNames.size(); ++i)
		//{
		//	l_tSQLReq.param[vecTypeNames[i]] = vecTypeNames[i];
		//}
		std::map<std::string, std::string> vecTypeNames;
		std::string l_curBjnr;
		l_curBjnr = _QueryMajorAlarmTypeNames(vecTypeNames);

		for (auto l_it = vecTypeNames.begin(); l_it != vecTypeNames.end(); l_it++)
		{
			l_tSQLReq.param[l_it->first] = vecTypeNames[l_it->second];
		}

		DataBase::SQLRequest r_tSQLReq;
		r_tSQLReq.sql_id = "select_type_name_jqgjz";
		
		bool l_joinFlag = false;
		DataBase::IResultSetPtr r_result = m_pDBConn->Exec(r_tSQLReq, true);
		
		if (l_curCount > 0)
		{
			l_curBjnr = l_curBjnr + "or bjnr like ";
			l_joinFlag = true;
		}
		ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", r_result->GetSQL().c_str());
		if (!r_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s], sql = [%s]", r_result->GetErrorMsg().c_str(), r_result->GetSQL().c_str());
		}
		while (r_result->Next() && l_curCount > 0)
		{
			l_curCount--;
			if (l_curCount > 0)
			{
				l_curBjnr = l_curBjnr + "'%" + r_result->GetValue("type_value") + "%' or bjnr like";
			}
			else
			{
				l_curBjnr = l_curBjnr + "'%" + r_result->GetValue("type_value") + "%'";
			}

			ICC_LOG_DEBUG(m_pLog, "jqgzj:[%s](l_curBjnr:%s)", r_result->GetValue("type_value").c_str(), l_curBjnr.c_str());
		}

		l_tSQLReq.param["master_point"] = l_curBjnr;
	}

	if (request.m_oBody.m_strIsAttentionAlarmFlag == "1")
	{
		l_tSQLReq.param["staff_code"] = request.m_oBody.m_strAttentionCode;
	}

	if (!request.m_oBody.m_strReceiverCode.empty())
	{
		l_tSQLReq.param["receiver_code_condition"] = m_pString->Format("jjdbh in (select alarm_id from icc_t_shift_relation as a left join icc_t_shift as b on a.shift_id=b.guid left join icc_t_jjdb as c on c.jjdbh = a.alarm_id where c.jjsj >= '%s' and receiver_code = '%s')",
			strInterval.c_str(), request.m_oBody.m_strReceiverCode.c_str());
	}		

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq, true);

	ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s], sql = [%s]", l_result->GetErrorMsg().c_str(), l_result->GetSQL().c_str());		
		return false;
	}	

	strTotalCount = l_result->GetValue(0, "num");

	if (!request.m_oBody.m_strSortParam.empty())
	{
		std::string strTmpSortParam = request.m_oBody.m_strSortParam;

		if (m_pString->Find(strTmpSortParam, "jqclztdm"))
		{
			strTmpSortParam = m_pString->ReplaceFirst(strTmpSortParam, "jqclztdm", "sortdm");
		}

		l_tSQLReq.param["sort_parm"] = strTmpSortParam;
	}
	else
	{
		l_tSQLReq.param["sort_parm"] = "jqclztdm, jjsj desc";
	}

	//查询当前页记录	
	l_tSQLReq.sql_id = SELECT_TODAY_ALARM;
	
	
	l_tSQLReq.param["page_size"] = request.m_oBody.m_strPageSize;
	l_tSQLReq.param["page_index"] = request.m_oBody.m_strPageIndex;
	
	pResult = m_pDBConn->Exec(l_tSQLReq, true);
	ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());		
		return false;
	}

	return true;
}

bool CBusinessImpl::_QueryTodayAlarmUpAttention(PROTOCOL::CTodayAlarm& request, std::string& strInterval, DataBase::IResultSetPtr& pResult, std::string& strTotalCount)
{
	ICC_LOG_DEBUG(m_pLog, "_QueryTodayAlarmUpAttention begin, interval:[%s]", strInterval.c_str());


	DataBase::SQLRequest l_tSQLReq;

	l_tSQLReq.sql_id = SELECT_TODAY_ALARM_CNT_UP_ATTENTION;

	std::string strCondition = m_pString->Format("jjsj is not null and jjsj >= '%s' and jjsj <= '%s' and scbs!='1' and jqclztdm != '00'", strInterval.c_str(), m_pDateTime->CurrentDateTimeStr().c_str());

	//是否包含子集
	if (request.m_oBody.m_strReceiptDeptCodeRecursion == "1")
	{
		if (!request.m_oBody.m_strReceiptDeptCode.empty())
		{
			strCondition += " and jjdwdm in (select code from public.icc_view_dept_recursive where path like '%";
			strCondition += request.m_oBody.m_strReceiptDeptCode;
			strCondition += "%')";
		}		
	}
	else
	{
		if (!request.m_oBody.m_strReceiptDeptCode.empty())
		{
			strCondition += m_pString->Format(" and jjdwdm = '%s'", request.m_oBody.m_strReceiptDeptCode.c_str());			
		}
	}

	if (!request.m_oBody.m_strState.empty())
	{
		strCondition += m_pString->Format(" and jqclztdm = '%s'", request.m_oBody.m_strState.c_str());		
	}

	if (!request.m_oBody.m_strCalledNoType.empty())
	{
		strCondition += m_pString->Format(" and jjlx = '%s'", request.m_oBody.m_strCalledNoType.c_str());		
	}

	if (!request.m_oBody.m_strReceiptCode.empty())
	{
		strCondition += m_pString->Format(" and jjybh = '%s'", request.m_oBody.m_strReceiptCode.c_str());		
	}	

	if (!request.m_oBody.m_strSeatNo.empty())
	{
		strCondition += m_pString->Format(" and jjxwh = '%s'", request.m_oBody.m_strSeatNo.c_str());		
	}

	if (!request.m_oBody.m_strIsInvalid.empty())
	{
		strCondition += m_pString->Format(" and wxbs = '%s'", request.m_oBody.m_strIsInvalid.c_str());		
	}

	if (!request.m_oBody.m_strIsClosure.empty())
	{
		strCondition += m_pString->Format(" and sfja = '%s'", request.m_oBody.m_strIsClosure.c_str());		
	}	

	if (!request.m_oBody.m_strIsSigned.empty())
	{
		strCondition += m_pString->Format(" and ( sfyqs = '%s' and jqclztdm = '02' )", request.m_oBody.m_strIsSigned.c_str());
	}

	if (!request.m_oBody.m_strCallerKey.empty())
	{
		strCondition += " and (bjdh like '%";
		strCondition += request.m_oBody.m_strCallerKey;
		strCondition += "%' or bjrzjhm like '%";
		strCondition += request.m_oBody.m_strCallerKey;
		strCondition += "%' or bjrmc like '%";
		strCondition += request.m_oBody.m_strCallerKey;
		strCondition += "%' or lxdh like '%";
		strCondition += request.m_oBody.m_strCallerKey;
		strCondition += "%')";		
	}

	if (!request.m_oBody.m_strQueryKey.empty())
	{		
		strCondition += " and (jjdbh like '%";
		strCondition += request.m_oBody.m_strQueryKey;
		strCondition += "%' or bjnr like '%";
		strCondition += request.m_oBody.m_strQueryKey;
		strCondition += "%' or jqdz like '%";
		strCondition += request.m_oBody.m_strQueryKey;
		strCondition += "%')";		
	}

	if (!request.m_oBody.m_strLevel.empty())
	{
		strCondition += m_pString->Format(" and (%s)", _BuildMultiConditions("jqdjdm", "=", request.m_oBody.m_strLevel).c_str());		
	}

	if (request.m_oBody.m_strIsMajorAlarmFlag == "1")
	{		
		int l_curCount = -1;
		DataBase::IResultSetPtr r_preResult = m_pDBConn->Exec("select count(1) num from public.icc_t_major_alarm_config where type_name = 'jqgjz'");
		if (r_preResult->IsValid())
		{
			l_curCount = m_pString->ToInt(r_preResult->GetValue(0, "num"));
		}

		std::map<std::string, std::string> vecTypeNames;
		std::string l_curBjnr;
		l_curBjnr = _QueryMajorAlarmTypeNames(vecTypeNames);

		for (auto l_it = vecTypeNames.begin(); l_it != vecTypeNames.end(); l_it++)
		{
			l_tSQLReq.param[l_it->first] = vecTypeNames[l_it->second];
		}

		DataBase::SQLRequest r_tSQLReq;
		r_tSQLReq.sql_id = "select_type_name_jqgjz";
		
		bool l_joinFlag = false;
		DataBase::IResultSetPtr r_result = m_pDBConn->Exec(r_tSQLReq, true);
		
		if (l_curCount > 0)
		{
			l_curBjnr = l_curBjnr + "or bjnr like ";
			l_joinFlag = true;
		}
		ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", r_result->GetSQL().c_str());
		if (!r_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s], sql = [%s]", r_result->GetErrorMsg().c_str(), r_result->GetSQL().c_str());
		}
		while (r_result->Next() && l_curCount > 0)
		{
			l_curCount--;
			if (l_curCount > 0)
			{
				l_curBjnr = l_curBjnr + "'%" + r_result->GetValue("type_value") + "%' or bjnr like";
			}
			else
			{
				l_curBjnr = l_curBjnr + "'%" + r_result->GetValue("type_value") + "%'";
			}

			ICC_LOG_DEBUG(m_pLog, "jqgzj:[%s](l_curBjnr:%s)", r_result->GetValue("type_value").c_str(), l_curBjnr.c_str());
		}
		
		strCondition += m_pString->Format(" and (%s)", l_curBjnr.c_str()); 
	}

	if (request.m_oBody.m_strIsAttentionAlarmFlag == "1")
	{
		strCondition += m_pString->Format(" and jjdbh in (select alarm_id from icc_t_alarm_attention where attention_staff='%s')", request.m_oBody.m_strAttentionCode.c_str());
	}

	if (!request.m_oBody.m_strReceiverCode.empty())
	{
		strCondition += m_pString->Format(" and jjdbh in (select alarm_id from icc_t_shift_relation as a left join icc_t_shift as b on a.shift_id=b.guid left join icc_t_jjdb as c on c.jjdbh = a.alarm_id where c.jjsj >= '%s' and receiver_code = '%s')",
			strInterval.c_str(), request.m_oBody.m_strReceiverCode.c_str());		
	}

	l_tSQLReq.param["attention_staff"] = request.m_oBody.m_strAttentionCode;
	l_tSQLReq.param["query_condition"] = strCondition;

	ICC_LOG_DEBUG(m_pLog, "_QueryTodayAlarmUpAttention condition complete");


	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq, true);

	ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s], sql = [%s]", l_result->GetErrorMsg().c_str(), l_result->GetSQL().c_str());
		return false;
	}

	strTotalCount = std::to_string(l_result->RecordSize());

	if (!request.m_oBody.m_strSortParam.empty())
	{
		std::string strTmpSortParam = request.m_oBody.m_strSortParam;

		if (m_pString->Find(strTmpSortParam, "jqclztdm"))
		{
			strTmpSortParam = m_pString->ReplaceFirst(strTmpSortParam, "jqclztdm", "sortdm");
		}

		if (m_pString->Find(strTmpSortParam, "bjsj"))
		{
			strTmpSortParam = m_pString->ReplaceFirst(strTmpSortParam, "bjsj", "receiving_time");
		}
		if (m_pString->Find(strTmpSortParam, "jjsj"))
		{
			strTmpSortParam = m_pString->ReplaceFirst(strTmpSortParam, "bjsj", "received_time");
		}
		l_tSQLReq.param["sort_parm"] = strTmpSortParam;
	}
	else
	{
		l_tSQLReq.param["sort_parm"] = "receiving_time desc";
	}

	//查询当前页记录	
	l_tSQLReq.sql_id = SELECT_TODAY_ALARM_UP_ATTENTION;

	l_tSQLReq.param["attention_staff"] = request.m_oBody.m_strAttentionCode;
	l_tSQLReq.param["query_condition"] = strCondition;
	l_tSQLReq.param["page_size"] = request.m_oBody.m_strPageSize;
	l_tSQLReq.param["page_index"] = request.m_oBody.m_strPageIndex;
	l_tSQLReq.param["staff_code"] = request.m_oBody.m_strAttentionCode;
	
	pResult = m_pDBConn->Exec(l_tSQLReq, true);
	ICC_LOG_DEBUG(m_pLog, "Excecute sql[%s]", pResult->GetSQL().c_str());
	if (!pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", pResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "_QueryTodayAlarmUpAttention query complete");

	return true;
}

//获取今日警情
void CBusinessImpl::OnCNotifiGetTodayAlarm(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string strMessage;
	if (p_pNotify == nullptr)
	{
		//strMessage = _ReadFile();
		ICC_LOG_ERROR(m_pLog, "OnCNotifiGetTodayAlarm notify is null.");
		return;
	}
	else
	{
		strMessage = p_pNotify->GetMessages();
	}
	

	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CTodayAlarm request;
	std::string l_strRecvMsg(strMessage);
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[GetTodayAlarm]Parse request error.");
		return;
	}
	ICC_LOG_INFO(m_pLog, "TodayAlarm page_size=%s,page_index=%s", request.m_oBody.m_strPageSize.c_str(), request.m_oBody.m_strPageIndex.c_str());

	PROTOCOL::CTodayAlarm response;
	response.m_oBody.m_strCount = "0";
	response.m_oBody.m_strAllCount = "0";
	response.m_oHeader.m_strResult = "0";
	response.m_oHeader.m_strMsg = "success";
	GenRespHeader(GET_TODAY_ALARM_RESPOND, request.m_oHeader, response.m_oHeader);

	do 
	{
		if (request.m_oBody.m_strPageSize.empty() || request.m_oBody.m_strPageIndex.empty())
		{
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "param error!";
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "OnCNotifiGetTodayAlarm 111111");

		DateTime::CDateTime datetime = m_pDateTime->CurrentDateTime();
		int64 l_IntervalTime = m_IntervalTime;
		std::string strInterval;
		if (!request.m_oBody.m_strDays.empty())
		{
			if (request.m_oBody.m_strDays == "0")
			{
				datetime = m_pDateTime->AddDays(datetime, -1);
				std::vector<std::string> splitvector;
				std::string strdatetime = m_pDateTime->ToString(datetime);
				m_pString->Split(strdatetime, " ", splitvector, true);
				if (splitvector.empty())
				{
					ICC_LOG_DEBUG(m_pLog, "datetime in null");
					return;
				}
				strdatetime = m_pString->Format("%s 16:00:00", splitvector[0].c_str());
				strInterval = strdatetime;
			}
			else
			{
				int l_nDays = std::atoi(request.m_oBody.m_strDays.c_str());
				if (l_nDays > 0 && l_nDays <= 7)
				{
					l_IntervalTime = m_IntervalTime * l_nDays;
				}
				datetime = m_pDateTime->AddSeconds(datetime, -l_IntervalTime);
				strInterval = m_pDateTime->ToString(datetime);
			}
		}	

		//查询记录总数
		DataBase::IResultSetPtr l_result = nullptr;		
		bool bQuery = false;
		if (request.m_oBody.m_strUpAttention == "1")
		{
			bQuery = _QueryTodayAlarmUpAttention(request, strInterval, l_result, response.m_oBody.m_strAllCount);
		}
		else
		{
			bQuery = _QueryTodayAlarmNotUpAttention(request, strInterval, l_result, response.m_oBody.m_strAllCount);
		}

		if (!bQuery || l_result == nullptr)
		{
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "exec sql failed!";
			 break;
		}				

		response.m_oBody.m_strCount = m_pString->Format("%d", l_result->RecordSize());
		
		while (l_result->Next())
		{			
			PROTOCOL::CAlarmInfo data;
			if (!data.ParseAlarmRecord(l_result))
			{
				ICC_LOG_ERROR(m_pLog, "Parse record failed.");
				response.m_oHeader.m_strResult = "1";
				response.m_oHeader.m_strMsg = "parse record failed!";
				break;
			}

			if (0 != request.m_oBody.m_strIgnorePrivacy.compare("true"))
			{
				if (0 == data.m_strPrivacy.compare("1"))
				{
					data.m_strCallerNo = "******";
					data.m_strCallerName = "******";
					data.m_strCallerAddr = "******";
					data.m_strCallerID = "******";
					data.m_strCallerIDType = "******";
					data.m_strCallerGender = "******";					
					data.m_strContactNo = "******";					
				}
			}
			//追加 三个字段    关注警情  移交给我的警情
			// 重大警情
			if (request.m_oBody.m_strIsMajorAlarmFlag == "1")
			{
				data.m_strMajorAlarmFlag = "1";
			}
			if (request.m_oBody.m_strIsAttentionAlarmFlag == "1")
			{
				data.m_strAttentionAlarmFlag = "1";
			}
			if (!request.m_oBody.m_strReceiverCode.empty())
			{
				data.m_strReceiverAlarmFlag = "1";
			}

			response.m_oBody.m_vecData.push_back(data);
		}

	} while (false);

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	response.m_oBody.m_vecData.clear();
	p_pNotify->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "TodayAlarm Info Msg Num [%s],msgID:%s,%s", response.m_oBody.m_strCount.c_str(), response.m_oHeader.m_strMsgId.c_str(), l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetSimilarAlarm(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSimilarAlarm request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[GetSimilarAlarm] Parse request error.");
		return;
	}

	std::string l_strStartTime = request.m_oBody.m_strStartTime;
	std::string l_strPhoneNo = request.m_oBody.m_strPhoneNo;
	std::string l_strContactNo = request.m_oBody.m_strContactNo;
	std::string l_strAddr = request.m_oBody.m_strAddr;

	PROTOCOL::CSimilarAlarm response;
	response.m_oBody.m_strCount = "0";
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;

	if (l_strStartTime.empty() 
		|| (l_strPhoneNo.empty() && l_strContactNo.empty() && l_strAddr.empty()))
	{
		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "param error!";

		std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
		response.m_oBody.m_vecData.clear();
		ICC_LOG_DEBUG(m_pLog, "SimilarAlarm Info Respond [%s], size[%d]", l_strMessage.c_str(), response.m_oBody.m_vecData.size());
		p_pNotify->Response(l_strMessage);

		return;
	}

	std::map<std::string, std::string> l_mapTemp;
	if (!l_strPhoneNo.empty())
	{
		l_mapTemp.insert(std::make_pair("bjdh", l_strPhoneNo));
	}
	if (!l_strContactNo.empty())
	{
		l_mapTemp.insert(std::make_pair("lxdh", l_strContactNo));
	}
	if (!l_strAddr.empty())
	{
		l_mapTemp.insert(std::make_pair("jqdz", l_strAddr));
	}

	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = "select_similar_alarm";
	tmp_oSQLRequest.param["receiving_time"] = l_strStartTime;
	tmp_oSQLRequest.param["cur_time"] = m_pDateTime->CurrentDateTimeStr();
	
	std::string tmp_strLikeCondition;
	int l_nIndex = 0;
	for (auto it : l_mapTemp)
	{
		if (0 == l_nIndex++)
		{
			//l_strSql.append(" and (");
		} 
		else
		{
			tmp_strLikeCondition.append(" or ");
		}

		tmp_strLikeCondition.append(it.first + " like '%" + it.second + "%'");
	}

	if (!tmp_strLikeCondition.empty())
	{
		tmp_oSQLRequest.param["like_condition"] = tmp_strLikeCondition;
	}
	
	//l_strSql.append(") order by id desc;");
	tmp_oSQLRequest.param["orderby"] = "bjsj desc";

	if (request.m_oBody.m_strSourceType == "09")
	{
		tmp_oSQLRequest.param["telhotline"] = "12345";
		tmp_oSQLRequest.param["start_time"] = l_strStartTime;
	}
	tmp_oSQLRequest.param["limit"] = "20";
	tmp_oSQLRequest.param["offset"] = "0";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "get similar alarm , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "exec sql error!";
		response.m_oBody.m_vecData.clear();

		std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
		ICC_LOG_DEBUG(m_pLog, "Similar Alarm Info Respond [%s], size[%d]", l_strMessage.c_str(), response.m_oBody.m_vecData.size());
		p_pNotify->Response(l_strMessage);

		return;
	}

	int l_nCount = 0;
	while (l_result->Next())
	{
		l_nCount++;

		if (l_nCount > 100)
		{
			ICC_LOG_WARNING(m_pLog, "Similar Alarm Info is too much, will save 100");
			break;
		}

		PROTOCOL::CAlarmInfo data;
		//data.m_strID = l_result->GetValue("id");
		//data.m_strMergeID = l_result->GetValue("merge_id");
		//data.m_strTitle = l_result->GetValue("title");
		//data.m_strContent = l_result->GetValue("content");
		//data.m_strTime = l_result->GetValue("time");
		////data.m_strActualOccurTime = l_result->GetValue("actual_occur_time");
		//data.m_strAddr = l_result->GetValue("addr");
		//data.m_strLongitude = l_result->GetValue("longitude");
		//data.m_strLatitude = l_result->GetValue("latitude");
		//data.m_strState = l_result->GetValue("state");
		//data.m_strLevel = l_result->GetValue("level");
		//data.m_strSourceType = l_result->GetValue("source_type");
		//data.m_strSourceID = l_result->GetValue("source_id");
		//data.m_strHandleType = l_result->GetValue("handle_type");
		//data.m_strFirstType = l_result->GetValue("first_type");
		//data.m_strSecondType = l_result->GetValue("second_type");
		//data.m_strThirdType = l_result->GetValue("third_type");
		//data.m_strFourthType = l_result->GetValue("fourth_type");
		//data.m_strVehicleNo = l_result->GetValue("vehicle_no");
		//data.m_strVehicleType = l_result->GetValue("vehicle_type");
		//data.m_strSymbolCode = l_result->GetValue("symbol_code");
		////data.m_strSymbolAddr = l_result->GetValue("symbol_addr");
		////data.m_strFireBuildingType = l_result->GetValue("fire_building_type");
		////data.m_strDispatchSuggestion = l_result->GetValue("dispatch_suggestion");
		////data.m_strEventType = l_result->GetValue("event_type");
		//data.m_strCalledNoType = l_result->GetValue("called_no_type");
		////data.m_strActualCalledNoType = l_result->GetValue("actual_called_no_type");
		//data.m_strCallerNo = l_result->GetValue("caller_no");
		//data.m_strCallerName = l_result->GetValue("caller_name");
		//data.m_strCallerAddr = l_result->GetValue("caller_addr");
		//data.m_strCallerID = l_result->GetValue("caller_id");
		//data.m_strCallerIDType = l_result->GetValue("caller_id_type");
		//data.m_strCallerGender = l_result->GetValue("caller_gender");
		////data.m_strCallerAge = l_result->GetValue("caller_age");
		////data.m_strCallerBirthday = l_result->GetValue("caller_birthday");
		//data.m_strContactNo = l_result->GetValue("contact_no");
		////data.m_strContactName = l_result->GetValue("contact_name");
		////data.m_strContactAddr = l_result->GetValue("contact_addr");
		////data.m_strContactID = l_result->GetValue("contact_id");
		////data.m_strContactIDType = l_result->GetValue("contact_id_type");
		////data.m_strContactGender = l_result->GetValue("contact_gender");
		////data.m_strContactAge = l_result->GetValue("contact_age");
		////data.m_strContactBirthday = l_result->GetValue("contact_birthday");
		////data.m_strAdminDeptDistrictCode = l_result->GetValue("admin_dept_district_code");
		//data.m_strAdminDeptCode = l_result->GetValue("admin_dept_code");
		//data.m_strAdminDeptName = l_result->GetValue("admin_dept_name");
		//data.m_strReceiptDeptDistrictCode = l_result->GetValue("receipt_dept_district_code");
		//data.m_strReceiptDeptCode = l_result->GetValue("receipt_dept_code");
		//data.m_strReceiptDeptName = l_result->GetValue("receipt_dept_name");
		////data.m_strLeaderCode = l_result->GetValue("leader_code");
		////data.m_strLeaderName = l_result->GetValue("leader_name");
		//data.m_strReceiptCode = l_result->GetValue("receipt_code");
		//data.m_strReceiptName = l_result->GetValue("receipt_name");
		////data.m_strReceiptSeatno = l_result->GetValue("receipt_seatno");
		//data.m_strCreateUser = l_result->GetValue("create_user");
		//data.m_strCreateTime = l_result->GetValue("create_time");
		//data.m_strUpdateUser = l_result->GetValue("update_user");
		//data.m_strUpdateTime = l_result->GetValue("update_time");
		//data.m_strIsMerge = l_result->GetValue("is_merge");
		//data.m_strIsFeedBack = l_result->GetValue("is_feedback");
		//data.m_strIsVisitor = l_result->GetValue("is_visitor");
		//data.m_strPrivacy = l_result->GetValue("is_privacy");
		//data.m_strRemark = l_result->GetValue("remark");

		if (!data.ParseAlarmRecord(l_result))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}
		
		response.m_oBody.m_vecData.push_back(data);
	}

	ICC_LOG_DEBUG(m_pLog, "SimilarAlarm Info Msg Num [%d]", l_nCount);
	response.m_oBody.m_strCount = m_pString->Number(l_nCount);
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	response.m_oBody.m_vecData.clear();
	p_pNotify->Response(l_strMessage);
}


std::string CBusinessImpl::_BuildMultiConditions(std::string p_strColumnName, std::string p_strOperator, std::string p_strConditionToSpilt)
{
	std::string l_strResult;
	vector<std::string> l_vecCondition;
	m_pString->Split(p_strConditionToSpilt, ",;", l_vecCondition, true);
	for (std::string l_strCondition : l_vecCondition)
	{
		if (l_strCondition.empty())
		{
			continue;
		}

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

void ICC::CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader)
{
	p_pRespHeader.m_strMsgId = p_pRequestHeader.m_strMsgId;

	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;	
}



void CBusinessImpl::OnNotifiGetCallCountStatistics(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg = p_pNotify->GetMessages();

	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CTodayCallCountRequest request;	
	if (!request.ParseString(l_strRecvMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetCallCountStatistics]Parse request error.");
		return;
	}

	PROTOCOL::CTodayCallCountRespond response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	std::string queryCallInCount, queryCallOutCount, queryCallReleaseCount, queryCallTalkTime;
	if (request.m_oBody.m_strIsRecursive == "1") {
		queryCallInCount = "query_call_in_count_by_recursive";
		queryCallOutCount = "query_call_out_count_by_recursive";
		queryCallReleaseCount = "query_call_release_count_by_recursive";
		queryCallTalkTime = "query_talktime_by_recursive";
	}
	else {
		queryCallInCount = "query_call_in_count";
		queryCallOutCount = "query_call_out_count";
		queryCallReleaseCount = "query_call_release_count";
		queryCallTalkTime = "query_talktime";
	}
	response.m_oBody.m_strCallInCount = _QueryCallInCount(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, queryCallInCount);
	response.m_oBody.m_strCallOutCount = _QueryCallOutCount(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, queryCallOutCount);
	response.m_oBody.m_strReleaseCount = _QueryCallReleaseCount(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, queryCallReleaseCount);
	response.m_oBody.m_strAverageTalkTime = _QueryCallAverageTalkTime(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, queryCallTalkTime);
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());	
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send response message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetCallInCountByPeriod(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CTodayCallInStatisticsRequest request;
	if (!request.ParseString(l_strRecvMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetCallInCountByPeriod]Parse request error.");
		return;
	}

	PROTOCOL::CTodayCallStatisticsRespond response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	request.m_oBody.m_strIncrement = "60";
	_QueryCallInByPeriod(request, response);	

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send response message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetAlarmCountByReceivedType(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CTodayStatisticsByReceiveTypeRequest request;
	if (!request.ParseString(l_strRecvMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAlarmCountByReceivedType]Parse request error.");
		return;
	}

	PROTOCOL::CTodayStatisticsByReceiveTypeRespond response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;

	_QueryAlarmByReceiveType(request, response);

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send response message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetAlarmCountByFirstType(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CTodayStatisticsByFirstTypeRequest request;
	if (!request.ParseString(l_strRecvMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAlarmCountByFirstType]Parse request error.");
		return;
	}

	PROTOCOL::CTodayStatisticsByFirstTypeRespond response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	
	_QueryAlarmByFirstType(request, response);

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send response message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetAlarmInfoByFirstType(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg = p_pNotify->GetMessages();	
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strRecvMsg.c_str());

	PROTOCOL::CTodayAlarmInfoByFirstTypeRequest request;
	if (!request.ParseString(l_strRecvMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAlarmInfoByFirstType]Parse request error.");
		return;
	}

	PROTOCOL::CTodayAlarmInfoByFirstTypeRespond response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;

	_QueryAlarmInfoByFirstType(request, response);

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send response message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::_QueryAlarmInfoByFirstType(const PROTOCOL::CTodayAlarmInfoByFirstTypeRequest& request, PROTOCOL::CTodayAlarmInfoByFirstTypeRespond& response)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.param["start_time"] = request.m_oBody.m_strBeginTime;
	tmp_oSQLRequest.param["end_time"] = request.m_oBody.m_strEndTime;
	tmp_oSQLRequest.param["dept_code"] = request.m_oBody.m_strDeptCode;
	tmp_oSQLRequest.param["first_type"] = request.m_oBody.m_strFirstType;
	if (request.m_oBody.m_strIsRecursive == "1") 
	{
		tmp_oSQLRequest.sql_id = "query_alarm_info_by_first_type_by_recursive";
	}
	else 
	{
		tmp_oSQLRequest.sql_id = "query_alarm_info_by_first_type";
	}

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "query alarm by receive type , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return ;
	}

	response.m_oBody.m_strCount = std::to_string(l_result->RecordSize());
	while (l_result->Next())
	{
		PROTOCOL::CAlarmInfo data;		
		if (!data.ParseAlarmRecordEx(l_result))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}

		response.m_oBody.m_vecDatas.push_back(data);
	}
}

void CBusinessImpl::CalrQueryCallInCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId, std::map<std::string, std::string>& mapData)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = strSqlId;// "query_call_in_count";
	tmp_oSQLRequest.param["start_time"] = strStartTime;
	tmp_oSQLRequest.param["end_time"] = strEndTime;
	tmp_oSQLRequest.param["dept_code"] = strDeptCode;

	std::string strResult = "0";
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "get call in count , sql = [%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return ;
	}
	while (l_result->Next())
	{
		mapData[l_result->GetValue("hour")] = l_result->GetValue("count");
	}
	return ;
}

std::string CBusinessImpl::_QueryCallInCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = strSqlId;// "query_call_in_count";
	tmp_oSQLRequest.param["start_time"] = strStartTime;
	tmp_oSQLRequest.param["end_time"] = strEndTime;
	tmp_oSQLRequest.param["dept_code"] = strDeptCode;
	std::string l_autoUrgeCallerNum;
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum))
	{
		tmp_oSQLRequest.param["autourge_caller"] = l_autoUrgeCallerNum;
	}
	std::string strResult = "0";

	do 
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
		ICC_LOG_DEBUG(m_pLog, "get call in count , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			strResult = l_result->GetValue("count");
		}

	} while (false);	

	return strResult;
}

std::string CBusinessImpl::_QueryCallOutCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = strSqlId;
	tmp_oSQLRequest.param["start_time"] = strStartTime;
	tmp_oSQLRequest.param["end_time"] = strEndTime;
	tmp_oSQLRequest.param["dept_code"] = strDeptCode;
	std::string l_autoUrgeCallerNum;
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum))
	{
		tmp_oSQLRequest.param["autourge_caller"] = l_autoUrgeCallerNum;
	}
	std::string strResult = "0";

	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
		ICC_LOG_DEBUG(m_pLog, "get call out , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			strResult = l_result->GetValue("count");
		}

	} while (false);

	return strResult;
}

std::string CBusinessImpl::_QueryCallReleaseCount(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = strSqlId;
	tmp_oSQLRequest.param["start_time"] = strStartTime;
	tmp_oSQLRequest.param["end_time"] = strEndTime;
	tmp_oSQLRequest.param["dept_code"] = strDeptCode;
	std::string l_autoUrgeCallerNum;
	if (m_pRedisClient->HGet("AutoUrgeInfo", "Caller_Num", l_autoUrgeCallerNum))
	{
		tmp_oSQLRequest.param["autourge_caller"] = l_autoUrgeCallerNum;
	}
	std::string strResult = "0";

	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
		ICC_LOG_DEBUG(m_pLog, "get call release , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			strResult = l_result->GetValue("count");
		}

	} while (false);

	return strResult;
}

std::string CBusinessImpl::_QueryCallAverageTalkTime(const std::string& strStartTime, const std::string& strEndTime, const std::string& strDeptCode, const std::string& strSqlId)
{
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = strSqlId;
	tmp_oSQLRequest.param["start_time"] = strStartTime;
	tmp_oSQLRequest.param["end_time"] = strEndTime;
	tmp_oSQLRequest.param["dept_code"] = strDeptCode;

	std::string strResult = "0";

	do
	{
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
		ICC_LOG_DEBUG(m_pLog, "get averate talk time , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		int iTotalTalkTime = 0;
		int iRecordCount = l_result->RecordSize();
		while (l_result->Next())
		{
			std::string strTalkTime = l_result->GetValue("talktime");
			if (!strTalkTime.empty())
			{
				iTotalTalkTime += std::stoi(strTalkTime);
			}
		}

		int iAverageTime = 0;
		if (iRecordCount != 0)
		{
			iAverageTime = iTotalTalkTime / iRecordCount;
		}
		
		strResult = std::to_string(iAverageTime);

	} while (false);

	return strResult;
}

void CBusinessImpl::_QueryCallInByPeriod(const PROTOCOL::CTodayCallInStatisticsRequest& request, PROTOCOL::CTodayCallStatisticsRespond& response)
{
	std::map<std::string, std::string> mapTimePeriods;
	// 起始时间 终止时间 间隔时间 时间容器 回复 
	_AsignedPeriod(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strIncrement, mapTimePeriods, response);
	// 回复给前端的数量  需要回复前端 数组
	int mapSize = mapTimePeriods.size();
	int index = 0;
	response.m_oBody.m_strCount = std::to_string(mapSize);
	std::map<std::string, std::string> result;
	if (request.m_oBody.m_strIsRecursive == "1") {
		CalrQueryCallInCount(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, "query_call_in_count_by_period_by_recursive", result);
	}
	else {
		CalrQueryCallInCount(request.m_oBody.m_strBeginTime, request.m_oBody.m_strEndTime, request.m_oBody.m_strDeptCode, "query_call_in_count_by_period", result);
	}
	if (result.empty()) {
		return;
	}

	// 如果查询到的数据 落在同一个时间区间  要不要排个序 map就不需要从头循环 不从头循环的话，索引需要同步更新 
	std::map<std::string, std::string>::const_iterator itStart = mapTimePeriods.begin();
	for (auto itrData = result.begin(); itrData != result.end(); ++itrData)
	{
		if (mapTimePeriods.find(itrData->first) != mapTimePeriods.end()) {
			int diffTime = m_pDateTime->SecondsDifference(m_pDateTime->FromString(itrData->first), m_pDateTime->FromString(mapTimePeriods.begin()->first)) / 60 / std::atoi(request.m_oBody.m_strIncrement.c_str());
			response.m_oBody.m_vecDatas[diffTime].m_strCallCount = itrData->second;
		}
	}
	return;
}


void CBusinessImpl::_QueryAlarmByReceiveType(const PROTOCOL::CTodayStatisticsByReceiveTypeRequest& request, PROTOCOL::CTodayStatisticsByReceiveTypeRespond& response)
{
	// 避免分组查询 结果数据为0 的数据
	_AanalyzeTypesByByReceive(request.m_oBody.m_strReceiveTypes, response);
	int size = response.m_oBody.m_vecDatas.size();
	response.m_oBody.m_strCount = std::to_string(size);
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.param["start_time"] = request.m_oBody.m_strBeginTime;
	tmp_oSQLRequest.param["end_time"] = request.m_oBody.m_strEndTime;
	tmp_oSQLRequest.param["dept_code"] = request.m_oBody.m_strDeptCode;
	if (request.m_oBody.m_strIsRecursive == "1") {
		tmp_oSQLRequest.sql_id = "query_alarm_count_by_receive_type_by_recursive";
	}
	else {
		tmp_oSQLRequest.sql_id = "query_alarm_count_by_receive_type";
	}

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "query alarm by receive type , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
	}

	while (l_result->Next())
	{
		for (int index = 0; index < size; index++) {
			if (response.m_oBody.m_vecDatas[index].m_strReceiveType == l_result->GetValue("jjlx")) {
				response.m_oBody.m_vecDatas[index].m_strCount = l_result->GetValue("count");
				break;
			}
		}
	}
}

void CBusinessImpl::_QueryAlarmByFirstType(const PROTOCOL::CTodayStatisticsByFirstTypeRequest& request, PROTOCOL::CTodayStatisticsByFirstTypeRespond& response)
{
	// 避免分组查询 结果数据为0 的数据
	_AanalyzeTypesByFirstType(request.m_oBody.m_strFirstTypes, response);
	int size = response.m_oBody.m_vecDatas.size();
	response.m_oBody.m_strCount = std::to_string(size);
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.param["start_time"] = request.m_oBody.m_strBeginTime;
	tmp_oSQLRequest.param["end_time"] = request.m_oBody.m_strEndTime;
	tmp_oSQLRequest.param["dept_code"] = request.m_oBody.m_strDeptCode;
	if (request.m_oBody.m_strIsRecursive == "1") {
		tmp_oSQLRequest.sql_id = "query_alarm_count_by_first_type_by_recursive";
	}
	else {
		tmp_oSQLRequest.sql_id = "query_alarm_count_by_first_type";
	}
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "query alarm by first_type , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
	}

	while (l_result->Next())
	{
		for (int index = 0; index < size; index++) {
			if (response.m_oBody.m_vecDatas[index].m_strFirstType == l_result->GetValue("jqlbdm")) {
				response.m_oBody.m_vecDatas[index].m_strCount = l_result->GetValue("count");
				break;
			}
		}
	}

}

void CBusinessImpl::_AsignedPeriod(const std::string& strStartTime, const std::string& strEndTime, const std::string& strIncrementTime, std::map<std::string, std::string>& mapPeriods, PROTOCOL::CTodayCallStatisticsRespond& response)
{
	if (strStartTime.size() != 19) {
		return;
	}
	std::string tempstr1 = strStartTime.substr(0, 13);
	std::string tempstr2 = strEndTime.substr(0, 13);
	DateTime::CDateTime startTime = m_pDateTime->FromString(tempstr1 + ":00:00");
 	DateTime::CDateTime endTime = m_pDateTime->FromString(tempstr2 + ":00:00");
	DateTime::CDateTime lasttime = m_pDateTime->FromString(strEndTime);
	std::string finaltime = m_pDateTime->ToString(lasttime);
	int iIncTime = m_pString->ToInt(strIncrementTime);
	DateTime::CDateTime tmpEndTime;
	startTime = m_pDateTime->AddMinutes(startTime, iIncTime);
	PROTOCOL::CTodayCallStatisticsRespond::CData t_Data;
	do 
	{
		std::string strTmpStartTime;
		std::string strTmpEndTime;
		
		strTmpStartTime = m_pDateTime->ToString(startTime);

		tmpEndTime = m_pDateTime->AddMinutes(startTime, iIncTime);
		if (tmpEndTime < endTime)
		{
			strTmpEndTime = m_pDateTime->ToString(tmpEndTime);
			startTime = tmpEndTime;
		}
		else
		{			
			strTmpEndTime = m_pDateTime->ToString(endTime);			
		}

		mapPeriods.insert(std::make_pair(strTmpStartTime, strTmpEndTime));
		t_Data.m_strStartTime = strTmpStartTime;
		t_Data.m_strEndTime = strTmpEndTime;
		t_Data.m_strCallCount = "0";
		response.m_oBody.m_vecDatas.push_back(t_Data);
	} while (tmpEndTime < endTime);
	mapPeriods.insert(std::make_pair(m_pDateTime->ToString(tmpEndTime), finaltime));
	t_Data.m_strStartTime = m_pDateTime->ToString(tmpEndTime);
	t_Data.m_strEndTime = finaltime;
	t_Data.m_strCallCount = "0";
	response.m_oBody.m_vecDatas.push_back(t_Data);
	return;
}

void CBusinessImpl::_AanalyzeTypesByByReceive(const std::string& strTypes, PROTOCOL::CTodayStatisticsByReceiveTypeRespond& response)
{
	std::vector<std::string> l_vecCondition;
	m_pString->Split(strTypes, ";", l_vecCondition, false);
	for (int i = 0; i < l_vecCondition.size(); ++i)
	{
		if (!l_vecCondition[i].empty())
		{
			PROTOCOL::CTodayStatisticsByReceiveTypeRespond::CData t_Data;
			t_Data.m_strReceiveType = l_vecCondition[i];
			t_Data.m_strCount = "0";
			response.m_oBody.m_vecDatas.push_back(t_Data);
		}
	}
}

void CBusinessImpl::_AanalyzeTypesByFirstType(const std::string& strTypes, PROTOCOL::CTodayStatisticsByFirstTypeRespond& response)
{
	std::vector<std::string> l_vecCondition;
	m_pString->Split(strTypes, ";", l_vecCondition, false);
	for (int i = 0; i < l_vecCondition.size(); ++i)
	{
		if (!l_vecCondition[i].empty())
		{
			PROTOCOL::CTodayStatisticsByFirstTypeRespond::CData t_Data;
			t_Data.m_strFirstType = l_vecCondition[i];
			t_Data.m_strCount = "0";
			response.m_oBody.m_vecDatas.push_back(t_Data);
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

std::string CBusinessImpl::_QueryMajorAlarmTypeNames(std::map<std::string, std::string>& vecTypeNames)
{
	std::string l_strDst;

	vecTypeNames["jqlbdm"] = "";
	vecTypeNames["jqlxdm"] = "";
	vecTypeNames["jqxldm"] = "";
	vecTypeNames["jqzldm"] = "";
	vecTypeNames["jqdjdm"] = "";
	DataBase::SQLRequest tmp_oSQLRequest;
	tmp_oSQLRequest.sql_id = "query_icc_t_major_alarm_config_alltypename";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(tmp_oSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "get major alarm type names , sql = [%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return "";
	}

	while (l_result->Next())
	{
		std::string strTypeName = l_result->GetValue("type_name");
		if (!strTypeName.empty())
		{
			vecTypeNames[strTypeName] = strTypeName;
		}
	}

	l_strDst = "(jqlbdm in (select type_value from icc_t_major_alarm_config where type_name='" + vecTypeNames["jqlbdm"] + "' and type_name != '')) or (jqlxdm in (select type_value from icc_t_major_alarm_config where type_name='" + vecTypeNames["jqlxdm"] + "' and type_name != '')) or (jqxldm in (select type_value from icc_t_major_alarm_config where type_name='" + vecTypeNames["jqxldm"] + "' and type_name != '')) or (jqzldm in (select type_value from icc_t_major_alarm_config where type_name='" + vecTypeNames["jqzldm"] + "' and type_name != '')) or (jqdjdm in (select type_value from icc_t_major_alarm_config where type_name='" + vecTypeNames["jqdjdm"] + "' and type_name != '')) ";

	return l_strDst;
}

std::string CBusinessImpl::_ReadFile()
{
	if (!m_pJsonFty)
	{
		return "";
	}

	JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
	if (!pJson)
	{
		return "";
	}

	if (!pJson->LoadFile("d:/test.txt"))
	{
		return "";
	}

	return pJson->ToString();
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
	m_strDispatchMode = l_pCfgReader->GetValue("ICC/Plugin/TodayAlarm/procthreaddispatchmode", "1");
	m_uProcThreadCount = m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/TodayAlarm/procthreadcount", "8"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (m_uProcThreadCount > uConCurrency)
	{
		m_uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "todayalarm proc thread count:%d, mode:%s", m_uProcThreadCount, m_strDispatchMode.c_str());

	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			pThread->SetLogPtr(m_pLog);
			m_vecProcThreads.push_back(pThread);
		}
		else
		{
			ICC_LOG_INFO(m_pLog, "dbagent proc thread count:%d", m_uProcThreadCount);
		}
	}

	m_uProcThreadCount = m_vecProcThreads.size();

	ICC_LOG_INFO(m_pLog, "todayalarm real proc thread count:%d", m_uProcThreadCount);
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

			ICC_LOG_LOWDEBUG(m_pLog, "today alarm THREADID_RECEIVENOTIFY %u begin! cmd:%s, guid:%s",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str());

			_DispatchNotify(msg_data.pNotifyPtr, msg_data.str_msg);

			std::chrono::system_clock::time_point curEnd;
			curEnd = std::chrono::system_clock::now();
			unsigned long long ullInterval = std::chrono::duration_cast<std::chrono::microseconds>(curEnd.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::microseconds>(curBegin.time_since_epoch()).count();


			ICC_LOG_LOWDEBUG(m_pLog, "today alarm THREADID_RECEIVENOTIFY %u end! cmd:%s, guid:%s, exec time:%lld",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str(), ullInterval);

			if (ullInterval > TIMOUT_INTERVAL)
			{
				ICC_LOG_WARNING(m_pLog, "exec slow! threadid:%u time:%llu, message:%s", msg_data.uThreadId,ullInterval, msg_data.pNotifyPtr->GetMessages().c_str());
			}
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception! cmd:%s", msg_data.str_msg.c_str());
		}
	}
	break;

	default:
		break;
	}
}

