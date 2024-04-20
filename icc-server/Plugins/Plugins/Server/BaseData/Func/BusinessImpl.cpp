#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define SYSTEMID ("icc_server_basedata")
#define SUBSYSTEMID ("icc_server_basedata")

#define Func ("Func")

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
	m_JsonFacPtr = ICCGetIJsonFactory();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
    m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pIJson = ICCGetIJsonFactory()->CreateJson();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();	
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{	
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	/*while (!LoadFuncInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.func failed to load func info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	//LoadFuncInfo();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_func_request", OnNotifiGetFuncRequest);//获取功能权限信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_func_request", OnNotifiSetFuncRequest);//设置功能权限信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_func_request", OnNotifiDeleteFuncRequest);//删除功能权限信息

	ICC_LOG_INFO(m_pLog, "plugin basedata.func started success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "func stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiGetFuncRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	std::string l_strMessage;
	JsonParser::IJsonPtr l_pJson = m_JsonFacPtr->CreateJson();
	PROTOCOL::CGetFuncRequest l_oGetFuncRequest;
	if (!l_oGetFuncRequest.ParseString(p_pNotify->GetMessages(), l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "OnNotifiGetFuncRequest ParseString Error, msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetFuncRespond l_oGetFuncRespond;
	std::map<std::string, std::string> l_mapFuncInfo;
	m_pRedisClient->HGetAll(Func, l_mapFuncInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetFuncRequest] HGetAll complete! msgid = %s", l_oGetFuncRequest.m_oHeader.m_strMsgid.c_str());

	for (auto it = l_mapFuncInfo.begin(); it != l_mapFuncInfo.end(); it++)
	{
		CFuncInfo l_oFunc;
		l_oFunc.Parse(it->second, m_JsonFacPtr->CreateJson());

		PROTOCOL::CGetFuncRespond::CBody::CData l_oData;
		l_oData.m_strCode = l_oFunc.m_strCode;
		l_oData.m_strGuid = l_oFunc.m_strGuid;
		l_oData.m_strName = l_oFunc.m_strName;
		l_oData.m_strParentGuid = l_oFunc.m_strParentGuid;
		l_oData.m_strSort = l_oFunc.m_strSort;
		l_oGetFuncRespond.m_oBody.m_vecData.push_back(l_oData);
	}
	int l_nCount = l_mapFuncInfo.size();
	l_oGetFuncRespond.m_oBody.m_strCount = m_pString->Number(l_nCount);
	GenRespHeader("get_func_respond", l_oGetFuncRequest.m_oHeader, l_oGetFuncRespond.m_oHeader);

	l_strMessage = l_oGetFuncRespond.ToString(m_JsonFacPtr->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_INFO(m_pLog, "FuncFunc Info Msg Sended total:[%d]", l_mapFuncInfo.size());
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiSetFuncRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSetFuncRequest l_oSetFuncRequest;
	JsonParser::IJsonPtr l_IJson = m_JsonFacPtr->CreateJson();
	if (!l_oSetFuncRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetFuncRequestRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	PROTOCOL::CSetFuncRespond l_oSetFuncRespond;
	l_oSetFuncRespond.m_oBody.m_strResult = "1";//失败
	GenRespHeader("set_func_respond", l_oSetFuncRequest.m_oHeader, l_oSetFuncRespond.m_oHeader);

	//ESyncType l_oSyncType = m_pRedisClient->HExists(Func, l_oSetFuncRequest.m_oBody.m_strGuid) ? ESyncType::SYNC_TYPE_UPDATE : ESyncType::SYNC_TYPE_ADD;
	ESyncType l_oSyncType = (l_oSetFuncRequest.m_oBody.m_strSyncType == "2" ? ESyncType::SYNC_TYPE_UPDATE : ESyncType::SYNC_TYPE_ADD);

	//更新缓存		
	CFuncInfo l_oFunc;
	l_oFunc.m_strGuid = l_oSetFuncRequest.m_oBody.m_strGuid;
	l_oFunc.m_strCode = l_oSetFuncRequest.m_oBody.m_strCode;
	l_oFunc.m_strName = l_oSetFuncRequest.m_oBody.m_strName;
	l_oFunc.m_strParentGuid = l_oSetFuncRequest.m_oBody.m_strParentGuid;
	l_oFunc.m_strSort = l_oSetFuncRequest.m_oBody.m_strSort;

	SyncFuncInfo(l_oFunc, l_oSyncType);
}

void CBusinessImpl::OnNotifiDeleteFuncRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJsonPtr = m_JsonFacPtr->CreateJson();
	PROTOCOL::CDeleteFuncRequest l_oDeleFuncRequest;
	if (!l_oDeleFuncRequest.ParseString(p_pNotify->GetMessages(), l_pJsonPtr))
	{
		ICC_LOG_ERROR(m_pLog, "DeleteFuncRequest Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	CFuncInfo l_tFunc;
	l_tFunc.m_strGuid = l_oDeleFuncRequest.m_oBody.m_strGuid;
	//同步功能权限信息				
	SyncFuncInfo(l_tFunc, ESyncType::SYNC_TYPE_DELETE);
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

bool CBusinessImpl::LoadFuncInfo()
{
	m_pRedisClient->Del(Func);

	bool l_bLoadFuncSuccess = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_func";
	
	ICC_LOG_DEBUG(m_pLog, "LoadFuncInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		CFuncInfo l_oFunc;
		l_oFunc.m_strGuid = l_result->GetValue("guid");
		l_oFunc.m_strCode = l_result->GetValue("code");
		l_oFunc.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oFunc.m_strName = l_result->GetValue("name");
		l_oFunc.m_strSort = l_result->GetValue("sort");

		std::string l_strVal = l_oFunc.ToJson(m_JsonFacPtr->CreateJson());
		m_pRedisClient->HSet(Func, l_oFunc.m_strGuid, l_strVal);
	}
		
	ICC_LOG_DEBUG(m_pLog, "LoadFuncInfo Success,Func Size[%d]", l_result->RecordSize());

	return true;
}

bool CBusinessImpl::SyncFuncInfo(const CFuncInfo& p_tFunc,const ESyncType& p_roSyncType)
{
	if (p_tFunc.m_strCode.empty() &&
		p_tFunc.m_strGuid.empty() &&
		p_tFunc.m_strName.empty() &&
		p_tFunc.m_strParentGuid.empty() &&
		p_tFunc.m_strSort.empty())
	{
		return false;
	}

	PROTOCOL::CFuncSync l_FuncSync;

	l_FuncSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_FuncSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_FuncSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_FuncSync.m_oHeader.m_strCmd = "func_sync";
	l_FuncSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_FuncSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_FuncSync.m_oHeader.m_strRequestType = "1";

	l_FuncSync.m_oBody.m_strSyncType = std::to_string(p_roSyncType);
	l_FuncSync.m_oBody.m_strCode = p_tFunc.m_strCode;
	l_FuncSync.m_oBody.m_strGuid = p_tFunc.m_strGuid;
	l_FuncSync.m_oBody.m_strName = p_tFunc.m_strName;
	l_FuncSync.m_oBody.m_strParentGuid = p_tFunc.m_strParentGuid;
	l_FuncSync.m_oBody.m_strSort = p_tFunc.m_strSort;

	std::string l_strMessage = l_FuncSync.ToString(m_JsonFacPtr->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_INFO(m_pLog, "SyncFunc success:[%s]", l_strMessage.c_str());
	return true;
}