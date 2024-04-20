#include "Boost.h"
#include "AdminInfo.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define ADMIN ("Admin")
#define SYSTEMID ("icc_server")
#define SUBSYSTEMID ("icc_server_basedata_Admin")

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

	//等待数据库连接成功后再进行后续操作
	while (!LoadAdminInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.admin failed to load admin info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	/*if (LoadAdminInfo())
	{
	ICC_LOG_INFO(m_pLog, "admin start success");
	}
	else
	{
	ICC_LOG_ERROR(m_pLog, "admin start failed");
	}*/
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_can_admin_dept_request", OnNotifiGetAllCanAdminDept);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_can_admin_dept_request", OnNotifiSetAdmin4anDept);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_can_admin_dept_request", OnNotifiDelDeptCanAdmin);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_can_admin_dept_request", OnNotifiGetCanAdminDept);

	ICC_LOG_INFO(m_pLog, "plugin basedata.admin start success");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "admin stop success");
}

void CBusinessImpl::OnDestroy()
{

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

//初始化缓存
bool CBusinessImpl::LoadAdminInfo()
{
	m_pRedisClient->Del(ADMIN);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_can_admin_dept";

	ICC_LOG_DEBUG(m_pLog, "LodingAdminInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		CAdminInfo l_oAdminInfo;
		l_oAdminInfo.m_strGuid = l_result->GetValue("guid");
		l_oAdminInfo.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oAdminInfo.m_strBindDeptCode = l_result->GetValue("bind_dept_code");
		l_oAdminInfo.m_strBindParentDeptCode = l_result->GetValue("bind_parent_dept_code");
		l_oAdminInfo.m_strBelongDeptCode = l_result->GetValue("belong_dept_code");

		std::string l_strVal = l_oAdminInfo.ToJson(m_pJsonFty->CreateJson());
		m_pRedisClient->HSet(ADMIN, l_oAdminInfo.m_strGuid, l_strVal);
	}

	ICC_LOG_DEBUG(m_pLog, "LodingAdminInfo Success,Admin Size[%d]", l_result->RecordSize());
	return true;
}

//获取所有单位下的所有可管辖单位信息
void CBusinessImpl::OnNotifiGetAllCanAdminDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCanAdminDept request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAllCanAdminDept]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	PROTOCOL::CCanAdminDept response;
	GenRespHeader("get_all_can_admin_dept_respond", request.m_oHeader, response.m_oHeader);

	std::map<std::string, std::string> l_mapAdminInfo;
	m_pRedisClient->HGetAll(ADMIN, l_mapAdminInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetAllCanAdminDept] HGetAll complete! msgid = %s", request.m_oHeader.m_strMsgid.c_str());

	if (l_mapAdminInfo.size() == 0)
	{
		response.m_oBody.m_strCount = "0";
		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		std::map<std::string, std::string>::iterator it;
		int count = 0;
		for (it = l_mapAdminInfo.begin(); it != l_mapAdminInfo.end(); it++)
		{
			CAdminInfo l_oAdmin;
			l_oAdmin.Parse(it->second, m_pJsonFty->CreateJson());

			PROTOCOL::CCanAdminDept::CAdmin Admin;
			Admin.m_strGuid = l_oAdmin.m_strGuid;
			Admin.m_strParentGuid = l_oAdmin.m_strParentGuid;
			Admin.m_strBindDeptCode = l_oAdmin.m_strBindDeptCode;
			Admin.m_strBindParentDeptCode = l_oAdmin.m_strBindParentDeptCode;
			Admin.m_BelongDeptCode = l_oAdmin.m_strBelongDeptCode;

			response.m_oBody.m_vecData.push_back(Admin);
			++count;
			if (count == MAX_COUNT)
			{
				count = 0;
                response.m_oBody.m_strCount = m_pString->Number(l_mapAdminInfo.size());
				std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
				response.m_oBody.m_vecData.clear();

				p_pNotify->Response(l_strMessage, true);
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}
		}
		if (count != 0)
		{
            response.m_oBody.m_strCount = m_pString->Number(l_mapAdminInfo.size());
			std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());

			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
	ICC_LOG_INFO(m_pLog, "Admin Info Msg Sended [total:%d]", l_mapAdminInfo.size());
}

//设置某一单位下某一个可管辖单位信息
void CBusinessImpl::OnNotifiSetAdmin4anDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CCanAdminDept request;
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetAdmin4anDept Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strSyncType;
	DataBase::SQLRequest l_oSQLReq;
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	if (m_pRedisClient->HExists(ADMIN, request.m_oBody.m_data.m_strGuid))
	{
		//更新操作
		l_strSyncType = "2";
		l_oSQLReq.sql_id = "update_icc_t_can_admin_dept";
		l_oSQLReq.param["guid"] = request.m_oBody.m_data.m_strGuid;
		l_oSQLReq.set["parent_guid"] = request.m_oBody.m_data.m_strParentGuid;
		l_oSQLReq.set["bind_dept_code"] = request.m_oBody.m_data.m_strBindDeptCode;
		l_oSQLReq.set["bind_parent_dept_code"] = request.m_oBody.m_data.m_strBindParentDeptCode;
		l_oSQLReq.set["belong_dept_code"] = request.m_oBody.m_data.m_BelongDeptCode;
		l_oSQLReq.set["update_user"] = "BaseData Server";
		l_oSQLReq.set["update_time"] = l_strCurrentTime;
	}
	else
	{
		//新增操作
		l_strSyncType = "1";
		l_oSQLReq.sql_id = "insert_icc_t_can_admin_dept";
		l_oSQLReq.param["guid"] = request.m_oBody.m_data.m_strGuid;
		l_oSQLReq.param["parent_guid"] = request.m_oBody.m_data.m_strParentGuid;
		l_oSQLReq.param["bind_dept_code"] = request.m_oBody.m_data.m_strBindDeptCode;
		l_oSQLReq.param["bind_parent_dept_code"] = request.m_oBody.m_data.m_strBindParentDeptCode;
		l_oSQLReq.param["belong_dept_code"] = request.m_oBody.m_data.m_BelongDeptCode;
		l_oSQLReq.param["create_user"] = "BaseData Server";
		l_oSQLReq.param["create_time"] = l_strCurrentTime;
		l_oSQLReq.param["update_user"] = "BaseData Server";
		l_oSQLReq.param["update_time"] = l_strCurrentTime;
	}

	//根据数据库操作结果构造响应消息
	PROTOCOL::CCanAdminDept response;
	GenRespHeader("set_can_admin_dept_respond", request.m_oHeader, response.m_oHeader);
	response.m_oBody.m_strResult = "1";//默认失败
	//执行数据库操作
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLReq, false, l_strGuid);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		m_pDBConn->Rollback(l_strGuid);
		ICC_LOG_ERROR(m_pLog, "set_can_Admin_dept_respond Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		//更新缓存
		CAdminInfo l_oAdminInfo;
		l_oAdminInfo.m_strGuid = request.m_oBody.m_data.m_strGuid;
		l_oAdminInfo.m_strParentGuid = request.m_oBody.m_data.m_strParentGuid;
		l_oAdminInfo.m_strBindDeptCode = request.m_oBody.m_data.m_BelongDeptCode;
		l_oAdminInfo.m_strBindParentDeptCode = request.m_oBody.m_data.m_strBindParentDeptCode;
		l_oAdminInfo.m_strBindDeptCode = request.m_oBody.m_data.m_strBindDeptCode;

		std::string l_strVal = l_oAdminInfo.ToJson(m_pJsonFty->CreateJson());
		if (m_pRedisClient->HSet(ADMIN, l_oAdminInfo.m_strGuid, l_strVal))
		{
			m_pDBConn->Commit(l_strGuid);
			response.m_oBody.m_strResult = "0";//成功
			ICC_LOG_INFO(m_pLog, "set_can_Admin_dept_respond Success:[%s]", p_pNotify->GetMessages().c_str());

			//发送同步信息
			PROTOCOL::CCanAdminDeptSync CCanAdminDeptSync;
			GenRespHeader("can_dispatch_dept_sync", request.m_oHeader, CCanAdminDeptSync.m_oHeader);
			CCanAdminDeptSync.m_oHeader.m_strRequest = "topic_basedata_sync";
			CCanAdminDeptSync.m_oBody.m_strSyncType = l_strSyncType;
			CCanAdminDeptSync.m_oBody.m_strGuid = request.m_oBody.m_data.m_strGuid;
			CCanAdminDeptSync.m_oBody.m_strBindParentDeptCode = request.m_oBody.m_data.m_strParentGuid;
			CCanAdminDeptSync.m_oBody.m_strBelongDeptCode = request.m_oBody.m_data.m_BelongDeptCode;
			CCanAdminDeptSync.m_oBody.m_strBindDeptCode = request.m_oBody.m_data.m_strBindParentDeptCode;
			CCanAdminDeptSync.m_oBody.m_strBindDeptCode = request.m_oBody.m_data.m_strBindDeptCode;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMessage = CCanAdminDeptSync.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
		else
		{
			m_pDBConn->Rollback(l_strGuid);
			ICC_LOG_ERROR(m_pLog, "Set Redis AdminInfo Failed guid[%s]", request.m_oBody.m_data.m_strGuid.c_str());
		}
	}

	std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//删除某一单位下某一个可管辖单位信息
void CBusinessImpl::OnNotifiDelDeptCanAdmin(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJsonPtr = m_pJsonFty->CreateJson();
	PROTOCOL::CCanAdminDept request;
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJsonPtr))
	{
		ICC_LOG_ERROR(m_pLog, "DelDeptCanAdmin Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CCanAdminDept response;
	GenRespHeader("delete_can_admin_dept_respond", request.m_oHeader, response.m_oHeader);
	response.m_oBody.m_strResult = "1";//失败
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	if (m_pRedisClient->HExists(ADMIN, response.m_oBody.m_data.m_strGuid))
	{
		//缓存中存在所请求的GUID单位信息，则将数据从数据库中删除
		DataBase::SQLRequest l_oDeleteFuncSQLReq;
		l_oDeleteFuncSQLReq.sql_id = "delete_icc_t_can_admin_dept";
		l_oDeleteFuncSQLReq.param["guid"] = request.m_oBody.m_data.m_strGuid;

		//执行数据库操作	
		std::string l_strTransactionGuid = m_pDBConn->BeginTransaction();
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteFuncSQLReq, false, l_strTransactionGuid);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			m_pDBConn->Rollback(l_strTransactionGuid);
			ICC_LOG_ERROR(m_pLog, "DeleteFunc Failed: [%s]", l_pRSet->GetErrorMsg().c_str());
		}
		else
		{
			std::string l_strAdminInfo;
			m_pRedisClient->HGet(ADMIN, request.m_oBody.m_data.m_strGuid, l_strAdminInfo);
			CAdminInfo l_oDeletedAdminInfo;
			l_oDeletedAdminInfo.Parse(l_strAdminInfo, m_pJsonFty->CreateJson());
			if (m_pRedisClient->HDel(ADMIN, request.m_oBody.m_data.m_strGuid))
			{
				//同步
				//发送同步信息
				m_pDBConn->Commit(l_strTransactionGuid);
				response.m_oBody.m_strResult = "0";//成功					

				//同步
				PROTOCOL::CCanAdminDeptSync CCanAdminDeptSync;
				GenRespHeader("can_dispatch_dept_sync", request.m_oHeader, CCanAdminDeptSync.m_oHeader);
				CCanAdminDeptSync.m_oHeader.m_strRequest = "topic_basedata_sync";
				CCanAdminDeptSync.m_oBody.m_strSyncType = "3";
				CCanAdminDeptSync.m_oBody.m_strGuid = request.m_oBody.m_data.m_strGuid;
				CCanAdminDeptSync.m_oBody.m_strBindParentDeptCode = l_oDeletedAdminInfo.m_strBindParentDeptCode;
				CCanAdminDeptSync.m_oBody.m_strBelongDeptCode = l_oDeletedAdminInfo.m_strBelongDeptCode;
				CCanAdminDeptSync.m_oBody.m_strBindDeptCode = l_oDeletedAdminInfo.m_strBindDeptCode;
				CCanAdminDeptSync.m_oBody.m_strParentGuid = l_oDeletedAdminInfo.m_strParentGuid;

				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMessage = CCanAdminDeptSync.ToString(l_pIJson);
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}
		}
		ICC_LOG_DEBUG(m_pLog, "DeptCanAdmin Success Func Cache Updated");
	}

	std::string l_strMessage;
	l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//获取某一单位下的所有可管辖单位信息
void CBusinessImpl::OnNotifiGetCanAdminDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCanAdminDept request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetCanAdminDept]Parse request error.");
		return;
	}

	PROTOCOL::CCanAdminDept response;
	GenRespHeader("get_can_admin_dept_respond", request.m_oHeader, response.m_oHeader);
	std::map<std::string, std::string> l_mapAdminInfo;
	m_pRedisClient->HGetAll(ADMIN, l_mapAdminInfo);
	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetCanAdminDept] HGetAll complete! msgid = %s", request.m_oHeader.m_strMsgid.c_str());
	if (l_mapAdminInfo.size() == 0)
	{
		response.m_oBody.m_strCount = "0";
		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		mapAdminInfo::iterator it;
		int count = 0;
		for (auto it = l_mapAdminInfo.begin(); it != l_mapAdminInfo.end(); it++)
		{
			CAdminInfo l_oAdminInfo;
			l_oAdminInfo.Parse(it->second, m_pJsonFty->CreateJson());
			if (!l_oAdminInfo.m_strBelongDeptCode.compare(request.m_oBody.m_data.m_BelongDeptCode))
			{
				PROTOCOL::CCanAdminDept::CAdmin Admin;
				Admin.m_strGuid = l_oAdminInfo.m_strGuid;
				Admin.m_strParentGuid = l_oAdminInfo.m_strParentGuid;
				Admin.m_strBindDeptCode = l_oAdminInfo.m_strBindDeptCode;
				Admin.m_strBindParentDeptCode = l_oAdminInfo.m_strBindParentDeptCode;
				Admin.m_BelongDeptCode = l_oAdminInfo.m_strBelongDeptCode;
				response.m_oBody.m_vecData.push_back(Admin);

				++count;
				if (count == MAX_COUNT)
				{
					count = 0;
					response.m_oBody.m_strCount = m_pString->Number(MAX_COUNT);
					std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
					response.m_oBody.m_vecData.clear();
					p_pNotify->Response(l_strMessage, true);
					ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
				}
			}
		}
		if (count != 0)
		{
			response.m_oBody.m_strCount = m_pString->Number(count);
			std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
	ICC_LOG_INFO(m_pLog, "Admin Info Msg Sended [total:%d]", l_mapAdminInfo.size());

}