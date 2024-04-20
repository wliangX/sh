#include <thread>
#include "Boost.h"
#include "BusinessImpl.h"

#define LOAD_WAITTIME 1000

#define BS001310001 "BS001310001"				//转入工单流程办理流水名称
#define BS001310002 "BS001310002"				//转入工单延期申请流水名称
#define BS001310003 "BS001310003"				//转入工单办理状态流水名称
#define BS001310005 "BS001310005"				//转入工单办理结果流水名称
#define BS001310008 "BS001310008"				//推送警单失败流水名称
#define BS001310009 "BS001310009"				//转入工单确认流水名称
#define BS001310010 "BS001310010"				//转入工单未确认流水名称

#define BS001310011 "BS001310011"				//转入工单办理反馈流水名称
#define BS001310012 "BS001310012"				//转入工单办理退回流水名称
#define BS001310013 "BS001310013"				//转入工单回访信息流水名称

#define PARAM_INFO	"ParamInfo"


CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}
void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(OBSERVER_CENTER_ALARM_TELHOTLINE);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_strNacosServerIp = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerIp", "127.0.0.1");
	m_strNacosServerPort = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerPort", "8848");
	m_strNacosServerNamespace = m_pConfig->GetValue("ICC/Component/HttpServer/NacosNamespace", "dev");
	m_strNacosServerGroupName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosGroupName", "global");
	m_strLang_code = m_pConfig->GetValue("ICC/Plugin/Synthetical/langCode", "zh-CN");

	m_sendMsgOverTime = m_pConfig->GetValue("ICC/Plugin/Synthetical/TelHotlineOverTime", "3");

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	m_nMsgCenterExecTime = 300;//失败重传等待时间
	m_nMsgCenterExecCount = 5;//失败重传次数

	//12345转110
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_telhotline_visit_request", OnCNotifiTHLVisitRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_telhotline_confirm_request", OnCNotifiTHLConfirmRequest);

	//110转12345
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "repulse_alarm_to12345", OnRepulseAlarmTo12345);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "push_alarm_to12345", OnPushAlarmTo12345);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "feedback_sync", OnPushRevisitTo12345);//接收同步信息后向12345推送反馈单信息

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "msgcenter_send_failed_retransmission", OnMsgCenterRetransmission);

	//泸州保留部分
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_telhotline_process", OnCNotifiTHLAddRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_telhotline_fpid", OnCNotifilTHLFpideRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_telhotline_handlestate", OnCNotifiTHLVisitRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_telhotlinet_visit_request", OnCNotifiTHLHandleStatRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_telhotlinet_result", OnCNotifiTHLRequest);

	if (m_pTimerMgr && m_nMsgCenterExecTime > 0)
	{
		//启动1s的违规检测定时器
		m_pTimerMgr->AddTimer("msgcenter_send_failed_retransmission", 10, 0);
	}

	ICC_LOG_INFO(m_pLog, "plugin basedata.acd start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "acd stop success");
}

void CBusinessImpl::OnDestroy()
{

}
void CBusinessImpl::OnCNotifiTHLConfirmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CTelHotlineConfirmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	//回复
	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;

	PROTOCOL::CAlarmLogSync l_oTHLData;
	std::vector<std::string> l_vecParamList;
	l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strConfigMsg;

	if (l_oRequest.m_oBody.m_strReceiveState == "1")
	{
		//更新联动表信息
		UpdateLinkedTableState(l_oRequest.m_oBody.m_strAlarmID, "08");

		m_pMsgCenter->BuildManualAcceptConfig("12345GetConfirm", l_strConfigMsg);
		l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", l_oRequest.m_oBody.m_strAlarmID);

		l_oTHLData.m_oBody.m_strOperate = BS001310009;
	}
	else if (l_oRequest.m_oBody.m_strReceiveState == "0")
	{
		//更新联动表信息
		UpdateLinkedTableState(l_oRequest.m_oBody.m_strAlarmID, "07");

		m_pMsgCenter->BuildManualAcceptConfig("12345NotConfirm", l_strConfigMsg);
		l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", l_oRequest.m_oBody.m_strAlarmID);

		l_oTHLData.m_oBody.m_strOperate = BS001310010;
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "unknown receive state:[%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_strRequest = "1";
		l_oRespond.m_strMsg = "unknown receive state";
		return;
	}

	//发同步消息
	SyncLinkedTableState(l_oRequest.m_oBody.m_strAlarmID);

	//写流水
	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
		l_oRespond.m_strRequest = "1";
		l_oRespond.m_strMsg = "insert alarm log info failed";
	}
	else
	{
		SyncAlarmLogInfo(l_oTHLData.m_oBody);
	}

	//发送至消息中心
	m_pMsgCenter->Send(l_strConfigMsg, l_oRequest.m_oBody.m_strAlarmID, true, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	l_oRespond.m_strResult = "0";

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

bool CBusinessImpl::GetAlarmerName(std::string l_alarmerId, std::string& l_alarmName)
{
	ICC::Data::CStaffInfo l_StaffInfo;
	std::string strStaffInfo;
	if (m_pRedisClient->HGet(STAFF_INFO_MAP_KEY, l_alarmerId, strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap Failed!!!");
		return false;
	}
	if (l_StaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse StaffInfoMap Failed!!!");
		return false;
	}
	l_alarmName = l_StaffInfo.m_strName;

	return true;
}

void CBusinessImpl::BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value)
{
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, l_manualAcceptType, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
	}
	l_value = l_ParamInfo.m_strValue;
}

void CBusinessImpl::OnCNotifiTHLVisitRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CTelHotlineVisitRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	//回复
	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
	std::string linked_state;
	// 写流水 
	PROTOCOL::CAlarmLogSync l_oTHLData;
	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
	std::vector<std::string> l_vecParamList;
	std::string l_curMsg;
	if (l_oRequest.m_oBody.m_strSendReason == "3")
	{
		std::string p_strResult;
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendTime);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendReason);

		l_vecParamList.push_back(l_oRequest.m_oBody.m_strVisitTime);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strVisitContent);

		if (l_oRequest.m_oBody.m_strVisitResult == "1")
		{
			m_pMsgCenter->BuildManualAcceptConfig("12345ReturnVisitResult1", p_strResult);
			l_vecParamList.push_back(p_strResult);
		}
		else if (l_oRequest.m_oBody.m_strVisitResult == "2")
		{
			m_pMsgCenter->BuildManualAcceptConfig("12345ReturnVisitResult2", p_strResult);
			l_vecParamList.push_back(p_strResult);
		}
		else if (l_oRequest.m_oBody.m_strVisitResult == "3")
		{
			m_pMsgCenter->BuildManualAcceptConfig("12345ReturnVisitResult3", p_strResult);
			l_vecParamList.push_back(p_strResult);
		}
		else
		{
			m_pMsgCenter->BuildManualAcceptConfig("12345ReturnVisitResultErr", p_strResult);
			l_vecParamList.push_back(p_strResult);
		}

		l_vecParamList.push_back(l_oRequest.m_oBody.m_strTelVisitResult);

		l_oTHLData.m_oBody.m_strOperate = BS001310013;

		//构建消息中心信息
		m_pMsgCenter->BuildManualAcceptConfig("12345ReturnVisit", l_curMsg);
		l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strAlarmID);

		linked_state = "03";
	}
	else if (l_oRequest.m_oBody.m_strSendReason == "1")
	{
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendTime);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendReason);

		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedbackTime);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmDisposal);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmResult);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedbackId);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedbackName);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedbackDeptCode);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedbackDeptName);

		l_oTHLData.m_oBody.m_strOperate = BS001310011;

		//构建消息中心信息
		m_pMsgCenter->BuildManualAcceptConfig("12345Result", l_curMsg);
		l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strAlarmID);

		linked_state = "04";
	}
	else if (l_oRequest.m_oBody.m_strSendReason == "2")
	{
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendTime);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strSendReason);

		l_vecParamList.push_back(l_oRequest.m_oBody.m_strReturnAlarmer);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strReturnDeptName);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strReturnResult);
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strReturnTime);

		l_oTHLData.m_oBody.m_strOperate = BS001310012;

		//构建消息中心信息
		m_pMsgCenter->BuildManualAcceptConfig("12345Repulse", l_curMsg);
		l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strAlarmID);

		linked_state = "05";
	}

	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	do {
		//更新警单状态,并发送同步
		PROTOCOL::CAlarmInfo p_roAlarmSync;
		if (IsSingle12345Alarm(l_oRequest.m_oBody.m_strAlarmID))
		{
			if (!UpdateAlarmState(p_roAlarmSync, l_oRequest.m_oBody.m_strAlarmID, "04"))
			{
				l_oRespond.m_strRequest = "1";
				l_oRespond.m_strMsg = "execute update alarm state info failed";
				ICC_LOG_ERROR(m_pLog, "update alarm state info failed");
				break;
			}
			else
			{
				SyncAlarmInfo(p_roAlarmSync);
				l_oRespond.m_strResult = "0";
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "not Single12345Alarm jump update Alarm State");
		}

		//写流水
		if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
		{
			l_oRespond.m_strRequest = "1";
			l_oRespond.m_strMsg = "execute insert alarm log info failed";
			ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
			break;
		}
		else
		{
			SyncAlarmLogInfo(l_oTHLData.m_oBody);
			l_oRespond.m_strResult = "0";
		}

		//修改联动单位表状态
		if (!UpdateLinkedTableState(l_oRequest.m_oBody.m_strAlarmID, linked_state))
		{
			l_oRespond.m_strRequest = "1";
			l_oRespond.m_strMsg = "UpdateLinkedTableState failed";
			ICC_LOG_ERROR(m_pLog, "UpdateLinkedTableState failed");
			break;
		}

		//发同步消息
		SyncLinkedTableState(l_oRequest.m_oBody.m_strAlarmID);

		//发送至消息中心
		m_pMsgCenter->Send(l_curMsg, l_oRequest.m_oBody.m_strAlarmID, true, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
	} while (false);

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}

bool CBusinessImpl::IsSingle12345Alarm(std::string l_strAlarmId)
{
	std::string l_pjdbCount;
	std::string l_linkedCount;
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_linked_dispatch_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = l_strAlarmId;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		if (l_pResult->GetValue("id").empty())
		{
			ICC_LOG_ERROR(m_pLog, "is not 12345 work", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "is not 12345 work", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	l_tSQLReq.sql_id = "select_icc_t_pjdb_count_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = l_strAlarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_strAlarmId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 68400 * 15);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 68400 * 15);
	}

	l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		if (l_pResult->GetValue("num").compare("0") == 0)
		{
			return true;
		}
	}

	return false;
}

bool CBusinessImpl::UpdateLinkedTableState(std::string l_alarmId, std::string state)
{
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "update_icc_t_linked_dispatch_state";
	l_strDeptSql.param["alarm_id"] = l_alarmId;
	l_strDeptSql.param["state"] = state;
	l_strDeptSql.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}
	else
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		return true;
	}
}

bool CBusinessImpl::UpdateLinkedTableState(PROTOCOL::CTelHotlineVisitRequest::CBody p_alarmInfo)
{
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "update_icc_t_linked_dispatch_state";
	l_strDeptSql.param["alarm_id"] = p_alarmInfo.m_strAlarmID;
	l_strDeptSql.param["state"] = p_alarmInfo.m_strAlarmResult;
	l_strDeptSql.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}
	else
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		return true;
	}
}


bool CBusinessImpl::Build12345Request(PROTOCOL::CPushAlarmRequest& p_sRequest, PROTOCOL::CPushAlarmTo12345Request& p_dRequest)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = p_sRequest.m_oBody.m_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_sRequest.m_oBody.m_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_dRequest.ParseAlarmRecord(l_pResult, p_sRequest);
	}

	l_tSQLReq.sql_id = "select_icc_t_dept_by_code";
	l_tSQLReq.param["code"] = p_dRequest.m_oBody.m_alarmDatas.GXDWDM;
	l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_code GXDWDM failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_dRequest.m_oBody.m_alarmDatas.GXDWMC = l_pResult->GetValue("name", "");
	}

	l_tSQLReq.sql_id = "select_icc_t_dept_by_code";
	l_tSQLReq.param["code"] = p_dRequest.m_oBody.m_alarmDatas.JJDWDM;
	l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_code JJDWDM failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_dRequest.m_oBody.m_alarmDatas.GXDWMC = l_pResult->GetValue("name", "");
		return true;
	}

	ICC_LOG_WARNING(m_pLog, "record is null");

	return false;
}

bool CBusinessImpl::IsPushTo12345(std::string l_strAlarmId)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = l_strAlarmId;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string l_curState = l_pResult->GetValue("state", "");

		return l_curState == "01" ? true : false;
	}

	return false;
}

void CBusinessImpl::OnPushAlarmTo12345(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	bool is_fail = false;
	// 解析请求消息
	PROTOCOL::CPushAlarmRequest l_oRequest;
	PROTOCOL::CPushAlarmResponse l_oRespond;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	//手动推送按钮判断是否已经推送过12345
	if (l_oRequest.m_oBody.m_strOptType == "1" && IsPushTo12345(l_oRequest.m_oBody.m_strAlarmID))
	{
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Cur Alarm Alardy Success Push To 12345!!";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		return;
	}

	SelectDictNameOnDictTable(l_oRequest.m_oBody.m_strAlarmID, l_oRequest);

	PROTOCOL::CPushAlarmTo12345Request m_oRequest;
	std::string strTarget = "/deal12345/pushJjd";
	PROTOCOL::CPushAlarmTo12345Response m_oResponse;
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;

	do {
		if (!Build12345Request(l_oRequest, m_oRequest))
		{
			ICC_LOG_ERROR(m_pLog, "failed select jjdb");
			is_fail = true;
			break;
		}

		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

		Get12345Service(m_str12345ServerIp, m_str12345ServerPort);

		ICC_LOG_DEBUG(m_pLog, "send message to 12345: %s", m_oRequest.ToString(m_pJsonFty->CreateJson()).c_str());

		//std::string strReceive = m_pHttpClient->PostEx(m_str12345ServerIp, m_str12345ServerPort, strTarget, mapHeaders, m_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage);
		std::string strReceive = m_pHttpClient->PostWithTimeout(m_str12345ServerIp, m_str12345ServerPort, strTarget, mapHeaders, m_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage, m_pString->ToInt(m_sendMsgOverTime));
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "post 12345 error!!!!err[%s]", strErrorMessage.c_str());
			is_fail = true;
			break;
		}
		else if (!pJson->LoadJson(strReceive))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed 12345 message:[%s]", strReceive.c_str());
			is_fail = true;
			break;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "get message from 12345: %s", strReceive.c_str());
			l_oRespond.m_oBody.m_code = pJson->GetNodeValue("/code", "");
			l_oRespond.m_oBody.m_data = pJson->GetNodeValue("/data", "");
			l_oRespond.m_oBody.m_message = pJson->GetNodeValue("/message", "");
		}
	} while (false);

	// 构造回复
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;
	l_oRespond.m_oBody.m_strResult = is_fail ? "1" : "0";

	if (l_oRespond.m_oBody.m_strResult == "0" && l_oRespond.m_oBody.m_code.compare("200") != 0)
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}

	l_oRespond.m_oHeader.m_strResult = l_oRespond.m_oBody.m_strResult;
	l_oRespond.m_oHeader.m_strMsg = l_oRespond.m_oBody.m_message;
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);

	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_oRespond.m_oBody.m_code.compare("200") != 0)
	{
		AddWorkToRetransmission(strTarget, m_oRequest.ToString(m_pJsonFty->CreateJson()), l_oRequest.m_oBody.m_strAlarmID);

		//发送推送失败消息至消息中心
		std::string l_strConfigMsg;
		m_pMsgCenter->BuildManualAcceptConfig("12345SendError", l_strConfigMsg);
		l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", l_oRequest.m_oBody.m_strAlarmID);
		m_pMsgCenter->Send(l_strConfigMsg, l_oRequest.m_oBody.m_strAlarmID, true, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

		PROTOCOL::CAlarmLogSync l_oTHLData;
		std::vector<std::string> l_vecParamList;
		l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
		l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
		l_oTHLData.m_oBody.m_strOperate = "BS001310008";
		l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
		l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
		l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

		if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
		{
			ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
			return;
		}
		else
		{
			SyncAlarmLogInfo(l_oTHLData.m_oBody);
		}
	}
	else
	{
		//更新联动表信息
		UpdateLinkedTableState(l_oRequest.m_oBody.m_strAlarmID, "02");
		//发同步消息
		SyncLinkedTableState(l_oRequest.m_oBody.m_strAlarmID);
	}
}

void CBusinessImpl::AddWorkToRetransmission(std::string l_strTar, std::string l_strMsg, std::string l_strKey)
{
	MsgCenterListNode l_curMsgCenterNode;
	l_curMsgCenterNode.sendCount = 0;
	l_curMsgCenterNode.sendTar = l_strTar;
	l_curMsgCenterNode.sendMsg = l_strMsg;
	l_curMsgCenterNode.lastExecTime = m_pDateTime->CurrentDateTime();
	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	if (m_mMsgCenterRetransmissionList.find(l_strKey) == m_mMsgCenterRetransmissionList.end())
	{
		m_mMsgCenterRetransmissionList[l_strKey] = l_curMsgCenterNode;
	}
	else
	{
		m_mMsgCenterRetransmissionList[l_strKey].sendCount = 0;
		m_mMsgCenterRetransmissionList[l_strKey].sendMsg = l_strMsg;
	}
}

bool CBusinessImpl::InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		//TODO::select_icc_t_jjdb_jjsj 查询jjsj
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_strAlarmID);

		if (strTime != "")
		{
			DataBase::SQLRequest l_sqlReqeust;
			l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
			l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
			DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
			if (!l_pRSetPtr->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
			}
			if (l_pRSetPtr->Next())
			{
				p_AlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
			}
		}
	}

	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		p_AlarmLogInfo.m_strReceivedTime = l_strCurTime;
	}

	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		return false;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	m_pHelpTool->Sleep(1);
	return true;
}

bool CBusinessImpl::Get12345Service(std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_str12345ServiceName = m_pConfig->GetValue("ICC/Plugin/Synthetical/vcsservicename", "commandcenter-icc-octg");
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_str12345ServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_str12345ServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_str12345ServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy 12345 service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy 12345 service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	//l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
	//l_oAlarmLogSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
	l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

std::string CBusinessImpl::BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::OnMsgCenterRetransmission(ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CRepulseAlarmResponse l_oRespond;
	std::string strReceive;
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	for (auto l_curIt = m_mMsgCenterRetransmissionList.begin(); l_curIt != m_mMsgCenterRetransmissionList.end();)
	{
		ICC_LOG_DEBUG(m_pLog, "OnMsgCenterRetransmission CurAlarmId:[%s], DiffTime:[%d]", l_curIt->first.c_str(), m_pDateTime->SecondsDifference(m_pDateTime->CurrentDateTime(), l_curIt->second.lastExecTime));
		if (m_pDateTime->SecondsDifference(m_pDateTime->CurrentDateTime(), l_curIt->second.lastExecTime) >= m_nMsgCenterExecTime)
		{
			l_curIt->second.lastExecTime = m_pDateTime->CurrentDateTime();
			strReceive = m_pHttpClient->PostEx(m_str12345ServerIp, m_str12345ServerPort, l_curIt->second.sendTar, mapHeaders, l_curIt->second.sendMsg, strErrorMessage);
			pJson->LoadJson(strReceive);
			if (l_curIt->second.sendTar == "/deal12345/pushJjd" && pJson->GetNodeValue("/code", "") == "200")
			{
				UpdateLinkedTableState(l_curIt->first, "02");
				SyncLinkedTableState(l_curIt->first);
			}
			else if (l_curIt->second.sendTar == "/deal12345/pushJjd" && pJson->GetNodeValue("/code", "") != "200")
			{
				//发送推送失败消息至消息中心
				std::string l_strConfigMsg;
				m_pMsgCenter->BuildManualAcceptConfig("12345SendError", l_strConfigMsg);
				l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", l_curIt->first);
				m_pMsgCenter->Send(l_strConfigMsg, l_curIt->first, true, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

				PROTOCOL::CAlarmLogSync l_oTHLData;
				std::vector<std::string> l_vecParamList;
				l_vecParamList.push_back(l_curIt->first);
				l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
				l_oTHLData.m_oBody.m_strOperate = "BS001310008";
				l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oTHLData.m_oBody.m_strAlarmID = l_curIt->first;
				l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

				if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
				{
					ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
					return;
				}
				else
				{
					SyncAlarmLogInfo(l_oTHLData.m_oBody);
				}
			}

			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "post 12345 error!!!!err[%s]", strErrorMessage.c_str());
				Get12345Service(m_str12345ServerIp, m_str12345ServerPort);
			}
			else if (!pJson->LoadJson(strReceive))
			{
				ICC_LOG_ERROR(m_pLog, "parser failed 12345 message:[%s]", strReceive.c_str());
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "get message from 12345: %s", strReceive.c_str());
				l_oRespond.m_oBody.m_code = pJson->GetNodeValue("/code", "");
				l_oRespond.m_oBody.m_data = pJson->GetNodeValue("/data", "");
				l_oRespond.m_oBody.m_message = pJson->GetNodeValue("/message", "");
			}

			l_curIt->second.sendCount++;

			if (l_oRespond.m_oBody.m_code.compare("200") == 0 || l_curIt->second.sendCount >= m_nMsgCenterExecCount)
			{
				l_curIt = m_mMsgCenterRetransmissionList.erase(l_curIt);
				continue;
			}
		}
		l_curIt++;
	}
}

void CBusinessImpl::OnRepulseAlarmTo12345(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CRepulseAlarmRequest l_oRequest;
	bool is_fail = false;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CAlarmLogSync l_oTHLData;
	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
	std::vector<std::string> l_vecParamList;
	std::string l_curMsg;
	std::string p_strName;

	PROTOCOL::CRepulseAlarmRequestTo12345 m_oRequest;
	m_oRequest.m_oBody.m_strRepulser = l_oRequest.m_oBody.m_strRepulser;
	m_oRequest.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	m_oRequest.m_oBody.m_strReason = l_oRequest.m_oBody.m_strReason;
	m_oRequest.m_oBody.m_strWorkID = l_oRequest.m_oBody.m_strWorkID;
	m_oRequest.m_oBody.m_strRepulseAlarmTime = m_pDateTime->CurrentDateTimeStr();
	m_oRequest.m_oBody.HANDLEBMNAME = l_oRequest.m_oBody.m_strRepulseDept;

	PROTOCOL::CRepulseAlarmTo12345Response m_oResponse;
	PROTOCOL::CRepulseAlarmResponse l_oRespond;

	if (!GetAlarmerName(l_oRequest.m_oBody.m_strRepulser, p_strName))
	{
		ICC_LOG_ERROR(m_pLog, "get alarmerName error, code :[%s]", l_oRequest.m_oBody.m_strRepulser.c_str());

		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "get alarmer name error";

		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		return;
	}

	std::string strTarget = "/deal12345/pushThxx";
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();

	ICC_LOG_DEBUG(m_pLog, "send message to 12345: %s", m_oRequest.ToString(m_pJsonFty->CreateJson()).c_str());
	Get12345Service(m_str12345ServerIp, m_str12345ServerPort);

	std::string strReceive = m_pHttpClient->PostWithTimeout(m_str12345ServerIp, m_str12345ServerPort, strTarget, mapHeaders, m_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage, m_pString->ToInt(m_sendMsgOverTime));
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "post 12345 error!!!!err[%s]", strErrorMessage.c_str());
		is_fail = true;
	}
	else if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed 12345 message:[%s]", strReceive.c_str());
		is_fail = true;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "get message from 12345: %s", strReceive.c_str());
		l_oRespond.m_oBody.m_code = pJson->GetNodeValue("/code", "");
		l_oRespond.m_oBody.m_data = pJson->GetNodeValue("/data", "");
		l_oRespond.m_oBody.m_message = pJson->GetNodeValue("/message", "");
	}

	if (!is_fail)
	{
		do {
			//更新警单状态,并发送同步
			PROTOCOL::CAlarmInfo p_roAlarmSync;
			if (Get12345WorkId(l_oRequest.m_oBody.m_strAlarmID))
			{
				if (!UpdateAlarmStateEx(p_roAlarmSync, l_oRequest.m_oBody.m_strAlarmID, "04", "05"))
				{
					l_oRespond.m_oBody.m_strResult = "1";
					l_oRespond.m_oBody.m_message = "execute update alarm state info failed";
					is_fail = true;
					ICC_LOG_ERROR(m_pLog, "update alarm state info failed");
					break;
				}
				else
				{
					SyncAlarmInfo(p_roAlarmSync);
					l_oRespond.m_oBody.m_strResult = "0";
				}
			}
			else
			{
				break;
			}

			l_vecParamList.push_back(l_oRequest.m_oBody.m_strAlarmID);
			l_vecParamList.push_back(p_strName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strReason);
			l_oTHLData.m_oBody.m_strOperate = "BS001310014";
			l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

			//写流水
			if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
			{
				l_oRespond.m_oBody.m_strResult = "1";
				l_oRespond.m_oBody.m_message = "execute insert alarm log info failed";
				is_fail = true;
				ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
				break;
			}
			else
			{
				SyncAlarmLogInfo(l_oTHLData.m_oBody);
				l_oRespond.m_oBody.m_strResult = "0";
			}

			//发送至消息中心
			m_pMsgCenter->BuildManualAcceptConfig("110To12345Repulse", l_curMsg);
			l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strAlarmID);
			l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", p_strName);
			l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strReason);
			m_pMsgCenter->Send(l_curMsg, l_oRequest.m_oBody.m_strAlarmID, true, false, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

		} while (false);
	}

	// 构造回复
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;
	l_oRespond.m_oBody.m_strResult = is_fail ? "1" : "0";

	if (l_oRespond.m_oBody.m_strResult == "0" && l_oRespond.m_oBody.m_code.compare("200") != 0)
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}

	l_oRespond.m_oHeader.m_strResult = l_oRespond.m_oBody.m_strResult;
	l_oRespond.m_oHeader.m_strMsg = l_oRespond.m_oBody.m_message;

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

}

bool CBusinessImpl::Get12345WorkId(PROTOCOL::CPushRevisitRequest& l_oRequest)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = l_oRequest.m_oBody.m_strCaseId;
	std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequest.m_oBody.m_strCaseId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success:[%s]", l_pResult->GetSQL().c_str());

		std::string l_alarmType = l_pResult->GetValue("jqlyfs");
		std::string l_12345WorkId = l_pResult->GetValue("jjlyh");

		if (l_alarmType == "09" && !l_12345WorkId.empty())
		{
			l_oRequest.m_oBody.m_str12345WorkId = l_12345WorkId;

			return true;
		}
	}

	return false;
}

bool CBusinessImpl::Get12345WorkId(std::string p_strAlarmId)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = p_strAlarmId;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success:[%s]", l_pResult->GetSQL().c_str());

		std::string l_alarmType = l_pResult->GetValue("jqlyfs");
		std::string l_12345WorkId = l_pResult->GetValue("jjlyh");

		if (l_alarmType == "09" && !l_12345WorkId.empty())
		{
			return true;
		}
	}

	return false;
}

std::string CBusinessImpl::ChangeTime(std::string l_utcTime)
{
	std::string l_strRes;
	l_strRes = m_pString->ReplaceFirst(l_utcTime, "T", " ");
	std::vector<std::string> l_splitTime;

	m_pString->Split(l_strRes, ".", l_splitTime, true);

	if (l_splitTime.size() >= 2)
	{
		return l_splitTime[0];
	}
	else
	{
		return l_strRes;
	}
}

bool CBusinessImpl::Get12345WorkId(std::string l_alarmId, std::string& l_workId)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = l_alarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_alarmId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success:[%s]", l_pResult->GetSQL().c_str());

		std::string l_alarmType = l_pResult->GetValue("jqlyfs");
		std::string l_12345WorkId = l_pResult->GetValue("jjlyh");

		if (l_alarmType == "09" && !l_12345WorkId.empty())
		{
			l_workId = l_12345WorkId;

			return true;
		}
	}

	return false;
}

std::vector<std::pair<std::string, std::string>> CBusinessImpl::GetSonAlarm(std::string l_alarmId, std::string l_createTime)
{
	std::vector<std::pair<std::string, std::string>> p_strSonAlarmList;
	DateTime::CDateTime l_cTime = m_pDateTime->FromString(l_createTime);
	DateTime::CDateTime l_startTime = m_pDateTime->AddDays(l_cTime, -5);
	DateTime::CDateTime l_endTime = m_pDateTime->AddDays(l_cTime, 5);
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_jjdb";
	l_strDeptSql.param["merge_id"] = l_alarmId;
	l_strDeptSql.param["jjsj_begin"] = m_pDateTime->ToString(l_startTime);
	l_strDeptSql.param["jjsj_end"] = m_pDateTime->ToString(l_endTime);

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql, true);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return p_strSonAlarmList;
	}
	else
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		while (l_sqlResult->Next())
		{
			std::string l_alarmType = l_sqlResult->GetValue("source_type");
			std::string l_12345WorkId = l_sqlResult->GetValue("source_id");

			if (l_alarmType == "09" && !l_12345WorkId.empty())
			{
				p_strSonAlarmList.push_back(std::pair<std::string, std::string>(l_sqlResult->GetValue("id"), l_sqlResult->GetValue("source_id")));
			}
		}
		return p_strSonAlarmList;
	}
}

void CBusinessImpl::OnPushRevisitTo12345(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CPushRevisitRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string p_strResult;
	m_pMsgCenter->BuildManualAcceptConfig("12345RevisitMessage", p_strResult);

	l_oRequest.m_oBody.m_strFeedbackTime = ChangeTime(l_oRequest.m_oBody.m_strFeedbackTime);
	l_oRequest.m_oBody.m_strCreateTime = ChangeTime(l_oRequest.m_oBody.m_strCreateTime);
	p_strResult = m_pString->ReplaceFirst(p_strResult, "$", l_oRequest.m_oBody.m_strFeedbackOrgName);
	p_strResult = m_pString->ReplaceFirst(p_strResult, "$", l_oRequest.m_oBody.m_strFeedbackName);
	p_strResult = m_pString->ReplaceFirst(p_strResult, "$", l_oRequest.m_oBody.m_strDisposalDec);
	l_oRequest.m_oBody.m_strResultDes = p_strResult;

	std::vector<std::pair<std::string, std::string>> p_strAlarmList;
	bool l_is12345WorkID = false;
	p_strAlarmList = GetSonAlarm(l_oRequest.m_oBody.m_strCaseId, l_oRequest.m_oBody.m_strCreateTime);

	std::string l_strWorkId;
	if (Get12345WorkId(l_oRequest.m_oBody.m_strCaseId, l_strWorkId))
	{
		p_strAlarmList.push_back(std::pair<std::string, std::string>(l_oRequest.m_oBody.m_strCaseId, l_strWorkId));
	}

	ICC_LOG_DEBUG(m_pLog, "current WorkId list count %d", p_strAlarmList.size());

	for (size_t i = 0; i < p_strAlarmList.size(); i++)
	{
		l_oRequest.m_oBody.m_str12345WorkId = p_strAlarmList[i].second;
		std::string strTarget = "/deal12345/pushFkd";
		std::string strErrorMessage;
		std::map<std::string, std::string> mapHeaders;
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

		Get12345Service(m_str12345ServerIp, m_str12345ServerPort);

		ICC_LOG_DEBUG(m_pLog, "send message to 12345: %s", l_oRequest.ToString(m_pJsonFty->CreateJson()).c_str());

		std::string strReceive = m_pHttpClient->PostWithTimeout(m_str12345ServerIp, m_str12345ServerPort, strTarget, mapHeaders, l_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage, m_pString->ToInt(m_sendMsgOverTime));
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "post 12345 error!!!!err[%s]", strErrorMessage.c_str());

			AddWorkToRetransmission(strTarget, l_oRequest.ToString(m_pJsonFty->CreateJson()), l_oRequest.m_oBody.m_str12345WorkId);
		}
		else if (!pJson->LoadJson(strReceive))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed 12345 message:[%s]", strReceive.c_str());

			AddWorkToRetransmission(strTarget, l_oRequest.ToString(m_pJsonFty->CreateJson()), l_oRequest.m_oBody.m_str12345WorkId);
		}
		else
		{
			std::string l_strResult = pJson->GetNodeValue("/code", "");
			if (l_strResult.compare("200") != 0)
			{
				AddWorkToRetransmission(strTarget, l_oRequest.ToString(m_pJsonFty->CreateJson()), l_oRequest.m_oBody.m_str12345WorkId);
			}

			ICC_LOG_DEBUG(m_pLog, "get message from 12345: %s", strReceive.c_str());
		}
	}
}

bool CBusinessImpl::SelectDictNameOnDictTable(std::string l_alarmId, PROTOCOL::CPushAlarmRequest& l_oRequest)
{
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_jjdb_ex";
	l_strDeptSql.param["jjdbh"] = l_alarmId;
	l_strDeptSql.param["lang_code"] = m_strLang_code;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_alarmId);
	if (strTime != "")
	{
		l_strDeptSql.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_strDeptSql.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}
	else
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		while (l_sqlResult->Next())
		{
			if (l_sqlResult->GetValue("jqlbdm") == l_sqlResult->GetValue("dict_key"))
				l_oRequest.m_oBody.m_strJqlbmc = l_sqlResult->GetValue("value");
			else if (l_sqlResult->GetValue("jqlxdm") == l_sqlResult->GetValue("dict_key"))
				l_oRequest.m_oBody.m_strJqlxmc = l_sqlResult->GetValue("value");
			else if (l_sqlResult->GetValue("jqxldm") == l_sqlResult->GetValue("dict_key"))
				l_oRequest.m_oBody.m_strJqxlmc = l_sqlResult->GetValue("value");
			else if (l_sqlResult->GetValue("jqzldm") == l_sqlResult->GetValue("dict_key"))
				l_oRequest.m_oBody.m_strJqzlmc = l_sqlResult->GetValue("value");
		}
		return true;
	}
}

void CBusinessImpl::SyncLinkedTableState(std::string l_alarmId)
{
	PROTOCOL::CSyncLinkedTableState l_syncStateData;
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_linked_dispatch_by_alarm_id";
	l_strDeptSql.param["alarm_id"] = l_alarmId;

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}
	else if (l_sqlResult->Next())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		l_syncStateData.m_oBody.alarm_id = l_sqlResult->GetValue("alarm_id");
		l_syncStateData.m_oBody.create_time = l_sqlResult->GetValue("create_time");
		l_syncStateData.m_oBody.create_user = l_sqlResult->GetValue("create_user");
		l_syncStateData.m_oBody.dispatch_code = l_sqlResult->GetValue("dispatch_code");
		l_syncStateData.m_oBody.dispatch_name = l_sqlResult->GetValue("dispatch_name");
		l_syncStateData.m_oBody.id = l_sqlResult->GetValue("id");
		l_syncStateData.m_oBody.linked_org_code = l_sqlResult->GetValue("linked_org_code");
		l_syncStateData.m_oBody.linked_org_name = l_sqlResult->GetValue("linked_org_name");
		l_syncStateData.m_oBody.linked_org_type = l_sqlResult->GetValue("linked_org_type");
		l_syncStateData.m_oBody.result = l_sqlResult->GetValue("result");
		l_syncStateData.m_oBody.state = l_sqlResult->GetValue("state");
		l_syncStateData.m_oBody.update_time = l_sqlResult->GetValue("update_time");
		l_syncStateData.m_oBody.update_user = l_sqlResult->GetValue("update_user");
	}
	else
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table not find, SQL: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str());

		return;
	}

	//同步发送至前端
	l_syncStateData.m_oHeader.m_strSystemID = "ICC";
	l_syncStateData.m_oHeader.m_strSubsystemID = "ICC-TelHotline";
	l_syncStateData.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_syncStateData.m_oHeader.m_strCmd = "sync_linked_dispatch_info";
	l_syncStateData.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_syncStateData.m_oHeader.m_strRequestType = "1";//主题
	l_syncStateData.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_syncStateData.m_oHeader.m_strRequestFlag = "mq";

	std::string l_strMsg;
	l_strMsg = l_syncStateData.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "[SyncLinkedTableState]sync alarm process msg[\n%s\n]", l_strMsg.c_str());

	//同步发送至VCS
	l_syncStateData.m_oHeader.m_strSystemID = "ICC";
	l_syncStateData.m_oHeader.m_strSubsystemID = "ICC-TelHotline";
	l_syncStateData.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_syncStateData.m_oHeader.m_strCmd = "linked_dispatch_sync";
	l_syncStateData.m_oHeader.m_strRequest = "topic_linked_sync";
	l_syncStateData.m_oHeader.m_strRequestType = "1";
	l_syncStateData.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_syncStateData.m_oHeader.m_strRequestFlag = "mq";

	l_strMsg = l_syncStateData.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "[SyncLinkedTableState]sync alarm process msg[\n%s\n]", l_strMsg.c_str());
}

bool CBusinessImpl::UpdateAlarmState(PROTOCOL::CAlarmInfo& p_roAlarmSync, std::string l_alarmId, std::string state)
{
	PROTOCOL::CSyncLinkedTableState l_syncStateData;
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "update_icc_t_jjdb";
	l_strDeptSql.param["id"] = l_alarmId;
	l_strDeptSql.set["state"] = state;
	l_strDeptSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "%s exec success: %s", l_strDeptSql.sql_id.c_str(), l_sqlResult->GetSQL().c_str());

	DataBase::SQLRequest l_strGetSql;
	l_strGetSql.sql_id = "select_icc_t_jjdb";
	l_strGetSql.param["id"] = l_alarmId;

	DataBase::IResultSetPtr l_sqlSelctResult;
	l_sqlSelctResult = m_pDBConn->Exec(l_strGetSql);

	if (!l_sqlSelctResult->IsValid())
	{
		std::string l_strSQL = l_sqlSelctResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strGetSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlSelctResult->GetErrorMsg().c_str());

		return false;
	}
	else if (l_sqlSelctResult->Next())
	{
		std::string l_strSQL = l_sqlSelctResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strGetSql.sql_id.c_str(),
			l_strSQL.c_str());

		p_roAlarmSync.ParseAlarmRecord(l_sqlSelctResult);
		if (p_roAlarmSync.m_strBusinessState == "01")
			p_roAlarmSync.m_strBusinessState = state;
		return true;
	}

	return false;
}

bool CBusinessImpl::UpdateAlarmStateEx(PROTOCOL::CAlarmInfo& p_roAlarmSync, std::string l_alarmId, std::string state, std::string l_lhlx)
{
	PROTOCOL::CSyncLinkedTableState l_syncStateData;
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "update_icc_t_jjdb";
	l_strDeptSql.param["id"] = l_alarmId;
	l_strDeptSql.set["state"] = state;
	l_strDeptSql.set["handle_type"] = l_lhlx;
	l_strDeptSql.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "%s exec success: %s", l_strDeptSql.sql_id.c_str(), l_sqlResult->GetSQL().c_str());

	DataBase::SQLRequest l_strGetSql;
	l_strGetSql.sql_id = "select_icc_t_jjdb";
	l_strGetSql.param["id"] = l_alarmId;

	DataBase::IResultSetPtr l_sqlSelctResult;
	l_sqlSelctResult = m_pDBConn->Exec(l_strGetSql);

	if (!l_sqlSelctResult->IsValid())
	{
		std::string l_strSQL = l_sqlSelctResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strGetSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlSelctResult->GetErrorMsg().c_str());

		return false;
	}
	else if (l_sqlSelctResult->Next())
	{
		std::string l_strSQL = l_sqlSelctResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",
			l_strGetSql.sql_id.c_str(),
			l_strSQL.c_str());

		p_roAlarmSync.ParseAlarmRecord(l_sqlSelctResult);
		if (p_roAlarmSync.m_strBusinessState == "01")
			p_roAlarmSync.m_strBusinessState = state;
		return true;
	}

	return false;
}

void CBusinessImpl::SyncAlarmInfo(const PROTOCOL::CAlarmInfo& p_roAlarmSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmSync.m_oHeader.m_strSubsystemID = "Alarm-TelHotline";
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = m_pString->CreateGuid();
	l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmSync.m_oHeader.m_strCmd = "alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	l_oAlarmSync.m_oBody.m_oAlarmInfo = p_roAlarmSync;

	if (!p_roAlarmSync.m_strMsgSource.compare("vcs"))
	{
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource = "vcs";
	}

	if (!p_roAlarmSync.m_strMsgSource.compare("mpa"))
	{
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource = "mpa";
	}

	l_oAlarmSync.m_oBody.m_strSyncType = std::to_string(4);
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLevel = p_roAlarmSync.m_strLevel.empty() ? "04" : p_roAlarmSync.m_strLevel;//默认最低级


	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//-----------------------------------------泸州保留部分---------------------------------------------------

//void CBusinessImpl::OnCNotifiTHLAddRequest(ObserverPattern::INotificationPtr p_pNotify)
//{
//	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
//	// 解析请求消息
//	PROTOCOL::CTelHotlineAddRequest l_oRequest;
//	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
//	{
//		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
//		return;
//	}
//	//回复
//	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
//
//	// 写流水 
//	PROTOCOL::CAlarmLogSync l_oTHLData;
//	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
//	std::vector<std::string> l_vecParamList;
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strHandleBMName);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strHandleUserName);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strNodeType);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strHandleOpinion);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strCode);
//	l_oTHLData.m_oBody.m_strOperate = BS001310001;
//	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
//	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strPoliceCode;
//	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
//	
//	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
//	{
//		l_oRespond.m_strRequest = "1";
//		l_oRespond.m_strMsg = "execute insert alarm log info failed";
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
//	}
//	else
//	{
//		SyncAlarmLogInfo(l_oTHLData.m_oBody);
//		l_oRespond.m_strResult = "0";
//	}
//
//	//构建消息中心信息
//	std::string l_curMsg;
//	BuildManualAcceptConfig("12345FlowPath", l_curMsg);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strPoliceCode);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strNodeType);
//	SendMsgToMsgCenter(l_curMsg, l_oRequest.m_oBody.m_strPoliceCode);
//
//	//回复消息
//	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
//	l_oRespond.SaveTo(tmp_spJson);
//	std::string tmp_strMsg(tmp_spJson->ToString());
//	p_pNotify->Response(tmp_strMsg);
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
//}
//
//void CBusinessImpl::OnCNotifilTHLFpideRequest(ObserverPattern::INotificationPtr p_pNotify)
//{
//	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
//	// 解析请求消息
//	PROTOCOL::CTelHotlineFpideRequest l_oRequest;
//	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
//	{
//		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
//		return;
//	}
//
//	//回复
//	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
//
//	// 写流水 
//	PROTOCOL::CAlarmLogSync l_oTHLData;
//	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
//	std::vector<std::string> l_vecParamList;
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strApplyBMName);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strApplyUserName);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strApplyTimeLimit);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strApplyTime);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strApplyContent);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strCode);
//	l_oTHLData.m_oBody.m_strOperate = BS001310002;
//	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
//	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strPoliceCode;
//	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
//
//	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
//	{
//		l_oRespond.m_strRequest = "1";
//		l_oRespond.m_strMsg = "execute insert alarm log info failed";
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
//	}
//	else
//	{
//		SyncAlarmLogInfo(l_oTHLData.m_oBody);
//		l_oRespond.m_strResult = "0";
//	}
//
//	//构建消息中心信息
//	std::string l_curMsg;
//	BuildManualAcceptConfig("12345Extension", l_curMsg);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strPoliceCode);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strApplyTimeLimit);
//	SendMsgToMsgCenter(l_curMsg, l_oRequest.m_oBody.m_strPoliceCode);
//
//	//回复消息
//	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
//	l_oRespond.SaveTo(tmp_spJson);
//	std::string tmp_strMsg(tmp_spJson->ToString());
//	p_pNotify->Response(tmp_strMsg);
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
//}
//
//void CBusinessImpl::OnCNotifiTHLHandleStatRequest(ObserverPattern::INotificationPtr p_pNotify)
//{
//	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
//	// 解析请求消息
//	PROTOCOL::CTelHotlineHandleStateRequest l_oRequest;
//	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
//	{
//		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
//		return;
//	}
//	//回复
//	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
//	// 写流水 
//	PROTOCOL::CAlarmLogSync l_oTHLData;
//	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
//	std::vector<std::string> l_vecParamList;
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strCode);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strHandleState);
//	l_oTHLData.m_oBody.m_strOperate = BS001310003;
//	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
//	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strPoliceCode;
//	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
//	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
//	{
//		l_oRespond.m_strRequest = "1";
//		l_oRespond.m_strMsg = "execute insert alarm log info failed";
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
//	}
//	else
//	{
//		SyncAlarmLogInfo(l_oTHLData.m_oBody);
//		l_oRespond.m_strResult = "0";
//	}
//
//	//构建消息中心信息
//	std::string l_curMsg;
//	BuildManualAcceptConfig("12345State", l_curMsg);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strPoliceCode);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strHandleState);
//	SendMsgToMsgCenter(l_curMsg, l_oRequest.m_oBody.m_strPoliceCode);
//
//	//回复消息
//	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
//	l_oRespond.SaveTo(tmp_spJson);
//	std::string tmp_strMsg(tmp_spJson->ToString());
//	p_pNotify->Response(tmp_strMsg);
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
//}

//void CBusinessImpl::OnCNotifiTHLVisitRequest(ObserverPattern::INotificationPtr p_pNotify)
//{
//	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
//	// 解析请求消息
//	PROTOCOL::CTelHotlineVisitRequest l_oRequest;
//	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
//	{
//		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
//		return;
//	}
//
//	//回复
//	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
//	// 写流水 
//	PROTOCOL::CAlarmLogSync l_oTHLData;
//	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
//	std::vector<std::string> l_vecParamList;
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strUserName);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strVisitMethod);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strSeatsContent);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strSeatsResult);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBMContent);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBMResult);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strFlowID);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strCode);
//	l_oTHLData.m_oBody.m_strOperate = BS001310004;
//	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
//	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strPoliceCode;
//	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
//
//	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
//	{
//		l_oRespond.m_strRequest = "1";
//		l_oRespond.m_strMsg = "execute insert alarm log info failed";
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
//	}
//	else
//	{
//		SyncAlarmLogInfo(l_oTHLData.m_oBody);
//		l_oRespond.m_strResult = "0";
//	}
//
//	//构建消息中心信息
//	std::string l_curMsg;
//	BuildManualAcceptConfig("12345ReturnVisit", l_curMsg);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strPoliceCode);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strSeatsContent);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strBMContent);
//	SendMsgToMsgCenter(l_curMsg, l_oRequest.m_oBody.m_strPoliceCode);
//
//	//回复消息
//	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
//	l_oRespond.SaveTo(tmp_spJson);
//	std::string tmp_strMsg(tmp_spJson->ToString());
//	p_pNotify->Response(tmp_strMsg);
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
//}

//void CBusinessImpl::OnCNotifiTHLRequest(ObserverPattern::INotificationPtr p_pNotify)
//{
//	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
//	// 解析请求消息
//	PROTOCOL::CTelHotlineRequest l_oRequest;
//	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
//	{
//		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
//		return;
//	}
//	//回复
//	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
//
//	// 写流水 
//	PROTOCOL::CAlarmLogSync l_oTHLData;
//	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
//	std::vector<std::string> l_vecParamList;
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBLBM);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBLR);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBLJG);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strBLZT);
//	l_vecParamList.push_back(l_oRequest.m_oBody.m_strGDBH);
//	l_oTHLData.m_oBody.m_strOperate = BS001310005;
//	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
//	l_oTHLData.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strPoliceCode;
//	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
//
//	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody))
//	{
//		l_oRespond.m_strRequest = "1";
//		l_oRespond.m_strMsg = "execute insert alarm log info failed";
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
//	}
//	else
//	{
//		SyncAlarmLogInfo(l_oTHLData.m_oBody);
//		l_oRespond.m_strResult = "0";
//	}
//
//	//构建消息中心信息
//	std::string l_curMsg;
//	BuildManualAcceptConfig("12345Result", l_curMsg);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strPoliceCode);
//	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", l_oRequest.m_oBody.m_strBLJG);
//	SendMsgToMsgCenter(l_curMsg, l_oRequest.m_oBody.m_strPoliceCode);
//
//	//回复消息
//	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
//	l_oRespond.SaveTo(tmp_spJson);
//	std::string tmp_strMsg(tmp_spJson->ToString());
//	p_pNotify->Response(tmp_strMsg);
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
//}
//
//bool CBusinessImpl::InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
//{
//	//TODO::记录警情流水日志
//	DataBase::SQLRequest l_tSQLReqInsertAlarm;
//	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
//	{
//		return false;
//	}
//	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false, strTransGuid);
//	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
//	if (!l_pRSet->IsValid())
//	{
//		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
//		return false;
//	}
//	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
//	m_pHelpTool->Sleep(1);
//	return true;
//}
//
//void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
//{
//	std::string l_strGuid = m_pString->CreateGuid();
//	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
//	//l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
//	//l_oAlarmLogSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
//	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
//	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
//	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
//	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
//	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
//	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
//	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
//	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";
//
//	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
//	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
//	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
//	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
//	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
//	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
//	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
//	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
//	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
//	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
//	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
//	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
//	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
//	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
//	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
//	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
//	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
//	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
//	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
//	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
//	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
//	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
//
//	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
//	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
//	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
//}
//
//std::string CBusinessImpl::BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
//{
//	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
//	unsigned int l_iIndex = 0;
//	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
//	{
//		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
//		l_iIndex++;
//	}
//	return l_pIJson->ToString();
//}