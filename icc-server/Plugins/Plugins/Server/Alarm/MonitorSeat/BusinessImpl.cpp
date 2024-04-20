#include "Boost.h"
#include "BusinessImpl.h"

#define TIMER_INITIATIVE_ALARM	"send_initiative_alarm"

#define PARAM_INFO "ParamInfo"
using namespace std;

//同步状态
enum EnSyncType
{
	ADD = 1, //添加
	EDIT, //更新
	DEL,   //删除
};

enum ApprovalType
{
	ALARM_UP = 1, //警情升级
	BLACK_APPLICATION, //申请拉黑
	DEPARTURE_APPLICATION,//申请离席
	PRIVACY_PROTECTION,   //申请解除隐私保护
};

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();

	m_strTiming = m_pConfig->GetValue("ICC/Plugin/MonitorSeat/Timing", "5");
	m_strSendDelayTime= m_pConfig->GetValue("ICC/Plugin/MonitorSeat/SendDelayTime","10");
	m_strNacosServerIp = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerIp", "127.0.0.1");
	m_strNacosServerPort = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerPort", "8848");
	m_strNacosServerNamespace = m_pConfig->GetValue("ICC/Component/HttpServer/NacosNamespace", "dev");
	m_strNacosServerGroupName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosGroupName", "global");

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_apply_request", OnCNotifiSetMonitorSeatApply);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_apply_request", OnCNotifiGetMonitorSeatApply);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_single_apply_request", OnCNotifiGetSingleMonitorSeatApply);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_INITIATIVE_ALARM, InitiativeAlarm);
	//unsigned int m_uintTiming = m_pString->ToUInt64(m_strTiming);
	int m_uintTiming = m_pString->ToInt(m_strTiming);
	m_pTimerMgr->AddTimer(TIMER_INITIATIVE_ALARM, m_uintTiming,60);

	m_pMsgCenter->Start();
	ICC_LOG_DEBUG(m_pLog, "Monitor seat start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "Monitor seat stop success");
}

void CBusinessImpl::OnDestroy()
{

}

bool CBusinessImpl::_AddOrUpdateApproveRecord(const PROTOCOL::CMonitorSeatApplyRequest& in_oReq, std::string& out_strErrInfo, bool in_bInsert)
{
	DataBase::SQLRequest l_tSQLReq;

	//插入？
	if (in_bInsert)
	{
		l_tSQLReq.sql_id = "insert_icc_t_alarm_monitor_seat_approval";
		l_tSQLReq.param["guid"] = in_oReq.m_oBody.m_strGuid;
		l_tSQLReq.param["apply_type"] = in_oReq.m_oBody.m_strApplyType;
		l_tSQLReq.param["apply_target_id"] = in_oReq.m_oBody.m_strApplyTargetID;
		l_tSQLReq.param["apply_time"] = in_oReq.m_oBody.m_strApplyTime;
		l_tSQLReq.param["apply_body"] = in_oReq.m_oBody.m_strApplyBody;
		l_tSQLReq.param["apply_staff_code"] = in_oReq.m_oBody.m_strApplyStaffCode;
		l_tSQLReq.param["apply_staff_name"] = in_oReq.m_oBody.m_strApplyStaffName;
		l_tSQLReq.param["apply_org_code"] = in_oReq.m_oBody.m_strApplyOrgCode;
		l_tSQLReq.param["apply_org_name"] = in_oReq.m_oBody.m_strApplyOrgName;
		l_tSQLReq.param["approve_staff_code"] = in_oReq.m_oBody.m_strApproveStaffCode;
		l_tSQLReq.param["approve_staff_name"] = in_oReq.m_oBody.m_strApproveStaffName;
		l_tSQLReq.param["approve_result"] = in_oReq.m_oBody.m_strApproveResult;
		l_tSQLReq.param["approve_time"] = in_oReq.m_oBody.m_strApproveTime;
		l_tSQLReq.param["approve_remark"] = in_oReq.m_oBody.m_strApproveRemark;
	}
	else
	{
		l_tSQLReq.sql_id = "update_icc_t_alarm_monitor_seat_approval";

		l_tSQLReq.param["approve_staff_code"] = in_oReq.m_oBody.m_strApproveStaffCode;
		l_tSQLReq.param["approve_staff_name"] = in_oReq.m_oBody.m_strApproveStaffName;
		l_tSQLReq.param["approve_result"] = in_oReq.m_oBody.m_strApproveResult;
		l_tSQLReq.param["approve_time"] = in_oReq.m_oBody.m_strApproveTime;
		l_tSQLReq.param["approve_remark"] = in_oReq.m_oBody.m_strApproveRemark;

		l_tSQLReq.param["guid"] = in_oReq.m_oBody.m_strGuid;
	
	}
	
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	
	ICC_LOG_DEBUG(m_pLog, "Exetue sql: %s", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		out_strErrInfo = "execute "+ l_tSQLReq.sql_id + " failed";
		ICC_LOG_ERROR(m_pLog, "UpdateMonitor seat failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}


bool CBusinessImpl::_ValidGetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyQueryRequest& in_oReq, std::string& out_strErrInfo)
{
	if (in_oReq.m_oBody.m_bSingelRequest)
	{
		if (in_oReq.m_oBody.m_strApplyType.empty())
		{
			out_strErrInfo = "apply type is empty";
			return false;
		}

		if (in_oReq.m_oBody.m_strApplyTargetID.empty())
		{
			out_strErrInfo = "apply target id is empty";
			return false;
		}
	}
	else
	{
		if (in_oReq.m_oBody.m_strStartTime.empty())
		{
			out_strErrInfo = "start time is empty";
			return false;
		}

		if (in_oReq.m_oBody.m_strEndTime.empty())
		{
			out_strErrInfo = "end time is empty";
			return false;
		}
	}
	
	return true;
}

bool CBusinessImpl::_GetApproveRecords(const PROTOCOL::CMonitorSeatApplyQueryRequest& in_oReq, PROTOCOL::CMonitorSeatApplyQueryRespond& out_oRes)
{
	DataBase::SQLRequest l_tSQLReq;

	//先查条目
	l_tSQLReq.sql_id = "select_icc_t_alarm_monitor_seat_approval_count";

	if (!in_oReq.m_oBody.m_strApplyType.empty())
	{
		l_tSQLReq.param["apply_type"] = in_oReq.m_oBody.m_strApplyType;
	}
	
	if (!in_oReq.m_oBody.m_strApplyTargetID.empty())
	{
		l_tSQLReq.param["apply_target_id"] = in_oReq.m_oBody.m_strApplyTargetID;
	}
	
	if (!in_oReq.m_oBody.m_strApplyOrgCode.empty())
	{
		l_tSQLReq.param["apply_org_code"] = in_oReq.m_oBody.m_strApplyOrgCode;
	}
	
	if (!in_oReq.m_oBody.m_strStartTime.empty())
	{
		l_tSQLReq.param["begin_time"] = in_oReq.m_oBody.m_strStartTime;
	}

	if (!in_oReq.m_oBody.m_strEndTime.empty())
	{
		l_tSQLReq.param["end_time"] = in_oReq.m_oBody.m_strEndTime;
	}

	if (!in_oReq.m_oBody.m_strApproveResult.empty())
	{
		l_tSQLReq.param["approve_result"] = in_oReq.m_oBody.m_strApproveResult;
	}

	DataBase::IResultSetPtr l_resultCount = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_resultCount->GetSQL().c_str());
	if (!l_resultCount->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultCount->GetErrorMsg().c_str());

		out_oRes.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	std::string tmp_strAllCount;

	if (l_resultCount->Next())
	{
		tmp_strAllCount = l_resultCount->GetValue("num");

		ICC_LOG_DEBUG(m_pLog, "Get all monitorapply size[%s]", tmp_strAllCount.c_str());
	}

	//表示没有记录
	if (tmp_strAllCount.empty() || "0" == tmp_strAllCount)
	{
		ICC_LOG_DEBUG(m_pLog, "no record");
		return true;
	}

	int l_nPageSize = m_pString->ToInt(in_oReq.m_oBody.m_strPageSize.c_str());
	int l_nPageIndex = m_pString->ToInt(in_oReq.m_oBody.m_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;
	l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;


	l_tSQLReq.sql_id = "select_icc_t_alarm_monitor_seat_approval";
	if (!in_oReq.m_oBody.m_bSingelRequest)
	{
		l_tSQLReq.param["limit"] = std::to_string(l_nPageSize);
		l_tSQLReq.param["offset"] = std::to_string(l_nPageIndex);
	}

	l_tSQLReq.param["orderby"] = "approve_result='0' desc,apply_time desc";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		out_oRes.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	out_oRes.m_oBody.m_strAllCount = tmp_strAllCount;

	while (l_result->Next())
	{
		PROTOCOL::CMonitorSeatApplyInfo tmp_oSeatApplyInfo;
		tmp_oSeatApplyInfo.m_strGuid = l_result->GetValue("guid");
		tmp_oSeatApplyInfo.m_strApplyType = l_result->GetValue("apply_type");
		tmp_oSeatApplyInfo.m_strApplyTargetID = l_result->GetValue("apply_target_id");
		tmp_oSeatApplyInfo.m_strApplyTime = l_result->GetValue("apply_time");
		tmp_oSeatApplyInfo.m_strApplyBody = l_result->GetValue("apply_body");
		tmp_oSeatApplyInfo.m_strApplyStaffCode = l_result->GetValue("apply_staff_code");
		tmp_oSeatApplyInfo.m_strApplyStaffName = l_result->GetValue("apply_staff_name");
		tmp_oSeatApplyInfo.m_strApplyOrgCode = l_result->GetValue("apply_org_code");
		tmp_oSeatApplyInfo.m_strApplyOrgName = l_result->GetValue("apply_org_name");
		tmp_oSeatApplyInfo.m_strApproveStaffCode = l_result->GetValue("approve_staff_code");
		tmp_oSeatApplyInfo.m_strApproveStaffName = l_result->GetValue("approve_staff_name");
		tmp_oSeatApplyInfo.m_strApproveResult = l_result->GetValue("approve_result");
		tmp_oSeatApplyInfo.m_strApproveTime = l_result->GetValue("approve_time");
		tmp_oSeatApplyInfo.m_strApproveRemark = l_result->GetValue("approve_remark");

		out_oRes.m_oBody.m_vecApplyInfo.push_back(tmp_oSeatApplyInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "Get success, apply info size[%d]", out_oRes.m_oBody.m_vecApplyInfo.size());

	out_oRes.m_oBody.m_strCount = std::to_string(out_oRes.m_oBody.m_vecApplyInfo.size());
	return true;

}

bool CBusinessImpl::_ValidSetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyRequest& in_oReq, std::string& out_strErrInfo)
{

	//判断请求guid字段
	if (in_oReq.m_oBody.m_strGuid.empty())
	{
		//申请类型（DIC060001重大警情升级审核、DIC060002添加黑名单审核、DIC060003离席申请）
		/*if (in_oReq.m_oBody.m_strApplyType != "DIC060001" && in_oReq.m_oBody.m_strApplyType != "DIC060002" && in_oReq.m_oBody.m_strApplyType != "DIC060003")
		{
			out_strErrInfo = "apply type("+ in_oReq.m_oBody.m_strApplyType+ ") is incorrect.";
			return false;
		}*/

		if (in_oReq.m_oBody.m_strApplyTargetID.empty())
		{
			out_strErrInfo = "apply target id is empty.";
			return false;
		}

		if (in_oReq.m_oBody.m_strApplyTime.empty())
		{
			out_strErrInfo = "apply time is empty.";
			return false;
		}

		//为添加黑名单，但黑名单信息为空
		if (in_oReq.m_oBody.m_strApplyType == "DIC060002" && in_oReq.m_oBody.m_strApplyBody.empty())
		{
			out_strErrInfo = "apply type is add blacklist, but apply body is empty.";
			return false;
		}

		if (in_oReq.m_oBody.m_strApplyOrgCode.empty())
		{
			out_strErrInfo = "apply org code is empty.";
			return false;
		}

		if (in_oReq.m_oBody.m_strApplyStaffCode.empty())
		{
			out_strErrInfo = "apply staff code is empty.";
			return false;
		}

		if (in_oReq.m_oBody.m_strApproveResult.empty())
		{
			in_oReq.m_oBody.m_strApproveResult = "0";
		}
		return true;
	}

	//判断结果字段
	if (in_oReq.m_oBody.m_strApproveResult.empty())
	{
		out_strErrInfo = "approve result is empty.";
		return false;
	}

	if (in_oReq.m_oBody.m_strApproveStaffCode.empty())
	{
		out_strErrInfo = "approve staff is empty.";
		return false;
	}

	if (in_oReq.m_oBody.m_strApproveTime.empty())
	{
		out_strErrInfo = "approve time is empty.";
		return false;
	}
	return true;
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

void CBusinessImpl::InitiativeAlarm(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DateTime::CDateTime l_CurrentTime = m_pDateTime->FromString(strCurrentTime);
	unsigned int m_uintSendDelayTime = m_pString->ToUInt64(m_strSendDelayTime);
	DateTime::CDateTime l_SendTime = m_pDateTime->FromString(m_strSendTime);
	int l_intSendDelayTime = m_pString->ToInt(m_strSendDelayTime);
	DateTime::CDateTime l_ChangeTime = m_pDateTime->AddMinutes(l_SendTime, l_intSendDelayTime);
	std::map<std::string, std::string> l_mapSeatInfo;
	if (!m_pRedisClient->HGetAll("SeatInfo", l_mapSeatInfo))
	{
		ICC_LOG_ERROR(m_pLog, " InitiativeAlarm l_mapSeatInfo  hgetall failed!!!");
		return;
	}
	for (auto it = l_mapSeatInfo.begin(); it != l_mapSeatInfo.end(); it++)
	{
		PROTOCOL::CSeatInfo l_SeatInfo;
		if (!l_SeatInfo.Parse(it->second, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_DEBUG(m_pLog, " parse seat info failed!!!");
			return ;
		}
		std::string l_strClientRegisterInfo = "";
		if (!m_pRedisClient->HGet("ClientRegisterInfo", l_SeatInfo.m_oBody.m_strNo, l_strClientRegisterInfo))
		{
			ICC_LOG_LOWDEBUG(m_pLog, " InitiativeAlarm ClientRegisterInfo hget failed!!!");
		}
		ICC_LOG_DEBUG(m_pLog, "l_strClientRegisterInfo:[%s]",l_strClientRegisterInfo.c_str());
		//如果没有话机没有用户绑定，发出告警通知班长席
		if (l_strClientRegisterInfo.empty())
		{
			std::string l_strConfigMsg;
			BuildManualAcceptConfig("InitiativeAlarm", l_strConfigMsg);
			
			l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", l_SeatInfo.m_oBody.m_strNo);
			ICC_LOG_DEBUG(m_pLog, "InitiativeAlarm Message:[%s]", l_strConfigMsg.c_str());
			std::string strChangeTime = m_pDateTime->ToString(l_SendTime);
			ICC_LOG_DEBUG(m_pLog, "Current Time[%s],Send Time[%s],%d", strCurrentTime.c_str(), m_strSendTime.c_str(), l_ChangeTime <= l_CurrentTime);
			if (l_ChangeTime <=l_CurrentTime)
			{
				m_pMsgCenter->Send(l_strConfigMsg, l_SeatInfo.m_oBody.m_strNo, l_SeatInfo.m_oBody.m_strDeptCode, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
				m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			}
		}
	}
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::OnCNotifiSetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify)
{
	bool l_sendMsg = false;
	if (!p_pNotify)
	{
		ICC_LOG_ERROR(m_pLog, "UpdateTodayAlarm Request is nullptr");

		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receive UpdateMonitor seat message:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CMonitorSeatApplyRequest tmp_oMonitorSeatApplyReq;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();

	if (!tmp_oMonitorSeatApplyReq.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[UpdateMonitor seat]Parse request error.");
		return;
	}


	PROTOCOL::CMonitorSeatApplyRespond tmp_oRespond;
	tmp_oRespond.m_oHeader = tmp_oMonitorSeatApplyReq.m_oHeader;

	bool tmp_bSuccess = false;  //成功标识

	EnSyncType tmp_enSyncType = ADD;

	do
	{
		//较验applytype
		if (!_ValidSetMonitorSeatApply(tmp_oMonitorSeatApplyReq, tmp_oRespond.m_oHeader.m_strMsg))
		{
			tmp_oRespond.m_oHeader.m_strResult = "1";
			break;
		}

		//如果未带GUID表示新增，否则表示更新
		if (tmp_oMonitorSeatApplyReq.m_oBody.m_strGuid.empty())
		{
			l_sendMsg = true;
			tmp_oMonitorSeatApplyReq.m_oBody.m_strGuid = m_pString->CreateGuid();

			tmp_enSyncType = EDIT;

			if (!_AddOrUpdateApproveRecord(tmp_oMonitorSeatApplyReq, tmp_oRespond.m_oHeader.m_strMsg))
			{
				tmp_oRespond.m_oHeader.m_strResult = "2";
				break;
			}

			tmp_oRespond.m_oBody.m_strGuid = tmp_oMonitorSeatApplyReq.m_oBody.m_strGuid;
		}
		else
		{
			if (!_AddOrUpdateApproveRecord(tmp_oMonitorSeatApplyReq, tmp_oRespond.m_oHeader.m_strMsg, false))
			{
				tmp_oRespond.m_oHeader.m_strResult = "2";
				break;
			}

			//如果是添加黑名单，需要加到黑名单列表,不实现，由客户端做
			/*if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060002")
			{

			}*/
		}


		tmp_bSuccess = true;
		

	} while (0);
	
	
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();

	if (NULL == tmp_spJson.get())
	{
		ICC_LOG_ERROR(m_pLog, "CreateJson failed.")

		return;
	}

	std::string strMsg(tmp_oRespond.ToString(tmp_spJson));
	p_pNotify->Response(strMsg);

	ICC_LOG_DEBUG(m_pLog, "Response msg: %s.", strMsg.c_str());

	PROTOCOL::CMonitorSeatApplySync tmp_oSyncMsg;
	if (tmp_bSuccess)
	{
		//广播
		PROTOCOL::CMonitorSeatApplySync tmp_oSyncMsg;
		tmp_oSyncMsg.m_oBody.m_oSeatApplyInfo = tmp_oMonitorSeatApplyReq.m_oBody;

		tmp_oSyncMsg.m_oBody.m_strSyncType = std::to_string(tmp_enSyncType);

		tmp_oSyncMsg.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		tmp_oSyncMsg.m_oHeader.m_strCmd = "topic_apply_sync";
		tmp_oSyncMsg.m_oHeader.m_strRequest = "topic_master";
		tmp_oSyncMsg.m_oHeader.m_strRequestType = "1";//主题
		tmp_oSyncMsg.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

		//同步消息
		std::string l_strMsg(tmp_oSyncMsg.ToString(m_pJsonFty->CreateJson()));
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}

	if (l_sendMsg)
	{
		//将消息推送至消息中心
		MsgCenter::MessageSendDTOData l_CDTOData;
		string l_curSendMsg;
		l_CDTOData.appCode = "icc";
		l_CDTOData.businessCode = "monitor";
		l_CDTOData.compensateType = 0;
		l_CDTOData.delayDuration = "";
		l_CDTOData.delayType = 0;
		l_CDTOData.title = tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyRemark;
		if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060001")
		{
			l_CDTOData.title = BuildApprovalMsg(ALARM_UP);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyTargetID);

			std::string l_strApplyBody;
			l_strApplyBody = m_pString->ReplaceAll(tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyBody, "\"", "");
			l_strApplyBody = m_pString->ReplaceAll(l_strApplyBody, "{", "");
			l_strApplyBody = m_pString->ReplaceAll(l_strApplyBody, "}", "");
			std::vector<std::string> l_curValues;
			std::vector<std::string> l_curLevel;
			std::vector<std::string> l_curReason;
			m_pString->Split(l_strApplyBody, ",", l_curValues, true);
			if (l_curValues.size() == 0)
			{
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", "unknow");
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", "unknow");
			}
			else
			{
				m_pString->Split(l_curValues[0], ":", l_curLevel, true);
				m_pString->Split(l_curValues[1], ":", l_curReason, true);

				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_curLevel[1]);
				l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_curReason[1]);
			}
		}
		else if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060002")
		{
			l_CDTOData.title = BuildApprovalMsg(BLACK_APPLICATION);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyTargetID);
		}
		else if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060003")
		{
			l_CDTOData.title = BuildApprovalMsg(DEPARTURE_APPLICATION);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyTargetID);
		}
		else if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060005")
		{
			l_CDTOData.title = BuildApprovalMsg(PRIVACY_PROTECTION);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
			l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyTargetID);
		}

		l_CDTOData.moduleCode = "monitor";
		l_CDTOData.needStorage = "true";
		l_CDTOData.sendType = "0";
		l_CDTOData.sync = "true";
		l_CDTOData.message = tmp_oMonitorSeatApplyReq.ToStringOnlyBody(ICCGetIJsonFactory()->CreateJson());

		std::string l_deptGuid = _GetDeptGuid(tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffCode, tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
		std::vector<std::string> l_curNoticeList = m_pMsgCenter->GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

		if (l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}
			l_CDTOData.receiveObjects.push_back(l_recObj);
		}

		if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
		{
			ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
			return;
		}

		m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);
		m_pMsgCenter->Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
	}
	else
	{
		if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyType == "DIC060003")
		{
			std::string strSendMsg;
			std::string result;
			m_pMsgCenter->BuildManualAcceptConfig("DepartureApplicationBack", strSendMsg);
			strSendMsg = m_pString->ReplaceFirst(strSendMsg, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApproveStaffName);
			strSendMsg = m_pString->ReplaceFirst(strSendMsg, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName);
			strSendMsg = m_pString->ReplaceFirst(strSendMsg, "$", tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyTargetID);
			if (tmp_oMonitorSeatApplyReq.m_oBody.m_strApproveResult == "1")
				m_pMsgCenter->BuildManualAcceptConfig("DepartureApplicationResultOk", result);
			else
				m_pMsgCenter->BuildManualAcceptConfig("DepartureApplicationResultNo", result);
			strSendMsg = m_pString->ReplaceFirst(strSendMsg, "$", result);

			m_pMsgCenter->Send(strSendMsg, tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffName, tmp_oMonitorSeatApplyReq.m_oBody.m_strApplyStaffCode, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName, "monitor");
		}
	}
}

std::string CBusinessImpl::_GetDeptGuid(std::string l_staffCode, std::string l_staffName)
{
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_staff_all";
	l_strDeptSql.param["name"] = l_staffName;
	l_strDeptSql.param["code"] = l_staffCode;
	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec(l_strDeptSql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_sqlResult->GetSQL().c_str());

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return "";
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success, guid=%s, SQL: [%s]",
			l_sqlResult->GetValue("dept_guid").c_str(),
			l_sqlResult->GetSQL().c_str());

		return l_sqlResult->GetValue("dept_guid");
	}

	ICC_LOG_DEBUG(m_pLog, "select_icc_t_staff_all error: %s, %s", l_staffName.c_str(), l_staffCode.c_str());
	return "";
}

void CBusinessImpl::_GetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify, bool in_bSingel)
{

	if (NULL == p_pNotify.get())
	{
		ICC_LOG_ERROR(m_pLog, "Notify pointer is NULL");
		return;
	}

	PROTOCOL::CMonitorSeatApplyQueryRequest tmp_oMonitorSeatApplyQueryReq;
	tmp_oMonitorSeatApplyQueryReq.m_oBody.m_bSingelRequest = in_bSingel;

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!tmp_oMonitorSeatApplyQueryReq.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[GetMonitor seat]Parse request error.");
		return;
	}

	PROTOCOL::CMonitorSeatApplyQueryRespond tmp_oMonitorSeatApplyQueryRes;
	tmp_oMonitorSeatApplyQueryRes.m_oHeader = tmp_oMonitorSeatApplyQueryRes.m_oHeader;
	tmp_oMonitorSeatApplyQueryRes.m_oBody.m_strAllCount = "0";
	tmp_oMonitorSeatApplyQueryRes.m_oBody.m_strCount = "0";

	ICC_LOG_DEBUG(m_pLog, "Monitor seat page_size=%s,page_index=%s", tmp_oMonitorSeatApplyQueryReq.m_oBody.m_strPageSize.c_str(), tmp_oMonitorSeatApplyQueryReq.m_oBody.m_strPageIndex.c_str());

	do
	{
		if (!_ValidGetMonitorSeatApply(tmp_oMonitorSeatApplyQueryReq, tmp_oMonitorSeatApplyQueryRes.m_oHeader.m_strMsg))
		{
			tmp_oMonitorSeatApplyQueryRes.m_oHeader.m_strResult = "1";
			break;
		}

		if (!_GetApproveRecords(tmp_oMonitorSeatApplyQueryReq, tmp_oMonitorSeatApplyQueryRes))
		{
			tmp_oMonitorSeatApplyQueryRes.m_oHeader.m_strResult = "2";
			break;
		}
	} while (0);

	//tmp_oMonitorSeatApplyQueryRes.m_oHeader.m_strMsgId = tmp_oMonitorSeatApplyQueryReq.m_oHeader.m_strMsgId;
	//tmp_oMonitorSeatApplyQueryRes.m_oHeader.m_strMsgid = tmp_oMonitorSeatApplyQueryReq.m_oHeader.m_strMsgid;

	std::string l_strMessage = tmp_oMonitorSeatApplyQueryRes.ToString(ICCGetIJsonFactory()->CreateJson());
	tmp_oMonitorSeatApplyQueryRes.m_oBody.m_vecApplyInfo.clear();

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send apply Info Msg [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	
	_GetMonitorSeatApply(p_pNotify, false);
}


void CBusinessImpl::OnCNotifiGetSingleMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	_GetMonitorSeatSingleApply(p_pNotify);
	
}

void CBusinessImpl::_GetMonitorSeatSingleApply(ObserverPattern::INotificationPtr p_pNotify)
{

	if (NULL == p_pNotify.get())
	{
		ICC_LOG_ERROR(m_pLog, "Notify pointer is NULL");
		return;
	}

	PROTOCOL::CMonitorSeatSingleApplyQueryRequest tmp_oMonitorSeatSingleApplyQueryReq;

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!tmp_oMonitorSeatSingleApplyQueryReq.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[GetMonitor seat]Parse request error.");
		return;
	}

	PROTOCOL::CMonitorSeatSingleApplyQueryRespond tmp_oMonitorSeatSingleApplyQueryRes;
	tmp_oMonitorSeatSingleApplyQueryRes.m_oHeader = tmp_oMonitorSeatSingleApplyQueryReq.m_oHeader;

	if (!_GetSingleApproveRecords(tmp_oMonitorSeatSingleApplyQueryReq, tmp_oMonitorSeatSingleApplyQueryRes))
	{
		tmp_oMonitorSeatSingleApplyQueryRes.m_oHeader.m_strResult = "2";

	}

	std::string l_strMessage = tmp_oMonitorSeatSingleApplyQueryRes.ToString(ICCGetIJsonFactory()->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send apply Info Msg [%s]", l_strMessage.c_str());
}

std::string CBusinessImpl::BuildApprovalMsg(int l_curType)
{
	std::string strName;
	if (l_curType == ALARM_UP)
	{
		strName = "ApprovalMsgAlarmUp";
	}
	else if (l_curType == BLACK_APPLICATION)
	{
		strName = "BlackApplication";
	}
	else if (l_curType == DEPARTURE_APPLICATION)
	{
		strName = "DepartureApplication";
	}
	else if (l_curType == PRIVACY_PROTECTION)
	{
		strName = "PrivacyProtection";
	}
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, strName, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "HGet Param INFO Failed!!!");
		return "";
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Param Info failed!!!");
		return "";
	}

	return l_ParamInfo.m_strValue;
}

bool CBusinessImpl::_GetSingleApproveRecords(const PROTOCOL::CMonitorSeatSingleApplyQueryRequest& in_oReq, PROTOCOL::CMonitorSeatSingleApplyQueryRespond& out_oRes)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_alarm_monitor_seat_single_approval";
	if (!in_oReq.m_oBody.m_strApplyType.empty())
	{
		l_tSQLReq.param["apply_type"] = in_oReq.m_oBody.m_strApplyType;
	}

	if (!in_oReq.m_oBody.m_strApplyTargetID.empty())
	{
		l_tSQLReq.param["apply_target_id"] = in_oReq.m_oBody.m_strApplyTargetID;
	}

	if (!in_oReq.m_oBody.m_strApplyOrgCode.empty())
	{
		l_tSQLReq.param["apply_org_code"] = in_oReq.m_oBody.m_strApplyOrgCode;
	}

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		out_oRes.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	if (l_result->Next())
	{
		PROTOCOL::CMonitorSeatApplyInfo tmp_oSeatApplyInfo;
		out_oRes.m_oBody.m_ApplyInfo.m_strGuid = l_result->GetValue("guid");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyType = l_result->GetValue("apply_type");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyTargetID = l_result->GetValue("apply_target_id");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyTime = l_result->GetValue("apply_time");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyBody = l_result->GetValue("apply_body");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyStaffCode = l_result->GetValue("apply_staff_code");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyStaffName = l_result->GetValue("apply_staff_name");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyOrgCode = l_result->GetValue("apply_org_code");
		out_oRes.m_oBody.m_ApplyInfo.m_strApplyOrgName = l_result->GetValue("apply_org_name");
		out_oRes.m_oBody.m_ApplyInfo.m_strApproveStaffCode = l_result->GetValue("approve_staff_code");
		out_oRes.m_oBody.m_ApplyInfo.m_strApproveStaffName = l_result->GetValue("approve_staff_name");
		out_oRes.m_oBody.m_ApplyInfo.m_strApproveResult = l_result->GetValue("approve_result");
		out_oRes.m_oBody.m_ApplyInfo.m_strApproveTime = l_result->GetValue("approve_time");
		out_oRes.m_oBody.m_ApplyInfo.m_strApproveRemark = l_result->GetValue("approve_remark");
	}

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
	if (!l_ParamInfo.Parse(strParamInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
		return;
	}
	l_value = l_ParamInfo.m_strValue;
}