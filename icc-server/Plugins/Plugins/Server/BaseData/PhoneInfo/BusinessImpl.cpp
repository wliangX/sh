#include "Boost.h"
#include "BusinessImpl.h"
#include <iostream>
#include <thread>

#define LOAD_WAITTIME 1000

#define PhoneInfo  "PhoneInfo"

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
	m_mapPhoneInfoLock = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
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

	while (!LoadPhoneInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.phoneinfo failed to load phone info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_phoneinfo_request", OnCNotifiGetAllPhoneInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_phoneinfo_request", OnCNotifiDeletePhoneInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_phoneinfo_request", OnCNotifiGetPhoneInfoByPhoneRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_phoneinfo_request", OnCNotifiSetPhoneInfoRequest);

	//LoadPhoneInfo();

	ICC_LOG_DEBUG(m_pLog, "plugin basedata.phoneinfo start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	//m_pRedisClient->Disconnect();
	ICC_LOG_DEBUG(m_pLog, "phoneinfo stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiGetAllPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetPhoneInfoRequest l_oGetSetRequest;
	if (!l_oGetSetRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "GetAllPhoneInfoRequest is Invilid [%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strPageSize = l_oGetSetRequest.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oGetSetRequest.m_oBody.m_strPageIndex;
	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	std::map<std::string, std::string> l_mapPhoneInfo;
	m_pRedisClient->HGetAll(PhoneInfo, l_mapPhoneInfo);
	ICC_LOG_DEBUG(m_pLog, "HGetAll PhoneInfo complete! msgid = %s", l_oGetSetRequest.m_oHeader.m_strMsgId.c_str());

	PROTOCOL::CGetPhoneInfoRespond l_oGetAllPhoneInfoRespond;
	l_oGetAllPhoneInfoRespond.m_oBody.m_strPageIndex = l_strPageIndex;
	l_oGetAllPhoneInfoRespond.m_oBody.m_strAllCount = std::to_string(l_mapPhoneInfo.size());
	l_oGetAllPhoneInfoRespond.m_oHeader.m_strMsgId = l_oGetSetRequest.m_oHeader.m_strMsgId;

	if (l_mapPhoneInfo.size() == 0)
	{
		ICC_LOG_WARNING(m_pLog, "Get AllPhoneInfo Error : No Search Data");

		l_oGetAllPhoneInfoRespond.m_oBody.m_strCount = "0";
		PROTOCOL::CGetPhoneInfoRespond::CBody::CData l_oData;
		l_oData.m_strPhone = "";
		l_oData.m_strName = "";
		l_oData.m_strAddress = "";
		l_oGetAllPhoneInfoRespond.m_oBody.m_vecData.push_back(l_oData);
	}
	else
	{
		int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
		int l_nEndIndex = l_nPageIndex * l_nPageSize;

		if (l_nBeginIndex > l_mapPhoneInfo.size())
		{
			l_oGetAllPhoneInfoRespond.m_oHeader.m_strResult = "1";
			l_oGetAllPhoneInfoRespond.m_oHeader.m_strMsg = "Out of range";
			l_oGetAllPhoneInfoRespond.m_oBody.m_strCount = "0";
		}
		else
		{
			std::vector<std::pair<std::string, std::string>> l_vecPhoneData(l_mapPhoneInfo.begin(), l_mapPhoneInfo.end());

			for (auto it = l_vecPhoneData.begin() + l_nBeginIndex;
				it < l_vecPhoneData.begin() + l_nEndIndex && it != l_vecPhoneData.end(); it++)
			{
				CPhoneInfo l_oPhoneInfo;
				PROTOCOL::CGetPhoneInfoRespond::CBody::CData l_oData;
				if (l_oPhoneInfo.Parse(it->second, m_pJsonFty->CreateJson()))
				{
					l_oData.m_strPhone = l_oPhoneInfo.m_strPhone;
					l_oData.m_strName = l_oPhoneInfo.m_strName;
					l_oData.m_strAddress = l_oPhoneInfo.m_strAddress;
					l_oGetAllPhoneInfoRespond.m_oBody.m_vecData.push_back(l_oData);
				}
			}

			l_oGetAllPhoneInfoRespond.m_oBody.m_strCount = std::to_string(l_oGetAllPhoneInfoRespond.m_oBody.m_vecData.size());
		}
	}

	std::string l_strMessage = l_oGetAllPhoneInfoRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send GetAllPhoneInfoRespond:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetPhoneInfoByPhoneRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetPhoneInfoByPhoneRequest l_oGetPhoneInfoByPhoneRequest;
	if (!l_oGetPhoneInfoByPhoneRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "GetPhoneInfoByPhoneRequest is Invilid [%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	bool l_bFind = true;
	std::string l_strPhoneInfo;
	std::string l_strPhoneNum = l_oGetPhoneInfoByPhoneRequest.m_oBody.m_strPhone;
	m_pRedisClient->HGet(PhoneInfo, l_strPhoneNum, l_strPhoneInfo);
	CPhoneInfo l_oPhoneInfoByPhone;
	if (!l_oPhoneInfoByPhone.Parse(l_strPhoneInfo, m_pJsonFty->CreateJson()))
	{
		l_bFind = false;
		ICC_LOG_DEBUG(m_pLog, "[%s] Not found phoneinfo or phoneinfo parse json failed", l_strPhoneNum.c_str());
	}

	PROTOCOL::CGetPhoneInfoByPhoneRespond l_oGetPhoneInfoByPhoneRespond;
	PROTOCOL::CGetPhoneInfoByPhoneRespond::CBody::CData l_oTempData;
	l_oTempData.m_strPhone = l_oPhoneInfoByPhone.m_strPhone;
	l_oTempData.m_strName = l_oPhoneInfoByPhone.m_strName;
	l_oTempData.m_strAddress = l_oPhoneInfoByPhone.m_strAddress;
	l_oGetPhoneInfoByPhoneRespond.m_oBody.m_vecData.push_back(l_oTempData);
	l_oGetPhoneInfoByPhoneRespond.m_oBody.m_strCount = (l_bFind ? "1" : "0");

	std::string l_strMessage = l_oGetPhoneInfoByPhoneRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetPhoneInfoByPhoneRespond:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiSetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSetPhoneInfoRequest l_oSetPhoneInfoRequest;
	if (!l_oSetPhoneInfoRequest.ParseString(p_pNotify->GetMessages(), ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "SetPhoneInfoRequest is Invilid [%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	std::string l_strPhone = l_oSetPhoneInfoRequest.m_oBody.m_strPhone;
	std::string l_strName = l_oSetPhoneInfoRequest.m_oBody.m_strName;
	std::string l_strAddress = l_oSetPhoneInfoRequest.m_oBody.m_strAddress;

	bool l_bIsNotAdd = false;
	bool l_bIsUpdate = false;
	CPhoneInfo l_oPhoneInfo;
	l_oPhoneInfo.m_strAddress = l_strAddress;
	l_oPhoneInfo.m_strName = l_strName;
	l_oPhoneInfo.m_strPhone = l_strPhone;
	PROTOCOL::CSetPhoneInfoRespond l_oSetPhoneInfoResp;
	l_oSetPhoneInfoResp.m_oHeader.m_strMsgId = l_oSetPhoneInfoRequest.m_oHeader.m_strMsgId;

	l_bIsNotAdd = m_pRedisClient->HExists(PhoneInfo, l_strPhone);
	if (!l_bIsNotAdd)
	{
		l_bIsUpdate = IsRecoverPhoneInfo(l_oPhoneInfo);
		if (!l_bIsUpdate)
		{
			SetPhoneInfo(l_oPhoneInfo, l_oSetPhoneInfoResp, p_pNotify);
			//发送同步
			SyncClient(l_oPhoneInfo, PHONEINFO_ADD);
			SendSetPhoneinfoResponse(l_oSetPhoneInfoResp, p_pNotify);
		}
	}
	if (l_bIsNotAdd || l_bIsUpdate)
	{
		UpdatePhoneInfo(l_oPhoneInfo, l_oSetPhoneInfoResp, p_pNotify);
		//发送同步
		if (l_bIsUpdate)
		{
			//发送恢复标志位同步
			SyncClient(l_oPhoneInfo, PHONEINFO_ADD);
		}
		else
		{
			//发送更新同步
			SyncClient(l_oPhoneInfo, PHONEINFO_UPDATE);
		}
		SendSetPhoneinfoResponse(l_oSetPhoneInfoResp, p_pNotify);
	}
}

void CBusinessImpl::OnCNotifiDeletePhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	
	PROTOCOL::CDeletePhoneInfoRequest l_oDeletePhoneInfoRequest;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_oDeletePhoneInfoRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse json failed");
		return;
	}
	
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	std::string l_strPhone = l_oDeletePhoneInfoRequest.m_oBody.m_strPhone;
	PROTOCOL::CDeletePhoneInfoRespond l_oDeletePhoneInfoResp;
	l_oDeletePhoneInfoResp.m_oHeader.m_strMsgId = l_oDeletePhoneInfoRequest.m_oHeader.m_strMsgId;

	//逻辑删除操作
	DataBase::SQLRequest l_oDelete;
	l_oDelete.sql_id = "update_icc_t_phoneinfo";
	l_oDelete.param.insert(std::pair<std::string, std::string>("phone", l_strPhone));
	l_oDelete.set.insert(std::pair<std::string, std::string>("is_delete", "true"));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
	l_oDelete.set.insert(std::pair<std::string, std::string>("update_time", l_strCurrentTime));
	DataBase::IResultSetPtr l_pDelete = m_pDBConn->Exec(l_oDelete);
	if (!l_pDelete->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,delete operation failed, sql: [%s], Error:[%s]", 
			l_pDelete->GetSQL().c_str(),l_pDelete->GetErrorMsg().c_str());

		l_oDeletePhoneInfoResp.m_oHeader.m_strResult = "1";
		l_oDeletePhoneInfoResp.m_oHeader.m_strMsg = "delete operation failed";
		std::string l_strMessage = l_oDeletePhoneInfoResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);

		ICC_LOG_DEBUG(m_pLog, "send DeletePhoneInfoRespond:[%s]", l_strMessage.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Exec sql success:[%s]", l_pDelete->GetSQL().c_str());

		std::string l_strPhoneInfo;
		m_pRedisClient->HGet(PhoneInfo, l_strPhone, l_strPhoneInfo);
		CPhoneInfo l_oPhoneInfo;
		l_oPhoneInfo.Parse(l_strPhoneInfo, m_pJsonFty->CreateJson());

		m_pRedisClient->HDel(PhoneInfo, l_strPhone);
		
		//发送同步
		SyncClient(l_oPhoneInfo, PHONEINFO_DELETE);

		l_oDeletePhoneInfoResp.m_oHeader.m_strResult = "0";
		std::string l_strMessage = l_oDeletePhoneInfoResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);

		ICC_LOG_DEBUG(m_pLog, "send DeletePhoneInfoRespond:[%s]", l_strMessage.c_str());
	}
}

bool CBusinessImpl::LoadPhoneInfo()
{
	m_pRedisClient->Del(PhoneInfo);

	bool l_bLoadTag = false;

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phoneinfo";
	l_SqlRequest.param["is_delete"] = "false";
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	if (l_result->RecordSize() > 0)
	{
		//初始化三字段信息缓存
		CPhoneInfo l_oPhoneInfo;
		while (l_result->Next())
		{
			l_oPhoneInfo.m_strPhone = l_result->GetValue("phone");
			l_oPhoneInfo.m_strName = l_result->GetValue("name");
			l_oPhoneInfo.m_strAddress = l_result->GetValue("address");

			std::string l_strVal = l_oPhoneInfo.ToJson(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(PhoneInfo, l_oPhoneInfo.m_strPhone.c_str(), l_strVal);
		}
	}
	ICC_LOG_DEBUG(m_pLog, "load user data success [count:%d]", l_result->RecordSize());
	return true;
}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader)
{
	p_oRespondHeader.m_strSystemID = p_oRequestHeader.m_strSystemID;
	p_oRespondHeader.m_strSubsystemID = p_oRequestHeader.m_strSubsystemID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = m_pString->ReplaceLast(p_oRequestHeader.m_strCmd, "_request", "_respond");
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

void CBusinessImpl::SyncClient(CPhoneInfo p_ClientRegisterInfo, ePhoneInfoStatus p_eState)
{
	if (p_ClientRegisterInfo.m_strPhone.empty())
	{
		ICC_LOG_ERROR(m_pLog, "PhoneInfo NotifiClientSync Info Error");
		return;
	}

	std::string l_strMessage;
	PROTOCOL::CPhoneInfoSync l_oPhoneInfoSync;
	std::string l_strSyncType = std::to_string(p_eState);
	l_oPhoneInfoSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oPhoneInfoSync.m_oHeader.m_strCmd = "phoneinfo_sync";
	l_oPhoneInfoSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oPhoneInfoSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;
	l_oPhoneInfoSync.m_oBody.m_strPhone = p_ClientRegisterInfo.m_strPhone;
	l_oPhoneInfoSync.m_oBody.m_strName = p_ClientRegisterInfo.m_strName;
	l_oPhoneInfoSync.m_oBody.m_strAddress = p_ClientRegisterInfo.m_strAddress;
	l_oPhoneInfoSync.m_oBody.m_strSyncType = l_strSyncType;

	l_strMessage = l_oPhoneInfoSync.ToString(m_pJsonFty->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send ClientisterSync [%s]", l_strMessage.c_str());
}

void CBusinessImpl::SetPhoneInfo(CPhoneInfo& p_pPhoneInfo, PROTOCOL::CSetPhoneInfoRespond& p_pSetPhoneInfoResp,ObserverPattern::INotificationPtr& p_pNotifiSetPhoneInfoRequest)
{
	//进行新增
	std::string l_strGuid = m_pString->CreateGuid();
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DataBase::SQLRequest l_oAdd;
	l_oAdd.sql_id = "insert_icc_t_phoneinfo";
	l_oAdd.param.insert(std::pair<std::string, std::string>("guid", l_strGuid));
	l_oAdd.param.insert(std::pair<std::string, std::string>("phone", p_pPhoneInfo.m_strPhone));
	l_oAdd.param.insert(std::pair<std::string, std::string>("name", p_pPhoneInfo.m_strName));
	l_oAdd.param.insert(std::pair<std::string, std::string>("address", p_pPhoneInfo.m_strAddress));
	l_oAdd.param.insert(std::pair<std::string, std::string>("is_delete", "false"));
	l_oAdd.param.insert(std::pair<std::string, std::string>("create_user", "BaseData"));
	l_oAdd.param.insert(std::pair<std::string, std::string>("create_time", l_strCurrentTime));
	l_oAdd.param.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
	l_oAdd.param.insert(std::pair<std::string, std::string>("update_time", l_strCurrentTime));

	DataBase::IResultSetPtr p_Add = m_pDBConn->Exec(l_oAdd);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Add->GetSQL().c_str());
	if (!p_Add->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,insert operation failed:[DB Error]:%s", p_Add->GetErrorMsg().c_str());

		p_pSetPhoneInfoResp.m_oHeader.m_strResult = "1";
		p_pSetPhoneInfoResp.m_oHeader.m_strMsg = "data is invalid,update operation failed";

		SendSetPhoneinfoResponse(p_pSetPhoneInfoResp, p_pNotifiSetPhoneInfoRequest);

		return;
	}
	ICC_LOG_DEBUG(m_pLog, "insert operation success");

	std::string l_strVal = p_pPhoneInfo.ToJson(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet(PhoneInfo, p_pPhoneInfo.m_strPhone.c_str(), l_strVal);
}

void CBusinessImpl::UpdatePhoneInfo(CPhoneInfo& p_pPhoneInfo, PROTOCOL::CSetPhoneInfoRespond& p_pSetPhoneInfoResp, ObserverPattern::INotificationPtr& p_pNotifiSetPhoneInfoRequest)
{
	//进行更新操作
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DataBase::SQLRequest l_oUpdate;
	l_oUpdate.sql_id = "update_icc_t_phoneinfo";
	l_oUpdate.param.insert(std::pair<std::string, std::string>("phone", p_pPhoneInfo.m_strPhone));
	l_oUpdate.set.insert(std::pair<std::string, std::string>("name", p_pPhoneInfo.m_strName));
	l_oUpdate.set.insert(std::pair<std::string, std::string>("address", p_pPhoneInfo.m_strAddress));
	l_oUpdate.set.insert(std::pair<std::string, std::string>("is_delete", "false"));
	l_oUpdate.set.insert(std::pair<std::string, std::string>("update_user", "BaseData"));
	l_oUpdate.set.insert(std::pair<std::string, std::string>("update_time", l_strCurrentTime));
	DataBase::IResultSetPtr Updateptr = m_pDBConn->Exec(l_oUpdate);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", Updateptr->GetSQL().c_str());
	if (!Updateptr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,update operation failed");

		p_pSetPhoneInfoResp.m_oHeader.m_strResult = "1";
		p_pSetPhoneInfoResp.m_oHeader.m_strMsg = "data is invalid,update operation failed";
		SendSetPhoneinfoResponse(p_pSetPhoneInfoResp, p_pNotifiSetPhoneInfoRequest);

		return;
	}

	std::string l_strVal = p_pPhoneInfo.ToJson(m_pJsonFty->CreateJson());
	m_pRedisClient->HSet(PhoneInfo, p_pPhoneInfo.m_strPhone, l_strVal);
	ICC_LOG_DEBUG(m_pLog, "update operation success");
}

bool CBusinessImpl::IsRecoverPhoneInfo(CPhoneInfo& p_pPhoneInfo)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phoneinfo";
	l_SqlRequest.param["phone"] = p_pPhoneInfo.m_strPhone;
	l_SqlRequest.param["is_delete"] = "true";
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	else if (l_result->RecordSize() > 0)
	{
		return true;
	}
	return false;
}

void CBusinessImpl::SendSetPhoneinfoResponse(PROTOCOL::CSetPhoneInfoRespond& p_pPhoneInfoResp,ObserverPattern::INotificationPtr p_pNotifiSetPhoneInfoRequest)
{
	std::string l_strMessage = p_pPhoneInfoResp.ToString(m_pJsonFty->CreateJson());
	p_pNotifiSetPhoneInfoRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}