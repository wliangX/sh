#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define SYSTEMID ("icc_server_basedata")
#define SUBSYSTEMID ("icc_server_basedata")
#define DEPT_TYPE_CODE ("dept_type_code")
#define DEPT_CODE_GUID_INFO ("DeptCodeGuidMap")
#define DEPT_LEVEL_INFO ("DeptLevel:")
const std::string ICC::CBusinessImpl::DeptInfoKey("DeptInfoKey");

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
    m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
    m_pString = ICCGetIStringFactory()->CreateString();
    m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
    m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{	
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	SetCommandDept();

	/*while (!LoadDept())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.dept failed to load dept info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	//LoadDept();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dept_request", OnNotifiGetDeptRequest);	//获取单位信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_sub_dept_request", OnNotifiGetSubDeptRequest);	//获取单位信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_dept_request", OnNotifiSetDeptRequest);	//设置单位信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_dept_request", OnNotifiDeleteDeptRequest);//删除单位信息

	ICC_LOG_INFO(m_pLog, "plugin basedata.dept start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "dept stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiSetDeptRequest(ObserverPattern::INotificationPtr p_pNotifiSetDeptRequest)
{
    PROTOCOL::CSetDeptRequest l_oSetDeptRequest;
    if (!l_oSetDeptRequest.ParseString(p_pNotifiSetDeptRequest->GetMessages(), m_pJsonFty->CreateJson()))
    {
        ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotifiSetDeptRequest->GetMessages().c_str());
        return;
    }

	//标志请求删除的单位信息是否存在于缓存
	//eSyncType l_eSyncType = m_pRedisClient->HExists(DeptInfoKey, l_oSetDeptRequest.m_oBody.m_strGuid) ? eSyncType::ICC_MODIFY : eSyncType::ICC_ADD;
	eSyncType l_eSyncType = l_oSetDeptRequest.m_oBody.m_strSyncType == "2" ? eSyncType::ICC_MODIFY : eSyncType::ICC_ADD;
    
	DataBase::SQLRequest l_oSetDeptSQLReq;
    std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	CDeptInfo l_oDeptCached;
	l_oDeptCached.m_strGuid = l_oSetDeptRequest.m_oBody.m_strGuid;
	l_oDeptCached.m_strParentGuid = l_oSetDeptRequest.m_oBody.m_strParentGuid;
	l_oDeptCached.m_strCode = l_oSetDeptRequest.m_oBody.m_strCode;
	l_oDeptCached.m_strDistrictCode = l_oSetDeptRequest.m_oBody.m_strDistrictCode;
	l_oDeptCached.m_strType = m_pString->AnsiToUtf8(l_oSetDeptRequest.m_oBody.m_strType);
	l_oDeptCached.m_strName = l_oSetDeptRequest.m_oBody.m_strName;
	l_oDeptCached.m_strPhone = l_oSetDeptRequest.m_oBody.m_strPhone;
	l_oDeptCached.m_strShortcut = l_oSetDeptRequest.m_oBody.m_strShortcut;
	l_oDeptCached.m_strSort = l_oSetDeptRequest.m_oBody.m_strSort;

	//更新缓存
	if (SetRedisDetpInfo(l_oDeptCached))
	{
		//同步单位信息
		std::string l_strInfo;
		m_pRedisClient->HGet(DeptInfoKey, l_oSetDeptRequest.m_oBody.m_strGuid, l_strInfo);
		CDeptInfo l_oDeptInfo;
		l_oDeptInfo.Parse(l_strInfo, m_pJsonFty->CreateJson());
		SyncDeptInfo(l_oDeptCached, l_eSyncType);
	}   
}

void CBusinessImpl::OnNotifiGetDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive GetDeptRequest [%s]", p_pNotify->GetMessages().c_str());

    PROTOCOL::CGetDeptRequest l_oGetDeptReq;
	if (!l_oGetDeptReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetDeptRequest] parse json error! msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}	

	std::string l_strUserCode = l_oGetDeptReq.m_oBody.m_strUserCode;
	std::string l_strPageSize = l_oGetDeptReq.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oGetDeptReq.m_oBody.m_strPageIndex;

	std::string l_strAllCount("0");
	std::map<std::string, std::string> l_mapDeptInfo;
	PROTOCOL::CGetDeptRespond l_oGetDeptResp;
	if (!LoadDeptByUser(l_strUserCode, l_strPageSize, l_strPageIndex, l_strAllCount, l_mapDeptInfo))
	{
		ICC_LOG_ERROR(m_pLog, "LoadDeptByUser error, user[%s]", l_oGetDeptReq.m_oBody.m_strUserCode.c_str());

		l_oGetDeptResp.m_oHeader.m_strResult = "1";
		l_oGetDeptResp.m_oHeader.m_strMsg = "Get dept fail";

		l_oGetDeptResp.m_oBody.m_strCount = "0";
		l_oGetDeptResp.m_oBody.m_strAllCount = "0";
		l_oGetDeptResp.m_oBody.m_strPageIndex = l_strPageIndex;

		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);

		return;
	}

	for (auto var : l_mapDeptInfo)
	{
		CDeptInfo l_oDeptInfo;
		l_oDeptInfo.Parse(var.second, m_pJsonFty->CreateJson());

		PROTOCOL::CGetDeptRespond::CDept l_oDept;
		l_oDept.m_strGuid = l_oDeptInfo.m_strGuid;
		l_oDept.m_strParentGuid = l_oDeptInfo.m_strParentGuid;
		l_oDept.m_strCode = l_oDeptInfo.m_strCode;
		l_oDept.m_strDistrictCode = l_oDeptInfo.m_strDistrictCode;
		l_oDept.m_strType = l_oDeptInfo.m_strType;
		l_oDept.m_strName = l_oDeptInfo.m_strName;
		l_oDept.m_strPhone = l_oDeptInfo.m_strPhone;
		l_oDept.m_strShortcut = l_oDeptInfo.m_strShortcut;
		l_oDept.m_strSort = l_oDeptInfo.m_strSort;

		l_oGetDeptResp.m_oBody.m_vecData.push_back(l_oDept);
	}
	std::sort(l_oGetDeptResp.m_oBody.m_vecData.begin(), l_oGetDeptResp.m_oBody.m_vecData.end(),
		[](const PROTOCOL::CGetDeptRespond::CDept& dept1, const PROTOCOL::CGetDeptRespond::CDept& dept2)
		{
			return atoi(dept1.m_strSort.c_str()) < atoi(dept2.m_strSort.c_str());
		});

	l_oGetDeptResp.m_oBody.m_strCount = std::to_string(l_mapDeptInfo.size());
	l_oGetDeptResp.m_oBody.m_strAllCount = l_strAllCount;
	l_oGetDeptResp.m_oBody.m_strPageIndex = l_strPageIndex;

	std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetSubDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive GetSubDeptRequest [%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetSubDeptRequest l_oGetDeptReq;
	if (!l_oGetDeptReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetDeptRequest] parse json error! msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strDeptCode = l_oGetDeptReq.m_oBody.m_strDeptCode;
	std::string l_strPageSize = l_oGetDeptReq.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oGetDeptReq.m_oBody.m_strPageIndex;

	std::string l_strDeptGuid;
	PROTOCOL::CGetDeptRespond l_oGetDeptResp;
	m_pRedisClient->HGet(DEPT_CODE_GUID_INFO, l_strDeptCode, l_strDeptGuid);
	if (l_strDeptGuid.empty())
	{
		l_oGetDeptResp.m_oHeader.m_strResult = "1";
		l_oGetDeptResp.m_oHeader.m_strMsg = "Get dept fail";

		l_oGetDeptResp.m_oBody.m_strCount = "0";
		l_oGetDeptResp.m_oBody.m_strAllCount = "0";
		l_oGetDeptResp.m_oBody.m_strPageIndex = l_strPageIndex;

		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		return;
	}

	std::map<std::string, std::string> l_mapDeptInfo;
	std::vector<PROTOCOL::CGetDeptRespond::CDept> l_vecDept;
	m_pRedisClient->HGetAll(DEPT_LEVEL_INFO + l_strDeptGuid, l_mapDeptInfo);
	for (auto var : l_mapDeptInfo)
	{
		CDeptInfo l_oDeptInfo;
		l_oDeptInfo.Parse(var.second, m_pJsonFty->CreateJson());

		PROTOCOL::CGetDeptRespond::CDept l_oDept;
		l_oDept.m_strGuid = l_oDeptInfo.m_strGuid;
		l_oDept.m_strParentGuid = l_oDeptInfo.m_strParentGuid;
		l_oDept.m_strCode = l_oDeptInfo.m_strCode;
		l_oDept.m_strDistrictCode = l_oDeptInfo.m_strDistrictCode;
		l_oDept.m_strType = l_oDeptInfo.m_strType;
		l_oDept.m_strName = l_oDeptInfo.m_strName;
		l_oDept.m_strPhone = l_oDeptInfo.m_strPhone;
		l_oDept.m_strShortcut = l_oDeptInfo.m_strShortcut;
		l_oDept.m_strSort = l_oDeptInfo.m_strSort;

		l_vecDept.push_back(l_oDept);
	}
	std::sort(l_vecDept.begin(), l_vecDept.end(),
		[](const PROTOCOL::CGetDeptRespond::CDept& dept1, const PROTOCOL::CGetDeptRespond::CDept& dept2)
		{
			return atoi(dept1.m_strSort.c_str()) < atoi(dept2.m_strSort.c_str());
		});

	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());
	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
	int l_nEndIndex = l_nPageIndex * l_nPageSize;
	if (l_nBeginIndex >= l_vecDept.size())
	{
		l_oGetDeptResp.m_oHeader.m_strResult = "1";
		l_oGetDeptResp.m_oHeader.m_strMsg = "Out of range";
		l_oGetDeptResp.m_oBody.m_strCount = "0";
	}
	else
	{
		for (auto it = l_vecDept.begin() + l_nBeginIndex;
			it < (l_vecDept.begin() + l_nEndIndex) && it != l_vecDept.end();
			it++)
		{
			l_oGetDeptResp.m_oBody.m_vecData.push_back(*it);
		}

		l_oGetDeptResp.m_oBody.m_strCount = std::to_string(l_oGetDeptResp.m_oBody.m_vecData.size());
	}

	l_oGetDeptResp.m_oBody.m_strAllCount = std::to_string(l_vecDept.size());;
	l_oGetDeptResp.m_oBody.m_strPageIndex = l_strPageIndex;

	std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiDeleteDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
    PROTOCOL::CDeleteDeptRequest l_oDeleDeptRequest;
	if (!l_oDeleDeptRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
    {
		ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
        return;
    }
    
	// 处警超时缓存
	m_pRedisClient->HDel(DEPT_TYPE_CODE, l_oDeleDeptRequest.m_oBody.m_strCode);

	//同步单位信息
	CDeptInfo l_oDeptInfo;
	l_oDeptInfo.m_strGuid = l_oDeleDeptRequest.m_oBody.m_strGuid;
	l_oDeptInfo.m_strCode = l_oDeleDeptRequest.m_oBody.m_strCode;
	SyncDeptInfo(l_oDeptInfo, eSyncType::ICC_DELETE);
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

bool CBusinessImpl::SyncDeptInfo(const CDeptInfo &p_oDept, eSyncType p_eSyncType)
{
    PROTOCOL::CDeptSync l_oDeptSync;
    l_oDeptSync.m_oHeader.m_strSystemID = SYSTEMID;
    l_oDeptSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
    l_oDeptSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
    l_oDeptSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
    l_oDeptSync.m_oHeader.m_strCmd = DEPT_SYNC;
    l_oDeptSync.m_oHeader.m_strRequest = "topic_sync";
    l_oDeptSync.m_oHeader.m_strRequestType = "1";

	l_oDeptSync.m_oBody.m_strSyncType = m_pString->Number(p_eSyncType);
    l_oDeptSync.m_oBody.m_strGuid = p_oDept.m_strGuid;
    l_oDeptSync.m_oBody.m_strParentGuid = p_oDept.m_strParentGuid;
    l_oDeptSync.m_oBody.m_strCode = p_oDept.m_strCode;
    l_oDeptSync.m_oBody.m_strDistrictCode = p_oDept.m_strDistrictCode;
    l_oDeptSync.m_oBody.m_strType = p_oDept.m_strType;
    l_oDeptSync.m_oBody.m_strName = p_oDept.m_strName;
    l_oDeptSync.m_oBody.m_strPhone = p_oDept.m_strPhone;
    l_oDeptSync.m_oBody.m_strShortcut = p_oDept.m_strShortcut;
    l_oDeptSync.m_oBody.m_strSort = p_oDept.m_strSort;

	JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
    if (!l_pJson)
    {
        ICC_LOG_ERROR(m_pLog, "sync dept failed to create json parser");
        return false;
    }
    std::string l_strSyncMsg(l_oDeptSync.ToString(l_pJson));
    if (l_strSyncMsg.empty())
    {
        ICC_LOG_ERROR(m_pLog, "sync dept failed to generate sync msg");
        return false;
    }

    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
    ICC_LOG_INFO(m_pLog, "sync dept:[%s]", l_strSyncMsg.c_str());
    return true;
}

bool CBusinessImpl::LoadDept()
{
	m_pRedisClient->Del(DeptInfoKey);

	bool loadtag = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dept";
	l_SqlRequest.param["is_delete"] = "false";

	ICC_LOG_DEBUG(m_pLog, "LoadingDeptInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	std::map<std::string, std::string> tmp_mapDeptInfo;
	std::map<std::string, std::string> l_mapDeptCodeInfo;
	std::map<std::string, std::map<std::string, std::string>> l_mapDeptLevelInfo;

	while (l_result->Next())
	{
		CDeptInfo pCDeptInfo;
		pCDeptInfo.m_strGuid = l_result->GetValue("guid");
		pCDeptInfo.m_strParentGuid = l_result->GetValue("parent_guid");
		pCDeptInfo.m_strCode = l_result->GetValue("code");
		pCDeptInfo.m_strDistrictCode = l_result->GetValue("district_code");
		pCDeptInfo.m_strName = l_result->GetValue("name");
		pCDeptInfo.m_strType = l_result->GetValue("type");
		pCDeptInfo.m_strPhone = l_result->GetValue("phone");
		pCDeptInfo.m_strLevel = l_result->GetValue("level");
		pCDeptInfo.m_strShortcut = l_result->GetValue("shortcut");
		pCDeptInfo.m_strSort = l_result->GetValue("sort");

		std::map<std::string, std::string> l_mapTemp;
		tmp_mapDeptInfo.insert(std::make_pair(pCDeptInfo.m_strGuid, pCDeptInfo.ToJson(m_pJsonFty->CreateJson())));
		l_mapDeptCodeInfo.insert(std::make_pair(pCDeptInfo.m_strCode, pCDeptInfo.m_strGuid));

		//后续需修改
		auto it = l_mapDeptLevelInfo.find(pCDeptInfo.m_strParentGuid);
		if (it != l_mapDeptLevelInfo.end())
		{
			it->second.insert(std::make_pair(pCDeptInfo.m_strGuid, pCDeptInfo.ToJson(m_pJsonFty->CreateJson())));
		} 
		else
		{
			std::map<std::string, std::string> l_mapData;
			l_mapData.insert(std::make_pair(pCDeptInfo.m_strGuid, pCDeptInfo.ToJson(m_pJsonFty->CreateJson())));

			l_mapDeptLevelInfo.insert(std::make_pair(pCDeptInfo.m_strParentGuid, l_mapData));
		}

		if (!SetRedisDetpInfo(pCDeptInfo))
		{
			ICC_LOG_ERROR(m_pLog, "Load DeptInfo to Redis Error");
			return false;
		}
	}

	if (!tmp_mapDeptInfo.empty())
	{
		m_pRedisClient->HMSet(DeptInfoKey, tmp_mapDeptInfo);
	}
	if (!l_mapDeptCodeInfo.empty())
	{
		m_pRedisClient->HMSet(DEPT_CODE_GUID_INFO, l_mapDeptCodeInfo);
	}

	for (auto it : l_mapDeptLevelInfo)
	{
		m_pRedisClient->HMSet(DEPT_LEVEL_INFO + it.first, it.second);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadingDeptInfo Success,Dept Size[%d]", l_result->RecordSize());
	return true;
}

bool ICC::CBusinessImpl::LoadDeptByUser(const std::string& p_strInUserCode, const std::string& p_strInPageSize, const std::string& p_strInPageIndex,
	std::string& p_strAllCount, std::map<std::string, std::string>& p_mapOutDeptInfo)
{
	DataBase::SQLRequest l_SqlCountRequest;
	l_SqlCountRequest.sql_id = "select_dept_count_by_user";
	l_SqlCountRequest.param["code"] = p_strInUserCode;

	DataBase::IResultSetPtr l_resultCount = m_pDBConn->Exec(l_SqlCountRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_resultCount->GetSQL().c_str());
	if (!l_resultCount->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultCount->GetErrorMsg().c_str());
		return false;
	}
	if (l_resultCount->Next())
	{
		p_strAllCount = l_resultCount->GetValue("num");

		ICC_LOG_DEBUG(m_pLog, "Get dept count by user[%s] success, all dept size[%s]", p_strInUserCode.c_str(), p_strAllCount.c_str());
	}

	bool loadtag = false;
	int l_nPageSize = std::atoi(p_strInPageSize.c_str());
	int l_nPageIndex = std::atoi(p_strInPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;
	l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_dept_by_user";
	l_SqlRequest.param["code"] = p_strInUserCode;
	l_SqlRequest.param["limit"] = std::to_string(l_nPageSize);
	l_SqlRequest.param["offset"] = std::to_string(l_nPageIndex);

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	while (l_result->Next())
	{
		CDeptInfo pCDeptInfo;
		pCDeptInfo.m_strGuid = l_result->GetValue("to_guid");
		pCDeptInfo.m_strParentGuid = l_result->GetValue("parent_guid");
		pCDeptInfo.m_strCode = l_result->GetValue("dept_code");
		pCDeptInfo.m_strDistrictCode = l_result->GetValue("district_code");
		pCDeptInfo.m_strName = l_result->GetValue("name");
		pCDeptInfo.m_strType = l_result->GetValue("dept_type");
		pCDeptInfo.m_strPhone = l_result->GetValue("phone");
		pCDeptInfo.m_strLevel = l_result->GetValue("level");
		pCDeptInfo.m_strShortcut = l_result->GetValue("shortcut");
		pCDeptInfo.m_strSort = l_result->GetValue("sort");

		p_mapOutDeptInfo[pCDeptInfo.m_strGuid] = pCDeptInfo.ToJson(m_pJsonFty->CreateJson());
	}

	ICC_LOG_DEBUG(m_pLog, "Get dept by user[%s] success, dept size[%d]", p_strInUserCode.c_str(), p_mapOutDeptInfo.size());
	return true;
}

bool ICC::CBusinessImpl::SetRedisDetpInfo(CDeptInfo& p_oDeptInfo)
{
	for (auto & var : m_lstCommCode)
	{
		if (p_oDeptInfo.m_strType == var)
		{
			// 处警单超时使用 [7/5/2018 w26326]
			m_pRedisClient->HSet(DEPT_TYPE_CODE, p_oDeptInfo.m_strCode, p_oDeptInfo.m_strType);
			break;
		}
	}
	std::string l_strVal = p_oDeptInfo.ToJson(m_pJsonFty->CreateJson());
	return m_pRedisClient->HSet(DeptInfoKey, p_oDeptInfo.m_strGuid, l_strVal);
}

void CBusinessImpl::SetCommandDept()
{
	unsigned int l_Count = m_pConfig->GetNodeCount("ICC/Plugin/Dept/ProcessTimeoutDeptTypes", "ProcessTimeoutDeptType");
	for (unsigned int l_index = 0; l_index < l_Count; l_index++)
	{
		std::string l_strIndex = m_pConfig->GetValue(m_pString->Format("ICC/Plugin/Dept/ProcessTimeoutDeptTypes/ProcessTimeoutDeptType[%d]", l_index), "");
		m_lstCommCode.push_back(l_strIndex);
	}
}
