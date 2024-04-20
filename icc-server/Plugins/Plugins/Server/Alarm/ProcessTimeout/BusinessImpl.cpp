#include "Boost.h"
#include "BusinessImpl.h"

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pLockFty = ICCGetILockFactory();
	m_pAlarmLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);
	ICC_LOG_DEBUG(m_pLog, "processtimeout start begin");

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, GET_ALARM_PROCESS_REQUEST, OnGetAlarmProTimout);

	m_timerInterval = m_pString->ToInt64(m_pConfig->GetValue("ICC/Plugin/ProcessTimeout/TimerSeconds", "900"));
	m_ProcessIntervalTime = m_pString->ToInt64(m_pConfig->GetValue("ICC/Plugin/ProcessTimeout/ProcessTimeOut", "60"));
	m_Duration = m_pString->ToInt64(m_pConfig->GetValue("ICC/Plugin/ProcessTimeout/ProcessValidDuration", "1440"));

	std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, m_timerInterval, DELAYSTARTTIMER);
	ICC_LOG_DEBUG(m_pLog, "Timer [%s] is start", l_strTimerName.c_str());
	ICC_LOG_DEBUG(m_pLog, "processtimeout start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
	ICC_LOG_DEBUG(m_pLog, "processtimeout start complete");
}

void CBusinessImpl::OnStop()
{

}

void CBusinessImpl::OnDestroy()
{
	ICC_LOG_DEBUG(m_pLog, "processtimeout stop success");
}

void CBusinessImpl::LoadAlarmProcessTimeout()
{
	DataBase::SQLRequest l_oSelectProcess;
	l_oSelectProcess.sql_id = SELECT_ICC_T_ALARM_AND_PROCESS;
	int l_iDuration = 0 - m_Duration;
	int l_iTimedistance = 0 - m_ProcessIntervalTime;
	DateTime::CDateTime l_iNowfirst = m_pDateTime->AddMinutes(m_pDateTime->CurrentDateTime(), l_iDuration);
	DateTime::CDateTime l_iNowSecond = m_pDateTime->AddMinutes(m_pDateTime->CurrentDateTime(), l_iTimedistance);
	l_oSelectProcess.param["timefirst_now"] = m_pDateTime->ToString(l_iNowfirst);
	l_oSelectProcess.param["timesecond_now"] = m_pDateTime->ToString(l_iNowSecond);


	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectProcess, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return;
	}
	PROTOCOL::CAlarmProcessTimeoutSync l_oAlarmProcTimeoutSync;
	PROTOCOL::CAlarmProcessTimeoutSync::CBody::CData l_oData;
	while (l_oResult->Next())
	{
		std::string l_strCode = l_oResult->GetValue("processdispatchcode");
		std::string l_strCount = l_oResult->GetValue("count");
		if (m_pRedisClient->HExists(DEPT_TYPE_CODE, l_strCode))
		{
			l_oData.m_strCount = l_strCount;
			l_oData.m_strDeptCode = l_strCode;
			l_oAlarmProcTimeoutSync.m_oBody.m_mapData[l_strCode] = l_strCount;
			if (l_oAlarmProcTimeoutSync.m_oBody.m_mapData.size() % MAX_COUNT == 0)
			{
				l_oAlarmProcTimeoutSync.m_oHeader.m_strCmd = ALARM_PROCESS_SYNC;
				l_oAlarmProcTimeoutSync.m_oHeader.m_strRequest = TOPIC_ALARM;
				l_oAlarmProcTimeoutSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;
				l_oAlarmProcTimeoutSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
				std::string l_strSyncMessage = l_oAlarmProcTimeoutSync.ToString(m_pJsonFty->CreateJson());
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
				ICC_LOG_DEBUG(m_pLog, "send Msg[%s]", l_strSyncMessage.c_str());
				l_oAlarmProcTimeoutSync.m_oBody.m_mapData.clear();
			}
		}
	}
	if (!l_oAlarmProcTimeoutSync.m_oBody.m_mapData.empty())
	{
		l_oAlarmProcTimeoutSync.m_oHeader.m_strCmd = ALARM_PROCESS_SYNC;
		l_oAlarmProcTimeoutSync.m_oHeader.m_strRequest = TOPIC_ALARM;
		l_oAlarmProcTimeoutSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;
		l_oAlarmProcTimeoutSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		std::string l_strSyncMessage = l_oAlarmProcTimeoutSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
		ICC_LOG_DEBUG(m_pLog, "send Msg[%s]", l_strSyncMessage.c_str());
	}
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{
	LoadAlarmProcessTimeout();
}


void CBusinessImpl::OnGetAlarmProTimout(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetAlarmProcessTimeout l_oGetAlarmProcess;
	l_oGetAlarmProcess.ParseString(p_pNotify->GetMessages(), l_pJson);
	
	DataBase::SQLRequest l_oSelectProcess;

	l_oSelectProcess.sql_id = SELECT_ICC_T_ALARM_AND_PROCESS_EX;

	int l_iDuration = 0 - m_Duration;
	int l_iTimedistance = 0 - m_ProcessIntervalTime;
	DateTime::CDateTime l_iNowfirst = m_pDateTime->AddMinutes(m_pDateTime->CurrentDateTime(), l_iDuration);
	DateTime::CDateTime l_iNowSecond = m_pDateTime->AddMinutes(m_pDateTime->CurrentDateTime(), l_iTimedistance);
	l_oSelectProcess.param["timefirst_now"] = m_pDateTime->ToString(l_iNowfirst);
	l_oSelectProcess.param["timesecond_now"] = m_pDateTime->ToString(l_iNowSecond); 

	size_t l_iIndex;
	if (m_pString->ToInt(l_oGetAlarmProcess.m_body.m_strPageIdex) <= 0)
	{
		l_iIndex = 0;
	}
	else
	{
		l_iIndex = (m_pString->ToInt(l_oGetAlarmProcess.m_body.m_strPageIdex) - 1) * m_pString->ToInt(l_oGetAlarmProcess.m_body.m_strPageSize);
	}
	l_oSelectProcess.param["offset"] = m_pString->Number(l_iIndex);
	if (l_oGetAlarmProcess.m_body.m_strPageSize.empty() || m_pString->ToInt(l_oGetAlarmProcess.m_body.m_strPageSize) > MAX_COUNT)
	{
		// 若客户端传值不规范，默认10 [7/6/2018 w26326]
		l_oGetAlarmProcess.m_body.m_strPageSize = "10";
	}
	l_oSelectProcess.param["pagesize"] = l_oGetAlarmProcess.m_body.m_strPageSize;
	l_oSelectProcess.param["dept_code"] = l_oGetAlarmProcess.m_body.m_strDeptCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectProcess, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return;
	}
	PROTOCOL::CGetAlarmProcessTimeoutRespond l_oAlarmProcTimeoutSync;
	std::string l_strCount = "0";
	while (l_oResult->Next())
	{
		PROTOCOL::CGetAlarmProcessTimeoutRespond::CBody::CData l_oData;
		l_oData.m_strID = l_oResult->GetValue("alarmid");
		l_oData.m_strProcessID = l_oResult->GetValue("processid");
		l_oData.m_strTimeSubmit = l_oResult->GetValue("createtime");
		l_oData.m_strCalledNoType = l_oResult->GetValue("callednotype");
		l_oData.m_strContent = l_oResult->GetValue("content");
		l_oData.m_strProcessDeptCode = l_oResult->GetValue("processdeptcode");
		l_oData.m_strProcessDeptName = l_oResult->GetValue("processdeptname");

		l_oData.m_strDispatchCode = l_oResult->GetValue("processdispatchcode");
		l_oData.m_strDispatchName = l_oResult->GetValue("processdispatchname");
		l_oData.m_strDispatchDeptCode = l_oResult->GetValue("processdispatchdeptcode");
		l_oData.m_strDispatchDeptName = l_oResult->GetValue("processdispatchdeptname");
		l_strCount = l_oResult->GetValue("count");
		l_oAlarmProcTimeoutSync.m_oBody.m_vData.push_back(l_oData);
	}

	l_oAlarmProcTimeoutSync.m_oBody.m_strCount = l_strCount;
	l_oAlarmProcTimeoutSync.m_oHeader.m_strCmd = GET_ALARM_PROCESS_RESPOND;
	l_oAlarmProcTimeoutSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmProcTimeoutSync.m_oHeader.m_strRelatedID = l_oGetAlarmProcess.m_oHeader.m_strMsgid;
	std::string l_strRespMessage = l_oAlarmProcTimeoutSync.ToString(m_pJsonFty->CreateJson());

	p_pNotify->Response(l_strRespMessage);
	ICC_LOG_DEBUG(m_pLog, "send msg[%s]", l_strRespMessage.c_str());
}


