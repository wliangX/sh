#include "Boost.h"
#include "BusinessImpl.h"
#include "DefineInfo.h"

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_WECHATST_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	SetBindMember();
}

void CBusinessImpl::SetBindMember()
{
	std::string l_strMembers = m_pConfig->GetValue("ICC/Plugin/WeChatST/ProcessResults", "");
	std::vector<std::string> l_vecSplitMembers;
	m_pString->Split(m_pString->ReplaceAll(l_strMembers, " ", ""), ";", l_vecSplitMembers, 1);
	for (auto strSplit : l_vecSplitMembers)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			if (CORRECTSIZE == l_vecSubSplit.size())
			{
				m_mProcessRes[eWeChatResult(m_pString->ToInt(l_vecSubSplit.at(0)))] = l_vecSubSplit.at(1);
			}
		}
	}
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/WeChatST/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "DS_AcceptWeChatAlarm", OnNotifiWeChatDSAcceptAlarmInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "DS_AcceptChangeInfo", OnNotifiWeChatDSAcceptChangeInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "ta_changeinfo_sync", OnNotifiWeChatOSAcceptChangeInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnNotifiWeChatProcessResult);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiWeChatDSAcceptAlarmInfo(ObserverPattern::INotificationPtr p_pDSAcceptWeChatAlarm)
{
	std::string l_strMsg = p_pDSAcceptWeChatAlarm->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CWeChatAlarmInfo l_oWeChatAlarmInfo;

	if (!l_oWeChatAlarmInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	for (PROTOCOL::CWeChatAlarmInfo::COMPLAINTLIST var :
		l_oWeChatAlarmInfo.m_oBody.m_Complaintlist)
	{
		std::string l_strUserPhone = var.m_strUSERPHONE;
		std::string l_strGuid = var.m_strGUID;
		std::string l_stDeptID = var.m_strTOORGID;

		DataBase::SQLRequest l_oBlacklistSql;
		l_oBlacklistSql.sql_id = SELECT_ICC_T_BLACKLIST;
		l_oBlacklistSql.param["limit_num"] = l_strUserPhone;
		l_oBlacklistSql.param["is_delete"] = BLACKLIST_DELETE_FLAG;

		DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oBlacklistSql);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
		if (!l_oResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
			continue;
		}
		// 判断是否黑名单或重复报警 [10/11/2018 w26326]
		if (l_oResult->Next() || SelectThirdAlarm(l_strGuid))
		{
			PROTOCOL::CWeChatProcessResult l_oProcessResult;
			l_oProcessResult.m_oHeader.m_strSystemID = SYSTEMID;
			l_oProcessResult.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
			l_oProcessResult.m_oHeader.m_strMsgid = m_pString->CreateGuid();
			l_oProcessResult.m_oHeader.m_strRelatedID = l_oWeChatAlarmInfo.m_oHeader.m_strMsgid;
			l_oProcessResult.m_oHeader.m_strCmd = WECHART_PROCESS_RESULT;
			l_oProcessResult.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oProcessResult.m_oHeader.m_strRequest = WECHART_TOPIC;
			l_oProcessResult.m_oHeader.m_strRequestType = WECHAT_MQTYPE_TOPIC;

			l_oProcessResult.m_oBody.m_strALARMGUID = l_strGuid;
			l_oProcessResult.m_oBody.m_strOPENID = var.m_strOPENID;
			l_oProcessResult.m_oBody.m_strCHECKDATE = m_pDateTime->CurrentDateTimeStr();

			if (l_oResult->Next())
			{
				// 黑名单回复服务受限 [10/11/2018 w26326]
				l_oProcessResult.m_oBody.m_strCHECKTYPE = std::to_string(LimitedService);
				l_oProcessResult.m_oBody.m_strCHECKRESULT = m_BlackMsg;
			}
			if (SelectThirdAlarm(l_strGuid))
			{
				// 重复报警 [11/29/2018 w26326]
				l_oProcessResult.m_oBody.m_strCHECKTYPE = std::to_string(RepeatandEnd);
			}

			// Topic_WebService [10/15/2018 w26326]
			std::string l_strMsg = l_oProcessResult.ToString(m_pJsonFty->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
			continue;
		}

		// 发送微信报警到客户端addwechatalarm [10/15/2018 w26326]
		SendAddWeChatAlarm(var);

		// 发送接收成功 [11/29/2018 w26326]
		SendAcceptWeChatAlarm(var.m_strGUID);
	}
}

void CBusinessImpl::OnNotifiWeChatDSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pDSAcceptChangeInfo)
{
	std::string l_strMsg = p_pDSAcceptChangeInfo->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CDSWeChatChangeInfo l_oWeChatChangeInfo;

	if (!l_oWeChatChangeInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	SendAddChangeInfo(l_oWeChatChangeInfo);
}

void CBusinessImpl::OnNotifiWeChatOSAcceptChangeInfo(ObserverPattern::INotificationPtr p_pOSAcceptChangeInfo)
{
	std::string l_strMsg = p_pOSAcceptChangeInfo->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CTASyncChangeInfoRequest l_oSyncChangeInfo;
	if (!l_oSyncChangeInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	if (l_oSyncChangeInfo.m_oBody.m_strType == WECHAT_TYPE_ST&&
		l_oSyncChangeInfo.m_oBody.m_strOrientation == POLICE2CONTACT)
	{
		PROTOCOL::COSWeChatChangeInfo l_oWeChatChangeInfo;

		l_oWeChatChangeInfo.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oWeChatChangeInfo.m_oHeader.m_strCmd = WECHART_CHANGE_INFO;
		l_oWeChatChangeInfo.m_oHeader.m_strRequest = WECHART_TOPIC;
		l_oWeChatChangeInfo.m_oHeader.m_strRequestType = WECHAT_MQTYPE_TOPIC;
		l_oWeChatChangeInfo.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

		l_oWeChatChangeInfo.m_oBody.m_strGUID = l_oSyncChangeInfo.m_oBody.m_strThirdAlarmGuid;
		l_oWeChatChangeInfo.m_oBody.m_strPHONE = l_oSyncChangeInfo.m_oBody.m_strPhone;
		l_oWeChatChangeInfo.m_oBody.m_strOPENID = l_oSyncChangeInfo.m_oBody.m_strOpenID;
		l_oWeChatChangeInfo.m_oBody.m_strCHANGETYPE = l_oSyncChangeInfo.m_oBody.m_strChangeType;
		l_oWeChatChangeInfo.m_oBody.m_strCHANGECONTENT = l_oSyncChangeInfo.m_oBody.m_strChangeContent;
		l_oWeChatChangeInfo.m_oBody.m_strCHANGEDATE = l_oSyncChangeInfo.m_oBody.m_strChangeDate;
		l_oWeChatChangeInfo.m_oBody.m_strREMARK1 = l_oSyncChangeInfo.m_oBody.m_strRemark1;
		l_oWeChatChangeInfo.m_oBody.m_strREMARK2 = l_oSyncChangeInfo.m_oBody.m_strRemark2;

		std::string l_strMsg = l_oWeChatChangeInfo.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiWeChatProcessResult(ObserverPattern::INotificationPtr p_pOSProcessResultInfo)
{
	std::string l_strMsg = p_pOSProcessResultInfo->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message[%s]", l_strMsg.c_str());
	PROTOCOL::CAlarmSync l_oSyncAlarmInfo;
	if (!l_oSyncAlarmInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error:[%s]", l_strMsg.c_str());
		return;
	}

	std::string l_strOpenID;
	if (l_oSyncAlarmInfo.m_oBody.m_strSourceType == WECHAT_TYPE_ST&&
		SelectThirdAlarm(l_oSyncAlarmInfo.m_oBody.m_strSourceID, l_strOpenID))
	{// 判断是否是省厅微信报警 DIC019090已反馈[11/29/2018 w26326]

		PROTOCOL::CWeChatProcessResult p_oProcess;
		p_oProcess.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		p_oProcess.m_oHeader.m_strCmd = WECHART_PROCESS_RESULT;
		p_oProcess.m_oHeader.m_strRequest = WECHART_TOPIC;
		p_oProcess.m_oHeader.m_strRequestType = WECHAT_MQTYPE_TOPIC;
		p_oProcess.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

		p_oProcess.m_oBody.m_strALARMGUID = l_oSyncAlarmInfo.m_oBody.m_strSourceID;
		p_oProcess.m_oBody.m_strOPENID = l_strOpenID;

		if (l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_PROCESSED ||
			l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_PROCESING)
		{
			p_oProcess.m_oBody.m_strCHECKTYPE = std::to_string(Processing);
			p_oProcess.m_oBody.m_strCHECKRESULT = GetResMsg(Processing);
		}
		else if (l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_INVALID)
		{
			p_oProcess.m_oBody.m_strCHECKTYPE = std::to_string(InvalidandEnd);
			p_oProcess.m_oBody.m_strCHECKRESULT = GetResMsg(InvalidandEnd);
		}
		else if (l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_REPEAT)
		{
			p_oProcess.m_oBody.m_strCHECKTYPE = GetResMsg(RepeatandEnd);
			p_oProcess.m_oBody.m_strCHECKRESULT = GetResMsg(RepeatandEnd);
		}
		else if (l_oSyncAlarmInfo.m_oBody.m_strState == ALARM_STATE_FEEDBACK)
		{
			p_oProcess.m_oBody.m_strCHECKTYPE = GetResMsg(Processed);
			p_oProcess.m_oBody.m_strCHECKRESULT = GetResMsg(Processed);
		}
		else
		{
			// 其它状态 [12/5/2018 w26326]
			ICC_LOG_ERROR(m_pLog, "other state:[%s]", l_oSyncAlarmInfo.m_oBody.m_strState.c_str());
			return;
		}

		p_oProcess.m_oBody.m_strACORGID = l_oSyncAlarmInfo.m_oBody.m_strReceiptDeptCode;
		p_oProcess.m_oBody.m_strACORGNAME = l_oSyncAlarmInfo.m_oBody.m_strReceiptDeptName;
		p_oProcess.m_oBody.m_strCHECKDATE = m_pDateTime->CurrentDateTimeStr();
		p_oProcess.m_oBody.m_strALARMCODE = l_oSyncAlarmInfo.m_oBody.m_strID;

		std::string l_strMsg = p_oProcess.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::SendAddWeChatAlarm(const PROTOCOL::CWeChatAlarmInfo::COMPLAINTLIST &p_oWeChatInfo)
{
	PROTOCOL::CAddWeChatAlarm l_oAddWeChatAlarm;
	l_oAddWeChatAlarm.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAddWeChatAlarm.m_oHeader.m_strCmd = ADD_WECHAT_ALARM;
	l_oAddWeChatAlarm.m_oHeader.m_strRequest = ADD_WECHAT_QUEUE;
	l_oAddWeChatAlarm.m_oHeader.m_strRequestType = WECHAT_MQTYPE_QUEUE;
	l_oAddWeChatAlarm.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	l_oAddWeChatAlarm.m_oBody.m_strGuid = p_oWeChatInfo.m_strGUID;
	l_oAddWeChatAlarm.m_oBody.m_strType = WECHAT_TYPE_ST;
	l_oAddWeChatAlarm.m_oBody.m_strAddress = p_oWeChatInfo.m_strADDRESS;
	l_oAddWeChatAlarm.m_oBody.m_strCrcontent = p_oWeChatInfo.m_strCRCONTENT;
	l_oAddWeChatAlarm.m_oBody.m_strCreateDate = p_oWeChatInfo.m_strCREATEDATE;
	l_oAddWeChatAlarm.m_oBody.m_strIsPhone = p_oWeChatInfo.m_bISPHONE;
	l_oAddWeChatAlarm.m_oBody.m_strUserCode = p_oWeChatInfo.m_strUSERCODE;
	l_oAddWeChatAlarm.m_oBody.m_strUserName = p_oWeChatInfo.m_strUSERNAME;
	l_oAddWeChatAlarm.m_oBody.m_strUserPhone = p_oWeChatInfo.m_strUSERPHONE;
	l_oAddWeChatAlarm.m_oBody.m_strUserAddress = p_oWeChatInfo.m_strUSERADDRESS;
	l_oAddWeChatAlarm.m_oBody.m_strUnitContactor = p_oWeChatInfo.m_strUNITCONTACTOR;
	l_oAddWeChatAlarm.m_oBody.m_strHandPhone = p_oWeChatInfo.m_strHANDPHONE;
	l_oAddWeChatAlarm.m_oBody.m_strToOrgID = p_oWeChatInfo.m_strTOORGID;
	l_oAddWeChatAlarm.m_oBody.m_strToOrgName = p_oWeChatInfo.m_strTOORGNAME;
	l_oAddWeChatAlarm.m_oBody.m_strComNo = p_oWeChatInfo.m_strCOMNO;
	l_oAddWeChatAlarm.m_oBody.m_strOpenID = p_oWeChatInfo.m_strOPENID;
	l_oAddWeChatAlarm.m_oBody.m_strLong = p_oWeChatInfo.m_strLONG;
	l_oAddWeChatAlarm.m_oBody.m_strLat = p_oWeChatInfo.m_strLAT;
	l_oAddWeChatAlarm.m_oBody.m_strRemark1 = p_oWeChatInfo.m_strREMARK1;
	l_oAddWeChatAlarm.m_oBody.m_strRemark2 = p_oWeChatInfo.m_strREMARK2;
	l_oAddWeChatAlarm.m_oBody.m_oMaterial = p_oWeChatInfo.m_material;

	std::string l_strMsg = l_oAddWeChatAlarm.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

bool ICC::CBusinessImpl::SelectThirdAlarm(std::string p_strGuid, std::string& p_strOpenID)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_ICC_T_THIRD_ALARM;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_ICC_T_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_strOpenID = l_pResult->GetValue("open_id");
		return true;
	}
	return false;
}

bool ICC::CBusinessImpl::SelectThirdAlarm(std::string p_strGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_ICC_T_THIRD_ALARM;
	l_Sql.param["guid"] = p_strGuid;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_ICC_T_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		return false;
	}
	return true;
}

void ICC::CBusinessImpl::SendAcceptWeChatAlarm(std::string p_strGuid)
{
	PROTOCOL::CAcceptWeChatAlarm l_oAcceptRes;
	l_oAcceptRes.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAcceptRes.m_oHeader.m_strCmd = WECHART_ACCEPT_INFO;
	l_oAcceptRes.m_oHeader.m_strRequest = WECHART_TOPIC;
	l_oAcceptRes.m_oHeader.m_strRequestType = WECHAT_MQTYPE_QUEUE;
	l_oAcceptRes.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();


	l_oAcceptRes.m_oBody.m_strGuid = p_strGuid;
	l_oAcceptRes.m_oBody.m_strIsAccept = WECAHT_ACCEPT;
	l_oAcceptRes.m_oBody.m_strAcceptData = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = l_oAcceptRes.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

std::string CBusinessImpl::GetResMsg(eWeChatResult p_eResType)
{
	std::string l_strRes;
	auto iter = m_mProcessRes.find(p_eResType);
	if (iter!=m_mProcessRes.end())
	{
		l_strRes = iter->second;
	}
	return l_strRes;
}

void CBusinessImpl::SendWeChatReturnMsg(PROTOCOL::CRetureMsg& p_oReturnMsg)
{
	p_oReturnMsg.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	p_oReturnMsg.m_oHeader.m_strCmd = WECHART_PROCESS_RESULT;
	p_oReturnMsg.m_oHeader.m_strRequest = WECHART_TOPIC;
	p_oReturnMsg.m_oHeader.m_strRequestType = WECHAT_MQTYPE_TOPIC;
	p_oReturnMsg.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMsg = p_oReturnMsg.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SendAddChangeInfo(PROTOCOL::CDSWeChatChangeInfo& p_oWeChatChangeInfo)
{
	p_oWeChatChangeInfo.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	p_oWeChatChangeInfo.m_oHeader.m_strCmd = ADD_CHANGE_INFO;
	p_oWeChatChangeInfo.m_oHeader.m_strRequest = ADD_WECHAT_QUEUE;
	p_oWeChatChangeInfo.m_oHeader.m_strRequestType = WECHAT_MQTYPE_QUEUE;
	p_oWeChatChangeInfo.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

	p_oWeChatChangeInfo.m_oBody.m_strGUID = m_pString->CreateGuid();
	p_oWeChatChangeInfo.m_oBody.m_strOrientation = CONTACT2POLICE;
	p_oWeChatChangeInfo.m_oBody.m_strState = NOT_READ;

	std::string l_strMsg = p_oWeChatChangeInfo.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}



             
