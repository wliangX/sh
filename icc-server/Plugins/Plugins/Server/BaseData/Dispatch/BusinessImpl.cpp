#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define CanDispatchDept ("CanDisPatchDept")

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
    //m_lCanDisLock = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
    //m_mapDispatchInfoLock = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
    m_vDBConnLock = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_isConnectDB = false;
	m_isConnectMQ = false;
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	while (!LoadDispatchInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.dipatch failed to load dipatch info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_can_dispatch_dept_request", OnNotifiGetAllCanDispatchDept);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_can_dispatch_dept_request", OnNotifiSetDispatch4anDept);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_can_dispatch_dept_request", OnNotifiDelDeptCanDispatch);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_can_dispatch_dept_request", OnNotifiGetAllCanDispatch4aDept);

	ICC_LOG_INFO(m_pLog, "plugin basedata.dipatch start success");
	
	////等待数据库连接成功后再进行后续操作
	//if (LoadDispatchInfo())
	//{
	//	ICC_LOG_INFO(m_pLog, "dipatch start success");
	//}
	//else
	//{
	//	ICC_LOG_ERROR(m_pLog, "dipatch start failed");
	//}

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "dipatch stop success");
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

bool CBusinessImpl::LoadDispatchInfo()
{
	m_pRedisClient->Del(CanDispatchDept);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = SELECT_ICC_T_CAN_DISPATCH_DEPT;

	ICC_LOG_DEBUG(m_pLog, "LodingDispatchInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());		
		return false;
	}
	while (l_result->Next())
	{
		CCanDispatchInfo dispatch;
		dispatch.m_strGuid = l_result->GetValue("guid");
		dispatch.m_strParentGuid = l_result->GetValue("parent_guid");
		dispatch.m_strBindDeptCode = l_result->GetValue("bind_dept_code");
		dispatch.m_strBindParentDeptCode = l_result->GetValue("bind_parent_dept_code");
		dispatch.m_BelongDeptCode = l_result->GetValue("belong_dept_code");
		dispatch.m_strCreateUser = l_result->GetValue("create_user");
		dispatch.m_strCreateTime = l_result->GetValue("create_time");
		dispatch.m_strUpdateUser = l_result->GetValue("update_user");
		dispatch.m_strUpdateTime = l_result->GetValue("update_time");
		
		std::string l_strVal = dispatch.ToJson(m_pJsonFty->CreateJson());
		m_pRedisClient->HSet(CanDispatchDept, dispatch.m_strGuid, l_strVal);
		//m_mapCanDispatchDept.insert(std::make_pair(dispatch.m_strGuid, dispatch));
	}
	
	ICC_LOG_DEBUG(m_pLog, "LodingDispatchInfo Success,Dispatch Size[%d]", l_result->RecordSize());
	 return true;
}

//获取所有单位下的所有可调派单位信息
void CBusinessImpl::OnNotifiGetAllCanDispatchDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCanDispatchDept request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAllCanDispatchDept]Parse request error. msg = %s", l_strRecvMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetAllCanDispatchDept [%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CCanDispatchDept response;
	GenRespHeader(GET_ALL_CAN_DISPATCH_DEPT_RESPOND, request.m_oHeader, response.m_oHeader);
	//Lock::AutoLock lock(m_mapDispatchInfoLock);
	CanDispatchInfoMap l_oCanDispatchInfo;	
	int iSize = GetAllCanDispatchDept(l_oCanDispatchInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetAllCanDispatchDept] HGetAll complete! msgid = %s", request.m_oHeader.m_strMsgid.c_str());

	if (iSize == 0)
	{
		response.m_oBody.m_strCount = "0";
		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{		
		int count = 0;
		for (auto it = l_oCanDispatchInfo.begin(); it != l_oCanDispatchInfo.end(); it++)
		{
			PROTOCOL::CCanDispatchDept::CDispatch dispatch;
			dispatch.m_strGuid = it->second.m_strGuid;
			dispatch.m_strParentGuid = it->second.m_strParentGuid;
			dispatch.m_strBindDeptCode = it->second.m_strBindDeptCode;
			dispatch.m_strBindParentDeptCode = it->second.m_strBindParentDeptCode;
			dispatch.m_BelongDeptCode = it->second.m_BelongDeptCode;
			response.m_oBody.m_vecData.push_back(dispatch);

			++count;
			if (count == MAX_COUNT)
			{
				count = 0;
                response.m_oBody.m_strCount = m_pString->Number(l_oCanDispatchInfo.size());
				std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
				response.m_oBody.m_vecData.clear();

				p_pNotify->Response(l_strMessage, true);
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}
		}

		response.m_oBody.m_strCount = m_pString->Number(l_oCanDispatchInfo.size());
		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		
	}
	ICC_LOG_INFO(m_pLog, "Dispatch Info Msg Sended [total:%d]", l_oCanDispatchInfo.size());
}

//设置某一单位下某一个可调派单位信息
void CBusinessImpl::OnNotifiSetDispatch4anDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CCanDispatchDept request;
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetDispatch4anDept Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	mapCanDispatchDept::iterator it;

	std::string l_strSyncType;
	DataBase::SQLRequest l_oSQLReq;
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	if (m_pRedisClient->HExists(CanDispatchDept, request.m_oBody.m_data.m_strGuid))
	{
		l_strSyncType = "2";
		l_oSQLReq.sql_id = UPDATE_ICC_T_CAN_DISPATCH_DEPT;
		l_oSQLReq.param["guid"] = it->second.m_strGuid;
		l_oSQLReq.set["parent_guid"] = request.m_oBody.m_data.m_strParentGuid;
		l_oSQLReq.set["bind_dept_code"] = request.m_oBody.m_data.m_strBindDeptCode;
		l_oSQLReq.set["bind_parent_dept_code"] = request.m_oBody.m_data.m_strBindParentDeptCode;
		l_oSQLReq.set["belong_dept_code"] = request.m_oBody.m_data.m_BelongDeptCode;
		l_oSQLReq.set["update_user"] = "BaseData Server";
		l_oSQLReq.set["update_time"] = l_strCurrentTime;
	}
	else
	{
		l_strSyncType = "1";
		l_oSQLReq.sql_id = INSERT_ICC_T_CAN_DISPATCH_DEPT;
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

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLReq);	
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	//根据数据库操作结果构造响应消息
	PROTOCOL::CCanDispatchDept response;
	GenRespHeader(SET_CAN_DISPATCH_DEPT_RESPOND, request.m_oHeader, response.m_oHeader);
	
	if (!l_pRSet->IsValid())
	{		
		response.m_oBody.m_strResult = "1";//失败
		ICC_LOG_ERROR(m_pLog, "set_can_dispatch_dept_respond Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		response.m_oBody.m_strResult = "0";//成功
		ICC_LOG_INFO(m_pLog, "set_can_dispatch_dept_respond Success:[%s]", p_pNotify->GetMessages().c_str());
		//更新缓存
		{
			//Lock::AutoLock lock(m_lCanDisLock);
			CCanDispatchInfo info;
			info.m_strGuid = request.m_oBody.m_data.m_strGuid;
			info.m_strParentGuid = request.m_oBody.m_data.m_strParentGuid;
            info.m_strBindDeptCode = request.m_oBody.m_data.m_strBindDeptCode;
			info.m_strBindParentDeptCode = request.m_oBody.m_data.m_strBindParentDeptCode;
			info.m_BelongDeptCode = request.m_oBody.m_data.m_BelongDeptCode;

			std::string l_strVal = info.ToJson(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(CanDispatchDept, info.m_strGuid, l_strVal);
			//m_mapCanDispatchDept[info.m_strGuid] = info;

			//同步信息
			PROTOCOL::CCanDispatchDept syncmsg = request;
			GenRespHeader(CAN_DISPATCH_DEPT_SYNC, request.m_oHeader, syncmsg.m_oHeader);
			syncmsg.m_oHeader.m_strRequest = "topic_basedata_sync";
			syncmsg.m_oHeader.m_strRequestType = "1";
			std::string strSyncMsg = syncmsg.Sync(m_pJsonFty->CreateJson(), l_strSyncType);
			if (!strSyncMsg.empty())
			{
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strSyncMsg));
				ICC_LOG_INFO(m_pLog, "SyntDispatch success:[%s]", strSyncMsg.c_str());
			}
		}
	}

	std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

//删除某一单位下某一个可调派单位信息
void CBusinessImpl::OnNotifiDelDeptCanDispatch(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJsonPtr = m_pJsonFty->CreateJson();
	PROTOCOL::CCanDispatchDept request;
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJsonPtr))
	{
		ICC_LOG_ERROR(m_pLog, "DelDeptCanDispatch Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CCanDispatchDept response;
	GenRespHeader(DELETE_CAN_DISPATCH_DEPT_RESPOND, request.m_oHeader, response.m_oHeader);
	if (m_pRedisClient->HExists(CanDispatchDept, request.m_oBody.m_data.m_strGuid))
	{
		//缓存中存在所请求的GUID单位信息，则将数据从数据库中删除
		DataBase::SQLRequest l_oDeleteFuncSQLReq;
		l_oDeleteFuncSQLReq.sql_id = DELETE_ICC_T_CAN_DISPATCH_DEPT;
		l_oDeleteFuncSQLReq.param["guid"] = request.m_oBody.m_data.m_strGuid;

		//执行数据库操作
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteFuncSQLReq);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

		//根据数据库操作结果构造响应消息		
		if (!l_pRSet->IsValid())
		{
			response.m_oBody.m_strResult = "1";//失败
			ICC_LOG_ERROR(m_pLog, "DeleteFunc Failed: [%s]", l_pRSet->GetErrorMsg().c_str());
		}
		else
		{
			response.m_oBody.m_strResult = "0";//成功
			//更新缓存		
			PROTOCOL::CCanDispatchDept syncmsg = request;
			GenRespHeader(CAN_DISPATCH_DEPT_SYNC, request.m_oHeader, syncmsg.m_oHeader);
			syncmsg.m_oHeader.m_strRequest = "topic_basedata_sync";
			syncmsg.m_oHeader.m_strRequestType = "1";
			std::string strSyncMsg = syncmsg.Sync(m_pJsonFty->CreateJson(), "3");
			if (!strSyncMsg.empty())
			{
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strSyncMsg));
				ICC_LOG_INFO(m_pLog, "SyntDispatch success:[%s]", strSyncMsg.c_str());
			}

			m_pRedisClient->HDel(CanDispatchDept, request.m_oBody.m_data.m_strGuid);
			ICC_LOG_DEBUG(m_pLog, "DeptCanDispatch Success Func Cache Updated: [%s]", p_pNotify->GetMessages().c_str());
		}

		std::string l_strMessage;
		l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		response.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "No func ,Delete Faild");
		//返回SetDeptRequest的结果信息
		std::string l_strMessage(response.ToStringResponse(m_pJsonFty->CreateJson()));
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

//获取某一单位下的所有可调派单位信息
void CBusinessImpl::OnNotifiGetAllCanDispatch4aDept(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCanDispatchDept request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAllCanDispatch4aDept]Parse request error. msg = %s", l_strRecvMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetAllCanDispatch4aDept [%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CCanDispatchDept response;
	GenRespHeader(GET_CAN_DISPATCH_DEPT_RESPOND, request.m_oHeader, response.m_oHeader);
	CanDispatchInfoMap l_oCanDispatchInfo;	

	int iSize = GetAllCanDispatchDept(l_oCanDispatchInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetAllCanDispatch4aDept] HGetAll complete! msgid = %s", request.m_oHeader.m_strMsgid.c_str());

	if (iSize == 0)
	{
		response.m_oBody.m_strCount = "0";
		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		int count = 0;
		for (auto it = l_oCanDispatchInfo.begin(); it != l_oCanDispatchInfo.end(); it++)
		{
			if (!it->second.m_BelongDeptCode.compare(request.m_oBody.m_data.m_BelongDeptCode))
			{
				PROTOCOL::CCanDispatchDept::CDispatch dispatch;
				dispatch.m_strGuid = it->second.m_strGuid;
				dispatch.m_strParentGuid = it->second.m_strParentGuid;
				dispatch.m_strBindDeptCode = it->second.m_strBindDeptCode;
				dispatch.m_strBindParentDeptCode = it->second.m_strBindParentDeptCode;
				dispatch.m_BelongDeptCode = it->second.m_BelongDeptCode;
				response.m_oBody.m_vecData.push_back(dispatch);

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
			std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());

			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
	ICC_LOG_INFO(m_pLog, "Dispatch Info Msg Sended [total:%d]", l_oCanDispatchInfo.size());
}

//返回获取到的可调派单位数量
int ICC::CBusinessImpl::GetAllCanDispatchDept(CanDispatchInfoMap& p_mapCanDispatchInfo)
{
	std::map<std::string, std::string> l_mapCanDispatchDept;
	int l_iNext = 0;
	while ((l_iNext = m_pRedisClient->HScan(CanDispatchDept, l_iNext, l_mapCanDispatchDept)) > 0);
		
	CCanDispatchInfo l_oCanDispatchInfo;
	for (auto it : l_mapCanDispatchDept)
	{
		l_oCanDispatchInfo.Parse(it.second, m_pJsonFty->CreateJson());		
		p_mapCanDispatchInfo[l_oCanDispatchInfo.m_strGuid] = l_oCanDispatchInfo;
	}
	return p_mapCanDispatchInfo.size();
}
