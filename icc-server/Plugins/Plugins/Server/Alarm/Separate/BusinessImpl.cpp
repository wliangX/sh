#include "Boost.h"
#include "BusinessImpl.h"
#include "AlarmMagager.h"
#include "SeatMagager.h"


#define SYSTEMID	("ICC")
#define SUBSYSTEMID ("Alarm-Receipt")

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	Init();
	
}

void CBusinessImpl::Init()
{
	// 初始化接警状态顺序
	m_mapReceiptStateOrder[ALARM_STATUS_HANDLING] = 1;
	m_mapReceiptStateOrder[ALARM_STATUS_HANDLED] = 2;
	m_mapReceiptStateOrder[ALARM_STATUS_PROCESSED] = 3;
	m_mapReceiptStateOrder[ALARM_STATUS_DISPATCHED] = 4;
	m_mapReceiptStateOrder[ALARM_STATUS_ARRIVED] = 5;
	m_mapReceiptStateOrder[ALARM_STATUS_FEEDBACK] = 6;
	m_mapReceiptStateOrder[ALARM_STATUS_CLOSED] = 7;
	m_mapReceiptStateOrder[ALARM_STATUS_TERMINAL] = 8;	
	m_mapReceiptStateOrder[ALARM_STATUS_PUTFILED] = 11;
	m_mapReceiptStateOrder[ALARM_STATUS_TRANS] = 12;


	//m_mapReceiptStateOrder["00"] = 1;
	//m_mapReceiptStateOrder["01"] = 2;
	////m_mapReceiptStateOrder["02"] = 3;
	//m_mapReceiptStateOrder["02"] = 4;
	//m_mapReceiptStateOrder["DIC019050"] = 5;
	//m_mapReceiptStateOrder["DIC019060"] = 6;
	//m_mapReceiptStateOrder["DIC019070"] = 7;
	//m_mapReceiptStateOrder["DIC019080"] = 8;
	//m_mapReceiptStateOrder["05"] = 9;
	//m_mapReceiptStateOrder["06"] = 10;
	//m_mapReceiptStateOrder["DIC019110"] = 11;
	//m_mapReceiptStateOrder["DIC019120"] = 12;

	/*m_mapReceiptStateOrder["DIC019010"] = 1;
	m_mapReceiptStateOrder["DIC019020"] = 2;
	m_mapReceiptStateOrder["DIC019030"] = 3;
	m_mapReceiptStateOrder["DIC019040"] = 4;
	m_mapReceiptStateOrder["DIC019050"] = 5;
	m_mapReceiptStateOrder["DIC019060"] = 6;
	m_mapReceiptStateOrder["DIC019070"] = 7;
	m_mapReceiptStateOrder["DIC019080"] = 8;
	m_mapReceiptStateOrder["DIC019090"] = 9;
	m_mapReceiptStateOrder["DIC019100"] = 10;
	m_mapReceiptStateOrder["DIC019110"] = 11;
	m_mapReceiptStateOrder["DIC019120"] = 12;*/

	// 初始化处警状态顺序
	m_mapProcessStateOrder[PROCESS_STATUS_HANDLING] = 0;
	m_mapProcessStateOrder[PROCESS_STATUS_TAKEN] = 1;
	m_mapProcessStateOrder[PROCESS_STATUS_ASSGIN] = 2;	//已下达	
	m_mapProcessStateOrder[PROCESS_STATUS_SIGIN] = 3;	//已接收
	m_mapProcessStateOrder[PROCESS_STATUS_ARRIVED] = 4;
	m_mapProcessStateOrder[PROCESS_STATUS_CLOSED] = 5;
	m_mapProcessStateOrder[PROCESS_STATUS_ARCHIVED] = 6;
	m_mapProcessStateOrder[PROCESS_STATUS_CANCEL] = 7;
	m_mapProcessStateOrder[PROCESS_STATUS_RETURN] = 8;

	/*m_mapProcessStateOrder["02"] = 1;
	m_mapProcessStateOrder["DIC020020"] = 2;
	m_mapProcessStateOrder["DIC020030"] = 3;
	m_mapProcessStateOrder["DIC020040"] = 4;
	m_mapProcessStateOrder["DIC020050"] = 5;
	m_mapProcessStateOrder["DIC020060"] = 6;
	m_mapProcessStateOrder["DIC020061"] = 7;
	m_mapProcessStateOrder["DIC020070"] = 8;
	m_mapProcessStateOrder["DIC020071"] = 9;
	m_mapProcessStateOrder["DIC020072"] = 10;
	m_mapProcessStateOrder["DIC020073"] = 11;*/

	/*m_mapProcessStateOrder["DIC020010"] = 1;
	m_mapProcessStateOrder["DIC020020"] = 2;
	m_mapProcessStateOrder["DIC020030"] = 3;
	m_mapProcessStateOrder["DIC020040"] = 4;
	m_mapProcessStateOrder["DIC020050"] = 5;
	m_mapProcessStateOrder["DIC020060"] = 6;
	m_mapProcessStateOrder["DIC020061"] = 7;
	m_mapProcessStateOrder["DIC020070"] = 8;
	m_mapProcessStateOrder["DIC020071"] = 9;
	m_mapProcessStateOrder["DIC020072"] = 10;
	m_mapProcessStateOrder["DIC020073"] = 11;*/

	CSeatManager::Instance()->Init(GetResourceManager());
	CAlarmQueue::Instance()->Init(GetResourceManager());


	m_iMaxCacheCount = 1000;
}

void CBusinessImpl::OnStart()
{
	m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_request", OnCNotifiAddOrUpdateAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_assign_request", OnNotifiAlarmAssignRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "process_done_request", OnNotifiProcessDoneRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "process_again_request", OnNotifiProcessAgainRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "edit_again_request", OnNotifiEditAgainRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_process_cash_request", OnNotifiGetProcessCashRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	ICC_LOG_DEBUG(m_pLog, "alarmreceipt start success,AssistantPolice = %s, Police = %s, ", m_strAssistantPolice.c_str(), m_strPolice.c_str());

	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, 2, 5);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "alarmreceipt stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiAddOrUpdateAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	PROTOCOL::CAddOrUpdateAlarmRequest l_oRequest;
	PROTOCOL::CAddOrUpdateAlarmRespond l_oRespond;
	BuildRespondHeader("add_or_update_alarm_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	if (l_strRecvMsg.empty() || !l_oRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_WARNING(m_pLog, "request string is empty or syntax error.[%s]", l_strRecvMsg.c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strRespondMsg);
		return;
	}

	std::string l_strTransGuid = m_pDBConn->BeginTransaction();
	bool l_bResult = false;
	if (l_oRequest.m_oBody.m_oAlarm.m_strID.empty())
	{
		l_bResult = AddAlarmInfo(l_oRequest, l_strTransGuid);
	}
	else
	{
		l_bResult = UpdateAlarmInfo(l_oRequest, l_strTransGuid);
	}

	if (!l_bResult)
	{
		m_pDBConn->Rollback(l_strTransGuid);
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "commit alarm to db failed,[%s] ", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		return;
	}

	m_pDBConn->Commit(l_strTransGuid);
	
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oRequest.m_oBody.m_oAlarm.m_strReceiptName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oRequest.m_oBody.m_oAlarm.m_strReceiptName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode);
	l_vecParamList.push_back(l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName);

	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_oAlarm.m_strID;
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oRequest.m_oBody.m_oAlarm.m_strReceiptName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_ALARM_SUBMIT;
	AddAlarmLogInfo(l_oAlarmLogInfo);

	//加入接警队列
	CAlarmQueue::Instance()->AddAlarm(l_oRequest);

	l_oRespond.m_oBody.m_strResult = l_bResult ? "0" : "1";
	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));	
	p_pNotify->Response(l_strRespondMsg);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());
}

void CBusinessImpl::OnNotifiAlarmAssignRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmAssignRequest l_oRequest;
	// 构造回复
	PROTOCOL::CAlarmAssignRespond l_oRespond;
	BuildRespondHeader("alarm_assign_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

		return;
	}

	if (l_oRequest.m_oBody.m_strSrcSeat != l_oRequest.m_oBody.m_strDescSeat)
	{
		//不是自己提交给自己的，获取该席位的缓存数量
		if (!l_oRequest.m_oBody.m_strDescSeat.empty())
		{
			int l_iCacheCount = CSeatManager::Instance()->GetCacheCount(l_oRequest.m_oBody.m_strDescSeat);
			if (l_iCacheCount > m_iMaxCacheCount - 1)
			{
				ICC_LOG_ERROR(m_pLog, "overload max process count seat[%s]", l_oRequest.m_oBody.m_strDescSeat.c_str());
				l_oRespond.m_oBody.m_strResult = "1";
				std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
				p_pNotify->Response(l_strRespondMsg);
				ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

				return;
			}

			ICC_LOG_DEBUG(m_pLog, "check seat process count [%s]-[%d]", l_oRequest.m_oBody.m_strDescSeat.c_str(), l_iCacheCount);		
		}
	}


	PROTOCOL::CAddOrUpdateAlarmRequest l_oAlarm;
	if (!GetAlarm(l_oRequest.m_oBody.m_strAlarmID, l_oAlarm))
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "can not find alarm info,[%s] ", l_oRequest.m_oBody.m_strAlarmID.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "get alarm info info success[%s]", l_oRequest.m_oBody.m_strAlarmID.c_str());

	//分配警单
	AlarmAssign(l_oAlarm, l_oRequest.m_oBody.m_strSrcSeat, l_oRequest.m_oBody.m_strDescSeat);

	l_oRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

	//接警暂存删除
	CAlarmQueue::Instance()->DelAlarm(l_oRequest.m_oBody.m_strAlarmID);
}


//获取处警缓存
void CBusinessImpl::OnNotifiGetProcessCashRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetProcessCashRequest l_oRequest;
	PROTOCOL::CGetProcessCashRespond l_oRespond;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		std::string l_strRespondMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

		return;
	}

	// 构造回复
	BuildRespondHeader("get_process_cash_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	std::list<std::string> l_listCache;
	if (!CSeatManager::Instance()->GetCacheList(l_oRequest.m_oBody.m_strSeatNo, l_listCache))
	{
		std::string l_strRespondMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "can not find seat info,[%s] ", l_oRequest.m_oBody.m_strSeatNo.c_str());
		return;
	}

	if (l_listCache.empty())
	{
		//分配公共处警单到该处警席
		std::string l_strAlarmID;
		if (!AssignPubProcess(l_oRequest.m_oBody.m_strSeatNo, l_strAlarmID))
		{
			std::string l_strRespondMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strRespondMsg);
			ICC_LOG_ERROR(m_pLog, "assign to public alarm to seat failed, seat[%s] ", l_oRequest.m_oBody.m_strSeatNo.c_str());
			return;
		}

		l_listCache.push_back(l_strAlarmID);
	}

	for (auto var : l_listCache)
	{
		std::string l_strAlarmID = var;
		PROTOCOL::CAddOrUpdateAlarmRequest l_oAlarm;
		if (GetAlarm(l_strAlarmID, l_oAlarm))
		{
			if (0 == l_oAlarm.m_oBody.m_oAlarm.m_strPrivacy.compare("1"))
			{
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerNo = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerName = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerAddr = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerID = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerIDType = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strCallerGender = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strCallerAge = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strCallerBirthday = "******";
				l_oAlarm.m_oBody.m_oAlarm.m_strContactNo = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactName = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactAddr = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactID = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactIDType = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactGender = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactAge = "******";
				//l_oAlarm.m_oBody.m_oAlarm.m_strContactBirthday = "******";
			}

			PROTOCOL::CGetProcessCashRespond::ProcessCashed l_processCashed;
			l_processCashed.m_strSeatNo = l_oRequest.m_oBody.m_strSeatNo;
			l_processCashed.m_strAlarmID = l_strAlarmID;
			l_processCashed.m_strContent = l_oAlarm.m_oBody.m_oAlarm.m_strContent;
			l_processCashed.m_strCaller = l_oAlarm.m_oBody.m_oAlarm.m_strCallerNo;
			l_processCashed.m_strTime = l_oAlarm.m_oBody.m_oAlarm.m_strTime;
			l_processCashed.m_strHandleType = l_oAlarm.m_oBody.m_oAlarm.m_strHandleType;
			l_processCashed.m_strState = l_oAlarm.m_oBody.m_oAlarm.m_strState;

			l_oRespond.m_oBody.m_ProcessCasheds.push_back(l_processCashed);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "get process cash info failed, [%s]", l_strAlarmID.c_str());
		}
	}

	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());
}

//处警完成
void CBusinessImpl::OnNotifiProcessDoneRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CProcessDoneRequest l_oRequest;
	// 构造回复
	PROTOCOL::CProcessDoneRespond l_oRespond;
	BuildRespondHeader("process_done_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	bool l_bResult = false;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

		return;
	}

	SeatInfo l_Seatinfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(l_oRequest.m_oBody.m_strSeatNo, l_Seatinfo))
	{
		//找不到席位
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "can not find seat info,[%s] ", l_oRequest.m_oBody.m_strSeatNo.c_str());
		return;
	}

	//更新警情状态
	PROTOCOL::CAddOrUpdateAlarmRequest l_updateAlarm;
	l_updateAlarm.m_oBody.m_oAlarm.m_strID = l_oRequest.m_oBody.m_strAlarmID;
	l_updateAlarm.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLED;//ALARM_STATUS_PROCESSED;
	if (!UpdateDBAlarmInfo(l_updateAlarm))
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,[%s] ", l_oRequest.m_oBody.m_strAlarmID.c_str());
		return;
	}

	//写流水,记录警情流水_完成处警单
	//向数据库添加接警流水并发送通知
	/* 不写BS001002002流水 2022-9-13
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;

	l_vecParamList.push_back(l_Seatinfo.m_strUserName);
	l_vecParamList.push_back(l_Seatinfo.m_strUserCode);
	l_vecParamList.push_back(l_Seatinfo.m_strDeptName);

	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_Seatinfo.m_strUserName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_PROCESS_COMPLETED;
	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	
	AddAlarmLogInfo(l_oAlarmLogInfo);
	*/
	CSeatManager::Instance()->DeleteCache(l_oRequest.m_oBody.m_strSeatNo, l_oRequest.m_oBody.m_strAlarmID);	//删除暂存单

	//分配公共处警单到该处警席
	std::string l_strAlarmID;
	bool l_isSuccess = AssignPubProcess(l_oRequest.m_oBody.m_strSeatNo, l_strAlarmID);
	if (l_isSuccess)
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	
	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

	GetAlarm(l_updateAlarm.m_oBody.m_oAlarm.m_strID, l_updateAlarm);
	SyncAlarmInfo(l_updateAlarm, ESyncType::EDIT);
}

// 再处警
void CBusinessImpl::OnNotifiProcessAgainRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CProcessAgainRequest l_oRequest;
	// 构造回复
	PROTOCOL::CProcessAgainRespond l_oRespond;
	BuildRespondHeader("process_again_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString ProcessAgain Request Failed");

		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

		return;
	}

	SeatInfo l_Seatinfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(l_oRequest.m_oBody.m_strSeatNo, l_Seatinfo))
	{
		//找不到席位
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "can not find seat info,[%s] ", l_oRequest.m_oBody.m_strSeatNo.c_str());
		return;
	}

	//更新警情状态
	PROTOCOL::CAddOrUpdateAlarmRequest l_updateAlarm;
	l_updateAlarm.m_oBody.m_oAlarm.m_strID = l_oRequest.m_oBody.m_strAlarmID;
	l_updateAlarm.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLED;//ALARM_STATUS_PROCESSED;//ALARM_STATUS_PROCESSING;

	if (!UpdateDBAlarmInfo(l_updateAlarm, "", true))
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,[%s] ", l_oRequest.m_oBody.m_strAlarmID.c_str());
		return;
	}
	
	//写流水,记录警情流水_完成处警单
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_Seatinfo.m_strStaffCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_Seatinfo.m_strUserName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_Seatinfo.m_strUserName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_Seatinfo.m_strStaffCode);
	l_vecParamList.push_back(l_Seatinfo.m_strDeptName);

	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_Seatinfo.m_strDeptName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_PROCESS_AGAIN;

	AddAlarmLogInfo(l_oAlarmLogInfo);

	l_oRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strRespondMsg.c_str());

	GetAlarm(l_updateAlarm.m_oBody.m_oAlarm.m_strID, l_updateAlarm);
	SyncAlarmInfo(l_updateAlarm, ESyncType::EDIT);
}

// 重新编辑无效警情
void CBusinessImpl::OnNotifiEditAgainRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive EditAgain Request Message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CEditAgainRequest l_oRequest;
	// 构造回复
	PROTOCOL::CEditAgainRespond l_oRespond;
	BuildRespondHeader("edit_again_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString EditAgain Request Failed");

		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		return;
	}

	SeatInfo l_Seatinfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(l_oRequest.m_oBody.m_strSeatNo, l_Seatinfo))
	{
		//找不到席位
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "can not find seat info,[%s] ", l_oRequest.m_oBody.m_strSeatNo.c_str());
		return;
	}

	//更新警情状态
	PROTOCOL::CAddOrUpdateAlarmRequest l_updateAlarm;
	l_updateAlarm.m_oBody.m_oAlarm.m_strID = l_oRequest.m_oBody.m_strAlarmID;
	l_updateAlarm.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLING;

	if (!UpdateDBAlarmInfo(l_updateAlarm, "", true, true))
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strRespondMsg);
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,[%s] ", l_oRequest.m_oBody.m_strAlarmID.c_str());
		return;
	}

	//写流水,记录警情流水_完成处警单
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_Seatinfo.m_strStaffCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), strStaffName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), strStaffName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_Seatinfo.m_strStaffCode);
	l_vecParamList.push_back(l_Seatinfo.m_strDeptName);

	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_Seatinfo.m_strDeptName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_ALARM_EDIT_AGAIN;

	AddAlarmLogInfo(l_oAlarmLogInfo);

	l_oRespond.m_oBody.m_strResult = "0";
	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);

	GetAlarm(l_updateAlarm.m_oBody.m_oAlarm.m_strID, l_updateAlarm);
	SyncAlarmInfo(l_updateAlarm, ESyncType::EDIT);
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{
	//自动分配警单给处警席
	PubAlarmList l_pubProcessList;
	if (!CSeatManager::Instance()->GetAllPubCache(l_pubProcessList))
	{		
		return;
	}

	for (auto var : l_pubProcessList)
	{
		PROTOCOL::CAddOrUpdateAlarmRequest l_alarm;
		if (GetAlarm(var, l_alarm))
		{
			std::string l_strSeatNo = CSeatManager::Instance()->GetFreeProcessSeat(l_alarm.m_oBody.m_oAlarm.m_strReceiptDeptCode);
			if (!l_strSeatNo.empty())
			{
				AssignToSeat(l_strSeatNo, var);
			}
		}
	}
}

void CBusinessImpl::BuildRespondHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
	p_pRespHeader.m_strResponse = "";
	p_pRespHeader.m_strResponseType = "";
}

bool CBusinessImpl::AddAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oAddAlarmInfo, std::string p_strTransGuid)
{
	//向数据库添加接警记录
	l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strID = m_pDateTime->CreateSerial();
	l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strCreateUser = l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;
	l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	//TODO::记录警情流水_受理接警单	
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strID;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode);
	l_vecParamList.push_back(l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName);
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oAddAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_RECEIPT_ACCEPTED;
	if (!AddAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_RECEIPT_ACCEPTED failed!");
		return false;
	}

	if (!InsertDBAlarmInfo(l_oAddAlarmInfo, p_strTransGuid))
	{
		//向数据库插入接警记录失败
		//回复失败消息
		return false;
	}

	//TODO::记录警情流水_完成接警信息录入
	//向数据库添加接警流水并发送通知
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_RECEIPT_RECORDED;
	if (!AddAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_RECEIPT_RECORDED failed!");
		return false;
	}

	//发送同步数据
	SyncAlarmInfo(l_oAddAlarmInfo, ESyncType::ADD);
	return true;
}

bool CBusinessImpl::UpdateAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& l_oUpdateAlarmInfo, std::string p_strTransGuid)
{
	std::string l_strCurTime(m_pDateTime->CurrentDateTimeStr());
	l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strUpdateUser = l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;
	l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strUpdateTime = l_strCurTime;
	//更新前备份该接警数据
	DataBase::SQLRequest l_oSQLReq;
	std::string l_strReceiptResourceID = m_pString->CreateGuid();
	
	if (InsertAlarmBookBeforeUpdate(l_oUpdateAlarmInfo, l_strReceiptResourceID, p_strTransGuid))
	{
		//更新该接警数据
		if (!UpdateDBAlarmInfo(l_oUpdateAlarmInfo, p_strTransGuid))
		{
			//更新数据库数据失败,不作其他操作
			
			return false;
		}
		else
		{

			//接警流水
			PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strID;
			std::vector<std::string> l_vecParamList;
			if (!m_pRedisClient->HGet("StaffInfoMap", l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode, l_strStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
			}
			l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
			strStaffType = l_oStaffInfo.m_strType;
			if (strStaffType == "JZLX101")
			{
				strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName.c_str());
			}
			else
			{
				strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName.c_str());
			}
			l_vecParamList.push_back(strStaffName);
			l_vecParamList.push_back(l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode);
			l_vecParamList.push_back(l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName);
			l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oUpdateAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;
			//l_oAlarmLogInfo.m_oBody.m_strResourceID = l_strReceiptResourceID;
			l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_ALARM_EDIT;
			if (!AddAlarmLogInfo(l_oAlarmLogInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_ALARM_EDIT failed!");
				return false;
			}
		}
	}

	SyncAlarmInfo(l_oUpdateAlarmInfo, ESyncType::EDIT);

	return true;
}

bool CBusinessImpl::InsertDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string strTransGuid)
{
	p_pAlarmInfo.m_oBody.m_oAlarm.m_strState = RealReceiptState(p_pAlarmInfo.m_oBody.m_oAlarm.m_strID, p_pAlarmInfo.m_oBody.m_oAlarm.m_strState, "02");//"DIC019030";

	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_jjdb";

	l_tSQLReqInsertAlarm.param["id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strID;
	l_tSQLReqInsertAlarm.param["merge_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strMergeID;
	l_tSQLReqInsertAlarm.param["title"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strTitle;
	l_tSQLReqInsertAlarm.param["content"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContent;
	l_tSQLReqInsertAlarm.param["time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strTime;
	//l_tSQLReqInsertAlarm.param["actual_occur_time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualOccurTime;
	l_tSQLReqInsertAlarm.param["addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAddr;
	l_tSQLReqInsertAlarm.param["longitude"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLongitude;
	l_tSQLReqInsertAlarm.param["latitude"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLatitude;
	l_tSQLReqInsertAlarm.param["state"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strState;
	l_tSQLReqInsertAlarm.param["level"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLevel;
	l_tSQLReqInsertAlarm.param["source_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceType;
	l_tSQLReqInsertAlarm.param["source_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceID;
	l_tSQLReqInsertAlarm.param["handle_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strHandleType;
	l_tSQLReqInsertAlarm.param["first_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFirstType;
	l_tSQLReqInsertAlarm.param["second_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSecondType;
	l_tSQLReqInsertAlarm.param["third_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strThirdType;
	l_tSQLReqInsertAlarm.param["fourth_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFourthType;
	l_tSQLReqInsertAlarm.param["vehicle_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleNo;
	l_tSQLReqInsertAlarm.param["vehicle_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleType;
	l_tSQLReqInsertAlarm.param["symbol_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolCode;
	//l_tSQLReqInsertAlarm.param["symbol_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolAddr;
	//l_tSQLReqInsertAlarm.param["fire_building_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFireBuildingType;

	//l_tSQLReqInsertAlarm.param["event_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strEventType;

	l_tSQLReqInsertAlarm.param["called_no_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCalledNoType;
	//l_tSQLReqInsertAlarm.param["actual_called_no_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualCalledNoType;

	l_tSQLReqInsertAlarm.param["caller_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerNo;
	l_tSQLReqInsertAlarm.param["caller_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerName;
	l_tSQLReqInsertAlarm.param["caller_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAddr;
	l_tSQLReqInsertAlarm.param["caller_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerID;
	l_tSQLReqInsertAlarm.param["caller_id_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerIDType;
	l_tSQLReqInsertAlarm.param["caller_gender"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerGender;
	//l_tSQLReqInsertAlarm.param["caller_age"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAge;
	//l_tSQLReqInsertAlarm.param["caller_birthday"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerBirthday;

	l_tSQLReqInsertAlarm.param["contact_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactNo;
	//l_tSQLReqInsertAlarm.param["contact_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactName;
	//l_tSQLReqInsertAlarm.param["contact_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAddr;
	//l_tSQLReqInsertAlarm.param["contact_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactID;
	//l_tSQLReqInsertAlarm.param["contact_id_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactIDType;
	//l_tSQLReqInsertAlarm.param["contact_gender"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactGender;
	//l_tSQLReqInsertAlarm.param["contact_age"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAge;
	//l_tSQLReqInsertAlarm.param["contact_birthday"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactBirthday;

	//l_tSQLReqInsertAlarm.param["admin_dept_district_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode;
	l_tSQLReqInsertAlarm.param["admin_dept_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptCode;
	l_tSQLReqInsertAlarm.param["admin_dept_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptName;

	l_tSQLReqInsertAlarm.param["receipt_dept_district_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName;
	//l_tSQLReqInsertAlarm.param["leader_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderCode;
	//l_tSQLReqInsertAlarm.param["leader_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderName;
	l_tSQLReqInsertAlarm.param["receipt_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode;
	l_tSQLReqInsertAlarm.param["receipt_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;

	//l_tSQLReqInsertAlarm.param["dispatch_suggestion"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strDispatchSuggestion;

	l_tSQLReqInsertAlarm.param["create_user"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCreateUser;
	l_tSQLReqInsertAlarm.param["create_time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCreateTime;

	l_tSQLReqInsertAlarm.param["is_privacy"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strPrivacy;
	l_tSQLReqInsertAlarm.param["remark"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strRemark;

	l_tSQLReqInsertAlarm.param["is_invalid"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strIsInvalid;
	l_tSQLReqInsertAlarm.param["is_merge"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strIsMerge;
	l_tSQLReqInsertAlarm.param["is_visitor"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strIsVisitor;
	l_tSQLReqInsertAlarm.param["is_feedback"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strIsFeedBack;
	l_tSQLReqInsertAlarm.param["is_delete"] = "0";
	l_tSQLReqInsertAlarm.param["had_push"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strHadPush;
	l_tSQLReqInsertAlarm.param["is_over"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strIsOver;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateDBAlarmInfo(PROTOCOL::CAddOrUpdateAlarmRequest& p_pAlarmInfo, std::string p_strTransGuid /* = "" */, bool p_isForce, bool p_isEditAgain)
{
	if (!p_isForce)
	{
		//默认走这里，但是由于再处警的原因，客户端要求状态可以回退，这里需要注意后续会不会有问题
		p_pAlarmInfo.m_oBody.m_oAlarm.m_strState = RealReceiptState(p_pAlarmInfo.m_oBody.m_oAlarm.m_strID, p_pAlarmInfo.m_oBody.m_oAlarm.m_strState, "02");//"DIC019030";
	}
	
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "update_icc_t_jjdb";

	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strMergeID.empty())
	{
		l_tSQLReqInsertAlarm.set["merge_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strMergeID;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strTitle.empty())
	{
		l_tSQLReqInsertAlarm.set["lable"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strTitle;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContent.empty())
	{
		l_tSQLReqInsertAlarm.set["content"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContent;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strTime.empty())
	{
		l_tSQLReqInsertAlarm.set["receiving_time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strTime;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualOccurTime.empty())
	{
		l_tSQLReqInsertAlarm.set["actual_occur_time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualOccurTime;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAddr;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["longitude"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLongitude;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["latitude"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLatitude;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strState.empty())
	{
		l_tSQLReqInsertAlarm.set["state"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strState;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strLevel.empty())
	{
		l_tSQLReqInsertAlarm.set["level"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLevel;
	}	
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceType.empty())
	{
		l_tSQLReqInsertAlarm.set["source_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceID.empty())
	{
		l_tSQLReqInsertAlarm.set["source_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSourceID;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strHandleType.empty())
	{
		l_tSQLReqInsertAlarm.set["handle_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strHandleType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strFirstType.empty())
	{
		//如果非重新编辑且是无效报警，设置为已终结		
		if (!p_isEditAgain && p_pAlarmInfo.m_oBody.m_oAlarm.m_strFirstType == "DIC003011")
		{
			l_tSQLReqInsertAlarm.set["state"] = "06";
		}
		l_tSQLReqInsertAlarm.set["first_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFirstType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strSecondType.empty())
	{
		l_tSQLReqInsertAlarm.set["second_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSecondType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strThirdType.empty())
	{
		l_tSQLReqInsertAlarm.set["third_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strThirdType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strFourthType.empty())
	{
		l_tSQLReqInsertAlarm.set["fourth_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFourthType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleNo.empty())
	{
		l_tSQLReqInsertAlarm.set["vehicle_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleNo;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleType.empty())
	{
		l_tSQLReqInsertAlarm.set["vehicle_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strVehicleType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolCode.empty())
	{
		l_tSQLReqInsertAlarm.set["symbol_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolCode;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["symbol_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strSymbolAddr;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strFireBuildingType.empty())
	{
		l_tSQLReqInsertAlarm.set["fire_building_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strFireBuildingType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strEventType.empty())
	{
		l_tSQLReqInsertAlarm.set["event_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strEventType;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCalledNoType.empty())
	{
		l_tSQLReqInsertAlarm.set["called_no_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCalledNoType;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualCalledNoType.empty())
	{
		l_tSQLReqInsertAlarm.set["actual_called_no_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strActualCalledNoType;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerNo.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerNo;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerName.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerName;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAddr;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerID.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerID;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerIDType.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_id_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerIDType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerGender.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_gender"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerGender;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAge.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_age"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerAge;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerBirthday.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_birthday"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strCallerBirthday;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactNo.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_no"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactNo;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactName.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactName;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_addr"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAddr;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactID.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactID;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactIDType.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_id_type"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactIDType;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactGender.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_gender"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactGender;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAge.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_age"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactAge;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactBirthday.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_birthday"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strContactBirthday;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_district_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptCode;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strAdminDeptName;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_district_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptCode;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderCode.empty())
	{
		l_tSQLReqInsertAlarm.set["leader_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderCode;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderName.empty())
	{
		l_tSQLReqInsertAlarm.set["leader_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strLeaderName;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_code"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_name"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;
	}
	/*if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strDispatchSuggestion.empty())
	{
		l_tSQLReqInsertAlarm.set["dispatch_suggestion"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strDispatchSuggestion;
	}*/
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strUpdateUser.empty())
	{
		l_tSQLReqInsertAlarm.set["update_user"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strUpdateUser;
	}
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strUpdateTime.empty())
	{
		l_tSQLReqInsertAlarm.set["update_time"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strUpdateTime;
	}
	
	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strPrivacy.empty())
	{
		l_tSQLReqInsertAlarm.set["is_privacy"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strPrivacy;
	}

	if (!p_pAlarmInfo.m_oBody.m_oAlarm.m_strRemark.empty())
	{
		l_tSQLReqInsertAlarm.set["remark"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strRemark;
	}

	l_tSQLReqInsertAlarm.param["id"] = p_pAlarmInfo.m_oBody.m_oAlarm.m_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_pAlarmInfo.m_oBody.m_oAlarm.m_strID);
	if (strTime != "")
	{
		l_tSQLReqInsertAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLReqInsertAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

std::string CBusinessImpl::RealReceiptState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid /* = "" */)
{
	// p_strCurState与p_strInputState比较顺序，返回顺序值较大的字符串
	std::string l_strState;
	if (p_strCurState.empty())
	{
		l_strState = p_strInputState;
	}
	else
	{
		l_strState = (m_mapReceiptStateOrder[p_strCurState] > m_mapReceiptStateOrder[p_strInputState])
			? p_strCurState : p_strInputState;
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strState;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return l_strState;
	}

	std::string l_strPreState = l_pResult->GetValue("state");

	// 数据库中的state值与p_strState比较顺序
	if (!l_strPreState.empty())
	{
		if (m_mapReceiptStateOrder[l_strPreState] > m_mapReceiptStateOrder[l_strState])
		{
			return l_strPreState;
		}
	}
	return l_strState;
}


std::string CBusinessImpl::_GetCurrentProcessState(const std::string& p_strID, const std::string& p_strInputState, const std::string& strTransGuid)
{
	//如果请求消息有携带，以请求为准
	if (!p_strInputState.empty())
	{
		return p_strInputState;
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	//如果没有这返回01状态
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty, processid: %s", p_strID.c_str());
		return PROCESS_STATUS_TAKEN;
	}

	std::string tmp_strState(l_pResult->GetValue("state"));

	ICC_LOG_DEBUG(m_pLog, "processid: %s, state: %s", p_strID.c_str(), tmp_strState.c_str());
	return tmp_strState;
}

std::string CBusinessImpl::RealProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, std::string strTransGuid /* = "" */)
{
	// p_strCurState与p_strInputState比较顺序，返回顺序值较大的字符串
	std::string l_strState;
	if (p_strCurState.empty())
	{
		l_strState = p_strInputState;
	}
	else
	{
		l_strState = (m_mapProcessStateOrder[p_strCurState] > m_mapProcessStateOrder[p_strInputState])
			? p_strCurState : p_strInputState;
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strState;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return l_strState;
	}

	std::string l_strPreState = l_pResult->GetValue("state");

	// 当前状态为已取消，请求状态为已签收时返回失败
	if (l_strPreState.compare("DIC020071") == 0 && p_strCurState.compare("DIC020030") == 0)
	{
		return "";
	}

	// 数据库中的state值与p_strState比较顺序
	if (!l_strPreState.empty())
	{
		if (m_mapProcessStateOrder[l_strPreState] > m_mapProcessStateOrder[l_strState])
		{
			return l_strPreState;
		}
	}
	return l_strState;
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

bool CBusinessImpl::AddAlarmLogInfo(PROTOCOL::CAlarmLogSync& p_pAlarmLogInfo, const std::string& strTransGuid /* = "" */)
{
	PROTOCOL::CAlarmLogSync l_oData;
	//std::string l_strCurTime(m_pDateTime->CurrentDateTimeStr());
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	std::string l_strGUID(m_pString->CreateGuid());


	if (p_pAlarmLogInfo.m_oBody.m_strID.empty())
	{
		l_oData.m_oBody.m_strID = l_strGUID;
	}
	else
	{
		l_oData.m_oBody.m_strID = p_pAlarmLogInfo.m_oBody.m_strID;
	}
	l_oData.m_oBody.m_strAlarmID = p_pAlarmLogInfo.m_oBody.m_strAlarmID;
	l_oData.m_oBody.m_strOperate = p_pAlarmLogInfo.m_oBody.m_strOperate;
	l_oData.m_oBody.m_strOperateContent = p_pAlarmLogInfo.m_oBody.m_strOperateContent;
	l_oData.m_oBody.m_strCreateUser = p_pAlarmLogInfo.m_oBody.m_strCreateUser;
	l_oData.m_oBody.m_strCreateTime = l_strCurTime;
	l_oData.m_oBody.m_strSourceName = "icc";
	if (!InsertDBAlarmLogInfo(l_oData))
	{
		return false;
	}

	SyncAlarmLogInfo(l_oData);
	return true;
}

void CBusinessImpl::SyncAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmRequest& p_roAlarmSync, ESyncType p_iSyncType)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmSync.m_oHeader.m_strCmd = "alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	l_oAlarmSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);
	l_oAlarmSync.m_oBody.m_oAlarmInfo = p_roAlarmSync.m_oBody.m_oAlarm;
	/*l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMergeID = p_roAlarmSync.m_oBody.m_oAlarm.m_strMergeID;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSeatNo = p_roAlarmSync.m_oBody.m_oAlarm.m_strSeatNo;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strTitle = p_roAlarmSync.m_oBody.m_oAlarm.m_strTitle;
	l_oAlarmSync.m_oBody.m_strContent = p_roAlarmSync.m_oBody.m_oAlarm.m_strContent;
	l_oAlarmSync.m_oBody.m_strTime = p_roAlarmSync.m_oBody.m_oAlarm.m_strTime;
	l_oAlarmSync.m_oBody.m_strActualOccurTime = p_roAlarmSync.m_oBody.m_oAlarm.m_strActualOccurTime;
	l_oAlarmSync.m_oBody.m_strAddr = p_roAlarmSync.m_oBody.m_oAlarm.m_strAddr;
	l_oAlarmSync.m_oBody.m_strLongitude = p_roAlarmSync.m_oBody.m_oAlarm.m_strLongitude;
	l_oAlarmSync.m_oBody.m_strLatitude = p_roAlarmSync.m_oBody.m_oAlarm.m_strLatitude;
	l_oAlarmSync.m_oBody.m_strState = p_roAlarmSync.m_oBody.m_oAlarm.m_strState;
	l_oAlarmSync.m_oBody.m_strLevel = p_roAlarmSync.m_oBody.m_oAlarm.m_strLevel;
	l_oAlarmSync.m_oBody.m_strSourceType = p_roAlarmSync.m_oBody.m_oAlarm.m_strSourceType;
	l_oAlarmSync.m_oBody.m_strSourceID = p_roAlarmSync.m_oBody.m_oAlarm.m_strSourceID;
	l_oAlarmSync.m_oBody.m_strHandleType = p_roAlarmSync.m_oBody.m_oAlarm.m_strHandleType;
	l_oAlarmSync.m_oBody.m_strFirstType = p_roAlarmSync.m_oBody.m_oAlarm.m_strFirstType;
	l_oAlarmSync.m_oBody.m_strSecondType = p_roAlarmSync.m_oBody.m_oAlarm.m_strSecondType;
	l_oAlarmSync.m_oBody.m_strThirdType = p_roAlarmSync.m_oBody.m_oAlarm.m_strThirdType;
	l_oAlarmSync.m_oBody.m_strFourthType = p_roAlarmSync.m_oBody.m_oAlarm.m_strFourthType;

	l_oAlarmSync.m_oBody.m_strVehicleNo = p_roAlarmSync.m_oBody.m_oAlarm.m_strVehicleNo;
	l_oAlarmSync.m_oBody.m_strVehicleType = p_roAlarmSync.m_oBody.m_oAlarm.m_strVehicleType;
	l_oAlarmSync.m_oBody.m_strSymbolCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strSymbolCode;
	l_oAlarmSync.m_oBody.m_strSymbolAddr = p_roAlarmSync.m_oBody.m_oAlarm.m_strSymbolAddr;

	l_oAlarmSync.m_oBody.m_strFireBuildingType = p_roAlarmSync.m_oBody.m_oAlarm.m_strFireBuildingType;

	l_oAlarmSync.m_oBody.m_strEventType = p_roAlarmSync.m_oBody.m_oAlarm.m_strEventType;

	l_oAlarmSync.m_oBody.m_strCalledNoType = p_roAlarmSync.m_oBody.m_oAlarm.m_strCalledNoType;
	l_oAlarmSync.m_oBody.m_strActualCalledNoType = p_roAlarmSync.m_oBody.m_oAlarm.m_strActualCalledNoType;

	l_oAlarmSync.m_oBody.m_strCallerNo = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerNo;
	l_oAlarmSync.m_oBody.m_strCallerName = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerName;
	l_oAlarmSync.m_oBody.m_strCallerAddr = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerAddr;
	l_oAlarmSync.m_oBody.m_strCallerID = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerID;
	l_oAlarmSync.m_oBody.m_strCallerIDType = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerIDType;
	l_oAlarmSync.m_oBody.m_strCallerGender = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerGender;
	l_oAlarmSync.m_oBody.m_strCallerAge = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerAge;
	l_oAlarmSync.m_oBody.m_strCallerBirthday = p_roAlarmSync.m_oBody.m_oAlarm.m_strCallerBirthday;

	l_oAlarmSync.m_oBody.m_strContactNo = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactNo;
	l_oAlarmSync.m_oBody.m_strContactName = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactName;
	l_oAlarmSync.m_oBody.m_strContactAddr = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactAddr;
	l_oAlarmSync.m_oBody.m_strContactID = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactID;
	l_oAlarmSync.m_oBody.m_strContactIDType = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactIDType;
	l_oAlarmSync.m_oBody.m_strContactGender = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactGender;
	l_oAlarmSync.m_oBody.m_strContactAge = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactAge;
	l_oAlarmSync.m_oBody.m_strContactBirthday = p_roAlarmSync.m_oBody.m_oAlarm.m_strContactBirthday;

	l_oAlarmSync.m_oBody.m_strAdminDeptDistrictCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode;
	l_oAlarmSync.m_oBody.m_strAdminDeptCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strAdminDeptCode;
	l_oAlarmSync.m_oBody.m_strAdminDeptName = p_roAlarmSync.m_oBody.m_oAlarm.m_strAdminDeptName;

	l_oAlarmSync.m_oBody.m_strReceiptDeptDistrictCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;
	l_oAlarmSync.m_oBody.m_strReceiptDeptCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strReceiptDeptCode;
	l_oAlarmSync.m_oBody.m_strReceiptDeptName = p_roAlarmSync.m_oBody.m_oAlarm.m_strReceiptDeptName;
	l_oAlarmSync.m_oBody.m_strLeaderCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strLeaderCode;
	l_oAlarmSync.m_oBody.m_strLeaderName = p_roAlarmSync.m_oBody.m_oAlarm.m_strLeaderName;
	l_oAlarmSync.m_oBody.m_strReceiptCode = p_roAlarmSync.m_oBody.m_oAlarm.m_strReceiptCode;
	l_oAlarmSync.m_oBody.m_strReceiptName = p_roAlarmSync.m_oBody.m_oAlarm.m_strReceiptName;
	l_oAlarmSync.m_oBody.m_strSeatNo = p_roAlarmSync.m_oBody.m_oAlarm.m_strSeatNo;

	l_oAlarmSync.m_oBody.m_strDispatchSuggestion = p_roAlarmSync.m_oBody.m_oAlarm.m_strDispatchSuggestion;

	l_oAlarmSync.m_oBody.m_strCreateUser = p_roAlarmSync.m_oBody.m_oAlarm.m_strCreateUser;
	l_oAlarmSync.m_oBody.m_strCreateTime = p_roAlarmSync.m_oBody.m_oAlarm.m_strCreateTime;
	l_oAlarmSync.m_oBody.m_strUpdateUser = p_roAlarmSync.m_oBody.m_oAlarm.m_strUpdateUser;
	l_oAlarmSync.m_oBody.m_strUpdateTime = p_roAlarmSync.m_oBody.m_oAlarm.m_strUpdateTime;
	l_oAlarmSync.m_oBody.m_strPrivacy = p_roAlarmSync.m_oBody.m_oAlarm.m_strPrivacy;
	l_oAlarmSync.m_oBody.m_strRemark = p_roAlarmSync.m_oBody.m_oAlarm.m_strRemark;*/

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//接警分配,接警完成，进入处警阶段
bool CBusinessImpl::AlarmAssign(const PROTOCOL::CAddOrUpdateAlarmRequest p_oAlarm, std::string p_strSrcSeat, std::string p_strDescSeat)
{
	std::string l_strAlarmID = p_oAlarm.m_oBody.m_oAlarm.m_strID;
	if (l_strAlarmID.empty() || p_strSrcSeat.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: AlarmID[%s]", p_oAlarm.m_oBody.m_oAlarm.m_strID.c_str());
		return false;
	}

	SeatInfo l_SourceSeatinfo, l_DescSeatinfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(p_strSrcSeat, l_SourceSeatinfo))
	{
		ICC_LOG_ERROR(m_pLog, "can not find seat info,[%s] ", p_strSrcSeat.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get online src seat info [%s]", p_strSrcSeat.c_str());

	//处警单位编码
	std::string l_strDeptCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptCode;

	if (!p_strDescSeat.empty())
	{
		SeatInfo l_SeatInfo;
		if (!CSeatManager::Instance()->GetOnlineSeat(p_strDescSeat, l_SeatInfo))
		{
			p_strDescSeat = CSeatManager::Instance()->GetFreeProcessSeat(l_strDeptCode);	//获取一空闲处警席
		}
	}
	else
	{
		p_strDescSeat = CSeatManager::Instance()->GetFreeProcessSeat(l_strDeptCode);	//获取一空闲处警席
	}

	ICC_LOG_DEBUG(m_pLog, "get free seat success [%s]", p_strDescSeat.c_str());

	if (!p_strDescSeat.empty())
	{
		//分配处警单到处警席
		AssignToSeat(p_strDescSeat, l_strAlarmID);

		// 记录警情流水_分配处警单
		PROTOCOL::CAlarmLogSync l_oProcessAlarmLogInfo;
		std::vector<std::string> l_vecParamList;
		if (!m_pRedisClient->HGet("StaffInfoMap", l_SourceSeatinfo.m_strUserCode, l_strStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
		}
		l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
		strStaffType = l_oStaffInfo.m_strType;
		if (strStaffType == "JZLX101")
		{
			strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_SourceSeatinfo.m_strUserName.c_str());
		}
		else
		{
			strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_SourceSeatinfo.m_strUserName.c_str());
		}
		l_vecParamList.push_back(strStaffName);
		l_vecParamList.push_back(l_SourceSeatinfo.m_strUserCode);
		l_vecParamList.push_back(l_SourceSeatinfo.m_strDeptName);
		l_vecParamList.push_back(l_DescSeatinfo.m_strDeptName);
		l_oProcessAlarmLogInfo.m_oBody.m_strOperate = LOG_PROCESS_ASSIGNED;
		l_oProcessAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
		AddAlarmLogInfo(l_oProcessAlarmLogInfo);

		CSeatManager::Instance()->AddCache(p_strDescSeat, l_strAlarmID);
	}
	else
	{
		//没有空闲席位			
		CSeatManager::Instance()->AddPubCache(l_strDeptCode, l_strAlarmID);		//放入公共队列			
	}

	return true;
}

bool CBusinessImpl::InsertDBAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_AlarmLogInfo)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo.m_oBody, l_tSQLReqInsertAlarm))
	{
		ICC_LOG_ERROR(m_pLog, "set insert alarm log data failed");
		return false;
	}

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_oBody.m_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReqInsertAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLReqInsertAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_oBody.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_oBody.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_oBody.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_oBody.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_oBody.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_oBody.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_oBody.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_oBody.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_oBody.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_oBody.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_oBody.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_oBody.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_oBody.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_oBody.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_oBody.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_oBody.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_oBody.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_oBody.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_oBody.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_oBody.m_strOperateAttachDesc;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::InsertAlarmBookBeforeUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid /* = "" */)
{
	return true;
	//根据alarm_id从icc_t_alarm表查询警单
}

bool CBusinessImpl::InsertAlarmBookAfterUpdate(const PROTOCOL::CAddOrUpdateAlarmRequest &p_oAlarmInfo, std::string& p_strResourceID, std::string p_strTransGuid /* = "" */)
{
	return true;
	//警单备份到icc_t_alarm_book表中

}

bool CBusinessImpl::ExecSql(DataBase::SQLRequest p_oSQLReq)
{
	DataBase::IResultSetPtr l_pRSet;
	l_pRSet = m_pDBConn->Exec(p_oSQLReq);

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecSql Error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 新增处警信息
bool CBusinessImpl::AddProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oAddProcessInfo, std::string p_strTransGuid /* = "" */)
{
	//向数据库添加处警记录
	l_oAddProcessInfo.m_strID = m_pDateTime->CreateSerial();
	l_oAddProcessInfo.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;
	l_oAddProcessInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	//TODO::记录警情流水_分配处警单
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oAddProcessInfo.m_strID;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_oAddProcessInfo.m_strDispatchCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oAddProcessInfo.m_strDispatchName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oAddProcessInfo.m_strDispatchName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchCode);
	l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchDeptName);
	l_vecParamList.push_back(l_oAddProcessInfo.m_strProcessDeptName);
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_PROCESS_ASSIGNED;
	if (!AddAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log failed!");
	}

	if (!InsertDBProcessInfo(l_oAddProcessInfo))
	{
		//向数据库插入处警记录失败
		//回复失败消息
		return false;
	}


	//TODO::记录警情流水_完成处警单		
	//向数据库添加接警流水并发送通知
	/* 不写BS001002002流水 2022-9-13
	l_vecParamList.clear();
	l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchName);
	l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchCode);
	l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchDeptName);
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;
	l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_PROCESS_COMPLETED;
	if (!AddAlarmLogInfo(l_oAlarmLogInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_PROCESS_COMPLETED failed!");
	}
	*/

	//发送同步数据
	SyncProcessInfo(l_oAddProcessInfo, ESyncType::ADD);
	return true;

}

bool CBusinessImpl::_InsertBackReason(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "insert_icc_t_backalarm_reason";
	l_tSQLRequest.param["guid"] = m_pString->CreateGuid();
	l_tSQLRequest.param["process_id"] = l_oUpdateProcessInfo.m_strID;
	l_tSQLRequest.param["back_reason"] = l_oUpdateProcessInfo.m_strCancelReason;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false);
	ICC_LOG_DEBUG(m_pLog, "icc Separate insert cancel reason sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "icc Separate insert cancel reason failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 更新处警信息
bool CBusinessImpl::UpdateProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo, std::string p_strTransGuid /* = "" */)
{
	//VCS过来的使用VCS派警单的状态
	std::string l_strState(l_oUpdateProcessInfo.m_strState);
	
	if ("vcs_relocated" != l_oUpdateProcessInfo.m_strMsgSource && "vcs" != l_oUpdateProcessInfo.m_strMsgSource)
	{
		// 如果对已签收或者已反馈的处警单进行取消调派操作，则返回false
		//l_strState = RealProcessState(l_oUpdateProcessInfo.m_strID, l_oUpdateProcessInfo.m_strState, "02", ""); //"DIC020010";
		l_strState = _GetCurrentProcessState(l_oUpdateProcessInfo.m_strID, l_oUpdateProcessInfo.m_strState);
	}
	
	if (l_strState.empty())
	{
		return false;
	}

	//TODO::记录警情流水_受理接警单	
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_oBody.m_strAlarmID = l_oUpdateProcessInfo.m_strAlarmID;
	std::vector<std::string> l_vecParamList;
	if (!m_pRedisClient->HGet("StaffInfoMap", l_oUpdateProcessInfo.m_strDispatchCode, l_strStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
	}
	l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
	strStaffType = l_oStaffInfo.m_strType;
	if (strStaffType == "JZLX101")
	{
		strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oUpdateProcessInfo.m_strDispatchName.c_str());
	}
	else
	{
		strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oUpdateProcessInfo.m_strDispatchName.c_str());
	}
	l_vecParamList.push_back(strStaffName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchCode);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchDeptName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strProcessDeptName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strCancelReason);
	l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oUpdateProcessInfo.m_strDispatchName;


	l_oUpdateProcessInfo.m_strUpdateUser = l_oUpdateProcessInfo.m_strDispatchName;
	l_oUpdateProcessInfo.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

	//如果state值为DIC020071，则修改警情状态为已取消
	if ((l_oUpdateProcessInfo.m_strState).compare("DIC020071") == 0)
	{
		//TODO::记录警情流水_取消调派
		if (!_InsertBackReason(l_oUpdateProcessInfo))
		{
			return false;
		}

		l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_DISPATCH_CANCELLED;
		//向数据库添加接警流水并发送通知
		if (!AddAlarmLogInfo(l_oAlarmLogInfo))
		{
			ICC_LOG_ERROR(m_pLog, "Insert icc_t_alarm_log failed!");
		}
	}

	//更新处警表前迁移数据到处警流水表
	DataBase::SQLRequest l_oSQLReq;
	std::string l_strResourceID = m_pString->CreateGuid();
	if (InsertProcessBookBeforeUpdate(l_oUpdateProcessInfo, l_strResourceID, p_strTransGuid))
	{
		//更新处警单
		if (!UpdateDBProcessInfo(l_oUpdateProcessInfo))
		{
			//向数据库插入接警记录失败
			//回复失败消息
			return false;
		}


		//TODO::记录警情流水完成编辑警单		
		//向数据库添加接警流水并发送通知
		if ((l_oUpdateProcessInfo.m_strState).compare("DIC020071") != 0)
		{
			l_vecParamList.clear();
			if (!m_pRedisClient->HGet("StaffInfoMap", l_oUpdateProcessInfo.m_strDispatchCode, l_strStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
			}
			l_oStaffInfo.Parse(l_strStaffInfo, m_pJsonFty->CreateJson());
			strStaffType = l_oStaffInfo.m_strType;
			if (strStaffType == "JZLX101")
			{
				strStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), l_oUpdateProcessInfo.m_strDispatchName.c_str());
			}
			else
			{
				strStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), l_oUpdateProcessInfo.m_strDispatchName.c_str());
			}
			l_vecParamList.push_back(strStaffName);
			l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchCode);
			l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchDeptName);
			l_oAlarmLogInfo.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_oBody.m_strCreateUser = l_oUpdateProcessInfo.m_strDispatchName;
			//l_oAlarmLogInfo.m_oBody.m_strResourceID = l_strResourceID;
			l_oAlarmLogInfo.m_oBody.m_strOperate = LOG_ALARM_EDIT;
			if (!AddAlarmLogInfo(l_oAlarmLogInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_ALARM_EDIT failed!");
			}
		}

		SyncProcessInfo(l_oUpdateProcessInfo, 2);
	}
	return true;
}

bool CBusinessImpl::UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	
	//VCS过来的使用VCS派警单的状态
	if ("vcs_relocated" != p_pProcessInfo.m_strMsgSource && "vcs" != p_pProcessInfo.m_strMsgSource)
	{
		//p_pProcessInfo.m_strState = RealProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState, "02", ""); //"DIC020010";
		p_pProcessInfo.m_strState = _GetCurrentProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState);
	}
	

	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "update_icc_t_pjdb";
	l_tSQLRequest.param["id"] = p_pProcessInfo.m_strID;
	std::string strTime = m_pDateTime->GetDispatchIdTime(p_pProcessInfo.m_strID);
	if (strTime != "")
	{
		l_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLRequest.set["dispatch_dept_district_code"] = p_pProcessInfo.m_strDispatchDeptDistrictCode;
	l_tSQLRequest.set["alarm_id"] = p_pProcessInfo.m_strAlarmID;
	l_tSQLRequest.set["dispatch_dept_code"] = p_pProcessInfo.m_strDispatchDeptCode;
	l_tSQLRequest.set["dispatch_code"] = p_pProcessInfo.m_strDispatchCode;
	l_tSQLRequest.set["dispatch_name"] = p_pProcessInfo.m_strDispatchName;
	l_tSQLRequest.set["record_id"] = p_pProcessInfo.m_strRecordID;
	l_tSQLRequest.set["dispatch_suggestion"] = p_pProcessInfo.m_strDispatchSuggestion;
	l_tSQLRequest.set["process_dept_code"] = p_pProcessInfo.m_strProcessDeptCode;
	l_tSQLRequest.set["time_submit"] = p_pProcessInfo.m_strTimeSubmit;
	l_tSQLRequest.set["time_arrived"] = p_pProcessInfo.m_strTimeArrived;
	l_tSQLRequest.set["time_signed"] = p_pProcessInfo.m_strTimeSigned;
	l_tSQLRequest.set["process_name"] = p_pProcessInfo.m_strProcessName;
	l_tSQLRequest.set["process_code"] = p_pProcessInfo.m_strProcessCode;
	l_tSQLRequest.set["dispatch_personnel"] = p_pProcessInfo.m_strDispatchPersonnel;
	l_tSQLRequest.set["dispatch_vehicles"] = p_pProcessInfo.m_strDispatchVehicles;
	l_tSQLRequest.set["dispatch_boats"] = p_pProcessInfo.m_strDispatchBoats;
	l_tSQLRequest.set["state"] = p_pProcessInfo.m_strState;
	l_tSQLRequest.set["create_time"] = p_pProcessInfo.m_strCreateTime;
	l_tSQLRequest.set["update_time"] = p_pProcessInfo.m_strUpdateTime;
	l_tSQLRequest.set["dispatch_dept_name"] = p_pProcessInfo.m_strDispatchDeptName;
	l_tSQLRequest.set["dispatch_dept_org_code"] = p_pProcessInfo.m_strDispatchDeptOrgCode;
	l_tSQLRequest.set["process_dept_name"] = p_pProcessInfo.m_strProcessDeptName;
	l_tSQLRequest.set["process_dept_org_code"] = p_pProcessInfo.m_strProcessDeptOrgCode;
	l_tSQLRequest.set["process_object_type"] = p_pProcessInfo.m_strProcessObjectType;
	l_tSQLRequest.set["process_object_name"] = p_pProcessInfo.m_strProcessObjectName;
	l_tSQLRequest.set["process_object_code"] = p_pProcessInfo.m_strProcessObjectCode;
	l_tSQLRequest.set["business_status"] = p_pProcessInfo.m_strBusinessStatus;
	l_tSQLRequest.set["seat_code"] = p_pProcessInfo.m_strSeatCode;
	l_tSQLRequest.set["cancel_time"] = p_pProcessInfo.m_strCancelTime;
	l_tSQLRequest.set["cancel_reason"] = p_pProcessInfo.m_strCancelReason;
	if (!p_pProcessInfo.m_strIsAutoAssignJob.empty())
	{
		l_tSQLRequest.set["is_auto_assign_job"] = p_pProcessInfo.m_strIsAutoAssignJob;
	}
	
	l_tSQLRequest.set["create_user"] = p_pProcessInfo.m_strCreateUser;
	l_tSQLRequest.set["update_user"] = p_pProcessInfo.m_strUpdateUser;
	l_tSQLRequest.set["overtime_state"] = p_pProcessInfo.m_strOvertimeState;

	if(!p_pProcessInfo.m_strTransfDeptOrjCode.empty())
	{
		l_tSQLRequest.set["transfers_dept_org_code"] = p_pProcessInfo.m_strTransfDeptOrjCode;
	}
	if (!p_pProcessInfo.m_strProcessObjectID.empty())
	{
		l_tSQLRequest.set["process_object_id"] = p_pProcessInfo.m_strProcessObjectID;
	}
	if (!p_pProcessInfo.m_strIsOver.empty())
	{
		l_tSQLRequest.set["is_over"] = p_pProcessInfo.m_strIsOver;
	}
	if (!p_pProcessInfo.m_strOverRemark.empty())
	{
		l_tSQLRequest.set["over_remark"] = p_pProcessInfo.m_strOverRemark;
	}
	if (!p_pProcessInfo.m_strParentID.empty())
	{
		l_tSQLRequest.set["parent_id"] = p_pProcessInfo.m_strParentID;
	}
	if (!p_pProcessInfo.m_strGZLDM.empty())
	{
		l_tSQLRequest.set["flow_code"] = p_pProcessInfo.m_strGZLDM;
	}

	if (!p_pProcessInfo.m_strCreateTeminal.empty())
	{
		l_tSQLRequest.set["createTeminal"] = p_pProcessInfo.m_strCreateTeminal;
	}
	if (!p_pProcessInfo.m_strUpdateTeminal.empty())
	{
		l_tSQLRequest.set["updateTeminal"] = p_pProcessInfo.m_strUpdateTeminal;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	//VCS过来的使用VCS派警单的状态
	if ("vcs_relocated" != p_pProcessInfo.m_strMsgSource && "vcs" != p_pProcessInfo.m_strMsgSource)
	{
		//p_pProcessInfo.m_strState = RealProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState, "02", ""); //"DIC020010";
		p_pProcessInfo.m_strState = _GetCurrentProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState);
	}
	

	DataBase::SQLRequest l_tSQLReqInsertProcess;

	l_tSQLReqInsertProcess.sql_id = "insert_icc_t_pjdb";

	l_tSQLReqInsertProcess.param["id"] = p_pProcessInfo.m_strID;
	l_tSQLReqInsertProcess.param["alarm_id"] = p_pProcessInfo.m_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_pProcessInfo.m_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReqInsertProcess.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLReqInsertProcess.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLReqInsertProcess.param["state"] = p_pProcessInfo.m_strState;
	l_tSQLReqInsertProcess.param["dispatch_dept_district_code"] = p_pProcessInfo.m_strDispatchDeptDistrictCode;
	l_tSQLReqInsertProcess.param["id"] = p_pProcessInfo.m_strID;
	l_tSQLReqInsertProcess.param["alarm_id"] = p_pProcessInfo.m_strAlarmID;
	l_tSQLReqInsertProcess.param["dispatch_dept_code"] = p_pProcessInfo.m_strDispatchDeptCode;
	l_tSQLReqInsertProcess.param["dispatch_code"] = p_pProcessInfo.m_strDispatchCode;
	l_tSQLReqInsertProcess.param["dispatch_name"] = p_pProcessInfo.m_strDispatchName;
	l_tSQLReqInsertProcess.param["record_id"] = p_pProcessInfo.m_strRecordID;
	l_tSQLReqInsertProcess.param["dispatch_suggestion"] = p_pProcessInfo.m_strDispatchSuggestion;
	l_tSQLReqInsertProcess.param["process_dept_code"] = p_pProcessInfo.m_strProcessDeptCode;
	l_tSQLReqInsertProcess.param["time_submit"] = p_pProcessInfo.m_strTimeSubmit;
	l_tSQLReqInsertProcess.param["time_arrived"] = p_pProcessInfo.m_strTimeArrived;
	l_tSQLReqInsertProcess.param["time_signed"] = p_pProcessInfo.m_strTimeSigned;
	l_tSQLReqInsertProcess.param["process_name"] = p_pProcessInfo.m_strProcessName;
	l_tSQLReqInsertProcess.param["process_code"] = p_pProcessInfo.m_strProcessCode;
	l_tSQLReqInsertProcess.param["dispatch_personnel"] = p_pProcessInfo.m_strDispatchPersonnel;
	l_tSQLReqInsertProcess.param["dispatch_vehicles"] = p_pProcessInfo.m_strDispatchVehicles;
	l_tSQLReqInsertProcess.param["dispatch_boats"] = p_pProcessInfo.m_strDispatchBoats;
	l_tSQLReqInsertProcess.param["dispatch_dept_name"] = p_pProcessInfo.m_strDispatchDeptName;
	l_tSQLReqInsertProcess.param["dispatch_dept_org_code"] = p_pProcessInfo.m_strDispatchDeptOrgCode;
	l_tSQLReqInsertProcess.param["process_dept_name"] = p_pProcessInfo.m_strProcessDeptName;
	l_tSQLReqInsertProcess.param["process_dept_org_code"] = p_pProcessInfo.m_strProcessDeptOrgCode;
	l_tSQLReqInsertProcess.param["process_object_type"] = p_pProcessInfo.m_strProcessObjectType;
	l_tSQLReqInsertProcess.param["process_object_name"] = p_pProcessInfo.m_strProcessObjectName;
	l_tSQLReqInsertProcess.param["process_object_code"] = p_pProcessInfo.m_strProcessObjectCode;
	l_tSQLReqInsertProcess.param["business_status"] = p_pProcessInfo.m_strBusinessStatus;
	l_tSQLReqInsertProcess.param["seat_code"] = p_pProcessInfo.m_strSeatCode;
	l_tSQLReqInsertProcess.param["cancel_time"] = p_pProcessInfo.m_strCancelTime;
	l_tSQLReqInsertProcess.param["cancel_reason"] = p_pProcessInfo.m_strCancelReason;
	if (!p_pProcessInfo.m_strIsAutoAssignJob.empty())
	{
		l_tSQLReqInsertProcess.param["is_auto_assign_job"] = p_pProcessInfo.m_strIsAutoAssignJob;
	}
	else
	{
		l_tSQLReqInsertProcess.param["is_auto_assign_job"] = "0";
	}
	
	l_tSQLReqInsertProcess.param["overtime_state"] = p_pProcessInfo.m_strOvertimeState;

	l_tSQLReqInsertProcess.param["create_user"] = p_pProcessInfo.m_strCreateUser;
	l_tSQLReqInsertProcess.param["create_time"] = p_pProcessInfo.m_strCreateTime;
	if (p_pProcessInfo.m_strUpdateTime.empty())
	{
		l_tSQLReqInsertProcess.param["update_time"] = p_pProcessInfo.m_strCreateTime;
	}
	else
	{
		l_tSQLReqInsertProcess.param["update_time"] = p_pProcessInfo.m_strUpdateTime;
	}

	l_tSQLReqInsertProcess.param["process_object_id"] = p_pProcessInfo.m_strProcessObjectID;
	l_tSQLReqInsertProcess.param["transfers_dept_org_code"] = p_pProcessInfo.m_strTransfDeptOrjCode;
	std::string l_strIsOver = "0";
	if (!p_pProcessInfo.m_strIsOver.empty())
	{
		l_strIsOver = p_pProcessInfo.m_strIsOver;
	}
	l_tSQLReqInsertProcess.param["is_over"] = l_strIsOver;
	l_tSQLReqInsertProcess.param["over_remark"] = p_pProcessInfo.m_strOverRemark;
	l_tSQLReqInsertProcess.param["parent_id"] = p_pProcessInfo.m_strParentID;
	l_tSQLReqInsertProcess.param["flow_code"] = p_pProcessInfo.m_strGZLDM;

	l_tSQLReqInsertProcess.param["createTeminal"] = p_pProcessInfo.m_strCreateTeminal;
	l_tSQLReqInsertProcess.param["updateTeminal"] = p_pProcessInfo.m_strUpdateTeminal;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertProcess);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

void CBusinessImpl::SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_rProcessToSync, int p_iSyncType)
{
	PROTOCOL::CAlarmProcessSync l_oAlarmProcessSync;
	l_oAlarmProcessSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmProcessSync.m_oHeader.m_strCmd = "alarm_process_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequestType = "1";//主题
	l_oAlarmProcessSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmProcessSync.m_oHeader.m_strCMSProperty = MSG_SOURCE;

	l_oAlarmProcessSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);// 1:add 2:update
	l_oAlarmProcessSync.m_oBody.m_ProcessData = p_rProcessToSync;

	//同步消息
	std::string l_strMsg(l_oAlarmProcessSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "[AlarmProcess]sync alarm process msg[\n%s\n]", l_strMsg.c_str());
}

bool CBusinessImpl::InsertProcessBookBeforeUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid /* = "" */)
{
	//不需要写处警book表
	return true;
}

bool CBusinessImpl::InsertProcessBookAfterUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, std::string p_strTransGuid /* = "" */)
{
	return true;
	//将查询到的警单备份到pjdb_book表中
}

void CBusinessImpl::AnalogProcessData(const PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarm, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	p_pProcessInfo.m_bIsNewProcess = true;																	//是否是新增的处警单，true代表是，false代表否

	p_pProcessInfo.m_strID = m_pString->CreateGuid();														//处警ID
	p_pProcessInfo.m_strAlarmID = p_oAlarm.m_oBody.m_oAlarm.m_strID;										//警情ID
	p_pProcessInfo.m_strState = "02";																//处警单状态 默认给"已下达"
	//p_pProcessInfo.m_strTimeEdit = m_pDateTime->CurrentDateTimeStr();										//派警单填写时间
	p_pProcessInfo.m_strTimeSubmit = m_pDateTime->CurrentDateTimeStr();;									//派警单提交时间

	p_pProcessInfo.m_strDispatchDeptDistrictCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//派警单位行政区划
	p_pProcessInfo.m_strDispatchDeptCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//派警单位代码
	p_pProcessInfo.m_strDispatchDeptName = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptName;					//派警单位名称
	p_pProcessInfo.m_strDispatchCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptCode;							//派警人警号
	p_pProcessInfo.m_strDispatchName = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptName;							//派警人姓名

/*
	p_pProcessInfo.m_strProcessDeptDistrictCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//处警单位行政区划
	p_pProcessInfo.m_strProcessDeptCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//处警单位代码
	p_pProcessInfo.m_strProcessDeptName = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptName;					//处警单位名称
	p_pProcessInfo.m_strProcessCode = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptCode;							//处警人警号
	p_pProcessInfo.m_strProcessName = p_oAlarm.m_oBody.m_oAlarm.m_strReceiptName;							//处警人姓名*/

}

bool CBusinessImpl::GetAlarm(std::string p_strID, PROTOCOL::CAddOrUpdateAlarmRequest& p_oAlarm)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb";
	l_oSeleteAlarmSQLReq.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get alarm failed, [%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		if (!p_oAlarm.m_oBody.m_oAlarm.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}
	}

	ICC_LOG_DEBUG(m_pLog, "get alarm success, [%s]", p_strID.c_str());

	return true;
}

bool CBusinessImpl::GetProcess(std::string p_strID, PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_pjdb";
	l_oSeleteAlarmSQLReq.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}
	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get process failed, [%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get process success, [%s]", p_strID.c_str());

	while (l_pResult->Next())
	{
		p_pProcessInfo.m_strDispatchDeptDistrictCode = l_pResult->GetValue("dispatch_dept_district_code");
		p_pProcessInfo.m_strID = l_pResult->GetValue("id");
		p_pProcessInfo.m_strAlarmID = l_pResult->GetValue("alarm_id");
		p_pProcessInfo.m_strDispatchDeptCode = l_pResult->GetValue("dispatch_dept_code");
		p_pProcessInfo.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
		p_pProcessInfo.m_strDispatchName = l_pResult->GetValue("dispatch_name");
		p_pProcessInfo.m_strRecordID = l_pResult->GetValue("record_id");
		p_pProcessInfo.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		p_pProcessInfo.m_strProcessDeptCode = l_pResult->GetValue("process_dept_code");
		p_pProcessInfo.m_strTimeSubmit = l_pResult->GetValue("time_submit");
		p_pProcessInfo.m_strTimeArrived = l_pResult->GetValue("time_arrived");
		p_pProcessInfo.m_strTimeSigned = l_pResult->GetValue("time_signed");
		p_pProcessInfo.m_strProcessName = l_pResult->GetValue("process_name");
		p_pProcessInfo.m_strProcessCode = l_pResult->GetValue("process_code");
		p_pProcessInfo.m_strDispatchPersonnel = l_pResult->GetValue("dispatch_personnel");
		p_pProcessInfo.m_strDispatchVehicles = l_pResult->GetValue("dispatch_vehicles");
		p_pProcessInfo.m_strDispatchBoats = l_pResult->GetValue("dispatch_boats");
		p_pProcessInfo.m_strState = l_pResult->GetValue("state");
		p_pProcessInfo.m_strCreateTime = l_pResult->GetValue("create_time");
		p_pProcessInfo.m_strUpdateTime = l_pResult->GetValue("update_time");
		p_pProcessInfo.m_strDispatchDeptName = l_pResult->GetValue("dispatch_dept_name");
		p_pProcessInfo.m_strDispatchDeptOrgCode = l_pResult->GetValue("dispatch_dept_org_code");
		p_pProcessInfo.m_strProcessDeptName = l_pResult->GetValue("process_dept_name");
		p_pProcessInfo.m_strProcessDeptOrgCode = l_pResult->GetValue("process_dept_org_code");
		p_pProcessInfo.m_strProcessObjectType = l_pResult->GetValue("process_object_type");
		p_pProcessInfo.m_strProcessObjectName = l_pResult->GetValue("process_object_name");
		p_pProcessInfo.m_strProcessObjectCode = l_pResult->GetValue("process_object_code");
		p_pProcessInfo.m_strBusinessStatus = l_pResult->GetValue("business_status");
		p_pProcessInfo.m_strSeatCode = l_pResult->GetValue("seat_code");
		p_pProcessInfo.m_strCancelTime = l_pResult->GetValue("cancel_time");
		p_pProcessInfo.m_strCancelReason = l_pResult->GetValue("cancel_reason");
		p_pProcessInfo.m_strIsAutoAssignJob = l_pResult->GetValue("is_auto_assign_job","0");
		p_pProcessInfo.m_strCreateUser = l_pResult->GetValue("create_user");
		p_pProcessInfo.m_strUpdateUser = l_pResult->GetValue("update_user");
		p_pProcessInfo.m_strOvertimeState = l_pResult->GetValue("overtime_state");

		p_pProcessInfo.m_strProcessObjectID = l_pResult->GetValue("process_object_id");
		p_pProcessInfo.m_strTransfDeptOrjCode = l_pResult->GetValue("transfers_dept_org_code");
		p_pProcessInfo.m_strIsOver = l_pResult->GetValue("is_over","0");
		p_pProcessInfo.m_strOverRemark = l_pResult->GetValue("over_remark");
		p_pProcessInfo.m_strParentID = l_pResult->GetValue("parent_id");
		p_pProcessInfo.m_strGZLDM = l_pResult->GetValue("flow_code");

		p_pProcessInfo.m_strCreateTeminal = l_pResult->GetValue("createteminal");
		p_pProcessInfo.m_strUpdateTeminal = l_pResult->GetValue("updateteminal");
	}

	return true;
}
//处警单分配
void CBusinessImpl::AssignToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID)
{
	if (p_strDescSeatNo.empty() || p_strAlarmID.empty())
	{
		return;
	}

	SeatInfo l_SeatInfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(p_strDescSeatNo, l_SeatInfo))
	{
		ICC_LOG_ERROR(m_pLog, "get seat info failed: seat[%s]", p_strDescSeatNo.c_str());
		return ;
	}
	
	//向处警席发送处警单
	PROTOCOL::Alarm::CProcessAppoint l_processAppoint;

	l_processAppoint.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_processAppoint.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_processAppoint.m_oHeader.m_strCmd = "process_appoint";
	l_processAppoint.m_oHeader.m_strRequest = p_strDescSeatNo;
	l_processAppoint.m_oHeader.m_strRequestType = "0";

	l_processAppoint.m_oBody.m_strAlarmID = p_strAlarmID;
	l_processAppoint.m_oBody.m_strSeatNo = p_strDescSeatNo;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_processAppoint.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
}

void CBusinessImpl::AssignProcessToSeat(const std::string& p_strDescSeatNo, std::string p_strAlarmID, std::string p_strProcessID)
{

}

//分配公共处警单
bool CBusinessImpl::AssignPubProcess(const std::string& p_strSeatNo, std::string& p_strAlarmID)
{
	SeatInfo l_SeatInfo;
	if (!CSeatManager::Instance()->GetOnlineSeat(p_strSeatNo, l_SeatInfo))
	{		
		ICC_LOG_ERROR(m_pLog, "seat[%s] is offline.", p_strSeatNo.c_str());
		return false;
	}

	if (l_SeatInfo.m_nSeatType != SEAT_PROCESS && l_SeatInfo.m_nSeatType != SEAT_SYNTHETICAL)	//非处警席不自动分配公共处警单
	{
		ICC_LOG_ERROR(m_pLog, "seat[%s] have no prcesss auth", p_strSeatNo.c_str());
		return false;
	}

	//获取一条公共处警单，并通知受理席删除该公共处警单		
	PubAlarmList l_pubList;
	CSeatManager::Instance()->GetPubCache(l_SeatInfo.m_strDeptCode, l_pubList);
	if (l_pubList.empty())	//无公共处警单
	{
		ICC_LOG_DEBUG(m_pLog, "dept[%s] has no prcesss", l_SeatInfo.m_strDeptCode.c_str());
		return false;
	}

	std::string l_strAlarmID = l_pubList.front();
	//分配处警单到处警席
	AssignToSeat(p_strSeatNo, l_strAlarmID);
	p_strAlarmID = l_strAlarmID;

	ICC_LOG_DEBUG(m_pLog, "assign alarm to seat success. [%s][%s]", l_strAlarmID.c_str(), p_strSeatNo.c_str());

	return true;
}