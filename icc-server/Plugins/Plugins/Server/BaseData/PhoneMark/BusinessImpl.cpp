#include "Boost.h"
#include "BusinessImpl.h"

#include <thread>

#define LOAD_WAITTIME 1000

#define PhoneMark ("PhoneMark")

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
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pIDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pJsonFac = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{	
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	while (!LoadPhoneMarkFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.phonemark failed to load phonemark info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	//LoadPhoneMarkFromDB();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_phonemark_request", OnCNotifiSetPhoneMarkRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_phonemark_request", OnCNotifiGetAllPhoneMarkRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_phonemark_request", OnNotifiDeletePhoneMarkRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_phonemark_request", OnCNotifiGetPhoneMarkRequest);

	ICC_LOG_DEBUG(m_pLog, "plugin basedata.phonemark start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);

}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "phonemark stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiSetPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSetPhoneMarkRequest l_oSetPhoneMarkRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oSetPhoneMarkRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "PhoneMark Error SetPhoneMarkRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = l_oSetPhoneMarkRequest.m_oBody.m_strGuid;

	DataBase::SQLRequest l_oPhoneMarkSQLReq;
	bool l_bUpdate = false;
	if (!l_strGuid.empty() && ExistInCache(l_strGuid))
	{
		l_oPhoneMarkSQLReq.sql_id = "update_icc_t_phonemark";
		l_oPhoneMarkSQLReq.set["phone"] = l_oSetPhoneMarkRequest.m_oBody.m_strPhone;
		l_oPhoneMarkSQLReq.set["type"] = l_oSetPhoneMarkRequest.m_oBody.m_strType;
		l_oPhoneMarkSQLReq.set["staff_code"] = l_oSetPhoneMarkRequest.m_oBody.m_strStaffCode;
		l_oPhoneMarkSQLReq.set["remark"] = l_oSetPhoneMarkRequest.m_oBody.m_strRemark;
		l_oPhoneMarkSQLReq.set["update_user"] = "icc_basedata_phonemark";
		l_oPhoneMarkSQLReq.set["is_delete"] = "false";
		l_oPhoneMarkSQLReq.set["update_time"] = l_strCurrentTime;

		l_oPhoneMarkSQLReq.param["guid"] = l_strGuid;
		l_bUpdate = true;
	}
	else
	{
		l_oPhoneMarkSQLReq.sql_id = "insert_icc_t_phonemark";
		l_oPhoneMarkSQLReq.param["phone"] = l_oSetPhoneMarkRequest.m_oBody.m_strPhone;
		l_oPhoneMarkSQLReq.param["type"] = l_oSetPhoneMarkRequest.m_oBody.m_strType;

		l_oPhoneMarkSQLReq.param["staff_code"] = l_oSetPhoneMarkRequest.m_oBody.m_strStaffCode;
		l_oPhoneMarkSQLReq.param["remark"] = l_oSetPhoneMarkRequest.m_oBody.m_strRemark;

		l_oPhoneMarkSQLReq.param["is_delete"] = "false";
		l_oPhoneMarkSQLReq.param["create_user"] = "icc_basedata_phonemark";
		l_oPhoneMarkSQLReq.param["create_time"] = l_strCurrentTime;

		if (l_strGuid.empty()) l_strGuid = m_pString->CreateGuid();
		l_oPhoneMarkSQLReq.param["guid"] = l_strGuid;
	}
	
	PROTOCOL::CSetPhoneMarkRespond l_oSetPhoneMarkRespond;
	l_oSetPhoneMarkRespond.m_oHeader.m_strMsgId = l_oSetPhoneMarkRequest.m_oHeader.m_strMsgId;
	if (!ExecSql(l_oPhoneMarkSQLReq, ""))
	{
		//l_oSetPhoneMarkRespond.m_oBody.m_strResult = "1";//失败
		l_oSetPhoneMarkRespond.m_oHeader.m_strResult = "Exec Sql Failed";
		l_oSetPhoneMarkRespond.m_oHeader.m_strMsg = "1";
	}
	else
	{
		//l_oSetPhoneMarkRespond.m_oBody.m_strResult = "0";//成功

		//更新缓存，同步电话标记
		CPhoneMark l_tPhoneMark;
		l_tPhoneMark.strGuid = l_strGuid;
		l_tPhoneMark.strPhone = l_oSetPhoneMarkRequest.m_oBody.m_strPhone;
		l_tPhoneMark.strType = l_oSetPhoneMarkRequest.m_oBody.m_strType;
		l_tPhoneMark.strStaffCode = l_oSetPhoneMarkRequest.m_oBody.m_strStaffCode;
		l_tPhoneMark.strRemark = l_oSetPhoneMarkRequest.m_oBody.m_strRemark;

		AddPhoneMark(l_strGuid, l_tPhoneMark);
		if (l_bUpdate)
		{
			SendSyncPhoneMark(l_tPhoneMark, ICC_MODIFY_PHONEREMARM);
		}
		else
		{
			SendSyncPhoneMark(l_tPhoneMark, ICC_ADD_PHONEREMARM);
		}
	}

	std::string l_strMessage = l_oSetPhoneMarkRespond.ToString(m_pJsonFac->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

std::string CBusinessImpl::_GetPhoneMarkTimes(const std::string& p_strPhone, const std::string& p_strType)
{
	std::string l_strRes = "";
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phonemark_count_by_phone";

	l_SqlRequest.param["phone"] = p_strPhone;
	l_SqlRequest.param["type"] = p_strType;
	l_SqlRequest.param["is_delete"] = "false";
	
	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strRes;
	}

	if (l_pResult->Next())
	{
		l_strRes = l_pResult->GetValue("count");
	}
	return l_strRes;
}

void CBusinessImpl::OnCNotifiGetPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetPhoneMarkRequest l_oGetPhoneMarkRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oGetPhoneMarkRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "PhoneMark Error SetPhoneMarkRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetPhoneMarkRespond l_oGetPhoneMarkRespond;
	l_oGetPhoneMarkRespond.m_oHeader.m_strMsgId = l_oGetPhoneMarkRequest.m_oHeader.m_strMsgId;
	//GenRespHeader("get_phonemark_respond", l_oGetPhoneMarkRequest.m_oHeader, l_oGetPhoneMarkRespond.m_oHeader);

	CPhoneMark l_tPhoneMark;
	int l_nCount = 0;

	std::map<std::string, std::string> l_mapPhoneMark;
	GetAllPhoneMarkInfo(l_mapPhoneMark);
	for (auto it = l_mapPhoneMark.begin(); it != l_mapPhoneMark.end(); it++)
	{
		CPhoneMark l_oPhoneMark;
		if (!l_oPhoneMark.Parse(it->second, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Parse PhoneMarkInfo From Redis Error [%s]", it->second.c_str());
			break;
		}

		PROTOCOL::CGetPhoneMarkRespond::CBody::CData l_oData;
		if (l_oGetPhoneMarkRequest.m_oBody.m_strPhone == l_oPhoneMark.strPhone)
		{
			l_oData.m_strGuid = l_oPhoneMark.strGuid;
			l_oData.m_strPhone = l_oPhoneMark.strPhone;
			l_oData.m_strRemark = l_oPhoneMark.strRemark;
			l_oData.m_strStaffCode = l_oPhoneMark.strStaffCode;
			l_oData.m_strType = l_oPhoneMark.strType;

			l_oData.m_strTimes = _GetPhoneMarkTimes(l_oPhoneMark.strPhone, l_oPhoneMark.strType);

			l_oGetPhoneMarkRespond.m_oBody.m_vecData.push_back(l_oData);
			l_nCount++;
		}
	}
	l_oGetPhoneMarkRespond.m_oBody.m_strCount = m_pString->Number(l_nCount);
	
	std::string l_strMessage = l_oGetPhoneMarkRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetAllPhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetAllPhoneMarkRequest l_oGetAllPhoneMarkRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oGetAllPhoneMarkRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCNotifiGetAllPhoneMarkRequest] parse json error! msg = [%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strPageSize = l_oGetAllPhoneMarkRequest.m_oBody.m_strPageSize;
	std::string l_strPageIndex = l_oGetAllPhoneMarkRequest.m_oBody.m_strPageIndex;
	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	//std::map<std::string, CPhoneMark> l_mapTmpCPhoneMark;
	std::map<std::string, std::string> l_mapPhoneMark;
	if (!l_oGetAllPhoneMarkRequest.m_oBody.m_strPhone.empty())
	{
		std::map<std::string, std::string> l_mapTmpPhoneMark;
		GetAllPhoneMarkInfo(l_mapTmpPhoneMark);
		for (auto tmpIt = l_mapTmpPhoneMark.begin(); tmpIt != l_mapTmpPhoneMark.end(); tmpIt++)
		{
			CPhoneMark l_oPhoneMark;
			if (!l_oPhoneMark.Parse(tmpIt->second, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "Parse PhoneMarkInfo From Redis Error [%s]", tmpIt->second.c_str());
				continue;
			}

			if (l_oPhoneMark.strPhone == l_oGetAllPhoneMarkRequest.m_oBody.m_strPhone)
			{
				l_mapPhoneMark[tmpIt->first] = tmpIt->second;
				//l_mapTmpCPhoneMark[l_oPhoneMark.strGuid] = l_oPhoneMark;
			}
		}
	}
	else
	{
		GetAllPhoneMarkInfo(l_mapPhoneMark);
	}

	ICC_LOG_DEBUG(m_pLog, "HGetAll PhoneMarkInfo complete! msgid = %s", l_oGetAllPhoneMarkRequest.m_oHeader.m_strMsgId.c_str());

	PROTOCOL::CGetPhoneMarkRespond l_oGetAllPhoneMarkRespond;
	//GenRespHeader("get_all_phonemark_respond", l_oGetAllPhoneMarkRequest.m_oHeader, l_oGetAllPhoneMarkRespond.m_oHeader);
	l_oGetAllPhoneMarkRespond.m_oHeader.m_strMsgId = l_oGetAllPhoneMarkRequest.m_oHeader.m_strMsgId;
	l_oGetAllPhoneMarkRespond.m_oBody.m_strPageIndex = l_oGetAllPhoneMarkRequest.m_oBody.m_strPageIndex;
	l_oGetAllPhoneMarkRespond.m_oBody.m_strAllCount = std::to_string(l_mapPhoneMark.size());

	if (l_mapPhoneMark.size() == 0)
	{
		ICC_LOG_WARNING(m_pLog, "Get AllPhoneMark Error : No Search Data");

		l_oGetAllPhoneMarkRespond.m_oBody.m_strCount = "0";

		PROTOCOL::CGetPhoneMarkRespond::CBody::CData l_oData;
		l_oData.m_strGuid = "";
		l_oData.m_strPhone = "";
		l_oData.m_strRemark = "";
		l_oData.m_strStaffCode = "";
		l_oData.m_strType = "";
		l_oGetAllPhoneMarkRespond.m_oBody.m_vecData.push_back(l_oData);
	}
	else
	{
		int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
		int l_nEndIndex = l_nPageIndex * l_nPageSize;

		if (l_nBeginIndex > l_mapPhoneMark.size())
		{
			l_oGetAllPhoneMarkRespond.m_oHeader.m_strResult = "1";
			l_oGetAllPhoneMarkRespond.m_oHeader.m_strMsg = "Out of range";
			l_oGetAllPhoneMarkRespond.m_oBody.m_strCount = "0";
		}
		else
		{
			std::vector<std::pair<std::string, std::string>> l_vecPhoneMark(l_mapPhoneMark.begin(), l_mapPhoneMark.end());

			for (auto it = l_vecPhoneMark.begin() + l_nBeginIndex;
				it < l_vecPhoneMark.begin() + l_nEndIndex && it != l_vecPhoneMark.end(); it++)
			{
				CPhoneMark l_oPhoneMark;
				if (!l_oPhoneMark.Parse(it->second, m_pJsonFac->CreateJson()))
				{
					ICC_LOG_ERROR(m_pLog, "Parse PhoneMarkInfo From Redis Error [%s]", it->second.c_str());
					break;
				}

				PROTOCOL::CGetPhoneMarkRespond::CBody::CData l_oData;
				l_oData.m_strGuid = l_oPhoneMark.strGuid;
				l_oData.m_strPhone = l_oPhoneMark.strPhone;
				l_oData.m_strRemark = l_oPhoneMark.strRemark;
				l_oData.m_strStaffCode = l_oPhoneMark.strStaffCode;
				l_oData.m_strType = l_oPhoneMark.strType;

				l_oData.m_strTimes = _GetPhoneMarkTimes(l_oPhoneMark.strPhone, l_oPhoneMark.strType);

				l_oGetAllPhoneMarkRespond.m_oBody.m_vecData.push_back(l_oData);
			}

			l_oGetAllPhoneMarkRespond.m_oBody.m_strCount = std::to_string(l_oGetAllPhoneMarkRespond.m_oBody.m_vecData.size());
		}
	}

	std::string l_strMessage = l_oGetAllPhoneMarkRespond.ToString(m_pJsonFac->CreateJson());

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiDeletePhoneMarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CDeletePhoneMarkRequest l_oDeletePhoneMarkRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oDeletePhoneMarkRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "PhoneMark Error SetPhoneMarkRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CDeletePhoneMarkRespond l_oDeletePhoneMarkRespond;
	//GenRespHeader("delete_phonemark_respond", l_oDeletePhoneMarkRequest.m_oHeader, l_oDeletePhoneMarkRespond.m_oHeader);
	l_oDeletePhoneMarkRespond.m_oHeader.m_strMsgId = l_oDeletePhoneMarkRequest.m_oHeader.m_strMsgId;

	std::string l_strGuid = l_oDeletePhoneMarkRequest.m_oBody.m_strGuid;

	if (!l_strGuid.empty() && ExistInCache(l_strGuid))
	{
		DataBase::SQLRequest l_oPhoneMarkSQLReq;
		l_oPhoneMarkSQLReq.sql_id = "update_icc_t_phonemark";
		l_oPhoneMarkSQLReq.set["is_delete"] = "true";
		l_oPhoneMarkSQLReq.set["update_user"] = "icc_basedata_phonemark";
		l_oPhoneMarkSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

		l_oPhoneMarkSQLReq.param["guid"] = l_strGuid;
		std::string l_strTransactionGuid = m_pIDBConn->BeginTransaction();
		if (!ExecSql(l_oPhoneMarkSQLReq, l_strTransactionGuid))
		{
			m_pIDBConn->Rollback(l_strTransactionGuid);
			ICC_LOG_ERROR(m_pLog, "PhoneMark Error SetPhoneMarkError");
			//l_oDeletePhoneMarkRespond.m_oBody.m_strResult = "1";//失败
			l_oDeletePhoneMarkRespond.m_oHeader.m_strResult = "1";
			l_oDeletePhoneMarkRespond.m_oHeader.m_strMsg = "Exec update sql failed";
		}

		CPhoneMark l_tPhoneMark;
		if (GetPhoneMarkByGuid(l_strGuid, l_tPhoneMark) && DeletePhoneMark(l_strGuid))
		{			
			SendSyncPhoneMark(l_tPhoneMark, ICC_DELETE_PHONEREMARM);
			//l_oDeletePhoneMarkRespond.m_oBody.m_strResult = "0";//成功
			m_pIDBConn->Commit(l_strTransactionGuid);
		}
		else
		{
			m_pIDBConn->Rollback(l_strTransactionGuid);
			//l_oDeletePhoneMarkRespond.m_oBody.m_strResult = "1";//失败
			ICC_LOG_ERROR(m_pLog, "PhoneMark Delete PhoneMark From Redis Fail guid[%s]",l_strGuid.c_str());
			l_oDeletePhoneMarkRespond.m_oHeader.m_strResult = "1";
			l_oDeletePhoneMarkRespond.m_oHeader.m_strMsg = "Exec update sql failed";
		}
	}
	/*else
	{
		l_oDeletePhoneMarkRespond.m_oBody.m_strResult = "0";//成功
	}*/
	
	std::string l_strMessage = l_oDeletePhoneMarkRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendSyncPhoneMark(CPhoneMark p_tPhoneMark, eSyncType p_eSyncType)
{
	if (p_tPhoneMark.strPhone.empty())
	{
		return;
	}
	PROTOCOL::CPhoneMarkSync l_oPhoneMarkSync;

	l_oPhoneMarkSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oPhoneMarkSync.m_oHeader.m_strCmd = "phonemark_sync";
	l_oPhoneMarkSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oPhoneMarkSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oPhoneMarkSync.m_oHeader.m_strRequestType = "1";//主题

	l_oPhoneMarkSync.m_oBody.m_strGuid = p_tPhoneMark.strGuid;
	l_oPhoneMarkSync.m_oBody.m_strPhone = p_tPhoneMark.strPhone;
	l_oPhoneMarkSync.m_oBody.m_strType = p_tPhoneMark.strType;
	l_oPhoneMarkSync.m_oBody.m_strStaffCode = p_tPhoneMark.strStaffCode;
	l_oPhoneMarkSync.m_oBody.m_strRemark = p_tPhoneMark.strRemark;
	l_oPhoneMarkSync.m_oBody.m_strSyncType = m_pString->Number(p_eSyncType);

	std::string l_strMessage = l_oPhoneMarkSync.ToString(m_pJsonFac->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "PhoneMark Send Sync Info:[%s]", l_strMessage.c_str());
}	

bool CBusinessImpl::LoadPhoneMarkFromDB()
{
	m_pRedisClient->Del(PhoneMark);

	DataBase::SQLRequest l_oPhoneMarkSQLReq;
	l_oPhoneMarkSQLReq.sql_id = "select_icc_t_phonemark";
    l_oPhoneMarkSQLReq.param["is_delete"] = "false";
	ICC_LOG_DEBUG(m_pLog, "LoadPhoneMarkInfo Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oPhoneMarkSQLReq, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = 0;
	while (l_result->Next())
	{
		CPhoneMark l_tPhoneMark;
		l_tPhoneMark.strGuid = l_result->GetValue("guid");
		l_tPhoneMark.strPhone = l_result->GetValue("phone");
		l_tPhoneMark.strType = l_result->GetValue("type");
		l_tPhoneMark.strStaffCode = l_result->GetValue("staff_code");
		l_tPhoneMark.strRemark = l_result->GetValue("remark");

		std::string l_strVal = l_tPhoneMark.ToJson(m_pJsonFac->CreateJson());
		if (!m_pRedisClient->HSet(PhoneMark, l_tPhoneMark.strGuid, l_strVal))
		{
			--l_iSize;
			ICC_LOG_ERROR(m_pLog, "Load PhoneMark To Redis Failed Guid [%s]", l_tPhoneMark.strGuid.c_str());
		}
		++l_iSize;	
	}
	ICC_LOG_DEBUG(m_pLog, "LoadPhoneMarkInfo Success PhoneMark size[%d] ", l_iSize);
	return true;
}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = "icc_server_basedata_PhoneMark";
	p_pRespHeader.m_strSubsystemID = "icc_server_basedata_PhoneMark";
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
}

bool CBusinessImpl::ExecSql(DataBase::SQLRequest p_oSQLReq, const std::string& strTransGuid)
{
	DataBase::IResultSetPtr l_pRSet = m_pIDBConn->Exec(p_oSQLReq, false, strTransGuid);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet)
	{
		ICC_LOG_ERROR(m_pLog, "PhoneMark Error:[%s]", "l_pRSet is null");
		return false;
	}
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "PhoneMark Error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool ICC::CBusinessImpl::ExistInCache(std::string p_strGuid)
{
	return m_pRedisClient->HExists(PhoneMark, p_strGuid);
}

bool ICC::CBusinessImpl::DeletePhoneMark(std::string p_strGuid)
{
	return m_pRedisClient->HDel(PhoneMark, p_strGuid);
}

bool ICC::CBusinessImpl::GetAllPhoneMarkInfo(std::map<std::string, std::string>& p_mapPhoneMark)
{
	/*int l_iNext = 0;

	while ((l_iNext = m_pRedisClient->HScan(PhoneMark, l_iNext, p_mapPhoneMark)) > 0);

	return l_iNext == 0;	*/
	return m_pRedisClient->HGetAll(PhoneMark, p_mapPhoneMark);
}

void ICC::CBusinessImpl::AddPhoneMark(std::string p_strGuid,const CPhoneMark& p_oPhoneMark)
{
	if (p_strGuid.empty())
	{
		return;
	}

	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	std::string l_strPhoneMark = p_oPhoneMark.ToJson(l_pJson);
	m_pRedisClient->HSet(PhoneMark, p_strGuid, l_strPhoneMark);
}

bool ICC::CBusinessImpl::GetPhoneMarkByGuid(std::string p_strGuid, CPhoneMark &p_oPhoneMark)
{
	std::string l_strPhoneMark;
	if (!m_pRedisClient->HGet(PhoneMark, p_strGuid, l_strPhoneMark))
	{
		return false;
	}
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	return p_oPhoneMark.Parse(l_strPhoneMark, l_pJson);
}
