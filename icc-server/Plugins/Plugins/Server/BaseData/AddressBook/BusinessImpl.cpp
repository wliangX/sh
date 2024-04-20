#include "Boost.h"
#include "BusinessImpl.h"

#define SYSTEMID			"icc_server_basedata"
#define SUBSYSTEMID			"icc_server_basedata_addressbook"

#define SET_DEPT_REQUEST	"set_org_book_request"
#define SET_DEPT_RESPOND	"set_org_book_respond"
#define GET_DEPT_REQUEST	"get_org_book_request"
#define GET_DEPT_RESPOND	"get_org_book_respond"
#define DEL_DEPT_REQUEST	"delete_org_book_request"
#define DEL_DEPT_RESPOND	"delete_org_book_respond"

#define SET_PHONE_REQUEST	"set_phone_book_request"
#define SET_PHONE_RESPOND	"set_phone_book_respond"
#define GET_PHONE_REQUEST	"get_phone_book_request"
#define GET_PHONE_RESPOND	"get_phone_book_respond"
#define DEL_PHONE_REQUEST	"delete_phone_book_request"
#define DEL_PHONE_RESPOND	"delete_phone_book_respond"

#define SET_GRP_REQUEST		"set_contact_group_request"
#define SET_GRP_RESPOND		"set_contact_group_respond"
#define GET_GRP_REQUEST		"get_contact_group_request"
#define GET_GRP_RESPOND		"get_contact_group_respond"
#define DEL_GRP_REQUEST		"delete_contact_group_request"
#define DEL_GRP_RESPOND		"delete_contact_group_respond"

#define SET_CONTACT_REQUEST	"set_contact_info_request"
#define SET_CONTACT_RESPOND	"set_contact_info_respond"
#define GET_CONTACT_REQUEST	"get_contact_info_request"
#define GET_CONTACT_RESPOND	"get_contact_info_respond"
#define DEL_CONTACT_REQUEST	"delete_contact_info_request"
#define DEL_CONTACT_RESPON	"delete_contact_info_respond"

#define CMD_DEPT_SYNC		"org_book_sync"
#define CMD_PHONE_SYNC		"phone_book_sync"
#define CMD_GRP_SYNC		"contact_group_sync"
#define CMD_CONTACT_SYNC	"contact_info_sync"

#define TOPIC_BOOK_SYNC		"topic_book_sync"
#define QUEUE_PHONE_BOOK	"queue_phone_book"

#define ADDRESS_BOOK_DEPT			"AddressBookOrg"
#define ADDRESS_BOOK_PHONE			"AddressBookPhone"
#define ADDRESS_BOOK_GRP			"AddressBookGroup"
#define ADDRESS_BOOK_CONTACT		"AddressBookContact"

#define INSERT_ADDRESSBOOK_ORG		"insert_icc_t_addressbook_org"
#define UPDATE_ADDRESSBOOK_ORG		"update_icc_t_addressbook_org"
#define SELECT_ADDRESSBOOK_ORG		"select_icc_t_addressbook_org"

#define INSERT_ADDRESSBOOK_PHONE	"insert_icc_t_addressbook_phone"
#define UPDATE_ADDRESSBOOK_PHONE	"update_icc_t_addressbook_phone"
#define SELECT_ADDRESSBOOK_PHONE	"select_icc_t_addressbook_phone"

#define INSERT_ADDRESSBOOK_GRP		"insert_icc_t_addressbook_group"
#define UPDATE_ADDRESSBOOK_GRP		"update_icc_t_addressbook_group"
#define SELECT_ADDRESSBOOK_GRP		"select_icc_t_addressbook_group"

#define INSERT_ADDRESSBOOK_CONTACT	"insert_icc_t_addressbook_contact"
#define UPDATE_ADDRESSBOOK_CONTACT	"update_icc_t_addressbook_contact"
#define SELECT_ADDRESSBOOK_CONTACT	"select_icc_t_addressbook_contact"

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

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, GET_DEPT_REQUEST, OnNotifiGetBookOrgRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, SET_DEPT_REQUEST, OnNotifiSetBookOrgRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DEL_DEPT_REQUEST, OnNotifiDeleteBookOrgRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, GET_PHONE_REQUEST, OnNotifiGetBookPhoneRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, SET_PHONE_REQUEST, OnNotifiSetBookPhoneRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DEL_PHONE_REQUEST, OnNotifiDeleteBookPhoneRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, GET_GRP_REQUEST, OnNotifiGetBookGroupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, SET_GRP_REQUEST, OnNotifiSetBookGroupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DEL_GRP_REQUEST, OnNotifiDeleteBookGroupRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, GET_CONTACT_REQUEST, OnNotifiGetBookContactRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, SET_CONTACT_REQUEST, OnNotifiSetBookContactRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, DEL_CONTACT_REQUEST, OnNotifiDeleteBookContactRequest);

	LoadBookOrg();
	LoadBookPhone();
	LoadBookGroup();
	LoadBookContact();

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "AddressBook stop success");
}

void CBusinessImpl::OnDestroy()
{

}
//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::LoadBookOrg()
{
	m_pRedisClient->Del(ADDRESS_BOOK_DEPT);

	bool loadtag = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = SELECT_ADDRESSBOOK_ORG;
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "LoadBookOrg failed sql: [%s] , Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		return false;
	}
	//ICC_LOG_DEBUG(m_pLog, "LoadBookOrg success, sql sucess:[%s]", l_result->GetSQL().c_str());

	std::map<std::string, std::string> tmp_mapDeptInfo;
	while (l_result->Next())
	{
		PROTOCOL::CBookOrgInfo pCDeptInfo;
		pCDeptInfo.m_oBody.m_strGuid = l_result->GetValue("guid");
		pCDeptInfo.m_oBody.m_strParentGuid = l_result->GetValue("parent_guid");
		pCDeptInfo.m_oBody.m_strCode = l_result->GetValue("code");
		pCDeptInfo.m_oBody.m_strName = l_result->GetValue("name");
		pCDeptInfo.m_oBody.m_strPhone = l_result->GetValue("phone");
		pCDeptInfo.m_oBody.m_strRemark = l_result->GetValue("remark");
		pCDeptInfo.m_oBody.m_strSort = l_result->GetValue("sort");

		tmp_mapDeptInfo[pCDeptInfo.m_oBody.m_strGuid] = pCDeptInfo.ToString(m_pJsonFty->CreateJson());
	}

	if (!tmp_mapDeptInfo.empty())
	{
		m_pRedisClient->HMSet(ADDRESS_BOOK_DEPT, tmp_mapDeptInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBookOrg Success, Org Size[%d]", l_result->RecordSize());
	return true;
}

void CBusinessImpl::OnNotifiGetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetBookOrgRequest:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CBaseRequest l_oGetDeptReq;
		if (!l_oGetDeptReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "GetBookOrgRequest parse json error! msg = %s", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CGetBookOrgRespond l_oGetDeptResp;
		BuildProtocolHeader(l_oGetDeptResp.m_oHeader, GET_DEPT_RESPOND, QUEUE_PHONE_BOOK, l_oGetDeptReq.m_oHeader.m_strMsgid);
		l_oGetDeptResp.m_oHeader.m_strMsgId = l_oGetDeptReq.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> p_mapDeptInfo;
		if (m_pRedisClient->HGetAll(ADDRESS_BOOK_DEPT, p_mapDeptInfo))
		{
			PROTOCOL::CGetBookOrgRespond l_oResTmp;
			JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
			for (auto var : p_mapDeptInfo)
			{
				PROTOCOL::CBookOrgInfo l_oDeptInfo;
				l_oDeptInfo.ParseString(var.second, l_pJson);

				PROTOCOL::CGetBookOrgRespond::COrg l_oDept;
				l_oDept.m_strGuid = l_oDeptInfo.m_oBody.m_strGuid;
				l_oDept.m_strParentGuid = l_oDeptInfo.m_oBody.m_strParentGuid;
				l_oDept.m_strCode = l_oDeptInfo.m_oBody.m_strCode;
				l_oDept.m_strName = l_oDeptInfo.m_oBody.m_strName;
				l_oDept.m_strPhone = l_oDeptInfo.m_oBody.m_strPhone;
				l_oDept.m_strRemark = l_oDeptInfo.m_oBody.m_strRemark;
				l_oDept.m_strSort = l_oDeptInfo.m_oBody.m_strSort;

				l_oResTmp.m_oBody.m_vecData.push_back(l_oDept);
			}
			std::sort(l_oResTmp.m_oBody.m_vecData.begin(), l_oResTmp.m_oBody.m_vecData.end(),
				[](const PROTOCOL::CGetBookOrgRespond::COrg& dept1, const PROTOCOL::CGetBookOrgRespond::COrg& dept2)
			{
				return atoi(dept1.m_strSort.c_str()) < atoi(dept2.m_strSort.c_str());
			});

			// 构建分包并发送
			int count = 0;
			for (auto it = l_oResTmp.m_oBody.m_vecData.begin(); it != l_oResTmp.m_oBody.m_vecData.end(); ++it)
			{
				l_oGetDeptResp.m_oBody.m_vecData.push_back(*it);
				++count;
				/*if (count == MAX_COUNT)
				{
					l_oGetDeptResp.m_oBody.m_strCount = m_pString->Number(p_mapDeptInfo.size());
					std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());

					p_pNotify->Response(l_strMessage, true);
					ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

					l_oGetDeptResp.m_oBody.m_vecData.clear();
					count = 0;
				}*/
			}

			l_oGetDeptResp.m_oBody.m_strCount = m_pString->Number(p_mapDeptInfo.size());
			std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
		else
		{
			l_oGetDeptResp.m_oBody.m_strCount = "0";
			std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
}

void CBusinessImpl::OnNotifiSetBookOrgRequest(ObserverPattern::INotificationPtr p_pNotifiSetDeptRequest)
{
	if (p_pNotifiSetDeptRequest)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive SetBookOrgRequest:[%s]", p_pNotifiSetDeptRequest->GetMessages().c_str());

		PROTOCOL::CSetBookOrgRequest l_oSetDeptRequest;
		if (!l_oSetDeptRequest.ParseString(p_pNotifiSetDeptRequest->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotifiSetDeptRequest->GetMessages().c_str());
			return;
		}

		PROTOCOL::CBookOrgInfo l_oDeptCached;
		l_oDeptCached.m_oBody.m_strGuid = l_oSetDeptRequest.m_oBody.m_strGuid;
		l_oDeptCached.m_oBody.m_strParentGuid = l_oSetDeptRequest.m_oBody.m_strParentGuid;
		l_oDeptCached.m_oBody.m_strCode = l_oSetDeptRequest.m_oBody.m_strCode;
		l_oDeptCached.m_oBody.m_strName = l_oSetDeptRequest.m_oBody.m_strName;
		l_oDeptCached.m_oBody.m_strPhone = l_oSetDeptRequest.m_oBody.m_strPhone;
		l_oDeptCached.m_oBody.m_strRemark = l_oSetDeptRequest.m_oBody.m_strRemark;
		l_oDeptCached.m_oBody.m_strSort = l_oSetDeptRequest.m_oBody.m_strSort;

		ICC::ESyncType l_eSyncType = ESyncType::SYNC_TYPE_ADD;
		DataBase::SQLRequest l_oSetDeptSQLReq;
		if (l_oDeptCached.m_oBody.m_strGuid.empty())
		{
			//新增
			l_oDeptCached.m_oBody.m_strGuid = m_pString->CreateGuid();

			l_oSetDeptSQLReq.sql_id = INSERT_ADDRESSBOOK_ORG;
			l_oSetDeptSQLReq.param["guid"] = l_oDeptCached.m_oBody.m_strGuid;
			l_oSetDeptSQLReq.param["parent_guid"] = l_oDeptCached.m_oBody.m_strParentGuid;
			l_oSetDeptSQLReq.param["code"] = l_oDeptCached.m_oBody.m_strCode;
			l_oSetDeptSQLReq.param["name"] = l_oDeptCached.m_oBody.m_strName;
			l_oSetDeptSQLReq.param["phone"] = l_oDeptCached.m_oBody.m_strPhone;
			l_oSetDeptSQLReq.param["remark"] = l_oDeptCached.m_oBody.m_strRemark;
			l_oSetDeptSQLReq.param["sort"] = l_oDeptCached.m_oBody.m_strSort;
			l_oSetDeptSQLReq.param["is_delete"] = "false";
			l_oSetDeptSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetDeptSQLReq.param["create_user"] = SYSTEMID;
		}
		else
		{
			//修改
			l_eSyncType = ESyncType::SYNC_TYPE_MODIFY;

			l_oSetDeptSQLReq.sql_id = UPDATE_ADDRESSBOOK_ORG;
			l_oSetDeptSQLReq.param["guid"] = l_oDeptCached.m_oBody.m_strGuid;
			l_oSetDeptSQLReq.set["parent_guid"] = l_oDeptCached.m_oBody.m_strParentGuid;
			l_oSetDeptSQLReq.set["code"] = l_oDeptCached.m_oBody.m_strCode;
			l_oSetDeptSQLReq.set["name"] = l_oDeptCached.m_oBody.m_strName;
			l_oSetDeptSQLReq.set["phone"] = l_oDeptCached.m_oBody.m_strPhone;
			l_oSetDeptSQLReq.set["remark"] = l_oDeptCached.m_oBody.m_strRemark;
			l_oSetDeptSQLReq.set["sort"] = l_oDeptCached.m_oBody.m_strSort;
			l_oSetDeptSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetDeptSQLReq.set["update_user"] = SYSTEMID;
		}

		bool l_bResult = true;
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oSetDeptSQLReq);
		if (!l_result->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "SetBookOrg fialed, sql:[%s], Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "SetBookOrg sucess, sql:[%s]", l_result->GetSQL().c_str());
		}

		PROTOCOL::CBaseRespond l_oSetDeptResp;
		BuildProtocolHeader(l_oSetDeptResp.m_oHeader, SET_DEPT_RESPOND, QUEUE_PHONE_BOOK, l_oSetDeptRequest.m_oHeader.m_strMsgid);
		l_oSetDeptResp.m_oHeader.m_strMsgId = l_oSetDeptRequest.m_oHeader.m_strMsgId;
		l_oSetDeptResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");
		l_oSetDeptResp.m_oBody.m_strMessage = l_oDeptCached.m_oBody.m_strGuid;

		std::string l_strMessage = l_oSetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotifiSetDeptRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			std::string l_strInfo = l_oDeptCached.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(ADDRESS_BOOK_DEPT, l_oDeptCached.m_oBody.m_strGuid, l_strInfo);

			SyncOrgInfo(l_oDeptCached, l_eSyncType);
		}
	}
}

void CBusinessImpl::OnNotifiDeleteBookOrgRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteBookOrgRequest:[%s]", p_pNotify->GetMessages().c_str());
		PROTOCOL::CBaseRequest l_oDeleDeptRequest;
		if (!l_oDeleDeptRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		bool l_bResult = true;
		DataBase::SQLRequest l_oDelete;
		l_oDelete.sql_id = UPDATE_ADDRESSBOOK_ORG;
		l_oDelete.param.insert(std::pair<std::string, std::string>("guid", l_oDeleDeptRequest.m_oBody.m_strGuid));
		l_oDelete.set.insert(std::pair<std::string, std::string>("is_delete", "true"));
		l_oDelete.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
		l_oDelete.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oDelete);
		if (!l_pResult->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "DeleteBookOrg fialed, sql:[%s], Error Message :[%s]", l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "DeleteBookOrg sucess, sql:[%s]", l_pResult->GetSQL().c_str());
		}
		
		PROTOCOL::CBaseRespond l_oDelDeptResp;
		BuildProtocolHeader(l_oDelDeptResp.m_oHeader, DEL_DEPT_RESPOND, QUEUE_PHONE_BOOK, l_oDeleDeptRequest.m_oHeader.m_strMsgid);
		l_oDelDeptResp.m_oHeader.m_strMsgId = l_oDeleDeptRequest.m_oHeader.m_strMsgId;
		l_oDelDeptResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");

		std::string l_strMessage = l_oDelDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			m_pRedisClient->HDel(ADDRESS_BOOK_DEPT, l_oDeleDeptRequest.m_oBody.m_strGuid);

			//同步单位信息
			PROTOCOL::CBookOrgInfo l_oDeptInfo;
			l_oDeptInfo.m_oBody.m_strGuid = l_oDeleDeptRequest.m_oBody.m_strGuid;
			SyncOrgInfo(l_oDeptInfo, ESyncType::SYNC_TYPE_DELETE);
		}
	}
}

void CBusinessImpl::SyncOrgInfo(const PROTOCOL::CBookOrgInfo& p_oDept, ESyncType p_eSyncType)
{
	PROTOCOL::CBookOrgInfo l_oDeptSync;
	BuildProtocolHeader(l_oDeptSync.m_oHeader, CMD_DEPT_SYNC, TOPIC_BOOK_SYNC, PROTOCOL_TOPIC);
	l_oDeptSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oDeptSync.m_oBody.m_strSyncType = m_pString->Number(p_eSyncType);
	l_oDeptSync.m_oBody.m_strGuid = p_oDept.m_oBody.m_strGuid;
	l_oDeptSync.m_oBody.m_strParentGuid = p_oDept.m_oBody.m_strParentGuid;
	l_oDeptSync.m_oBody.m_strCode = p_oDept.m_oBody.m_strCode;
	l_oDeptSync.m_oBody.m_strName = p_oDept.m_oBody.m_strName;
	l_oDeptSync.m_oBody.m_strPhone = p_oDept.m_oBody.m_strPhone;
	l_oDeptSync.m_oBody.m_strRemark = p_oDept.m_oBody.m_strRemark;
	l_oDeptSync.m_oBody.m_strSort = p_oDept.m_oBody.m_strSort;

	std::string l_strSyncMsg = l_oDeptSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
	ICC_LOG_DEBUG(m_pLog, "Sync org:[%s]", l_strSyncMsg.c_str());
}

//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::LoadBookPhone()
{
	m_pRedisClient->Del(ADDRESS_BOOK_PHONE);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = SELECT_ADDRESSBOOK_PHONE;
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "LoadBookPhone failed sql: [%s] , Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		return false;
	}
	//ICC_LOG_DEBUG(m_pLog, "LoadBookPhone success, sql:[%s]", l_result->GetSQL().c_str());

	std::map<std::string, std::string> tmp_mapStaffInfo;
	while (l_result->Next())
	{
		PROTOCOL::CBookPhoneInfo l_oLoadStaff;
		l_oLoadStaff.m_oBody.m_strGuid = l_result->GetValue("guid");
		l_oLoadStaff.m_oBody.m_strOrgGuid = l_result->GetValue("org_guid");
		l_oLoadStaff.m_oBody.m_strCode = l_result->GetValue("code");
		l_oLoadStaff.m_oBody.m_strName = l_result->GetValue("name");
		l_oLoadStaff.m_oBody.m_strSex = l_result->GetValue("gender");
		l_oLoadStaff.m_oBody.m_strMobile = l_result->GetValue("mobile");
		l_oLoadStaff.m_oBody.m_strPhone = l_result->GetValue("phone");
		l_oLoadStaff.m_oBody.m_strSort = l_result->GetValue("sort");
		l_oLoadStaff.m_oBody.m_strRemark = l_result->GetValue("remark");

		tmp_mapStaffInfo[l_oLoadStaff.m_oBody.m_strGuid] = l_oLoadStaff.ToString(m_pJsonFty->CreateJson());
	}

	if (!tmp_mapStaffInfo.empty())
	{
		m_pRedisClient->HMSet(ADDRESS_BOOK_PHONE, tmp_mapStaffInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBookPhone success, phone size[%d]", tmp_mapStaffInfo.size());
	return true;
}

void CBusinessImpl::OnNotifiGetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetBookPhoneRequest message:[%s]", p_pNotify->GetMessages().c_str());
		
		PROTOCOL::CBaseRequest l_oReq;
		if (!l_oReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "GetBookPhoneRequest parse json error! msg = %s", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CGetBookPhoneRespond l_oGetStaffResp;
		BuildProtocolHeader(l_oGetStaffResp.m_oHeader, GET_PHONE_RESPOND, QUEUE_PHONE_BOOK, l_oReq.m_oHeader.m_strMsgid);
		l_oGetStaffResp.m_oHeader.m_strMsgId = l_oReq.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> l_mapStaffInfo;
		m_pRedisClient->HGetAll(ADDRESS_BOOK_PHONE, l_mapStaffInfo);
		if (l_mapStaffInfo.size() == 0)
		{
			l_oGetStaffResp.m_oBody.m_strCount = "0";
			std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
		else
		{
			int count = 0;
			for (auto it : l_mapStaffInfo)
			{
				PROTOCOL::CBookPhoneInfo l_oStaffInfo;
				JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
				if (l_pJson && l_oStaffInfo.ParseString(it.second, l_pJson))
				{
					PROTOCOL::CGetBookPhoneRespond::CStaff l_oStaff;
					l_oStaff.m_strGuid = l_oStaffInfo.m_oBody.m_strGuid;
					l_oStaff.m_strDeptGuid = l_oStaffInfo.m_oBody.m_strOrgGuid;
					l_oStaff.m_strCode = l_oStaffInfo.m_oBody.m_strCode;
					l_oStaff.m_strName = l_oStaffInfo.m_oBody.m_strName;
					l_oStaff.m_strSex = l_oStaffInfo.m_oBody.m_strSex;
					l_oStaff.m_strMobile = l_oStaffInfo.m_oBody.m_strMobile;
					l_oStaff.m_strPhone = l_oStaffInfo.m_oBody.m_strPhone;
					l_oStaff.m_strSort = l_oStaffInfo.m_oBody.m_strSort;
					l_oStaff.m_strRemark = l_oStaffInfo.m_oBody.m_strRemark;
					l_oGetStaffResp.m_oBody.m_vecData.push_back(l_oStaff);

					++count;
				}
				
				/*if (count == MAX_COUNT)
				{
					l_oGetStaffResp.m_oBody.m_strCount = m_pString->Number(l_mapStaffInfo.size());
					std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
					l_oGetStaffResp.m_oBody.m_vecData.clear();
					p_pNotify->Response(l_strMessage, true);
					ICC_LOG_DEBUG(m_pLog, "send phone info msg [num:%d][%s]", count, l_strMessage.c_str());

					count = 0;
				}*/
			}

			l_oGetStaffResp.m_oBody.m_strCount = m_pString->Number(l_mapStaffInfo.size());
			std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send phone info msg [num:%d][%s]", count, l_strMessage.c_str());

		}
		ICC_LOG_DEBUG(m_pLog, "send phone info msg [total:%d]", l_mapStaffInfo.size());
	}
}

void CBusinessImpl::OnNotifiSetBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive SetBookPhone message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CSetBookPhoneRequest l_oSetStaffRequest;
		if (!l_oSetStaffRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "SetBookPhone request is not json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CBookPhoneInfo l_oStaffCached;
		l_oStaffCached.m_oBody.m_strGuid = l_oSetStaffRequest.m_oBody.m_strGuid;
		l_oStaffCached.m_oBody.m_strOrgGuid = l_oSetStaffRequest.m_oBody.m_strOrgGuid;
		l_oStaffCached.m_oBody.m_strCode = l_oSetStaffRequest.m_oBody.m_strCode;
		l_oStaffCached.m_oBody.m_strName = l_oSetStaffRequest.m_oBody.m_strName;
		l_oStaffCached.m_oBody.m_strSex = l_oSetStaffRequest.m_oBody.m_strSex;
		l_oStaffCached.m_oBody.m_strMobile = l_oSetStaffRequest.m_oBody.m_strMobile;
		l_oStaffCached.m_oBody.m_strPhone = l_oSetStaffRequest.m_oBody.m_strPhone;
		l_oStaffCached.m_oBody.m_strSort = l_oSetStaffRequest.m_oBody.m_strSort;
		l_oStaffCached.m_oBody.m_strRemark = l_oSetStaffRequest.m_oBody.m_strRemark;

		ICC::ESyncType l_eSyncType = SYNC_TYPE_ADD;
		DataBase::SQLRequest l_oSetStaffSQLReq;
		if (l_oStaffCached.m_oBody.m_strGuid.empty())
		{
			//新增
			l_oStaffCached.m_oBody.m_strGuid = m_pString->CreateGuid();

			l_oSetStaffSQLReq.sql_id = INSERT_ADDRESSBOOK_PHONE;
			l_oSetStaffSQLReq.param["guid"] = l_oStaffCached.m_oBody.m_strGuid;
			l_oSetStaffSQLReq.param["org_guid"] = l_oStaffCached.m_oBody.m_strOrgGuid;
			l_oSetStaffSQLReq.param["code"] = l_oStaffCached.m_oBody.m_strCode;
			l_oSetStaffSQLReq.param["name"] = l_oStaffCached.m_oBody.m_strName;
			l_oSetStaffSQLReq.param["gender"] = l_oStaffCached.m_oBody.m_strSex;
			l_oSetStaffSQLReq.param["mobile"] = l_oStaffCached.m_oBody.m_strMobile;
			l_oSetStaffSQLReq.param["phone"] = l_oStaffCached.m_oBody.m_strPhone;
			l_oSetStaffSQLReq.param["remark"] = l_oStaffCached.m_oBody.m_strRemark;
			l_oSetStaffSQLReq.param["sort"] = l_oStaffCached.m_oBody.m_strSort;
			l_oSetStaffSQLReq.param["is_delete"] = "false";
			l_oSetStaffSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetStaffSQLReq.param["create_user"] = SYSTEMID;
		}
		else
		{
			//修改
			l_eSyncType = SYNC_TYPE_MODIFY;

			l_oSetStaffSQLReq.sql_id = UPDATE_ADDRESSBOOK_PHONE;
			l_oSetStaffSQLReq.param["guid"] = l_oStaffCached.m_oBody.m_strGuid;
			l_oSetStaffSQLReq.set["org_guid"] = l_oStaffCached.m_oBody.m_strOrgGuid;
			l_oSetStaffSQLReq.set["code"] = l_oStaffCached.m_oBody.m_strCode;
			l_oSetStaffSQLReq.set["name"] = l_oStaffCached.m_oBody.m_strName;
			l_oSetStaffSQLReq.set["gender"] = l_oStaffCached.m_oBody.m_strSex;
			l_oSetStaffSQLReq.set["mobile"] = l_oStaffCached.m_oBody.m_strMobile;
			l_oSetStaffSQLReq.set["phone"] = l_oStaffCached.m_oBody.m_strPhone;
			l_oSetStaffSQLReq.set["remark"] = l_oStaffCached.m_oBody.m_strRemark;
			l_oSetStaffSQLReq.set["sort"] = l_oStaffCached.m_oBody.m_strSort;
			l_oSetStaffSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetStaffSQLReq.set["update_user"] = SYSTEMID;
		}

		bool l_bResult = true;
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oSetStaffSQLReq);
		if (!l_result->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "SetBookPhone fialed, sql:[%s], Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "SetBookPhone sucess, sql:[%s]", l_result->GetSQL().c_str());
		}

		PROTOCOL::CBaseRespond l_oSetStaffResp;
		BuildProtocolHeader(l_oSetStaffResp.m_oHeader, SET_PHONE_RESPOND, QUEUE_PHONE_BOOK, l_oSetStaffRequest.m_oHeader.m_strMsgid);
		l_oSetStaffResp.m_oHeader.m_strMsgId = l_oSetStaffRequest.m_oHeader.m_strMsgId;
		l_oSetStaffResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");
		l_oSetStaffResp.m_oBody.m_strMessage = l_oStaffCached.m_oBody.m_strGuid;

		std::string l_strMessage = l_oSetStaffResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			std::string l_strInfo = l_oStaffCached.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(ADDRESS_BOOK_PHONE, l_oStaffCached.m_oBody.m_strGuid, l_strInfo);

			SyncPhoneInfo(l_oStaffCached, l_eSyncType);
		}
	}
}

void CBusinessImpl::OnNotifiDeleteBookPhoneRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive DeleteBookPhone message:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CBaseRequest l_oDeleStaffRequest;
	if (!l_oDeleStaffRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "DeleteBookPhone request not json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	bool l_bResult = true;
	DataBase::SQLRequest l_oDelete;
	l_oDelete.sql_id = UPDATE_ADDRESSBOOK_PHONE;
	l_oDelete.param.insert(std::pair<std::string, std::string>("guid", l_oDeleStaffRequest.m_oBody.m_strGuid));
	l_oDelete.set.insert(std::pair<std::string, std::string>("is_delete", "true"));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oDelete);
	if (!l_pResult->IsValid())
	{
		l_bResult = false;
		ICC_LOG_ERROR(m_pLog, "DeleteBookPhone fialed, sql:[%s], Error Message :[%s]", l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "DeleteBookPhone sucess, sql:[%s]", l_pResult->GetSQL().c_str());
	}
	 
	PROTOCOL::CBaseRespond l_oDelStaffResp;
	BuildProtocolHeader(l_oDelStaffResp.m_oHeader, DEL_PHONE_RESPOND, QUEUE_PHONE_BOOK, l_oDeleStaffRequest.m_oHeader.m_strMsgid);
	l_oDelStaffResp.m_oHeader.m_strMsgId = l_oDeleStaffRequest.m_oHeader.m_strMsgId;
	l_oDelStaffResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");

	std::string l_strMessage = l_oDelStaffResp.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

	if (l_bResult)
	{
		m_pRedisClient->HDel(ADDRESS_BOOK_PHONE, l_oDeleStaffRequest.m_oBody.m_strGuid);

		PROTOCOL::CBookPhoneInfo l_oDeleteStaff;
		l_oDeleteStaff.m_oBody.m_strGuid = l_oDeleStaffRequest.m_oBody.m_strGuid;
		SyncPhoneInfo(l_oDeleteStaff, ESyncType::SYNC_TYPE_DELETE);
	}
}

void CBusinessImpl::SyncPhoneInfo(const PROTOCOL::CBookPhoneInfo &p_oStaff, ESyncType p_eSyncType)
{
	PROTOCOL::CBookPhoneInfo l_oStaffSync;
	BuildProtocolHeader(l_oStaffSync.m_oHeader, CMD_PHONE_SYNC, TOPIC_BOOK_SYNC, PROTOCOL_TOPIC);
	l_oStaffSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oStaffSync.m_oBody.m_strSyncType = std::to_string(p_eSyncType);
	l_oStaffSync.m_oBody.m_strGuid = p_oStaff.m_oBody.m_strGuid;
	l_oStaffSync.m_oBody.m_strOrgGuid = p_oStaff.m_oBody.m_strOrgGuid;
	l_oStaffSync.m_oBody.m_strCode = p_oStaff.m_oBody.m_strCode;
	l_oStaffSync.m_oBody.m_strName = p_oStaff.m_oBody.m_strName;
	l_oStaffSync.m_oBody.m_strSex = p_oStaff.m_oBody.m_strSex;
	l_oStaffSync.m_oBody.m_strMobile = p_oStaff.m_oBody.m_strMobile;
	l_oStaffSync.m_oBody.m_strPhone = p_oStaff.m_oBody.m_strPhone;
	l_oStaffSync.m_oBody.m_strSort = p_oStaff.m_oBody.m_strSort;
	l_oStaffSync.m_oBody.m_strRemark = p_oStaff.m_oBody.m_strRemark;

	std::string l_strStaffSyncMsg = l_oStaffSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strStaffSyncMsg));
	ICC_LOG_DEBUG(m_pLog, "Send SyncPhoneInfo Msg:[%s]", l_strStaffSyncMsg.c_str());
}

//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::LoadBookGroup()
{
	m_pRedisClient->Del(ADDRESS_BOOK_GRP);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = SELECT_ADDRESSBOOK_GRP;
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "LoadBookGroup failed sql: [%s] , Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		return false;
	}
	//ICC_LOG_DEBUG(m_pLog, "LoadBookGroup success, sql:[%s]", l_result->GetSQL().c_str());

	std::map<std::string, std::string> tmp_mapGroupInfo;
	while (l_result->Next())
	{
		PROTOCOL::CContactGrpInfo l_oLoadGrp;
		l_oLoadGrp.m_oBody.m_strGuid = l_result->GetValue("guid");
		l_oLoadGrp.m_oBody.m_strTitle = l_result->GetValue("title");
		l_oLoadGrp.m_oBody.m_strUserCode = l_result->GetValue("user_code");
		l_oLoadGrp.m_oBody.m_strType = l_result->GetValue("type");
		l_oLoadGrp.m_oBody.m_strSort = l_result->GetValue("sort");

		tmp_mapGroupInfo[l_oLoadGrp.m_oBody.m_strGuid] = l_oLoadGrp.ToString(m_pJsonFty->CreateJson());
	}

	if (!tmp_mapGroupInfo.empty())
	{
		m_pRedisClient->HMSet(ADDRESS_BOOK_GRP, tmp_mapGroupInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBookGroup success, group size[%d]", tmp_mapGroupInfo.size());
	return true;
}

//获取面板组信息
void CBusinessImpl::OnNotifiGetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetBookGroupRequest message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CBaseRequest l_oReq;
		if (!l_oReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "GetBookGroupRequest parse json error! msg = %s", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CGetContactGrpRespond l_oGetGrpResp;
		BuildProtocolHeader(l_oGetGrpResp.m_oHeader, GET_GRP_RESPOND, QUEUE_PHONE_BOOK, l_oReq.m_oHeader.m_strMsgid);
		l_oGetGrpResp.m_oHeader.m_strMsgId = l_oReq.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> l_mapGroupInfo;
		m_pRedisClient->HGetAll(ADDRESS_BOOK_GRP, l_mapGroupInfo);
		if (l_mapGroupInfo.size() == 0)
		{
			l_oGetGrpResp.m_oBody.m_strCount = "0";
			std::string l_strMessage = l_oGetGrpResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
		else
		{
			int count = 0;
			for (auto it : l_mapGroupInfo)
			{
				PROTOCOL::CContactGrpInfo l_oGrpInfo;
				JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
				if (l_pJson && l_oGrpInfo.ParseString(it.second, l_pJson))
				{
					PROTOCOL::CGetContactGrpRespond::CGroup l_oGroup;
					l_oGroup.m_strGuid = l_oGrpInfo.m_oBody.m_strGuid;
					l_oGroup.m_strTitle = l_oGrpInfo.m_oBody.m_strTitle;
					l_oGroup.m_strUserCode = l_oGrpInfo.m_oBody.m_strUserCode;
					l_oGroup.m_strType = l_oGrpInfo.m_oBody.m_strType;
					l_oGroup.m_strSort = l_oGrpInfo.m_oBody.m_strSort;
					l_oGetGrpResp.m_oBody.m_vecData.push_back(l_oGroup);

					++count;
				}
				
				/*if (count == MAX_COUNT)
				{
					l_oGetGrpResp.m_oBody.m_strCount = m_pString->Number(l_mapGroupInfo.size());
					std::string l_strMessage = l_oGetGrpResp.ToString(m_pJsonFty->CreateJson());
					l_oGetGrpResp.m_oBody.m_vecData.clear();
					p_pNotify->Response(l_strMessage, true);
					ICC_LOG_DEBUG(m_pLog, "send group info msg [num:%d][%s]", count, l_strMessage.c_str());

					count = 0;
				}*/
			}

			l_oGetGrpResp.m_oBody.m_strCount = m_pString->Number(l_mapGroupInfo.size());
			std::string l_strMessage = l_oGetGrpResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send group info msg [num:%d][%s]", count, l_strMessage.c_str());

		}
		ICC_LOG_DEBUG(m_pLog, "send group info msg [total:%d]", l_mapGroupInfo.size());
	}
}
//设置面板组信息
void CBusinessImpl::OnNotifiSetBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive SetBookGroup message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CSetContactGrpRequest l_oSetGrpRequest;
		if (!l_oSetGrpRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "SetBookGroup request is not json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CContactGrpInfo l_oGrpCached;
		l_oGrpCached.m_oBody.m_strGuid = l_oSetGrpRequest.m_oBody.m_strGuid;
		l_oGrpCached.m_oBody.m_strTitle = l_oSetGrpRequest.m_oBody.m_strTitle;
		l_oGrpCached.m_oBody.m_strUserCode = l_oSetGrpRequest.m_oBody.m_strUserCode;
		l_oGrpCached.m_oBody.m_strType = l_oSetGrpRequest.m_oBody.m_strType;
		l_oGrpCached.m_oBody.m_strSort = l_oSetGrpRequest.m_oBody.m_strSort;

		ICC::ESyncType l_eSyncType = SYNC_TYPE_ADD;
		DataBase::SQLRequest l_oSetGrpSQLReq;
		if (l_oGrpCached.m_oBody.m_strGuid.empty())
		{
			//新增
			l_oGrpCached.m_oBody.m_strGuid = m_pString->CreateGuid();

			l_oSetGrpSQLReq.sql_id = INSERT_ADDRESSBOOK_GRP;
			l_oSetGrpSQLReq.param["guid"] = l_oGrpCached.m_oBody.m_strGuid;
			l_oSetGrpSQLReq.param["title"] = l_oGrpCached.m_oBody.m_strTitle;
			l_oSetGrpSQLReq.param["user_code"] = l_oGrpCached.m_oBody.m_strUserCode;
			l_oSetGrpSQLReq.param["type"] = l_oGrpCached.m_oBody.m_strType;
			l_oSetGrpSQLReq.param["sort"] = l_oGrpCached.m_oBody.m_strSort;
			l_oSetGrpSQLReq.param["is_delete"] = "false";
			l_oSetGrpSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetGrpSQLReq.param["create_user"] = SYSTEMID;
		}
		else
		{
			//修改
			l_eSyncType = SYNC_TYPE_MODIFY;

			l_oSetGrpSQLReq.sql_id = UPDATE_ADDRESSBOOK_GRP;
			l_oSetGrpSQLReq.param["guid"] = l_oGrpCached.m_oBody.m_strGuid;
			l_oSetGrpSQLReq.set["title"] = l_oGrpCached.m_oBody.m_strTitle;
			l_oSetGrpSQLReq.set["user_code"] = l_oGrpCached.m_oBody.m_strUserCode;
			l_oSetGrpSQLReq.set["type"] = l_oGrpCached.m_oBody.m_strType;
			l_oSetGrpSQLReq.set["sort"] = l_oGrpCached.m_oBody.m_strSort;
			l_oSetGrpSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetGrpSQLReq.set["update_user"] = SYSTEMID;
		}

		bool l_bResult = true;
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oSetGrpSQLReq);
		if (!l_result->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "SetBookGroup fialed, sql:[%s], Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "SetBookGroup sucess, sql:[%s]", l_result->GetSQL().c_str());
		}

		PROTOCOL::CBaseRespond l_oSetGrpResp;
		BuildProtocolHeader(l_oSetGrpResp.m_oHeader, SET_GRP_RESPOND, QUEUE_PHONE_BOOK, l_oSetGrpRequest.m_oHeader.m_strMsgid);
		l_oSetGrpResp.m_oHeader.m_strMsgId = l_oSetGrpRequest.m_oHeader.m_strMsgId;
		l_oSetGrpResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");
		l_oSetGrpResp.m_oBody.m_strMessage = l_oGrpCached.m_oBody.m_strGuid;
		std::string l_strMessage = l_oSetGrpResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			std::string l_strInfo = l_oGrpCached.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(ADDRESS_BOOK_GRP, l_oGrpCached.m_oBody.m_strGuid, l_strInfo);

			SyncGroupInfo(l_oGrpCached, l_eSyncType);
		}
	}
}
//删除面板组信息
void CBusinessImpl::OnNotifiDeleteBookGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteBookGroup message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CDeleteContactGrpRequest l_oDeleGrpRequest;
		if (!l_oDeleGrpRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "DeleteBookGroup request not json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		std::string l_strGrpId = l_oDeleGrpRequest.m_oBody.m_strGuid;

		bool l_bResult = true;
		DataBase::SQLRequest l_oDelete;
		l_oDelete.sql_id = UPDATE_ADDRESSBOOK_GRP;
		l_oDelete.param.insert(std::pair<std::string, std::string>("guid", l_strGrpId));
		l_oDelete.set.insert(std::pair<std::string, std::string>("is_delete", "true"));
		l_oDelete.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
		l_oDelete.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oDelete);
		if (!l_pResult->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "DeleteBookGroup fialed, sql:[%s], Error Message :[%s]", l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			//删除组里的所有联系成员
			DelContactByGrpId(l_strGrpId);
			ICC_LOG_DEBUG(m_pLog, "DeleteBookGroup sucess, sql:[%s]", l_pResult->GetSQL().c_str());
		}

		PROTOCOL::CBaseRespond l_oDelGrpResp;
		BuildProtocolHeader(l_oDelGrpResp.m_oHeader, DEL_GRP_RESPOND, QUEUE_PHONE_BOOK, l_oDeleGrpRequest.m_oHeader.m_strMsgid);
		l_oDelGrpResp.m_oHeader.m_strMsgId = l_oDeleGrpRequest.m_oHeader.m_strMsgId;
		l_oDelGrpResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");

		std::string l_strMessage = l_oDelGrpResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			m_pRedisClient->HDel(ADDRESS_BOOK_GRP, l_strGrpId);

			PROTOCOL::CContactGrpInfo l_oDeleteGrp;
			l_oDeleteGrp.m_oBody.m_strGuid = l_strGrpId;
			SyncGroupInfo(l_oDeleteGrp, ESyncType::SYNC_TYPE_DELETE);
		}
	}
}

void CBusinessImpl::SyncGroupInfo(const PROTOCOL::CContactGrpInfo& p_oGroup, ESyncType p_eSyncType)
{
	PROTOCOL::CContactGrpInfo l_oGrpSync;
	BuildProtocolHeader(l_oGrpSync.m_oHeader, CMD_GRP_SYNC, TOPIC_BOOK_SYNC, PROTOCOL_TOPIC);
	l_oGrpSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oGrpSync.m_oBody.m_strSyncType = std::to_string(p_eSyncType);
	l_oGrpSync.m_oBody.m_strGuid = p_oGroup.m_oBody.m_strGuid;
	l_oGrpSync.m_oBody.m_strTitle = p_oGroup.m_oBody.m_strTitle;
	l_oGrpSync.m_oBody.m_strUserCode = p_oGroup.m_oBody.m_strUserCode;
	l_oGrpSync.m_oBody.m_strType = p_oGroup.m_oBody.m_strType;
	l_oGrpSync.m_oBody.m_strSort = p_oGroup.m_oBody.m_strSort;

	std::string l_strGrpSyncMsg = l_oGrpSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strGrpSyncMsg));
	ICC_LOG_DEBUG(m_pLog, "Send SyncGrpInfo Msg:[%s]", l_strGrpSyncMsg.c_str());
}
//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::LoadBookContact()
{
	m_pRedisClient->Del(ADDRESS_BOOK_CONTACT);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = SELECT_ADDRESSBOOK_CONTACT;
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "LoadBookContact failed sql: [%s] , Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		return false;
	}
	//ICC_LOG_DEBUG(m_pLog, "LoadBookContact success, sql:[%s]", l_result->GetSQL().c_str());

	std::map<std::string, std::string> tmp_mapInfo;
	while (l_result->Next())
	{
		PROTOCOL::CContactInfo l_oLoadInfo;
		l_oLoadInfo.m_oBody.m_strGuid = l_result->GetValue("guid");
		l_oLoadInfo.m_oBody.m_strGrpGuid = l_result->GetValue("group_guid");
		l_oLoadInfo.m_oBody.m_strType = l_result->GetValue("type");
		l_oLoadInfo.m_oBody.m_strName = l_result->GetValue("name");
		l_oLoadInfo.m_oBody.m_strPhone = l_result->GetValue("phone");
		l_oLoadInfo.m_oBody.m_strSort = l_result->GetValue("sort");
		l_oLoadInfo.m_oBody.m_strRemark = l_result->GetValue("remark");

		tmp_mapInfo[l_oLoadInfo.m_oBody.m_strGuid] = l_oLoadInfo.ToString(m_pJsonFty->CreateJson());
	}

	if (!tmp_mapInfo.empty())
	{
		m_pRedisClient->HMSet(ADDRESS_BOOK_CONTACT, tmp_mapInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBookContact success, contact size[%d]", tmp_mapInfo.size());
	return true;
}

bool CBusinessImpl::DelContactByGrpId(const std::string& p_strGrpId)
{
	bool l_bResult = true;

	std::vector<std::string> l_vecFields;
	std::map<std::string, std::string> l_mapContactInfo;
	m_pRedisClient->HGetAll(ADDRESS_BOOK_CONTACT, l_mapContactInfo);
	for (auto it : l_mapContactInfo)
	{
		PROTOCOL::CContactInfo l_oContactInfo;
		JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
		if (l_pJson && l_oContactInfo.ParseString(it.second, l_pJson))
		{
			std::string l_strGrpId = l_oContactInfo.m_oBody.m_strGrpGuid;
			if (l_strGrpId.compare(p_strGrpId) == 0)
			{
				std::string l_strGuid = l_oContactInfo.m_oBody.m_strGuid;
				if (DelContactInfo(l_strGuid))
				{
					l_vecFields.push_back(l_strGuid);

					PROTOCOL::CContactInfo l_oDeleteInfo;
					l_oDeleteInfo.m_oBody.m_strGuid = l_strGuid;
					SyncContactInfo(l_oDeleteInfo, ESyncType::SYNC_TYPE_DELETE);
				}
			}
		}
	}

	if (l_vecFields.size() > 0)
	{
		m_pRedisClient->HDelFields(ADDRESS_BOOK_CONTACT, l_vecFields);
	}

	return l_bResult;
}
bool CBusinessImpl::DelContactInfo(const std::string& p_strContactId)
{
	bool l_bResult = true;

	DataBase::SQLRequest l_oDelete;
	l_oDelete.sql_id = UPDATE_ADDRESSBOOK_CONTACT;
	l_oDelete.param.insert(std::pair<std::string, std::string>("guid", p_strContactId));
	l_oDelete.set.insert(std::pair<std::string, std::string>("is_delete", "true"));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_user", SYSTEMID));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oDelete);
	if (!l_pResult->IsValid())
	{
		l_bResult = false;
		ICC_LOG_ERROR(m_pLog, "DeleteBookContact fialed, sql:[%s], Error Message :[%s]", l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "DeleteBookContact sucess, sql:[%s]", l_pResult->GetSQL().c_str());
	}

	return l_bResult;
}

//获取电话信息
void CBusinessImpl::OnNotifiGetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive GetBookContact message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CBaseRequest l_oReq;
		if (!l_oReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "GetBookContact parse json error! msg = %s", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CGetContactInfoRespond l_oGetInfoResp;
		BuildProtocolHeader(l_oGetInfoResp.m_oHeader, GET_CONTACT_RESPOND, QUEUE_PHONE_BOOK, l_oReq.m_oHeader.m_strMsgid);
		l_oGetInfoResp.m_oHeader.m_strMsgId = l_oReq.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> l_mapContactInfo;
		m_pRedisClient->HGetAll(ADDRESS_BOOK_CONTACT, l_mapContactInfo);
		if (l_mapContactInfo.size() == 0)
		{
			l_oGetInfoResp.m_oBody.m_strCount = "0";
			std::string l_strMessage = l_oGetInfoResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
		else
		{
			int count = 0;
			for (auto it : l_mapContactInfo)
			{
				PROTOCOL::CContactInfo l_oContactInfo;
				JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
				if (l_pJson && l_oContactInfo.ParseString(it.second, l_pJson))
				{
					PROTOCOL::CGetContactInfoRespond::CInfo l_oInfo;
					l_oInfo.m_strGuid = l_oContactInfo.m_oBody.m_strGuid;
					l_oInfo.m_strGrpGuid = l_oContactInfo.m_oBody.m_strGrpGuid;
					l_oInfo.m_strType = l_oContactInfo.m_oBody.m_strType;
					l_oInfo.m_strName = l_oContactInfo.m_oBody.m_strName;
					l_oInfo.m_strPhone = l_oContactInfo.m_oBody.m_strPhone;
					l_oInfo.m_strSort = l_oContactInfo.m_oBody.m_strSort;
					l_oInfo.m_strRemark = l_oContactInfo.m_oBody.m_strRemark;
					l_oGetInfoResp.m_oBody.m_vecData.push_back(l_oInfo);

					++count;
				}
				
				/*if (count == MAX_COUNT)
				{
					l_oGetInfoResp.m_oBody.m_strCount = m_pString->Number(l_mapContactInfo.size());
					std::string l_strMessage = l_oGetInfoResp.ToString(m_pJsonFty->CreateJson());
					l_oGetInfoResp.m_oBody.m_vecData.clear();
					p_pNotify->Response(l_strMessage, true);
					ICC_LOG_DEBUG(m_pLog, "send contact info msg [num:%d][%s]", count, l_strMessage.c_str());

					count = 0;
				}*/
			}

			l_oGetInfoResp.m_oBody.m_strCount = m_pString->Number(l_mapContactInfo.size());
			std::string l_strMessage = l_oGetInfoResp.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send contact info msg [num:%d][%s]", count, l_strMessage.c_str());

		}
		ICC_LOG_DEBUG(m_pLog, "send contact info msg [total:%d]", l_mapContactInfo.size());
	}
}
//设置电话信息
void CBusinessImpl::OnNotifiSetBookContactRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive SetBookContact message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CSetContactInfoRequest l_oSetInfoRequest;
		if (!l_oSetInfoRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "SetBookContact request is not json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CContactInfo l_oInfoCached;
		l_oInfoCached.m_oBody.m_strGuid = l_oSetInfoRequest.m_oBody.m_strGuid;
		l_oInfoCached.m_oBody.m_strGrpGuid = l_oSetInfoRequest.m_oBody.m_strGrpGuid;
		l_oInfoCached.m_oBody.m_strType = l_oSetInfoRequest.m_oBody.m_strType;
		l_oInfoCached.m_oBody.m_strName = l_oSetInfoRequest.m_oBody.m_strName;
		l_oInfoCached.m_oBody.m_strPhone = l_oSetInfoRequest.m_oBody.m_strPhone;
		l_oInfoCached.m_oBody.m_strSort = l_oSetInfoRequest.m_oBody.m_strSort;
		l_oInfoCached.m_oBody.m_strRemark = l_oSetInfoRequest.m_oBody.m_strRemark;

		ICC::ESyncType l_eSyncType = SYNC_TYPE_ADD;
		DataBase::SQLRequest l_oSetInfoSQLReq;
		if (l_oInfoCached.m_oBody.m_strGuid.empty())
		{
			//新增
			l_oInfoCached.m_oBody.m_strGuid = m_pString->CreateGuid();

			l_oSetInfoSQLReq.sql_id = INSERT_ADDRESSBOOK_CONTACT;
			l_oSetInfoSQLReq.param["guid"] = l_oInfoCached.m_oBody.m_strGuid;
			l_oSetInfoSQLReq.param["group_guid"] = l_oInfoCached.m_oBody.m_strGrpGuid;
			l_oSetInfoSQLReq.param["type"] = l_oInfoCached.m_oBody.m_strType;
			l_oSetInfoSQLReq.param["name"] = l_oInfoCached.m_oBody.m_strName;
			l_oSetInfoSQLReq.param["phone"] = l_oInfoCached.m_oBody.m_strPhone;
			l_oSetInfoSQLReq.param["sort"] = l_oInfoCached.m_oBody.m_strSort;
			l_oSetInfoSQLReq.param["remark"] = l_oInfoCached.m_oBody.m_strRemark;
			l_oSetInfoSQLReq.param["is_delete"] = "false";
			l_oSetInfoSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetInfoSQLReq.param["create_user"] = SYSTEMID;
		}
		else
		{
			//修改
			l_eSyncType = SYNC_TYPE_MODIFY;

			l_oSetInfoSQLReq.sql_id = UPDATE_ADDRESSBOOK_CONTACT;
			l_oSetInfoSQLReq.param["guid"] = l_oInfoCached.m_oBody.m_strGuid;
			l_oSetInfoSQLReq.set["group_guid"] = l_oInfoCached.m_oBody.m_strGrpGuid;
			l_oSetInfoSQLReq.set["type"] = l_oInfoCached.m_oBody.m_strType;
			l_oSetInfoSQLReq.set["name"] = l_oInfoCached.m_oBody.m_strName;
			l_oSetInfoSQLReq.set["phone"] = l_oInfoCached.m_oBody.m_strPhone;
			l_oSetInfoSQLReq.set["sort"] = l_oInfoCached.m_oBody.m_strSort;
			l_oSetInfoSQLReq.set["remark"] = l_oInfoCached.m_oBody.m_strRemark;
			l_oSetInfoSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSetInfoSQLReq.set["update_user"] = SYSTEMID;
		}

		bool l_bResult = true;
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oSetInfoSQLReq);
		if (!l_result->IsValid())
		{
			l_bResult = false;
			ICC_LOG_ERROR(m_pLog, "SetBookContact fialed, sql:[%s], Error Message :[%s]", l_result->GetSQL().c_str(), l_result->GetErrorMsg().c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "SetBookContact sucess, sql:[%s]", l_result->GetSQL().c_str());
		}

		PROTOCOL::CBaseRespond l_oSetInfoResp;
		BuildProtocolHeader(l_oSetInfoResp.m_oHeader, SET_CONTACT_RESPOND, QUEUE_PHONE_BOOK, l_oSetInfoRequest.m_oHeader.m_strMsgid);
		l_oSetInfoResp.m_oHeader.m_strMsgId = l_oSetInfoRequest.m_oHeader.m_strMsgId;
		l_oSetInfoResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");
		l_oSetInfoResp.m_oBody.m_strMessage = l_oInfoCached.m_oBody.m_strGuid;

		std::string l_strMessage = l_oSetInfoResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			std::string l_strInfo = l_oInfoCached.ToString(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(ADDRESS_BOOK_CONTACT, l_oInfoCached.m_oBody.m_strGuid, l_strInfo);
			SyncContactInfo(l_oInfoCached, l_eSyncType);
		}
	}
}
//删除电话信息
void CBusinessImpl::OnNotifiDeleteBookContactRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive DeleteBookContact message:[%s]", p_pNotify->GetMessages().c_str());

		PROTOCOL::CDeleteContactInfoRequest l_oDeleInfoRequest;
		if (!l_oDeleInfoRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "DeleteBookContact request not json:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		bool l_bResult = true;
		std::string l_strGuid = l_oDeleInfoRequest.m_oBody.m_strGuid;
		l_bResult = DelContactInfo(l_strGuid);

		PROTOCOL::CBaseRespond l_oDelInfoResp;
		BuildProtocolHeader(l_oDelInfoResp.m_oHeader, DEL_CONTACT_RESPON, QUEUE_PHONE_BOOK, l_oDeleInfoRequest.m_oHeader.m_strMsgid);
		l_oDelInfoResp.m_oHeader.m_strMsgId = l_oDeleInfoRequest.m_oHeader.m_strMsgId;
		l_oDelInfoResp.m_oBody.m_strResult = (l_bResult ? "0" : "1");

		std::string l_strMessage = l_oDelInfoResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send response:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			m_pRedisClient->HDel(ADDRESS_BOOK_CONTACT, l_strGuid);

			PROTOCOL::CContactInfo l_oDeleteInfo;
			l_oDeleteInfo.m_oBody.m_strGuid = l_strGuid;
			SyncContactInfo(l_oDeleteInfo, ESyncType::SYNC_TYPE_DELETE);
		}
	}
}
void CBusinessImpl::SyncContactInfo(const PROTOCOL::CContactInfo& p_oContact, ESyncType p_eSyncType)
{
	PROTOCOL::CContactInfo l_oContactSync;
	BuildProtocolHeader(l_oContactSync.m_oHeader, CMD_CONTACT_SYNC, TOPIC_BOOK_SYNC, PROTOCOL_TOPIC);
	l_oContactSync.m_oHeader.m_strRequestFlag = "MQ";
	l_oContactSync.m_oBody.m_strSyncType = std::to_string(p_eSyncType);
	l_oContactSync.m_oBody.m_strGuid = p_oContact.m_oBody.m_strGuid;
	l_oContactSync.m_oBody.m_strGrpGuid = p_oContact.m_oBody.m_strGrpGuid;
	l_oContactSync.m_oBody.m_strType = p_oContact.m_oBody.m_strType;
	l_oContactSync.m_oBody.m_strName = p_oContact.m_oBody.m_strName;
	l_oContactSync.m_oBody.m_strPhone = p_oContact.m_oBody.m_strPhone;
	l_oContactSync.m_oBody.m_strSort = p_oContact.m_oBody.m_strSort;
	l_oContactSync.m_oBody.m_strRemark = p_oContact.m_oBody.m_strRemark;

	std::string l_strSyncMsg = l_oContactSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMsg));
	ICC_LOG_DEBUG(m_pLog, "Send SyncContactInfo Msg:[%s]", l_strSyncMsg.c_str());
}

void CBusinessImpl::BuildProtocolHeader(PROTOCOL::CHeader& p_oOutHeader, const std::string& p_strCmd, const std::string& p_strRequest,
	const std::string& p_strRequestType /*= PROTOCOL_QUEUE*/, const std::string& p_strRelateId /*= ""*/)
{
	p_oOutHeader.m_strSystemID = SYSTEMID;
	p_oOutHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oOutHeader.m_strMsgid = m_pString->CreateGuid();
	p_oOutHeader.m_strRelatedID = p_strRelateId;
	p_oOutHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oOutHeader.m_strCmd = p_strCmd;
	p_oOutHeader.m_strRequest = p_strRequest;
	p_oOutHeader.m_strRequestType = p_strRequestType;
	p_oOutHeader.m_strResponse = "";
	p_oOutHeader.m_strResponseType = "";
}