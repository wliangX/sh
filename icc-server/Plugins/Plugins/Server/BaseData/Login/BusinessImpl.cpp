#include "Boost.h"
#include "BusinessImpl.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
using namespace std;
using namespace boost::archive::iterators;

#define LOAD_WAITTIME 1000
using namespace std;

#define SYSTEMID	"icc_server"
#define SUBSYSTEMID "icc_server_login"
#define RESULT_SUCCESS "0"
#define RESULT_FAILED  "1"
#define REPEAT_LOGIN  "2"
#define IPAUTH_FAILED  "3"
#define CLIENT_REGISTER_INFO "ClientRegisterInfo"
#define WEB_LOGIN_INFO "WebLoginInfo"
#define MQCONNECT_INFO "MQConnectInfo"
#define ALARMONLINE_LIST "AlarmOnlineList"
#define DBONLINE_LIST "DBOnlineList"
#define AUTH_USER_INFO "UserInfo"
#define CONFIG_INFO "ConfigInfo"
#define IP_LENGTH      13
#define BIND_INFO  "BindInfo"
#define WEB_LOGIN_INFO "WebLoginInfo"

#define TIMER_CMD_NAME "login_timer"

#define ASSIGN_SEAT_NOS 1
#define ASSIGN_DEPT_CODE 2
#define ASSIGN_DISTRICT_CODE 3

#define Param_INFO "ParamInfo"

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pJsonFac = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_bAlarmFirst = false;
	m_bDbFirst = false;
	m_bIpAuthFlag = false;

	m_bStartedTimerFlag = false;
	m_uiTimeOutTime = 30 * 3; //默认90s
	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

const unsigned int REDELETE_COUNT = 5;
void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/Login/MultiClient", "");
	if (l_strIsUsing == "1")
	{
		m_strClientName = m_pConfig->GetValue("ICC/Plugin/Login/MultiClientName", "");
		ICC_LOG_DEBUG(m_pLog, "MultiClient Start[%s]", m_strClientName.c_str());
	}
	std::string l_strIsIpAuth = m_pConfig->GetValue("ICC/Plugin/Login/IPAuth", "");
	if (l_strIsIpAuth == "1")
	{
		m_bIpAuthFlag = true;
	}

	m_strHeartBeatTime = m_pConfig->GetValue("ICC/Plugin/Login/HeartbeatTime", "30");

	m_uiTimeOutTime = m_pString->ToUInt(m_strHeartBeatTime) * 3;
	/*
	int iRecount = 0;
	while (iRecount <= REDELETE_COUNT)
	{
		int iRs = m_pRedisClient->Del(CLIENT_REGISTER_INFO);
		if (iRs >= 0)
		{
			break;
		}
		iRecount++;
		ICC_LOG_ERROR(m_pLog, "delete  CLIENT_REGISTER_INFO fialed!!! recount = %d", iRecount);
		m_pHelpTool->Sleep(100);
	}
	
	m_pRedisClient->Del(WEB_LOGIN_INFO);//删除注册信息的同时，也要删除web的登录缓存，保持两边数据的一致
	*/

	/*while (!LoadUserInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.login failed to load user info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	ICC_LOG_DEBUG(m_pLog, "load user info success, heartbeat time: %s", m_strHeartBeatTime.c_str());

	while (!InitConfigCache())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.login failed to load config info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	ICC_LOG_DEBUG(m_pLog, "load config info success");


	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "ConnectionInfo", OnCNotifiConnect);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "RemoveInfo", OnCNotifiDisConnect);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_client_register_request", OnCNotifiClientRegisterRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_client_register_request", OnCNotifiGetClientInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_client_onoff_request", OnCNotifiGetOnOffInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_agent_state_sync", OnCNotifiSetInOutInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_client_inout_request", OnCNotifiGetInOutInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_client_register_request", OnCNotifiDeleteClientRegisterRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "check_user_request", OnCNotifiCheckUserRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_user_request", OnCNotifiGetUserRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_user_request", OnCNotifiSetUserRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_user_request", OnCNotifiDeleteUserRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "load_config_request", OnNotifiLoadConfigRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "upload_config_request", OnNotifiUploadConfigRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_config_request", OnGetAllConfigRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_system_param_request", OnNotifiUpdateParamRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_system_param_request", OnNotifiGetAllParamRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_agent_state_sync", OnCNotifiAcdAgentStateRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_ip_seat_request", OnCNotifiSetBindIpSeat);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_ip_seat_request", OnCNotifiDeleteBindIpSeat);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_ip_seat_request", OnCNotifiGetBindIpSeat);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_server_lock", OnNotifiDoubleServerSync);	

    //增加客户端心跳处理
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_client_heartbeat", OnNotifClientHeartBeat);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, m_pString->ToUInt(m_strHeartBeatTime), 60); //60s后开始执行

	//查询空闲席位
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_idle_seat", OnQueryFreeSeatRequest);

	//查询当前用户绑定的席位
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_seatno_by_user", OnQueryPreUserBindSeat);

	if (!GetParamInfo())
	{
		ICC_LOG_ERROR(m_pLog, "When Login start,GetParamInfo failed!!!");
	}
	
	ICC_LOG_DEBUG(m_pLog, "plugin basedata.login start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	//m_pRedisClient->Disconnect();
	ICC_LOG_DEBUG(m_pLog, "login stop success");
}

std::string CBusinessImpl::_QueryGuidFromInOutByClientId(const std::string& strClientId)
{
	DataBase::SQLRequest l_oSelectICCInfo;
	l_oSelectICCInfo.sql_id = "select_icc_t_client_in_out";
	l_oSelectICCInfo.param["client_id"] = strClientId;
	l_oSelectICCInfo.param["orderby"] = "create_time desc";

	DataBase::IResultSetPtr l_oSelectResult = m_pDBConn->Exec(l_oSelectICCInfo);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oSelectResult->GetSQL().c_str());
	if (!l_oSelectResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select failed:db error:[%s]", l_oSelectResult->GetErrorMsg().c_str());
		return "";
	}
	std::string l_strGuid;
	if (!l_oSelectResult->Next())
	{
		ICC_LOG_ERROR(m_pLog, "get inout data failed, client_id[%s]", strClientId.c_str());
		return "";
	}

	l_strGuid = l_oSelectResult->GetValue("guid");

	return l_strGuid;
}

bool CBusinessImpl::_UpdateClientInOutTime(const std::string& strGuid)
{
	std::string strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_client_in_out_when_server_start";
	l_oSQLUpDate.param["guid"] = strGuid;
	l_oSQLUpDate.param["current_time"] = strCurrentTime;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "update logout time for icc_t_client_in_out Failed:[%s]", l_oSQLUpDate.sql_id.c_str());
		return false;
	}

	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update logout time for icc_t_client_in_out Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

std::string CBusinessImpl::_QueryGuidFromOnOffByClientId(const std::string& strClientId)
{
	DataBase::SQLRequest l_oSQLQuery;
	l_oSQLQuery.sql_id = "select_icc_t_client_on_off";
	l_oSQLQuery.param["client_id"] = strClientId;

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select failed:db error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return "";
	}

	std::string l_strGuid;
	if (!l_pRSet->Next())
	{
		ICC_LOG_ERROR(m_pLog, "get inout data failed, client_id[%s]", strClientId.c_str());
		return "";
	}

	l_strGuid = l_pRSet->GetValue("guid");
	return l_strGuid;
}

bool CBusinessImpl::_UpdateClientOnOffTime(const std::string& strGuid)
{	
	std::string strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_client_on_off_when_server_start";
	l_oSQLUpDate.param["guid"] = strGuid;
	l_oSQLUpDate.param["current_time"] = strCurrentTime;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "update logout time for icc_t_client_on_off Failed:[%s]", l_oSQLUpDate.sql_id.c_str());
		return false;
	}

	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update logout time for icc_t_client_on_off Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

void CBusinessImpl::_ClearAllICCClientRegistInfo()
{
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);

	CRegisterInfo l_oClientRegisterInfo;	
	for (auto iter = l_mapClientRegisterInfo.begin(); iter != l_mapClientRegisterInfo.end(); ++iter)
	{
		if (l_oClientRegisterInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
		{
			//if ("ctype_client" == l_oClientRegisterInfo.m_strClientType) //不需要判断用户类型
			{
				std::string strInOutGuid = _QueryGuidFromInOutByClientId(l_oClientRegisterInfo.m_strClientID);
				if (!strInOutGuid.empty())
				{
					_UpdateClientInOutTime(strInOutGuid);
				}

				std::string strOnOffGuid = _QueryGuidFromOnOffByClientId(l_oClientRegisterInfo.m_strClientID);
				if (!strOnOffGuid.empty())
				{
					_UpdateClientOnOffTime(strOnOffGuid);
					//_UpdateStatistic(strOnOffGuid);
				}
				
				m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oClientRegisterInfo.m_strClientID);

				//增加通知客户端退出
				SyncClient(l_oClientRegisterInfo, ICC_CLIENT_LOGOUT,1);
			}
		}
	}

	
}

void CBusinessImpl::_SynServerStatus(const std::string& strServerId)
{
	PROTOCOL::CSyncServerStatus status;
	status.m_oHeader.m_strCmd = SERVER_STATUS_START_COMPLETE;
	status.m_oHeader.m_strSystemID = SYSTEMID;
	status.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	status.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	status.m_oHeader.m_strRelatedID = "";
	
	status.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	status.m_oHeader.m_strRequest = "topic_server_status_sync";
	status.m_oHeader.m_strRequestType = "1";

	status.m_oBody.m_strServerID = strServerId;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSyncMessage = status.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSyncMessage.c_str());
}


void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		return;
	}

	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncServerLock l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}
	std::string l_strServiceState = l_oSyncObj.m_oBody.m_strServiceState;
	if (SERVER_ALARM != l_oSyncObj.m_oBody.m_strServiceID)
	{
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "Server Role Sync,Config ServerFalg:%s, clientid:%s", m_pConfig->ServerFlag().c_str(), l_oSyncObj.m_oBody.m_strClientID.c_str());
	//
	if (l_strServiceState.compare("master") == 0)
	{
		if (l_oSyncObj.m_oBody.m_strClientID == m_pConfig->ServerFlag())
		{
			if (!m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: local Active, Start delete client register info");
				//刷新 删除之前先刷新数据库
				//_ClearAllICCClientRegistInfo();
				_SynServerStatus(l_oSyncObj.m_oBody.m_strClientID);

				//ICC_LOG_ERROR(m_pLog, "delete  CLIENT_REGISTER_INFO success!!!");
				m_bStartedTimerFlag = true;
			}
		}
		else
		{
			if (m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: remote Active, reset delete flag1");
				m_bStartedTimerFlag = false;
			}
		}
	}
	else
	{
		if (l_oSyncObj.m_oBody.m_strClientID == m_pConfig->ServerFlag())
		{
			if (m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: local Standby, reset delete flag2");
				m_bStartedTimerFlag = false;
			}
		}
	}
#if 0
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");

			return;
		}

		std::string l_strServiceState = l_oSyncObj.m_oBody.m_strServiceState;

		if (l_oSyncObj.m_oBody.m_strClientID == m_pConfig->ServerFlag() && l_strServiceState.compare("master") == 0)
		{
			if (!m_bStartedTimerFlag && l_oSyncObj.m_oBody.m_strServiceID == SERVER_ALARM)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Active, Start delete client register info");

				//刷新 删除之前先刷新数据库


				// 删除登录信息
				/*
				int iRecount = 0;
				while (iRecount <= REDELETE_COUNT)
				{
					int iRs = m_pRedisClient->Del(CLIENT_REGISTER_INFO);
					if (iRs >= 0)
					{
						break;
					}
					iRecount++;
					ICC_LOG_ERROR(m_pLog, "delete  CLIENT_REGISTER_INFO fialed!!! recount = %d", iRecount);
					m_pHelpTool->Sleep(100);
				}

				m_pRedisClient->Del(WEB_LOGIN_INFO);//删除注册信息的同时，也要删除web的登录缓存，保持两边数据的一致
			   */

				_ClearAllICCClientRegistInfo();
				_SynServerStatus(l_oSyncObj.m_oBody.m_strClientID);

				ICC_LOG_ERROR(m_pLog, "delete  CLIENT_REGISTER_INFO success!!!");

				m_bStartedTimerFlag = true;
			}
		}
		else if (SERVER_ALARM == l_oSyncObj.m_oBody.m_strServiceID)
		{
			if (m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Standby, reset delete flag");

				m_bStartedTimerFlag = false;
			}
		}
	}

#endif

}

DataBase::SQLRequest CBusinessImpl::ConstructSQLStruct(CUserInfo& p_oAccountInfo, std::string p_strSqlID)
{
	DataBase::SQLRequest l_oSQLExec;
	if (p_strSqlID == "select_icc_t_user")
	{
		l_oSQLExec.sql_id = p_strSqlID;
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("is_delete", p_oAccountInfo.m_strIsDelete));
	}
	else if (p_strSqlID == "update_icc_t_user")
	{
		l_oSQLExec.sql_id = p_strSqlID;
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("guid", p_oAccountInfo.m_strGuid));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("code", p_oAccountInfo.m_strCode));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("name", p_oAccountInfo.m_strName));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("pwd", p_oAccountInfo.m_strPwd));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("is_delete", p_oAccountInfo.m_strIsDelete));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("remark", p_oAccountInfo.m_strRemark));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_user", "AuthBusiness"));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	}
	else if (p_strSqlID == "recover_icc_t_user_flag")
	{
		l_oSQLExec.sql_id = p_strSqlID;
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("guid", p_oAccountInfo.m_strGuid));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("code", p_oAccountInfo.m_strCode));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("name", p_oAccountInfo.m_strName));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("pwd", p_oAccountInfo.m_strPwd));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("is_delete", p_oAccountInfo.m_strIsDelete));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("remark", p_oAccountInfo.m_strRemark));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_user", "AuthBusiness"));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	}
	else if (p_strSqlID == "insert_icc_t_user")
	{
		l_oSQLExec.sql_id = p_strSqlID;
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("guid", p_oAccountInfo.m_strGuid));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("code", p_oAccountInfo.m_strCode));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("name", p_oAccountInfo.m_strName));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("pwd", p_oAccountInfo.m_strPwd));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("is_delete", p_oAccountInfo.m_strIsDelete));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("create_user", "AuthBusiness"));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->CurrentDateTimeStr()));
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("remark", p_oAccountInfo.m_strRemark));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_user", "AuthBusiness"));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	}
	else if (p_strSqlID == "delete_icc_t_user")
	{
		l_oSQLExec.sql_id = "update_icc_t_user";
		l_oSQLExec.param.insert(std::pair<std::string, std::string>("guid", p_oAccountInfo.m_strGuid));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("is_delete", p_oAccountInfo.m_strIsDelete));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_user", "AuthBusiness"));
		l_oSQLExec.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
	}

	return l_oSQLExec;
}

void CBusinessImpl::OnCNotifiAcdAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMessage = p_pNotifiRequest->GetMessages();
	PROTOCOL::CAcdAgentState l_oRequest;
	if (!l_oRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse string error:[%s]", l_strMessage.c_str());
	}
	ICC_LOG_DEBUG(m_pLog, "receive AcdAgentStateRequest [%s]", l_strMessage.c_str());
	if (l_oRequest.m_oBody.m_strLoginMode == CTI_LOGIN_STATE)
	{
		if (l_oRequest.m_oBody.m_strReadyState == READY_STATE_BUSY)
		{
			UpdateUserDept(l_oRequest.m_oBody.m_strAgent,READY_STATE_BUSY);
		}
		else if (l_oRequest.m_oBody.m_strReadyState == READY_STATE_IDLE)
		{
			UpdateUserDept(l_oRequest.m_oBody.m_strAgent, READY_STATE_IDLE);
		}
	}
}

void ICC::CBusinessImpl::SetUserDept(const CRegisterInfo& p_ClientRegisterInfo)
{
	CUserDept l_oUserDept;
	l_oUserDept.m_strSeatNo = p_ClientRegisterInfo.m_strClientID;
	l_oUserDept.m_strUserName = p_ClientRegisterInfo.m_strClientName;

	/*DataBase::SQLRequest l_oSelectDept;
	l_oSelectDept.sql_id = "select_icc_t_user_dept";
	l_oSelectDept.param["user_code"] = p_ClientRegisterInfo.m_strClientName;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectDept);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return;
	}
	if (l_oResult->Next())
	{
		l_oUserDept.m_strDeptName = l_oResult->GetValue("dept_name");
		l_oUserDept.m_strDeptCode = l_oResult->GetValue("dept_code");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "select dept failed user_code:[%s]", p_ClientRegisterInfo.m_strClientName.c_str());
		return;
	}*/
	//前端传过来，服务端不用查
	l_oUserDept.m_strDeptDistrictCode = p_ClientRegisterInfo.m_strDistrictCode;
	l_oUserDept.m_strDeptName = p_ClientRegisterInfo.m_strDeptName;
	l_oUserDept.m_strDeptCode = p_ClientRegisterInfo.m_strDeptCode;
	l_oUserDept.m_strBuyIdle = READY_STATE_IDLE;
	std::string l_str = l_oUserDept.ToJson(m_pJsonFac->CreateJson());

	if (m_pRedisClient->HSet(LOGIN_USER_DEPT, l_oUserDept.m_strUserName, l_str))
	{
		ICC_LOG_DEBUG(m_pLog, "set login_user_dept success :[%s : %s]", l_oUserDept.m_strUserName.c_str(), l_str.c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "set login_user_dept failed :[%s : %s]", l_oUserDept.m_strUserName.c_str(), l_str.c_str());
	}
}


void ICC::CBusinessImpl::UpdateUserDept(std::string p_strSeatNo,std::string p_strState)
{
	std::map<std::string, std::string> l_mapItem;
	m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem);

	for (auto var : l_mapItem)
	{
		CUserDept l_oUserDept;
		l_oUserDept.Parse(var.second, m_pJsonFac->CreateJson());

		if (l_oUserDept.m_strSeatNo == p_strSeatNo)
		{
			l_oUserDept.m_strBuyIdle = p_strState;

			std::string l_strVal = l_oUserDept.ToJson(m_pJsonFac->CreateJson());
			m_pRedisClient->HSet(LOGIN_USER_DEPT, l_oUserDept.m_strUserName, l_strVal);
			break;
		}
	}
}


void ICC::CBusinessImpl::DelUserDept(std::string p_strSeatNo)
{
	std::map<std::string, std::string> l_mapItem;
	m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem);

	for (auto var : l_mapItem)
	{
		CUserDept l_oUserDept;
		l_oUserDept.Parse(var.second, m_pJsonFac->CreateJson());

		if (l_oUserDept.m_strSeatNo == p_strSeatNo)
		{
			m_pRedisClient->HDel(LOGIN_USER_DEPT, l_oUserDept.m_strUserName);
			break;
		}
	}
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiConnect(ObserverPattern::INotificationPtr p_pNotifiConnect)
{
	std::string l_strMessage = p_pNotifiConnect->GetMessages();
	PROTOCOL::CConnect l_oConnect;
	if (!l_oConnect.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receive ConnectionInfo [%s]", l_strMessage.c_str());

	return;  //不处理

	CMQClientInfo l_tMQClientInfo;
	l_tMQClientInfo.m_strConnectionID = l_oConnect.m_strConnectID;
	l_tMQClientInfo.m_strClientID = l_oConnect.m_strClientID;
	l_tMQClientInfo.m_strClientIP = l_oConnect.m_strClientIP;

	//添加MQ客户端连接信息
	std::string l_strVal = l_tMQClientInfo.ToJson(m_pJsonFac->CreateJson());
	m_pRedisClient->HSet(MQCONNECT_INFO, l_tMQClientInfo.m_strConnectionID, l_strVal);
	ICC_LOG_DEBUG(m_pLog, "MQ Connect Success:[%s][%s]", l_tMQClientInfo.m_strConnectionID.c_str(), l_oConnect.m_strClientIP.c_str());

}

void CBusinessImpl::OnCNotifiDisConnect(ObserverPattern::INotificationPtr p_pNotifiDisConnect)
{
	std::string l_strMessage = p_pNotifiDisConnect->GetMessages();
	PROTOCOL::CDisConnect l_oDisConnect;
	if (!l_oDisConnect.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "monitor error parsestring error:[%s]", l_strMessage.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recieve RemoveInfo [%s]", l_strMessage.c_str());

	return;  //不处理

	std::string l_strMQClientInfo;
	//根据connectId查找clientId
	if (!m_pRedisClient->HGet(MQCONNECT_INFO, l_oDisConnect.m_strConnectID, l_strMQClientInfo))
	{
		ICC_LOG_ERROR(m_pLog, "can not find connectid:[%s]", l_oDisConnect.m_strConnectID.c_str());
		return;
	}

	CMQClientInfo l_tMQClientInfo;
	if (!l_tMQClientInfo.Parse(l_strMQClientInfo, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "mqclientinfo parse json failed");
		return;
	}

	//根据clientId查找clientName
	std::string l_strClientRegisterInfo;
	CRegisterInfo l_oClientRegisterInfo;
	if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_tMQClientInfo.m_strClientID, l_strClientRegisterInfo))
	{
		ICC_LOG_ERROR(m_pLog, "can not find register info clientid[%s]", l_tMQClientInfo.m_strClientID.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "find register info by clientid[%s], content[%s]", l_tMQClientInfo.m_strClientID.c_str(), l_strClientRegisterInfo.c_str());

	if (!l_strClientRegisterInfo.empty())
	{
		if (!l_oClientRegisterInfo.Parse(l_strClientRegisterInfo, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse json failed");
		}
	}

	if (l_oClientRegisterInfo.m_strClientType == "ctype_client")
	{
		UpdateClientOffTime(l_oClientRegisterInfo.m_strClientID, l_oClientRegisterInfo.m_strClientName);	

		//同步客户端退出
		m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oClientRegisterInfo.m_strClientID);
		m_pRedisClient->HDel(MQCONNECT_INFO, l_oDisConnect.m_strConnectID);
		//m_pRedisClient->HDel(ONLINE_CLIENT, l_oClientRegisterInfo.m_strClientName);
		SyncClient(l_oClientRegisterInfo, ICC_CLIENT_LOGOUT,1);

		ICC_LOG_DEBUG(m_pLog, "sync client logout success[%s,%s]", l_oClientRegisterInfo.m_strClientID.c_str(), l_oClientRegisterInfo.m_strClientType.c_str());
	}
	else
	{	
		m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_tMQClientInfo.m_strClientID);
	}

	//删除其他服务MQ连接
	m_pRedisClient->HDel(MQCONNECT_INFO, l_tMQClientInfo.m_strConnectionID);
	ICC_LOG_DEBUG(m_pLog, "delete mq connect success[%s]", l_tMQClientInfo.m_strConnectionID.c_str());		
}
void CBusinessImpl::UpdateClientOffTime(const std::string& p_strClientId, const std::string& p_strClientName)
{
	DataBase::SQLRequest l_oSQLQuery;
	l_oSQLQuery.sql_id = "select_icc_t_client_on_off";
	l_oSQLQuery.param["client_id"] = p_strClientId;
	l_oSQLQuery.param["client_name"] = p_strClientName;

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLQuery);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Client name not exist :[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pRSet->Next() && l_pRSet->RecordSize() > 1)
		{
			for (unsigned int i = 0; i < l_pRSet->RecordSize(); i++)
			{
				std::string l_strOffTime;
				//已经做了倒序，如果有多条，第一条是否需要赋值？
				if (i == 0)
				{
					l_strOffTime = m_pDateTime->CurrentDateTimeStr();
				}
				else
				{
					l_strOffTime = l_pRSet->GetValue(i - 1, "on_time");
				}
				std::string l_strOnTime = l_pRSet->GetValue(i, "on_time");
				DateTime::CDateTime l_oOnTime = m_pDateTime->FromString(l_strOnTime);
				DateTime::CDateTime l_oOffTime = m_pDateTime->FromString(l_strOffTime);
				DateTime::CDateTime l_iDuringTime = (l_oOffTime - l_oOnTime) / CONVERT_SECOND;
				std::string l_strDuringTime = m_pString->Number((boost::uint64_t)l_iDuringTime);
				std::string strGuid = l_pRSet->GetValue(i, "guid");
				DataBase::SQLRequest l_oSQLUpDate;
				l_oSQLUpDate.sql_id = "update_icc_t_client_on_off";
				l_oSQLUpDate.param["guid"] = strGuid;
				l_oSQLUpDate.set["off_time"] = l_strOffTime;
				l_oSQLUpDate.set["duration"] = l_strDuringTime;
				l_oSQLUpDate.set["update_user"] = "Monitor Server";
				l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

				DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
				if (!l_oResult->IsValid())
				{
					ICC_LOG_ERROR(m_pLog, "Off Line Failed:[%s]", l_oResult->GetErrorMsg().c_str());
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "User Off time recover success:[%s]", l_oResult->GetSQL().c_str());
					//_UpdateStatistic(strGuid);
				}
			}
		}
		else if (l_pRSet->RecordSize() == 1)
		{
			std::string l_strOnTime = l_pRSet->GetValue(0, "on_time");
			DateTime::CDateTime l_oOnTime = m_pDateTime->FromString(l_strOnTime);
			DateTime::CDateTime l_iDuringTime = (m_pDateTime->CurrentDateTime() - l_oOnTime) / CONVERT_SECOND;
			std::string l_strDuringTime = m_pString->Number((boost::uint64_t)l_iDuringTime);
			std::string strGuid = l_pRSet->GetValue(0, "guid");
			DataBase::SQLRequest l_oSQLUpDate;
			l_oSQLUpDate.sql_id = "update_icc_t_client_on_off";
			l_oSQLUpDate.param["guid"] = strGuid;
			l_oSQLUpDate.set["off_time"] = m_pDateTime->CurrentDateTimeStr();
			l_oSQLUpDate.set["duration"] = l_strDuringTime;
			l_oSQLUpDate.set["update_user"] = "Monitor Server";
			l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

			DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
			if (!l_oResult->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "Off Line Failed:[%s]", l_oResult->GetErrorMsg().c_str());
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "User Off Line success:[%s]", l_oResult->GetSQL().c_str());
				//_UpdateStatistic(strGuid);
			}
		}
	}
}

bool CBusinessImpl::_UpdateStatistic(const std::string& strGuid)
{
	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_statistic_by_client_on_off";
	l_oSQLUpDate.param["guid"] = strGuid;	

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for client_in_off Failed:[%s]", l_oSQLUpDate.sql_id.c_str());
		return false;
	}

	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for client_in_off Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "update statistic info for client_in_off success:[%s][%s]", l_oSQLUpDate.sql_id.c_str(), l_oResult->GetSQL().c_str());

	return true;
}

bool Base64Encode(const string & input, string * output)
{
	typedef base64_from_binary<transform_width<string::const_iterator, 6, 8>> Base64EncodeIterator;
	stringstream result;
	try {
		copy(Base64EncodeIterator(input.begin()), Base64EncodeIterator(input.end()), ostream_iterator<char>(result));
	}
	catch (...) {
		return false;
	}
	size_t equal_count = (3 - input.length() % 3) % 3;
	for (size_t i = 0; i < equal_count; i++)
	{
		result.put('=');
	}
	*output = result.str();
	return output->empty() == false;
}

bool Base64Decode(const string & input, string * output)
{
	typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> Base64DecodeIterator;
	stringstream result;
	try {
		copy(Base64DecodeIterator(input.begin()), Base64DecodeIterator(input.end()), ostream_iterator<char>(result));
	}
	catch (...) {
		return false;
	}
	*output = result.str();
	return output->empty() == false;
}

std::string CBusinessImpl::_PacketAuthInfo(const std::string& strUser, const std::string& strPwd)
{
	std::string strTmp = strUser;
	strTmp += ":";
	strTmp += strPwd;

	string strBase64;
	Base64Encode(strTmp, &strBase64);

	return std::string("Basic ") + strBase64;
}

std::string CBusinessImpl::_PacketUrl(const std::string& strServer)
{
	std::string strPath = "/api/jolokia/read/org.apache.activemq:type=Broker,brokerName=*,connector=clientConnectors,connectorName=openwire,connectionViewType=clientId,connectionName=*";
	std::string strUrl = "http://";
	strUrl += strServer;
	strUrl += strPath;
	return strUrl;
}

bool CBusinessImpl::_IsConnectionInfoEnable(const std::string& strAllConnectionInfo)
{
	std::string strEnableFlag = "\"status\":200";
	if (strAllConnectionInfo.rfind(strEnableFlag) != std::string::npos)
	{
		return true;
	}
	return false;
}

void CBusinessImpl::_GetAllActiveConnectionInfo(const std::string& strServer, std::string& strAllConnectionInfo)
{
	std::string strUserName = m_pConfig->GetValue("ICC/Component/AmqClient/UserName", "admin");
	std::string strPassword = m_pConfig->GetValue("ICC/Component/AmqClient/Password", "admin");
	std::string strAuthInfo = _PacketAuthInfo(strUserName, strPassword);
	std::map<std::string, std::string> l_ParamsMap;
	l_ParamsMap["Authorization"] = strAuthInfo;
	l_ParamsMap["Connection"] = "close";
	m_pHttpClient->SetHeaders(l_ParamsMap);
	
	std::string strUrl = _PacketUrl(strServer);
	strAllConnectionInfo = m_pHttpClient->Get(strUrl, "");
}

bool CBusinessImpl::_ExistClientConnect(const std::string& strAllConnectionInfo, const std::string& strClientId)
{
	std::string strBase = "\"ClientId\":\"";
	strBase += strClientId;
	strBase += "\"";
	if (strAllConnectionInfo.find(strBase) != std::string::npos)
	{
		return true;
	}
	return false;
}

void CBusinessImpl::_GetMqServers(std::vector<std::string>& vecServers)
{
	std::string strPort = m_pConfig->GetValue("ICC/Component/AmqClient/ManagerPort", "22060");

	ICC_LOG_DEBUG(m_pLog, "mq manager port :[%s]", strPort.c_str());
	
	int iCount = m_pConfig->GetNodeCount("ICC/Component/AmqClient/Servers", "Server");
	for (int i = 0; i < iCount; i++)
	{
		std::string strPath = m_pString->Format("ICC/Component/AmqClient/Servers/Server[%d]", i);

		std::string strHost = m_pConfig->GetValue(strPath + "/Host", "");
		

		std::string strServer = strHost;
		strServer += ":";
		strServer += strPort;
		vecServers.push_back(strServer);
	}
}

bool CBusinessImpl::_IsClientActive(const std::string& strClientId)
{
	ICC_LOG_DEBUG(m_pLog, "_IsClientActive client id:[%s]", strClientId.c_str());

	std::vector<std::string> vecServers;
	_GetMqServers(vecServers);
	for (unsigned int i = 0; i < vecServers.size(); ++i)
	{
		std::string strAllActiveConnectionInfo;
		_GetAllActiveConnectionInfo(vecServers[i], strAllActiveConnectionInfo);
		if (!strAllActiveConnectionInfo.empty() && _IsConnectionInfoEnable(strAllActiveConnectionInfo) && _ExistClientConnect(strAllActiveConnectionInfo, strClientId))
		{
			ICC_LOG_DEBUG(m_pLog, "_IsClientActive client id is active :[%s], [%s]", strClientId.c_str(), strAllActiveConnectionInfo.c_str());
			return true;
		}
	}
	
	ICC_LOG_DEBUG(m_pLog, "_IsClientActive client id is lost :[%s]", strClientId.c_str());

	return false;
}

bool CBusinessImpl::_ClearRegistInfo(const std::string& strClientId)
{
	if (!m_pRedisClient->HDel(CLIENT_REGISTER_INFO, strClientId))
	{
		ICC_LOG_ERROR(m_pLog, "redis delete failed,client id is [%s]", strClientId.c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "redis delete success,client id is [%s]", strClientId.c_str());
	}
	return true;
}


void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{


	if (NULL == p_pNotifiRequest.get())
	{
		ICC_LOG_DEBUG(m_pLog, "on timer, input is null");
		return;
	}

	std::string l_strMessage(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "receive client heartbeat request:[%s]", l_strMessage.c_str());

	if (!m_bStartedTimerFlag)
	{
		ICC_LOG_DEBUG(m_pLog, "on timer, not master");
		return;
	}

	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);

	CRegisterInfo l_oClientRegisterInfo;
	PROTOCOL::CGetClientRespond::CBody::CData l_oData;
	
	//判断心跳是否超时
	DateTime::CDateTime tmp_oCurrTime = m_pDateTime->CurrentDateTime();
	std::string tmp_strCurrTime(m_pDateTime->ToString(tmp_oCurrTime));
	for (auto iter = l_mapClientRegisterInfo.begin(); iter != l_mapClientRegisterInfo.end(); ++iter)
	{
		if (l_oClientRegisterInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
		{
			unsigned int tmp_uiSecondDiff = m_pDateTime->SecondsDifference(m_pDateTime->FromString(l_oClientRegisterInfo.m_strHeartTime), tmp_oCurrTime);

			//是否心跳超时了
			if (tmp_uiSecondDiff >= m_uiTimeOutTime)
			{
				ICC_LOG_WARNING(m_pLog, "Client timeout, seatno: %s, now time: %s, last heartbeat time: %s, seconddiff: %u, timeouttime: %u", 
					iter->first.c_str(), tmp_strCurrTime.c_str(), l_oClientRegisterInfo.m_strHeartTime.c_str(), tmp_uiSecondDiff, m_uiTimeOutTime);

				UpdateClientOffTime(l_oClientRegisterInfo.m_strClientID, l_oClientRegisterInfo.m_strClientName);

				//同步客户端退出
				m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oClientRegisterInfo.m_strClientID);
				
				_SyncAgentTimeOut(l_oClientRegisterInfo, iter->first);

				SyncClient(l_oClientRegisterInfo, ICC_CLIENT_LOGOUT,2);

				ICC_LOG_DEBUG(m_pLog, "sync client logout success[%s,%s]", l_oClientRegisterInfo.m_strClientID.c_str(), l_oClientRegisterInfo.m_strClientType.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Parse failed, seatno: %s,  msg: %s", iter->first.c_str(), iter->second.c_str());
		}
	}
}

void CBusinessImpl::OnNotifClientHeartBeat(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMessage(p_pNotifiRequest->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "receive client heartbeat request:[%s]", l_strMessage.c_str());

	PROTOCOL::CHeaderEx tmp_oHeader;
	if (!tmp_oHeader.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse client heartbeat request failed");
		return;
	}


	PROTOCOL::CClientHeartbeatRespond tmp_oRespond;
	tmp_oRespond.m_oHeader = tmp_oHeader;
	tmp_oRespond.m_oBody.m_strInterval = m_strHeartBeatTime; 
	do
	{
		std::string l_strVal;
		if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO, tmp_oHeader.m_strSeatNo, l_strVal))
		{
			tmp_oRespond.m_oHeader.m_strResult = "1";
			tmp_oRespond.m_oHeader.m_strMsg = "not find client register data";
			break;
		}

		CRegisterInfo tmp_oRegistInfo;

		if (!tmp_oRegistInfo.Parse(l_strVal, m_pJsonFac->CreateJson()))
		{
			tmp_oRespond.m_oHeader.m_strResult = "2";
			tmp_oRespond.m_oHeader.m_strMsg = "parse client register data failed";
			ICC_LOG_ERROR(m_pLog, "parse client register data failed, data: %s", l_strVal.c_str());
			break;
		}

		tmp_oRegistInfo.m_strHeartTime = m_pDateTime->CurrentDateTimeStr();

		l_strVal = tmp_oRegistInfo.ToJson(m_pJsonFac->CreateJson());

		if (l_strVal.empty())
		{
			tmp_oRespond.m_oHeader.m_strResult = "3";
			tmp_oRespond.m_oHeader.m_strMsg = "update to json failed";
			break;
		}

		if (!m_pRedisClient->HSet(CLIENT_REGISTER_INFO, tmp_oHeader.m_strSeatNo, l_strVal))
		{
			tmp_oRespond.m_oHeader.m_strResult = "4";
			tmp_oRespond.m_oHeader.m_strMsg = "update register data failed";
			break;
		}

	}while(0);
	
	l_strMessage = tmp_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "send message [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiRegisterRequest)
{
	std::string l_strMessage = p_pNotifiRegisterRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive client register request:[%s]", l_strMessage.c_str());

	PROTOCOL::CClientRegisterRequest l_oClientRegRequest;
	if (!l_oClientRegRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "client register ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}

	CRegisterInfo l_oRegisterInfo;
	std::string l_strUserCode = l_oClientRegRequest.m_oBody.m_strClientName;
	l_oRegisterInfo.m_strClientType = l_oClientRegRequest.m_oBody.m_strClientType;
	l_oRegisterInfo.m_strClientName = l_strUserCode;
	l_oRegisterInfo.m_strClientID = l_oClientRegRequest.m_oHeader.m_strSeatNo;
	l_oRegisterInfo.m_strClientIP = l_oClientRegRequest.m_oBody.m_strIP;
	l_oRegisterInfo.m_strHeartTime = m_pDateTime->CurrentDateTimeStr();

	l_oRegisterInfo.m_strDeptCode = l_oClientRegRequest.m_oBody.m_strStaffDeptCode;
	l_oRegisterInfo.m_strDeptName = l_oClientRegRequest.m_oBody.m_strStaffDeptName;
	l_oRegisterInfo.m_strStaffCode = l_oClientRegRequest.m_oBody.m_strStaffCode;
	l_oRegisterInfo.m_strStaffName = l_oClientRegRequest.m_oBody.m_strStaffName;
	l_oRegisterInfo.m_strUserName = l_oClientRegRequest.m_oBody.m_strStaffName;

	l_oRegisterInfo.m_strDistrictCode = l_oClientRegRequest.m_oBody.m_strDistrictCode;
	bool tmp_bClientLogined = false;
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);
	do
	{
		std::string tmp_strRedisValue;
		m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_oRegisterInfo.m_strClientID, tmp_strRedisValue);

		//没有登陆的数据，返回
		if (tmp_strRedisValue.empty())
		{
			CRegisterInfo l_oClientRegisterInfo;
			// 如果这次坐席不同，轮询 内存里面还有没有此账号信息
			for (auto iter = l_mapClientRegisterInfo.begin(); iter != l_mapClientRegisterInfo.end(); ++iter)
			{
				if (l_oClientRegisterInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
				{
					if (l_oRegisterInfo.m_strClientName == l_oClientRegisterInfo.m_strClientName && l_oRegisterInfo.m_strUserName == l_oClientRegisterInfo.m_strUserName)
					{
						ICC_LOG_DEBUG(m_pLog, " ClientRegister info [%s-%s-%s]", l_oRegisterInfo.m_strClientID.c_str(), l_oRegisterInfo.m_strClientName.c_str(), l_oRegisterInfo.m_strUserName.c_str());
						std::string l_strGuid = m_pString->CreateGuid();
						std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
						/*if (!InsertUserOnOff(l_strGuid, l_oClientRegisterInfo.m_strClientName, l_oClientRegisterInfo.m_strClientID, l_oClientRegisterInfo.m_strDeptCode, l_strCurrentTime, l_oClientRegRequest.m_oBody))
						{
							ICC_LOG_ERROR(m_pLog, "db client regist info failed:[%s]", l_oClientRegRequest.m_oBody.m_strClientID.c_str());
						}*/
						if (!m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oClientRegisterInfo.m_strClientID))
						{
							ICC_LOG_ERROR(m_pLog, "del redis client regist info failed:[%s]", l_oClientRegRequest.m_oBody.m_strClientID.c_str());
						}
						// 发送同步消息
						SyncClient(l_oClientRegisterInfo, ICC_CLIENT_LOGOUT, 1);
						break;
					}
				}
			}
			break;
		}

		//解析是否成功
		CRegisterInfo tmp_oExistUser;
		if (!tmp_oExistUser.Parse(tmp_strRedisValue, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "client id already exist, but parse failed, msg: %s.", tmp_strRedisValue.c_str());
			break;
		}

		if (tmp_oExistUser.m_strClientName == l_oRegisterInfo.m_strClientName)
		{
			tmp_bClientLogined = true;
			ICC_LOG_DEBUG(m_pLog, "clientalready login, id: %s, clientname: %s.", l_oRegisterInfo.m_strClientID.c_str(), l_oRegisterInfo.m_strClientName.c_str());
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "clientid logined, id: %s, clientname is different, reg msg: %s.", l_oRegisterInfo.m_strClientID.c_str(), tmp_strRedisValue.c_str());
		}

	} while (0);


	PROTOCOL::CClientRegisterRespond l_oClientRegRespond;
	l_oClientRegRespond.m_oHeader.m_strMsgId = l_oClientRegRequest.m_oHeader.m_strMsgId;

	//std::string l_strSatffCode, l_strSatffName, l_strDeptCode, l_strDeptName;
	/*if (!GetBindInfoByCode(l_strUserCode, l_strSatffCode, l_strSatffName, l_strDeptCode, l_strDeptName))
	{
		l_oClientRegRespond.m_oHeader.m_strResult = RESULT_FAILED;
		BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, RESULT_FAILED);
		l_oClientRegRespond.m_oHeader.m_strMsg = "not find bind info by code " + l_strUserCode;
		l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRegisterRequest->Response(l_strMessage);
		ICC_LOG_ERROR(m_pLog, "get user[%s] bind info failed.", l_strUserCode.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "user[%s] bind staff[%s][%s] deptcode[%s][%s]",
		l_strUserCode.c_str(),
		l_strSatffCode.c_str(), l_strSatffName.c_str(),
		l_strDeptCode.c_str(), l_strDeptName.c_str());

	l_oRegisterInfo.m_strUserName = l_strSatffName;		//对于登录的客户端，是用户名绑定的警员的名字
	l_oRegisterInfo.m_strDeptCode = l_strDeptCode;		//用户所属的单位编码
	l_oRegisterInfo.m_strDeptName = l_strDeptName;		//用户所属的单位名称
	l_oRegisterInfo.m_strStaffCode = l_strSatffCode;
	l_oRegisterInfo.m_strStaffName = l_strSatffName;*/

	std::string l_strDeptCode = l_oRegisterInfo.m_strDeptCode;
	std::string l_strDeptType = GetDeptType(l_strDeptCode);
	if (l_strDeptType == "DIC014013" || l_strDeptType == "DIC014001")
	{
		//市局或者分局
		l_oRegisterInfo.m_strSeatType = std::to_string(GetSeatType(l_oRegisterInfo.m_strClientName));//席位类型，默认暂定是接处警席
	}
	else if (l_strDeptType == "DIC014002" || l_strDeptType == "DIC014007")
	{
		//派出所，警务站
		l_oRegisterInfo.m_strSeatType = "4";
	}

	//进行客户端IP鉴权
	/*if (m_bIpAuthFlag)
	{
		ICC_LOG_DEBUG(m_pLog, "begin to auth user[%s]", l_strUserCode.c_str());
		if (!Auth(l_oClientRegRequest.m_oBody.m_strClientName, l_oClientRegRequest.m_oBody.m_strClientID, l_oClientRegRequest.m_oBody.m_strIP))
		{
			BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, IPAUTH_FAILED);
			l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
			p_pNotifiRegisterRequest->Response(l_strMessage);
			return;
		}

		ICC_LOG_DEBUG(m_pLog, "complete to auth user[%s]", l_strUserCode.c_str());
	}*/

	//多客户端登录
	/*bool b_IsMultiClient = (!m_strClientName.empty() && m_strClientName == l_oClientRegRequest.m_oBody.m_strClientName);
	if (!b_IsMultiClient)
	{
		ICC_LOG_DEBUG(m_pLog, "begin to multi client auth user[%s]", l_strUserCode.c_str());

		if (!MultiClientAuth(l_oClientRegRequest.m_oBody.m_strClientName, l_oClientRegRequest.m_oBody.m_strClientID, l_oClientRegRequest.m_oBody.m_strIP))
		{
			BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, REPEAT_LOGIN);
			l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
			p_pNotifiRegisterRequest->Response(l_strMessage);
			return;
		}

		UpdateClientOffTime(l_oClientRegRequest.m_oBody.m_strClientID, l_oClientRegRequest.m_oBody.m_strClientName);
		ICC_LOG_DEBUG(m_pLog, "complete to multi client auth user[%s]", l_strUserCode.c_str());
	}*/

	std::string l_strVal = l_oRegisterInfo.ToJson(m_pJsonFac->CreateJson());
	if (!m_pRedisClient->HSet(CLIENT_REGISTER_INFO, l_oRegisterInfo.m_strClientID, l_strVal))
	{
		l_oClientRegRespond.m_oHeader.m_strResult = RESULT_FAILED;
		BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, RESULT_FAILED);
		l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRegisterRequest->Response(l_strMessage);
		ICC_LOG_ERROR(m_pLog, "client regist failed:[%s]", l_oClientRegRequest.m_oBody.m_strClientID.c_str());
		return;
	}

	//没有登陆需要操作数据库
	//将上线信息存入数据库，先查询用户对应的部门名称

	if (!tmp_bClientLogined)
	{
		std::string l_strGuid = m_pString->CreateGuid();
		std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
		//上下线信息入库
		if (!InsertUserOnOff(l_strGuid, l_strUserCode, l_oClientRegRequest.m_oBody.m_strClientID, l_strDeptCode, l_strCurrentTime, l_oClientRegRequest.m_oBody))
		{
			if (!m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oRegisterInfo.m_strClientID)) //如果失败，删除redis中的数据 
			{
				ICC_LOG_ERROR(m_pLog, "del redis client regist info failed:[%s]", l_oClientRegRequest.m_oBody.m_strClientID.c_str());
			}

			l_oClientRegRespond.m_oHeader.m_strResult = RESULT_FAILED;
			BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, RESULT_FAILED);
			l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
			p_pNotifiRegisterRequest->Response(l_strMessage);
			ICC_LOG_ERROR(m_pLog, "insert user[%s] on off failed.", l_strUserCode.c_str());
			return;
		}

		//_InsertStatisticRecord(l_strGuid, l_oClientRegRequest.m_oBody.m_strClientName, l_oClientRegRequest.m_oBody.m_strClientID, l_strDeptCode, l_strCurrentTime);
	}

	ICC_LOG_DEBUG(m_pLog, "insert user[%s] on off success, user login success.", l_strUserCode.c_str());

	SyncClient(l_oRegisterInfo, ICC_CLIENT_LOGIN,0);

	l_oClientRegRespond.m_oBody.m_strUserName = l_oRegisterInfo.m_strStaffName;		//对于登录的客户端，是用户名绑定的警员的名字
	l_oClientRegRespond.m_oBody.m_strDeptCode = l_strDeptCode;		//用户所属的单位编码
	l_oClientRegRespond.m_oBody.m_strDeptName = l_oRegisterInfo.m_strDeptName;		//用户所属的单位名称
	l_oClientRegRespond.m_oBody.m_strStaffCode = l_oRegisterInfo.m_strStaffCode;
	l_oClientRegRespond.m_oBody.m_strStaffName = l_oRegisterInfo.m_strStaffName;

	BuildRespondBody(l_oClientRegRespond.m_oBody, l_oClientRegRequest.m_oBody, RESULT_SUCCESS);
	l_strMessage = l_oClientRegRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRegisterRequest->Response(l_strMessage);

	ICC_LOG_DEBUG(m_pLog, "send ClientRegisterRespond [%s]", l_strMessage.c_str());
}

bool CBusinessImpl::Auth(const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_IPList)
{
	CUserInfo l_oUserInfo;
	if (!GetUserInfo(p_strUserCode, l_oUserInfo))
	{
		return false;
	}

	// 进行IP合法性校验
	std::string l_strIP = l_oUserInfo.m_strRemark;
	bool l_bIsSuperIp = false;

	if (l_strIP.empty())
	{
		//IP为空，默认不鉴权
		l_bIsSuperIp = true;
	}

	if (!l_bIsSuperIp)
	{
		//格式不对，默认不鉴权
		if (!m_pString->RegexMatch(l_strIP,
			"^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$")
			&& !m_pString->RegexMatch(l_strIP,
			"^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))-((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"))
		{
			l_bIsSuperIp = true;
		}
	}

	//进行非*类IP鉴权
	if (!l_bIsSuperIp)
	{
		std::string l_strClientIPBegin;
		std::string l_strClientIPEnd;

		std::vector<std::string> l_vecIP;
		int l_iIpNum = m_pString->Split(p_IPList, ",", l_vecIP, 0);
		l_strClientIPBegin = m_pString->Left(l_oUserInfo.m_strRemark, m_pString->FindIndex(l_oUserInfo.m_strRemark, "-", 0));
		l_strClientIPEnd = m_pString->Right(l_oUserInfo.m_strRemark, (l_oUserInfo.m_strRemark.length()) - (m_pString->FindIndex(l_oUserInfo.m_strRemark, "-", 0)) - 1);
		for (unsigned int l_iIndex = 0; l_iIndex < l_vecIP.size(); l_iIndex++)
		{
			unsigned int l_iSrcIp = m_pHelpTool->IpToInt(l_vecIP[l_iIndex]);
			unsigned int l_iBeginIp = m_pHelpTool->IpToInt(l_strClientIPBegin);
			unsigned int l_iEndIp = m_pHelpTool->IpToInt(l_strClientIPEnd);
			if (l_iSrcIp < l_iBeginIp || l_iSrcIp > l_iEndIp || l_iBeginIp > l_iEndIp)
			{
				ICC_LOG_ERROR(m_pLog, "client ip [%s] is out of range:[%s]", p_IPList.c_str(), l_oUserInfo.m_strRemark.c_str());
				return false;
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Client Auth IP Success:[%s,%s]", l_oUserInfo.m_strRemark.c_str(), p_IPList.c_str());
	}

	return true;
}

bool CBusinessImpl::MultiClientAuth(const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_IPList)
{
	// 非多个客户端登录进行重复登录判断
	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);

	CRegisterInfo l_oClientRegisterInfo;
	PROTOCOL::CGetClientRespond::CBody::CData l_oData;
	for (auto iter = l_mapClientRegisterInfo.begin(); iter != l_mapClientRegisterInfo.end(); ++iter)
	{
		if (l_oClientRegisterInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
		{
			if (p_strUserCode == l_oClientRegisterInfo.m_strClientName)//重复登录
			{
				ICC_LOG_DEBUG(m_pLog, "user[%s] has repeat login", p_strUserCode.c_str());

				if (p_IPList == l_oClientRegisterInfo.m_strClientIP)
				{
					ICC_LOG_ERROR(m_pLog, "user[%s], clientid[%s] has repeat login and ip is same, will delete redis", p_strUserCode.c_str(), p_strClientID.c_str());
					_ClearRegistInfo(p_strClientID);
				}
				else
				{
					if (_IsClientActive(l_oClientRegisterInfo.m_strClientID))
					{
						return false;
					}
					else
					{
						ICC_LOG_ERROR(m_pLog, "user[%s], clientid[%s] has repeat login and Client is lost:, will delete redis", 
							p_strUserCode.c_str(), p_strClientID.c_str());
						_ClearRegistInfo(p_strClientID);
					}
				}
			}
		}
	}

	return true;
}

bool CBusinessImpl::GetDeptCodeByUser(const std::string& p_strUserCode, std::string& p_strDeptCode, std::string& p_strDeptName)
{
	DataBase::SQLRequest l_oSelectDept;
	l_oSelectDept.sql_id = "select_icc_t_user_dept";
	l_oSelectDept.param["user_code"] = p_strUserCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectDept);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_oResult->Next())
	{
		ICC_LOG_ERROR(m_pLog, "select dept failed user_code:[%s]", p_strUserCode.c_str());
		return false;
	}

	p_strDeptCode = l_oResult->GetValue("dept_code");
	p_strDeptName = l_oResult->GetValue("dept_name");

	return true;
}

bool CBusinessImpl::InsertUserOnOff(const std::string& p_strGuid, const std::string& p_strUserCode, const std::string& p_strClientID, const std::string& p_strDeptCode, const std::string& p_strCurrentTime, const PROTOCOL::CClientRegisterRequest::CBody& p_oRequestBody)
{
	DataBase::SQLRequest l_oSQLReq;
	l_oSQLReq.sql_id = "insert_icc_t_client_on_off";
	l_oSQLReq.param["guid"] = p_strGuid;
	l_oSQLReq.param["client_name"] = p_strUserCode;
	l_oSQLReq.param["client_id"] = p_strClientID;
	l_oSQLReq.param["on_time"] = p_strCurrentTime;
	l_oSQLReq.param["create_user"] = "Monitor Server";
	l_oSQLReq.param["create_time"] = p_strCurrentTime;
	l_oSQLReq.param["update_user"] = "Monitor Server";
	l_oSQLReq.param["update_time"] = p_strCurrentTime;
	l_oSQLReq.param["dept_code"] = p_strDeptCode;
	l_oSQLReq.param["receipt_code"] = p_oRequestBody.m_strStaffCode;
	l_oSQLReq.param["receipt_name"] = p_oRequestBody.m_strStaffName;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLReq);

	return l_pRSet->IsValid();
}

//通过用户名获取席位类型
unsigned int CBusinessImpl::GetSeatType(const std::string& p_strUserCode)
{
	DataBase::SQLRequest l_oSelectUserFunc;
	l_oSelectUserFunc.sql_id = "get_user_bind_func";
	l_oSelectUserFunc.param["code"] = p_strUserCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectUserFunc);
	ICC_LOG_DEBUG(m_pLog, "GetSeatType sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	std::map<std::string, std::string> l_funcCodes;
	while (l_oResult->Next())
	{
		std::string l_strFuncCode = l_oResult->GetValue("func_code");
		std::string l_strName = l_oResult->GetValue("name");
		l_funcCodes[l_strFuncCode] = l_strName;
	}
	//ICC01000000 接警
	//ICC02000000 处警
	unsigned int l_nSeatType = 0;
	if (l_funcCodes.count("ICC01000000") > 0 && l_funcCodes.count("ICC02000000") == 0)
	{
		l_nSeatType = 1;	//接警
	}
	else if (l_funcCodes.count("ICC01000000") == 0 && l_funcCodes.count("ICC02000000") > 0)
	{
		l_nSeatType = 2;	//处警
	}
	else if (l_funcCodes.count("ICC01000000") > 0 && l_funcCodes.count("ICC02000000") > 0)
	{
		l_nSeatType = 3;	//接处警
	}
	else
	{
		l_nSeatType = 3;	//接处警
	}
	return l_nSeatType;
}

//通过获取单位类型
std::string CBusinessImpl::GetDeptType(const std::string& p_strDeptCode)
{
	DataBase::SQLRequest l_oSelect;
	l_oSelect.sql_id = "select_icc_t_dept";
	l_oSelect.param["code"] = p_strDeptCode;

	std::string l_strDeptType = "0";

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelect);
	ICC_LOG_DEBUG(m_pLog, "GetSeatType sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return l_strDeptType;
	}
	
	if (l_oResult->Next())
	{
		l_strDeptType = l_oResult->GetValue("type");
	}

	return l_strDeptType;
}

bool CBusinessImpl::GetUserInfo(const std::string & p_strUserCode, CUserInfo& p_oUserInfo)
{
	std::map<std::string, std::string> l_mapUserInfo;
	if (m_pRedisClient->HGetAll(AUTH_USER_INFO, l_mapUserInfo))
	{
		for (auto iter = l_mapUserInfo.begin(); iter != l_mapUserInfo.end(); ++iter)
		{
			if (p_oUserInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
			{
				if (p_oUserInfo.m_strCode == p_strUserCode)
				{					
					return true;
				}
			}
		}
	}

	return false;
}

bool CBusinessImpl::GetBindInfoByCode(const std::string & p_strUserCode, std::string & p_strStaffCode, std::string & p_strStaffName, std::string & p_strDeptCode, std::string & p_strDeptName)
{
	DataBase::SQLRequest l_oSelectDept;
	l_oSelectDept.sql_id = "get_bindInfo_by_code";
	l_oSelectDept.param["code"] = p_strUserCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectDept);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_oResult->Next())
	{
		ICC_LOG_ERROR(m_pLog, "select user_code bind info failed user_code:[%s]", p_strUserCode.c_str());
		return false;
	}

	p_strStaffCode = l_oResult->GetValue("staff_code");
	p_strStaffName = l_oResult->GetValue("name");
	p_strDeptCode = l_oResult->GetValue("dept_code");
	p_strDeptName = l_oResult->GetValue("dept_name");

	return true;
}

bool CBusinessImpl::_InsertStatisticRecord(const std::string& strGuid, const std::string& strClientName, const std::string& strClientId, const std::string& strDeptCode, const std::string& strOnTime)
{	
	DataBase::SQLRequest sqlInsertStatistic;
	sqlInsertStatistic.sql_id = "insert_icc_t_statistic_record";
	sqlInsertStatistic.param["guid"] = strGuid;
	sqlInsertStatistic.param["client_name"] = strClientName;
	sqlInsertStatistic.param["client_id"] = strClientId;
	sqlInsertStatistic.param["dept_code"] = strDeptCode;
	sqlInsertStatistic.param["on_time"] = strOnTime;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(sqlInsertStatistic);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "insert statistic record failed: sql id: %s", sqlInsertStatistic.sql_id.c_str());
		return false;
	}
	
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert statistic record failed:[DB Error]:%s", l_oResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "insert statistic record success : [%s][%s]", sqlInsertStatistic.sql_id.c_str(), l_oResult->GetSQL().c_str());

	return true;
}

void CBusinessImpl::OnCNotifiGetClientInfoRequest(ObserverPattern::INotificationPtr p_pNotifiGetClientInfo)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiGetClientInfo->GetMessages().c_str());

	std::string l_strMessage = p_pNotifiGetClientInfo->GetMessages();
	
	PROTOCOL::CGetClientRequest l_oGetClientRequest;
	if (!l_oGetClientRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCNotifiGetClientInfoRequest]  ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}

	
	PROTOCOL::CGetClientRespond l_oGetClientRespond;
	l_oGetClientRespond.m_oHeader.m_strMsgId = l_oGetClientRequest.m_oHeader.m_strMsgId;
	l_oGetClientRespond.m_oHeader.m_strRequestFlag = l_oGetClientRequest.m_oHeader.m_strRequestFlag;
	//BuildRespondHeader(l_oGetClientRespond.m_oHeader, l_oGetClientRequest.m_oHeader);

	std::map<std::string, std::string> l_mapClientRegisterInfo;
	m_pRedisClient->HGetAll(CLIENT_REGISTER_INFO, l_mapClientRegisterInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnCNotifiGetClientInfoRequest] HGetAll complete! msgid = %s， mapsize: %d", l_oGetClientRequest.m_oHeader.m_strMsgid.c_str(), l_mapClientRegisterInfo.size());

	CRegisterInfo l_oClientRegisterInfo;
	PROTOCOL::CGetClientRespond::CBody::CData l_oData;
	bool l_bAlarmOnce = false;
	bool l_bDbOnce = false;
	l_oGetClientRespond.m_oBody.m_vecData.clear();
	for (auto iter = l_mapClientRegisterInfo.begin(); iter != l_mapClientRegisterInfo.end(); ++iter)
	{
		if (l_oClientRegisterInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
		{
			std::string l_strClientIdHead = m_pString->Left(l_oClientRegisterInfo.m_strClientID, m_pString->FindIndex(l_oClientRegisterInfo.m_strClientID, ".", 2));
			if (l_oClientRegisterInfo.m_strClientType == CTYPE_ALARM || l_oClientRegisterInfo.m_strClientType == CTYPE_DBAGENT)
			{
				if (l_oClientRegisterInfo.m_strClientType == CTYPE_ALARM)
				{
					if (m_pRedisClient->HExists(ALARMONLINE_LIST, l_oClientRegisterInfo.m_strClientID))
					{
						if (!l_bAlarmOnce)
						{
							if (l_oGetClientRequest.m_oBody.m_strClientType.empty()
								|| m_pString->StartsWith(l_oClientRegisterInfo.m_strClientType, l_oGetClientRequest.m_oBody.m_strClientType))
							{
								l_oData.m_strClientID = l_strClientIdHead;
								l_oData.m_strClientName = l_oClientRegisterInfo.m_strClientName;
								l_oData.m_strClientType = l_oClientRegisterInfo.m_strClientType;
								l_oData.m_strClientIP = l_oClientRegisterInfo.m_strClientIP;
								l_oData.m_strStaffCode = l_oClientRegisterInfo.m_strStaffCode;
								l_oData.m_strStaffName = l_oClientRegisterInfo.m_strStaffName;
								l_oData.m_strDistrictCode = l_oClientRegisterInfo.m_strDistrictCode;
								l_oData.m_strStaffDeptCode = l_oClientRegisterInfo.m_strDeptCode;
								l_oData.m_strStaffDeptName = l_oClientRegisterInfo.m_strDeptName;
								l_oGetClientRespond.m_oBody.m_vecData.push_back(l_oData);
							}
							l_bAlarmOnce = true;
						}
						continue;
					}
				}
				if (l_oClientRegisterInfo.m_strClientType == CTYPE_DBAGENT)
				{
					if (m_pRedisClient->HExists(DBONLINE_LIST, l_oClientRegisterInfo.m_strClientID))
					{
						if (!l_bDbOnce)
						{
							if (l_oGetClientRequest.m_oBody.m_strClientType.empty()
								|| m_pString->StartsWith(l_oClientRegisterInfo.m_strClientType, l_oGetClientRequest.m_oBody.m_strClientType))
							{
								l_oData.m_strClientID = l_strClientIdHead;
								l_oData.m_strClientName = l_oClientRegisterInfo.m_strClientName;
								l_oData.m_strClientType = l_oClientRegisterInfo.m_strClientType;
								l_oData.m_strClientIP = l_oClientRegisterInfo.m_strClientIP;
								l_oData.m_strStaffCode = l_oClientRegisterInfo.m_strStaffCode;
								l_oData.m_strStaffName = l_oClientRegisterInfo.m_strStaffName;
								l_oData.m_strDistrictCode = l_oClientRegisterInfo.m_strDistrictCode;
								l_oData.m_strStaffDeptCode = l_oClientRegisterInfo.m_strDeptCode;
								l_oData.m_strStaffDeptName = l_oClientRegisterInfo.m_strDeptName;
								l_oGetClientRespond.m_oBody.m_vecData.push_back(l_oData);
							}
							l_bDbOnce = true;
						}
						continue;
					}
				}
			}
			else
			{
				if ((l_oGetClientRequest.m_oBody.m_strClientType.empty()&&(!l_oClientRegisterInfo.m_strClientID.empty()))
					|| m_pString->StartsWith(l_oClientRegisterInfo.m_strClientType, l_oGetClientRequest.m_oBody.m_strClientType))
				{
					l_oData.m_strClientID = l_oClientRegisterInfo.m_strClientID;
					l_oData.m_strClientName = l_oClientRegisterInfo.m_strClientName;
					l_oData.m_strClientType = l_oClientRegisterInfo.m_strClientType;
					l_oData.m_strClientIP = l_oClientRegisterInfo.m_strClientIP;
					l_oData.m_strStaffCode = l_oClientRegisterInfo.m_strStaffCode;
					l_oData.m_strStaffName = l_oClientRegisterInfo.m_strStaffName;
					l_oData.m_strDistrictCode = l_oClientRegisterInfo.m_strDistrictCode;
					l_oData.m_strStaffDeptCode = l_oClientRegisterInfo.m_strDeptCode;
					l_oData.m_strStaffDeptName = l_oClientRegisterInfo.m_strDeptName;
					l_oGetClientRespond.m_oBody.m_vecData.push_back(l_oData);
				}				
			}
		}
	}

	l_oGetClientRespond.m_oBody.m_strCount = m_pString->Number(l_oGetClientRespond.m_oBody.m_vecData.size());
	l_strMessage = l_oGetClientRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiGetClientInfo->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetClientInfoResond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiDeleteClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiDeleteRegisterRequest)
{
	std::string l_strMessage = p_pNotifiDeleteRegisterRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive DeleteClientRegisterRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CDeleteClientRegisterRequest l_oDeleteClientRegRequest;
	if (!l_oDeleteClientRegRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}

	PROTOCOL::CClientRegisterRespond l_oDeleteClientRegRespond;
	l_oDeleteClientRegRespond.m_oHeader.m_strMsgId = l_oDeleteClientRegRequest.m_oHeader.m_strMsgId;

	std::string l_strClientRegisterInfo;
	if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_oDeleteClientRegRequest.m_oBody.m_strClientID, l_strClientRegisterInfo))
	{
		ICC_LOG_ERROR(m_pLog, "can not find connectid:[%s]", l_oDeleteClientRegRequest.m_oBody.m_strClientID.c_str());

		l_oDeleteClientRegRespond.m_oHeader.m_strResult = "1";
		l_oDeleteClientRegRespond.m_oHeader.m_strMsg= "Get redis cache failed";

		l_strMessage = l_oDeleteClientRegRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiDeleteRegisterRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DeleteClientRegisterRespond [%s]", l_strMessage.c_str());

		return;
	}

	if (!m_pRedisClient->HDel(CLIENT_REGISTER_INFO, l_oDeleteClientRegRequest.m_oBody.m_strClientID))
	{
		ICC_LOG_ERROR(m_pLog, "redis delete failed,client id is [%s]", l_oDeleteClientRegRequest.m_oBody.m_strClientID.c_str());
	}
	else
	{
	ICC_LOG_DEBUG(m_pLog, "redis delete success,client id is [%s]", l_oDeleteClientRegRequest.m_oBody.m_strClientID.c_str());
	}

	CRegisterInfo l_oRegisterInfo;
	if (!l_oRegisterInfo.Parse(l_strClientRegisterInfo, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse json failed");

		l_oDeleteClientRegRespond.m_oHeader.m_strResult = "1";
		l_oDeleteClientRegRespond.m_oHeader.m_strMsg = "parse json failed";

		l_strMessage = l_oDeleteClientRegRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiDeleteRegisterRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DeleteClientRegisterRespond [%s]", l_strMessage.c_str());

		return;
	}
	DataBase::SQLRequest l_oSQLQuery;
	l_oSQLQuery.sql_id = "select_icc_t_client_on_off";
	l_oSQLQuery.param["client_id"] = l_oRegisterInfo.m_strClientID;
	l_oSQLQuery.param["client_name"] = l_oRegisterInfo.m_strClientName;

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Client name not exist :[%s]", l_pRSet->GetErrorMsg().c_str());

		l_oDeleteClientRegRespond.m_oHeader.m_strResult = "1";
		l_oDeleteClientRegRespond.m_oHeader.m_strMsg = "Exec sql failed";

		l_strMessage = l_oDeleteClientRegRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiDeleteRegisterRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DeleteClientRegisterRespond [%s]", l_strMessage.c_str());

		return;
	}
	if (l_pRSet->Next() && l_pRSet->RecordSize() > 1)
	{
		for (unsigned int i = 0; i < l_pRSet->RecordSize(); i++)
		{
			std::string l_strOffTime;
			if (i == l_pRSet->RecordSize() - 1)
			{
				l_strOffTime = m_pDateTime->CurrentDateTimeStr();
			}
			else
			{
				l_strOffTime = l_pRSet->GetValue(i + 1, "on_time");
			}
			std::string l_strOnTime = l_pRSet->GetValue(i, "on_time");
			DateTime::CDateTime l_oOnTime = m_pDateTime->FromString(l_strOnTime);
			DateTime::CDateTime l_oOffTime = m_pDateTime->FromString(l_strOffTime);
			DateTime::CDateTime l_iDuringTime = (l_oOffTime - l_oOnTime) / CONVERT_SECOND;
			std::string l_strDuringTime = m_pString->Number((boost::uint64_t)l_iDuringTime);
			std::string strGuid = l_pRSet->GetValue(i, "guid");
			DataBase::SQLRequest l_oSQLUpDate;
			l_oSQLUpDate.sql_id = "update_icc_t_client_on_off";
			l_oSQLUpDate.param["guid"] = strGuid;
			l_oSQLUpDate.set["off_time"] = l_strOffTime;
			l_oSQLUpDate.set["duration"] = l_strDuringTime;
			l_oSQLUpDate.set["update_user"] = "Monitor Server";
			l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

			if (!m_pDBConn->Exec(l_oSQLUpDate)->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "Off Line Failed:[%s]", l_pRSet->GetErrorMsg().c_str());

				l_oDeleteClientRegRespond.m_oHeader.m_strResult = "1";
				l_oDeleteClientRegRespond.m_oHeader.m_strMsg = "Exec Off Line failed";

				l_strMessage = l_oDeleteClientRegRespond.ToString(m_pJsonFac->CreateJson());
				p_pNotifiDeleteRegisterRequest->Response(l_strMessage);
				ICC_LOG_DEBUG(m_pLog, "send DeleteClientRegisterRespond [%s]", l_strMessage.c_str());

				return;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "User Off time recover success:[%s]", l_oRegisterInfo.m_strClientName.c_str());
				SyncClient(l_oRegisterInfo, ICC_CLIENT_LOGOUT,0);
				//l_oDeleteClientRegRespond.m_oBody.m_strResult = "0";

				//if (!_UpdateStatistic(strGuid))
				//{
				//}
			}
		}
	}
	else if (l_pRSet->RecordSize() == 1)
	{
		std::string l_strOnTime = l_pRSet->GetValue(0, "on_time");
		DateTime::CDateTime l_oOnTime = m_pDateTime->FromString(l_strOnTime);
		DateTime::CDateTime l_iDuringTime = (m_pDateTime->CurrentDateTime() - l_oOnTime) / CONVERT_SECOND;

		std::string l_strDuringTime = m_pString->Number((boost::uint64_t)l_iDuringTime);
		std::string strGuid = l_pRSet->GetValue(0, "guid");
		DataBase::SQLRequest l_oSQLUpDate;
		l_oSQLUpDate.sql_id = "update_icc_t_client_on_off";
		l_oSQLUpDate.param["guid"] = strGuid;
		l_oSQLUpDate.set["off_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLUpDate.set["duration"] = l_strDuringTime;
		l_oSQLUpDate.set["update_user"] = "Monitor Server";
		l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

		if (!m_pDBConn->Exec(l_oSQLUpDate)->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Off Line Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
			return;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "User Off Line success:[%s]", l_oRegisterInfo.m_strClientName.c_str());
			SyncClient(l_oRegisterInfo, ICC_CLIENT_LOGOUT,0);
		//	l_oDeleteClientRegRespond.m_oBody.m_strResult = "0";

			//if (!_UpdateStatistic(strGuid))
			//{
			//}
		}

	}
	l_strMessage = l_oDeleteClientRegRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiDeleteRegisterRequest->Response(l_strMessage);	
	ICC_LOG_DEBUG(m_pLog, "send DeleteClientRegisterRespond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetOnOffInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());

	PROTOCOL::CGetOnOffInfo request;
	PROTOCOL::CGetOnOffInfo response;
	
	if (!request.ParseString(l_strRecvMsg, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "[GetOnOffInfo]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetOnOffInfo [%s]", p_pNotifiRequest->GetMessages().c_str());
	
	BuildRespondHeader(response.m_oHeader,request.m_oHeader);

	//查询记录总数
	DataBase::SQLRequest l_tSQLReqCnt;
	l_tSQLReqCnt.sql_id = "select_icc_t_client_on_off_count";

	if (!request.m_oBody.m_oData.m_strClientName.empty())
	{
		l_tSQLReqCnt.param["client_name"] = request.m_oBody.m_oData.m_strClientName;
	}

	l_tSQLReqCnt.param["on_time"] = request.m_oBody.m_oData.m_strOnTime;
	l_tSQLReqCnt.param["off_time"] = request.m_oBody.m_oData.m_strOffTime;

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		std::string l_strMessage = response.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send GetOnOffInfoRespond [%s]", l_strMessage.c_str());
		return;
	}

	response.m_oBody.m_strCount = l_result->GetValue(0, "num");

	DataBase::SQLRequest l_oSQLReq;
	l_oSQLReq.sql_id = "select_icc_t_client_on_off_during";
	if (!request.m_oBody.m_oData.m_strClientName.empty())
	{
		l_oSQLReq.param["client_name"] = request.m_oBody.m_oData.m_strClientName;
	}

	l_oSQLReq.param["on_time"] = request.m_oBody.m_oData.m_strOnTime;
	l_oSQLReq.param["off_time"] = request.m_oBody.m_oData.m_strOffTime;
	l_oSQLReq.param["page_size"] = request.m_oBody.m_oData.m_strPageSize;
	l_oSQLReq.param["page_index"] = request.m_oBody.m_oData.m_strPageIndex;

	DataBase::IResultSetPtr l_pQuery = m_pDBConn->Exec(l_oSQLReq);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pQuery->GetSQL().c_str());
	if (!l_pQuery->IsValid())
	{
		std::string l_strMessage = response.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_ERROR(m_pLog, "Get OnOffInfo Failed:[%s]", l_pQuery->GetErrorMsg().c_str());
		ICC_LOG_DEBUG(m_pLog, "send GetOnOffInfoRespond [%s]", l_strMessage.c_str());
		return;
	}

	if (l_pQuery->RecordSize() == 0)
	{
		std::string l_strMessage = response.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Get OnOffInfo Failed,client_name not exist");
		ICC_LOG_DEBUG(m_pLog, "send GetOnOffInfoRespond [%s]", l_strMessage.c_str());
		return;
	}

	while (l_pQuery->Next())
	{
		PROTOCOL::CGetOnOffInfo::CBody::CData l_oTempData;
		l_oTempData.m_strClientID = l_pQuery->GetValue("client_id");
		l_oTempData.m_strClientName = l_pQuery->GetValue("client_name");
		l_oTempData.m_strOnTime = l_pQuery->GetValue("on_time");
		l_oTempData.m_strOffTime = l_pQuery->GetValue("off_time");
		l_oTempData.m_strDuration = l_pQuery->GetValue("duration");
		response.m_oBody.m_vecData.push_back(l_oTempData);
	}

	std::string l_strMessage = response.ToString(m_pJsonFac->CreateJson());

	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetOnOffInfoRespond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiSetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMessage = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive SetBindIpSeatRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CSetBindIpSeatRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error, Msg:[%s]", l_strMessage.c_str());
		return;
	}

	std::string l_strCode = l_oRequest.m_oHeader.m_strCode;
	std::string l_strIP = l_oRequest.m_oBody.m_strIP;
	std::string l_strSeatNo = l_oRequest.m_oBody.m_strSeatNo;

	PROTOCOL::CSimpleRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	DataBase::SQLRequest l_oSQLQuery;
	l_oSQLQuery.sql_id = "select_icc_t_bind_ip_seat";
	l_oSQLQuery.param["ip"] = l_strIP;
	l_oSQLQuery.param["is_delete"] = "false";
	//l_oSQLQuery.param["seat_no"] = l_strSeatNo;

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLQuery);
	ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Exec select sql failed, sql:[%s], error:[%s]", 
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());

		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Exec sql failed";

		l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send SetBindIpSeatRespond [%s]", l_strMessage.c_str());

		// 返回失败
		return;
	}
	if (l_pRSet->Next())
	{
			DataBase::SQLRequest l_oSQLUpDate;
			l_oSQLUpDate.sql_id = "update_icc_t_bind_ip_seat";
			l_oSQLUpDate.param["ip"] = l_strIP;
			l_oSQLUpDate.set["seat_no"] = l_strSeatNo;
			l_oSQLUpDate.set["update_user"] = l_strCode;
			l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

			DataBase::IResultSetPtr l_pUpdateSet = m_pDBConn->Exec(l_oSQLUpDate);
			if (!l_pUpdateSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "Exec update sql failed, sql:[%s], error:[%s]", 
					l_pUpdateSet->GetSQL().c_str(), l_pUpdateSet->GetErrorMsg().c_str());

				l_oRespond.m_oHeader.m_strResult = "1";
				l_oRespond.m_oHeader.m_strMsg= "Exec sql failed";

				l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
				p_pNotifiRequest->Response(l_strMessage);
				ICC_LOG_DEBUG(m_pLog, "send SetBindIpSeatRespond [%s]", l_strMessage.c_str());

				// 返回失败
				return;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Exec update sql success, sql:[%s]",
					l_pUpdateSet->GetSQL().c_str());
			}
	}
	else
	{
		DataBase::SQLRequest l_oSQLInsert;
		l_oSQLInsert.sql_id = "insert_icc_t_bind_ip_seat";
		l_oSQLInsert.param["guid"] = m_pString->CreateGuid();
		l_oSQLInsert.param["ip"] = l_strIP;
		l_oSQLInsert.param["seat_no"] = l_strSeatNo;
		l_oSQLInsert.param["is_delete"] = "false";
		l_oSQLInsert.param["create_user"] = l_strCode;
		l_oSQLInsert.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pInsertSet = m_pDBConn->Exec(l_oSQLInsert);
		if (!l_pInsertSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Exec insert sql failed, sql:[%s], error:[%s]", l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "Exec sql failed";

			l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
			p_pNotifiRequest->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send SetBindIpSeatRespond [%s]", l_strMessage.c_str());

			return;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "Exec insert sql success, sql:[%s]",
				l_pInsertSet->GetSQL().c_str());
		}
	}

	l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send SetBindIpSeatRespond [%s]", l_strMessage.c_str());
}
void CBusinessImpl::OnCNotifiDeleteBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMessage = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive DeleteBindIpSeatRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CSetBindIpSeatRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error, Msg:[%s]", l_strMessage.c_str());
		return;
	}

	std::string l_strCode = l_oRequest.m_oHeader.m_strCode;
	std::string l_strSeatNo = l_oRequest.m_oBody.m_strSeatNo;

	PROTOCOL::CSimpleRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_bind_ip_seat";
	l_oSQLUpDate.param["seat_no"] = l_strSeatNo;
	l_oSQLUpDate.set["is_delete"] = "true";
	l_oSQLUpDate.set["update_user"] = l_strCode;
	l_oSQLUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pUpdateSet = m_pDBConn->Exec(l_oSQLUpDate);
	if (!l_pUpdateSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Exec update sql failed, sql:[%s], error:[%s]",
			l_pUpdateSet->GetSQL().c_str(), l_pUpdateSet->GetErrorMsg().c_str());

		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Exec sql failed";

		l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DeleteBindIpSeatRespond [%s]", l_strMessage.c_str());

		// 返回失败
		return;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Exec update sql success, sql:[%s]",
			l_pUpdateSet->GetSQL().c_str());
	}

	l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send DeleteBindIpSeatRespond [%s]", l_strMessage.c_str());
}
void CBusinessImpl::OnCNotifiGetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strMessage = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive GetBindIpSeatRequest [%s]", l_strMessage.c_str());
	PROTOCOL::CGetBindIpSeatRequest l_oRequest;
	if (!l_oRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "ParseString Error, Msg:[%s]", l_strMessage.c_str());
		return;
	}

	std::string l_strCode = l_oRequest.m_oHeader.m_strCode;
	std::string l_strIP = l_oRequest.m_oBody.m_strIP;

	PROTOCOL::CGetBindIpSeatRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	DataBase::SQLRequest l_oSQLQuery;
	l_oSQLQuery.sql_id = "select_icc_t_bind_ip_seat";
	l_oSQLQuery.param["ip"] = l_strIP;
	l_oSQLQuery.param["is_delete"] = "false";

	//执行数据库操作
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSQLQuery);
	ICC_LOG_DEBUG(m_pLog, "sql: [%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Exec sql failed, sql :[%s]", l_pRSet->GetErrorMsg().c_str());

		// 返回失败
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Exec sql failed";

		l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send SetBindIpSeatRespond [%s]", l_strMessage.c_str());

		return;
	}

	std::string l_strSeatNo = "";
	if (l_pRSet->Next())
	{
		l_strSeatNo = l_pRSet->GetValue("seat_no");
	}

	l_oRespond.m_oBody.m_strSeatNo = l_strSeatNo;
	l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetBindIpSeatRespond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiSetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiSetInOutInfo)
{
	std::string l_strMessage = p_pNotifiSetInOutInfo->GetMessages();
	PROTOCOL::CSetInOutInfo l_oSetInOutInfo;
	if (!l_oSetInOutInfo.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receice SetInOutInfoRequest [%s]", l_strMessage.c_str());
	if (READY_STATE_IDLE == l_oSetInOutInfo.m_oBody.m_strReadyState)
	{
		// 在席 [4/25/2018 w26326]
		InsertICCClientInOut(l_oSetInOutInfo.m_oBody);
	} 
	else if (READY_STATE_BUSY == l_oSetInOutInfo.m_oBody.m_strReadyState)
	{
		// 离席 [4/25/2018 w26326]
		UpdateICCClientInOut(l_oSetInOutInfo.m_oBody);
	}
}

void CBusinessImpl::OnCNotifiGetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiGetInOutInfo)
{
	std::string l_strMessage = p_pNotifiGetInOutInfo->GetMessages();
	PROTOCOL::CGetInOutInfoRequest l_oClientInOutRequest;
	PROTOCOL::CGetInOutInfoRespond l_oClientInOutRespond;
	if (!l_oClientInOutRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receice GetInOutInfoRequest [%s]", l_strMessage.c_str());

	l_oClientInOutRespond.m_oHeader.m_strCmd = "get_client_inout_respond";
	l_oClientInOutRespond.m_oHeader.m_strMsgId = l_oClientInOutRequest.m_oHeader.m_strMsgId;

	DataBase::SQLRequest l_oSqlCount;
	std::string offset = m_pString->Number(m_pString->ToInt(l_oClientInOutRequest.m_oBody.m_strPageSize) *
		(m_pString->ToInt(l_oClientInOutRequest.m_oBody.m_strPageIndex) - 1));
	l_oSqlCount.sql_id = "select_icc_t_client_in_out_count";
	l_oSqlCount.param["begin_time"] = l_oClientInOutRequest.m_oBody.m_strStartTime;
	l_oSqlCount.param["end_time"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
	l_oSqlCount.param["client_name"] = l_oClientInOutRequest.m_oBody.m_strClientName;

	DataBase::IResultSetPtr l_oResultCount = m_pDBConn->Exec(l_oSqlCount);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResultCount->GetSQL().c_str());

	DataBase::SQLRequest l_oSql;
	l_oSql.sql_id = "select_icc_t_client_in_out_page";
	l_oSql.param["begin_time"] = l_oClientInOutRequest.m_oBody.m_strStartTime;
	l_oSql.param["end_time"] = l_oClientInOutRequest.m_oBody.m_strEndTime;
	l_oSql.param["page_size"] = l_oClientInOutRequest.m_oBody.m_strPageSize;
	l_oSql.param["off_set"] = offset;
	l_oSql.param["client_name"] = l_oClientInOutRequest.m_oBody.m_strClientName;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSql);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	unsigned int l_nSumCount = 0;
	if (l_oResultCount->IsValid() && l_oResult->IsValid())
	{
		while (l_oResult->Next())
		{
			PROTOCOL::CGetInOutInfoRespond::CBody::CData l_oData;
			l_oData.m_strClientID = l_oResult->GetValue("client_id");
			l_oData.m_strClientName = l_oResult->GetValue("client_name");
			l_oData.m_strDuration = l_oResult->GetValue("duration");
			l_oData.m_strInTime = l_oResult->GetValue("login_time");
			l_oData.m_strOutTime = l_oResult->GetValue("logout_time");
			l_oClientInOutRespond.m_oBody.m_vData.push_back(l_oData);
		}
		if (l_oResultCount->Next())
		{
			l_nSumCount = m_pString->ToInt(l_oResultCount->GetValue("count"));
		}
	}
	else
	{
		if (!l_oResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		}
		if (!l_oResultCount->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Failed:[%s]", l_oResultCount->GetErrorMsg().c_str());
		}
	}
	l_oClientInOutRespond.m_oBody.m_strCount = m_pString->Number(l_nSumCount);
	l_strMessage = l_oClientInOutRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiGetInOutInfo->Response(l_strMessage);	
	ICC_LOG_DEBUG(m_pLog, "send GetInOutInfoRespond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::InsertICCClientInOut(PROTOCOL::CSetInOutInfo::CBody l_InOutInfo)
{
	//根据clientId查找clientName
	std::string l_strClientRegisterInfo;
	CRegisterInfo l_oClientRegisterInfo; 
	std::string l_clientName;
	if (m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_InOutInfo.m_strClient, l_strClientRegisterInfo))
	{
		l_oClientRegisterInfo.Parse(l_strClientRegisterInfo, m_pJsonFac->CreateJson());
		l_clientName = l_oClientRegisterInfo.m_strClientName;
	}
	if (l_clientName.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "client is not logined [%s]", l_InOutInfo.m_strClient.c_str());
		return;
	}

	{
		DataBase::SQLRequest l_oSelectICCInfo;
		l_oSelectICCInfo.sql_id = "select_icc_t_client_in_out";
		l_oSelectICCInfo.param["client_id"] = l_InOutInfo.m_strClient;
		l_oSelectICCInfo.param["orderby"] = "create_time desc";

		DataBase::IResultSetPtr l_oSelectResult = m_pDBConn->Exec(l_oSelectICCInfo);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oSelectResult->GetSQL().c_str());
		if (!l_oSelectResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "select failed:db error:[%s]", l_oSelectResult->GetErrorMsg().c_str());
			return;
		}
		std::string l_strGuid;
		if (l_oSelectResult->Next())
		{
			std::string l_strlogoutTime = l_oSelectResult->GetValue("logout_time");
			if (l_strlogoutTime.empty())
			{
				std::string l_strloginTime = l_oSelectResult->GetValue("login_time");
				ICC_LOG_DEBUG(m_pLog, "last loginTime:[%s]", l_strloginTime.c_str());
				return;
			}
			
		}
	}

	DataBase::SQLRequest l_oInsertICCInfo;
	l_oInsertICCInfo.sql_id = "insert_icc_t_client_in_out";
	l_oInsertICCInfo.param["guid"] = m_pString->CreateGuid();
	l_oInsertICCInfo.param["client_id"] = l_InOutInfo.m_strClient;
	l_oInsertICCInfo.param["client_name"] = l_clientName;
	l_oInsertICCInfo.param["login_time"] = l_InOutInfo.m_strTime;
	//l_oInsertICCInfo.param["logout_time"] = l_InOutInfo.m_strTime;
	l_oInsertICCInfo.param["create_user"] = l_clientName;
	l_oInsertICCInfo.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oInsertICCInfo);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Insert failed:DB Error:[%s]", l_oResult->GetErrorMsg().c_str());
	}
}

void CBusinessImpl::UpdateICCClientInOut(PROTOCOL::CSetInOutInfo::CBody l_InOutInfo)
{
	DataBase::SQLRequest l_oSelectICCInfo;
	l_oSelectICCInfo.sql_id = "select_icc_t_client_in_out";
	l_oSelectICCInfo.param["client_id"] = l_InOutInfo.m_strClient;	
	l_oSelectICCInfo.param["orderby"] = "create_time desc limit 1";

	DataBase::IResultSetPtr l_oSelectResult = m_pDBConn->Exec(l_oSelectICCInfo);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oSelectResult->GetSQL().c_str());
	if (!l_oSelectResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select failed:db error:[%s]", l_oSelectResult->GetErrorMsg().c_str());
		return;
	}
	std::string l_strGuid;
	if (!l_oSelectResult->Next())
	{
		ICC_LOG_ERROR(m_pLog, "get inout data failed, client_id[%s]", l_InOutInfo.m_strClient.c_str());
		return;
	}

	l_strGuid = l_oSelectResult->GetValue("guid");

	DateTime::CDateTime l_iBeginTime = m_pDateTime->FromString(l_oSelectResult->GetValue("login_time"));
	DateTime::CDateTime l_iEndTime = m_pDateTime->FromString(l_InOutInfo.m_strTime);

	int l_iDuration = (l_iEndTime - l_iBeginTime) / (1000 * 1000);//微秒转化为秒

	//根据clientId查找clientName
	std::string l_strClientRegisterInfo;
	CRegisterInfo l_oClientRegisterInfo;
	std::string l_clientName;
	if (m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_InOutInfo.m_strClient, l_strClientRegisterInfo))
	{
		l_oClientRegisterInfo.Parse(l_strClientRegisterInfo, m_pJsonFac->CreateJson());
		l_clientName = l_oClientRegisterInfo.m_strClientName;
	}

	DataBase::SQLRequest l_oInsertICCInfo;
	l_oInsertICCInfo.sql_id = "update_icc_t_client_in_out";
	l_oInsertICCInfo.param["guid"] = l_strGuid;
	if (!l_clientName.empty())
	{
		l_oInsertICCInfo.set["client_name"] = l_clientName;
		l_oInsertICCInfo.set["update_user"] = l_clientName;
	}
	l_oInsertICCInfo.set["duration"] = m_pString->Number(l_iDuration);
	l_oInsertICCInfo.set["logout_time"] = l_InOutInfo.m_strTime;
	
	l_oInsertICCInfo.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oInsertICCInfo);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update failed:db error:[%s]", l_oResult->GetErrorMsg().c_str());
	}
}

void CBusinessImpl::_SyncAgentTimeOut(const CRegisterInfo& p_ClientRegisterInfo, const std::string& p_strSeatNo)
{
	PROTOCOL::CAgentTimeoutSync tmp_oAgentTimeOutSync;
	tmp_oAgentTimeOutSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	tmp_oAgentTimeOutSync.m_oHeader.m_strCmd = "agent_timeout_sync";
	tmp_oAgentTimeOutSync.m_oHeader.m_strRequest = "queue_agent_state_sync";
	tmp_oAgentTimeOutSync.m_oHeader.m_strRequestType = MQTYPE_QUEUE;

	tmp_oAgentTimeOutSync.m_oBody.m_strAgent = p_strSeatNo;
	tmp_oAgentTimeOutSync.m_oBody.m_strClientName = p_ClientRegisterInfo.m_strClientName;
	tmp_oAgentTimeOutSync.m_oBody.m_strClientIp = p_ClientRegisterInfo.m_strClientIP;

	std::string l_strMessage(tmp_oAgentTimeOutSync.ToString(m_pJsonFac->CreateJson()));

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send agent timeout sync [%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncClient(CRegisterInfo p_ClientRegisterInfo, eClientStatus p_eState, int p_nLogoutCase)
{
	if (p_ClientRegisterInfo.m_strClientID.empty()
		&& p_ClientRegisterInfo.m_strClientType.empty())
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error NotifiClientSync Client Info Error");
		return;
	}

	std::string l_strMessage;
	PROTOCOL::CClientRegisterSync l_oClientisterSync;

	l_oClientisterSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oClientisterSync.m_oHeader.m_strCmd = "client_register_sync";
	l_oClientisterSync.m_oHeader.m_strRequest = "topic_monitor_sync";
	l_oClientisterSync.m_oHeader.m_strRequestType = MQTYPE_TOPIC;

	std::string l_strClientID = m_pString->Left(p_ClientRegisterInfo.m_strClientID, m_pString->FindIndex(p_ClientRegisterInfo.m_strClientID, ".", 2));

	if (p_ClientRegisterInfo.m_strClientType == CTYPE_ALARM || p_ClientRegisterInfo.m_strClientType == CTYPE_DBAGENT)
	{
		l_oClientisterSync.m_oBody.m_strClientID = l_strClientID;
	}
	else
	{
		l_oClientisterSync.m_oBody.m_strClientID = p_ClientRegisterInfo.m_strClientID;
	}
	l_oClientisterSync.m_oBody.m_strClientName = p_ClientRegisterInfo.m_strClientName;
	l_oClientisterSync.m_oBody.m_strClientType = p_ClientRegisterInfo.m_strClientType;
	l_oClientisterSync.m_oBody.m_strClientIP = p_ClientRegisterInfo.m_strClientIP;


	l_oClientisterSync.m_oBody.m_strStaffCode = p_ClientRegisterInfo.m_strStaffCode;
	l_oClientisterSync.m_oBody.m_strStaffName = p_ClientRegisterInfo.m_strStaffName;
	l_oClientisterSync.m_oBody.m_strDeptCode = p_ClientRegisterInfo.m_strDeptCode;
	l_oClientisterSync.m_oBody.m_strDeptName = p_ClientRegisterInfo.m_strDeptName;
	l_oClientisterSync.m_oBody.m_strDistrictCode = p_ClientRegisterInfo.m_strDistrictCode;
	if (ICC_CLIENT_LOGIN == p_eState)
	{
		l_oClientisterSync.m_oBody.m_strClientStatus = "1";
		SetUserDept(p_ClientRegisterInfo);
		
	}
	else if (ICC_CLIENT_LOGOUT == p_eState)
	{
		PROTOCOL::CSetInOutInfo::CBody l_InOutInfo;
		l_InOutInfo.m_strClient = p_ClientRegisterInfo.m_strClientID;
		l_InOutInfo.m_strTime = m_pDateTime->CurrentDateTimeStr();
		UpdateICCClientInOut(l_InOutInfo);
		l_oClientisterSync.m_oBody.m_strClientStatus = "0";
		l_oClientisterSync.m_oBody.m_strLogoutCase = m_pString->Format("%d", p_nLogoutCase);
		DelUserDept(p_ClientRegisterInfo.m_strClientID);
	}
	l_strMessage = l_oClientisterSync.ToString(m_pJsonFac->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send ClientisterSync [%s]", l_strMessage.c_str());
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

/*
void CBusinessImpl::BuildRespond(PROTOCOL::CClientRegisterRespond& p_oRespond, const PROTOCOL::CClientRegisterRequest& p_oRequest, std::string p_strResult)
{
	p_oRespond.m_oHeader.m_strResult = p_strResult;

	p_oRespond.m_oBody.m_strClientID = p_oRequest.m_oHeader.m_strSeatNo;
	p_oRespond.m_oBody.m_strClientName = p_oRequest.m_oHeader.m_strCode;
	p_oRespond.m_oBody.m_strClientType = p_oRequest.m_oBody.m_strClientType;
}*/
void CBusinessImpl::BuildRespondBody(PROTOCOL::CClientRegisterRespond::CBody& p_oRespondBody, const PROTOCOL::CClientRegisterRequest::CBody& p_oRequestBody,std::string p_strResult)
{
	p_oRespondBody.m_strClientID = p_oRequestBody.m_strClientID;
	p_oRespondBody.m_strClientName = p_oRequestBody.m_strClientName;
	p_oRespondBody.m_strClientType = p_oRequestBody.m_strClientType;
	p_oRespondBody.m_strIP = p_oRequestBody.m_strIP;
	//p_oRespondBody.m_strResult = p_strResult;
}

void CBusinessImpl::OnCNotifiCheckUserRequest(ObserverPattern::INotificationPtr p_pNotifiCheckUserRequest)
{
	std::string l_strMessage = p_pNotifiCheckUserRequest->GetMessages();
	PROTOCOL::CAuthCheckUserRequest l_oCNotifiCheckUserRequest;
	PROTOCOL::CAuthCheckUserRespond l_oCNotifiCheckUserResp;
	if (!l_oCNotifiCheckUserRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request json string failed.");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receive CheckUserRequest [%s]", l_strMessage.c_str());
	BuildRespondHeader(l_oCNotifiCheckUserResp.m_oHeader, l_oCNotifiCheckUserRequest.m_oHeader);
	l_oCNotifiCheckUserResp.m_oBody.m_strResult = RESULT_FAILED;

	std::string l_strUserCode = l_oCNotifiCheckUserRequest.m_oBody.m_strUser_Code;
	std::string l_strPassword = l_oCNotifiCheckUserRequest.m_oBody.m_strUser_Pwd;

	CUserInfo l_oUserInfo;
	if (!GetUserInfo(l_strUserCode, l_oUserInfo))
	{
		ICC_LOG_ERROR(m_pLog, "get user_code[%s] failed", l_strUserCode.c_str());
		l_strMessage = l_oCNotifiCheckUserResp.ToString(m_pJsonFac->CreateJson());
		p_pNotifiCheckUserRequest->Response(l_strMessage);
		return ;
	}

	if (l_oUserInfo.m_strCode == l_strUserCode && l_oUserInfo.m_strPwd == l_strPassword)
	{
		//认证成功
		l_oCNotifiCheckUserResp.m_oBody.m_strResult = RESULT_SUCCESS;
		ICC_LOG_DEBUG(m_pLog, "user_code[%s] auth success", l_strUserCode.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "user_code[%s] auth failed, wrong password", l_strUserCode.c_str());
	}

	l_strMessage = l_oCNotifiCheckUserResp.ToString(m_pJsonFac->CreateJson());
	p_pNotifiCheckUserRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send CheckUserRequest [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiGetUserRequest(ObserverPattern::INotificationPtr p_pNotifiGetUserRequest)
{
	PROTOCOL::CAuthGetUserRequest l_oAuthGetUserRequest;
	PROTOCOL::CAuthGetUserRespond l_oAuthGetUserRespond;
	if (!l_oAuthGetUserRequest.ParseString(p_pNotifiGetUserRequest->GetMessages(), m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request json string failed.");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetUserRequest [%s]", p_pNotifiGetUserRequest->GetMessages().c_str());
	BuildRespondHeader(l_oAuthGetUserRespond.m_oHeader, l_oAuthGetUserRequest.m_oHeader);

	std::map<std::string, std::string> l_mapUserInfo;
	m_pRedisClient->HGetAll(AUTH_USER_INFO, l_mapUserInfo);

	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetStaffRequest] HGetAll complete! msgid = %s", l_oAuthGetUserRequest.m_oHeader.m_strMsgid.c_str());

	CUserInfo l_oUserInfo;
	PROTOCOL::CAuthGetUserRespond::CBody::CData l_oData;
	int count = 0;
	for (auto iter = l_mapUserInfo.begin(); iter != l_mapUserInfo.end(); ++iter)
	{
		if (l_oUserInfo.Parse(iter->second, m_pJsonFac->CreateJson()))
		{
			l_oData.m_strGuid = l_oUserInfo.m_strGuid;
			l_oData.m_strCode = l_oUserInfo.m_strCode;
			l_oData.m_strName = l_oUserInfo.m_strName;
			l_oData.m_strPwd = l_oUserInfo.m_strPwd;
			l_oData.m_strRemark = l_oUserInfo.m_strRemark;
			l_oAuthGetUserRespond.m_oBody.m_vecData.push_back(l_oData);
			++count;
		}

		if (count == MAX_COUNT)
		{
			l_oAuthGetUserRespond.m_oBody.m_strCount = m_pString->Number(l_mapUserInfo.size());
			std::string l_strMessage = l_oAuthGetUserRespond.ToString(m_pJsonFac->CreateJson());

			p_pNotifiGetUserRequest->Response(l_strMessage, true);
			ICC_LOG_DEBUG(m_pLog, "send staff info msg [num:%d][%s]", count, l_strMessage.c_str());

			l_oAuthGetUserRespond.m_oBody.m_vecData.clear();
			count = 0;
		}
	}

	if (count != 0)
	{
		l_oAuthGetUserRespond.m_oBody.m_strCount = m_pString->Number(l_oAuthGetUserRespond.m_oBody.m_vecData.size());
		std::string l_strMessage = l_oAuthGetUserRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiGetUserRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send GetUserRespond [%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::OnCNotifiSetUserRequest(ObserverPattern::INotificationPtr p_pNotifiSetUserRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiSetUserRequest->GetMessages().c_str());

	std::string l_strMessage = p_pNotifiSetUserRequest->GetMessages();
	PROTOCOL::CAuthSetRequest l_oCNotifiSetRequest;
	if (!l_oCNotifiSetRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "p_pNotifiSetRequest is nullptr");
		return;
	}

	PROTOCOL::CSetUserSync l_SetUserSync;
	l_SetUserSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_SetUserSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_SetUserSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_SetUserSync.m_oHeader.m_strCmd = "user_sync";
	l_SetUserSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_SetUserSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_SetUserSync.m_oHeader.m_strRequestType = "1";

	l_SetUserSync.m_oBody.m_strUserGuid = l_oCNotifiSetRequest.m_oBody.m_strUser_Guid;
	l_SetUserSync.m_oBody.m_strUserCode = l_oCNotifiSetRequest.m_oBody.m_strUser_Code;
	l_SetUserSync.m_oBody.m_strUserName = l_oCNotifiSetRequest.m_oBody.m_strUser_Name;
	l_SetUserSync.m_oBody.m_strUserPwd = l_oCNotifiSetRequest.m_oBody.m_strUser_Pwd;
	l_SetUserSync.m_oBody.m_strRemark = l_oCNotifiSetRequest.m_oBody.m_strRemark;
	l_SetUserSync.m_oBody.m_strSyncType = l_oCNotifiSetRequest.m_oBody.m_strSyncType;
	l_SetUserSync.m_oBody.m_strDelete = "false";

	/*if (!m_pRedisClient->HExists(AUTH_USER_INFO, l_oCNotifiSetRequest.m_oBody.m_strUser_Guid))
	{		
		l_SetUserSync.m_oBody.m_strSyncType = "1";
	}
	else
	{		
		l_SetUserSync.m_oBody.m_strSyncType = "2";	
	}*/

	l_strMessage = l_SetUserSync.ToString(m_pJsonFac->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "set SetUserSync [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCNotifiDeleteUserRequest(ObserverPattern::INotificationPtr p_pNotifiDeleteUserRequest)
{
	std::string l_strMessage = p_pNotifiDeleteUserRequest->GetMessages();
	PROTOCOL::CAuthDeleteRequest l_oDeleteUserRequest;

	if (!l_oDeleteUserRequest.ParseString(l_strMessage, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request json string failed.");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "receive DeleteUserRequest [%s]", l_strMessage.c_str());

	if (!m_pRedisClient->HExists(AUTH_USER_INFO, l_oDeleteUserRequest.m_oBody.m_strGuid))
	{
		ICC_LOG_ERROR(m_pLog, "user guid[%s] not exist in redis.", l_oDeleteUserRequest.m_oBody.m_strGuid.c_str());
		return;
	}
	
	PROTOCOL::CSetUserSync l_SetUserSync;
	l_SetUserSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_SetUserSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_SetUserSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_SetUserSync.m_oHeader.m_strCmd = "user_sync";
	l_SetUserSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_SetUserSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_SetUserSync.m_oHeader.m_strRequestType = "1";
	l_SetUserSync.m_oBody.m_strUserGuid = l_oDeleteUserRequest.m_oBody.m_strGuid;
	l_SetUserSync.m_oBody.m_strUserCode = l_oDeleteUserRequest.m_oBody.m_strCode;
	l_SetUserSync.m_oBody.m_strUserName = l_oDeleteUserRequest.m_oBody.m_strName;
	l_SetUserSync.m_oBody.m_strUserPwd = l_oDeleteUserRequest.m_oBody.m_strPwd;
	l_SetUserSync.m_oBody.m_strRemark = l_oDeleteUserRequest.m_oBody.m_strRemark;
	l_SetUserSync.m_oBody.m_strDelete = "true";
	l_SetUserSync.m_oBody.m_strSyncType = "3";
	l_strMessage = l_SetUserSync.ToString(m_pJsonFac->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

	ICC_LOG_DEBUG(m_pLog, "send DeleteUserRespond [%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiLoadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiLoadConfigRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiLoadConfigRequest->GetMessages().c_str());

	std::string l_strMessage;
	PROTOCOL::CLoadConfigRequest l_oLoadConfigRequest;
	PROTOCOL::CLoadConfigRespond l_oLoadConfigRespond;
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	l_oLoadConfigRequest.ParseString(p_pNotifiLoadConfigRequest->GetMessages(), l_pJson);

	l_oLoadConfigRespond.m_oHeader.m_strSystemID = l_oLoadConfigRequest.m_oHeader.m_strSystemID;
	l_oLoadConfigRespond.m_oHeader.m_strSubsystemID = l_oLoadConfigRequest.m_oHeader.m_strSubsystemID;
	l_oLoadConfigRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oLoadConfigRespond.m_oHeader.m_strRelatedID = l_oLoadConfigRequest.m_oHeader.m_strMsgid;
	l_oLoadConfigRespond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oLoadConfigRespond.m_oHeader.m_strCmd = "load_config_respond";
	l_oLoadConfigRespond.m_oHeader.m_strRequest = "";
	l_oLoadConfigRespond.m_oHeader.m_strRequestType = "";
	l_oLoadConfigRespond.m_oHeader.m_strResponse = "";
	l_oLoadConfigRespond.m_oHeader.m_strResponseType = "";

	std::string l_strConfigInfo;
	CConfigInfo l_oConfigInfo;
	m_pRedisClient->HGet(CONFIG_INFO, l_oLoadConfigRequest.m_oBody.m_strIp, l_strConfigInfo);
	if (!l_strConfigInfo.empty())
	{
		if (l_oConfigInfo.Parse(l_strConfigInfo, m_pJsonFac->CreateJson()))
		{
			l_oLoadConfigRespond.m_oBody.m_strIp = l_oLoadConfigRequest.m_oBody.m_strIp;
			l_oLoadConfigRespond.m_oBody.m_strConfig = l_oConfigInfo.m_strConfig;
		}
	}
	else
	{
		l_oLoadConfigRespond.m_oBody.m_strIp = l_oLoadConfigRequest.m_oBody.m_strIp;
		l_oLoadConfigRespond.m_oBody.m_strConfig = "";
		ICC_LOG_ERROR(m_pLog, "serach no result");
	}
	l_strMessage = l_oLoadConfigRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiLoadConfigRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiUploadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiUploadConfigRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiUploadConfigRequest->GetMessages().c_str());

	std::string l_strMessage;
	DataBase::IResultSetPtr l_pResult;
	PROTOCOL::CUploadConfigRequest l_oUploadConfigRequest;
	PROTOCOL::CUploadConfigRespond l_oUploadConfigRespond;
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	l_oUploadConfigRequest.ParseString(p_pNotifiUploadConfigRequest->GetMessages(), l_pJson);

	l_oUploadConfigRespond.m_oHeader.m_strSystemID = l_oUploadConfigRequest.m_oHeader.m_strSystemID;
	l_oUploadConfigRespond.m_oHeader.m_strSubsystemID = l_oUploadConfigRequest.m_oHeader.m_strSubsystemID;
	l_oUploadConfigRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oUploadConfigRespond.m_oHeader.m_strRelatedID = l_oUploadConfigRequest.m_oHeader.m_strMsgid;
	l_oUploadConfigRespond.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oUploadConfigRespond.m_oHeader.m_strCmd = "upload_config_respond";
	l_oUploadConfigRespond.m_oHeader.m_strRequest = "";
	l_oUploadConfigRespond.m_oHeader.m_strRequestType = "";
	l_oUploadConfigRespond.m_oHeader.m_strResponse = "";
	l_oUploadConfigRespond.m_oHeader.m_strResponseType = "";

	// 默认失败
	l_oUploadConfigRespond.m_oBody.m_strResult = "1";
	CConfigInfo l_oConfigInfo;
	if (!m_pRedisClient->HExists(CONFIG_INFO, l_oUploadConfigRequest.m_oBody.m_strIp))
	{

		//insert操作,先判断数据库中是否存在
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_config";
		l_SqlRequest.param["ip"] = l_oUploadConfigRequest.m_oBody.m_strIp;
		l_SqlRequest.param["is_delete"] = "true";
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
			return;
		}
		else if (l_result->RecordSize() > 0)
		{
			if (l_result->Next())
			{
				//恢复标志位false
				DataBase::SQLRequest l_oUpdate;
				l_oUpdate.sql_id = "update_icc_t_config";
				l_oUpdate.param.insert(std::pair<std::string, std::string>("ip", l_oUploadConfigRequest.m_oBody.m_strIp));
				l_oUpdate.set.insert(std::pair<std::string, std::string>("config", l_oUploadConfigRequest.m_oBody.m_strConfig));
				l_oUpdate.set.insert(std::pair<std::string, std::string>("is_delete", "false"));
				l_oUpdate.set.insert(std::pair<std::string, std::string>("update_user", "Server_Config"));
				l_oUpdate.set.insert(std::pair<std::string, std::string>("update_time", m_pDateTime->CurrentDateTimeStr()));
				DataBase::IResultSetPtr Updateptr = m_pDBConn->Exec(l_oUpdate);
				ICC_LOG_INFO(m_pLog, "sql:[%s]", Updateptr->GetSQL().c_str());
				if (!Updateptr->IsValid())
				{
					ICC_LOG_ERROR(m_pLog, "data is invalid,Recover operation failed:[DB Error]:%s", Updateptr->GetErrorMsg().c_str());
					l_oUploadConfigRespond.m_oBody.m_strResult = "1";
					std::string l_strMessage = l_oUploadConfigRespond.ToString(m_pJsonFac->CreateJson());
					p_pNotifiUploadConfigRequest->Response(l_strMessage);

					ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
					return;
				}
				ICC_LOG_DEBUG(m_pLog, "Recover operation success");
				l_oConfigInfo.m_strIp = l_oUploadConfigRequest.m_oBody.m_strIp;
				l_oConfigInfo.m_strConfig = l_oUploadConfigRequest.m_oBody.m_strConfig;

				std::string l_strVal = l_oConfigInfo.ToJson(m_pJsonFac->CreateJson());
				m_pRedisClient->HSet(CONFIG_INFO, l_oUploadConfigRequest.m_oBody.m_strIp, l_strVal);
				l_oUploadConfigRespond.m_oBody.m_strResult = "0";
			}
		}
		else
		{
			DataBase::SQLRequest l_Insert;
			l_Insert.sql_id = "insert_icc_t_config";
			l_Insert.param.insert(std::pair<std::string, std::string>("ip", l_oUploadConfigRequest.m_oBody.m_strIp));
			l_Insert.param.insert(std::pair<std::string, std::string>("config", l_oUploadConfigRequest.m_oBody.m_strConfig));
			l_Insert.param.insert(std::pair<std::string, std::string>("shortcut", ""));
			l_Insert.param.insert(std::pair<std::string, std::string>("is_delete", "false"));
			l_Insert.param.insert(std::pair<std::string, std::string>("level", "0"));
			l_Insert.param.insert(std::pair<std::string, std::string>("sort", "0"));
			l_Insert.param.insert(std::pair<std::string, std::string>("create_user", "Server_Config"));
			l_Insert.param.insert(std::pair<std::string, std::string>("create_time", m_pDateTime->CurrentDateTimeStr()));
			l_pResult = m_pDBConn->Exec(l_Insert);
			ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
			if (l_pResult->IsValid())
			{

				l_oConfigInfo.m_strIp = l_oUploadConfigRequest.m_oBody.m_strIp;
				l_oConfigInfo.m_strConfig = l_oUploadConfigRequest.m_oBody.m_strConfig;

				std::string l_strVal = l_oConfigInfo.ToJson(m_pJsonFac->CreateJson());
				m_pRedisClient->HSet(CONFIG_INFO, l_oUploadConfigRequest.m_oBody.m_strIp, l_strVal);
				l_oUploadConfigRespond.m_oBody.m_strResult = "0";
			}
			else
			{
				l_oUploadConfigRespond.m_oBody.m_strResult = "1";
				ICC_LOG_ERROR(m_pLog, "[Insert Error :[%s]]", l_pResult->GetErrorMsg().c_str());
			}
		}	
	}
	else
	{
		if (l_oUploadConfigRequest.m_oBody.m_strConfig.empty())
		{
			//delete操作,逻辑删除，仅将flag置为0
			DataBase::SQLRequest l_Delete;
			l_Delete.sql_id = "update_icc_t_config";
			l_Delete.param.insert(std::pair<std::string, std::string>("ip", l_oUploadConfigRequest.m_oBody.m_strIp));
			l_Delete.set["is_delete"] = "true";
			l_Delete.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_pResult = m_pDBConn->Exec(l_Delete);
			ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
			if (l_pResult->IsValid())
			{
				m_pRedisClient->HDel(CONFIG_INFO, l_oUploadConfigRequest.m_oBody.m_strIp);
				l_oUploadConfigRespond.m_oBody.m_strResult = "0";
				ICC_LOG_DEBUG(m_pLog, "delete config success");
			}
			else
			{
				l_oUploadConfigRespond.m_oBody.m_strResult = "1";
				ICC_LOG_ERROR(m_pLog, "[Delete Error :[%s]]", l_pResult->GetErrorMsg().c_str());
			}
		}
		else
		{	//update操作
			DataBase::SQLRequest l_Update;
			l_Update.sql_id = "update_icc_t_config";
			l_Update.set["config"] = l_oUploadConfigRequest.m_oBody.m_strConfig;
			l_Update.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			l_Update.param.insert(std::pair<std::string, std::string>("ip", l_oUploadConfigRequest.m_oBody.m_strIp));
			l_pResult = m_pDBConn->Exec(l_Update);
			ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
			if (l_pResult->IsValid())
			{
				CConfigInfo l_oConfigInfo;
				l_oConfigInfo.m_strIp = l_oUploadConfigRequest.m_oBody.m_strIp;
				l_oConfigInfo.m_strConfig = l_oUploadConfigRequest.m_oBody.m_strConfig;

				std::string l_strVal = l_oConfigInfo.ToJson(m_pJsonFac->CreateJson());
				m_pRedisClient->HSet(CONFIG_INFO, l_oUploadConfigRequest.m_oBody.m_strIp, l_strVal);
				l_oUploadConfigRespond.m_oBody.m_strResult = "0";
				ICC_LOG_DEBUG(m_pLog, "update config success");
			}
			else
			{
				l_oUploadConfigRespond.m_oBody.m_strResult = "1";
				ICC_LOG_ERROR(m_pLog, "[Update Error :[%s]]", l_pResult->GetErrorMsg().c_str());
			}
		}
	}
	l_strMessage = l_oUploadConfigRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiUploadConfigRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnGetAllConfigRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiRequest->GetMessages().c_str());

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetAllConfigRespond l_oRespond;
	l_oRespond.m_oHeader.m_strCmd = GET_ALL_CONFIG_RESPOND;
	std::map<std::string, std::string> l_mapConfigInfo;
	m_pRedisClient->HGetAll(CONFIG_INFO, l_mapConfigInfo);
	CConfigInfo l_oConfigInfo;
	PROTOCOL::CGetAllConfigRespond::CData l_oData;
	int l_iCount = 0;
	for (auto iter = l_mapConfigInfo.begin(); iter != l_mapConfigInfo.end(); ++iter)
	{
		if (l_oConfigInfo.Parse(iter->second, l_pIJson))
		{
			l_oData.m_strIP = l_oConfigInfo.m_strIp;
			l_oData.m_strConfigValue = l_oConfigInfo.m_strConfig;
			l_oRespond.m_oBody.m_vData.push_back(l_oData);

			l_iCount++;
		}
		if (l_iCount%MAX_COUNT == 0)
		{
			l_oRespond.m_oBody.m_strCount = std::to_string(l_mapConfigInfo.size());
			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMessage = l_oRespond.ToString(l_pIJson);
			p_pNotifiRequest->Response(l_strMessage,true);
			l_iCount = 0;
			l_oRespond.m_oBody.m_vData.clear();
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
	l_oRespond.m_oBody.m_strCount = std::to_string(l_iCount);
	//l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiUpdateParamRequest(ObserverPattern::INotificationPtr p_pNotifiUpdateParamRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotifiUpdateParamRequest->GetMessages().c_str());

	PROTOCOL::CUpdateParamRequest l_oUpdateParamRequest;
	PROTOCOL::CUpdateParamRespond l_oUpdateParamResp;
	if (!l_oUpdateParamRequest.ParseString(p_pNotifiUpdateParamRequest->GetMessages(), m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "UpdateParamRequest is Invilid [%s]", p_pNotifiUpdateParamRequest->GetMessages().c_str());
		return;
	}
	BuildRespondHeader(l_oUpdateParamResp.m_oHeader, l_oUpdateParamRequest.m_oHeader);
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	std::string l_strName = l_oUpdateParamRequest.m_oBody.m_strName;
	std::string l_strValue = l_oUpdateParamRequest.m_oBody.m_strValue;
	std::string l_strIsVisibility = l_oUpdateParamRequest.m_oBody.m_strIsVisibility;
	std::string l_strIsReadonly = l_oUpdateParamRequest.m_oBody.m_strIsReadonly;
	std::string l_strGuid = m_pString->CreateGuid();

	DataBase::SQLRequest l_oUpdateParam;
	l_oUpdateParam.sql_id = "update_icc_t_param";
	l_oUpdateParam.param["name"] = l_strName;
	if (!l_strValue.empty()) {
		l_oUpdateParam.set["value"] = l_strValue;
	}
	if (!l_strIsVisibility.empty()) {
		l_oUpdateParam.set["is_visibility"] = l_strIsVisibility;
	}
	if (!l_strIsReadonly.empty()) {
		l_oUpdateParam.set["is_readonly"] = l_strIsReadonly;
	}
	l_oUpdateParam.set["update_user"] = "ConfigServer";
	l_oUpdateParam.set["update_time"] = l_strCurrentTime;

	DataBase::IResultSetPtr Updateptr = m_pDBConn->Exec(l_oUpdateParam);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", Updateptr->GetSQL().c_str());
	if (!Updateptr->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,update operation failed");
		l_oUpdateParamResp.m_oBody.m_strResult = "1";
		std::string l_strMessage = l_oUpdateParamResp.ToString(m_pJsonFac->CreateJson());
		p_pNotifiUpdateParamRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		return;
	}

	std::string strParamInfo;
	if (!m_pRedisClient->HGet(Param_INFO, l_strName, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "HGet Param INFO Failed!!!");
		return ;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Param Info failed!!!");
		return ;
	}
	l_ParamInfo.m_strValue = l_strValue;
	l_ParamInfo.m_strIsReadonly = l_strIsReadonly;
	l_ParamInfo.m_strIsVisibility = l_strIsVisibility;
	l_ParamInfo.m_UpdateTime = l_strCurrentTime;
	l_ParamInfo.m_UpdateUser= "ConfigServer";
	std::string strTempInfo = l_ParamInfo.ToJson(l_pJson);
	if (!m_pRedisClient->HSet(Param_INFO, l_strName, strTempInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Update HSet Param INFO Failed!!!");
		return;
	}

	l_oUpdateParamResp.m_oBody.m_strResult = "0";
	//发送同步
	PROTOCOL::CUpdateParamSync l_oUpdateParamSync;
	l_oUpdateParamSync.m_oHeader.m_strSystemID = "ICC";
	l_oUpdateParamSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oUpdateParamSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oUpdateParamSync.m_oHeader.m_strRelatedID = "";
	l_oUpdateParamSync.m_oHeader.m_strSendTime = l_strCurrentTime;
	l_oUpdateParamSync.m_oHeader.m_strCmd = "system_param_sync";
	l_oUpdateParamSync.m_oHeader.m_strRequest = "topic_config_sync";
	l_oUpdateParamSync.m_oHeader.m_strRequestType = "1";
	l_oUpdateParamSync.m_oHeader.m_strResponse = "";
	l_oUpdateParamSync.m_oHeader.m_strResponseType = "";
	l_oUpdateParamSync.m_oBody.m_strName = l_strName;
	l_oUpdateParamSync.m_oBody.m_strValue = l_strValue;
	l_oUpdateParamSync.m_oBody.m_strSyncType = "2";
	l_oUpdateParamSync.m_oBody.l_strIsReadonly = l_strIsReadonly;
	l_oUpdateParamSync.m_oBody.l_strIsVisibility = l_strIsVisibility;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strSyncMessage = l_oUpdateParamSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
	ICC_LOG_DEBUG(m_pLog, "send sync message:[%s]", l_strSyncMessage.c_str());

	std::string l_strMessage = l_oUpdateParamResp.ToString(m_pJsonFac->CreateJson());
	p_pNotifiUpdateParamRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

}

void CBusinessImpl::OnNotifiGetAllParamRequest(ObserverPattern::INotificationPtr p_pNotifiGetAllParamRequest)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetAllParamRequest l_oGetAllParamRequest;
	PROTOCOL::CGetAllParamRespond l_oGetAllParamResp;
	if (!l_oGetAllParamRequest.ParseString(p_pNotifiGetAllParamRequest->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "GetAllParamRequest is Invilid [%s]", p_pNotifiGetAllParamRequest->GetMessages().c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive GetAllParamRequest [%s]", p_pNotifiGetAllParamRequest->GetMessages().c_str());

	BuildRespondHeader(l_oGetAllParamResp.m_oHeader, l_oGetAllParamRequest.m_oHeader);
	DataBase::SQLRequest l_oQuery;
	l_oQuery.sql_id = "select_icc_t_param";
	if (!l_oGetAllParamRequest.m_oBody.m_strName.empty()) {
		l_oQuery.param["name"] = l_oGetAllParamRequest.m_oBody.m_strName;
	}
	if (!l_oGetAllParamRequest.m_oBody.m_strValue.empty()) {
		l_oQuery.param["value"] = l_oGetAllParamRequest.m_oBody.m_strValue;
	}
	if (!l_oGetAllParamRequest.m_oBody.m_strIsVisibility.empty()) {
		l_oQuery.param["is_visibility"] = l_oGetAllParamRequest.m_oBody.m_strIsVisibility;
	}
	if (!l_oGetAllParamRequest.m_oBody.m_strIsReadonly.empty()) {
		l_oQuery.param["is_readonly"] = l_oGetAllParamRequest.m_oBody.m_strIsReadonly;
	}

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		l_oGetAllParamResp.m_oBody.m_strResult = "1";
		std::string l_strMessage = l_oGetAllParamResp.ToString(m_pJsonFac->CreateJson());
		p_pNotifiGetAllParamRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send get all param response [%s]", l_strMessage.c_str());
		return;
	}


	int l_iCount = 0;
	l_oGetAllParamResp.m_oBody.m_strResult = "0";
	while (l_result->Next())
	{
		PROTOCOL::CGetAllParamRespond::CData l_oData;
		l_oData.m_strName = l_result->GetValue("name");
		l_oData.m_strValue = l_result->GetValue("value");
		l_oData.m_strIsVisibility = l_result->GetValue("is_visibility");
		l_oData.m_strIsReadonly = l_result->GetValue("is_readonly");
		l_oData.m_strCreateUser = l_result->GetValue("create_user");
		l_oData.m_strDescription = l_result->GetValue("description");
		l_oGetAllParamResp.m_oBody.m_vData.push_back(l_oData);

		l_iCount++;

		if (l_iCount%MAX_COUNT == 0)
		{
			l_oGetAllParamResp.m_oBody.m_strCount = std::to_string(l_iCount);
			std::string l_strMessage = l_oGetAllParamResp.ToString(m_pJsonFac->CreateJson());
			p_pNotifiGetAllParamRequest->Response(l_strMessage);
			l_iCount = 0;
			l_oGetAllParamResp.m_oBody.m_vData.clear();
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}
	l_oGetAllParamResp.m_oBody.m_strCount = std::to_string(l_iCount);
	std::string l_strMessage = l_oGetAllParamResp.ToString(m_pJsonFac->CreateJson());
	p_pNotifiGetAllParamRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

bool ICC::CBusinessImpl::LoadUserInfo()
{
	CUserInfo l_tAccountInfo;
	l_tAccountInfo.m_strIsDelete = "false";
	DataBase::SQLRequest l_oQuery = ConstructSQLStruct(l_tAccountInfo, "select_icc_t_user");
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	m_pRedisClient->Del(AUTH_USER_INFO);

	//初始化用户信息缓存
	for (unsigned int i = 0; i < l_pResult->RecordSize(); i++)
	{
		CUserInfo l_tAccountInfo;
		l_tAccountInfo.m_strGuid = l_pResult->GetValue(i, "guid");
		l_tAccountInfo.m_strCode = l_pResult->GetValue(i, "code");
		l_tAccountInfo.m_strName = l_pResult->GetValue(i, "name");
		l_tAccountInfo.m_strPwd = l_pResult->GetValue(i, "pwd");
		l_tAccountInfo.m_strIsDelete = l_pResult->GetValue(i, "is_delete");
		l_tAccountInfo.m_strLevel = l_pResult->GetValue(i, "level");
		l_tAccountInfo.m_strSort = l_pResult->GetValue(i, "sort");
		l_tAccountInfo.m_strShortCut = l_pResult->GetValue(i, "shortcut");
		l_tAccountInfo.m_strCreateUser = l_pResult->GetValue(i, "create_user");
		l_tAccountInfo.m_strCreateTime = l_pResult->GetValue(i, "create_time");
		l_tAccountInfo.m_strUpdateUser = l_pResult->GetValue(i, "update_user");
		l_tAccountInfo.m_strUpdateTime = l_pResult->GetValue(i, "update_time");
		l_tAccountInfo.m_strRemark = l_pResult->GetValue(i, "remark");
		//更新Redis缓存
		std::string l_strVal = l_tAccountInfo.ToJson(m_pJsonFac->CreateJson());
		m_pRedisClient->HSet(AUTH_USER_INFO, l_tAccountInfo.m_strGuid, l_strVal);
	}

	ICC_LOG_DEBUG(m_pLog, "load user data success [count:%d]", l_pResult->RecordSize());
	return true;
}

bool CBusinessImpl::InitConfigCache()
{
	DataBase::IResultSetPtr l_pResult;
	DataBase::SQLRequest m_Query;
	m_Query.sql_id = "select_icc_t_config";
	m_Query.param.insert(std::pair<std::string, std::string>("is_delete", "false"));
	l_pResult = m_pDBConn->Exec(m_Query);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "[Initialize configcache failed, Query Error :[%s]]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		unsigned int l_iRowNum = l_pResult->RecordSize();
		for (unsigned int i = 0; i < l_iRowNum; i++)
		{
			CConfigInfo l_oConfigInfo;
			l_oConfigInfo.m_strIp = l_pResult->GetValue(i, "ip");
			l_oConfigInfo.m_strConfig = l_pResult->GetValue(i, "config");

			std::string l_strVal = l_oConfigInfo.ToJson(m_pJsonFac->CreateJson());
			m_pRedisClient->HSet(CONFIG_INFO, l_oConfigInfo.m_strIp, l_strVal);
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Initialize configcache failed, Data Table is empty");
	}
	return true;
}

//查询空闲席位
void CBusinessImpl::OnQueryFreeSeatRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CQueryFreeSeatRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	std::map<std::string, std::string> l_mapItem;
	
	if (!m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem))
	{
		ICC_LOG_ERROR(m_pLog, "HGetALL failed");
	}

	PROTOCOL::CQueryFreeSeatResponse response;
	for (auto var : l_mapItem)
	{
		PROTOCOL::CQueryFreeSeatResponse::CData data;
		CUserDept l_oUserDept;
		if (!l_oUserDept.Parse(var.second, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog,"Parse failed");
		}
		if (l_oUserDept.m_strBuyIdle.compare(READY_STATE_IDLE) == 0)
		{
			int search_type = m_pString->ToInt(l_request.m_oBody.m_type);
			switch (search_type)
			{
				case ASSIGN_SEAT_NOS:
				{
					data.m_seat_no = l_oUserDept.m_strSeatNo;
					response.m_oBody.m_vecData.push_back(data);
					break;
				}
				case ASSIGN_DEPT_CODE:
				{
					if (l_request.m_oBody.m_dept_code.compare(l_oUserDept.m_strDeptCode) == 0)
					{
						data.m_seat_no = l_oUserDept.m_strSeatNo;
						response.m_oBody.m_vecData.push_back(data);
					}
					break;
				}
				case ASSIGN_DISTRICT_CODE:
				{
					if (l_request.m_oBody.m_district_code.compare(l_oUserDept.m_strDeptDistrictCode) == 0)
					{
						data.m_seat_no = l_oUserDept.m_strSeatNo;
						response.m_oBody.m_vecData.push_back(data);
					}
				}
				default:
					break;
			}
		}
	}

	//回复消息
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
}

//查询当前用户绑定的席位
void CBusinessImpl::OnQueryPreUserBindSeat(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CQueryPreSeatBindByUserRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	std::map<std::string, std::string> l_mapItem;
	m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem);
	if (!m_pRedisClient->HGetAll(LOGIN_USER_DEPT, l_mapItem))
	{
		ICC_LOG_ERROR(m_pLog, "HGetALL failed");
	}
	PROTOCOL::CQueryPreSeatBindByUserResponse response;
	for (auto it : l_mapItem)
	{
		CUserDept l_oUserDept;
		l_oUserDept.Parse(it.second, m_pJsonFac->CreateJson());
		if (l_request.m_oBody.m_user.compare(l_oUserDept.m_strUserName)==0)
		{
			response.m_oBody.m_seat_no = l_oUserDept.m_strSeatNo;
		}
	}
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str()); 
}

bool CBusinessImpl::GetParamInfo()
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_param";
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SQLRequest);
	std::vector<std::map<std::string, std::string>> l_vecParamMap;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "resultset is invalid!sql: % s", l_pResult->GetSQL().c_str());
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	std::vector<std::string> vecFiledNames = l_pResult->GetFieldNames();
	while (l_pResult->Next())
	{
		std::map <std::string,std::string> l_ParamMap;
		unsigned int iCount = vecFiledNames.size();
		for (unsigned int i = 0; i < iCount; i++)
		{
			l_ParamMap.insert(std::make_pair(vecFiledNames[i], l_pResult->GetValue(vecFiledNames[i])));
		}
		l_vecParamMap.push_back(l_ParamMap);
	}

	if (!InsertParamInfoToRedis(l_vecParamMap))
	{
		ICC_LOG_ERROR(m_pLog, "Insert Param Info To Redis failed!!!");
		return false;
	}

	return true;
}

bool CBusinessImpl::InsertParamInfoToRedis(std::vector<std::map<std::string, std::string>>& vecParamInfo)
{
	unsigned int iCount = vecParamInfo.size();
	for (unsigned int i = 0; i < iCount; i++)
	{
		PROTOCOL::CParamInfo l_ParamInfo;
		l_ParamInfo.m_strCreateTime = vecParamInfo[i]["create_time"];
		l_ParamInfo.m_strCreateUser = vecParamInfo[i]["create_user"];
		l_ParamInfo.m_strDescription = vecParamInfo[i]["description"];
		l_ParamInfo.m_strIsReadonly = vecParamInfo[i]["is_readonly"];
		l_ParamInfo.m_strIsVisibility = vecParamInfo[i]["is_visibility"];
		l_ParamInfo.m_strName = vecParamInfo[i]["name"];
		l_ParamInfo.m_strValue = vecParamInfo[i]["value"];
		l_ParamInfo.m_UpdateUser = vecParamInfo[i]["update_user"];
		l_ParamInfo.m_UpdateTime = vecParamInfo[i]["update_time"];
		std::string m_strParamInfo = l_ParamInfo.ToJson(m_pJsonFac->CreateJson());
		if (!m_pRedisClient->HSet(Param_INFO, l_ParamInfo.m_strName, m_strParamInfo))
		{
			ICC_LOG_LOWDEBUG(m_pLog, "Hset Param INFO failed");
			return false;
		}
	}
	ICC_LOG_LOWDEBUG(m_pLog, "Insert Param Info To Redis finished");
	return true;
}