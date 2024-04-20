#include "Boost.h"
#include "BusinessImpl.h"

#include <thread>

#define LOAD_WAITTIME 1000

#define SYSTEMID ("icc_server_basedata")
#define SUBSYSTEMID ("icc_server_basedata")


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
	m_JsonFacPtr = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{  
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	/*while (!LoadStaffInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin started failed,error:failed to load staff info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_staff_request", OnNotifiGetStaffRequest);//获取警员信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_staff_request", OnNotifiSetStaffRequest);//设置警员信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_staff_request", OnNotifiDeleteStaffRequest);//删除警员信息

	ICC_LOG_INFO(m_pLog, "plugin basedata.staff started success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "plugin basedata.staff stop success");
}

void CBusinessImpl::OnDestroy()
{

}

/*
* 方法: CBusiness::OnNotifiGetStaffRequest
* 功能: 响应获取警员信息请求，并分包返回结果消息
* 参数: CNotifiGetStaffRequest * p_pNotifiGetStaffRequest
* 返回: void
* 作者：[3/6/2018 t26150]
*/
void CBusinessImpl::OnNotifiGetStaffRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive GetStaffRequest:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CGetStaffRequest l_oReq;
	if (!l_oReq.ParseString(p_pNotify->GetMessages(), m_JsonFacPtr->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "GetStaffRequest parse json error! msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strDeptCode = l_oReq.m_oBody.m_strDeptCode;
	std::string l_strPageSize = l_oReq.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oReq.m_oBody.m_strPageIndex;
	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	std::string l_strDeptGuid;
	PROTOCOL::CGetStaffRespond l_oGetStaffResp;
	m_pRedisClient->HGet("DeptCodeGuidInfo", l_strDeptCode, l_strDeptGuid);
	if (l_strDeptGuid.empty())
	{
		l_oGetStaffResp.m_oHeader.m_strResult = "1";
		l_oGetStaffResp.m_oHeader.m_strMsg = "Get dept fail";

		l_oGetStaffResp.m_oBody.m_strCount = "0";
		l_oGetStaffResp.m_oBody.m_strAllCount = "0";
		l_oGetStaffResp.m_oBody.m_strPageIndex = l_strPageIndex;

		std::string l_strMessage = l_oGetStaffResp.ToString(m_JsonFacPtr->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		return;
	}

	std::map<std::string, std::string> l_mapStaffInfo;
	m_pRedisClient->HGetAll(STAFF_INFO_KEY, l_mapStaffInfo);
	ICC_LOG_DEBUG(m_pLog, "HGetAll complete! Staff size = %d", l_mapStaffInfo.size());

	if (l_mapStaffInfo.size() == 0)
	{
		l_oGetStaffResp.m_oBody.m_strCount = "0";
		l_oGetStaffResp.m_oBody.m_strAllCount = "0";
		l_oGetStaffResp.m_oBody.m_strPageIndex = l_strPageIndex;

		std::string l_strMessage = l_oGetStaffResp.ToString(m_JsonFacPtr->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		std::vector<PROTOCOL::CGetStaffRespond::CStaff> l_vecStaff;

		for (auto it = l_mapStaffInfo.begin(); it != l_mapStaffInfo.end(); it++)
		{
			CStaffInfo l_oStaffInfo;
			JsonParser::IJsonPtr l_pJson = m_JsonFacPtr->CreateJson();
			if (l_pJson && l_oStaffInfo.Parse(it->second, l_pJson) && 
				l_oStaffInfo.m_strDeptGuid.compare(l_strDeptGuid) == 0)
			{
				PROTOCOL::CGetStaffRespond::CStaff l_oStaff;
				l_oStaff.m_strGuid = l_oStaffInfo.m_strGuid;
				l_oStaff.m_strDeptGuid = l_oStaffInfo.m_strDeptGuid;
				l_oStaff.m_strCode = l_oStaffInfo.m_strCode;
				l_oStaff.m_strName = l_oStaffInfo.m_strName;
				l_oStaff.m_strSex = l_oStaffInfo.m_strSex;
				l_oStaff.m_strType = l_oStaffInfo.m_strType;
				l_oStaff.m_strPosition = l_oStaffInfo.m_strPosition;
				l_oStaff.m_strMobile = l_oStaffInfo.m_strMobile;
				l_oStaff.m_strPhone = l_oStaffInfo.m_strPhone;
				l_oStaff.m_strShortcut = l_oStaffInfo.m_strShortcut;
				l_oStaff.m_strSort = l_oStaffInfo.m_strSort;
				l_oStaff.m_strIsLeader = l_oStaffInfo.m_strIsLeader;
				//l_oGetStaffResp.m_oBody.m_vecData.push_back(l_oStaff);

				l_vecStaff.push_back(l_oStaff);
			}
		}

		std::sort(l_vecStaff.begin(), l_vecStaff.end(),
			[](const PROTOCOL::CGetStaffRespond::CStaff& lp, const PROTOCOL::CGetStaffRespond::CStaff& rp)
			{
				return std::atol(lp.m_strSort.c_str()) < std::atol(rp.m_strSort.c_str());
			}
		);

		int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
		int l_nEndIndex = l_nPageIndex * l_nPageSize;
		if (l_nBeginIndex >= l_vecStaff.size())
		{
			l_oGetStaffResp.m_oHeader.m_strResult = "1";
			l_oGetStaffResp.m_oHeader.m_strMsg = "Out of range";
			l_oGetStaffResp.m_oBody.m_strCount = "0";
		} 
		else
		{
			for (auto it = l_vecStaff.begin() + l_nBeginIndex;
				it < (l_vecStaff.begin() + l_nEndIndex) && it != l_vecStaff.end();
				it++)
			{
				l_oGetStaffResp.m_oBody.m_vecData.push_back(*it);
			}

			l_oGetStaffResp.m_oBody.m_strCount = std::to_string(l_oGetStaffResp.m_oBody.m_vecData.size());
		}
		
		l_oGetStaffResp.m_oBody.m_strAllCount = std::to_string(l_vecStaff.size());
		l_oGetStaffResp.m_oBody.m_strPageIndex = l_strPageIndex;
		std::string l_strMessage = l_oGetStaffResp.ToString(m_JsonFacPtr->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send staff info msg [%s]", l_strMessage.c_str());

	}
	//ICC_LOG_INFO(m_pLog, "send staff info msg [total:%d]", l_mapStaffInfo.size());
}

void CBusinessImpl::OnNotifiSetStaffRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonFactoryPtr l_pJsonFty = ICCGetIJsonFactory();
	std::string l_strMsg(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "SetStaffRequest Recv:[%s]", l_strMsg.c_str());
	PROTOCOL::CSetStaffRequest l_oSetStaffRequest;
	if (!l_oSetStaffRequest.ParseString(l_strMsg, l_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "set staff request is not json:[%s]", l_strMsg.c_str());
		return;
	}	

	//ESyncType l_eSyncType = m_pRedisClient->HExists(STAFF_INFO_KEY, l_oSetStaffRequest.m_oBody.m_strGuid) ? ESyncType::UPDATE : ESyncType::ADD;
	ESyncType l_eSyncType = (l_oSetStaffRequest.m_oBody.m_strSyncType == "2" ? ESyncType::UPDATE : ESyncType::ADD);
	
	CStaffInfo l_oStaffInfoRecv;
	l_oStaffInfoRecv.m_strGuid = l_oSetStaffRequest.m_oBody.m_strGuid;
	l_oStaffInfoRecv.m_strDeptGuid = l_oSetStaffRequest.m_oBody.m_strDeptGuid;
	l_oStaffInfoRecv.m_strCode = l_oSetStaffRequest.m_oBody.m_strCode;
	l_oStaffInfoRecv.m_strName = l_oSetStaffRequest.m_oBody.m_strName;
	l_oStaffInfoRecv.m_strSex = l_oSetStaffRequest.m_oBody.m_strSex;
	l_oStaffInfoRecv.m_strType = l_oSetStaffRequest.m_oBody.m_strType;
	l_oStaffInfoRecv.m_strPosition = l_oSetStaffRequest.m_oBody.m_strPosition;
	l_oStaffInfoRecv.m_strMobile = l_oSetStaffRequest.m_oBody.m_strMobile;
	l_oStaffInfoRecv.m_strPhone = l_oSetStaffRequest.m_oBody.m_strPhone;
	l_oStaffInfoRecv.m_strShortcut = l_oSetStaffRequest.m_oBody.m_strShortcut;
	l_oStaffInfoRecv.m_strSort = l_oSetStaffRequest.m_oBody.m_strSort;
	l_oStaffInfoRecv.m_strIsLeader = l_oSetStaffRequest.m_oBody.m_strIsLeader;

	SyncStaffInfo(l_oStaffInfoRecv, l_eSyncType);
}

void CBusinessImpl::OnNotifiDeleteStaffRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CDeleteStaffRequest l_oDeleStaffRequest;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	JsonParser::IJsonFactoryPtr l_pJsonFty = ICCGetIJsonFactory();

	if (!l_oDeleStaffRequest.ParseString(l_strRecvMsg, l_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "delete_staff_request request not json:[%s]", l_strRecvMsg.c_str());
		return;
	}

	CStaffInfo l_oDeleteStaff;
	l_oDeleteStaff.m_strGuid = l_oDeleStaffRequest.m_oBody.m_strStaffGuid;
	SyncStaffInfo(l_oDeleteStaff, DELETE);
}

/*
* 方法: CStaffBusiness::SyncStaffInfo
* 功能: 同步警员信息到主题 topic_sync
* 参数: const CStaffInfo & p_oStaff
* 参数: const IJsonFactoryPtr & l_pJsonFty
* 返回: bool 同步信息是否成功，成功true，失败false
* 作者：[3/10/2018 t26150]
*/
bool CBusinessImpl::SyncStaffInfo(const CStaffInfo &p_oStaff, ESyncType p_eSyncType)
{
	PROTOCOL::CStaffSync l_oStaffSync;
	l_oStaffSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oStaffSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oStaffSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oStaffSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oStaffSync.m_oHeader.m_strCmd = "staff_sync";
	l_oStaffSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oStaffSync.m_oHeader.m_strRequestType = "1";

	l_oStaffSync.m_oBody.m_strSyncType = std::to_string(p_eSyncType);
	l_oStaffSync.m_oBody.m_strGuid = p_oStaff.m_strGuid;
	l_oStaffSync.m_oBody.m_strDeptGuid = p_oStaff.m_strDeptGuid;
	l_oStaffSync.m_oBody.m_strCode = p_oStaff.m_strCode;
	l_oStaffSync.m_oBody.m_strName = p_oStaff.m_strName;
	l_oStaffSync.m_oBody.m_strSex = p_oStaff.m_strSex;
	l_oStaffSync.m_oBody.m_strType = p_oStaff.m_strType;
	l_oStaffSync.m_oBody.m_strPosition = p_oStaff.m_strPosition;
	l_oStaffSync.m_oBody.m_strMobile = p_oStaff.m_strMobile;
	l_oStaffSync.m_oBody.m_strPhone = p_oStaff.m_strPhone;
	l_oStaffSync.m_oBody.m_strShortcut = p_oStaff.m_strShortcut;
	l_oStaffSync.m_oBody.m_strSort = p_oStaff.m_strSort;
	l_oStaffSync.m_oBody.m_strIsLeader = p_oStaff.m_strIsLeader;

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (l_pJson == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "SyncStaffInfo failed to create json parser");
		return false;
	}
	std::string l_strStaffSyncMsg(l_oStaffSync.ToString(l_pJson));
	if (l_strStaffSyncMsg.empty())
	{
		ICC_LOG_ERROR(m_pLog, "SyncStaffInfo failed to generate sync msg");
		return false;
	}

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strStaffSyncMsg));
	ICC_LOG_INFO(m_pLog, "Send SyncStaffInfo Msg:[%s]", l_strStaffSyncMsg.c_str());
	return true;
}

void CBusinessImpl::GeneralHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
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

bool CBusinessImpl::LoadStaffInfo()
{
	ICC_LOG_DEBUG(m_pLog, "loading staff info begin");

	m_pRedisClient->Del(STAFF_INFO_KEY);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_staff";
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "execquery error ,error message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	std::map<std::string, std::string> tmp_mapStaffInfo;
	int l_iCount = 0;
	while (l_result->Next())
	{
		CStaffInfo l_oLoadStaff;
		l_oLoadStaff.m_strGuid = l_result->GetValue("guid");
		l_oLoadStaff.m_strDeptGuid = l_result->GetValue("dept_guid");
		l_oLoadStaff.m_strCode = l_result->GetValue("code");
		l_oLoadStaff.m_strName = l_result->GetValue("name");
		l_oLoadStaff.m_strSex = l_result->GetValue("sex");
		l_oLoadStaff.m_strType = l_result->GetValue("type");
		l_oLoadStaff.m_strPosition = l_result->GetValue("post");
		l_oLoadStaff.m_strMobile = l_result->GetValue("mobile");
		l_oLoadStaff.m_strPhone = l_result->GetValue("phone");
		l_oLoadStaff.m_strShortcut = l_result->GetValue("shortcut");
		l_oLoadStaff.m_strSort = l_result->GetValue("sort");
		l_oLoadStaff.m_strIsLeader = l_result->GetValue("isleader");

		tmp_mapStaffInfo[l_oLoadStaff.m_strGuid] = l_oLoadStaff.ToJson(m_JsonFacPtr->CreateJson());
		l_iCount++;
		//m_mapStaffInfo.insert(std::make_pair(l_oLoadStaff.m_strGuid, l_oLoadStaff));
		//std::string l_strVal = l_oLoadStaff.ToJson(m_JsonFacPtr->CreateJson());
		//m_pRedisClient->HSet(StaffInfoKey, l_oLoadStaff.m_strGuid, l_strVal);
	}

	if (!tmp_mapStaffInfo.empty())
	{
		m_pRedisClient->HMSet(STAFF_INFO_KEY, tmp_mapStaffInfo);
	}
	
	ICC_LOG_DEBUG(m_pLog, "loading staff info success, staff size[%d] db_number[%d] sql[%s]", tmp_mapStaffInfo.size(), l_iCount, l_result->GetSQL().c_str());
	return true;
}

