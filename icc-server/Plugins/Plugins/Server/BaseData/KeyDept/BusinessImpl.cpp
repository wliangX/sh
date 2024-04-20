#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define KeyDept ("KeyDept")

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
	m_pManagerKeyDept = boost::make_shared<CManagerKeyDept>(GetResourceManager());

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	while (!LoadKeyDeptFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.keydept failed to load keydept info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	//LoadKeyDeptFromDB();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_key_dept_request", OnCNotifiSetKeyDeptRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_key_dept_request", OnCNotifiGetAllKeyDeptRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_key_dept_request", OnNotifiDeleteKeyDeptRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "check_if_key_dept_request", OnCNotifiCheckDeptRequest);

	ICC_LOG_INFO(m_pLog, "plugin basedata.keydept start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "keydept stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiSetKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSetKeyDeptRequest l_oSetKeyDeptRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oSetKeyDeptRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error SetKeyDeptRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CSetKeyDeptRespond l_oSetKeyDeptRespond;
	GenRespHeader("set_key_dept_respond", l_oSetKeyDeptRequest.m_oHeader, l_oSetKeyDeptRespond.m_oHeader);

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strPhone = l_oSetKeyDeptRequest.m_oBody.m_strPhone;

	DataBase::SQLRequest l_oKeyDeptSQLReq;
	bool l_bUpdate = false;
	std::string l_strID = m_pString->CreateGuid();
	if (ExistInCache(l_strPhone))
	{
		l_oKeyDeptSQLReq.sql_id = "update_icc_t_keydept";
		l_oKeyDeptSQLReq.set["type"] = l_oSetKeyDeptRequest.m_oBody.m_strType;
		l_oKeyDeptSQLReq.set["name"] = l_oSetKeyDeptRequest.m_oBody.m_strName;
		l_oKeyDeptSQLReq.set["phone"] = l_oSetKeyDeptRequest.m_oBody.m_strPhone;
		l_oKeyDeptSQLReq.set["address"] = l_oSetKeyDeptRequest.m_oBody.m_strAddress;
		l_oKeyDeptSQLReq.set["longitude"] = l_oSetKeyDeptRequest.m_oBody.m_strLongitude;
		l_oKeyDeptSQLReq.set["latitude"] = l_oSetKeyDeptRequest.m_oBody.m_strLattitude;

		l_oKeyDeptSQLReq.set["update_user"] = "icc_basedata_keydept";
		l_oKeyDeptSQLReq.set["is_delete"] = "false";
		l_oKeyDeptSQLReq.set["update_time"] = l_strCurrentTime;

        l_oKeyDeptSQLReq.param["phone"] = l_oSetKeyDeptRequest.m_oBody.m_strPhone;
		l_bUpdate = true;
	}
	else
	{
		l_oKeyDeptSQLReq.sql_id = "insert_icc_t_keydept";
		l_oKeyDeptSQLReq.param["type"] = l_oSetKeyDeptRequest.m_oBody.m_strType;
		l_oKeyDeptSQLReq.param["name"] = l_oSetKeyDeptRequest.m_oBody.m_strName;
		l_oKeyDeptSQLReq.param["phone"] = l_oSetKeyDeptRequest.m_oBody.m_strPhone;
		l_oKeyDeptSQLReq.param["address"] = l_oSetKeyDeptRequest.m_oBody.m_strAddress;
		l_oKeyDeptSQLReq.param["longitude"] = l_oSetKeyDeptRequest.m_oBody.m_strLongitude;
		l_oKeyDeptSQLReq.param["latitude"] = l_oSetKeyDeptRequest.m_oBody.m_strLattitude;


		l_oKeyDeptSQLReq.param["is_delete"] = "false";
		l_oKeyDeptSQLReq.param["create_user"] = "icc_basedata_keydept";
		l_oKeyDeptSQLReq.param["create_time"] = l_strCurrentTime;

		l_oKeyDeptSQLReq.param["guid"] = l_strID;
	}
	
	std::string l_strGuid = m_pIDBConn->BeginTransaction();
	if (!ExecSql(l_oKeyDeptSQLReq, l_strGuid))
	{
		m_pIDBConn->Rollback(l_strGuid);
		l_oSetKeyDeptRespond.m_oBody.m_strResult = "1";//失败
	}
	else
	{
		//更新缓存，同步重点单位
		CKeyDept l_tKeyDept;
        if (!GetKeyDeptByPhone(l_strPhone, l_tKeyDept))
        {
            l_tKeyDept.strID = l_strID;
        }        	
		l_tKeyDept.strType = l_oSetKeyDeptRequest.m_oBody.m_strType;		
		l_tKeyDept.strName = l_oSetKeyDeptRequest.m_oBody.m_strName;
		l_tKeyDept.strPhone = l_oSetKeyDeptRequest.m_oBody.m_strPhone;
		l_tKeyDept.strAddress = l_oSetKeyDeptRequest.m_oBody.m_strAddress;
		l_tKeyDept.strLongitude = l_oSetKeyDeptRequest.m_oBody.m_strLongitude; 
		l_tKeyDept.strLatitude = l_oSetKeyDeptRequest.m_oBody.m_strLattitude;	

		if (AddKeyDept(l_strPhone, l_tKeyDept))
		{
			if (l_bUpdate)
			{

				SendSyncKeyDept(l_tKeyDept, ICC_MODIFY_KDYDEPT);
			}
			else
			{
				SendSyncKeyDept(l_tKeyDept, ICC_ADD_KEYDEPT);
			}
			l_oSetKeyDeptRespond.m_oBody.m_strResult = "0";//成功
			m_pIDBConn->Commit(l_strGuid);
		}
		else
		{
			m_pIDBConn->Rollback(l_strGuid);
		}
	}

	std::string l_strMessage = l_oSetKeyDeptRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiCheckDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CCheckKeyDeptRequest l_oCheckKeyDeptRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oCheckKeyDeptRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error SetKeyDeptRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	PROTOCOL::CCheckKeyDeptRespond l_oGetCheckKeyDeptRespond;
	GenRespHeader("check_if_key_dept_respond", l_oCheckKeyDeptRequest.m_oHeader, l_oGetCheckKeyDeptRespond.m_oHeader);

	CKeyDept l_tKeyDept;
	
	l_oGetCheckKeyDeptRespond.m_oBody.m_strResult = "0";
	l_oGetCheckKeyDeptRespond.m_oBody.m_strIsKeyDept = "0";
	std::string l_strPhoneNum = l_oCheckKeyDeptRequest.m_oBody.m_strPhone;
	do
	{
		//更新缓存，同步重点单位
		CKeyDept l_oKeyDept;
		if (GetKeyDeptByPhone(l_strPhoneNum, l_oKeyDept))
		{
			PROTOCOL::CCheckKeyDeptRespond::CBody::CData l_oData;
			l_oData.m_strType = l_oKeyDept.strType;
			l_oData.m_strName = l_oKeyDept.strName;
			l_oData.m_strPhone = l_oKeyDept.strPhone;
			l_oData.m_strAddress = l_oKeyDept.strAddress;
			l_oData.m_strLongitude = l_oKeyDept.strLongitude;
			l_oData.m_strLattitude = l_oKeyDept.strLatitude;
			l_oGetCheckKeyDeptRespond.m_oBody.m_vecData.push_back(l_oData);

			l_oGetCheckKeyDeptRespond.m_oBody.m_strIsKeyDept = "1";
			l_oGetCheckKeyDeptRespond.m_oBody.m_strResult = "0";
		}
	} while (0);
	l_oGetCheckKeyDeptRespond.m_oBody.m_strCount = m_pString->Format("%d", l_oGetCheckKeyDeptRespond.m_oBody.m_vecData.size());

	std::string l_strMessage = l_oGetCheckKeyDeptRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetAllKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetAllKeyDeptRequest l_oGetAllKeyDeptRequest;

	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oGetAllKeyDeptRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error SetKeyDeptRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetAllKeyDeptRespond l_oGetAllKeyDeptRespond;
	//GenRespHeader("get_all_key_dept_respond", l_oGetAllKeyDeptRequest.m_oHeader, l_oGetAllKeyDeptRespond.m_oHeader);
	l_oGetAllKeyDeptRespond.m_oHeader = l_oGetAllKeyDeptRequest.m_oHeader;

	unsigned int tmp_uiAllCount = 0;

	do
	{

	
		if (l_oGetAllKeyDeptRequest.m_oBody.m_strPageSize.empty())
		{
			l_oGetAllKeyDeptRespond.m_oHeader.m_strResult = "1";
			l_oGetAllKeyDeptRespond.m_oHeader.m_strMsg = "page size is empty";
			break;
		}

		if (l_oGetAllKeyDeptRequest.m_oBody.m_strPageIndex.empty())
		{
			l_oGetAllKeyDeptRespond.m_oHeader.m_strResult = "1";
			l_oGetAllKeyDeptRespond.m_oHeader.m_strMsg = "page index is empty";
			break;
		}


		std::map < std::string, std::string > l_mapKeyDept;
		GetAllKeyDept(l_mapKeyDept);
		ICC_LOG_INFO(m_pLog, "getkey dept page_size=%s,page_index=%s", l_oGetAllKeyDeptRequest.m_oBody.m_strPageSize.c_str(), l_oGetAllKeyDeptRequest.m_oBody.m_strPageIndex.c_str());

		unsigned int tmp_uiPageSize = m_pString->ToUInt(l_oGetAllKeyDeptRequest.m_oBody.m_strPageSize);
	
		unsigned int tmp_uiPageIndex = m_pString->ToUInt(l_oGetAllKeyDeptRequest.m_oBody.m_strPageIndex);
		int l_nCount = 0;
		int l_nIndex = 0;

	
		if (l_oGetAllKeyDeptRequest.m_oBody.m_strFilterKey.empty())
		{

			tmp_uiAllCount = l_mapKeyDept.size();

			l_oGetAllKeyDeptRespond.m_oBody.m_strAllCount = m_pString->Number(tmp_uiAllCount);

			for (auto iter = l_mapKeyDept.begin(); iter != l_mapKeyDept.end(); ++iter)
			{
				CKeyDept l_oKeyDept;
				l_oKeyDept.Parse(iter->second, m_pJsonFac->CreateJson());

				PROTOCOL::CGetAllKeyDeptRespond::CBody::CData l_oData;
				l_oData.m_strType = l_oKeyDept.strType;
				l_oData.m_strName = l_oKeyDept.strName;
				l_oData.m_strPhone = l_oKeyDept.strPhone;
				l_oData.m_strAddress = l_oKeyDept.strAddress;
				l_oData.m_strLongitude = l_oKeyDept.strLongitude;
				l_oData.m_strLattitude = l_oKeyDept.strLatitude;
				l_oGetAllKeyDeptRespond.m_oBody.m_vecData.push_back(l_oData);

			
				l_nCount++;
				if (tmp_uiPageSize == l_nCount)
				{
					++l_nIndex;

					if (l_nIndex == tmp_uiPageIndex)
					{
						l_oGetAllKeyDeptRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiAllCount);
						l_oGetAllKeyDeptRespond.m_oBody.m_strCount = std::to_string(l_oGetAllKeyDeptRespond.m_oBody.m_vecData.size());

						std::string l_strMessage = l_oGetAllKeyDeptRespond.ToString(m_pJsonFac->CreateJson());

						p_pNotify->Response(l_strMessage, true);
						ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
						return;
					}
					
					l_oGetAllKeyDeptRespond.m_oBody.m_vecData.clear();
					l_nCount = 0;
				}
			}
		}
		else
		{

			bool bFind = false;
			for (auto iter = l_mapKeyDept.begin(); iter != l_mapKeyDept.end(); ++iter)
			{
				CKeyDept l_oKeyDept;
				l_oKeyDept.Parse(iter->second, m_pJsonFac->CreateJson());

				if (l_oKeyDept.strName.find(l_oGetAllKeyDeptRequest.m_oBody.m_strFilterKey) != std::string::npos
					|| l_oKeyDept.strPhone.find(l_oGetAllKeyDeptRequest.m_oBody.m_strFilterKey) != std::string::npos
					|| l_oKeyDept.strAddress.find(l_oGetAllKeyDeptRequest.m_oBody.m_strFilterKey) != std::string::npos)
				{

					if (!bFind)
					{
						PROTOCOL::CGetAllKeyDeptRespond::CBody::CData l_oData;
						l_oData.m_strType = l_oKeyDept.strType;
						l_oData.m_strName = l_oKeyDept.strName;
						l_oData.m_strPhone = l_oKeyDept.strPhone;
						l_oData.m_strAddress = l_oKeyDept.strAddress;
						l_oData.m_strLongitude = l_oKeyDept.strLongitude;
						l_oData.m_strLattitude = l_oKeyDept.strLatitude;
						l_oGetAllKeyDeptRespond.m_oBody.m_vecData.push_back(l_oData);
					}
					
					++l_nCount;

					++tmp_uiAllCount;

					//达到行数
					if (tmp_uiPageSize == l_nCount)
					{
						l_nCount = 0;
						
						//是否是要查询的页面
						if (l_nIndex == tmp_uiPageIndex)
						{
							bFind = true;
						}
						else
						{
							//如果没找到当前页时，需要清除这个数据
							if (!bFind)
							{
								l_oGetAllKeyDeptRespond.m_oBody.m_vecData.clear();
							}
						}
					}
				}
			
			}
		}
	} while (0);

	l_oGetAllKeyDeptRespond.m_oBody.m_strAllCount = std::to_string(tmp_uiAllCount);
	l_oGetAllKeyDeptRespond.m_oBody.m_strCount = std::to_string(l_oGetAllKeyDeptRespond.m_oBody.m_vecData.size());

	std::string l_strMessage = l_oGetAllKeyDeptRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiDeleteKeyDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CDeleteKeyDeptRequest l_oDeleteKeyDeptRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oDeleteKeyDeptRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error SetKeyDeptRequest Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CDeleteKeyDeptRespond l_oDeleteKeyDeptRespond;
	GenRespHeader("delete_key_dept_respond", l_oDeleteKeyDeptRequest.m_oHeader, l_oDeleteKeyDeptRespond.m_oHeader);

	std::string l_strPhone = l_oDeleteKeyDeptRequest.m_oBody.m_strPhone;	
	if (ExistInCache(l_strPhone))
	{
		DataBase::SQLRequest l_oKeyDeptSQLReq;
		l_oKeyDeptSQLReq.sql_id = "update_icc_t_keydept";
		l_oKeyDeptSQLReq.set["is_delete"] = "true";
		l_oKeyDeptSQLReq.set["update_user"] = "icc_basedata_keydept";
		l_oKeyDeptSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

        l_oKeyDeptSQLReq.param["phone"] = l_strPhone;

		std::string l_strGuid = m_pIDBConn->BeginTransaction();
		if (!ExecSql(l_oKeyDeptSQLReq, l_strGuid))
		{
			m_pIDBConn->Rollback(l_strGuid);
			ICC_LOG_ERROR(m_pLog, "KeyDept Error SetKeyDeptError");
			l_oDeleteKeyDeptRespond.m_oBody.m_strResult = "1";//失败
		}

		CKeyDept l_tKeyDept;
		if (GetKeyDeptByPhone(l_strPhone, l_tKeyDept) && DeleteKeyDept(l_strPhone))
		{
			SendSyncKeyDept(l_tKeyDept, ICC_DELETE_KEYDEPT);
			l_oDeleteKeyDeptRespond.m_oBody.m_strResult = "0";//成功
			m_pIDBConn->Commit(l_strGuid);
		}
		else
		{
			m_pIDBConn->Rollback(l_strGuid);
		}
	}
	else
	{
		l_oDeleteKeyDeptRespond.m_oBody.m_strResult = "1";//失败
	}
	
	std::string l_strMessage = l_oDeleteKeyDeptRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SendSyncKeyDept(CKeyDept& p_tKeyDept, eSyncType p_eSyncType)
{
	if (p_tKeyDept.strType.empty())
	{
		return;
	}
	PROTOCOL::CKeyDeptSync l_oKeyDeptSync;

	l_oKeyDeptSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oKeyDeptSync.m_oHeader.m_strCmd = "key_dept_sync";
	l_oKeyDeptSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oKeyDeptSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oKeyDeptSync.m_oHeader.m_strRequestType = "1";//主题

	l_oKeyDeptSync.m_oBody.m_strSyncType = m_pString->Number(p_eSyncType);
	l_oKeyDeptSync.m_oBody.m_strType = p_tKeyDept.strType;
	l_oKeyDeptSync.m_oBody.m_strName = p_tKeyDept.strName;
	l_oKeyDeptSync.m_oBody.m_strPhone = p_tKeyDept.strPhone;
	l_oKeyDeptSync.m_oBody.m_strAddress = p_tKeyDept.strAddress;
	l_oKeyDeptSync.m_oBody.m_strLongitude = p_tKeyDept.strLongitude;
	l_oKeyDeptSync.m_oBody.m_strLattitude = p_tKeyDept.strLatitude;

	std::string l_strMessage = l_oKeyDeptSync.ToString(m_pJsonFac->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "KeyDept Send Sync Info:[%s]", l_strMessage.c_str());
}	

bool CBusinessImpl::LoadKeyDeptFromDB()
{
	m_pRedisClient->Del(KeyDept);

	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "select_icc_t_keydept";
	l_oKeyDeptSQLReq.param["is_delete"] = "false";
	ICC_LOG_DEBUG(m_pLog, "LoadKeyDeptInfo Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oKeyDeptSQLReq, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = 0;
	while (l_result->Next())
	{
		CKeyDept l_tKeyDept;
		l_tKeyDept.strID = l_result->GetValue("guid");
		l_tKeyDept.strType = l_result->GetValue("type");
		l_tKeyDept.strName = l_result->GetValue("name");
		l_tKeyDept.strPhone = l_result->GetValue("phone");
		l_tKeyDept.strAddress = l_result->GetValue("address");
		l_tKeyDept.strLongitude = l_result->GetValue("longitude");
		l_tKeyDept.strLatitude = l_result->GetValue("latitude");

		if (!AddKeyDept(l_tKeyDept.strPhone, l_tKeyDept))
		{
			--l_iSize;
			ICC_LOG_ERROR(m_pLog, "Load KeyDept To Redis Failed phone[%s]", l_tKeyDept.strPhone.c_str());
			return false;
		}
		++l_iSize;
	}
	ICC_LOG_DEBUG(m_pLog, "LoadKeyDeptInfo Success size[%d]", l_iSize);
	return true;
}

void CBusinessImpl::GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = "icc_server_basedata_KeyDept";
	p_pRespHeader.m_strSubsystemID = "icc_server_basedata_KeyDept";
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
		ICC_LOG_ERROR(m_pLog, "KeyDept Error:[%s]", "l_pRSet is null");
		return false;
	}
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "KeyDept Error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool ICC::CBusinessImpl::AddKeyDept(std::string p_strPhone, const CKeyDept& p_oKeyDept)
{
	if (p_strPhone.empty())
	{
		return false;
	}

	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	std::string l_strKeyDept = p_oKeyDept.ToJson(l_pJson);
	return m_pRedisClient->HSet(KeyDept, p_strPhone, l_strKeyDept);
}

bool ICC::CBusinessImpl::DeleteKeyDept(std::string p_strPhone)
{
	return m_pRedisClient->HDel(KeyDept, p_strPhone);
}
	
bool ICC::CBusinessImpl::GetAllKeyDept(std::map<std::string, std::string>& p_mapKeyDept)
{
	int l_iNext = 0;

	while ((l_iNext = m_pRedisClient->HScan(KeyDept, l_iNext, p_mapKeyDept)) > 0);

	return l_iNext == 0;
}

bool ICC::CBusinessImpl::GetKeyDeptByPhone(std::string p_strPhone, CKeyDept& p_OKeyDept)
{
	std::string l_strKeyDept;
	if (!m_pRedisClient->HGet(KeyDept, p_strPhone, l_strKeyDept))
	{
		return false;
	}
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	return p_OKeyDept.Parse(l_strKeyDept, l_pJson);
}

std::string ICC::CBusinessImpl::GetIDByPhone(std::string p_strPhone)
{
	std::string l_strKeyDept;
	if (!m_pRedisClient->HGet(KeyDept, p_strPhone, l_strKeyDept))
	{
		return "";
	}
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	CKeyDept l_oKeyDept;
	l_oKeyDept.Parse(l_strKeyDept, l_pJson);
	return l_oKeyDept.strID;
}

bool ICC::CBusinessImpl::ExistInCache(std::string p_strPhone)
{
	return m_pRedisClient->HExists(KeyDept, p_strPhone);
}

