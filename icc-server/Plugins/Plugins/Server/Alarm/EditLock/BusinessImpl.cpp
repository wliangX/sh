#include "Boost.h"
#include "BusinessImpl.h"

#define SYSTEMID ("icc_server")
#define SUBSYSTEMID ("icc_server_alarm_edit_lock")
#define TIMER_CMD_NAME "alarm_edit_lock_timer"

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

	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_edit_lock_request", OnNotifiAlarmEditLockRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_edit_unlock_request", OnNotifiAlarmEditUnlockRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_edit_lock_request", OnNotifiGetAlarmEditlockStatusRequest);

	LoadAlarmEditLock();
    ICC_LOG_DEBUG(m_pLog, "edit lock start success!");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "edit lock stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::LoadAlarmEditLock()
{
	std::string l_strTimeInterval = m_pConfig->GetValue("ICC/Plugin/Alarm/timeInterval", "");
	std::string l_strDelayTime = m_pConfig->GetValue("ICC/Plugin/Alarm/delayTime", "");
	unsigned int l_iTime = m_pString->ToUInt(l_strTimeInterval);
	unsigned int l_iDelayTime = m_pString->ToUInt(l_strDelayTime);

	//解锁全部编辑状态
	DataBase::SQLRequest l_oDelete;
	DataBase::SQLRequest l_SqlRequestbefor;
	l_SqlRequestbefor.sql_id = "select_icc_t_alarm_edit_lock";
	DataBase::IResultSetPtr l_resultbufor = m_pDBConn->Exec(l_SqlRequestbefor, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_resultbufor->GetSQL().c_str());
	if (!l_resultbufor->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_resultbufor->GetErrorMsg().c_str());
		return;
	}
	else
	{
		//启动解锁所有警单
		while (l_resultbufor->Next())
		{
			PROTOCOL::CAlarmEditLockSync l_oAlarmEditUnLockSync;
			l_oAlarmEditUnLockSync.m_oHeader.m_strSystemID = SYSTEMID;
			l_oAlarmEditUnLockSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
			l_oAlarmEditUnLockSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
			l_oAlarmEditUnLockSync.m_oHeader.m_strRelatedID = "";
			l_oAlarmEditUnLockSync.m_oHeader.m_strCmd = "alarm_edit_unlock_sync";
			l_oAlarmEditUnLockSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmEditUnLockSync.m_oHeader.m_strRequest = "topic_alarm";
			l_oAlarmEditUnLockSync.m_oHeader.m_strRequestType = "1";

			l_oAlarmEditUnLockSync.m_oBody.m_strId = l_resultbufor->GetValue("id");
			l_oAlarmEditUnLockSync.m_oBody.m_strCode = l_resultbufor->GetValue("user_code");
			l_oAlarmEditUnLockSync.m_oBody.m_strLockStatus = "0";

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strSyncMessage = l_oAlarmEditUnLockSync.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSyncMessage.c_str());
		}
	}
	//从数据库中删除
	l_oDelete.sql_id = "delete_icc_t_alarm_edit_lock";
	DataBase::IResultSetPtr l_pDelete = m_pDBConn->Exec(l_oDelete);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pDelete->GetSQL().c_str());
	if (!l_pDelete->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,delete operation failed:[DB Error]:%s", l_pDelete->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "All EditLock is Released");
	}
	std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, l_iTime, l_iDelayTime);

	ICC_LOG_DEBUG(m_pLog, "Timer [%s] is start", l_strTimerName.c_str());
}

void CBusinessImpl::AlarmEditLockSync(PROTOCOL::CAlarmEditLockSync& p_pAlarmEditLockSync, PROTOCOL::CAlarmEditLock& p_pRequest, std::string p_strSyncType)
{
	p_pAlarmEditLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
	p_pAlarmEditLockSync.m_oBody.m_strId = p_pRequest.m_oBody.m_strId;
	p_pAlarmEditLockSync.m_oBody.m_strCode = p_pRequest.m_oBody.m_strCode;
	p_pAlarmEditLockSync.m_oBody.m_strLockStatus = p_strSyncType;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSyncMessage = p_pAlarmEditLockSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "Send AlarmEditLockSync [%s]", l_strSyncMessage.c_str());
}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
}


//请求加锁协议
void CBusinessImpl::OnNotifiAlarmEditLockRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmEditLock request;
	PROTOCOL::CAlarmEditLock response;
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	GenRespHeader("alarm_edit_lock_respond", request.m_oHeader, response.m_oHeader);
	//查询用户是否已经加锁
	bool l_bIsExit = false;
	DataBase::SQLRequest l_Query;
	l_Query.sql_id = "select_icc_t_alarm_edit_lock";
	l_Query.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));
	DataBase::IResultSetPtr p_Query = m_pDBConn->Exec(l_Query, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Query->GetSQL().c_str());
	if (!p_Query->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select operation failed: %s", p_Query->GetErrorMsg().c_str());
		response.m_strResult = "1";
	}
	else
	{
		if (p_Query->Next())
		{
			if (p_Query->GetValue("user_code") == request.m_oBody.m_strCode)
			{
				l_bIsExit = true;
				ICC_LOG_DEBUG(m_pLog, "user [%s] has locked the alarm [%s]", request.m_oBody.m_strCode.c_str(), request.m_oBody.m_strId.c_str());
			}
		}		
	}
	if (l_bIsExit)
	{
		DataBase::SQLRequest l_Update;
		l_Update.sql_id = "update_icc_t_alarm_edit_lock";
		l_Update.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));
		l_Update.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
		l_Update.set.insert(std::pair<std::string, std::string>("update_time", l_strCurrentTime));
		DataBase::IResultSetPtr p_Update = m_pDBConn->Exec(l_Update);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Update->GetSQL().c_str());
		if (!p_Update->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "data is invalid,update editLock failed: %s", p_Update->GetErrorMsg().c_str());
			response.m_strResult = "1";
		}
		else
		{
			response.m_strResult = "0";
			ICC_LOG_DEBUG(m_pLog, "update editLock success");
		}
	}
	else
	{
		//新增警情编辑锁
		DataBase::SQLRequest l_Add;
		l_Add.sql_id = "insert_icc_t_alarm_edit_lock";
		l_Add.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));
		l_Add.param.insert(std::pair<std::string, std::string>("user_code", request.m_oBody.m_strCode));
		l_Add.param.insert(std::pair<std::string, std::string>("lock_time", request.m_oBody.m_strLockTime));
		l_Add.param.insert(std::pair<std::string, std::string>("create_user", "BaseData"));
		l_Add.param.insert(std::pair<std::string, std::string>("create_time", l_strCurrentTime));
		l_Add.param.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
		l_Add.param.insert(std::pair<std::string, std::string>("update_time", l_strCurrentTime));

		DataBase::IResultSetPtr p_Query = m_pDBConn->Exec(l_Add);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Query->GetSQL().c_str());
		if (!p_Query->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "data is invalid,add editLock failed: %s", p_Query->GetErrorMsg().c_str());
			response.m_strResult = "1";
		}
		else
		{
			response.m_strResult = "0";
			ICC_LOG_DEBUG(m_pLog, "add editLock success");
		}
	}	
	std::string l_strMessage = response.ToStringResponse(ICCGetIJsonFactory()->CreateJson());
	
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send AlarmEditLockRequest [%s]", l_strMessage.c_str());
	if (response.m_strResult == "0")
	{
		//发送加锁同步消息
		PROTOCOL::CAlarmEditLockSync l_oAlarmEditLockSync;
		l_oAlarmEditLockSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oAlarmEditLockSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
		l_oAlarmEditLockSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oAlarmEditLockSync.m_oHeader.m_strCmd = "alarm_edit_lock_sync";
		l_oAlarmEditLockSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAlarmEditLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
		l_oAlarmEditLockSync.m_oHeader.m_strRequestType = "1";

		l_oAlarmEditLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
		l_oAlarmEditLockSync.m_oBody.m_strId = request.m_oBody.m_strId;
		l_oAlarmEditLockSync.m_oBody.m_strCode = request.m_oBody.m_strCode;
		l_oAlarmEditLockSync.m_oBody.m_strLockStatus = "1";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strSyncMessage = l_oAlarmEditLockSync.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
		ICC_LOG_DEBUG(m_pLog, "send AlarmEditLockSync [%s]", l_strSyncMessage.c_str());
	}		
}

//请求解锁协议
void CBusinessImpl::OnNotifiAlarmEditUnlockRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CAlarmEditLock request;
	PROTOCOL::CAlarmEditLock response;
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!request.ParseString(l_strRecvMsg, l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	//解除警情编辑锁，对非警情锁持有者的解锁请求进行忽略
	DataBase::SQLRequest l_Query;
	l_Query.sql_id = "select_icc_t_alarm_edit_lock";
	l_Query.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));
	DataBase::IResultSetPtr p_Query = m_pDBConn->Exec(l_Query, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Query->GetSQL().c_str());
	if (!p_Query->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select operation failed: %s", p_Query->GetErrorMsg().c_str());
		response.m_strResult = "1";
	}
	else
	{
		if (p_Query->Next())
		{
			if (p_Query->GetValue("user_code") == request.m_oBody.m_strCode)
			{
				//进行解锁
				DataBase::SQLRequest l_oDelete;
				l_oDelete.sql_id = "delete_icc_t_alarm_edit_lock";
				l_oDelete.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));

				DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oDelete);
				ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
				if (!l_result->IsValid())
				{
					ICC_LOG_ERROR(m_pLog, "UnLock editLock failed ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
					response.m_strResult = "1";
				}
				else
				{
					response.m_strResult = "0";
					ICC_LOG_DEBUG(m_pLog, "UnLock editLock success");
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "user code not match,unlock failed");
				response.m_strResult = "1";
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "user code is null,unlock failed");
			response.m_strResult = "1";
		}
	}
	
	GenRespHeader("alarm_edit_unlock_respond", request.m_oHeader, response.m_oHeader);

	std::string l_strMessage = response.ToStringResponse(ICCGetIJsonFactory()->CreateJson());
	
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send AlarmEditUnlockRespond [%s]", l_strMessage.c_str());
	if (response.m_strResult == "0")
	{
		//发送解锁同步消息
		PROTOCOL::CAlarmEditLockSync l_oAlarmEditUnLockSync;
		l_oAlarmEditUnLockSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oAlarmEditUnLockSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
		l_oAlarmEditUnLockSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oAlarmEditUnLockSync.m_oHeader.m_strCmd = "alarm_edit_lock_sync";
		l_oAlarmEditUnLockSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAlarmEditUnLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
		l_oAlarmEditUnLockSync.m_oHeader.m_strRequestType = "1";

		l_oAlarmEditUnLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
		l_oAlarmEditUnLockSync.m_oBody.m_strId = request.m_oBody.m_strId;
		l_oAlarmEditUnLockSync.m_oBody.m_strCode = request.m_oBody.m_strCode;
		l_oAlarmEditUnLockSync.m_oBody.m_strLockStatus = "0";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strSyncMessage = l_oAlarmEditUnLockSync.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
		ICC_LOG_DEBUG(m_pLog, "send AlarmEditUnlockSync [%s]", l_strSyncMessage.c_str());
	}

}

//获取警情状态协议
void CBusinessImpl::OnNotifiGetAlarmEditlockStatusRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CAlarmEditLock request;
	PROTOCOL::CAlarmEditLock response;
	std::string l_strRecvMsg = p_pNotify->GetMessages();
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	
	response.m_oBody.m_strId = request.m_oBody.m_strId;
	
	//查询用户是否已经加锁
	DataBase::SQLRequest l_Query;
	l_Query.sql_id = "select_icc_t_alarm_edit_lock";
	l_Query.param.insert(std::pair<std::string, std::string>("id", request.m_oBody.m_strId));
	DataBase::IResultSetPtr p_Query = m_pDBConn->Exec(l_Query, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Query->GetSQL().c_str());
	if (!p_Query->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select operation failed: %s", p_Query->GetErrorMsg().c_str());
		response.m_oBody.m_strLock_Status = "0";
	}
	else
	{
		if (p_Query->Next())
		{
			response.m_oBody.m_strCode = p_Query->GetValue("user_code");
			response.m_oBody.m_strLock_Status = "1";
			ICC_LOG_DEBUG(m_pLog, "alarm id [%s] status is locked", request.m_oBody.m_strId.c_str());
		}
		else
		{
			response.m_oBody.m_strLock_Status = "0";
			ICC_LOG_DEBUG(m_pLog, "alarm id [%s] status is unlock", request.m_oBody.m_strId.c_str());
		}
	}
			
	GenRespHeader("get_alarm_edit_lock_respond", request.m_oHeader, response.m_oHeader);
	std::string l_strMessage = response.ToGet(ICCGetIJsonFactory()->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetAlarmEditlockStatusRespond [%s]", l_strMessage.c_str());
}

//定时器的响应
void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{
	//查询用户加锁是否超时
	DataBase::SQLRequest l_Query;
	l_Query.sql_id = "select_icc_t_alarm_edit_lock";
	DataBase::IResultSetPtr p_Query = m_pDBConn->Exec(l_Query, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Query->GetSQL().c_str());
	if (!p_Query->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,select operation failed: %s", p_Query->GetErrorMsg().c_str());
	}
	else
	{
		while(p_Query->Next())
		{
			int l_iLockTime = m_pString->ToInt(p_Query->GetValue("lock_time"));
			DateTime::CDateTime l_iCreateTime = m_pDateTime->AddSeconds(m_pDateTime->FromString(p_Query->GetValue("create_time")), l_iLockTime);
			DateTime::CDateTime l_iCurrentTime = m_pDateTime->CurrentDateTime();

			//已经超过被锁时间
			if (l_iCreateTime <= l_iCurrentTime)
			{
				DataBase::SQLRequest l_oDelete;
				l_oDelete.sql_id = "delete_icc_t_alarm_edit_lock";
				l_oDelete.param.insert(std::pair<std::string, std::string>("id", p_Query->GetValue("id")));
				DataBase::IResultSetPtr l_pQuery = m_pDBConn->Exec(l_oDelete);
				ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pQuery->GetSQL().c_str());
				if (l_pQuery->IsValid())
				{
					//发送同步解锁消息
					PROTOCOL::CAlarmEditLockSync l_oAlarmEditUnLockSync;
					l_oAlarmEditUnLockSync.m_oHeader.m_strSystemID = SYSTEMID;
					l_oAlarmEditUnLockSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
					l_oAlarmEditUnLockSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
					l_oAlarmEditUnLockSync.m_oHeader.m_strRelatedID = "";
					l_oAlarmEditUnLockSync.m_oHeader.m_strCmd = "alarm_edit_lock_sync";
					l_oAlarmEditUnLockSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
					l_oAlarmEditUnLockSync.m_oHeader.m_strRequest = TOPIC_NAME;
					l_oAlarmEditUnLockSync.m_oHeader.m_strRequestType = "1";
					l_oAlarmEditUnLockSync.m_oBody.m_strId = p_Query->GetValue("id");
					l_oAlarmEditUnLockSync.m_oBody.m_strCode = p_Query->GetValue("user_code");
					l_oAlarmEditUnLockSync.m_oBody.m_strLockStatus = "0";

					JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
					std::string l_strSyncMessage = l_oAlarmEditUnLockSync.ToString(l_pIJson);
					m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
					ICC_LOG_DEBUG(m_pLog, "Timer send AlarmEditUnLockSync [%s]", l_strSyncMessage.c_str());
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "delete editLock failed ,Error Message :[%s]", l_pQuery->GetErrorMsg().c_str());
				}
			}
		}
	}	
}



