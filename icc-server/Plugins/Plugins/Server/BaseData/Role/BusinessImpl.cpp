#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define SYSTEMID "icc_server_basedata"
#define SUBSYSTEMID "icc_server_basedata"
#define ROLE_INFO_KEY  "RoleInfo"

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
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pIJson = ICCGetIJsonFactory()->CreateJson();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	/*while (!LoadRoleInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.role failed to load role info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	/*if (!LoadRoleInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.role failed to load role info");
		return;
	}*/

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_role_request", OnNotifiGetRoleRequest);//获取角色信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_role_request", OnNotifiSetRoleRequest);//设置角色信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_role_request", OnNotifiDeleteRoleRequest);//删除角色信息

	ICC_LOG_INFO(m_pLog, "plugin basedata.role started success.");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "plugin basedata.role stop.");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiGetRoleRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strMessage;
	JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
	PROTOCOL::CGetRoleRequest l_oGetRoleReq;
	if (!l_oGetRoleReq.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "OnNotifiGetRoleRequest Message is Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetRoleRespond l_oGetRoleRespond;
	GenRespHeader("get_role_respond", l_oGetRoleReq.m_oHeader, l_oGetRoleRespond.m_oHeader);
	
	std::map<std::string, std::string> l_mapRoleInfo;
	bool bGet = m_pRedisClient->HGetAll(ROLE_INFO_KEY, l_mapRoleInfo);
	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetRoleRequest] HGetAll complete! msgid = %s", l_oGetRoleReq.m_oHeader.m_strMsgid.c_str());
	if (bGet)
	{
		for (auto it = l_mapRoleInfo.begin(); it != l_mapRoleInfo.end(); it++)
		{
			CRoleInfo l_oRoleInfo;
			if (l_oRoleInfo.Parse(it->second,m_pJsonFty->CreateJson()))
			{
				PROTOCOL::CGetRoleRespond::CBody::CData l_oData;
				l_oData.m_strGuid = l_oRoleInfo.m_strGuid;
				l_oData.m_strCode = l_oRoleInfo.m_strCode;
				l_oData.m_strName = l_oRoleInfo.m_strName;
				l_oData.m_strParentGuid = l_oRoleInfo.m_strParentGuid;
				l_oData.m_strSort = l_oRoleInfo.m_strSort;
				l_oGetRoleRespond.m_oBody.m_vecData.push_back(l_oData);
			}
		}
	}

	int l_iCount = l_oGetRoleRespond.m_oBody.m_vecData.size();
	l_oGetRoleRespond.m_oBody.m_strCount = m_pString->Number(l_iCount);
	l_strMessage = l_oGetRoleRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send Role Info Msg Num [%d]", l_iCount);
}

void CBusinessImpl::OnNotifiSetRoleRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSetRoleRequest l_oSetRoleRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!l_oSetRoleRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetRoleRequestRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	//bool l_bUpDate = m_pRedisClient->HExists(ROLE_INFO_KEY, l_oSetRoleRequest.m_oBody.m_strGuid);
	std::string l_strSyncType = l_oSetRoleRequest.m_oBody.m_strSyncType;

	CRoleInfo l_oRoleInfo;
	l_oRoleInfo.m_strGuid = l_oSetRoleRequest.m_oBody.m_strGuid;
	l_oRoleInfo.m_strCode = l_oSetRoleRequest.m_oBody.m_strCode;
	l_oRoleInfo.m_strName = l_oSetRoleRequest.m_oBody.m_strName;
	l_oRoleInfo.m_strParentGuid = l_oSetRoleRequest.m_oBody.m_strParentGuid;
	l_oRoleInfo.m_strSort = l_oSetRoleRequest.m_oBody.m_strSort;
	SyncRoleInfo(l_oRoleInfo, l_strSyncType);
	/*if (l_bUpDate)
	{
		SyncRoleInfo(l_oRoleInfo, "2");
	}
	else
	{
		SyncRoleInfo(l_oRoleInfo, "1");
	}*/
}

void CBusinessImpl::OnNotifiDeleteRoleRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJsonPtr = m_pJsonFty->CreateJson();
	PROTOCOL::CDeleteRoleRequest l_oDeleRoleRequest;
	if (!l_oDeleRoleRequest.ParseString(p_pNotify->GetMessages(), l_pJsonPtr))
	{
		ICC_LOG_ERROR(m_pLog, "DeleteRoleRequest Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	CRoleInfo l_tRole;
	l_tRole.m_strGuid = l_oDeleRoleRequest.m_oBody.m_strGuid;

	//TODO::删除Role信息发什么同步消息？暂定guid,其余为空
	SyncRoleInfo(l_tRole, "3");
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

bool CBusinessImpl::LoadRoleInfo()
{
	m_pRedisClient->Del(ROLE_INFO_KEY);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_role";
	ICC_LOG_DEBUG(m_pLog, "LoadRoleInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, " ExecQuery Error Load RoleInfo Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		CRoleInfo l_oRole;
		l_oRole.m_strGuid = l_result->GetValue("guid");
		l_oRole.m_strCode = l_result->GetValue("code");
		l_oRole.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oRole.m_strName = l_result->GetValue("name");
		l_oRole.m_strSort = l_result->GetValue("sort");

		std::string l_strVal = l_oRole.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(ROLE_INFO_KEY, l_oRole.m_strGuid, l_strVal))
		{
			ICC_LOG_ERROR(m_pLog, "LoadRoleInfo In Redis Failed Role Guid[%s]", l_oRole.m_strGuid.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LoadRoleInfo Success,Role Size[%d]", l_result->RecordSize());
	return true;
}

bool CBusinessImpl::SyncRoleInfo(const CRoleInfo& p_oRoleInfo, std::string SyncType)
{
	if (p_oRoleInfo.m_strCode.empty() &&
		p_oRoleInfo.m_strGuid.empty() &&
		p_oRoleInfo.m_strName.empty() &&
		p_oRoleInfo.m_strParentGuid.empty() &&
		p_oRoleInfo.m_strSort.empty())
	{
		return false;
	}

	PROTOCOL::CRoleSync l_RoleSync;

	l_RoleSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_RoleSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_RoleSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_RoleSync.m_oHeader.m_strCmd = "role_sync";
	l_RoleSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_RoleSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_RoleSync.m_oHeader.m_strRequestType = "1";

	l_RoleSync.m_oBody.m_strCode = p_oRoleInfo.m_strCode;
	l_RoleSync.m_oBody.m_strGuid = p_oRoleInfo.m_strGuid;
	l_RoleSync.m_oBody.m_strName = p_oRoleInfo.m_strName;
	l_RoleSync.m_oBody.m_strParentGuid = p_oRoleInfo.m_strParentGuid;
	l_RoleSync.m_oBody.m_strSort = p_oRoleInfo.m_strSort;
	l_RoleSync.m_oBody.m_strSyncType = SyncType;

	std::string l_strMessage = l_RoleSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "SyncRole success:[%s]", l_strMessage.c_str());
	return true;
}