#include "Boost.h"
#include "BusinessImpl.h"

#define CLIENT_REGISTER_INFO "ClientRegisterInfo"
#define STAFF_ID_NO      "1"
#define STAFF_CODE       "2"

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pLockFty = ICCGetILockFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_iTimeOut = 0;

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);
	
	m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
	m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/CodeMode", "1");
	SetBindMember();
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_REQUEST, OnCNotifiTransportAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_ACCEPT_REQUEST, OnCNotifiTransportAcceptAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_REFUSE_REQUEST, OnCNotifiTransportRefuseAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_CACHE_REQUEST, OnCNotifiTransportCacheRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_ADDALARM_RESPOND, OnTransNewAlarmIDResponse);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, ALARM_TRANSPORT_TRANSFERCALL_SYNC, OnTransferCallSync);

	ICC_LOG_INFO(m_pLog, "transportalarm start success!");
	ICC_LOG_DEBUG(m_pLog,"OnStart complete! plugin = %s，AssistantPolice=%s,Police=%s,CodeMode:[%s]\n", MODULE_NAME,m_strAssistantPolice.c_str(), m_strPolice.c_str(), m_strCodeMode.c_str());

}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "transportalarm stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::SetBindMember()
{
	// 部门VDN分组关系绑定 [8/3/2018 w26326]
	std::string l_strMembers = m_pConfig->GetValue("ICC/Plugin/TransportAlarm/DeptAcdMappings", "");
	std::vector<std::string> l_vecSplitTopics;
	const unsigned int l_iCorrectSize = 3;
	m_pString->Split(m_pString->ReplaceAll(l_strMembers, " ", ""), ";", l_vecSplitTopics, 1);
	for (auto strSplit : l_vecSplitTopics)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			if (l_iCorrectSize == l_vecSubSplit.size())
			{
				m_pRedisClient->HSet(MANGE_TRANSPORT_BINDMEMBER, l_vecSubSplit.at(0), l_vecSubSplit.at(1));
			}
		}
	}

	m_strDefaultVDN = m_pConfig->GetValue("ICC/Plugin/TransportAlarm/DefaultDeptAcd", "110");
	m_iTimeOut = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/TransportAlarm/TimeOut", "20"));
}

std::string CBusinessImpl::GetFreeSeatNo(std::string p_strDeptCode,std::string p_strSrcSeatNo)
{
	// 获取空闲席位
	std::vector<std::string> l_vFreeSeatNo;
	std::map<std::string, std::string> l_mapItem;
	m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem);

	for (auto var : l_mapItem)
	{
		CUserDept l_oUserDept;
		l_oUserDept.Parse(var.second, m_pJsonFty->CreateJson());

		if (l_oUserDept.m_strDeptCode == p_strDeptCode &&
			l_oUserDept.m_strSeatNo != p_strSrcSeatNo &&
			l_oUserDept.m_strBuyIdle == READY_STATE_IDLE)
		{
			l_vFreeSeatNo.push_back(l_oUserDept.m_strSeatNo);

			break;
		}
	}


	unsigned int l_iIndex = 0;
	l_iIndex = GetRandIndex(l_vFreeSeatNo.size());
	std::string l_strFreeSeatNo;
	if (l_vFreeSeatNo.empty())
	{
		ICC_LOG_INFO(m_pLog, "seat all busy!");
		return l_strFreeSeatNo;
	}
	l_strFreeSeatNo = l_vFreeSeatNo.at(l_iIndex);
	return l_strFreeSeatNo;
}

bool CBusinessImpl::IsFreeSeatNo(std::string p_strDeptCode,std::string p_strSeatNo)
{
	std::map<std::string, std::string> l_mapItem;
	m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem);

	for (auto var : l_mapItem)
	{
		CUserDept l_oUserDept;
		l_oUserDept.Parse(var.second, m_pJsonFty->CreateJson());

		if (l_oUserDept.m_strDeptCode == p_strDeptCode &&
			l_oUserDept.m_strSeatNo == p_strSeatNo &&
			l_oUserDept.m_strBuyIdle == READY_STATE_IDLE)
		{
			return true;
		}
	}

	ICC_LOG_ERROR(m_pLog, "not free seatno, dept code: %s, seatno: %s", p_strDeptCode.c_str(), p_strSeatNo.c_str());
	return false;
}

unsigned int CBusinessImpl::GetRandIndex(unsigned int p_nSize)
{
	// 产生随机席位的索引 [8/3/2018 w26326]
	if (p_nSize <= 0)
	{
		return 0;
	}
	unsigned int nRand = m_pString->CreateRandom(10, 10 * (p_nSize + 1));
	unsigned int nIndex = (nRand / 10) - 1;

	return (nIndex < p_nSize) ? nIndex : p_nSize - 1;
}

bool CBusinessImpl::IsValidForTransPortRequest(const PROTOCOL::CTransPortAlarmRequest& l_oTransAlarmRequest, std::string& out_strErrorMsg)
{
	if (l_oTransAlarmRequest.m_oBody.m_strAlarmID.empty() || 
		l_oTransAlarmRequest.m_oBody.m_strDeptCode.empty() ||
		l_oTransAlarmRequest.m_oBody.m_strDeptName.empty() ||
		l_oTransAlarmRequest.m_oBody.m_strSeatNo.empty())
	{
		out_strErrorMsg = "invalid transportalrm request, Reasion: AlarmID DeptCode DeptName or SeatNo is mull";
		ICC_LOG_ERROR(m_pLog, "Check failed,invalid transportalrm request[%s], Reasion: AlarmID DeptCode DeptName or SeatNo is mull", 
			l_oTransAlarmRequest.m_oHeader.m_strMsgid.c_str());
		return false;
	}

	if (!l_oTransAlarmRequest.m_oBody.m_strCallRefID.empty())
	{
		if (l_oTransAlarmRequest.m_oBody.m_strTargetDeptCode.empty())
		{
			out_strErrorMsg = "invalid transportalrm request, Reasion: callref is not empty, target DeptCode is mull";
			ICC_LOG_ERROR(m_pLog, "Check failed,invalid transportalrm request[%s], Reasion: TargetDeptCode is null",
				l_oTransAlarmRequest.m_oHeader.m_strMsgid.c_str());
			return false;
		}
	}
	return true;
}

void CBusinessImpl::OnCNotifiTransportAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strMessage = p_pNotify->GetMessages();
	PROTOCOL::CTransPortAlarmRequest l_oTransAlarmRequest;
	if (!l_oTransAlarmRequest.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parseString error:[%s]", l_strMessage.c_str());
		return;
	}
	std::string l_strRelateID = l_oTransAlarmRequest.m_oHeader.m_strMsgId;
	std::string tmp_strMsg("success");
	if (!IsValidForTransPortRequest(l_oTransAlarmRequest, tmp_strMsg))
	{
		SendTransportAlarmRespond(TransportFail, p_pNotify, l_strRelateID, tmp_strMsg);
		ICC_LOG_ERROR(m_pLog, "invalid transport request");
		return;
	}

	if (!UpdateAlarm(l_oTransAlarmRequest.m_oBody.m_oAlarm) ||
		!InsertTransportTable(l_oTransAlarmRequest))
	{
		tmp_strMsg = "update alarm or insert transport table failed";
		SendTransportAlarmRespond(TransportFail, p_pNotify, l_strRelateID, tmp_strMsg);
		ICC_LOG_ERROR(m_pLog, "insert DB error");
		return;
	}

	std::string l_strTransportType = l_oTransAlarmRequest.m_oBody.m_strTransportType;
	std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, TIMER_ONE_TIME, m_iTimeOut);

	CTransportQueue l_oTransportQueue;
	l_oTransportQueue.strAlarmID = l_oTransAlarmRequest.m_oBody.m_strAlarmID;
	l_oTransportQueue.oSourceInfo = l_oTransAlarmRequest;
	l_oTransportQueue.strTimerName = l_strTimerName;
	l_oTransportQueue.strTransportType = l_strTransportType;

	// 添加转警队列 [6/1/2018 w26326]
	AddTransportQue(l_oTransAlarmRequest.m_oBody.m_strAlarmID, l_oTransportQueue);

	SendTransportAlarmRespond(TransportSuccess, p_pNotify, l_strRelateID, tmp_strMsg);

	if (l_strTransportType.compare(TRANSPORT_TYPE_TAKEOVER) == 0)
	{
		//话务接管转警，生成新警单
		SendTransNewAlarmIDRequest(l_oTransAlarmRequest.m_oBody.m_strAlarmID);
	}
	else
	{
		std::vector<std::string> l_vecParamList;
		Data::CStaffInfo l_oStaffInfo;
		std::string strStaffCode = _GetStaffCodeBySeat(l_oTransAlarmRequest.m_oBody.m_strSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oTransAlarmRequest.m_oBody.m_strUserName);
		l_vecParamList.push_back(strTransportName);
		l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strSeatNo);
		l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strDeptName);
		if (l_oTransAlarmRequest.m_oBody.m_strAssignFlag == ASSIGNED)
		{
			// {0}[工号-{1}, 单位-{2}]申请转移警单到{3}[工号-{4}, 单位-{5}]的转警请求 [5/3/2018 w26326]
			Data::CStaffInfo l_oStaffInfo;
			std::string strStaffCode = _GetStaffCodeBySeat(l_oTransAlarmRequest.m_oBody.m_strTargetSeatNo);
			if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
			}
			std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oStaffInfo.m_strName);
	
			l_vecParamList.push_back(strTargetName);
			l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strTargetSeatNo);
			l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strTargetDeptName);
			l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strTransportReason);
			EditTransportAlarmLog(l_vecParamList, l_oTransAlarmRequest.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_EX_APPLY);
		}
		else
		{
			// 申请转警流水{0}[工号-{1}, 单位-{2}]申请转移警单到[单位-{3}] [5/3/2018 w26326]
			l_vecParamList.push_back(l_oTransAlarmRequest.m_oBody.m_strTargetDeptName);
			EditTransportAlarmLog(l_vecParamList, l_oTransAlarmRequest.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_APPLY);
		}

		// 发送转警同步 [6/21/2018 w26326]
		if (!l_oTransAlarmRequest.m_oBody.m_strCallRefID.empty())
		{
			// 发送转电话请求 [6/11/2018 w26326]
			SendTransferCallRequest(l_oTransAlarmRequest.m_oBody.m_strAlarmID);
		}
		else
		{
			if (l_oTransAlarmRequest.m_oBody.m_strAssignFlag == ASSIGNED)
			{
				if (l_strTransportType != "3")
				{
					// 不带话务转警、指定坐席 [8/3/2018 w26326]
					if (!IsFreeSeatNo(l_oTransAlarmRequest.m_oBody.m_strTargetDeptCode, l_oTransAlarmRequest.m_oBody.m_strTargetSeatNo))
					{
						SendTopicTransResultSync(l_oTransAlarmRequest.m_oBody.m_strAlarmID, TransportFail, "");
						return;
					}
					else
					{
						SendTopicTransportInfo(l_oTransAlarmRequest.m_oBody.m_strAlarmID, l_oTransAlarmRequest.m_oBody.m_strTargetSeatNo, l_strTransportType);
					}
				}
				else
				{
					SendTopicTransportInfo(l_oTransAlarmRequest.m_oBody.m_strAlarmID, l_oTransAlarmRequest.m_oBody.m_strTargetSeatNo, l_strTransportType);
				}
				
			}
			else
			{
				// 不带话务转警、转部门 [8/3/2018 w26326]
				std::string strSeatNo = GetFreeSeatNo(l_oTransAlarmRequest.m_oBody.m_strTargetDeptCode, l_oTransAlarmRequest.m_oBody.m_strSeatNo);
				if (strSeatNo.empty())
				{
					SendTopicTransResultSync(l_oTransAlarmRequest.m_oBody.m_strAlarmID, TransportFail, "");
					return;
				}
				SendTopicTransportInfo(l_oTransAlarmRequest.m_oBody.m_strAlarmID, strSeatNo, l_strTransportType);
				SetTraget(strSeatNo, l_oTransAlarmRequest.m_oBody.m_strAlarmID);
			}
		}
	}
}

void CBusinessImpl::OnCNotifiTransportAcceptAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strMessage = p_pNotify->GetMessages();
	PROTOCOL::CAlarmTransportAcceptRequest l_oAcceptAlarmInfo;
	if (!l_oAcceptAlarmInfo.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parsestring error:[%s]", l_strMessage.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "xxx transport accept 1.0.");

	PROTOCOL::CTransPortAlarmRequest l_oSourceInfo;
	if (!GetAlarmInfoByAlarmID(l_oAcceptAlarmInfo.m_oBody.m_strAlarmID, l_oSourceInfo))
	{
		SendTransportAcceptAlarmRespond(TransportHangUp, p_pNotify, l_oAcceptAlarmInfo.m_oHeader.m_strMsgId);
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "xxx transport accept, GetAlarmInfoByAlarmID  2.0.");
	std::string l_strCallrefID = l_oSourceInfo.m_oBody.m_strCallRefID;

	if (!UpdateTransportTable(l_oAcceptAlarmInfo.m_oBody))
	{
		ICC_LOG_ERROR(m_pLog, "update transportalarm fail");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "xxx transport accept, UpdateTransportTable  3.0.");

	// {0}[工号-{1}, 单位-{2}]接受{3}[工号-{4}, 单位-{5}]的转警请求 [5/3/2018 w26326]
	std::vector<std::string> l_vecParamList;

	Data::CStaffInfo l_oStaffInfo;
	std::string strStaffCode = _GetStaffCodeBySeat(l_oAcceptAlarmInfo.m_oBody.m_strSeatNo);
	if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
	}
	std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oAcceptAlarmInfo.m_oBody.m_strUserName);
	l_vecParamList.push_back(strTransportName);
	l_vecParamList.push_back(l_oAcceptAlarmInfo.m_oBody.m_strSeatNo);
	l_vecParamList.push_back(l_oAcceptAlarmInfo.m_oBody.m_strDeptName);
	strStaffCode = _GetStaffCodeBySeat(l_oAcceptAlarmInfo.m_oBody.m_strSeatNo);
	if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
	}
	std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oAcceptAlarmInfo.m_oBody.m_strUserName);
	l_vecParamList.push_back(strTargetName);
	l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strSeatNo);
	l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strDeptName);
	EditTransportAlarmLog(l_vecParamList, l_oSourceInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_ACCEPT);

	ICC_LOG_DEBUG(m_pLog, "xxx transport accept, EditTransportAlarmLog  4.0.");

	SetTraget(l_oAcceptAlarmInfo.m_oBody.m_strUserName, l_oAcceptAlarmInfo.m_oBody.m_strUserCode, l_oAcceptAlarmInfo.m_oBody.m_strAlarmID);

	if (l_oSourceInfo.m_oBody.m_strCallRefID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "xxx transport accept, not with calling  5.0.");

		SendTransNewAlarmIDRequest(l_oSourceInfo.m_oBody.m_strAlarmID);
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "xxx transport accept, with calling  6.0.");

		// 挂电话后的带话务转警处理、只转话务不转警情 [7/19/2018 w26326]
		PROTOCOL::CAlarmTransferCallRespond l_oTransCallInfo;
		if (GetTransCallInfo(l_oSourceInfo.m_oBody.m_strAlarmID, l_oTransCallInfo) &&
			l_oTransCallInfo.m_oBody.m_strNewCallRefId.empty())
		{

			ICC_LOG_DEBUG(m_pLog, "xxx transport accept, with calling  6.1.");
			SendTransNewAlarmIDRequest(l_oSourceInfo.m_oBody.m_strAlarmID);
		}
	}

	// 回复成功、收到转警接收消息 [5/9/2018 w26326]
	SendTransportAcceptAlarmRespond(TransportSuccess, p_pNotify, l_oAcceptAlarmInfo.m_oHeader.m_strMsgId);
}

void CBusinessImpl::OnCNotifiTransportRefuseAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strMessage = p_pNotify->GetMessages();
	PROTOCOL::CAlarmTransportRefuseRequest l_oRefuseAlarmInfo;
	if (!l_oRefuseAlarmInfo.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parsesing error:[%s]", l_strMessage.c_str());
		return;
	}

	// 发起重连电话请求 [6/19/2018 w26326]
	PROTOCOL::CTransPortAlarmRequest l_oInfo;
	if (!GetAlarmInfoByAlarmID(l_oRefuseAlarmInfo.m_oBody.m_strAlarmID, l_oInfo))
	{
		return;
	}
	if (!l_oInfo.m_oBody.m_strCallRefID.empty())
	{
		SendReCallRequest(l_oRefuseAlarmInfo.m_oBody.m_strAlarmID);
	}
	
	PROTOCOL::CAlarmTransportRespond l_oRefuseRespond;
	l_oRefuseRespond.m_oHeader.m_strCmd = ALARM_TRANSPORT_REFUSE_RESPOND;
	l_oRefuseRespond.m_oHeader.m_strMsgId = l_oRefuseAlarmInfo.m_oHeader.m_strMsgId;

	l_oRefuseRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oRefuseRespond.m_oHeader.m_strRelatedID = l_oRefuseAlarmInfo.m_oHeader.m_strMsgid;
	l_oRefuseRespond.m_oBody.m_strResult = ALARM_TRANSPORT_RESULT_STATE_SUCCESS;

	l_strMessage = l_oRefuseRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "OnCNotifiTransportRefuseAlarmRequest send message:[%s]", l_strMessage.c_str());

	// {0}[工号-{1}, 单位-{2}]拒绝{3}[工号-{4}, 单位-{5}]的转警请求,原因[6] [5/3/2018 w26326]
	std::vector<std::string> l_vecParamList;
	Data::CStaffInfo l_oStaffInfo;
	std::string strStaffCode = _GetStaffCodeBySeat(l_oRefuseAlarmInfo.m_oBody.m_strSeatNo);
	if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
	}
	std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oRefuseAlarmInfo.m_oBody.m_strUserName);
	l_vecParamList.push_back(strTransportName);
	l_vecParamList.push_back(l_oRefuseAlarmInfo.m_oBody.m_strSeatNo);
	l_vecParamList.push_back(l_oRefuseAlarmInfo.m_oBody.m_strDeptName);

	strStaffCode = _GetStaffCodeBySeat(l_oInfo.m_oBody.m_strSeatNo);
	if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
	}
	std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oInfo.m_oBody.m_strUserName);
	l_vecParamList.push_back(strTargetName);
	l_vecParamList.push_back(l_oInfo.m_oBody.m_strSeatNo);
	l_vecParamList.push_back(l_oInfo.m_oBody.m_strDeptName);
	l_vecParamList.push_back(l_oRefuseAlarmInfo.m_oBody.m_strRefuseReason);
	EditTransportAlarmLog(l_vecParamList, l_oInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_REFUSE);

	SendTopicTransResultSync(l_oRefuseAlarmInfo.m_oBody.m_strAlarmID, TransportRefuse, l_oRefuseAlarmInfo.m_oBody.m_strRefuseReason);
}

void CBusinessImpl::OnCNotifiTransportCacheRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	//获取转警缓存
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strMessage = p_pNotify->GetMessages();
	PROTOCOL::CAlarmTransportCacheRequest l_oTransCacheRequest;
	PROTOCOL::CAlarmTransportCacheRespond l_oTransCacheRespond;
	if (!l_oTransCacheRequest.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parsestring error:[%s]", l_strMessage.c_str());
		return;
	}
	l_oTransCacheRespond.m_oHeader.m_strCmd = ALARM_TRANSPORT_CACHE_RESPOND;
	l_oTransCacheRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oTransCacheRespond.m_oHeader.m_strRelatedID = l_oTransCacheRequest.m_oHeader.m_strMsgid;
	l_oTransCacheRespond.m_oHeader.m_strSystemID = CREATEUSER;
	DataBase::SQLRequest l_oSelectTransport;
	l_oSelectTransport.sql_id = SELECT_ICC_T_ALARM_TRANSPORT;
	l_oSelectTransport.param["source_seat_no"] = l_oTransCacheRequest.m_oBody.m_strSeatNo;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSelectTransport);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		l_oTransCacheRespond.m_oBody.m_strResult = ALARM_TRANSPORT_RESULT_STATE_FAIL;
	}
	else
	{
		while (l_pResult->Next())
		{
			PROTOCOL::CAlarmTransportCacheRespond::CData l_oData;
			l_oData.m_strSourceAlarmID = l_pResult->GetValue("source_alarm_id");
			l_oData.m_strSourceUserCode = l_pResult->GetValue("source_user_code");
			l_oData.m_strSourceSeatNo = l_pResult->GetValue("source_seat_no");
			l_oData.m_strTargetUserCode = l_pResult->GetValue("target_user_code");
			l_oData.m_strTargetSeatNo = l_pResult->GetValue("target_seat_no");
			l_oData.m_strTargetDeptCode = l_pResult->GetValue("target_dept_name");
			l_oData.m_strTargetAlarmID = l_pResult->GetValue("target_alarm_id");

			l_oTransCacheRespond.m_oBody.m_vData.push_back(l_oData);
		}

		l_oTransCacheRespond.m_oBody.m_strResult = ALARM_TRANSPORT_RESULT_STATE_SUCCESS;
	}
	l_oTransCacheRespond.m_oHeader.m_strResult = l_oTransCacheRespond.m_oBody.m_strResult;
	l_strMessage = l_oTransCacheRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendTransportAlarmRespond(eTransportResult p_eState, ObserverPattern::INotificationPtr p_pNotifiRequest, std::string p_strRelateID, const std::string& p_strMsg)
{
	PROTOCOL::CAlarmTransportRespond l_oAlarmTransportRespond;
	l_oAlarmTransportRespond.m_oHeader.m_strCmd = ALARM_TRANSPORT_RESQOND;
	l_oAlarmTransportRespond.m_oHeader.m_strMsgId = p_strRelateID;



	l_oAlarmTransportRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmTransportRespond.m_oHeader.m_strResult = m_pString->Number(p_eState);
	l_oAlarmTransportRespond.m_oBody.m_strResult = m_pString->Number(p_eState);
	l_oAlarmTransportRespond.m_oHeader.m_strRelatedID = p_strRelateID;
	l_oAlarmTransportRespond.m_oHeader.m_strMsg = p_strMsg;
	std::string l_strMessage = l_oAlarmTransportRespond.ToString(m_pJsonFty->CreateJson());
	
	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendTopicTransportInfo(std::string p_strAlarmID, std::string p_strTargetClient,const std::string &p_strTransportType)
{
	std::string l_strMessage;
	PROTOCOL::CTransPortAlarmSync l_oAlarmTransportSync;
	PROTOCOL::CAlarmTransferCallRespond l_oCallInfo;
	PROTOCOL::CTransPortAlarmRequest _oTransAlarmInfo;
	if (!GetAlarmInfoByAlarmID(p_strAlarmID, _oTransAlarmInfo) || !GetTransCallInfo(p_strAlarmID, l_oCallInfo))
	{
		return;
	}
	l_oAlarmTransportSync.m_oHeader.m_strCmd = ALARM_TRANSPORT_SYNC_CMD;
	l_oAlarmTransportSync.m_oHeader.m_strRequest = ALARM_TRANSPORT_TOAPIC_NAME;
	l_oAlarmTransportSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;
	l_oAlarmTransportSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();

	l_oAlarmTransportSync.m_oBody.m_strAlarmID = _oTransAlarmInfo.m_oBody.m_strAlarmID;
	l_oAlarmTransportSync.m_oBody.m_strCallRefID = l_oCallInfo.m_oBody.m_strNewCallRefId;
	l_oAlarmTransportSync.m_oBody.m_strUserCode = _oTransAlarmInfo.m_oBody.m_strUserCode;
	l_oAlarmTransportSync.m_oBody.m_strUserName = _oTransAlarmInfo.m_oBody.m_strUserName;
	l_oAlarmTransportSync.m_oBody.m_strDeptCode = _oTransAlarmInfo.m_oBody.m_strDeptCode;
	l_oAlarmTransportSync.m_oBody.m_strDeptName = _oTransAlarmInfo.m_oBody.m_strDeptName;
	l_oAlarmTransportSync.m_oBody.m_strSeatNo = _oTransAlarmInfo.m_oBody.m_strSeatNo;

	l_oAlarmTransportSync.m_oBody.m_strTargetDeptCode = _oTransAlarmInfo.m_oBody.m_strTargetDeptCode;
	l_oAlarmTransportSync.m_oBody.m_strTargetDeptName = _oTransAlarmInfo.m_oBody.m_strTargetDeptName;
	l_oAlarmTransportSync.m_oBody.m_strTargetSeatNo = p_strTargetClient;
	l_oAlarmTransportSync.m_oBody.m_strTransportReason = _oTransAlarmInfo.m_oBody.m_strTransportReason;
	l_oAlarmTransportSync.m_oBody.m_strTransportType = p_strTransportType;
	if (l_oAlarmTransportSync.m_oBody.m_strTransportType.empty())
	{
		l_oAlarmTransportSync.m_oBody.m_strTransportType = "0";
	}
	l_strMessage = l_oAlarmTransportSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendTransportAcceptAlarmRespond(eTransportResult p_eState, ObserverPattern::INotificationPtr p_pNotifiRequest, std::string p_strRelateID)
{
	PROTOCOL::CAlarmTransportAcceptRespond l_oAlarmTransportAccRespond;
	l_oAlarmTransportAccRespond.m_oHeader.m_strCmd = ALARM_TRANSPORT_ACCEPT_RESPOND_CMD;
	l_oAlarmTransportAccRespond.m_oHeader.m_strMsgId = p_strRelateID;


	l_oAlarmTransportAccRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmTransportAccRespond.m_oHeader.m_strRelatedID = p_strRelateID;
	l_oAlarmTransportAccRespond.m_oHeader.m_strResult = m_pString->Number(p_eState);
	l_oAlarmTransportAccRespond.m_oBody.m_strResult = m_pString->Number(p_eState);

	std::string l_strMessage = l_oAlarmTransportAccRespond.ToString(m_pJsonFty->CreateJson());

	p_pNotifiRequest->Response(l_strMessage);
}

void CBusinessImpl::SendTopicTransResultSync(std::string p_strAlarmID, eTransportResult p_estate, std::string p_strNewAlarmID, std::string p_strTransportType)
{
	if (p_strAlarmID.empty())
	{
		return;
	}
	PROTOCOL::CTransPortAcceptAlarmSync l_oAlarmTransportAcceptSync;

	PROTOCOL::CAlarmTransferCallRespond l_oCallInfo;
	PROTOCOL::CTransPortAlarmRequest l_oSourceInfo;
	if (!GetAlarmInfoByAlarmID(p_strAlarmID, l_oSourceInfo) || !GetTransCallInfo(p_strAlarmID, l_oCallInfo))
	{
		ICC_LOG_ERROR(m_pLog, "SourceInfo not exist, AlarmId: [%s]", p_strAlarmID.c_str());

		return;
	}

	l_oAlarmTransportAcceptSync.m_oHeader.m_strCmd = ALARM_TRANSPORT_ACCEPT_SYNC_CMD;
	l_oAlarmTransportAcceptSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmTransportAcceptSync.m_oHeader.m_strRelatedID = l_oSourceInfo.m_oHeader.m_strMsgid;
	l_oAlarmTransportAcceptSync.m_oHeader.m_strRequest = ALARM_TRANSPORT_TOAPIC_NAME;
	l_oAlarmTransportAcceptSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;

	l_oAlarmTransportAcceptSync.m_oBody.m_strUserCode = l_oSourceInfo.m_oBody.m_strTargetUserCode;
	l_oAlarmTransportAcceptSync.m_oBody.m_strUserName = l_oSourceInfo.m_oBody.m_strTargetUserName;
	l_oAlarmTransportAcceptSync.m_oBody.m_strDeptCode = l_oSourceInfo.m_oBody.m_strTargetDeptCode;
	l_oAlarmTransportAcceptSync.m_oBody.m_strDeptName = l_oSourceInfo.m_oBody.m_strTargetDeptName;
	l_oAlarmTransportAcceptSync.m_oBody.m_strSeatNo = l_oSourceInfo.m_oBody.m_strTargetSeatNo;
	if (!l_oSourceInfo.m_oBody.m_strCallRefID.empty())
	{
		if (!l_oCallInfo.m_oBody.m_strTarget.empty())
		{
			l_oAlarmTransportAcceptSync.m_oBody.m_strSeatNo = l_oCallInfo.m_oBody.m_strTarget;
		}
	}

	l_oAlarmTransportAcceptSync.m_oBody.m_strSoureAlarmID = l_oSourceInfo.m_oBody.m_strAlarmID;
	if (p_estate == TransportRefuse)
	{
		//拒绝时，p_strNewAlarmID，参数所传是拒绝原因
		l_oAlarmTransportAcceptSync.m_oBody.m_strRefuseReason = p_strNewAlarmID;
	}
	else
	{
		l_oAlarmTransportAcceptSync.m_oBody.m_strNewAlarmID = p_strNewAlarmID;
		if (!UpdateCarInfo(l_oSourceInfo.m_oBody.m_strAlarmID, p_strNewAlarmID))
		{
			ICC_LOG_DEBUG(m_pLog, "update car failed!");
		}
	}
	l_oAlarmTransportAcceptSync.m_oBody.m_strCallref = l_oCallInfo.m_oBody.m_strNewCallRefId;
	l_oAlarmTransportAcceptSync.m_oBody.m_strResult = m_pString->Number(p_estate);
	l_oAlarmTransportAcceptSync.m_oBody.m_strTransportType = p_strTransportType;

	std::string l_strMessage = l_oAlarmTransportAcceptSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMessage.c_str());
	// 将失败的转警记录从数据库中删除 [5/15/2018 w26326]
	if (p_estate != TransportSuccess)
	{
		DeleteTransportTable(p_strAlarmID);
	}

	DeleteTransportQue(p_strAlarmID);
}

void CBusinessImpl::SendTransferCallRequest(std::string p_strAlarmID)
{
	PROTOCOL::CSendTransferCallRequest l_oRequest;
	l_oRequest.m_oHeader.m_strCmd = ALARM_TRANSPORT_TRANSFERCALL_REQUEST;
	l_oRequest.m_oHeader.m_strRequest = ALARM_TRANSPORT_CTI_QUEUE;
	l_oRequest.m_oHeader.m_strRequestType = MQTYPE_QUEUE;
	std::string l_strRelateID = m_pString->CreateGuid();
	l_oRequest.m_oHeader.m_strMsgid = l_strRelateID;

	PROTOCOL::CTransPortAlarmRequest l_oSourceInfo;
	if (!GetAlarmInfoByAlarmID(p_strAlarmID, l_oSourceInfo))
	{
		return;
	}
	l_oRequest.m_oBody.m_strHeldCallrefID = l_oSourceInfo.m_oBody.m_strCallRefID;
	AddRelateID(l_oSourceInfo.m_oBody.m_strCallRefID, p_strAlarmID);
	l_oRequest.m_oBody.m_strSponsor = l_oSourceInfo.m_oBody.m_strSeatNo;

	if (l_oSourceInfo.m_oBody.m_strAssignFlag == ASSIGNED)
	{
		l_oRequest.m_oBody.m_strTarget = l_oSourceInfo.m_oBody.m_strTargetSeatNo;
		l_oRequest.m_oBody.m_strIsAcd = TRANSFER_CALL_TYPE_ASSIGN;
	}
	else
	{
		// 转部门电话、获取查询部门VDN分组 [8/3/2018 w26326]
		std::string l_strVDN;
		if (m_pRedisClient->HGet(MANGE_TRANSPORT_BINDMEMBER, l_oSourceInfo.m_oBody.m_strTargetDeptCode, l_strVDN) && !l_strVDN.empty())
		{
			l_oRequest.m_oBody.m_strTarget = l_strVDN;
		}
		else
		{
			l_oRequest.m_oBody.m_strTarget = m_strDefaultVDN;
		}
		l_oRequest.m_oBody.m_strIsAcd = TRANSFER_CALL_TYPE_VDN;
	}
	l_oRequest.m_oBody.m_strTargetDeviceType = DEVICE_TYPE;
	std::string l_strMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SendReCallRequest(std::string p_strAlarmID)
{
	PROTOCOL::CancelTransferRequest l_oRequest;
	l_oRequest.m_oHeader.m_strCmd = ALARM_TRANSPORT_RECONNECT_REQUEST;
	l_oRequest.m_oHeader.m_strRequest = ALARM_TRANSPORT_CTI_QUEUE;
	l_oRequest.m_oHeader.m_strRequestType = MQTYPE_QUEUE;

	std::string l_strRelateID = m_pString->CreateGuid();
	l_oRequest.m_oHeader.m_strMsgid = l_strRelateID;
	AddRelateID(l_strRelateID, p_strAlarmID);

	PROTOCOL::CAlarmTransferCallRespond l_oTransCallInfo;
	if (!GetTransCallInfo(p_strAlarmID, l_oTransCallInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get Alarm info is not exist");
		return;
	}
	l_oRequest.m_oBody.m_strHeldCallrefID = l_oTransCallInfo.m_oBody.m_strCallRefId;
	l_oRequest.m_oBody.m_strSponsor = l_oTransCallInfo.m_oBody.m_strSponsor;
	l_oRequest.m_oBody.m_strDevice = l_oTransCallInfo.m_oBody.m_strTarget;

	std::string l_strMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_INFO(m_pLog, "send Msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::_AssignReceipterInfo(const std::string& strSeatNo, std::string& strStaffCode, std::string& strStaffName)
{
	ICC_LOG_DEBUG(m_pLog, "get receipt info by seatno, seatno: %s.", strSeatNo.c_str());

	do 
	{
		std::string tmp_strRedisValue;
		m_pRedisClient->HGet(CLIENT_REGISTER_INFO, strSeatNo, tmp_strRedisValue);

		//没有登陆的数据，返回
		if (tmp_strRedisValue.empty())
		{
			ICC_LOG_ERROR(m_pLog, "login info is empty! seatno:%s", strSeatNo.c_str());
			break;
		}

		JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
		if (nullptr == pJson)
		{
			ICC_LOG_ERROR(m_pLog, "get login info create json failed! seatno:%s", strSeatNo.c_str());
			break;
		}

		if (!pJson->LoadJson(tmp_strRedisValue))
		{
			ICC_LOG_ERROR(m_pLog, "load login info failed! seatno:%s, info:%s", strSeatNo.c_str(), tmp_strRedisValue.c_str());
			break;
		}		

		strStaffCode = pJson->GetNodeValue("/staff_code", "");
		strStaffName = pJson->GetNodeValue("/staff_name", "");

		ICC_LOG_DEBUG(m_pLog, "get receipt info by seatno, seatno: %s, receipt_code:%s, receipt_name:%s.", strSeatNo.c_str(), strStaffCode.c_str(), strStaffName.c_str());

	} while (false);
	
}

void CBusinessImpl::SendTransNewAlarmIDRequest(std::string p_strAlarmID)
{
	PROTOCOL::CAddAlarm l_oRequest;
	l_oRequest.m_oHeader.m_strCmd = ALARM_TRANSPORT_ADDALARM_REQUEST;
	l_oRequest.m_oHeader.m_strRequest = ALARM_TRANSPORT_ALARM_QUEUE;
	l_oRequest.m_oHeader.m_strRequestType = MQTYPE_QUEUE;
	l_oRequest.m_oHeader.m_strSubsystemID = "ICC_SERVER_TRANSPORT";

	std::string l_strRelateID = m_pString->CreateGuid();
	l_oRequest.m_oHeader.m_strMsgid = l_strRelateID;
	AddRelateID(l_strRelateID, p_strAlarmID);

	PROTOCOL::CTransPortAlarmRequest l_oSourceInfo;
	PROTOCOL::CAlarmTransferCallRespond l_oCallInfo;
	if (!GetAlarmInfoByAlarmID(p_strAlarmID, l_oSourceInfo) || !GetTransCallInfo(p_strAlarmID, l_oCallInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get Alarm info is not exist");
		return;
	}

	l_oRequest.m_oBody.m_alarm = l_oSourceInfo.m_oBody.m_oAlarm;
	if (!l_oSourceInfo.m_oBody.m_strCallRefID.empty())
	{
		l_oRequest.m_oBody.m_alarm.m_strSeatNo = l_oCallInfo.m_oBody.m_strTarget;
	}
	else
	{
		l_oRequest.m_oBody.m_alarm.m_strSeatNo = l_oSourceInfo.m_oBody.m_strTargetSeatNo;
	}

	l_oRequest.m_oBody.m_alarm.m_strReceiptDeptCode = l_oSourceInfo.m_oBody.m_strTargetDeptCode;
	l_oRequest.m_oBody.m_alarm.m_strReceiptDeptName = l_oSourceInfo.m_oBody.m_strTargetDeptName;	
	l_oRequest.m_oBody.m_alarm.m_strReceiptDeptOrgCode = l_oSourceInfo.m_oBody.m_strTargetDeptOrgCode;

	if (l_oSourceInfo.m_oBody.m_strCallRefID.empty() && !l_oSourceInfo.m_oBody.m_oAlarm.m_strSourceID.empty())
	{
		l_oRequest.m_oBody.m_strTransAlarm = "1";
	}
	else
	{
		l_oRequest.m_oBody.m_alarm.m_strSourceID = l_oCallInfo.m_oBody.m_strNewCallRefId;
	}

	if (l_oSourceInfo.m_oBody.m_strTargetUserCode.empty() || l_oSourceInfo.m_oBody.m_strTargetUserName.empty())
	{
		_AssignReceipterInfo(l_oRequest.m_oBody.m_alarm.m_strSeatNo, l_oSourceInfo.m_oBody.m_strTargetUserCode, l_oSourceInfo.m_oBody.m_strTargetUserName);
	}
	
	l_oRequest.m_oBody.m_alarm.m_strReceiptName = l_oSourceInfo.m_oBody.m_strTargetUserName;
	l_oRequest.m_oBody.m_alarm.m_strReceiptCode = l_oSourceInfo.m_oBody.m_strTargetUserCode;
	l_oRequest.m_oBody.m_alarm.m_strCreateUser = l_oSourceInfo.m_oBody.m_strTargetUserName;

	//l_oRequest.m_oBody.m_alarm.m_strMergeID = l_oSourceInfo.m_oBody.m_oAlarm.m_strMergeID;
	//l_oRequest.m_oBody.m_alarm.m_strTitle = l_oSourceInfo.m_oBody.m_oAlarm.m_strTitle;
	//l_oRequest.m_oBody.m_alarm.m_strContent = l_oSourceInfo.m_oBody.m_oAlarm.m_strContent;
	//l_oRequest.m_oBody.m_alarm.m_strTime = l_oSourceInfo.m_oBody.m_oAlarm.m_strTime;
	//l_oRequest.m_oBody.m_alarm.m_strActualOccurTime = l_oSourceInfo.m_oBody.m_oAlarm.m_strActualOccurTime;
	//l_oRequest.m_oBody.m_alarm.m_strAddr = l_oSourceInfo.m_oBody.m_oAlarm.m_strAddr;
	//l_oRequest.m_oBody.m_alarm.m_strLongitude = l_oSourceInfo.m_oBody.m_oAlarm.m_strLongitude;
	//l_oRequest.m_oBody.m_alarm.m_strLatitude = l_oSourceInfo.m_oBody.m_oAlarm.m_strLatitude;
	//l_oRequest.m_oBody.m_alarm.m_strState = l_oSourceInfo.m_oBody.m_oAlarm.m_strState;
	//l_oRequest.m_oBody.m_alarm.m_strLevel = l_oSourceInfo.m_oBody.m_oAlarm.m_strLevel;
	//l_oRequest.m_oBody.m_alarm.m_strSourceType = l_oSourceInfo.m_oBody.m_oAlarm.m_strSourceType;
	//l_oRequest.m_oBody.m_alarm.m_strSourceID = l_oCallInfo.m_oBody.m_strNewCallRefId;
	//l_oRequest.m_oBody.m_alarm.m_strHandleType = l_oSourceInfo.m_oBody.m_oAlarm.m_strHandleType;
	//l_oRequest.m_oBody.m_alarm.m_strFirstType = l_oSourceInfo.m_oBody.m_oAlarm.m_strFirstType;
	//l_oRequest.m_oBody.m_alarm.m_strSecondType = l_oSourceInfo.m_oBody.m_oAlarm.m_strSecondType;
	//l_oRequest.m_oBody.m_alarm.m_strThirdType = l_oSourceInfo.m_oBody.m_oAlarm.m_strThirdType;
	//l_oRequest.m_oBody.m_alarm.m_strFourthType = l_oSourceInfo.m_oBody.m_oAlarm.m_strFourthType;
	//l_oRequest.m_oBody.m_alarm.m_strVehicleNo = l_oSourceInfo.m_oBody.m_oAlarm.m_strVehicleNo;
	//l_oRequest.m_oBody.m_alarm.m_strVehicleType = l_oSourceInfo.m_oBody.m_oAlarm.m_strVehicleType;
	//l_oRequest.m_oBody.m_alarm.m_strSymbolCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strSymbolCode;
	//l_oRequest.m_oBody.m_alarm.m_strSymbolAddr = l_oSourceInfo.m_oBody.m_oAlarm.m_strSymbolAddr;
	//l_oRequest.m_oBody.m_alarm.m_strFireBuildingType = l_oSourceInfo.m_oBody.m_oAlarm.m_strFireBuildingType;
	//l_oRequest.m_oBody.m_alarm.m_strEventType = l_oSourceInfo.m_oBody.m_oAlarm.m_strEventType;
	//l_oRequest.m_oBody.m_alarm.m_strCalledNoType = l_oSourceInfo.m_oBody.m_oAlarm.m_strCalledNoType;
	//l_oRequest.m_oBody.m_alarm.m_strActualCalledNoType = l_oSourceInfo.m_oBody.m_oAlarm.m_strActualCalledNoType;
	//l_oRequest.m_oBody.m_alarm.m_strCallerNo = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerNo;
	//l_oRequest.m_oBody.m_alarm.m_strCallerName = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerName;
	//l_oRequest.m_oBody.m_alarm.m_strCallerAddr = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerAddr;
	//l_oRequest.m_oBody.m_alarm.m_strCallerID = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerID;
	//l_oRequest.m_oBody.m_alarm.m_strCallerIDType = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerIDType;
	//l_oRequest.m_oBody.m_alarm.m_strCallerGender = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerGender;
	//l_oRequest.m_oBody.m_alarm.m_strCallerAge = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerAge;
	//l_oRequest.m_oBody.m_alarm.m_strCallerBirthday = l_oSourceInfo.m_oBody.m_oAlarm.m_strCallerBirthday;
	//l_oRequest.m_oBody.m_alarm.m_strContactNo = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactNo;
	//l_oRequest.m_oBody.m_alarm.m_strContactName = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactName;
	//l_oRequest.m_oBody.m_alarm.m_strContactAddr = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactAddr;
	//l_oRequest.m_oBody.m_alarm.m_strContactID = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactID;
	//l_oRequest.m_oBody.m_alarm.m_strContactIDType = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactIDType;
	//l_oRequest.m_oBody.m_alarm.m_strContactGender = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactGender;
	//l_oRequest.m_oBody.m_alarm.m_strContactAge = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactAge;
	//l_oRequest.m_oBody.m_alarm.m_strContactBirthday = l_oSourceInfo.m_oBody.m_oAlarm.m_strContactBirthday;
	//l_oRequest.m_oBody.m_alarm.m_strAdminDeptDistrictCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode;
	//l_oRequest.m_oBody.m_alarm.m_strAdminDeptCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strAdminDeptCode;
	//l_oRequest.m_oBody.m_alarm.m_strAdminDeptName = l_oSourceInfo.m_oBody.m_oAlarm.m_strAdminDeptName;
	//l_oRequest.m_oBody.m_alarm.m_strReceiptDeptDistrictCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;
	//l_oRequest.m_oBody.m_alarm.m_strReceiptDeptCode = l_oSourceInfo.m_oBody.m_strTargetDeptCode;
	//l_oRequest.m_oBody.m_alarm.m_strReceiptDeptName = l_oSourceInfo.m_oBody.m_strTargetDeptName;
	//l_oRequest.m_oBody.m_alarm.m_strLeaderCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strLeaderCode;
	//l_oRequest.m_oBody.m_alarm.m_strLeaderName = l_oSourceInfo.m_oBody.m_oAlarm.m_strLeaderName;
	//l_oRequest.m_oBody.m_alarm.m_strReceiptName = l_oSourceInfo.m_oBody.m_strTargetUserName;
	//l_oRequest.m_oBody.m_alarm.m_strReceiptCode = l_oSourceInfo.m_oBody.m_strTargetUserCode;
	//l_oRequest.m_oBody.m_alarm.m_strIsMerg = l_oSourceInfo.m_oBody.m_oAlarm.m_strIsMerge;
	//l_oRequest.m_oBody.m_alarm.m_strCityCode = l_oSourceInfo.m_oBody.m_oAlarm.m_strCityCode;
	//l_oRequest.m_oBody.m_alarm.m_strDispatchSuggestion = l_oSourceInfo.m_oBody.m_oAlarm.m_strDispatchSuggestion;
	//l_oRequest.m_oBody.m_alarm.m_strCreateUser = l_oSourceInfo.m_oBody.m_strTargetUserName;
	//l_oRequest.m_oBody.m_alarm.m_strTime = l_oSourceInfo.m_oBody.m_oAlarm.m_strTime;//m_pDateTime->CurrentDateTimeStr();
	//l_oRequest.m_oBody.m_alarm.m_strPrivacy = l_oSourceInfo.m_oBody.m_oAlarm.m_strPrivacy;
	//l_oRequest.m_oBody.m_alarm.m_strRemark = l_oSourceInfo.m_oBody.m_oAlarm.m_strRemark;

	std::string l_strMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_INFO(m_pLog, "transport alarm, add new alarm, send Msg[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnTransferCallSync(ObserverPattern::INotificationPtr p_oTransferCallResp)
{
	ICC_LOG_DEBUG(m_pLog, " receive message[%s]", p_oTransferCallResp->GetMessages().c_str());
	std::string l_strMsg = p_oTransferCallResp->GetMessages();
	PROTOCOL::CAlarmTransferCallRespond l_oTransferCallResp;
	if (!l_oTransferCallResp.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parsestring error:[%s]", l_strMsg.c_str());
	}

	std::string l_strAlarmID;
	PROTOCOL::CTransPortAlarmRequest l_oInfo;
	if (!GetAlarmIDByRelateID(l_oTransferCallResp.m_oBody.m_strCallRefId, l_strAlarmID))
	{
		ICC_LOG_DEBUG(m_pLog, "invalid mag:[%s]", l_strMsg.c_str());
		return;
	}
	if (!GetAlarmInfoByAlarmID(l_strAlarmID, l_oInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "GetAlarmInfoByAlarmId failed");
		return;
	}
	std::string l_strTransportType = l_oInfo.m_oBody.m_strTransportType;

	unsigned int l_iResultID = m_pString->ToUInt(l_oTransferCallResp.m_oBody.m_strResult);
	switch (l_iResultID)
	{
	case TransferCallSuccess:
	{
		// 转警成功建立通话 [7/6/2018 w26326]
		std::string strSync;
		if (GetAlarmIDByRelateID("Send_NewAlarm_True" + l_strAlarmID, strSync))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		SetTransCallInfo(l_oTransferCallResp, l_strAlarmID);
		SendTransNewAlarmIDRequest(l_strAlarmID);
		// 解决多服务订阅同一主题、重复消息问题 [7/12/2018 w26326]
		AddRelateID("Send_NewAlarm_True" + l_strAlarmID, l_strAlarmID);
		ICC_LOG_DEBUG(m_pLog, "transfercall success msg:[%s]", l_strMsg.c_str());
		break;
	}
	case TransferCall_RingStae:
	{
		// 被转单位电话振铃、发送同步信息到客户端 [7/6/2018 w26326]
		std::string strSync;
		if (GetAlarmIDByRelateID("Send_topic_True" + l_strAlarmID, strSync))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		SetTransCallInfo(l_oTransferCallResp, l_strAlarmID);
		SendTopicTransportInfo(l_strAlarmID, l_oTransferCallResp.m_oBody.m_strTarget, l_strTransportType);

		// 解决多服务订阅同一主题、重复消息问题 [7/12/2018 w26326]
		AddRelateID("Send_topic_True" + l_strAlarmID, l_strAlarmID);
		ICC_LOG_DEBUG(m_pLog, "transfercall ringstate msg:[%s]", l_strMsg.c_str());
		break;
	}
	case TransferCall_NoCallrefID:
	{
		// 带已经挂机话务转警、只转警情，走不带话务转警流程 [7/19/2018 w26326]
		std::string strSync;
		if (GetAlarmIDByRelateID("Send_Sync_True" + l_strAlarmID, strSync))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		if (l_oInfo.m_oBody.m_strAssignFlag == ASSIGNED)
		{
			if (!IsFreeSeatNo(l_oInfo.m_oBody.m_strTargetDeptCode, l_oInfo.m_oBody.m_strTargetSeatNo))
			{
				SendTopicTransResultSync(l_oInfo.m_oBody.m_strAlarmID, TransportFail, "");
			}
			else
			{
				SendTopicTransportInfo(l_oInfo.m_oBody.m_strAlarmID, l_oInfo.m_oBody.m_strTargetSeatNo, l_strTransportType);
			}
		}
		else
		{
			std::string strSeatNo = GetFreeSeatNo(l_oInfo.m_oBody.m_strTargetDeptCode, l_oInfo.m_oBody.m_strSeatNo);
			if (strSeatNo.empty())
			{
				SendTopicTransResultSync(l_oInfo.m_oBody.m_strAlarmID, TransportFail, "");
			}
			SendTopicTransportInfo(l_oInfo.m_oBody.m_strAlarmID, strSeatNo, l_strTransportType);
			SetTraget(strSeatNo, l_oInfo.m_oBody.m_strAlarmID);
		}
		// 解决多服务订阅同一主题、重复消息问题 [7/12/2018 w26326]
		AddRelateID("Send_Sync_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_Busy:
	{
		// 带已经挂机话务转警、只转警情，走不带话务转警流程 [7/19/2018 w26326]
		std::string strBusy;
		if (GetAlarmIDByRelateID("Send_Busy_True" + l_strAlarmID, strBusy))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}

		// 无空闲席位，转电话失败 [7/6/2018 w26326]
		std::vector<std::string> l_vecParamList;
		Data::CStaffInfo l_oStaffInfo;
		std::string strStaffCode = _GetStaffCodeBySeat(l_oInfo.m_oBody.m_strSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oInfo.m_oBody.m_strUserName);
		l_vecParamList.push_back(strTransportName);
		l_vecParamList.push_back(l_oInfo.m_oBody.m_strSeatNo);
		l_vecParamList.push_back(l_oInfo.m_oBody.m_strDeptName);

		strStaffCode = _GetStaffCodeBySeat(l_oInfo.m_oBody.m_strTargetSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oStaffInfo.m_strName);
		l_vecParamList.push_back(strTargetName);
		l_vecParamList.push_back(l_oInfo.m_oBody.m_strTargetSeatNo);
		l_vecParamList.push_back(l_oInfo.m_oBody.m_strTargetDeptName);
		EditTransportAlarmLog(l_vecParamList, l_oInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_FAIL_TRANSCALLFAIL);

		// 发起重拾电话 [6/28/2018 w26326]
		SendReCallRequest(l_strAlarmID);
		SendTopicTransResultSync(l_strAlarmID, TransportFail, "");
		ICC_LOG_DEBUG(m_pLog, "transfercall failed: busy msg:[%s]", l_strMsg.c_str());
		// 解决多服务订阅同一主题、重复消息问题 [7/12/2018 w26326]
		AddRelateID("Send_Busy_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_Timeout:
	{
		std::string l_strFail;
		if (GetAlarmIDByRelateID("Send_Fail_True" + l_strAlarmID, l_strFail))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		SendTopicTransResultSync(l_strAlarmID, TransportAcceptTimeOut, "");
		ICC_LOG_DEBUG(m_pLog, "transfercall failed: timeout msg:[%s]", l_strMsg.c_str());
		AddRelateID("Send_Fail_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_HangUp:
	{
		std::string l_strFail;
		if (GetAlarmIDByRelateID("Send_Fail_True" + l_strAlarmID, l_strFail))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		SendTopicTransResultSync(l_strAlarmID, TransportHangUp, "");
		ICC_LOG_DEBUG(m_pLog, "transfercall failed: hang up msg:[%s]", l_strMsg.c_str());
		AddRelateID("Send_Fail_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_Cancel:
	{
		std::string l_strFail;
		if (GetAlarmIDByRelateID("Send_Fail_True" + l_strAlarmID, l_strFail))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "transfercall reconnect failed msg:[%s]", l_strMsg.c_str());
		AddRelateID("Send_Fail_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_HangUpByOwner:
	{
		std::string l_strFail;
		if (GetAlarmIDByRelateID("Send_Fail_True" + l_strAlarmID, l_strFail))
		{
			ICC_LOG_DEBUG(m_pLog, "repeat transfercall sync:[%s]", l_strMsg.c_str());
			return;
		}
		SendTopicTransResultSync(l_strAlarmID, TransportHangUp, "");
		ICC_LOG_DEBUG(m_pLog, "transfercall failed: hang up by transfer msg:[%s]", l_strMsg.c_str());
		AddRelateID("Send_Fail_True" + l_strAlarmID, l_strAlarmID);
		break;
	}
	case TransferCallFail_UnIdentify:
	{
		ICC_LOG_DEBUG(m_pLog, "un identify msg:[%s]", l_strMsg.c_str());
		break;
	}
	default:
		ICC_LOG_DEBUG(m_pLog, "unknown result id:[%u]", l_iResultID);
		break;
	}
}

void CBusinessImpl::OnTransNewAlarmIDResponse(ObserverPattern::INotificationPtr p_oNewAlarmIDRespond)
{
	ICC_LOG_DEBUG(m_pLog, " receive message[%s]", p_oNewAlarmIDRespond->GetMessages().c_str());
	std::string l_strMsg = p_oNewAlarmIDRespond->GetMessages();
	PROTOCOL::CGetNewAlarmIDRespond l_oNewAlarmIDInfo;
	if (!l_oNewAlarmIDInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse string error:[%s]", l_strMsg.c_str());
	}
	std::string l_strAlarmID;
	if (!GetAlarmIDByRelateID(l_oNewAlarmIDInfo.m_oHeader.m_strRelatedID, l_strAlarmID))
	{
		ICC_LOG_ERROR(m_pLog, "invalid msg:[%s]", l_strMsg.c_str());
		return;
	}

	std::string l_strTransportType = TRANSPORT_TYPE_NORMAL;
	PROTOCOL::CTransPortAlarmRequest l_oSourceInfo;
	if (!GetAlarmInfoByAlarmIDEx(l_strAlarmID, l_oSourceInfo, l_strTransportType))
	{
		ICC_LOG_ERROR(m_pLog, "get Alarm info is not exist");
		return;
	}
	std::string l_strNewAlarmID = l_oNewAlarmIDInfo.m_oBody.m_strAlarmID;

	std::vector<std::string> l_vecParamList;
	if (l_strTransportType.compare(TRANSPORT_TYPE_TAKEOVER) == 0)
	{
		std::string l_strCallInfo = "";
		std::string l_strCTICallRefId = l_oSourceInfo.m_oBody.m_strCallRefID;
		if (m_pRedisClient->HGet(CALL_INFO, l_strCTICallRefId, l_strCallInfo))
		{
			PROTOCOL::CCallInfo l_oCallInfo;
			if (!l_oCallInfo.ParseString(l_strCallInfo, m_pJsonFty->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "Invalid callinfo protocol: [%s]", l_strCallInfo.c_str());

				return;
			}

			//<Resource Key = "BS001009004" Value = "[席位号-{0}]接管话务[主叫-{1}, 被叫-{2}, 呼入时间-{3}]" / >
			//	话务接管流水内容
			l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
			l_vecParamList.push_back(l_oCallInfo.m_strCallerId);
			l_vecParamList.push_back(l_oCallInfo.m_strCalledId);
			l_vecParamList.push_back(l_oCallInfo.m_strTime);
			EditTakeOverCallLog(l_vecParamList, l_strNewAlarmID, l_strCTICallRefId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Redis not exist callinfo: [%s]", l_strCallInfo.c_str());
		}

		/*<Resource Key = "BS001005010" Value = "{0}[席位号-{1}, 单位-{2}]接管{3}[席位号-{4}, 单位-{5}]的警单[{6}], 新警单[{7}]" / >*/
		l_vecParamList.clear();

		Data::CStaffInfo l_oStaffInfo;
		std::string strStaffCode = _GetStaffCodeBySeat(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oStaffInfo.m_strName);
		l_vecParamList.push_back(strTransportName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetDeptName);

		strStaffCode = _GetStaffCodeBySeat(l_oSourceInfo.m_oBody.m_strSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oSourceInfo.m_oBody.m_strUserName);
		l_vecParamList.push_back(strTargetName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strSeatNo);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strDeptName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strAlarmID);
		l_vecParamList.push_back(l_strNewAlarmID);
		EditTransportAlarmLog(l_vecParamList, l_oSourceInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_TAKEOVER_DEST_EX);

		/*<Resource Key = "BS001005011" Value = "{0}[席位号-{1}, 单位-{2}]接管此警单，新警单号[{3}]" / >*/  //新警单复制旧警单的流水
		/*l_vecParamList.clear();
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetUserName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetDeptName);
		l_vecParamList.push_back(l_strNewAlarmID);
		EditTransportAlarmLog(l_vecParamList, l_oSourceInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_TAKEOVER_SRC);*/
	} 
	else
	{
		// {0}[席位号-{1}, 单位-{2}]转移警单{3}到{4}[席位号-{5}, 单位-{6}]{7} [5/2/2018 w26326]
		Data::CStaffInfo l_oStaffInfo;
		std::string strStaffCode = _GetStaffCodeBySeat(l_oSourceInfo.m_oBody.m_strSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oSourceInfo.m_oBody.m_strUserName);
		l_vecParamList.push_back(strTransportName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strSeatNo);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strDeptName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strAlarmID);

		strStaffCode = _GetStaffCodeBySeat(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
		if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
		}
		std::string strTargetName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oStaffInfo.m_strName);
		l_vecParamList.push_back(strTargetName);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetSeatNo);
		l_vecParamList.push_back(l_oSourceInfo.m_oBody.m_strTargetDeptName);
		l_vecParamList.push_back(l_strNewAlarmID);
	//	EditTransportAlarmLog(l_vecParamList, l_strNewAlarmID, LOG_TRANSPORT_ALARM_SUCCESS); //新警单复制旧警单的流水
		EditTransportAlarmLog(l_vecParamList, l_oSourceInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_SUCCESS);
	}
	std::string l_strNewCallrefID;
	CopyAlarmLog(l_strNewAlarmID, l_strAlarmID);
	UpdateTransportTable(l_strAlarmID, l_strNewAlarmID, l_strNewCallrefID);
	UpdateAlarm(l_strAlarmID, l_strNewAlarmID, l_oSourceInfo, l_oSourceInfo.m_oBody.m_strCallRefID, l_strNewCallrefID);
	SendTopicTransResultSync(l_strAlarmID, TransportSuccess, l_strNewAlarmID, l_strTransportType);
}

bool CBusinessImpl::AddAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody &p_pAlarmLogInfo)
{
	PROTOCOL::CAlarmLogSync::CBody l_oData;
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	std::string l_strGUID(m_pString->CreateGuid());

	if (p_pAlarmLogInfo.m_strID.empty())
	{
		l_oData.m_strID = l_strGUID;
	}
	else
	{
		l_oData.m_strID = p_pAlarmLogInfo.m_strID;
	}
	l_oData.m_strAlarmID = p_pAlarmLogInfo.m_strAlarmID;
	l_oData.m_strProcessID = p_pAlarmLogInfo.m_strProcessID;
	l_oData.m_strFeedbackID = p_pAlarmLogInfo.m_strFeedbackID;
	l_oData.m_strOperate = p_pAlarmLogInfo.m_strOperate;
	l_oData.m_strOperateContent = p_pAlarmLogInfo.m_strOperateContent;
	l_oData.m_strFromType = p_pAlarmLogInfo.m_strFromType;
	l_oData.m_strFromObject = p_pAlarmLogInfo.m_strFromObject;
	l_oData.m_strFromObjectName = p_pAlarmLogInfo.m_strFromObjectName;
	l_oData.m_strFromObjectOrgName = p_pAlarmLogInfo.m_strFromObjectOrgName;
	l_oData.m_strFromObjectOrgCode = p_pAlarmLogInfo.m_strFromObjectOrgCode;
	l_oData.m_strFromOrgIdentifier = p_pAlarmLogInfo.m_strFromOrgIdentifier;
	l_oData.m_strToType = p_pAlarmLogInfo.m_strToType;
	l_oData.m_strToObject = p_pAlarmLogInfo.m_strToObject;
	l_oData.m_strToObjectName = p_pAlarmLogInfo.m_strToObjectName;
	l_oData.m_strToObjectOrgName = p_pAlarmLogInfo.m_strToObjectOrgName;
	l_oData.m_strToObjectOrgCode = p_pAlarmLogInfo.m_strToObjectOrgCode;
	l_oData.m_strToObjectOrgIdentifier = p_pAlarmLogInfo.m_strToObjectOrgIdentifier;
	l_oData.m_strCreateUser = p_pAlarmLogInfo.m_strCreateUser;
	l_oData.m_strSourceName = p_pAlarmLogInfo.m_strSourceName;
	l_oData.m_strOperateAttachDesc = p_pAlarmLogInfo.m_strOperateAttachDesc;
	l_oData.m_strCreateTime = l_strCurTime;

	l_oData.m_strSeatNo = p_pAlarmLogInfo.m_strSeatNo;
	l_oData.m_strDeptOrgCode = p_pAlarmLogInfo.m_strDeptOrgCode;
	if (!InsertDBAlarmLogInfo(l_oData))
	{
		return false;
	}

	SyncAlarmLogInfo(l_oData);
	return true;
}

bool CBusinessImpl::InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo)
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


	if (PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		std::string l_strUser = p_AlarmLogInfo.m_strCreateUser;
		std::string l_strTime = p_AlarmLogInfo.m_strCreateTime;
		if (l_strUser.empty())
		{
			l_tSQLReqInsertAlarm.param["create_user"] = CREATEUSER;
		}
		else
		{
			l_tSQLReqInsertAlarm.param["create_user"] = l_strUser;
		}
		if (l_strTime.empty())
		{
			DateTime::CDateTime l_iCurrent = m_pDateTime->CurrentDateTime();
			l_tSQLReqInsertAlarm.param["create_time"] = m_pDateTime->ToString(l_iCurrent, DateTime::DEFAULT_DATETIME_STRING_FORMAT);
		}
		else
		{
			l_tSQLReqInsertAlarm.param["create_time"] = l_strTime;
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "set insert alarm log data failed");
		return false;
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	m_pHelpTool->Sleep(1);
	return true;
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	//std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;

	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = ALARM_LOG_SYNC_TOPIC_NAME;
	l_oAlarmLogSync.m_oHeader.m_strRequest = ALARM_TRANSPORT_TOAPIC_NAME;
	l_oAlarmLogSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;

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

bool CBusinessImpl::InsertTransportTable(const PROTOCOL::CTransPortAlarmRequest &p_oTransAlarmRequest)
{
	DataBase::SQLRequest l_oInsertTransport;
	l_oInsertTransport.sql_id = INSERT_ICC_T_ALARM_TRANSPORT;

	l_oInsertTransport.param["source_alarm_id"] = p_oTransAlarmRequest.m_oBody.m_strAlarmID;
	l_oInsertTransport.param["source_callref_id"] = p_oTransAlarmRequest.m_oBody.m_strCallRefID;
	l_oInsertTransport.param["source_user_code"] = p_oTransAlarmRequest.m_oBody.m_strUserCode;
	l_oInsertTransport.param["source_user_name"] = p_oTransAlarmRequest.m_oBody.m_strUserName;
	l_oInsertTransport.param["source_seat_no"] = p_oTransAlarmRequest.m_oBody.m_strSeatNo;
	l_oInsertTransport.param["source_dept_code"] = p_oTransAlarmRequest.m_oBody.m_strDeptCode;
	l_oInsertTransport.param["source_dept_name"] = p_oTransAlarmRequest.m_oBody.m_strDeptName;
	l_oInsertTransport.param["target_dept_code"] = p_oTransAlarmRequest.m_oBody.m_strTargetDeptCode;
	l_oInsertTransport.param["target_dept_name"] = p_oTransAlarmRequest.m_oBody.m_strTargetDeptName;

	l_oInsertTransport.param["create_user"] = CREATEUSER;
	l_oInsertTransport.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

	l_oInsertTransport.param["guid"] = m_pString->CreateGuid();
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oInsertTransport);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Error:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::DeleteTransportTable(std::string p_strAlarmID)
{
	if (p_strAlarmID.empty())
	{
		return false;
	}
	DataBase::SQLRequest l_oDeleteTransport;
	l_oDeleteTransport.sql_id = DELETE_ICC_T_ALARM_TRANSPORT;
	l_oDeleteTransport.param["source_alarm_id"] = p_strAlarmID;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oDeleteTransport);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "DB Error:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateTransportTable(const PROTOCOL::CAlarmTransportAcceptRequest::CBody &p_UpdateInfo)
{
	DataBase::SQLRequest l_oUpdateTransport;
	l_oUpdateTransport.sql_id = UPDATE_ICC_T_ALARM_TRANSPORT;
	l_oUpdateTransport.set["target_user_code"] = p_UpdateInfo.m_strUserCode;
	l_oUpdateTransport.set["target_user_name"] = p_UpdateInfo.m_strUserName;
	l_oUpdateTransport.set["target_seat_no"] = p_UpdateInfo.m_strSeatNo;
	l_oUpdateTransport.set["target_dept_code"] = p_UpdateInfo.m_strDeptCode;
	l_oUpdateTransport.set["target_dept_name"] = p_UpdateInfo.m_strDeptName;
	l_oUpdateTransport.set["target_alarm_id"] = p_UpdateInfo.m_strAlarmID;

	l_oUpdateTransport.set["update_user"] = CREATEUSER;
	l_oUpdateTransport.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	l_oUpdateTransport.param["source_alarm_id"] = p_UpdateInfo.m_strAlarmID;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oUpdateTransport);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "DB Error:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateTransportTable(std::string p_strAlarmID, std::string p_strNewAlarmID, std::string& p_strNewCallrefID)
{
	DataBase::SQLRequest l_oUpdateTransport;
	PROTOCOL::CAlarmTransferCallRespond l_oTransCallInfo;
	if (!GetTransCallInfo(p_strAlarmID, l_oTransCallInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get Alarm info is not exist");
		return false;
	}

	l_oUpdateTransport.sql_id = UPDATE_ICC_T_ALARM_TRANSPORT;

	l_oUpdateTransport.set["target_callref_id"] = l_oTransCallInfo.m_oBody.m_strNewCallRefId;
	l_oUpdateTransport.set["target_alarm_id"] = p_strNewAlarmID;
	l_oUpdateTransport.param["source_alarm_id"] = p_strAlarmID;
	p_strNewCallrefID = l_oTransCallInfo.m_oBody.m_strNewCallRefId;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oUpdateTransport);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "DB Error:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateAlarm(const PROTOCOL::CAlarmInfo&  p_oAlarmInfo)
{
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = UPDATE_ICC_T_ALARM;

	//l_tSQLReqInsertAlarm.param["id"] = p_oAlarmInfo.m_strID;
	//l_tSQLReqInsertAlarm.set["merge_id"] = p_oAlarmInfo.m_strMergeID;
	//l_tSQLReqInsertAlarm.set["title"] = p_oAlarmInfo.m_strTitle;
	//l_tSQLReqInsertAlarm.set["content"] = p_oAlarmInfo.m_strContent;
	//l_tSQLReqInsertAlarm.set["time"] = p_oAlarmInfo.m_strTime;
	////l_tSQLReqInsertAlarm.set["actual_occur_time"] = p_oAlarmInfo.m_strActualOccurTime;
	//l_tSQLReqInsertAlarm.set["addr"] = p_oAlarmInfo.m_strAddr;
	//l_tSQLReqInsertAlarm.set["longitude"] = p_oAlarmInfo.m_strLongitude;
	//l_tSQLReqInsertAlarm.set["latitude"] = p_oAlarmInfo.m_strLatitude;
	//l_tSQLReqInsertAlarm.set["level"] = p_oAlarmInfo.m_strLevel;
	//l_tSQLReqInsertAlarm.set["source_type"] = p_oAlarmInfo.m_strSourceType;
	//l_tSQLReqInsertAlarm.set["source_id"] = p_oAlarmInfo.m_strSourceID;
	//l_tSQLReqInsertAlarm.set["handle_type"] = p_oAlarmInfo.m_strHandleType;
	//l_tSQLReqInsertAlarm.set["first_type"] = p_oAlarmInfo.m_strFirstType;
	//l_tSQLReqInsertAlarm.set["second_type"] = p_oAlarmInfo.m_strSecondType;
	//l_tSQLReqInsertAlarm.set["third_type"] = p_oAlarmInfo.m_strThirdType;
	//l_tSQLReqInsertAlarm.set["fourth_type"] = p_oAlarmInfo.m_strFourthType;
	//l_tSQLReqInsertAlarm.set["vehicle_no"] = p_oAlarmInfo.m_strVehicleNo;
	//l_tSQLReqInsertAlarm.set["vehicle_type"] = p_oAlarmInfo.m_strVehicleType;
	//l_tSQLReqInsertAlarm.set["symbol_code"] = p_oAlarmInfo.m_strSymbolCode;
	////l_tSQLReqInsertAlarm.set["symbol_addr"] = p_oAlarmInfo.m_strSymbolAddr;
	////l_tSQLReqInsertAlarm.set["fire_building_type"] = p_oAlarmInfo.m_strFireBuildingType;

	////l_tSQLReqInsertAlarm.set["event_type"] = p_oAlarmInfo.m_strEventType;

	//l_tSQLReqInsertAlarm.set["called_no_type"] = p_oAlarmInfo.m_strCalledNoType;
	////l_tSQLReqInsertAlarm.set["actual_called_no_type"] = p_oAlarmInfo.m_strActualCalledNoType;

	//l_tSQLReqInsertAlarm.set["caller_no"] = p_oAlarmInfo.m_strCallerNo;
	//l_tSQLReqInsertAlarm.set["caller_name"] = p_oAlarmInfo.m_strCallerName;
	//l_tSQLReqInsertAlarm.set["caller_addr"] = p_oAlarmInfo.m_strCallerAddr;
	//l_tSQLReqInsertAlarm.set["caller_id"] = p_oAlarmInfo.m_strCallerID;
	//l_tSQLReqInsertAlarm.set["caller_id_type"] = p_oAlarmInfo.m_strCallerIDType;
	//l_tSQLReqInsertAlarm.set["caller_gender"] = p_oAlarmInfo.m_strCallerGender;
	////l_tSQLReqInsertAlarm.set["caller_age"] = p_oAlarmInfo.m_strCallerAge;
	////l_tSQLReqInsertAlarm.set["caller_birthday"] = p_oAlarmInfo.m_strCallerBirthday;

	//l_tSQLReqInsertAlarm.set["contact_no"] = p_oAlarmInfo.m_strContactNo;
	////l_tSQLReqInsertAlarm.set["contact_name"] = p_oAlarmInfo.m_strContactName;
	////l_tSQLReqInsertAlarm.set["contact_addr"] = p_oAlarmInfo.m_strContactAddr;
	////l_tSQLReqInsertAlarm.set["contact_id"] = p_oAlarmInfo.m_strContactID;
	////l_tSQLReqInsertAlarm.set["contact_id_type"] = p_oAlarmInfo.m_strContactIDType;
	////l_tSQLReqInsertAlarm.set["contact_gender"] = p_oAlarmInfo.m_strContactGender;
	////l_tSQLReqInsertAlarm.set["contact_age"] = p_oAlarmInfo.m_strContactAge;
	////l_tSQLReqInsertAlarm.set["contact_birthday"] = p_oAlarmInfo.m_strContactBirthday;

	////l_tSQLReqInsertAlarm.set["admin_dept_district_code"] = p_oAlarmInfo.m_strAdminDeptDistrictCode;
	//l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_oAlarmInfo.m_strAdminDeptCode;
	//l_tSQLReqInsertAlarm.set["admin_dept_name"] = p_oAlarmInfo.m_strAdminDeptName;

	//l_tSQLReqInsertAlarm.set["receipt_dept_district_code"] = p_oAlarmInfo.m_strReceiptDeptDistrictCode;
	//l_tSQLReqInsertAlarm.set["receipt_dept_code"] = p_oAlarmInfo.m_strReceiptDeptCode;
	//l_tSQLReqInsertAlarm.set["receipt_dept_name"] = p_oAlarmInfo.m_strReceiptDeptName;
	////l_tSQLReqInsertAlarm.set["leader_code"] = p_oAlarmInfo.m_strLeaderCode;
	////l_tSQLReqInsertAlarm.set["leader_name"] = p_oAlarmInfo.m_strLeaderName;
	//l_tSQLReqInsertAlarm.set["receipt_code"] = p_oAlarmInfo.m_strReceiptCode;
	//l_tSQLReqInsertAlarm.set["receipt_name"] = p_oAlarmInfo.m_strReceiptName;

	////l_tSQLReqInsertAlarm.set["dispatch_suggestion"] = p_oAlarmInfo.m_strDispatchSuggestion;

	//l_tSQLReqInsertAlarm.set["update_user"] = p_oAlarmInfo.m_strReceiptName;
	////l_tSQLReqInsertAlarm.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	//l_tSQLReqInsertAlarm.set["is_privacy"] = p_oAlarmInfo.m_strPrivacy;
	//l_tSQLReqInsertAlarm.set["remark"] = p_oAlarmInfo.m_strRemark;

	l_tSQLReqInsertAlarm.sql_id = "select_icc_t_jjdb";
	l_tSQLReqInsertAlarm.param["id"] = p_oAlarmInfo.m_strID;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_DEBUG(m_pLog, "Select Alarm sql:[%s]", l_pRSet->GetSQL().c_str());

	std::string strLongitude;
	std::string strLatitude;
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	if (l_pRSet->Next())
	{
		if (l_pRSet->GetValue("bjrxzb").empty()&& !p_oAlarmInfo.m_strManualLongitude.empty())
		{
			strLongitude = p_oAlarmInfo.m_strManualLongitude;
		}
		if (l_pRSet->GetValue("bjryzb").empty()&&!p_oAlarmInfo.m_strManualLatitude.empty())
		{
			strLatitude = p_oAlarmInfo.m_strManualLatitude;
		}
	}
	ICC_LOG_LOWDEBUG(m_pLog, "Longitude:[%s],Latitude:[%s]", strLongitude.c_str(), strLatitude.c_str());

	l_tSQLReqInsertAlarm.param.clear();
	l_tSQLReqInsertAlarm.param["id"] = p_oAlarmInfo.m_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_oAlarmInfo.m_strID);
	if (strTime != "")
	{
		l_tSQLReqInsertAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReqInsertAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	l_tSQLReqInsertAlarm.set["merge_id"] = p_oAlarmInfo.m_strMergeID;
	l_tSQLReqInsertAlarm.set["merge_type"] = p_oAlarmInfo.m_strMergeType;
	l_tSQLReqInsertAlarm.set["receipt_seatno"] = p_oAlarmInfo.m_strSeatNo;
	l_tSQLReqInsertAlarm.set["label"] = p_oAlarmInfo.m_strTitle;
	l_tSQLReqInsertAlarm.set["content"] = p_oAlarmInfo.m_strContent;
	l_tSQLReqInsertAlarm.set["receiving_time"] = p_oAlarmInfo.m_strTime;
	l_tSQLReqInsertAlarm.set["received_time"] = p_oAlarmInfo.m_strReceivedTime;
	l_tSQLReqInsertAlarm.set["addr"] = p_oAlarmInfo.m_strAddr;
	if (!p_oAlarmInfo.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["longitude"] = p_oAlarmInfo.m_strLongitude;
	}
	else if (!strLongitude.empty()&& p_oAlarmInfo.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["longitude"] = strLongitude;
	}
	if (!p_oAlarmInfo.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["latitude"] = p_oAlarmInfo.m_strLatitude;
	}
	else if (!strLatitude.empty()&& p_oAlarmInfo.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["latitude"] = strLatitude;
	}
	
	l_tSQLReqInsertAlarm.set["state"] = p_oAlarmInfo.m_strState;
	l_tSQLReqInsertAlarm.set["level"] = p_oAlarmInfo.m_strLevel;
	l_tSQLReqInsertAlarm.set["source_type"] = p_oAlarmInfo.m_strSourceType;
	l_tSQLReqInsertAlarm.set["source_id"] = p_oAlarmInfo.m_strSourceID;
	l_tSQLReqInsertAlarm.set["handle_type"] = p_oAlarmInfo.m_strHandleType;
	l_tSQLReqInsertAlarm.set["first_type"] = p_oAlarmInfo.m_strFirstType;
	l_tSQLReqInsertAlarm.set["second_type"] = p_oAlarmInfo.m_strSecondType;
	l_tSQLReqInsertAlarm.set["third_type"] = p_oAlarmInfo.m_strThirdType;
	l_tSQLReqInsertAlarm.set["fourth_type"] = p_oAlarmInfo.m_strFourthType;
	l_tSQLReqInsertAlarm.set["vehicle_no"] = p_oAlarmInfo.m_strVehicleNo;
	l_tSQLReqInsertAlarm.set["vehicle_type"] = p_oAlarmInfo.m_strVehicleType;
	l_tSQLReqInsertAlarm.set["symbol_code"] = p_oAlarmInfo.m_strSymbolCode;

	l_tSQLReqInsertAlarm.set["called_no_type"] = p_oAlarmInfo.m_strCalledNoType;

	l_tSQLReqInsertAlarm.set["caller_no"] = p_oAlarmInfo.m_strCallerNo;
	l_tSQLReqInsertAlarm.set["caller_name"] = p_oAlarmInfo.m_strCallerName;
	l_tSQLReqInsertAlarm.set["caller_addr"] = p_oAlarmInfo.m_strCallerAddr;
	l_tSQLReqInsertAlarm.set["caller_id"] = p_oAlarmInfo.m_strCallerID;
	l_tSQLReqInsertAlarm.set["caller_id_type"] = p_oAlarmInfo.m_strCallerIDType;
	l_tSQLReqInsertAlarm.set["caller_gender"] = p_oAlarmInfo.m_strCallerGender;

	l_tSQLReqInsertAlarm.set["contact_no"] = p_oAlarmInfo.m_strContactNo;

	l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_oAlarmInfo.m_strAdminDeptCode;
	l_tSQLReqInsertAlarm.set["admin_dept_name"] = p_oAlarmInfo.m_strAdminDeptName;
	l_tSQLReqInsertAlarm.set["receipt_dept_district_code"] = p_oAlarmInfo.m_strReceiptDeptDistrictCode;
	l_tSQLReqInsertAlarm.set["receipt_dept_code"] = p_oAlarmInfo.m_strReceiptDeptCode;
	l_tSQLReqInsertAlarm.set["receipt_dept_name"] = p_oAlarmInfo.m_strReceiptDeptName;

	l_tSQLReqInsertAlarm.set["receipt_code"] = p_oAlarmInfo.m_strReceiptCode;
	l_tSQLReqInsertAlarm.set["receipt_name"] = p_oAlarmInfo.m_strReceiptName;

	l_tSQLReqInsertAlarm.set["create_time"] = p_oAlarmInfo.m_strCreateTime;
	l_tSQLReqInsertAlarm.set["create_user"] = p_oAlarmInfo.m_strCreateUser;
	l_tSQLReqInsertAlarm.set["update_time"] = p_oAlarmInfo.m_strUpdateTime;
	l_tSQLReqInsertAlarm.set["update_user"] = p_oAlarmInfo.m_strUpdateUser;
	l_tSQLReqInsertAlarm.set["is_privacy"] = p_oAlarmInfo.m_strPrivacy;
	l_tSQLReqInsertAlarm.set["remark"] = p_oAlarmInfo.m_strRemark;
	if (!p_oAlarmInfo.m_strIsVisitor.empty())
	{
		l_tSQLReqInsertAlarm.set["is_visitor"] = p_oAlarmInfo.m_strIsVisitor;
	}
	if (p_oAlarmInfo.m_strIsFeedBack.empty())
	{
		l_tSQLReqInsertAlarm.set["is_feedback"] = p_oAlarmInfo.m_strIsFeedBack;
	}
	
	//新增
	l_tSQLReqInsertAlarm.set["alarm_addr"] = p_oAlarmInfo.m_strAlarmAddr;
	l_tSQLReqInsertAlarm.set["caller_user_name"] = p_oAlarmInfo.m_strCallerUserName;
	l_tSQLReqInsertAlarm.set["erpetrators_number"] = p_oAlarmInfo.m_strErpetratorsNumber;

	l_tSQLReqInsertAlarm.set["is_armed"] = p_oAlarmInfo.m_strIsArmed;

	l_tSQLReqInsertAlarm.set["is_hazardous_substances"] = p_oAlarmInfo.m_strIsHazardousSubstances;

	l_tSQLReqInsertAlarm.set["is_explosion_or_leakage"] = p_oAlarmInfo.m_strIsExplosionOrLeakage;

	l_tSQLReqInsertAlarm.set["desc_of_trapped"] = p_oAlarmInfo.m_strDescOfTrapped;
	l_tSQLReqInsertAlarm.set["desc_of_injured"] = p_oAlarmInfo.m_strDescOfInjured;
	l_tSQLReqInsertAlarm.set["desc_of_dead"] = p_oAlarmInfo.m_strDescOfDead;
	l_tSQLReqInsertAlarm.set["is_foreign_language"] = p_oAlarmInfo.m_strIsForeignLanguage;
	l_tSQLReqInsertAlarm.set["manual_longitude"] = p_oAlarmInfo.m_strManualLongitude;
	l_tSQLReqInsertAlarm.set["manual_latitude"] = p_oAlarmInfo.m_strManualLatitude;
	l_tSQLReqInsertAlarm.set["emergency_rescue_level"] = p_oAlarmInfo.m_strEmergencyRescueLevel;
	l_tSQLReqInsertAlarm.set["is_hazardous_vehicle"] = p_oAlarmInfo.m_strIsHazardousVehicle;
	l_tSQLReqInsertAlarm.set["receipt_srv_name"] = p_oAlarmInfo.m_strReceiptSrvName;
	l_tSQLReqInsertAlarm.set["admin_dept_org_code"] = p_oAlarmInfo.m_strAdminDeptOrgCode;
	l_tSQLReqInsertAlarm.set["receipt_dept_org_code"] = p_oAlarmInfo.m_strReceiptDeptOrgCode;
	if (!p_oAlarmInfo.m_strIsInvalid.empty())
	{
		l_tSQLReqInsertAlarm.set["is_invalid"] = p_oAlarmInfo.m_strIsInvalid;
	}
	
	l_tSQLReqInsertAlarm.set["business_status"] = p_oAlarmInfo.m_strBusinessState;
	l_tSQLReqInsertAlarm.set["update_user"] = p_oAlarmInfo.m_strReceiptName;
    l_tSQLReqInsertAlarm.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	if (!p_oAlarmInfo.m_strJurisdictionalOrgcode.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgcode"] = p_oAlarmInfo.m_strJurisdictionalOrgcode;
	}
	if (!p_oAlarmInfo.m_strJurisdictionalOrgname.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgname"] = p_oAlarmInfo.m_strJurisdictionalOrgname;
	}
	if (!p_oAlarmInfo.m_strJurisdictionalOrgidentifier.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgidentifier"] = p_oAlarmInfo.m_strJurisdictionalOrgidentifier;
	}
	
	if (!p_oAlarmInfo.m_strHadPush.empty())
	{
		l_tSQLReqInsertAlarm.set["had_push"] = p_oAlarmInfo.m_strHadPush;
	}
	
	if (!p_oAlarmInfo.m_strCreateTeminal.empty())
	{
		l_tSQLReqInsertAlarm.set["createTeminal"] = p_oAlarmInfo.m_strCreateTeminal;
	}

	if (!p_oAlarmInfo.m_strUpdateTeminal.empty())
	{
		l_tSQLReqInsertAlarm.set["updateTeminal"] = p_oAlarmInfo.m_strUpdateTeminal;
	}

	l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateAlarm(std::string p_strAlarmID, std::string p_strNewAlarmID, const PROTOCOL::CTransPortAlarmRequest& p_oInfo, std::string strCallRefID, std::string strNewCallrefID)
{
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = UPDATE_ICC_T_ALARM;

	l_tSQLReqInsertAlarm.set["state"] = ALARM_STATE;
	l_tSQLReqInsertAlarm.set["is_delete"] = "1";  //改为已删除
	std::string strTempTime = m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT);
	l_tSQLReqInsertAlarm.set["update_time"] = strTempTime;
	std::string strAdminDeptCode = _IsExistAdminDept(p_strAlarmID);
	if (strAdminDeptCode.empty())
	{
		std::string strAdminDeptName;
		std::string strAdminDeptDistrictCode;
		if (!p_oInfo.m_oBody.m_strDeptCode.empty())
		{
			//防止获取不到管辖单位数据导致数据入库失败,故取Redis数据失败只报错不返回
			if (!_BuildDeptInfo(p_oInfo.m_oBody.m_strDeptCode, strAdminDeptName, strAdminDeptDistrictCode))
			{
				ICC_LOG_DEBUG(m_pLog, "build dept info faled,dept_code:[%s]", p_oInfo.m_oBody.m_strDeptCode.c_str());
			}
			else
			{
				l_tSQLReqInsertAlarm.set["admin_dept_name"] = strAdminDeptName;
				l_tSQLReqInsertAlarm.set["admin_dept_org_code"] = strAdminDeptDistrictCode;
				l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_oInfo.m_oBody.m_strDeptCode;
			}
		}
	}

	l_tSQLReqInsertAlarm.param["id"] = p_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReqInsertAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReqInsertAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Error Msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		// 需要同步更新Redis 因为其发出去的MQ的消息里面数据 是从Redis里面取值
		SetUpdateTime(strTempTime, p_strAlarmID);
		// 同步更新警单 [5/11/2018 w26326]
		SyncAlarmReceiptInfo(p_strAlarmID, ALARM_STATE,true);
	}

	if (!strCallRefID.empty() && !p_strNewAlarmID.empty()) {
		DataBase::SQLRequest l_tSQLReqInsertCallref;
		l_tSQLReqInsertCallref.sql_id = "update_icc_t_callevent_alarmid";
		l_tSQLReqInsertCallref.param["callref_id"] = strCallRefID;
		if (strTime != "")
		{
			l_tSQLReqInsertCallref.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_tSQLReqInsertCallref.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
		}
		l_tSQLReqInsertCallref.param["rel_alarm_id"] = p_strNewAlarmID;
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReqInsertCallref);
		ICC_LOG_INFO(m_pLog, " update_icc_t_callevent_alarmid sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Error Msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
		else {
			ICC_LOG_INFO(m_pLog, "update  strCallRefID %s  p_strAlarmID:[%s]", strCallRefID.c_str(), p_strNewAlarmID.c_str());
		}
		if (!strNewCallrefID.empty()) {
			l_tSQLReqInsertCallref.param["callref_id"] = strNewCallrefID;
		}
		l_pResult = m_pDBConn->Exec(l_tSQLReqInsertCallref);
		ICC_LOG_INFO(m_pLog, " update_icc_t_callevent_alarmid sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Error Msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
		else {
			ICC_LOG_INFO(m_pLog, "update  strCallRefID %s  p_strAlarmID:[%s]", strNewCallrefID.c_str(), p_strNewAlarmID.c_str());
		}
	}
	return true;
}

std::string CBusinessImpl::GenAlarmLogContent(const std::vector<std::string>& p_vecParamList)
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

std::string CBusinessImpl::GenAlarmLogAttach(std::string& type, std::string& id)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

	std::string l_strPath = "/";
	std::string l_strNum = std::to_string(0);

	if (0 == type.compare("7"))
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", "jjlyh");
	}
	else
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", type);
	}

	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/id", id);
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/name", "");
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/path", "");

	return l_pIJson->ToString();
}

void CBusinessImpl::EditTransportAlarmLog(const std::vector<std::string>& p_vecParamList, std::string p_strAlarmID, std::string p_strType)
{
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = p_strAlarmID;
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(p_vecParamList);
	std::string type = LOG_ALARM_RESOURCETYPE;
	std::string id;
	l_oAlarmLogInfo.m_strOperateAttachDesc = GenAlarmLogAttach(type, id);
	l_oAlarmLogInfo.m_strOperate = p_strType;
	l_oAlarmLogInfo.m_strSourceName = "icc";

	if (!AddAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Edit Transportalarm Log Fail");
	}
}
void CBusinessImpl::EditTakeOverCallLog(const std::vector<std::string>& p_vecParamList, const std::string&  p_strAlarmID, const std::string& p_strCTICallRefId)
{
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
	l_oAlarmLogInfo.m_strAlarmID = p_strAlarmID;
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(p_vecParamList);
	std::string type = LOG_PHONE_RESOURCETYPE;
	std::string id = p_strCTICallRefId;
	l_oAlarmLogInfo.m_strOperateAttachDesc = GenAlarmLogAttach(type, id);
	l_oAlarmLogInfo.m_strOperate = LOG_TAKEOVER_CALL;
	l_oAlarmLogInfo.m_strCreateUser = CREATEUSER;
	l_oAlarmLogInfo.m_strCreateTime = m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT);
	l_oAlarmLogInfo.m_strSourceName = "icc";

	if (InsertDBAlarmLogInfo(l_oAlarmLogInfo))
	{
		SyncAlarmLogInfo(l_oAlarmLogInfo);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Edit TakeOverCall Log Fail");
	}
}
bool CBusinessImpl::CopyAlarmLog(const std::string& p_strNewAlarmID, const std::string& p_strAlarmID)
{
	PROTOCOL::CGetAlarmLogRespond::CBody l_mapAlarmLogInfo;

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_log";
	l_SqlRequest.param["jjdbh"] = p_strAlarmID;
	l_SqlRequest.param["orderby"] = "operatetime";

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "Select AlarmLog By AlarmID, sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	while (l_pResult->Next())
	{
		PROTOCOL::CGetAlarmLogRespond::CData l_oData;
		
		l_oData.m_strAlarmID = p_strNewAlarmID;
		l_oData.m_strID = l_pResult->GetValue("id");
		l_oData.m_strProcessID = l_pResult->GetValue("pjdbh");
		l_oData.m_strFeedbackID = l_pResult->GetValue("fkdbh");
		l_oData.m_strSeatNo = l_pResult->GetValue("seat_no");
		l_oData.m_strOperate = l_pResult->GetValue("operate");
		l_oData.m_strOperateContent = l_pResult->GetValue("operate_content");
		l_oData.m_strFromType = l_pResult->GetValue("from_type");
		l_oData.m_strFromObject = l_pResult->GetValue("from_object");
		l_oData.m_strFromObjectName = l_pResult->GetValue("from_object_name");
		l_oData.m_strFromObjectOrgName = l_pResult->GetValue("from_object_org_name");
		l_oData.m_strFromObjectOrgCode = l_pResult->GetValue("from_object_org_code");
		l_oData.m_strFromOrgIdentifier = l_pResult->GetValue("from_object_org_identifier");
		l_oData.m_strToType = l_pResult->GetValue("to_type");
		l_oData.m_strToObject = l_pResult->GetValue("to_object");
		l_oData.m_strToObjectName = l_pResult->GetValue("to_object_name");
		l_oData.m_strToObjectOrgName = l_pResult->GetValue("to_object_org_name");
		l_oData.m_strToObjectOrgCode = l_pResult->GetValue("to_object_org_code");
		l_oData.m_strToObjectOrgIdentifier = l_pResult->GetValue("to_object_org_identifier");
		l_oData.m_strCreateUser = l_pResult->GetValue("create_user");
		l_oData.m_strCreateTime = l_pResult->GetValue("create_time");
		l_oData.m_strDeptOrgCode = l_pResult->GetValue("dept_org_code");
		l_oData.m_strSourceName = l_pResult->GetValue("source_name");
		l_oData.m_strOperateAttachDesc = l_pResult->GetValue("operate_attach_desc");

		l_mapAlarmLogInfo.m_vecData.push_back(l_oData);
	}

	for (auto l_objLogInfo : l_mapAlarmLogInfo.m_vecData)
	{
		PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
		l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
		l_oAlarmLogInfo.m_strAlarmID = l_objLogInfo.m_strAlarmID;
		l_oAlarmLogInfo.m_strProcessID = l_objLogInfo.m_strProcessID;
		l_oAlarmLogInfo.m_strFeedbackID = l_objLogInfo.m_strFeedbackID;
		l_oAlarmLogInfo.m_strSeatNo = l_objLogInfo.m_strSeatNo;
		l_oAlarmLogInfo.m_strOperate = l_objLogInfo.m_strOperate;
		l_oAlarmLogInfo.m_strOperateContent = l_objLogInfo.m_strOperateContent;
		l_oAlarmLogInfo.m_strFromType = l_objLogInfo.m_strFromType;
		l_oAlarmLogInfo.m_strFromObject = l_objLogInfo.m_strFromObject;
		l_oAlarmLogInfo.m_strFromObjectName = l_objLogInfo.m_strFromObjectName;
		l_oAlarmLogInfo.m_strFromObjectOrgName = l_objLogInfo.m_strFromObjectOrgName;
		l_oAlarmLogInfo.m_strFromObjectOrgCode = l_objLogInfo.m_strFromObjectOrgCode;
		l_oAlarmLogInfo.m_strFromOrgIdentifier = l_objLogInfo.m_strFromOrgIdentifier;
		l_oAlarmLogInfo.m_strToType = l_objLogInfo.m_strToType;
		l_oAlarmLogInfo.m_strToObject = l_objLogInfo.m_strToObject;
		l_oAlarmLogInfo.m_strToObjectName = l_objLogInfo.m_strToObjectName;
		l_oAlarmLogInfo.m_strToObjectOrgName = l_objLogInfo.m_strToObjectOrgName;
		l_oAlarmLogInfo.m_strToObjectOrgCode = l_objLogInfo.m_strToObjectOrgCode;
		l_oAlarmLogInfo.m_strToObjectOrgIdentifier = l_objLogInfo.m_strToObjectOrgIdentifier;
		l_oAlarmLogInfo.m_strCreateUser = l_objLogInfo.m_strCreateUser;
		l_oAlarmLogInfo.m_strCreateTime = l_objLogInfo.m_strCreateTime;
		l_oAlarmLogInfo.m_strDeptOrgCode = l_objLogInfo.m_strDeptOrgCode;
		l_oAlarmLogInfo.m_strSourceName = "icc";
		l_oAlarmLogInfo.m_strOperateAttachDesc = l_objLogInfo.m_strOperateAttachDesc;

		if (InsertDBAlarmLogInfo(l_oAlarmLogInfo))
		{
			SyncAlarmLogInfo(l_oAlarmLogInfo);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Copy Alarm Log Fail");
		}
	}

	return true;
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMsg = p_pNotifiRequest->GetMessages();
	PROTOCOL::CTimer l_oTimerInfo;
	if (!l_oTimerInfo.ParseString(l_strMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parase sting error[%s]", l_strMsg.c_str());
		return;
	}

	std::string l_strAlarmID;
	PROTOCOL::CTransPortAlarmRequest l_oInfo;
	if (!GetInfoByTimerName(l_oTimerInfo.m_oBody.m_strTimerName, l_strAlarmID,l_oInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "invalid timer");
		return;
	}
	// 写超时转警流水 {0}[工号-{1}, 单位-{2}]转移警单到[单位-{3}]失败：超时 [5/2/2018 w26326]
	std::vector<std::string> l_vecParamList;

	Data::CStaffInfo l_oStaffInfo;
	std::string strStaffCode = _GetStaffCodeBySeat(l_oInfo.m_oBody.m_strSeatNo);
	if (!_GetStaffInfo(strStaffCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!!!");
	}
	std::string strTransportName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oInfo.m_oBody.m_strUserName);
	l_vecParamList.push_back(strTransportName);
	l_vecParamList.push_back(l_oInfo.m_oBody.m_strSeatNo);
	l_vecParamList.push_back(l_oInfo.m_oBody.m_strDeptName);
	l_vecParamList.push_back(l_oInfo.m_oBody.m_strTargetDeptName);
	EditTransportAlarmLog(l_vecParamList, l_oInfo.m_oBody.m_strAlarmID, LOG_TRANSPORT_ALARM_FAIL_TIMEOUT);

	if (!l_oInfo.m_oBody.m_strCallRefID.empty())
	{
		// 发起重拾电话 [6/28/2018 w26326]
		SendReCallRequest(l_strAlarmID);
	}
	// 发送转警接受超时同步 [5/8/2018 w26326]
	SendTopicTransResultSync(l_strAlarmID, TransportAcceptTimeOut, "");
}

void CBusinessImpl::SyncAlarmReceiptInfo(std::string p_strAlarmID, std::string p_strState,bool bIsDelete)
{
	PROTOCOL::CTransPortAlarmRequest l_info;
	if (!GetAlarmInfoByAlarmID(p_strAlarmID, l_info))
	{
		ICC_LOG_DEBUG(m_pLog, "get Alarm info is not exist");
		return;
	}
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmInfo l_oAlarmInfo = l_info.m_oBody.m_oAlarm;
	PROTOCOL::CAlarmSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmSync.m_oHeader.m_strCmd = ALARM_SYNC;
	l_oAlarmSync.m_oHeader.m_strRequest = ALARM_TRANSPORT_TOAPIC_NAME;
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	if (!l_oAlarmInfo.m_strMsgSource.compare("vcs"))
	{
		l_oAlarmSync.m_oHeader.m_strCMSProperty = MSG_SOURCE_VCS;
	}
	else
	{
		l_oAlarmSync.m_oHeader.m_strCMSProperty = MSG_SOURCE;
	}

	l_oAlarmSync.m_oBody.m_oAlarmInfo = l_oAlarmInfo;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strState = p_strState;
	if (bIsDelete)
	{
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strDeleteFlag = "1";
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strIsInvalid = "1";
		l_oAlarmSync.m_oBody.m_strSyncType = "3";
	}
	else
	{
		l_oAlarmSync.m_oBody.m_strSyncType = std::to_string(UPADATE_ALARM_SYNC);
	}

	
	/*l_oAlarmSync.m_oBody.m_strID = l_oAlarmInfo.m_strID;
	l_oAlarmSync.m_oBody.m_strMergeID = l_oAlarmInfo.m_strMergeID;
	l_oAlarmSync.m_oBody.m_strSeatNo = l_oAlarmInfo.m_strSeatNo;
	l_oAlarmSync.m_oBody.m_strTitle = l_oAlarmInfo.m_strTitle;
	l_oAlarmSync.m_oBody.m_strContent = l_oAlarmInfo.m_strContent;
	l_oAlarmSync.m_oBody.m_strTime = l_oAlarmInfo.m_strTime;
	l_oAlarmSync.m_oBody.m_strActualOccurTime = l_oAlarmInfo.m_strActualOccurTime;
	l_oAlarmSync.m_oBody.m_strAddr = l_oAlarmInfo.m_strAddr;
	l_oAlarmSync.m_oBody.m_strLongitude = l_oAlarmInfo.m_strLongitude;
	l_oAlarmSync.m_oBody.m_strLatitude = l_oAlarmInfo.m_strLatitude;
	l_oAlarmSync.m_oBody.m_strState = p_strState;
	l_oAlarmSync.m_oBody.m_strLevel = l_oAlarmInfo.m_strLevel;
	l_oAlarmSync.m_oBody.m_strSourceType = l_oAlarmInfo.m_strSourceType;
	l_oAlarmSync.m_oBody.m_strSourceID = l_oAlarmInfo.m_strSourceID;
	l_oAlarmSync.m_oBody.m_strHandleType = l_oAlarmInfo.m_strHandleType;
	l_oAlarmSync.m_oBody.m_strFirstType = l_oAlarmInfo.m_strFirstType;
	l_oAlarmSync.m_oBody.m_strSecondType = l_oAlarmInfo.m_strSecondType;
	l_oAlarmSync.m_oBody.m_strThirdType = l_oAlarmInfo.m_strThirdType;
	l_oAlarmSync.m_oBody.m_strFourthType = l_oAlarmInfo.m_strFourthType;

	l_oAlarmSync.m_oBody.m_strVehicleNo = l_oAlarmInfo.m_strVehicleNo;
	l_oAlarmSync.m_oBody.m_strVehicleType = l_oAlarmInfo.m_strVehicleType;
	l_oAlarmSync.m_oBody.m_strSymbolCode = l_oAlarmInfo.m_strSymbolCode;
	l_oAlarmSync.m_oBody.m_strSymbolAddr = l_oAlarmInfo.m_strSymbolAddr;

	l_oAlarmSync.m_oBody.m_strFireBuildingType = l_oAlarmInfo.m_strFireBuildingType;

	l_oAlarmSync.m_oBody.m_strEventType = l_oAlarmInfo.m_strEventType;

	l_oAlarmSync.m_oBody.m_strCalledNoType = l_oAlarmInfo.m_strCalledNoType;
	l_oAlarmSync.m_oBody.m_strActualCalledNoType = l_oAlarmInfo.m_strActualCalledNoType;

	l_oAlarmSync.m_oBody.m_strCallerNo = l_oAlarmInfo.m_strCallerNo;
	l_oAlarmSync.m_oBody.m_strCallerName = l_oAlarmInfo.m_strCallerName;
	l_oAlarmSync.m_oBody.m_strCallerAddr = l_oAlarmInfo.m_strCallerAddr;
	l_oAlarmSync.m_oBody.m_strCallerID = l_oAlarmInfo.m_strCallerID;
	l_oAlarmSync.m_oBody.m_strCallerIDType = l_oAlarmInfo.m_strCallerIDType;
	l_oAlarmSync.m_oBody.m_strCallerGender = l_oAlarmInfo.m_strCallerGender;
	l_oAlarmSync.m_oBody.m_strCallerAge = l_oAlarmInfo.m_strCallerAge;
	l_oAlarmSync.m_oBody.m_strCallerBirthday = l_oAlarmInfo.m_strCallerBirthday;

	l_oAlarmSync.m_oBody.m_strContactNo = l_oAlarmInfo.m_strContactNo;
	l_oAlarmSync.m_oBody.m_strContactName = l_oAlarmInfo.m_strContactName;
	l_oAlarmSync.m_oBody.m_strContactAddr = l_oAlarmInfo.m_strContactAddr;
	l_oAlarmSync.m_oBody.m_strContactID = l_oAlarmInfo.m_strContactID;
	l_oAlarmSync.m_oBody.m_strContactIDType = l_oAlarmInfo.m_strContactIDType;
	l_oAlarmSync.m_oBody.m_strContactGender = l_oAlarmInfo.m_strContactGender;
	l_oAlarmSync.m_oBody.m_strContactAge = l_oAlarmInfo.m_strContactAge;
	l_oAlarmSync.m_oBody.m_strContactBirthday = l_oAlarmInfo.m_strContactBirthday;

	l_oAlarmSync.m_oBody.m_strAdminDeptDistrictCode = l_oAlarmInfo.m_strAdminDeptDistrictCode;
	l_oAlarmSync.m_oBody.m_strAdminDeptCode = l_oAlarmInfo.m_strAdminDeptCode;
	l_oAlarmSync.m_oBody.m_strAdminDeptName = l_oAlarmInfo.m_strAdminDeptName;

	l_oAlarmSync.m_oBody.m_strReceiptDeptDistrictCode = l_oAlarmInfo.m_strReceiptDeptDistrictCode;
	l_oAlarmSync.m_oBody.m_strReceiptDeptCode = l_oAlarmInfo.m_strReceiptDeptCode;
	l_oAlarmSync.m_oBody.m_strReceiptDeptName = l_oAlarmInfo.m_strReceiptDeptName;
	l_oAlarmSync.m_oBody.m_strLeaderCode = l_oAlarmInfo.m_strLeaderCode;
	l_oAlarmSync.m_oBody.m_strLeaderName = l_oAlarmInfo.m_strLeaderName;
	l_oAlarmSync.m_oBody.m_strReceiptCode = l_oAlarmInfo.m_strReceiptCode;
	l_oAlarmSync.m_oBody.m_strReceiptName = l_oAlarmInfo.m_strReceiptName;

	l_oAlarmSync.m_oBody.m_strDispatchSuggestion = l_oAlarmInfo.m_strDispatchSuggestion;

	l_oAlarmSync.m_oBody.m_strCreateUser = l_oAlarmInfo.m_strCreateUser;
	l_oAlarmSync.m_oBody.m_strCreateTime = l_oAlarmInfo.m_strCreateTime;
	l_oAlarmSync.m_oBody.m_strUpdateUser = l_oAlarmInfo.m_strUpdateUser;
	l_oAlarmSync.m_oBody.m_strUpdateTime = l_oAlarmInfo.m_strUpdateTime;
	l_oAlarmSync.m_oBody.m_strPrivacy = l_oAlarmInfo.m_strPrivacy;
	l_oAlarmSync.m_oBody.m_strRemark = l_oAlarmInfo.m_strRemark;*/

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::AddTransportQue(std::string p_strAlarmID, CTransportQueue& p_tTransportQueue)
{
	if (p_strAlarmID.empty())
	{
		return;
	}
	//JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
	std::string l_strInfo = p_tTransportQueue.ToJson(ICCGetIJsonFactory());

	m_pRedisClient->HSet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo);
}

void CBusinessImpl::DeleteTransportQue(std::string p_strAlarmID)
{
	m_pRedisClient->HDel(MANGE_TRANSPORT_ALRM, p_strAlarmID);
}

void CBusinessImpl::AddRelateID(std::string p_strRelateID, std::string p_strAlarmID)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		return;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	l_oTrans.m_mapRelateID[p_strRelateID] = p_strAlarmID;
	AddTransportQue(p_strAlarmID,l_oTrans);
}

bool CBusinessImpl::GetAlarmIDByRelateID(std::string p_strRelateID, std::string& p_strAlarmID)
{
	int l_iNext = 0;
	std::map<std::string, std::string> l_MapRe;
	while ((l_iNext = m_pRedisClient->HScan(MANGE_TRANSPORT_ALRM, l_iNext, l_MapRe)) > 0);

	auto iter = l_MapRe.begin();
	for (; iter != l_MapRe.end();iter++)
	{
		CTransportQueue l_oTrans;
		l_oTrans.Parse(iter->second, ICCGetIJsonFactory());

		auto it = l_oTrans.m_mapRelateID.find(p_strRelateID);
		if (it != l_oTrans.m_mapRelateID.end())
		{
			p_strAlarmID = l_oTrans.strAlarmID;
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::GetAlarmInfoByAlarmID(std::string p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		ICC_LOG_ERROR(m_pLog, "get transport alarminfo failed,not exit[%s]", p_strAlarmID.c_str());
		return false;
	}

	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	p_oInfo = l_oTrans.oSourceInfo;

	ICC_LOG_DEBUG(m_pLog, "get transport alarminfo. [%s]", l_strInfo.c_str());
	return true;
}
bool CBusinessImpl::GetAlarmInfoByAlarmIDEx(std::string p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo, std::string& p_strTransportType)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		ICC_LOG_ERROR(m_pLog, "get transport alarminfo failed,not exit[%s]", p_strAlarmID.c_str());
		return false;
	}

	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	p_oInfo = l_oTrans.oSourceInfo;
	p_strTransportType = l_oTrans.strTransportType;

	ICC_LOG_DEBUG(m_pLog, "get transport alarminfo. [%s], transport type: [%s]", l_strInfo.c_str(), p_strTransportType.c_str());
	return true;
}

void CBusinessImpl::SetTransCallInfo(const PROTOCOL::CAlarmTransferCallRespond& p_oTransCallByDeptResp, std::string p_strAlarmID)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		return;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	l_oTrans.oTransferCallInfo = p_oTransCallByDeptResp;
	AddTransportQue(p_strAlarmID, l_oTrans);
}

void CBusinessImpl::SetTraget(std::string p_strTarget, std::string p_strAlarmID)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		return;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo,ICCGetIJsonFactory());
	l_oTrans.oSourceInfo.m_oBody.m_strTargetSeatNo = p_strTarget;
	AddTransportQue(p_strAlarmID, l_oTrans);
}

void CBusinessImpl::SetUpdateTime(std::string p_strUpdateTime, std::string p_strAlarmID)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		return;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	l_oTrans.oSourceInfo.m_oBody.m_oAlarm.m_strUpdateTime = p_strUpdateTime;
	AddTransportQue(p_strAlarmID, l_oTrans);
}

void CBusinessImpl::SetTraget(std::string p_strUserName, std::string p_strUserCode, std::string p_strAlarmID)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		return;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	l_oTrans.oSourceInfo.m_oBody.m_strTargetUserCode = p_strUserCode;
	l_oTrans.oSourceInfo.m_oBody.m_strTargetUserName = p_strUserName;
	AddTransportQue(p_strAlarmID, l_oTrans);
}

bool CBusinessImpl::GetTransCallInfo(std::string p_strAlarmID, PROTOCOL::CAlarmTransferCallRespond&p_oInfo)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
	{
		ICC_LOG_ERROR(m_pLog, "get transport transfercall failed,not exit[%s]", p_strAlarmID.c_str());
		return false;
	}
	CTransportQueue l_oTrans;
	l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
	p_oInfo = l_oTrans.oTransferCallInfo;
	return true;
}

bool CBusinessImpl::GetInfoByTimerName(std::string p_strTimerName, std::string &p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo)
{
	int l_iNext = 0;
	std::map<std::string, std::string> l_MapRe;
	while ((l_iNext = m_pRedisClient->HScan(MANGE_TRANSPORT_ALRM, l_iNext, l_MapRe)) > 0);
	bool l_bGet = false;
	auto iter = l_MapRe.begin();
	for (; iter != l_MapRe.end(); iter++)
	{
		CTransportQueue l_oTrans;
		l_oTrans.Parse(iter->second, ICCGetIJsonFactory());

		if (l_oTrans.strTimerName == p_strTimerName)
		{
			p_strAlarmID = l_oTrans.strAlarmID;
			std::string l_strInfo;
			if (!m_pRedisClient->HGet(MANGE_TRANSPORT_ALRM, p_strAlarmID, l_strInfo))
			{
				ICC_LOG_ERROR(m_pLog, "get transport alarminfo failed,not exit[%s]", p_strAlarmID.c_str());
				l_bGet = false;
			}
			CTransportQueue l_oTrans;
			l_oTrans.Parse(l_strInfo, ICCGetIJsonFactory());
			p_oInfo = l_oTrans.oSourceInfo;
			l_bGet = true;
		}
	}
	return l_bGet;
}

bool CBusinessImpl::UpdateCarInfo(const std::string& strSourceAlarmID, const std::string& strNewAlarmID)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_car_info_to_new_alarm_id";
	l_SQLRequest.param["new_alarm_id"] = strNewAlarmID;
	l_SQLRequest.param["source_alarm_id"] = strSourceAlarmID;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "update car info failed!source alarm id:[%s],new alarm id:[%s]", strSourceAlarmID.c_str(), strNewAlarmID.c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::_BuildDeptInfo(const std::string& strDeptCode, std::string& strDeptName, std::string& strDeptDistrictCode)
{
	std::string strDeptInfo;
	if (!m_pRedisClient->HGet("DeptCodeInfoKey", strDeptCode, strDeptInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "hget DeptCodeInfoKey failed,deptcode:[%s]", strDeptCode.c_str());
		return false;
	}
	PROTOCOL::CDeptInfo l_DeptInfo;
	if (!l_DeptInfo.Parse(strDeptInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse dept info  failed,dept_info:[%s]", strDeptInfo.c_str());
		return false;
	}
	strDeptName = l_DeptInfo.m_strName;
	strDeptDistrictCode = l_DeptInfo.m_strPucOrgIdentifier;

	return true;
}

std::string CBusinessImpl::_IsExistAdminDept(const std::string& strAlarmID)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_jjdb";
	l_tSQLRequest.param["id"] = strAlarmID;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "exec sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}
	if (l_pResult->Next())
	{
		return l_pResult->GetValue("admin_dept_code");
	}
	return "";
}


bool CBusinessImpl::_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo)
{
	std::string strStaffInfo;
	if (m_strCodeMode == STAFF_ID_NO)
	{
		if (!m_pRedisClient->HGet("StaffIdInfo", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffIdInfo failed,staff_id_no:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	else if (m_strCodeMode == STAFF_CODE)
	{
		if (!m_pRedisClient->HGet("StaffInfoMap", strStaffCode, strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed,staff_code:[%s]", strStaffCode.c_str());
			return false;
		}
	}
	if (!l_oStaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse staff info failed!!!");
		return false;
	}
	return true;
}

std::string	CBusinessImpl::_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName)
{
	std::string strEndStaffName;
	if (strStaffType == "JZLX101")
	{
		strEndStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), strStaffName.c_str());
	}
	else
	{
		strEndStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), strStaffName.c_str());
	}
	return strEndStaffName;
}

std::string CBusinessImpl::_GetStaffCodeBySeat(const::std::string& strSeatNo)
{
	std::string l_strClientRegisterInfo;
	if (!m_pRedisClient->HGet("ClientRegisterInfo", strSeatNo, l_strClientRegisterInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ClientRegisterInfo Code failed!!!");
		return "";
	}
	CRegisterInfo l_oClientRegisterInfo;
	if (!l_oClientRegisterInfo.Parse(l_strClientRegisterInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse staff info failed");
		return "";
	}
	return l_oClientRegisterInfo.m_strStaffCode;
}

