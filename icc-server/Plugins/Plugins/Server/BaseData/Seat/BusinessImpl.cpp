#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

const std::string ICC::CBusinessImpl::SeatKey("SeatInfo");

CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}
void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_LockFacPtr = ICCGetILockFactory();
	m_pJsonFty = ICCGetIJsonFactory();	
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	while (!LoadSeat())
	{
		ICC_LOG_ERROR(m_pLog, "plugin seat started failed,error:failed to load seat info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	/*if (!LoadSeat())
	{
		ICC_LOG_ERROR(m_pLog, "plugin seat started failed,error:failed to load staff info");
		return;
	}*/
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_seat_request", OnCNotifiGetAllSeatRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_seat_request", OnCNotifiGetSeatByDeptRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_seat_request", OnCNotifiSetSeatRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_seat_request", OnCNotifiDeleteSeatRequest);

	ICC_LOG_INFO(m_pLog, "plugin basedata.seat start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "seat stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiGetAllSeatRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetSeatRequest l_oGetSetRequest;
	if (!l_oGetSetRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString error! msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	std::map<std::string, std::string> l_mapCachedSeatInfo;
	if (!m_pRedisClient->HGetAll(SeatKey, l_mapCachedSeatInfo))
	{
		ICC_LOG_ERROR(m_pLog, "an error occured when getting seatinfo from redis");
	}

	ICC_LOG_DEBUG(m_pLog, "GetAllSeat complete! msgid = %s", l_oGetSetRequest.m_oHeader.m_strMsgId.c_str());

	PROTOCOL::CGetSeatRespond l_oGetAllSeatRespond;
	l_oGetAllSeatRespond.m_oHeader.m_strMsgId = l_oGetSetRequest.m_oHeader.m_strMsgId;
	if (l_mapCachedSeatInfo.size() == 0)
	{
		ICC_LOG_WARNING(m_pLog, "No Seat Data Found");
		l_oGetAllSeatRespond.m_oBody.m_strCount = "0";
		JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
		std::string l_strMessage = l_oGetAllSeatRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		/*int count = 0;*/
		for (auto it = l_mapCachedSeatInfo.begin(); it != l_mapCachedSeatInfo.end(); it++)
		{
			CSeatInfo l_oSeat;
			l_oSeat.Parse(it->second, m_pJsonFty->CreateJson());
			PROTOCOL::CGetSeatRespond::CBody::CData l_oData;
			l_oData.m_strNo = l_oSeat.m_strNo;
			l_oData.m_strName = l_oSeat.m_strName;
			l_oData.m_strDeptCode = l_oSeat.m_strDeptCode;
			l_oData.m_strDeptName = l_oSeat.m_strDeptName;
			l_oGetAllSeatRespond.m_oBody.m_vecData.push_back(l_oData);
			/*count++;
			if (count%MAX_COUNT == 0)
			{
				l_oGetAllSeatRespond.m_oBody.m_strCount = m_pString->Number(l_mapCachedSeatInfo.size());
				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMessage = l_oGetAllSeatRespond.ToString(l_pIJson);
				p_pNotify->Response(l_strMessage, true);
				count = 0;
				l_oGetAllSeatRespond.m_oBody.m_vecData.clear();
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}*/
		}

		l_oGetAllSeatRespond.m_oBody.m_strCount = m_pString->Number(l_mapCachedSeatInfo.size());
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oGetAllSeatRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetSeatByDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetSeatByDeptRequest l_oGetSetByDeptRequest;
	if (!l_oGetSetByDeptRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCNotifiGetSeatByDeptRequest] parse json error! msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	std::map<std::string, std::string> l_mapCachedSeatInfo;
	if (!m_pRedisClient->HGetAll(SeatKey, l_mapCachedSeatInfo))
	{
		ICC_LOG_ERROR(m_pLog, "an error occured when getting seatinfo from redis");
	}
	ICC_LOG_DEBUG(m_pLog, "GetSeatByDept HGetAll complete! msgid = %s", l_oGetSetByDeptRequest.m_oHeader.m_strMsgId.c_str());

	std::vector<CSeatInfo> l_vecSeatByDept;
	std::string l_strDeptCode = l_oGetSetByDeptRequest.m_oBody.m_strDeptCode;
	for (auto it = l_mapCachedSeatInfo.begin(); it != l_mapCachedSeatInfo.end(); it++)
	{
		CSeatInfo l_oSeatInfo;
		l_oSeatInfo.Parse(it->second, m_pJsonFty->CreateJson());
		if (l_oSeatInfo.m_strDeptCode == l_strDeptCode)
		{
			l_vecSeatByDept.push_back(l_oSeatInfo);
		}
	}

	PROTOCOL::CGetSeatRespond l_oGetSeatRespond;
	l_oGetSeatRespond.m_oHeader.m_strMsgId = l_oGetSetByDeptRequest.m_oHeader.m_strMsgId;
	if (l_vecSeatByDept.size() == 0)
	{
		ICC_LOG_ERROR(m_pLog, "No Search Seat Data Found By DeptCode:[%s]", l_strDeptCode.c_str());
		l_oGetSeatRespond.m_oBody.m_strCount = "0";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oGetSeatRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{		
		//int count = 0;
		for (auto it = l_vecSeatByDept.begin(); it != l_vecSeatByDept.end(); it++)
		{
			PROTOCOL::CGetSeatRespond::CBody::CData l_oData;
			l_oData.m_strNo = it->m_strNo;
			l_oData.m_strName = it->m_strName;
			l_oData.m_strDeptCode = it->m_strDeptCode;
			l_oData.m_strDeptName = it->m_strDeptName;

			l_oGetSeatRespond.m_oBody.m_vecData.push_back(l_oData);
			/*count++;
			if (count%MAX_COUNT == 0)
			{
				l_oGetSeatRespond.m_oBody.m_strCount = m_pString->Number(l_vecSeatByDept.size());
				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMessage = l_oGetSeatRespond.ToString(l_pIJson);
				p_pNotify->Response(l_strMessage, true);
				count = 0;
				l_oGetSeatRespond.m_oBody.m_vecData.clear();
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}*/
		}

		l_oGetSeatRespond.m_oBody.m_strCount = m_pString->Number(l_vecSeatByDept.size());
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oGetSeatRespond.ToString(l_pIJson);
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::OnCNotifiSetSeatRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSetSeatRequest l_oSetSeatRequest;
	if (!l_oSetSeatRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	bool l_bSeatExistInCache = m_pRedisClient->HExists(SeatKey, l_oSetSeatRequest.m_oBody.m_strNo);
	SyncType l_eSyncType;
	DataBase::SQLRequest l_oSetSeatSQLReq;
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	if (l_bSeatExistInCache)
	{
		l_eSyncType = SyncType::UPDATE;
		l_oSetSeatSQLReq.sql_id = "update_icc_t_seat";		
		l_oSetSeatSQLReq.set["no"] = l_oSetSeatRequest.m_oBody.m_strNo;
		l_oSetSeatSQLReq.set["name"] = l_oSetSeatRequest.m_oBody.m_strName;
		l_oSetSeatSQLReq.set["dept_code"] = l_oSetSeatRequest.m_oBody.m_strDeptCode;
		l_oSetSeatSQLReq.set["dept_name"] = l_oSetSeatRequest.m_oBody.m_strDeptName;
		l_oSetSeatSQLReq.set["update_user"] = "BaseData.Seat";
		l_oSetSeatSQLReq.set["update_time"] = l_strCurrentTime;
		//l_oSetSeatSQLReq.set["is_delete"] = "false";
		l_oSetSeatSQLReq.param["no"] = l_oSetSeatRequest.m_oBody.m_strNo;
	}
	else
	{
		l_eSyncType = SyncType::ADD;
		l_oSetSeatSQLReq.sql_id = "insert_icc_t_seat";
		l_oSetSeatSQLReq.param["guid"] = m_pString->CreateGuid();
		l_oSetSeatSQLReq.param["no"] = l_oSetSeatRequest.m_oBody.m_strNo;
		l_oSetSeatSQLReq.param["name"] = l_oSetSeatRequest.m_oBody.m_strName;
		l_oSetSeatSQLReq.param["dept_code"] = l_oSetSeatRequest.m_oBody.m_strDeptCode;
		l_oSetSeatSQLReq.param["dept_name"] = l_oSetSeatRequest.m_oBody.m_strDeptName;
		l_oSetSeatSQLReq.param["is_delete"] = "false";
		l_oSetSeatSQLReq.param["create_user"] = "BaseData.Seat";
		l_oSetSeatSQLReq.param["create_time"] = l_strCurrentTime;		
	}
	//执行数据库操作
	std::string l_strTransactionGuid = m_pDBConn->BeginTransaction();
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSeatSQLReq, false, l_strTransactionGuid);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	PROTOCOL::CSetSeatRespond l_oSetSeatResp;
	l_oSetSeatResp.m_oHeader.m_strMsgId = l_oSetSeatRequest.m_oHeader.m_strMsgId;
	if (!l_pRSet->IsValid())
	{
		l_oSetSeatResp.m_oHeader.m_strResult = "1";
		l_oSetSeatResp.m_oHeader.m_strMsg = "Exec sql failed";

		m_pDBConn->Rollback(l_strTransactionGuid);
		ICC_LOG_ERROR(m_pLog, "SetSeatRequest Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		CSeatInfo l_oSeat;
		l_oSeat.m_strNo = l_oSetSeatRequest.m_oBody.m_strNo;
		l_oSeat.m_strDeptCode = l_oSetSeatRequest.m_oBody.m_strDeptCode;
		l_oSeat.m_strDeptName = l_oSetSeatRequest.m_oBody.m_strDeptName;
		l_oSeat.m_strName = l_oSetSeatRequest.m_oBody.m_strName;

		JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();

		std::string l_strVal = l_oSeat.ToJson(l_pJson);
		if (m_pRedisClient->HSet(SeatKey, l_oSeat.m_strNo, l_strVal))
		{	//Redis缓存更新成功
			m_pDBConn->Commit(l_strTransactionGuid);
			
			SyncSeatInfo(l_oSeat, l_eSyncType);
			ICC_LOG_DEBUG(m_pLog, "SetSeatRequest Success:[%s]", p_pNotify->GetMessages().c_str());
		}
		else
		{
			l_oSetSeatResp.m_oHeader.m_strResult = "1";
			l_oSetSeatResp.m_oHeader.m_strMsg = "Update redist failed";

			m_pDBConn->Rollback(l_strTransactionGuid);
			ICC_LOG_ERROR(m_pLog, "SetSeatRequest Failed,fail to update seat info in redis:[%s]", l_oSeat.ToJson(l_pJson).c_str());
		}
	}

	std::string l_strMessage(l_oSetSeatResp.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiDeleteSeatRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonFactoryPtr l_pJsonFty = ICCGetIJsonFactory();
	PROTOCOL::CDeleteSeatRequest l_oDeleteSeatRequest;
	if (!l_oDeleteSeatRequest.ParseString(p_pNotify->GetMessages(), l_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CDeleteSeatRespond l_oDeleteSeatResp;
	l_oDeleteSeatResp.m_oHeader.m_strMsgId = l_oDeleteSeatRequest.m_oHeader.m_strMsgId;

	//std::string l_strGuid;
	bool l_bSeatExistInCache = m_pRedisClient->HExists(SeatKey, l_oDeleteSeatRequest.m_oBody.m_strNo);
	if (l_bSeatExistInCache)
	{
		DataBase::SQLRequest l_oDeleteSeatSQLReq;
		l_oDeleteSeatSQLReq.sql_id = "update_icc_t_seat";
		l_oDeleteSeatSQLReq.param["no"] = l_oDeleteSeatRequest.m_oBody.m_strNo;
		l_oDeleteSeatSQLReq.set["is_delete"] = "true";

		//执行数据库操作                
		std::string l_strTransactionGuid = m_pDBConn->BeginTransaction();
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSeatSQLReq, false, l_strTransactionGuid);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			l_oDeleteSeatResp.m_oHeader.m_strResult = "1";
			l_oDeleteSeatResp.m_oHeader.m_strMsg = "Exec sql failed";

			m_pDBConn->Rollback(l_strTransactionGuid);
			ICC_LOG_ERROR(m_pLog, "DeleteSeatRequest Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
		}
		else if (m_pRedisClient->HDelFields(SeatKey, l_oDeleteSeatRequest.m_oBody.m_strNo.c_str(), NULL))
		{
			m_pDBConn->Commit(l_strTransactionGuid);
			
			ICC_LOG_DEBUG(m_pLog, "DeleteSeatRequest Success:[%s]", p_pNotify->GetMessages().c_str());

			CSeatInfo l_oSeat;
			l_oSeat.m_strNo = l_oDeleteSeatRequest.m_oBody.m_strNo;
			SyncSeatInfo(l_oSeat, SyncType::DELETE);
		}
		else
		{
			l_oDeleteSeatResp.m_oHeader.m_strResult = "1";
			l_oDeleteSeatResp.m_oHeader.m_strMsg = "Set Redis failed";

			m_pDBConn->Rollback(l_strTransactionGuid);
			ICC_LOG_WARNING(m_pLog, "[Fail To Delete Seat Info In Redis]DeleteSeatRequest Failed");
		}
	}

	std::string l_strMessage(l_oDeleteSeatResp.ToString(l_pJsonFty->CreateJson()));	
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::LoadSeat()
{
	m_pRedisClient->Del(SeatKey);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_seat";
	l_SqlRequest.param["is_delete"] = "false";

	ICC_LOG_DEBUG(m_pLog, "Loding Seat Begin");

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		CSeatInfo l_oData;
		l_oData.m_strNo = l_result->GetValue("no");
		l_oData.m_strDeptCode = l_result->GetValue("dept_code");
		l_oData.m_strDeptName = l_result->GetValue("dept_name");
		l_oData.m_strName = l_result->GetValue("name");

		std::string l_strVal = l_oData.ToJson(m_pJsonFty->CreateJson());
		m_pRedisClient->HSet(SeatKey, l_result->GetValue("no"), l_strVal);
	}

	ICC_LOG_DEBUG(m_pLog, "Loading Seat Success,Seat Size[%d]", l_result->RecordSize());
	return true;
}

void ICC::CBusinessImpl::SyncSeatInfo(const CSeatInfo& p_oSeat, SyncType p_eSyncType)
{
	PROTOCOL::CSeatSync l_CSeatSync;
	l_CSeatSync.m_oHeader.m_strSystemID = "ICC";
	l_CSeatSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer"; 
	l_CSeatSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CSeatSync.m_oHeader.m_strRelatedID = "";
	l_CSeatSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CSeatSync.m_oHeader.m_strCmd = "seat_sync";
	l_CSeatSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_CSeatSync.m_oHeader.m_strRequestType = "1";
	l_CSeatSync.m_oHeader.m_strResponse = "";
	l_CSeatSync.m_oHeader.m_strResponseType = "";

	l_CSeatSync.m_oBody.m_strSyncType = m_pString->Number(p_eSyncType);
	l_CSeatSync.m_oBody.m_strNo = p_oSeat.m_strNo;
	l_CSeatSync.m_oBody.m_strName = p_oSeat.m_strName;
	l_CSeatSync.m_oBody.m_strDeptName = p_oSeat.m_strDeptName;
	l_CSeatSync.m_oBody.m_strDeptCode = p_oSeat.m_strDeptCode;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_CSeatSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "Sync Seat Info cmd[seat_sync] sync_type[%d] no[%s] name[%s] dept_name[%s] dept_code[%s]"
		, p_eSyncType, p_oSeat.m_strNo.c_str(),p_oSeat.m_strName.c_str(),p_oSeat.m_strDeptName.c_str(),p_oSeat.m_strDeptCode.c_str());
	ICC_LOG_DEBUG(m_pLog, "Sync Seat Info:[%s]", l_strMessage.c_str());
}
