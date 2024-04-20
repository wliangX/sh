#include "Boost.h"
#include "BindInfo.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define SYSTEMID "icc_server_basedata"
#define SUBSYSTEMID "icc_server_basedata"
#define BIND_INFO "BindInfo"

#define USER_BIND_STAFF ("user_bind_staff")
#define USER_BIND_DEPT ("user_bind_dept")
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
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pIJson = ICCGetIJsonFactory()->CreateJson();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_JsonFacPtr = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	/*while (!LoadBindInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.bindinfo failed to load bindinfo info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	//LoadBindInfo();
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bind_request", OnNotifiGetBindInfoRequest);//获取绑定信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_bind_request", OnNotifiSetBindInfo);//设置绑定信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_bind_request", OnNotifiDeleteBindInfoRequest);//删除绑定信息

	ICC_LOG_INFO(m_pLog, "plugin basedata.bindinfo start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "bindinfo stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void ICC::CBusinessImpl::OnNotifiDeleteBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJson = m_JsonFacPtr->CreateJson();
	PROTOCOL::CDeleteBindRequest l_oDeleteBindInfoRequest;
	if (!l_oDeleteBindInfoRequest.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "OnNotifiDeleteBindInfo Message is Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	PROTOCOL::CDeleteBindRequest::CBody& l_oDeleteRequestBody = l_oDeleteBindInfoRequest.m_oBody;
	//m_strType不能为空，m_strFromGuid和m_strToGuid不能同时为空
	if (l_oDeleteRequestBody.m_strType.empty() || (l_oDeleteRequestBody.m_strFromGuid.empty() && l_oDeleteRequestBody.m_strToGuid.empty()))
	{
		ICC_LOG_ERROR(m_pLog, "OnNotifiDeleteBindInfo request parameter is empty type[%s] from_guid[%s]", 
			l_oDeleteRequestBody.m_strType.c_str(), l_oDeleteRequestBody.m_strFromGuid.c_str());
		return;
	}

	CBindInfo l_oBindInfo;
	std::string l_strSyncType = "3";
	l_oBindInfo.m_strType = l_oDeleteRequestBody.m_strType;
	l_oBindInfo.m_strGuid = l_oDeleteRequestBody.m_strGuid;
	l_oBindInfo.m_strFromGuid = l_oDeleteRequestBody.m_strFromGuid;
	l_oBindInfo.m_strToGuid = l_oDeleteRequestBody.m_strToGuid;
	SyncBindInfo(l_oBindInfo, l_strSyncType);

	// SMP 网关已把Redis数据删除，在此获取不到
	/*std::map<std::string, std::string> l_mapBindInfo;
	m_pRedisClient->HGetAll(BIND_INFO, l_mapBindInfo);
	
	CBindInfo l_oBindInfo;
	std::string l_strSyncType = "3";
	if (l_oDeleteRequestBody.m_strFromGuid.empty())
	{
		//删除请求中只包含 type 和 to_guid
		for (auto it = l_mapBindInfo.begin(); it != l_mapBindInfo.end(); ++it)
		{
			l_oBindInfo.Parse(it->second, m_JsonFacPtr->CreateJson());
			if (l_oBindInfo.m_strType == l_oDeleteRequestBody.m_strType
				&& l_oBindInfo.m_strToGuid == l_oDeleteRequestBody.m_strToGuid)
			{
				SyncBindInfo(l_oBindInfo, l_strSyncType);
			}
		}
	}
	else if (l_oDeleteRequestBody.m_strToGuid.empty())
	{
		//删除用户绑定警员关系,此时的删除请求中只包含 type 和 from_guid（user_guid）字段，没有to_guid
		for (auto it = l_mapBindInfo.begin(); it != l_mapBindInfo.end();++it)
		{
			l_oBindInfo.Parse(it->second, m_JsonFacPtr->CreateJson());
			if (l_oBindInfo.m_strType == l_oDeleteRequestBody.m_strType
				&& l_oBindInfo.m_strFromGuid == l_oDeleteRequestBody.m_strFromGuid)
			{				
				SyncBindInfo(l_oBindInfo, l_strSyncType);
			}
		}
	}
	else
	{//删除非用户绑定警员关系
		std::string l_strBindInfo;
		if (!m_pRedisClient->HGet(BIND_INFO, l_oDeleteRequestBody.m_strFromGuid + "_" + l_oDeleteRequestBody.m_strToGuid, l_strBindInfo) || l_strBindInfo.empty())
		{//缓存中没有欲删除的绑定关系			
			ICC_LOG_ERROR(m_pLog, "Not Found BindInfo:From[%s] To[%s]", l_oDeleteRequestBody.m_strFromGuid.c_str(), l_oDeleteRequestBody.m_strToGuid.c_str());
		}
		l_oBindInfo.Parse(l_strBindInfo, m_JsonFacPtr->CreateJson());

		SyncBindInfo(l_oBindInfo, l_strSyncType);
	}*/
}

void CBusinessImpl::OnNotifiGetBindInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	std::string l_strMessage;
	JsonParser::IJsonPtr l_pJson = m_JsonFacPtr->CreateJson();
	PROTOCOL::CGetBindInfoRequest l_oBindInfoRequest;
	if (!l_oBindInfoRequest.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "OnNotifiGetBindInfoRequest Message is Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetBindInfoRequest [%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetBindInfoRespond l_oGetBindInfoRespond;
	GenRespHeader("get_bind_respond", l_oBindInfoRequest.m_oHeader, l_oGetBindInfoRespond.m_oHeader);

	int count = 0;
	std::map<std::string, std::string> l_mapBindInfo;
	m_pRedisClient->HGetAll(BIND_INFO, l_mapBindInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetBindInfoRequest] HGetAll complete! msgid = %s", l_oBindInfoRequest.m_oHeader.m_strMsgid.c_str());

	for (auto it = l_mapBindInfo.begin(); it != l_mapBindInfo.end(); it++)
	{
		CBindInfo l_oBindInfo;
		l_oBindInfo.Parse(it->second, m_JsonFacPtr->CreateJson());

		PROTOCOL::CGetBindInfoRespond::CBindInfo l_CData;
		l_CData.m_strGuid = l_oBindInfo.m_strGuid;
		l_CData.m_strType = l_oBindInfo.m_strType;
		l_CData.m_strFromGuid = l_oBindInfo.m_strFromGuid;
		l_CData.m_strToGuid = l_oBindInfo.m_strToGuid;
		l_CData.m_strSort = l_oBindInfo.m_strSort;
		l_oGetBindInfoRespond.m_oBody.m_vecData.push_back(l_CData);
		count++;
		if (count%MAX_COUNT == 0)
		{
            l_oGetBindInfoRespond.m_oBody.m_strCount = m_pString->Number(l_mapBindInfo.size());
			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			l_strMessage = l_oGetBindInfoRespond.ToString(l_pIJson);
			p_pNotify->Response(l_strMessage, true);
			l_oGetBindInfoRespond.m_oBody.m_vecData.clear();
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}

	l_oGetBindInfoRespond.m_oBody.m_strCount = m_pString->Number(l_mapBindInfo.size());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	l_strMessage = l_oGetBindInfoRespond.ToString(l_pIJson);
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiSetBindInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSetBindInfo request;
	JsonParser::IJsonPtr l_IJson = m_JsonFacPtr->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetBindInfo Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	//1:add 2:update
	std::string l_strSyncType = m_pRedisClient->HExists(BIND_INFO, request.m_oBody.m_strGuid) ? "2" : "1";
	
	//更新缓存
	CBindInfo info;
	info.m_strGuid = request.m_oBody.m_strGuid;
	info.m_strType = request.m_oBody.m_strType;
	info.m_strFromGuid = request.m_oBody.m_strFromGuid;
	info.m_strToGuid = request.m_oBody.m_strToGuid;
	info.m_strSort = request.m_oBody.m_strSort;
	//同步绑定信息
	SyncBindInfo(info, l_strSyncType);
}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
}

bool CBusinessImpl::LoadBindInfo()
{	
	m_pRedisClient->Del(BIND_INFO);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_bind";
	//l_SqlRequest.param["is_delete"] = "false";
	ICC_LOG_DEBUG(m_pLog, "LoadBindInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, " ExecQuery Error Load Time Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return true;
	}
	while (l_result->Next())
	{
		CBindInfo l_oBind;
		l_oBind.m_strGuid = l_result->GetValue("guid");
		l_oBind.m_strFromGuid = l_result->GetValue("from_guid");
		l_oBind.m_strToGuid = l_result->GetValue("to_guid");
		l_oBind.m_strType = l_result->GetValue("type");;
		l_oBind.m_strSort = l_result->GetValue("sort");
		std::string l_strVal = l_oBind.ToJson(m_JsonFacPtr->CreateJson());
		m_pRedisClient->HSet(BIND_INFO, l_oBind.m_strFromGuid + std::string("_") + l_oBind.m_strToGuid, l_strVal);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBindInfo Success,Bind Size[%d]", l_result->RecordSize());
	return true;
}

bool CBusinessImpl::SyncBindInfo(const CBindInfo& p_info, std::string p_strSyncType)
{
	PROTOCOL::CSetBindInfo l_info;
	l_info.m_oHeader.m_strSystemID = SYSTEMID;
	l_info.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_info.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_info.m_oHeader.m_strCmd = "bind_sync";
	l_info.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_info.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_info.m_oHeader.m_strRequestType = "1";

	l_info.m_oBody.m_strGuid = p_info.m_strGuid;
	l_info.m_oBody.m_strType = p_info.m_strType;
	l_info.m_oBody.m_strFromGuid = p_info.m_strFromGuid;
	l_info.m_oBody.m_strToGuid = p_info.m_strToGuid;
	l_info.m_oBody.m_strSort = p_info.m_strSort;
	l_info.m_oBody.m_strSyncType = p_strSyncType;

	std::string l_strMessage = l_info.ToString(m_JsonFacPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "SyncBind success:[%s]", l_strMessage.c_str());
	return true;
}