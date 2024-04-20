#include "Boost.h"
#include "StaffInfo.h"
#include "DeptInfo.h"
#include "BusinessImpl.h"

#define NOTICE_INFO				"NoticeInfo"

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
	: m_bIsLinkAlive(false)
{
	g_pBusinessImpl = this;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_WEB_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	//m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pTimerMgr = nullptr;
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/Web/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	int iHostCount = m_pConfig->GetNodeCount("ICC/Plugin/Web/FilterDicts", "DictCode");
	for (int i = 0; i < iHostCount; ++i)
	{
		std::string strPath = m_pString->Format("ICC/Plugin/Web/FilterDicts/DictCode[%d]", i);
		std::string strCode = m_pConfig->GetValue(strPath, "");		
		if (!strCode.empty())
		{
			m_mapFilterDicts.insert(std::make_pair(strCode, strCode));
			ICC_LOG_DEBUG(m_pLog, "dict code %s will be filter.", strCode.c_str());
		}
	}

	// 注册观察者
	AddObserverInfo();
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);

	// 清空Redis缓存
	//OnNotifiWebServiceCloseNotify(nullptr);
	//m_pRedisClient->Del(WEB_LOGIN_INFO);

	// 创建链路检测定时器
	//std::string l_strCheckLinkTime = m_pConfig->GetValue("ICC/Plugin/Web/CheckLinkTime","3");
	//unsigned int l_uiCheckLinkTime = m_pString->ToUInt(l_strCheckLinkTime);

	//std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, l_uiCheckLinkTime, 0);

	//m_bStartedTimerFlag = false;

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	// 释放资源
	//m_pRedisClient->Disconnect();
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::AddObserverInfo()
{
#define ADDOBSERVER_EX(RecvCmdName, SendCmdName, HandleFunc) \
{ \
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, RecvCmdName, HandleFunc); \
	m_mapCmdName[RecvCmdName] = SendCmdName; \
}

	ADDOBSERVER_EX("web_check_link_request", "web_check_link_respond", OnNotifiWebCheckLinkRequest);
	ADDOBSERVER_EX("web_login_request", "web_login_respond", OnNotifiWebLoginRequest);
	ADDOBSERVER_EX("web_logout_request", "web_logout_respond", OnNotifiWebLogoutRequest);
	ADDOBSERVER_EX("web_service_close_notify", "", OnNotifiWebServiceCloseNotify);
	ADDOBSERVER_EX("web_get_dict_request", "web_get_dict_respond", OnNotifiWebGetDictRequest);
	ADDOBSERVER_EX("web_get_alarm_request", "web_get_alarm_respond", OnNotifiWebGetAlarmRequest);
	ADDOBSERVER_EX("web_get_alarm_detail_request", "web_get_alarm_detail_respond", OnNotifiWebGetAlarmDetailRequest);
	ADDOBSERVER_EX("web_add_alarm_request", "web_add_alarm_respond", OnNotifiWebAddAlarmRequest);
	ADDOBSERVER_EX("web_update_alarm_request", "web_update_alarm_respond", OnNotifiWebUpdateAlarmRequest);
	ADDOBSERVER_EX("web_alarm_sign_request", "web_alarm_sign_respond", OnNotifiWebSignRequest);
	ADDOBSERVER_EX("web_add_alarm_remark_request", "web_add_alarm_remark_respond", OnNotifiWebAddAlarmRemarkRequest);
	ADDOBSERVER_EX("web_get_alarm_feedback_request", "web_get_alarm_feedback_respond", OnNotifiWebGetAlarmFeedbackRequest);
	ADDOBSERVER_EX("web_add_alarm_feedback_request", "web_add_alarm_feedback_respond", OnNotifiWebAddAlarmFeedbackRequest);
	ADDOBSERVER_EX("web_back_alarm_request", "web_back_alarm_respond", OnNotifiWebBackAlarmRequest);
	ADDOBSERVER_EX("web_search_back_alarm_request", "", OnNotifiWebSearchBackAlarmRequest);
	ADDOBSERVER_EX("get_back_alarm_respond", "", OnNotifiBackAlarmResponse);
	ADDOBSERVER_EX("web_get_dept_request", "web_get_dept_respond", OnNotifiWebGetDeptRequest);
	ADDOBSERVER_EX("dept_sync", "", OnNotifiWebDeptSyncRequest);
	ADDOBSERVER_EX("web_get_staff_request", "web_get_staff_respond", OnNotifiWebGetStaffRequest);
	ADDOBSERVER_EX("staff_sync", "", OnNotifiWebStaffSyncRequest);

	ADDOBSERVER_EX("alarm_sync", "", OnNotifiAlarmSync);
	ADDOBSERVER_EX("alarm_process_sync", "", OnNotifiAlarmProcessSync);
	ADDOBSERVER_EX("alarm_remark_sync", "", OnNotifiAlarmRemarkSync);
	ADDOBSERVER_EX("alarm_feedback_sync", "", OnNotifiAlarmFeedbackSync);
	ADDOBSERVER_EX("alarm_log_sync", "", OnNotifiAlarmLogSync);

	ADDOBSERVER_EX("add_notice_sync", "", OnNotifiAddNoticeRequest);
	ADDOBSERVER_EX("add_urges_sync", "", OnNotifiAddUrgesRequest);
	ADDOBSERVER_EX("web_get_notices_request", "web_get_notices_respond", OnNotifiGetNoticeRequest);

	ADDOBSERVER_EX("sync_server_lock", "", OnNotifiDoubleServerSync);
}

void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive sync lock message. will opt timer!", MODULE_NAME);

	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");

			return;
		}

		std::string l_strServiceState = l_oSyncObj.m_oBody.m_strServiceState;
		if (l_strServiceState.compare("master") == 0)
		{
			if (!m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Active, Start CheckLinkTime TIMER");
				// 创建链路检测定时器

				//OnNotifiWebServiceCloseNotify(nullptr);//屏蔽掉网关启动或者切换清除redis数据

				m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
				std::string l_strCheckLinkTime = m_pConfig->GetValue("ICC/Plugin/Web/CheckLinkTime","3");
				unsigned int l_uiCheckLinkTime = m_pString->ToUInt(l_strCheckLinkTime);

				std::string l_strTimerName = m_pTimerMgr->AddTimer(TIMER_CMD_NAME, l_uiCheckLinkTime, 0);

				m_bStartedTimerFlag = true;
			}
		}
		else
		{
			if (m_bStartedTimerFlag)
			{
				ICC_LOG_DEBUG(m_pLog, "Server Role Sync: Standby, Stop CheckLinkTime TIMER");
				
				m_pTimerMgr = nullptr;

				m_bStartedTimerFlag = false;
			}
		}
		
	}	
}

void CBusinessImpl::OnNotifiWebSearchBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv web search back alarm msg[%s]", l_strRecvMsg.c_str());

	std::string strGuid = m_pString->CreateGuid();

	std::string strTmp = m_pString->Format("{\"msgid\":\"%s\",\"cmd\":\"get_back_alarm_request\",\"request\":\"queue_bills\",\"request_type\":\"0\"}", strGuid.c_str());

	std::string strSendMsg = m_pString->ReplaceFirst(l_strRecvMsg, "{\"cmd\":\"web_search_back_alarm_request\"}", strTmp);
	strSendMsg = m_pString->ReplaceFirst(strSendMsg, "begin_time", "start_time");
	//strSendMsg = m_pString->ReplaceFirst(strSendMsg, "icc_queue_web", "queue_bills");

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strSendMsg, ObserverPattern::ERequestMode::Request_Respond, p_pNotify));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiBackAlarmResponse(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "recv web search back alarm msg[%s]", l_strRecvMsg.c_str());

	std::string strSendMsg = m_pString->ReplaceFirst(l_strRecvMsg, "get_back_alarm_respond", "web_search_back_alarm_respond");
	strSendMsg = m_pString->ReplaceFirst(strSendMsg, "queue_bills", "icc_queue_web");

	p_pNotify->Response(strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiWebCheckLinkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CWebCheckLinkRequest l_oWebCheckLinkRequest;
	if (!l_oWebCheckLinkRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebCheckLinkRespond l_oWebCheckLinkRespond;
	BuildRespondHeader(l_oWebCheckLinkRespond.m_oHeader, l_oWebCheckLinkRequest.m_oHeader);

	std::string l_strSendMsg = l_oWebCheckLinkRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	m_bIsLinkAlive = true;
}

void CBusinessImpl::OnNotifiWebLoginRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebLoginRequest l_oWebLoginRequest;
	if (!l_oWebLoginRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}
	
	// 发送回复消息
	PROTOCOL::CWebLoginRespond l_oWebLoginRespond;
	bool l_bRes = BuildWebLoginRespond(l_oWebLoginRespond, l_oWebLoginRequest);
	std::string l_strSendMsg = l_oWebLoginRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果登录成功
		// 存储Token
		// 在缓存中用response空字段存放请求中的IPList
		l_oWebLoginRespond.m_oHeader.m_strResponse = l_oWebLoginRequest.m_oBody.m_strIPList;
		std::string l_strVal = l_oWebLoginRespond.ToString(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(WEB_LOGIN_INFO, l_oWebLoginRespond.m_oBody.m_strToken, l_strVal))
		{
			ICC_LOG_ERROR(m_pLog, "SetHash fail!");
		}

		// 就发送注册信息到监控服务
		PROTOCOL::CSetClientRegisterRequest l_oSetClientRegisterRequest;
		BuildSetClientRegisterRequest(l_oSetClientRegisterRequest, l_oWebLoginRequest);
		SendRequestMsg(l_oSetClientRegisterRequest.ToString(m_pJsonFty->CreateJson()));
	}
}

void CBusinessImpl::OnNotifiWebLogoutRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebLogoutRequest l_oWebLogoutRequest;
	if (!l_oWebLogoutRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebLogoutRespond l_oWebLogoutRespond;
	bool l_bRes = false;

	if (!l_oWebLogoutRequest.m_oBody.m_strToken.empty())
	{
		//为了处理重复登录，按 usercode 删除已登录的用户
		l_oWebLogoutRequest.m_oBody.m_strUserCode = Token2UserCode(l_oWebLogoutRequest.m_oBody.m_strToken);
		ICC_LOG_DEBUG(m_pLog, "Get user code: [%s] by token", l_oWebLogoutRequest.m_oBody.m_strUserCode.c_str());
	}

	if (l_oWebLogoutRequest.m_oBody.m_strUserCode.empty())
	{
		l_bRes = BuildRespond(l_oWebLogoutRespond.m_oHeader, l_oWebLogoutRespond.m_oBody.m_strResult,
			l_oWebLogoutRequest.m_oHeader, l_oWebLogoutRequest.m_oBody.m_strToken);
	}
	else
	{		
		l_bRes = BuildRespondEx(l_oWebLogoutRespond.m_oHeader, l_oWebLogoutRespond.m_oBody.m_strResult,
			l_oWebLogoutRequest.m_oHeader, l_oWebLogoutRequest.m_oBody.m_strToken, l_oWebLogoutRequest.m_oBody.m_strUserCode);
	}

	std::string l_strSendMsg = l_oWebLogoutRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果退出登录成功
		// 删除缓存
		if (!m_pRedisClient->HDel(WEB_LOGIN_INFO, l_oWebLogoutRequest.m_oBody.m_strToken))
		{
			ICC_LOG_DEBUG(m_pLog, "DeleteHash fail!");
		}
		// 发送取消注册信息到监控服务
		PROTOCOL::CDeleteClientRegisterRequest l_oRequest;
		BuildDeleteClientRegisterRequest(l_oRequest, l_oWebLogoutRequest);
		SendRequestMsg(l_oRequest.ToString(m_pJsonFty->CreateJson()));
	}
}

void CBusinessImpl::OnNotifiWebServiceCloseNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
		// 解析请求消息
		PROTOCOL::CWebServiceCloseNotify l_oWebServiceCloseNotify;
		if (!l_oWebServiceCloseNotify.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			return;
		}
		m_strServerGuid = l_oWebServiceCloseNotify.m_oBody.m_strServerGuid;
	} 
	else
	{
		m_strServerGuid = "__all_server_close__";
	}
	

	std::map<std::string, std::string> l_mapToken;
	if (!m_pRedisClient->HGetAll(WEB_LOGIN_INFO, l_mapToken))
	{
		ICC_LOG_DEBUG(m_pLog, "GetHash fail!");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "HGetAll l_mapToken.size[%d]", l_mapToken.size());

	PROTOCOL::CWebLoginRespond l_oWebLoginRespond;
	auto it = l_mapToken.begin();
	for (; it != l_mapToken.end(); it++)
	{
		if (l_oWebLoginRespond.ParseString(it->second, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_DEBUG(m_pLog, "Parse WebLoginInfo Success![%s]", l_oWebLoginRespond.ToString(m_pJsonFty->CreateJson()).c_str());
			
			// 如果重启Web服务，则m_strServerGuid == "--init"，执行清空所有缓存操作
			if (l_oWebLoginRespond.m_oBody.m_strServerGuid == m_strServerGuid || m_strServerGuid == "__all_server_close__")
			{
				if (!m_pRedisClient->HDel(WEB_LOGIN_INFO, it->first))
				{
					ICC_LOG_DEBUG(m_pLog, "DeleteHash fail!");
				}
				ICC_LOG_DEBUG(m_pLog, "HDel[%s],m_strServerGuid[%s]", (it->first).c_str(), m_strServerGuid.c_str());

				PROTOCOL::CDeleteClientRegisterRequest l_oRequest;
				BuildRequestHeader(l_oRequest.m_oHeader, "delete_client_register_request", "queue_monitor");
				l_oRequest.m_oBody.m_strClientID = Token2UserCode(it->first);
				SendRequestMsg(l_oRequest.ToString(m_pJsonFty->CreateJson()));
			}
		}	
	}
}

void CBusinessImpl::OnNotifiWebGetDictRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebGetDictRequest l_oWebGetDictRequest;
	if (!l_oWebGetDictRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebGetDictRespond l_oWebGetDictRespond;
	BuildWebGetDictRespond(l_oWebGetDictRespond, l_oWebGetDictRequest);
	
	std::string l_strSendMsg = l_oWebGetDictRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiWebGetAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebGetAlarmRequest l_oWebGetAlarmRequest;
	if (!l_oWebGetAlarmRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	ICC_LOG_INFO(m_pLog, "TodayAlarm page_size=%s,page_index=%s", l_oWebGetAlarmRequest.m_oBody.m_strPageSize.c_str(), l_oWebGetAlarmRequest.m_oBody.m_strPageIndex.c_str());

	// 发送回复消息
	PROTOCOL::CWebGetAlarm l_oWebGetAlarmRespond;
	BuildWebGetAlarmRespond(l_oWebGetAlarmRespond, l_oWebGetAlarmRequest);
	
	std::string l_strSendMsg = l_oWebGetAlarmRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

// 根据alarm id获取接警信息和对应的所有处警信息 [8/13/2018 t26150]
void CBusinessImpl::OnNotifiWebGetAlarmDetailRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebGetAlarmDetailRequest l_oWebGetAlarmDetailRequest;
	if (!l_oWebGetAlarmDetailRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse request msg error:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	/*if (l_oWebGetAlarmDetailRequest.m_oBody.m_strID.empty())
	{
		ICC_LOG_WARNING(m_pLog, "return,unkown alarm id in OnNotifiWebGetAlarmDetailRequest:msg_id[%s]",
			l_oWebGetAlarmDetailRequest.m_oHeader.m_strMsgid.c_str());
		return;
	}*/
	
	// 构造回复消息
	PROTOCOL::CWebGetAlarmDetailRespond l_oWebGetAlarmDetailRespond;
	BuildWebGetAlarmDetailRespond(l_oWebGetAlarmDetailRespond, l_oWebGetAlarmDetailRequest);

	std::string l_strSendMsg = l_oWebGetAlarmDetailRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (m_bIsLinkAlive)
	{
		ICC_LOG_DEBUG(m_pLog, "Link is alive");
		m_bIsLinkAlive = false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Link is dead");
		OnNotifiWebServiceCloseNotify(nullptr);
	}	
}

void CBusinessImpl::OnNotifiWebAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAddAlarmRequest l_oWebAddAlarmRequest;
	if (!l_oWebAddAlarmRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebAddAlarmRespond l_oWebAddAlarmRespond;
	bool l_bRes = BuildRespond(l_oWebAddAlarmRespond.m_oHeader, l_oWebAddAlarmRespond.m_oBody.m_strResult, 
		l_oWebAddAlarmRequest.m_oHeader, l_oWebAddAlarmRequest.m_oBody.m_strToken);

	std::string l_strSendMsg = l_oWebAddAlarmRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果Web对接服务新增警情成功
		// 就转发新增警情到警情服务
		BuildWebAddAlarmRequest(l_oWebAddAlarmRequest);
		SendRequestMsg(l_oWebAddAlarmRequest.ToString(m_pJsonFty->CreateJson()));
	}
}
void CBusinessImpl::OnNotifiWebUpdateAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "receive update alarm request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CWebAddAlarmRequest l_oUpdateAlarmRequest;
		if (!l_oUpdateAlarmRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		// 发送回复消息
		PROTOCOL::CWebAddAlarmRespond l_oWebUpdateAlarmRespond;
		bool l_bRes = BuildRespond(l_oWebUpdateAlarmRespond.m_oHeader, l_oWebUpdateAlarmRespond.m_oBody.m_strResult,
			l_oUpdateAlarmRequest.m_oHeader, l_oUpdateAlarmRequest.m_oBody.m_strToken);

		std::string l_strSendMsg = l_oWebUpdateAlarmRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

		if (l_bRes)
		{
			// 就转发修改警情到警情服务
			BuildWebUpdateAlarmRequest(l_oUpdateAlarmRequest);
			SendRequestMsg(l_oUpdateAlarmRequest.ToString(m_pJsonFty->CreateJson()));
		}
	}
}

void CBusinessImpl::OnNotifiWebSignRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebSignRequest l_oWebSignRequest;
	if (!l_oWebSignRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebSignRespond l_oWebSignRespond;
	bool l_bRes = BuildRespond(l_oWebSignRespond.m_oHeader, l_oWebSignRespond.m_oBody.m_strResult,
		l_oWebSignRequest.m_oHeader, l_oWebSignRequest.m_oBody.m_strToken);
	
	std::string l_strSendMsg = l_oWebSignRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果Web对接服务新增签收成功
		// 就转发新增签收到警情服务
		BuildWebSignRequest(l_oWebSignRequest);
		SendRequestMsg(l_oWebSignRequest.ToString(m_pJsonFty->CreateJson()));
	}
}

void CBusinessImpl::OnNotifiWebAddAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAddAlarmRemarkRequest l_oWebAddAlarmRemarkRequest;
	if (!l_oWebAddAlarmRemarkRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebAddAlarmRemarkRespond l_oWebAddAlarmRemarkRespond;
	bool l_bRes = BuildRespond(l_oWebAddAlarmRemarkRespond.m_oHeader, l_oWebAddAlarmRemarkRespond.m_oBody.m_strResult,
		l_oWebAddAlarmRemarkRequest.m_oHeader, l_oWebAddAlarmRemarkRequest.m_oBody.m_strToken);

	std::string l_strSendMsg = l_oWebAddAlarmRemarkRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果Web对接服务新增备注成功
		// 就转发新增备注到警情服务
		BuildWebAddAlarmRemarkRequest(l_oWebAddAlarmRemarkRequest);
		SendRequestMsg(l_oWebAddAlarmRemarkRequest.ToString(m_pJsonFty->CreateJson()));
	}
}

void CBusinessImpl::OnNotifiWebGetAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebGetAlarmFeedBackRequest l_oWebGetAlarmFeedBackRequest;
	if (!l_oWebGetAlarmFeedBackRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebGetAlarmFeedBackRespond l_oWebGetAlarmFeedBackRespond;
	BuildWebGetAlarmFeedbackRespond(l_oWebGetAlarmFeedBackRespond, l_oWebGetAlarmFeedBackRequest);

	std::string l_strSendMsg = l_oWebGetAlarmFeedBackRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiWebAddAlarmFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAddAlarmFeedBackRequest l_oWebAddAlarmFeedBackRequest;
	if (!l_oWebAddAlarmFeedBackRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CWebAddAlarmFeedBackRespond l_oWebAddAlarmFeedBackRespond;
	bool l_bRes = BuildRespond(l_oWebAddAlarmFeedBackRespond.m_oHeader, l_oWebAddAlarmFeedBackRespond.m_oBody.m_strResult,
		l_oWebAddAlarmFeedBackRequest.m_oHeader, l_oWebAddAlarmFeedBackRequest.m_oBody.m_strToken);
	
	std::string l_strSendMsg = l_oWebAddAlarmFeedBackRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{// 如果Web对接服务新增反馈成功
		// 就转发新增反馈到警情服务
		BuildWebAddAlarmFeedBackRequest(l_oWebAddAlarmFeedBackRequest);
		SendRequestMsg(l_oWebAddAlarmFeedBackRequest.ToString(m_pJsonFty->CreateJson()));
	}
}

void CBusinessImpl::OnNotifiWebBackAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebBackAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	
	bool l_bIsSucess(false);
	std::string l_strErrorCode = WEB_ERROR_TOKEN;
	// token检验
	if (CheckToken(l_oRequest.m_oBody.m_strToken))
	{
		// 退单处理
		l_strErrorCode = WEB_ERROR_DB;
		std::string l_strGuid = m_pDBConn->BeginTransaction();
		if (BackAlarmHandle(l_oRequest, l_strGuid))
		{
			l_bIsSucess = true;
		}
		l_bIsSucess ? m_pDBConn->Commit(l_strGuid) : m_pDBConn->Rollback(l_strGuid);
	}	

	// 发送退单申请应答
	PROTOCOL::CWebBackAlarmRespond l_oRespond;
	BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader);
	l_oRespond.m_oBody.m_strResult = (l_bIsSucess ? "0" : l_strErrorCode);
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 如果退单成功，则发送退单申请同步消息
	if ("0" == l_oRespond.m_oBody.m_strResult)
	{
		PROTOCOL::CWebBackAlarmSync l_oSync;
		BuildWebBackAlarmSync(l_oRequest, l_oSync);
		std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiAlarmSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAlarmSync l_oWebAlarmSync;
	if (!l_oWebAlarmSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	BuildRespondHeader(l_oWebAlarmSync.m_oHeader,"web_alarm_sync");
	BuildWebProcessDept(l_oWebAlarmSync.m_oBody.m_oAlarm.m_strID, l_oWebAlarmSync.m_oBody.m_vecProcessDept);	
	
	std::string l_strSendMsg = l_oWebAlarmSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiAlarmProcessSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAlarmProcessSync l_oWebAlarmProcessSync;
	if (!l_oWebAlarmProcessSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	BuildRespondHeader(l_oWebAlarmProcessSync.m_oHeader,"web_alarm_process_sync");
	BuildWebProcessDept(l_oWebAlarmProcessSync.m_oBody.m_oProcess.m_strAlarmID, l_oWebAlarmProcessSync.m_oBody.m_vecProcessDept);
	
	std::string l_strSendMsg = l_oWebAlarmProcessSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiAlarmRemarkSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAlarmRemarkSync l_oWebAlarmRemarkSync;
	if (!l_oWebAlarmRemarkSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	BuildRespondHeader(l_oWebAlarmRemarkSync.m_oHeader,"web_alarm_remark_sync");
	BuildWebProcessDept(l_oWebAlarmRemarkSync.m_oBody.m_oAlarmRemark.m_strAlarmID, l_oWebAlarmRemarkSync.m_oBody.m_vecProcessDept);
	
	std::string l_strSendMsg = l_oWebAlarmRemarkSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiAlarmFeedbackSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CWebAlarmFeedBackSync l_oWebAlarmFeedBackSync;
	if (!l_oWebAlarmFeedBackSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	BuildRespondHeader(l_oWebAlarmFeedBackSync.m_oHeader,"web_alarm_feedback_sync");
	BuildWebProcessDept(l_oWebAlarmFeedBackSync.m_oBody.m_oFeedback.m_strAlarmID, l_oWebAlarmFeedBackSync.m_oBody.m_vecProcessDept);
	
	std::string l_strSendMsg = l_oWebAlarmFeedBackSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiAlarmLogSync(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "Receive AlarmLogSync Message:[%s]", l_strRecvMsg.c_str());

	std::string strSendMsg = m_pString->ReplaceFirst(l_strRecvMsg, "alarm_log_sync", "web_alarm_log_sync");
	strSendMsg = m_pString->ReplaceFirst(strSendMsg, "topic_alarm", "icc_topic_web");
	strSendMsg = m_pString->ReplaceFirst(strSendMsg, "ICC-ApplicationServer", "WEB");

	/*
	// 解析请求消息
	PROTOCOL::CAlarmLogSync l_oWebAlarmLogSync;
	if (!l_oWebAlarmLogSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发同步消息
	BuildRespondHeader(l_oWebAlarmLogSync.m_oHeader, "web_alarm_log_sync");

	std::string l_strSendMsg = l_oWebAlarmLogSync.ToString(m_pJsonFty->CreateJson());*/	


	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "Send AlarmLogSync Message:[%s]", strSendMsg.c_str());
}

void CBusinessImpl::OnNotifiAddNoticeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Add Notice Sync:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CAddNoticeSync l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CAddNoticeSync l_oAddNoticeSync;
		l_oAddNoticeSync.m_oHeader.m_strSystemID = l_oRequest.m_oHeader.m_strSystemID;
		l_oAddNoticeSync.m_oHeader.m_strSubsystemID = l_oRequest.m_oHeader.m_strSubsystemID;
		l_oAddNoticeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oAddNoticeSync.m_oHeader.m_strRelatedID = l_oRequest.m_oHeader.m_strMsgid;
		l_oAddNoticeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAddNoticeSync.m_oHeader.m_strCmd = "web_add_notice_sync";
		l_oAddNoticeSync.m_oHeader.m_strRequest = "icc_topic_web";
		l_oAddNoticeSync.m_oHeader.m_strRequestType = l_oRequest.m_oHeader.m_strRequestType;
		l_oAddNoticeSync.m_oHeader.m_strResponse = "";
		l_oAddNoticeSync.m_oHeader.m_strResponseType = "";

		l_oAddNoticeSync.m_oBody.m_strNoticeId = l_oRequest.m_oHeader.m_strMsgid;
		l_oAddNoticeSync.m_oBody.m_strUserCode = l_oRequest.m_oBody.m_strUserCode;
		l_oAddNoticeSync.m_oBody.m_strUserName = l_oRequest.m_oBody.m_strUserName;
		l_oAddNoticeSync.m_oBody.m_strSeatNo = l_oRequest.m_oBody.m_strSeatNo;
		l_oAddNoticeSync.m_oBody.m_strDeptCode = l_oRequest.m_oBody.m_strDeptCode;
		l_oAddNoticeSync.m_oBody.m_strDeptName = l_oRequest.m_oBody.m_strDeptName;
	//	l_oAddNoticeSync.m_oBody.m_strTargetDeptCode = l_oRequest.m_oBody.m_strTargetDeptCode;
	//	l_oAddNoticeSync.m_oBody.m_strTargetDeptName = l_oRequest.m_oBody.m_strTargetDeptName;
		l_oAddNoticeSync.m_oBody.m_strNoticeType = l_oRequest.m_oBody.m_strNoticeType;
		l_oAddNoticeSync.m_oBody.m_strTitle = l_oRequest.m_oBody.m_strTitle;
		l_oAddNoticeSync.m_oBody.m_strContent = l_oRequest.m_oBody.m_strContent;
	//	l_oAddNoticeSync.m_oBody.m_strTime = l_oRequest.m_oBody.m_strTime;

		std::string l_strSendMsg = l_oAddNoticeSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "Send message:[%s]", l_strSendMsg.c_str());
	}
}
void CBusinessImpl::OnNotifiGetNoticeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Get Notice Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CBaseRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CGetNoticesRespond l_oRespond;
		BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader);

		std::map<std::string, std::string> l_mapNotice;
		if (m_pRedisClient->HGetAll(NOTICE_INFO, l_mapNotice))
		{
			for (auto l_oTemp : l_mapNotice)
			{
				PROTOCOL::CNoticeInfo l_oNoticeInfo;
				if (l_oNoticeInfo.ParseString(l_oTemp.second, m_pJsonFty->CreateJson()))
				{
					PROTOCOL::CGetNoticesRespond::CData l_oData;
					l_oData.m_strNoticeId = l_oNoticeInfo.m_strNoticeId;
					l_oData.m_strUserCode = l_oNoticeInfo.m_strUserCode;
					l_oData.m_strUserName = l_oNoticeInfo.m_strUserName;
					l_oData.m_strSeatNo = l_oNoticeInfo.m_strSeatNo;

					l_oData.m_strDeptCode = l_oNoticeInfo.m_strDeptCode;
					l_oData.m_strDeptName = l_oNoticeInfo.m_strDeptName;
					l_oData.m_strTargetDeptCode = l_oNoticeInfo.m_strTargetDeptCode;
					l_oData.m_strTargetDeptName = l_oNoticeInfo.m_strTargetDeptName;

					l_oData.m_strNoticeType = l_oNoticeInfo.m_strNoticeType;
					l_oData.m_strTitle = l_oNoticeInfo.m_strTitle;
					l_oData.m_strContent = l_oNoticeInfo.m_strContent;
					l_oData.m_strTime = l_oNoticeInfo.m_strTime;

					l_oRespond.m_oBody.m_vecData.push_back(l_oData);
				}
			}

			l_oRespond.m_oBody.m_strCount = std::to_string(l_mapNotice.size());
		}

		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send GetNotice Respond:[%s]", l_strMessage.c_str());
	}
}
void CBusinessImpl::OnNotifiAddUrgesRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Add Urges Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CUrgesRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		PROTOCOL::CUrgesRequest l_oAddUrgesSync;
		l_oAddUrgesSync.m_oHeader.m_strSystemID = l_oRequest.m_oHeader.m_strSystemID;
		l_oAddUrgesSync.m_oHeader.m_strSubsystemID = l_oRequest.m_oHeader.m_strSubsystemID;
		l_oAddUrgesSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oAddUrgesSync.m_oHeader.m_strRelatedID = l_oRequest.m_oHeader.m_strMsgid;
		l_oAddUrgesSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oAddUrgesSync.m_oHeader.m_strCmd = "web_add_urges_sync";
		l_oAddUrgesSync.m_oHeader.m_strRequest = "icc_topic_web";
		l_oAddUrgesSync.m_oHeader.m_strRequestType = l_oRequest.m_oHeader.m_strRequestType;
		l_oAddUrgesSync.m_oHeader.m_strResponse = "";
		l_oAddUrgesSync.m_oHeader.m_strResponseType = "";

		l_oAddUrgesSync.m_oBody.m_strAlarmId = l_oRequest.m_oBody.m_strAlarmId;
		l_oAddUrgesSync.m_oBody.m_strProcessId = l_oRequest.m_oBody.m_strProcessId;
		l_oAddUrgesSync.m_oBody.m_strDispatchDeptCode = l_oRequest.m_oBody.m_strDispatchDeptCode;
		l_oAddUrgesSync.m_oBody.m_strDispatchDeptName = l_oRequest.m_oBody.m_strDispatchDeptName;
		l_oAddUrgesSync.m_oBody.m_strProcessDeptCode = l_oRequest.m_oBody.m_strProcessDeptCode;
		l_oAddUrgesSync.m_oBody.m_strProcessDeptName = l_oRequest.m_oBody.m_strProcessDeptName;
		l_oAddUrgesSync.m_oBody.m_strContent = l_oRequest.m_oBody.m_strContent;

		std::string l_strSendMsg = l_oAddUrgesSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
		ICC_LOG_DEBUG(m_pLog, "Send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiWebGetDeptRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive GetDeptRequest:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetDeptRequest l_oGetDeptReq;
	if (!l_oGetDeptReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse json error! GetDeptRequest msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetDeptRespond l_oGetDeptResp;
	BuildRespondHeader(l_oGetDeptResp.m_oHeader, l_oGetDeptReq.m_oHeader);

	if (!CheckToken(l_oGetDeptReq.m_oBody.m_strToken))
	{
		l_oGetDeptResp.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		l_oGetDeptResp.m_oBody.m_strCount = "0";
		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Get dept info failed, ret msg [%s]", l_strMessage.c_str());

		return;
	}

	std::map<std::string, std::string> l_mapDeptInfo;
	m_pRedisClient->HGetAll(DEPT_INFO_KEY, l_mapDeptInfo);
	/*if (!LoadAllDept(l_mapDeptInfo))
	{
		ICC_LOG_ERROR(m_pLog, "LoadDeptByUser error, user[%s]", l_oGetDeptReq.m_oBody.m_strUserCode.c_str());

		l_oGetDeptResp.m_oBody.m_strResult = WEB_ERROR_DB;
		l_oGetDeptResp.m_oBody.m_strCount = "0";
		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		return;
	}*/

	if (l_mapDeptInfo.size() == 0)
	{
		l_oGetDeptResp.m_oBody.m_strResult = WEB_ERROR_UNKNOWN;
		l_oGetDeptResp.m_oBody.m_strCount = "0";

		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		return;
	}
	else
	{
		PROTOCOL::CGetDeptRespond l_oResTmp;
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

			l_oResTmp.m_oBody.m_vecData.push_back(l_oDept);
		}
		std::sort(l_oResTmp.m_oBody.m_vecData.begin(), l_oResTmp.m_oBody.m_vecData.end(),
			[](const PROTOCOL::CGetDeptRespond::CDept& dept1, const PROTOCOL::CGetDeptRespond::CDept& dept2)
		{
			return atoi(dept1.m_strSort.c_str()) < atoi(dept2.m_strSort.c_str());
		});

		// 构建分包并发送
		int l_nCount = 0;
		for (auto it = l_oResTmp.m_oBody.m_vecData.begin(); it != l_oResTmp.m_oBody.m_vecData.end(); ++it)
		{
			l_oGetDeptResp.m_oBody.m_vecData.push_back(*it);
			++l_nCount;
			/*if (l_nCount == (MAX_COUNT))
			{
				l_oGetDeptResp.m_oBody.m_strResult = "0";
				l_oGetDeptResp.m_oBody.m_strCount = m_pString->Number(l_mapDeptInfo.size());
				std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());

				p_pNotify->Response(l_strMessage, true);
				ICC_LOG_DEBUG(m_pLog, "Send message:[%s]", l_strMessage.c_str());

				l_oGetDeptResp.m_oBody.m_vecData.clear();
				l_nCount = 0;
			}*/
		}

		l_oGetDeptResp.m_oBody.m_strResult = "0";
		l_oGetDeptResp.m_oBody.m_strCount = m_pString->Number(l_mapDeptInfo.size());
		std::string l_strMessage = l_oGetDeptResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send message:[%s]", l_strMessage.c_str());
	}
}
void CBusinessImpl::OnNotifiWebDeptSyncRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive DeptSync Message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CDeptSync l_oDeptSync;
	if (!l_oDeptSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse json error! DeptSync msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	// 转发同步消息
	PROTOCOL::CDeptSync l_oWebDeptSync;
	BuildRespondHeader(l_oWebDeptSync.m_oHeader, "web_dept_sync");
	l_oWebDeptSync.m_oBody.m_strSyncType = l_oDeptSync.m_oBody.m_strSyncType;
	l_oWebDeptSync.m_oBody.m_strGuid = l_oDeptSync.m_oBody.m_strGuid;
	l_oWebDeptSync.m_oBody.m_strParentGuid = l_oDeptSync.m_oBody.m_strParentGuid;
	l_oWebDeptSync.m_oBody.m_strCode = l_oDeptSync.m_oBody.m_strCode;
	l_oWebDeptSync.m_oBody.m_strDistrictCode = l_oDeptSync.m_oBody.m_strDistrictCode;
	l_oWebDeptSync.m_oBody.m_strType = l_oDeptSync.m_oBody.m_strType;
	l_oWebDeptSync.m_oBody.m_strName = l_oDeptSync.m_oBody.m_strName;
	l_oWebDeptSync.m_oBody.m_strPhone = l_oDeptSync.m_oBody.m_strPhone;
	l_oWebDeptSync.m_oBody.m_strShortcut = l_oDeptSync.m_oBody.m_strShortcut;
	l_oWebDeptSync.m_oBody.m_strSort = l_oDeptSync.m_oBody.m_strSort;
	

	std::string l_strSendMsg = l_oWebDeptSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "Send DeptSync Message:[%s]", l_strSendMsg.c_str());
}
void CBusinessImpl::OnNotifiWebGetStaffRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive GetStaffRequest:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetStaffRequest l_oGetStaffReq;
	if (!l_oGetStaffReq.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse json error! GetStaffRequest msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CGetStaffRespond l_oGetStaffResp;
	BuildRespondHeader(l_oGetStaffResp.m_oHeader, l_oGetStaffReq.m_oHeader);

	if (!CheckToken(l_oGetStaffReq.m_oBody.m_strToken))
	{
		l_oGetStaffResp.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		l_oGetStaffResp.m_oBody.m_strCount = "0";

		std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Get staff info failed, msg [%s]", l_strMessage.c_str());

		return;
	}

	std::map<std::string, std::string> l_mapStaffInfo;
	m_pRedisClient->HGetAll(STAFF_INFO_KEY, l_mapStaffInfo);
	ICC_LOG_DEBUG(m_pLog, "HGetAll Staff complete! size = %d, msgid = %s", l_mapStaffInfo.size(), l_oGetStaffReq.m_oHeader.m_strMsgid.c_str());

	if (l_mapStaffInfo.size() == 0)
	{
		l_oGetStaffResp.m_oBody.m_strResult = WEB_ERROR_UNKNOWN;
		l_oGetStaffResp.m_oBody.m_strCount = "0";

		std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
	else
	{
		int l_nCount = 0;
		std::string l_strDeptGuid;
		std::string l_strParentDeptGuid;
		std::string l_strDeptCode = l_oGetStaffReq.m_oBody.m_strDeptCode;
		GetDeptGuid(l_strDeptCode, l_strDeptGuid, l_strParentDeptGuid);

		for (auto it = l_mapStaffInfo.begin(); it != l_mapStaffInfo.end(); it++)
		{
			CStaffInfo l_oStaffInfo;
			JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
			if (l_pJson && l_oStaffInfo.Parse(it->second, l_pJson))
			{
				if (l_strDeptGuid.compare(l_oStaffInfo.m_strDeptGuid) == 0 ||
					l_strParentDeptGuid.compare(l_oStaffInfo.m_strDeptGuid) == 0)
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
					l_oGetStaffResp.m_oBody.m_vecData.push_back(l_oStaff);
					++l_nCount;
				}
			}

			/*if (l_nCount == MAX_COUNT)
			{
				l_oGetStaffResp.m_oBody.m_strResult = "0";
				l_oGetStaffResp.m_oBody.m_strCount = m_pString->Number(l_mapStaffInfo.size());
				std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
				l_oGetStaffResp.m_oBody.m_vecData.clear();
				p_pNotify->Response(l_strMessage, true);
				ICC_LOG_DEBUG(m_pLog, "send staff info msg [num:%d][%s]", l_nCount, l_strMessage.c_str());
				l_nCount = 0;
			}*/
		}

		l_oGetStaffResp.m_oBody.m_strResult = "0";
		l_oGetStaffResp.m_oBody.m_strCount = m_pString->Number(l_oGetStaffResp.m_oBody.m_vecData.size());
		std::string l_strMessage = l_oGetStaffResp.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send staff info msg [num:%d][%s]", l_nCount, l_strMessage.c_str());

	}

	ICC_LOG_DEBUG(m_pLog, "send staff info msg [total:%d]", l_mapStaffInfo.size());
}
void CBusinessImpl::OnNotifiWebStaffSyncRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "Receive StaffSync Message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CStaffSync l_oStaffSync;
	if (!l_oStaffSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse json error! StaffSync msg = %s", p_pNotify->GetMessages().c_str());
		return;
	}

	// 转发同步消息
	PROTOCOL::CStaffSync l_oWebStaffSync;
	BuildRespondHeader(l_oWebStaffSync.m_oHeader, "web_staff_sync");
	l_oWebStaffSync.m_oBody.m_strSyncType = l_oStaffSync.m_oBody.m_strSyncType;
	l_oWebStaffSync.m_oBody.m_strGuid = l_oStaffSync.m_oBody.m_strGuid;
	l_oWebStaffSync.m_oBody.m_strDeptGuid = l_oStaffSync.m_oBody.m_strDeptGuid;
	l_oWebStaffSync.m_oBody.m_strCode = l_oStaffSync.m_oBody.m_strCode;
	l_oWebStaffSync.m_oBody.m_strName = l_oStaffSync.m_oBody.m_strName;
	l_oWebStaffSync.m_oBody.m_strSex = l_oStaffSync.m_oBody.m_strSex;
	l_oWebStaffSync.m_oBody.m_strType = l_oStaffSync.m_oBody.m_strType;
	l_oWebStaffSync.m_oBody.m_strPosition = l_oStaffSync.m_oBody.m_strPosition;
	l_oWebStaffSync.m_oBody.m_strMobile = l_oStaffSync.m_oBody.m_strMobile;
	l_oWebStaffSync.m_oBody.m_strPhone = l_oStaffSync.m_oBody.m_strPhone;
	l_oWebStaffSync.m_oBody.m_strShortcut = l_oStaffSync.m_oBody.m_strShortcut;
	l_oWebStaffSync.m_oBody.m_strSort = l_oStaffSync.m_oBody.m_strSort;
	l_oWebStaffSync.m_oBody.m_strIsLeader = l_oStaffSync.m_oBody.m_strIsLeader;

	std::string l_strSendMsg = l_oWebStaffSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "Send StaffSync Message:[%s]", l_strSendMsg.c_str());
}

std::string CBusinessImpl::GenToken(std::string p_strUserCode)
{
	std::string l_strTime = m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), "%04d%02d%02d%02d%02d%02d%06d");
	return "icc_web_" + p_strUserCode + "_" + l_strTime;
}

std::string CBusinessImpl::Token2UserCode(std::string p_strToken)
{
	std::string l_strPrefix = "icc_web:";
	unsigned int l_uiPostfixLength = 21;		// 格式为_"%04d%02d%02d%02d%02d%02d%06d"的时间串长度
	return p_strToken.substr(l_strPrefix.size(), p_strToken.size() - l_strPrefix.size() - l_uiPostfixLength);
}

bool CBusinessImpl::CkeckIsNotLogin(std::string p_strUserCode, std::string p_strIPList)
{
	std::map<std::string, std::string> l_mapToken;
	if (!m_pRedisClient->HGetAll(WEB_LOGIN_INFO, l_mapToken))
	{
		ICC_LOG_DEBUG(m_pLog, "GetHash fail!");
	}

	std::string l_strToken = "^icc_web_" + p_strUserCode + ".+";
	auto it = l_mapToken.begin();
	for (; it != l_mapToken.end(); it++)
	{
		if (m_pString->RegexMatch(it->first, l_strToken))
		{
			// 经讨论，暂时取消掉该功能
			// 经讨论，又启用该功能(2019-03-06),emm.....
			PROTOCOL::CWebLoginRespond l_oRespond;
			if (!l_oRespond.ParseString(it->second, m_pJsonFty->CreateJson()))
			{// 解析失败
				ICC_LOG_DEBUG(m_pLog, "ParseString Redis Value fail[%s]!",it->second.c_str());
				return true;
			}

			if (0 == l_oRespond.m_oHeader.m_strResponse.compare(p_strIPList))
			{// 相同IP不限制重复登录,单需先清理掉之前登录信息
				if (!m_pRedisClient->HDel(WEB_LOGIN_INFO, it->first))
				{
					ICC_LOG_DEBUG(m_pLog, "DeleteHash fail!");
				}
				ICC_LOG_DEBUG(m_pLog, "same ip login,del befor token");
				return true;
			}
			return false;
		}
	}
	return true;
}

bool CBusinessImpl::CheckToken(std::string p_strToken)
{
	if (!m_pRedisClient->HExists(WEB_LOGIN_INFO, p_strToken))
	{
		ICC_LOG_ERROR(m_pLog, "token auth failed [%s]", p_strToken.c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::CheckUserCode(const std::string& p_strUserCode, std::string& p_strToken)
{
	std::map<std::string, std::string> l_mapToken;
	if (!m_pRedisClient->HGetAll(WEB_LOGIN_INFO, l_mapToken))
	{
		ICC_LOG_DEBUG(m_pLog, "GetHash fail!");
	}

	std::string l_strToken = "^icc_web_" + p_strUserCode + ".+";
	auto it = l_mapToken.begin();
	for (; it != l_mapToken.end(); it++)
	{
		if (m_pString->RegexMatch(it->first, l_strToken))
		{			
			PROTOCOL::CWebLoginRespond l_oRespond;
			if (!l_oRespond.ParseString(it->second, m_pJsonFty->CreateJson()))
			{// 解析失败
				ICC_LOG_WARNING(m_pLog, "ParseString Redis Value fail[%s]!", it->second.c_str());
				continue;
			}

			if (l_oRespond.m_oBody.m_oUser.m_strCode == p_strUserCode)
			{
				p_strToken = it->first;
				ICC_LOG_DEBUG(m_pLog, "Get token [%s] by UserCode [%s]", p_strToken.c_str(), p_strUserCode.c_str());
				return true;
			}			
		}
	}

	return false;
}

bool CBusinessImpl::CheckIP(std::string p_strCode, std::string p_strInputIP)
{
	// 1.获取输入IP
	std::vector<std::string> l_vecInputIP;
	m_pString->Split(p_strInputIP, ",", l_vecInputIP, 1);
	
	// 2.获取用户指定的IP范围
	std::string l_strIPRange;
	std::map<std::string, std::string> l_mapUserInfo;
	m_pRedisClient->HGetAll(USER_INFO, l_mapUserInfo);

	PROTOCOL::CWebGetUserInfo l_oUserInfo;
	for (auto iter = l_mapUserInfo.cbegin(); iter != l_mapUserInfo.cend(); ++iter)
	{
		if (!l_oUserInfo.ParseString(iter->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		if (l_oUserInfo.m_oBody.m_strCode == p_strCode)
		{
			l_strIPRange = l_oUserInfo.m_oBody.m_strRemark;
			break;
		}
	}

	// 3.根据输入IP与指定的IP范围进行鉴权
	if (CheckIP(l_vecInputIP, l_strIPRange))
	{
		return true;
	}

	return false;
}

bool CBusinessImpl::CheckIP(std::vector<std::string> p_vecInputIP, std::string p_strIPRange)
{
	/*
	* p_strIPRange格式为：*			匹配规则：任意IP校验通过
	* p_strIPRange格式为：IP1		匹配规则：指定IP校验通过
	* p_strIPRange格式为：IP1-IP2	匹配规则：在集合[IP1,IP2]范围内的IP校验通过
	*/
	if (p_strIPRange.empty())
	{
		return true;
	}
	else if (!m_pString->Find(p_strIPRange,"-"))
	{
		// IP合法性校验
		if (!m_pString->RegexMatch(p_strIPRange, "^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"))
		{
			ICC_LOG_DEBUG(m_pLog, "IP:[%s] is not legal!", p_strIPRange.c_str());
			return true;
		}			
		// IP匹配规则校验
		for (auto inputIP : p_vecInputIP)
		{
			if (0 == inputIP.compare(p_strIPRange))
			{
				return true;
			}
		}
	}
	else
	{
		// IP合法性校验
		if (!m_pString->RegexMatch(p_strIPRange, "^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))-((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"))
		{
			ICC_LOG_DEBUG(m_pLog, "IP:[%s] is not legal!", p_strIPRange.c_str());
			return true;
		}

		// 范围IP格式化到vector
		std::vector<std::string> l_vecIPRange;
		m_pString->Split(p_strIPRange, "-", l_vecIPRange, 1);
		unsigned int l_uiBeginIP = m_pHelpTool->IpToInt(l_vecIPRange[0]);
		unsigned int l_uiEndIP = m_pHelpTool->IpToInt(l_vecIPRange[1]);
		if (l_uiBeginIP > l_uiEndIP)
		{
			ICC_LOG_DEBUG(m_pLog, "IP:[%s] is not legal!", p_strIPRange.c_str());
			return false;
		}

		// IP匹配规则校验
		for (auto inputIP : p_vecInputIP)
		{
			unsigned int l_uiInputIP = m_pHelpTool->IpToInt(inputIP);
			if ((l_uiInputIP >= l_uiBeginIP) && (l_uiInputIP <= l_uiEndIP))
			{
				return true;
			}
		}
	}
	return false;
}

bool CBusinessImpl::ReadLoginRespondInfo(std::string p_strToken, PROTOCOL::CWebLoginRespond& l_oRespond)
{
	std::string l_strLoginRespondMsg;
	if (!m_pRedisClient->HGet(WEB_LOGIN_INFO, p_strToken, l_strLoginRespondMsg))
	{
		ICC_LOG_DEBUG(m_pLog, "GetHash Fail");
		return false;
	}
	else
	{
		if (!l_oRespond.ParseString(l_strLoginRespondMsg, m_pJsonFty->CreateJson()))
		{
			return false;
		}
	}
	return true;
}

void CBusinessImpl::SendRequestMsg(std::string p_strSendMsg)
{
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", p_strSendMsg.c_str());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(p_strSendMsg, ObserverPattern::ERequestMode::Request_Respond));
}

void CBusinessImpl::BuildRequestHeader(PROTOCOL::CHeader& p_pRequestHeader, std::string p_strCmd, std::string p_strQueueName)
{
	p_pRequestHeader.m_strSystemID = "ICC";
	p_pRequestHeader.m_strSubsystemID = "WEB";
	//p_pRequestHeader.m_strMsgid = p_pRequestHeader.m_strMsgid;
	p_pRequestHeader.m_strRelatedID = "";
	p_pRequestHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRequestHeader.m_strCmd = p_strCmd;
	p_pRequestHeader.m_strRequest = p_strQueueName;
	p_pRequestHeader.m_strRequestType = "0";
	p_pRequestHeader.m_strResponse = "";
	p_pRequestHeader.m_strResponseType = "";
}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader)
{
	p_oRespondHeader.m_strSystemID = p_oRequestHeader.m_strSystemID;
	p_oRespondHeader.m_strSubsystemID = p_oRequestHeader.m_strSubsystemID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = m_mapCmdName[p_oRequestHeader.m_strCmd];
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd)
{
	p_oRespondHeader.m_strSystemID = "ICC";
	p_oRespondHeader.m_strSubsystemID = "WEB";
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	//p_oRespondHeader.m_strRelatedID = p_oRespondHeader.m_strRelatedID;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = p_strCmd;
	p_oRespondHeader.m_strRequest = "icc_topic_web";
	p_oRespondHeader.m_strRequestType = "1";
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

bool CBusinessImpl::BuildRespond(PROTOCOL::CHeader& p_pRequestHeader, std::string& p_strResult, const PROTOCOL::CHeader& p_oRequestHeader, std::string p_strToken)
{
	BuildRespondHeader(p_pRequestHeader, p_oRequestHeader);

	if (!CheckToken(p_strToken))
	{
		p_strResult = WEB_ERROR_TOKEN;
		return false;
	}
	else
	{
		p_strResult = "0";
		return true;
	}
}

bool CBusinessImpl::BuildRespondEx(PROTOCOL::CHeader& p_pRequestHeader, std::string& p_strResult, const PROTOCOL::CHeader& p_oRequestHeader, std::string& p_strToken, std::string p_strUserCode)
{
	BuildRespondHeader(p_pRequestHeader, p_oRequestHeader);

	if (!CheckUserCode(p_strUserCode, p_strToken))
	{
		p_strResult = WEB_ERROR_UNKNOWN;
		return false;
	}
	else
	{
		p_strResult = "0";
		return true;
	}
}

bool CBusinessImpl::GetAlarmInfo(std::string p_strAlarmID, PROTOCOL::CWebGetAlarmRespond::CAlarm &p_oAlarm)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm";
	l_SqlRequest.param["id"] = p_strAlarmID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResult->Next())
	{ // 查询结果为空
		ICC_LOG_DEBUG(m_pLog, "result is null");
		return false;
	}

	// 构建接警单信息
#define GETVALUE_ALARM(DestName,SourceName) p_oAlarm.DestName = l_pResult->GetValue(SourceName);

	GETVALUE_ALARM(m_strID, "id");
	GETVALUE_ALARM(m_strMergeID, "merge_id");
	GETVALUE_ALARM(m_strTitle, "title");
	GETVALUE_ALARM(m_strContent, "content");
	GETVALUE_ALARM(m_strTime, "time");
	GETVALUE_ALARM(m_strActualOccurTime, "actual_occur_time");
	GETVALUE_ALARM(m_strAddr, "addr");
	GETVALUE_ALARM(m_strLongitude, "longitude");
	GETVALUE_ALARM(m_strLatitude, "latitude");
	GETVALUE_ALARM(m_strState, "state");
	GETVALUE_ALARM(m_strLevel, "level");
	GETVALUE_ALARM(m_strSourceType, "source_type");
	GETVALUE_ALARM(m_strSourceID, "source_id");
	GETVALUE_ALARM(m_strHandleType, "handle_type");
	GETVALUE_ALARM(m_strFirstType, "first_type");
	GETVALUE_ALARM(m_strSecondType, "second_type");
	GETVALUE_ALARM(m_strThirdType, "third_type");
	GETVALUE_ALARM(m_strFourthType, "fourth_type");
	GETVALUE_ALARM(m_strVehicleNo, "vehicle_no");
	GETVALUE_ALARM(m_strVehicleType, "vehicle_type");
	GETVALUE_ALARM(m_strSymbolCode, "symbol_code");
	GETVALUE_ALARM(m_strSymbolAddr, "symbol_addr");
	GETVALUE_ALARM(m_strFireBuildingType, "fire_building_type");
	GETVALUE_ALARM(m_strEventType, "event_type");
	GETVALUE_ALARM(m_strCalledNoType, "called_no_type");
	GETVALUE_ALARM(m_strActualCalledNoType, "actual_called_no_type");
	GETVALUE_ALARM(m_strCallerNo, "caller_no");
	GETVALUE_ALARM(m_strCallerName, "caller_name");
	GETVALUE_ALARM(m_strCallerAddr, "caller_addr");
	GETVALUE_ALARM(m_strCallerID, "caller_id");
	GETVALUE_ALARM(m_strCallerIDType, "caller_id_type");
	GETVALUE_ALARM(m_strCallerGender, "caller_gender");
	GETVALUE_ALARM(m_strCallerAge, "caller_age");
	GETVALUE_ALARM(m_strCallerBirthday, "caller_birthday");
	GETVALUE_ALARM(m_strContactNo, "contact_no");
	GETVALUE_ALARM(m_strContactName, "contact_name");
	GETVALUE_ALARM(m_strContactAddr, "contact_addr");
	GETVALUE_ALARM(m_strContactID, "contact_id");
	GETVALUE_ALARM(m_strContactIDType, "contact_id_type");
	GETVALUE_ALARM(m_strContactGender, "contact_gender");
	GETVALUE_ALARM(m_strContactAge, "contact_age");
	GETVALUE_ALARM(m_strContactBirthday, "contact_birthday");
	GETVALUE_ALARM(m_strAdminDeptDistrictCode, "admin_dept_district_code");
	GETVALUE_ALARM(m_strAdminDeptCode, "admin_dept_code");
	GETVALUE_ALARM(m_strAdminDeptName, "admin_dept_name");
	GETVALUE_ALARM(m_strReceiptDeptDistrictCode, "receipt_dept_district_code");
	GETVALUE_ALARM(m_strReceiptDeptCode, "receipt_dept_code");
	GETVALUE_ALARM(m_strReceiptDeptName, "receipt_dept_name");
	GETVALUE_ALARM(m_strLeaderCode, "leader_code");
	GETVALUE_ALARM(m_strLeaderName, "leader_name");
	GETVALUE_ALARM(m_strReceiptCode, "receipt_code");
	GETVALUE_ALARM(m_strReceiptName, "receipt_name");
	GETVALUE_ALARM(m_strDispatchSuggestion, "dispatch_suggestion");

	return true;
}

bool CBusinessImpl::GetProcessInfo(std::string p_strAlarmID, std::string p_strDeptCode, PROTOCOL::CWebGetAlarmRespond::CAlarmProcess &p_oProcess)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_process";
	l_SqlRequest.param["alarm_id"] = p_strAlarmID;
	l_SqlRequest.param["process_dept_code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResult->Next())
	{ // 查询结果为空
		ICC_LOG_DEBUG(m_pLog, "result is null");
		return false;
	}

	// 构建处警单信息
#define GETVALUE_PROCESS(DestName,SourceName) p_oProcess.DestName = l_pResult->GetValue(SourceName);

	GETVALUE_PROCESS(m_strID, "id");
	GETVALUE_PROCESS(m_strAlarmID, "alarm_id");
	GETVALUE_PROCESS(m_strState, "state");
	GETVALUE_PROCESS(m_strTimeEdit, "time_edit");
	GETVALUE_PROCESS(m_strTimeSubmit, "time_submit");
	GETVALUE_PROCESS(m_strTimeArrived, "time_arrived");
	GETVALUE_PROCESS(m_strTimeSigned, "time_signed");
	GETVALUE_PROCESS(m_strTimeFeedBack, "time_feedback");
	GETVALUE_PROCESS(m_strIsNeedFeedback, "is_need_feedback");
	GETVALUE_PROCESS(m_strDispatchDeptDistrictCode, "dispatch_dept_district_code");
	GETVALUE_PROCESS(m_strDispatchDeptCode, "dispatch_dept_code");
	GETVALUE_PROCESS(m_strDispatchDeptName, "dispatch_dept_name");
	GETVALUE_PROCESS(m_strDispatchCode, "dispatch_code");
	GETVALUE_PROCESS(m_strDispatchName, "dispatch_name");
	GETVALUE_PROCESS(m_strDispatchLeaderCode, "dispatch_leader_code");
	GETVALUE_PROCESS(m_strDispatchLeaderName, "dispatch_leader_name");
	GETVALUE_PROCESS(m_strDispatchSuggestion, "dispatch_suggestion");
	GETVALUE_PROCESS(m_strDispatchLeaderInstruction, "dispatch_leader_instruction");
	GETVALUE_PROCESS(m_strProcessDeptDistrictCode, "process_dept_district_code");
	GETVALUE_PROCESS(m_strProcessDeptCode, "process_dept_code");
	GETVALUE_PROCESS(m_strProcessDeptName, "process_dept_name");
	GETVALUE_PROCESS(m_strProcessCode, "process_code");
	GETVALUE_PROCESS(m_strProcessName, "process_name");
	GETVALUE_PROCESS(m_strProcessLeaderCode, "process_leader_code");
	GETVALUE_PROCESS(m_strProcessLeaderName, "process_leader_name");
	GETVALUE_PROCESS(m_strProcessFeedback, "process_feedback");
	GETVALUE_PROCESS(m_strProcessLeaderInstruction, "process_leader_instruction");

	return true;
}

bool CBusinessImpl::GetFeedbackInfo(std::string p_strProcessID, PROTOCOL::CWebGetAlarmFeedBackRespond::CFeedback &p_oFeedback)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_feedback";
	l_SqlRequest.param["process_id"] = p_strProcessID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{// 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResult->Next())
	{ // 查询结果为空
		ICC_LOG_DEBUG(m_pLog, "result is null");
		return false;
	}

	// 构建反馈单信息
#define GETVALUE_FEEDBACK(DestName,SourceName) p_oFeedback.DestName = l_pResult->GetValue(SourceName);

	GETVALUE_FEEDBACK(m_strID, "id");
	GETVALUE_FEEDBACK(m_strAlarmID, "alarm_id");
	GETVALUE_FEEDBACK(m_strProcessID, "process_id");
	GETVALUE_FEEDBACK(m_strResultType, "result_type");
	GETVALUE_FEEDBACK(m_strResultContent, "result_content");
	GETVALUE_FEEDBACK(m_strLeaderInstruction, "leader_instruction");
	GETVALUE_FEEDBACK(m_strState, "state");
	GETVALUE_FEEDBACK(m_strTimeEdit, "time_edit");
	GETVALUE_FEEDBACK(m_strTimeSubmit, "time_submit");
	GETVALUE_FEEDBACK(m_strTimeArrived, "time_arrived");
	GETVALUE_FEEDBACK(m_strTimeSigned, "time_signed");
	GETVALUE_FEEDBACK(m_strTimePoliceDispatch, "time_police_dispatch");
	GETVALUE_FEEDBACK(m_strTimePoliceArrived, "time_police_arrived");
	GETVALUE_FEEDBACK(m_strActualOccurTime, "actual_occur_time");
	GETVALUE_FEEDBACK(m_strActualOccurAddr, "actual_occur_addr");
	GETVALUE_FEEDBACK(m_strFeedbackDeptDistrictCode, "feedback_dept_district_code");
	GETVALUE_FEEDBACK(m_strFeedbackDeptCode, "feedback_dept_code");
	GETVALUE_FEEDBACK(m_strFeedbackDeptName, "feedback_dept_name");
	GETVALUE_FEEDBACK(m_strFeedbackCode, "feedback_code");
	GETVALUE_FEEDBACK(m_strFeedbackName, "feedback_name");
	GETVALUE_FEEDBACK(m_strFeedbackLeaderCode, "feedback_leader_code");
	GETVALUE_FEEDBACK(m_strFeedbackLeaderName, "feedback_leader_name");
	GETVALUE_FEEDBACK(m_strProcessDeptDistrictCode, "process_dept_district_code");
	GETVALUE_FEEDBACK(m_strProcessDeptCode, "process_dept_code");
	GETVALUE_FEEDBACK(m_strProcessDeptName, "process_dept_name");
	GETVALUE_FEEDBACK(m_strProcessCode, "process_code");
	GETVALUE_FEEDBACK(m_strProcessName, "process_name");
	GETVALUE_FEEDBACK(m_strProcessLeaderCode, "process_leader_code");
	GETVALUE_FEEDBACK(m_strProcessLeaderName, "process_leader_name");
	GETVALUE_FEEDBACK(m_strDispatchDeptDistrictCode, "dispatch_dept_district_code");
	GETVALUE_FEEDBACK(m_strDispatchDeptCode, "dispatch_dept_code");
	GETVALUE_FEEDBACK(m_strDispatchDeptName, "dispatch_dept_name");
	GETVALUE_FEEDBACK(m_strDispatchCode, "dispatch_code");
	GETVALUE_FEEDBACK(m_strDispatchName, "dispatch_name");
	GETVALUE_FEEDBACK(m_strDispatchLeaderCode, "dispatch_leader_code");
	GETVALUE_FEEDBACK(m_strDispatchLeaderName, "dispatch_leader_name");
	GETVALUE_FEEDBACK(m_strPersonId, "person_id");
	GETVALUE_FEEDBACK(m_strPersonIdType, "person_id_type");
	GETVALUE_FEEDBACK(m_strPersonNationality, "person_nationality");
	GETVALUE_FEEDBACK(m_strPersonName, "person_name");
	GETVALUE_FEEDBACK(m_strPersonSlaveId, "person_slave_id");
	GETVALUE_FEEDBACK(m_strPersonSlaveIdType, "person_slave_id_type");
	GETVALUE_FEEDBACK(m_strPersonSlaveNationality, "person_slave_nationality");
	GETVALUE_FEEDBACK(m_strPersonSlaveName, "person_slave_name");
	GETVALUE_FEEDBACK(m_strAlarmFirstType, "alarm_first_type");
	GETVALUE_FEEDBACK(m_strAlarmSecondType, "alarm_second_type");
	GETVALUE_FEEDBACK(m_strAlarmThirdType, "alarm_third_type");
	GETVALUE_FEEDBACK(m_strAlarmFourthType, "alarm_fourth_type");
	GETVALUE_FEEDBACK(m_strAlarmAddrDeptName, "alarm_addr_dept_name");
	GETVALUE_FEEDBACK(m_strAlarmAddrFirstType, "alarm_addr_first_type");
	GETVALUE_FEEDBACK(m_strAlarmAddrSecondType, "alarm_addr_second_type");
	GETVALUE_FEEDBACK(m_strAlarmAddrThirdType, "alarm_addr_third_type");
	GETVALUE_FEEDBACK(m_strAlarmLongitude, "alarm_longitude");
	GETVALUE_FEEDBACK(m_strAlarmLatitude, "alarm_latitude");
	GETVALUE_FEEDBACK(m_strAlarmRegionType, "alarm_region_type");
	GETVALUE_FEEDBACK(m_strAlarmLocationType, "alarm_location_type");
	GETVALUE_FEEDBACK(m_strPeopleNumCapture, "people_num_capture");
	GETVALUE_FEEDBACK(m_strPeopleNumRescue, "people_num_rescue");
	GETVALUE_FEEDBACK(m_strPeopleNumSlightInjury, "people_num_slight_injury");
	GETVALUE_FEEDBACK(m_strPeopleNumSeriousInjury, "people_num_serious_injury");
	GETVALUE_FEEDBACK(m_strPeopleNumDeath, "people_num_death");
	GETVALUE_FEEDBACK(m_strPoliceNumDispatch, "police_num_dispatch");
	GETVALUE_FEEDBACK(m_strPoliceCarNumDispatch, "police_car_num_dispatch");
	GETVALUE_FEEDBACK(m_strEconomyLoss, "economy_loss");
	GETVALUE_FEEDBACK(m_strRetrieveEconomyLoss, "retrieve_economy_loss");
	GETVALUE_FEEDBACK(m_strFirePutOutTime, "fire_put_out_time");
	GETVALUE_FEEDBACK(m_strFireBuildingFirstType, "fire_building_first_type");
	GETVALUE_FEEDBACK(m_strFireBuildingSecondType, "fire_building_second_type");
	GETVALUE_FEEDBACK(m_strFireBuildingThirdType, "fire_building_third_type");
	GETVALUE_FEEDBACK(m_strFireSourceType, "fire_source_type");
	GETVALUE_FEEDBACK(m_strFireRegionType, "fire_region_type");
	GETVALUE_FEEDBACK(m_strFireCauseFirstType, "fire_cause_first_type");
	GETVALUE_FEEDBACK(m_strFireCauseSecondType, "fire_cause_second_type");
	GETVALUE_FEEDBACK(m_strFireCauseThirdType, "fire_cause_third_type");
	GETVALUE_FEEDBACK(m_strFireArea, "fire_area");
	GETVALUE_FEEDBACK(m_strTrafficRoadLevel, "traffic_road_level");
	GETVALUE_FEEDBACK(m_strTrafficAccidentLevel, "traffic_accident_level");
	GETVALUE_FEEDBACK(m_strTrafficVehicleNo, "traffic_vehicle_no");
	GETVALUE_FEEDBACK(m_strTrafficVehicleType, "traffic_vehicle_type");
	GETVALUE_FEEDBACK(m_strTrafficSlaveVehicleNo, "traffic_slave_vehicle_no");
	GETVALUE_FEEDBACK(m_strTrafficSlaveVehicleType, "traffic_slave_vehicle_type");
	GETVALUE_FEEDBACK(m_strEventType, "event_type");
	GETVALUE_FEEDBACK(m_strAlarmCalledNoType, "alarm_called_no_type");

	return true;
}

bool CBusinessImpl::BuildWebLoginRespond(PROTOCOL::CWebLoginRespond& p_oRespond, const PROTOCOL::CWebLoginRequest& p_oRequest)
{
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	//1.校验用户名密码
	DataBase::SQLRequest l_SqlCheckUserPwd;
	l_SqlCheckUserPwd.sql_id = "web_login_user_pwd_check";
	l_SqlCheckUserPwd.param["code"] = p_oRequest.m_oBody.m_oUser.m_strCode;
	//l_SqlCheckUserPwd.param["pwd"] = p_oRequest.m_oBody.m_oUser.m_strPwd;

	DataBase::IResultSetPtr l_pResultCheckUserPwd = m_pDBConn->Exec(l_SqlCheckUserPwd);
	ICC_LOG_DEBUG(m_pLog, "web_login_user_pwd_check, sql:[%s]", l_pResultCheckUserPwd->GetSQL().c_str());

	if (!l_pResultCheckUserPwd->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_DB;//表示数据库操作失败
		ICC_LOG_ERROR(m_pLog, "check user info ,exec sql fail[%s]", l_pResultCheckUserPwd->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResultCheckUserPwd->Next())
	{ // 查询结果为空
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_PWD;// 账号密码错误，或者账号所属单位权限错误
		ICC_LOG_DEBUG(m_pLog, "user or password is wrong");
		return false;
	}

	//2.校验用户名单位属性
	DataBase::SQLRequest l_SqlCheckUserDept;
	l_SqlCheckUserDept.sql_id = "web_login_user_dept_check";
	l_SqlCheckUserDept.param["code"] = p_oRequest.m_oBody.m_oUser.m_strCode;

	DataBase::IResultSetPtr l_pResultCheckUserDept = m_pDBConn->Exec(l_SqlCheckUserDept);
	ICC_LOG_DEBUG(m_pLog, "web_login_user_dept_check, sql:[%s]", l_pResultCheckUserDept->GetSQL().c_str());

	if (!l_pResultCheckUserDept->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_DB;//表示数据库操作失败
		ICC_LOG_ERROR(m_pLog, "check user info ,exec sql fail[%s]", l_pResultCheckUserDept->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResultCheckUserDept->Next())
	{ // 查询结果为空
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_PURVIEW;// 账号所属单位权限错误
		ICC_LOG_DEBUG(m_pLog, "user's dept property is wrong");
		return false;
	}

	// 构建登录回复信息
	p_oRespond.m_oBody.m_strResult = "0";//成功
	p_oRespond.m_oBody.m_strToken = GenToken(l_pResultCheckUserPwd->GetValue("user_code"));
	// 获取用户信息
	p_oRespond.m_oBody.m_oUser.m_strCode = l_pResultCheckUserPwd->GetValue("user_code");
	p_oRespond.m_oBody.m_oUser.m_strName = l_pResultCheckUserPwd->GetValue("user_name");
	// 获取绑定警员信息
	p_oRespond.m_oBody.m_oStaff.m_strCode = l_pResultCheckUserPwd->GetValue("staff_code");
	p_oRespond.m_oBody.m_oStaff.m_strName = l_pResultCheckUserPwd->GetValue("staff_name");
	p_oRespond.m_oBody.m_oStaff.m_strSex = l_pResultCheckUserPwd->GetValue("staff_sex");
	p_oRespond.m_oBody.m_oStaff.m_strType = l_pResultCheckUserPwd->GetValue("staff_type");
	p_oRespond.m_oBody.m_oStaff.m_strPosition = l_pResultCheckUserPwd->GetValue("staff_position");
	p_oRespond.m_oBody.m_oStaff.m_strMobile = l_pResultCheckUserPwd->GetValue("staff_mobile");
	p_oRespond.m_oBody.m_oStaff.m_strPhone = l_pResultCheckUserPwd->GetValue("staff_phone");
	// 获取该警员所在单位信息
	p_oRespond.m_oBody.m_oDept.m_strCode = l_pResultCheckUserPwd->GetValue("dept_code");
	p_oRespond.m_oBody.m_oDept.m_strName = l_pResultCheckUserPwd->GetValue("dept_name");
	p_oRespond.m_oBody.m_oDept.m_strDistrictCode = l_pResultCheckUserPwd->GetValue("dept_district_code");
	p_oRespond.m_oBody.m_oDept.m_strType = l_pResultCheckUserPwd->GetValue("dept_type");
	p_oRespond.m_oBody.m_oDept.m_strPhone = l_pResultCheckUserPwd->GetValue("dept_phone");
	p_oRespond.m_oBody.m_oDept.m_strPhone = l_pResultCheckUserPwd->GetValue("dept_phone");
	p_oRespond.m_oBody.m_strServerGuid = p_oRequest.m_oBody.m_strServerGuid;
	
	//////////////////////////////////////////////////////////////////////////
	// 旧版本：流程设计不允许重复登录
	// 新版本：流程设计为，相同机器登录则顶掉
	if (!CkeckIsNotLogin(p_oRequest.m_oBody.m_oUser.m_strCode, p_oRequest.m_oBody.m_strIPList))
	{
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_REPEATLOGIN;//该账户已在别处登录
		ICC_LOG_DEBUG(m_pLog, "[%s] repeat login", p_oRequest.m_oBody.m_oUser.m_strCode.c_str());
		return false;
	}

	// IP检验
	std::string l_strCheckIP = m_pConfig->GetValue("ICC/Plugin/Web/IsCheckIP", "0");
	if ("1" == l_strCheckIP)
	{
		if (!CheckIP(p_oRequest.m_oBody.m_oUser.m_strCode, p_oRequest.m_oBody.m_strIPList))
		{
			p_oRespond.m_oBody.m_strResult = WEB_ERROR_IPCHECK;//IP校验失败
			ICC_LOG_DEBUG(m_pLog, "CheckIP [%s] fail!", p_oRequest.m_oBody.m_strIPList.c_str());
			return false;
		}
	}

	return true;
}

void CBusinessImpl::BuildSetClientRegisterRequest(PROTOCOL::CSetClientRegisterRequest& p_oRequest, const PROTOCOL::CWebLoginRequest& p_oWebLoginRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "set_client_register_request", "queue_monitor");
	p_oRequest.m_oBody.m_strClientID = p_oWebLoginRequest.m_oBody.m_oUser.m_strCode;
	p_oRequest.m_oBody.m_strClientName = p_oWebLoginRequest.m_oBody.m_oUser.m_strCode;
	p_oRequest.m_oBody.m_strClientType = "ctype_client_web";
	p_oRequest.m_oBody.m_strClientIP = p_oWebLoginRequest.m_oBody.m_strIPList;
}

void CBusinessImpl::BuildDeleteClientRegisterRequest(PROTOCOL::CDeleteClientRegisterRequest& p_oRequest, const PROTOCOL::CWebLogoutRequest& p_oWebLogoutRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "delete_client_register_request", "queue_monitor");
	p_oRequest.m_oBody.m_strClientID = Token2UserCode(p_oWebLogoutRequest.m_oBody.m_strToken);
}

bool CBusinessImpl::BuildWebGetDictRespond(PROTOCOL::CWebGetDictRespond& p_oRespond, const PROTOCOL::CWebGetDictRequest& p_oRequest)
{
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!CheckToken(p_oRequest.m_oBody.m_strToken))
	{// 校验授权码失败
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_web_dict_info";
	l_SqlRequest.param["lang_code"] = p_oRequest.m_oBody.m_strLangCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_DB;
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		PROTOCOL::CWebGetDictRespond::CDict l_oDict;
		// 获取字典信息
		l_oDict.m_strCode = l_pResult->GetValue("code");
		l_oDict.m_strParentCode = l_pResult->GetValue("parent_code");
		l_oDict.m_strValue = l_pResult->GetValue("value");

		if (m_mapFilterDicts.find(l_oDict.m_strCode) == m_mapFilterDicts.end() && m_mapFilterDicts.find(l_oDict.m_strParentCode) == m_mapFilterDicts.end())
		{
			p_oRespond.m_oBody.m_vecDict.push_back(l_oDict);
		}		
	}
	p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

void CBusinessImpl::BuildWebGetAlarmRespond(PROTOCOL::CWebGetAlarm& p_oRespond, const PROTOCOL::CWebGetAlarmRequest& p_oRequest)
{
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	p_oRespond.m_oBody.m_strStateID = p_oRequest.m_oBody.m_strStateID;
	p_oRespond.m_oBody.m_strResult = WEB_ERROR_UNKNOWN;
	if (!CheckToken(p_oRequest.m_oBody.m_strToken))
	{
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		return ;
	}

	std::string l_strLoginRespondMsg;
	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);	

	if (!p_oRequest.m_oBody.m_strPageSize.empty() && !p_oRequest.m_oBody.m_strPageIndex.empty())
	{
		//记录总数
		DataBase::SQLRequest l_tSQLReqCnt;
		l_tSQLReqCnt.sql_id = SELECT_WEB_ALARM_COUNT;
		l_tSQLReqCnt.param["process_dept_code"] = l_oRespond.m_oBody.m_oDept.m_strCode;
		//当前页记录
		DataBase::SQLRequest l_tSQLRecord;
		l_tSQLRecord.sql_id = SELECT_WEB_ALARM;
		l_tSQLRecord.param["page_size"] = p_oRequest.m_oBody.m_strPageSize;
		l_tSQLRecord.param["page_index"] = p_oRequest.m_oBody.m_strPageIndex;
		l_tSQLRecord.param["process_dept_code"] = l_oRespond.m_oBody.m_oDept.m_strCode;
		if (!p_oRequest.m_oBody.m_strBeginTime.empty() && !p_oRequest.m_oBody.m_strEndTime.empty())
		{
			l_tSQLReqCnt.param["begin_time"] = p_oRequest.m_oBody.m_strBeginTime;
			l_tSQLReqCnt.param["end_time"] = p_oRequest.m_oBody.m_strEndTime;

			l_tSQLRecord.param["begin_time"] = p_oRequest.m_oBody.m_strBeginTime;
			l_tSQLRecord.param["end_time"] = p_oRequest.m_oBody.m_strEndTime;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strId.empty())
		{
			l_tSQLReqCnt.param["id"] = p_oRequest.m_oBody.m_oAlarm.m_strId;
			l_tSQLRecord.param["id"] = p_oRequest.m_oBody.m_oAlarm.m_strId;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strContent.empty())
		{
			l_tSQLReqCnt.param["content"] = p_oRequest.m_oBody.m_oAlarm.m_strContent;
			l_tSQLRecord.param["content"] = p_oRequest.m_oBody.m_oAlarm.m_strContent;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strTime.empty())
		{
			l_tSQLReqCnt.param["time"] = p_oRequest.m_oBody.m_oAlarm.m_strTime;
			l_tSQLRecord.param["time"] = p_oRequest.m_oBody.m_oAlarm.m_strTime;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strAddr.empty())
		{
			l_tSQLReqCnt.param["addr"] = p_oRequest.m_oBody.m_oAlarm.m_strAddr;
			l_tSQLRecord.param["addr"] = p_oRequest.m_oBody.m_oAlarm.m_strAddr;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strState.empty())
		{
			l_tSQLReqCnt.param["state"] = p_oRequest.m_oBody.m_oAlarm.m_strState;
			l_tSQLRecord.param["state"] = p_oRequest.m_oBody.m_oAlarm.m_strState;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strFirstType.empty())
		{
			l_tSQLReqCnt.param["first_type"] = p_oRequest.m_oBody.m_oAlarm.m_strFirstType;
			l_tSQLRecord.param["first_type"] = p_oRequest.m_oBody.m_oAlarm.m_strFirstType;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strSecondType.empty())
		{
			l_tSQLReqCnt.param["second_type"] = p_oRequest.m_oBody.m_oAlarm.m_strSecondType;
			l_tSQLRecord.param["second_type"] = p_oRequest.m_oBody.m_oAlarm.m_strSecondType;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strThirdType.empty())
		{
			l_tSQLReqCnt.param["third_type"] = p_oRequest.m_oBody.m_oAlarm.m_strThirdType;
			l_tSQLRecord.param["third_type"] = p_oRequest.m_oBody.m_oAlarm.m_strThirdType;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strFourthType.empty())
		{
			l_tSQLReqCnt.param["fourth_type"] = p_oRequest.m_oBody.m_oAlarm.m_strFourthType;
			l_tSQLRecord.param["fourth_type"] = p_oRequest.m_oBody.m_oAlarm.m_strFourthType;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strContactNo.empty())
		{
			l_tSQLReqCnt.param["contact_no"] = p_oRequest.m_oBody.m_oAlarm.m_strContactNo;
			l_tSQLRecord.param["contact_no"] = p_oRequest.m_oBody.m_oAlarm.m_strContactNo;
		}
		if (!p_oRequest.m_oBody.m_oAlarm.m_strContactName.empty())
		{
			l_tSQLReqCnt.param["contact_name"] = p_oRequest.m_oBody.m_oAlarm.m_strContactName;
			l_tSQLRecord.param["contact_name"] = p_oRequest.m_oBody.m_oAlarm.m_strContactName;
		}
		if (!p_oRequest.m_oBody.m_strStateID.compare("1"))//未签收或者未反馈警单
		{
			l_tSQLReqCnt.param["signed_or_feedback_is_null"] = "TRUE";
			l_tSQLRecord.param["signed_or_feedback_is_null"] = "TRUE";
		}
		if (!p_oRequest.m_oBody.m_strStateID.compare("2"))//除去未签收/未反馈的所有警单
		{
			l_tSQLReqCnt.param["signed_and_feedback_is_not_null"] = "TRUE";
			l_tSQLRecord.param["signed_and_feedback_is_not_null"] = "TRUE";
		}
		//执行查询记录总数sql
		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt);
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

			return;
		}
		ICC_LOG_DEBUG(m_pLog, "ExecQuery alarm count success ,SQL :[%s]", l_result->GetSQL().c_str());
		p_oRespond.m_oBody.m_strTotalCount = l_result->GetValue(0, "num");
		//执行查询当前页记录sql
		l_result = m_pDBConn->Exec(l_tSQLRecord);
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());

			return;
		}
		ICC_LOG_DEBUG(m_pLog, "ExecQuery alarm success ,SQL :[%s]", l_result->GetSQL().c_str());
		// 查询成功
		while (l_result->Next())
		{
			PROTOCOL::CWebGetAlarm::CAlarm l_oAlarm;
			// 当前页记录信息
			l_oAlarm.m_strID = l_result->GetValue("id");
			l_oAlarm.m_strContent = l_result->GetValue("content");
			l_oAlarm.m_strTime = l_result->GetValue("time");
			l_oAlarm.m_strAddr = l_result->GetValue("addr");
			l_oAlarm.m_strState = l_result->GetValue("state");
			l_oAlarm.m_strLevel = l_result->GetValue("level");
			l_oAlarm.m_strFirstType = l_result->GetValue("first_type");
			l_oAlarm.m_strSecondType = l_result->GetValue("second_type");
			l_oAlarm.m_strThirdType = l_result->GetValue("third_type");
			l_oAlarm.m_strFourthType = l_result->GetValue("fourth_type");
			l_oAlarm.m_strCalledNoType = l_result->GetValue("called_no_type");
			l_oAlarm.m_strContactNo = l_result->GetValue("contact_no");
			l_oAlarm.m_strContactName = l_result->GetValue("contact_name");
			l_oAlarm.m_strReceiptName = l_result->GetValue("receipt_name");
			l_oAlarm.m_strSourceType = l_result->GetValue("source_type");
			l_oAlarm.m_strSourceId = l_result->GetValue("source_id");
			l_oAlarm.m_strReceiptDeptCode = l_result->GetValue("receipt_dept_code");
			l_oAlarm.m_strReceiptDeptName = l_result->GetValue("receipt_dept_name");

			l_oAlarm.m_alarmProcess.m_strID = l_result->GetValue("process_id");
			l_oAlarm.m_alarmProcess.m_strState = l_result->GetValue("process_state");
			l_oAlarm.m_alarmProcess.m_strTimeSigned = l_result->GetValue("time_signed");
			l_oAlarm.m_alarmProcess.m_strTimeFeedBack = l_result->GetValue("time_feedback");
			l_oAlarm.m_alarmProcess.m_strProcessDeptCode = l_result->GetValue("process_dept_code");

			p_oRespond.m_oBody.m_vecAlarm.push_back(l_oAlarm);
		}
		p_oRespond.m_oBody.m_strResult = "0";
	}

	return ;
}

bool CBusinessImpl::BuildWebGetAlarmDetailRespond(PROTOCOL::CWebGetAlarmDetailRespond& p_oRespond, const PROTOCOL::CWebGetAlarmDetailRequest& p_oRequest)
{
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!CheckToken(p_oRequest.m_oBody.m_strToken))
	{
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		return false;
	}

	if (p_oRequest.m_oBody.m_strID.empty())
	{
		ICC_LOG_WARNING(m_pLog, "return,unkown alarm id in OnNotifiWebGetAlarmDetailRequest:msg_id[%s]",
			p_oRequest.m_oHeader.m_strMsgid.c_str());
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_ALARMID;
		return false;
	}

	DataBase::SQLRequest l_oGetAlarmSQL;
	l_oGetAlarmSQL.sql_id = "select_icc_t_alarm";
	l_oGetAlarmSQL.param["id"] = p_oRequest.m_oBody.m_strID;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oGetAlarmSQL);
	ICC_LOG_DEBUG(m_pLog, "select alarm by alarm id,sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm by alarm id error:[%s]", l_pResult->GetErrorMsg().c_str());
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_DB;
		return false;
	}

	// 查询成功
	if (l_pResult->RecordSize() > 0)
	{
		p_oRespond.m_oBody.m_strResult = "0";//查询成功
		while (l_pResult->Next())
		{
			PROTOCOL::CWebGetAlarmDetailRespond::CAlarm l_oAlarm;
			// 获取接警信息
			l_oAlarm.m_strID = l_pResult->GetValue("id");
			l_oAlarm.m_strMergeID = l_pResult->GetValue("merge_id");
			l_oAlarm.m_strTitle = l_pResult->GetValue("title");
			l_oAlarm.m_strContent = l_pResult->GetValue("content");
			l_oAlarm.m_strTime = l_pResult->GetValue("time");
			l_oAlarm.m_strActualOccurTime = l_pResult->GetValue("actual_occur_time");
			l_oAlarm.m_strAddr = l_pResult->GetValue("addr");
			l_oAlarm.m_strLongitude = l_pResult->GetValue("longitude");
			l_oAlarm.m_strLatitude = l_pResult->GetValue("latitude");
			l_oAlarm.m_strState = l_pResult->GetValue("state");
			l_oAlarm.m_strLevel = l_pResult->GetValue("level");
			l_oAlarm.m_strSourceType = l_pResult->GetValue("source_type");
			l_oAlarm.m_strSourceID = l_pResult->GetValue("source_id");
			l_oAlarm.m_strHandleType = l_pResult->GetValue("handle_type");
			l_oAlarm.m_strFirstType = l_pResult->GetValue("first_type");
			l_oAlarm.m_strSecondType = l_pResult->GetValue("second_type");
			l_oAlarm.m_strThirdType = l_pResult->GetValue("third_type");
			l_oAlarm.m_strFourthType = l_pResult->GetValue("fourth_type");
			l_oAlarm.m_strVehicleNo = l_pResult->GetValue("vehicle_no");
			l_oAlarm.m_strVehicleType = l_pResult->GetValue("vehicle_type");
			l_oAlarm.m_strSymbolCode = l_pResult->GetValue("symbol_code");
			l_oAlarm.m_strSymbolAddr = l_pResult->GetValue("symbol_addr");
			l_oAlarm.m_strFireBuildingType = l_pResult->GetValue("fire_building_type");
			l_oAlarm.m_strEventType = l_pResult->GetValue("event_type");
			l_oAlarm.m_strCalledNoType = l_pResult->GetValue("called_no_type");
			l_oAlarm.m_strActualCalledNoType = l_pResult->GetValue("actual_called_no_type");
			l_oAlarm.m_strCallerNo = l_pResult->GetValue("caller_no");
			l_oAlarm.m_strCallerName = l_pResult->GetValue("caller_name");
			l_oAlarm.m_strCallerAddr = l_pResult->GetValue("caller_addr");
			l_oAlarm.m_strCallerID = l_pResult->GetValue("caller_id");
			l_oAlarm.m_strCallerIDType = l_pResult->GetValue("caller_id_type");
			l_oAlarm.m_strCallerGender = l_pResult->GetValue("caller_gender");
			l_oAlarm.m_strCallerAge = l_pResult->GetValue("caller_age");
			l_oAlarm.m_strCallerBirthday = l_pResult->GetValue("caller_birthday");
			l_oAlarm.m_strContactNo = l_pResult->GetValue("contact_no");
			l_oAlarm.m_strContactName = l_pResult->GetValue("contact_name");
			l_oAlarm.m_strContactAddr = l_pResult->GetValue("contact_addr");
			l_oAlarm.m_strContactID = l_pResult->GetValue("contact_id");
			l_oAlarm.m_strContactIDType = l_pResult->GetValue("contact_id_type");
			l_oAlarm.m_strContactGender = l_pResult->GetValue("contact_gender");
			l_oAlarm.m_strContactAge = l_pResult->GetValue("contact_age");
			l_oAlarm.m_strContactBirthday = l_pResult->GetValue("contact_birthday");
			l_oAlarm.m_strAdminDeptDistrictCode = l_pResult->GetValue("admin_dept_district_code");
			l_oAlarm.m_strAdminDeptCode = l_pResult->GetValue("admin_dept_code");
			l_oAlarm.m_strAdminDeptName = l_pResult->GetValue("admin_dept_name");
			l_oAlarm.m_strReceiptDeptDistrictCode = l_pResult->GetValue("receipt_dept_district_code");
			l_oAlarm.m_strReceiptDeptCode = l_pResult->GetValue("receipt_dept_code");
			l_oAlarm.m_strReceiptDeptName = l_pResult->GetValue("receipt_dept_name");
			l_oAlarm.m_strLeaderCode = l_pResult->GetValue("leader_code");
			l_oAlarm.m_strLeaderName = l_pResult->GetValue("leader_name");
			l_oAlarm.m_strReceiptCode = l_pResult->GetValue("receipt_code");
			l_oAlarm.m_strReceiptName = l_pResult->GetValue("receipt_name");
			l_oAlarm.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
			p_oRespond.m_oBody.m_vecAlarm.push_back(l_oAlarm);

			// 查询该接警单下的所有处警单_开始
			DataBase::SQLRequest l_SqlProcessRequest;
			l_SqlProcessRequest.sql_id = "select_icc_t_alarm_process";
			l_SqlProcessRequest.param["alarm_id"] = l_oAlarm.m_strID;

			// 执行SQL
			DataBase::IResultSetPtr l_pProcessResult = m_pDBConn->Exec(l_SqlProcessRequest);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pProcessResult->GetSQL().c_str());

			if (!l_pProcessResult->IsValid())
			{
				// 执行SQL失败,未能查到相应警单的处警信息			
				ICC_LOG_ERROR(m_pLog, "query process info by alarm id fail:[%s]", l_pProcessResult->GetErrorMsg().c_str());
				continue;
			}

			// 查询成功
			std::vector<PROTOCOL::CWebGetAlarmDetailRespond::CAlarmProcess> l_vecProcess;
			while (l_pProcessResult->Next())
			{
				PROTOCOL::CWebGetAlarmDetailRespond::CAlarmProcess l_oProcess;
				// 获取处警信息
				l_oProcess.m_strID = l_pProcessResult->GetValue("id");
				l_oProcess.m_strAlarmID = l_pProcessResult->GetValue("alarm_id");
				l_oProcess.m_strState = l_pProcessResult->GetValue("state");
				l_oProcess.m_strTimeEdit = l_pProcessResult->GetValue("time_edit");
				l_oProcess.m_strTimeSubmit = l_pProcessResult->GetValue("time_submit");
				l_oProcess.m_strTimeArrived = l_pProcessResult->GetValue("time_arrived");
				l_oProcess.m_strTimeSigned = l_pProcessResult->GetValue("time_signed");
				l_oProcess.m_strTimeFeedBack = l_pProcessResult->GetValue("time_feedback");
				l_oProcess.m_strIsNeedFeedback = l_pProcessResult->GetValue("is_need_feedback");
				l_oProcess.m_strDispatchDeptDistrictCode = l_pProcessResult->GetValue("dispatch_dept_district_code");
				l_oProcess.m_strDispatchDeptCode = l_pProcessResult->GetValue("dispatch_dept_code");
				l_oProcess.m_strDispatchDeptName = l_pProcessResult->GetValue("dispatch_dept_name");
				l_oProcess.m_strDispatchCode = l_pProcessResult->GetValue("dispatch_code");
				l_oProcess.m_strDispatchName = l_pProcessResult->GetValue("dispatch_name");
				l_oProcess.m_strDispatchLeaderCode = l_pProcessResult->GetValue("dispatch_leader_code");
				l_oProcess.m_strDispatchLeaderName = l_pProcessResult->GetValue("dispatch_leader_name");
				l_oProcess.m_strDispatchSuggestion = l_pProcessResult->GetValue("dispatch_suggestion");
				l_oProcess.m_strDispatchLeaderInstruction = l_pProcessResult->GetValue("dispatch_leader_instruction");
				l_oProcess.m_strProcessDeptDistrictCode = l_pProcessResult->GetValue("process_dept_district_code");
				l_oProcess.m_strProcessDeptCode = l_pProcessResult->GetValue("process_dept_code");
				l_oProcess.m_strProcessDeptName = l_pProcessResult->GetValue("process_dept_name");
				l_oProcess.m_strProcessCode = l_pProcessResult->GetValue("process_code");
				l_oProcess.m_strProcessName = l_pProcessResult->GetValue("process_name");
				l_oProcess.m_strProcessLeaderCode = l_pProcessResult->GetValue("process_leader_code");
				l_oProcess.m_strProcessLeaderName = l_pProcessResult->GetValue("process_leader_name");
				l_oProcess.m_strProcessFeedback = l_pProcessResult->GetValue("process_feedback");
				l_oProcess.m_strProcessLeaderInstruction = l_pProcessResult->GetValue("process_leader_instruction");

				l_vecProcess.push_back(l_oProcess);
			}
			p_oRespond.m_oBody.m_vecAlarmProcess.push_back(l_vecProcess);
		}
	}
	DataBase::SQLRequest l_oGetAlarmLogSQL;
	l_oGetAlarmLogSQL.sql_id = "select_icc_t_alarm_log";
	l_oGetAlarmLogSQL.param["alarm_id"] = p_oRequest.m_oBody.m_strID;
	DataBase::IResultSetPtr l_pAlarmLogResult = m_pDBConn->Exec(l_oGetAlarmLogSQL);
	ICC_LOG_DEBUG(m_pLog, "select alarm log by alarm id,sql:[%s]", l_pAlarmLogResult->GetSQL().c_str());

	if (!l_pAlarmLogResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm log by alarm id error:[%s]", l_pAlarmLogResult->GetErrorMsg().c_str());
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_DB;
		return false;
	}

	// 查询成功
	if (l_pAlarmLogResult->RecordSize() > 0)
	{
		p_oRespond.m_oBody.m_strResult = "0";//查询成功
		while (l_pAlarmLogResult->Next())
		{
			PROTOCOL::CWebGetAlarmDetailRespond::CAlarmLog l_oAlarmLog;
			// 获取接警信息
			l_oAlarmLog.m_strAlarmID = l_pAlarmLogResult->GetValue("alarm_id");
			l_oAlarmLog.m_strCode = l_pAlarmLogResult->GetValue("code");
			l_oAlarmLog.m_strContent = l_pAlarmLogResult->GetValue("content");
			l_oAlarmLog.m_strResourceType = l_pAlarmLogResult->GetValue("resource_type");
			l_oAlarmLog.m_strResourceId = l_pAlarmLogResult->GetValue("resource_id");
			l_oAlarmLog.m_strCreateUser = l_pAlarmLogResult->GetValue("create_user");
			l_oAlarmLog.m_strCreateTime = l_pAlarmLogResult->GetValue("create_time");
			l_oAlarmLog.m_strUpdateUser = l_pAlarmLogResult->GetValue("update_user");
			l_oAlarmLog.m_strUpdateTime = l_pAlarmLogResult->GetValue("update_time");

			p_oRespond.m_oBody.m_vecAlarmLog.push_back(l_oAlarmLog);
		}
	}
	return true;
}

void CBusinessImpl::BuildWebAddAlarmRequest(PROTOCOL::CWebAddAlarmRequest& p_oRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "add_or_update_alarm_and_process_request", "queue_alarm");
	
	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);

	p_oRequest.m_oBody.m_oAlarm.m_strActualOccurTime = p_oRequest.m_oBody.m_oAlarm.m_strTime;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptName = l_oRespond.m_oBody.m_oStaff.m_strName;


	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	p_oRequest.m_oBody.m_oAlarm.m_strUpdateTime = l_strCurrentTime;
	p_oRequest.m_oBody.m_oAlarm.m_strUpdateUser = l_oRespond.m_oBody.m_oStaff.m_strCode;

	// Web客户端新增警情时自动签收
	p_oRequest.m_oBody.m_oProcess.m_strState = "DIC020030";  // 已接收
	
	p_oRequest.m_oBody.m_oProcess.m_strTimeSubmit = l_strCurrentTime;
	p_oRequest.m_oBody.m_oProcess.m_strTimeSigned = l_strCurrentTime;
}
void CBusinessImpl::BuildWebUpdateAlarmRequest(PROTOCOL::CWebAddAlarmRequest& p_oRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "add_or_update_alarm_and_process_request", "queue_alarm");

	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);

	p_oRequest.m_oBody.m_oAlarm.m_strActualOccurTime = p_oRequest.m_oBody.m_oAlarm.m_strTime;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strAdminDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
	p_oRequest.m_oBody.m_oAlarm.m_strReceiptName = l_oRespond.m_oBody.m_oStaff.m_strName;


	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	p_oRequest.m_oBody.m_oAlarm.m_strUpdateTime = l_strCurrentTime;
	p_oRequest.m_oBody.m_oAlarm.m_strUpdateUser = l_oRespond.m_oBody.m_oStaff.m_strCode;
}

void CBusinessImpl::BuildWebAddAlarmRemarkRequest(PROTOCOL::CWebAddAlarmRemarkRequest& p_oRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "add_alarm_remark_request", "queue_alarm");

	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);

	p_oRequest.m_oBody.m_oAlarmRemark.m_strFeedbackDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oAlarmRemark.m_strFeedbackDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oAlarmRemark.m_strFeedbackCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
	p_oRequest.m_oBody.m_oAlarmRemark.m_strFeedbackName = l_oRespond.m_oBody.m_oStaff.m_strName;
	p_oRequest.m_oBody.m_oAlarmRemark.m_strFeedbackTime = m_pDateTime->CurrentDateTimeStr();
}

void CBusinessImpl::BuildWebGetAlarmFeedbackRespond(PROTOCOL::CWebGetAlarmFeedBackRespond& p_oRespond, const PROTOCOL::CWebGetAlarmFeedBackRequest& p_oRequest)
{
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader);

	if (!CheckToken(p_oRequest.m_oBody.m_strToken))
	{// 校验授权码失败
		p_oRespond.m_oBody.m_strResult = WEB_ERROR_TOKEN;
		return;
	}

	p_oRespond.m_oBody.m_strResult = "0";

	// 根据Token获取process_dept_code
	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);

	// 根据alarm_id与process_dept_code获取处警单信息
	PROTOCOL::CWebGetAlarmRespond::CAlarmProcess l_oProcess;
	if (GetProcessInfo(p_oRequest.m_oBody.m_strAlarmID, l_oRespond.m_oBody.m_oDept.m_strCode, l_oProcess))
	{
		// 根据process_id获取反馈单信息
		if(!GetFeedbackInfo(l_oProcess.m_strID, p_oRespond.m_oBody.m_oFeedback))
		{// 获取失败，则构建默认响应消息
			// 根据alarm_id获取接警单信息
			PROTOCOL::CWebGetAlarmRespond::CAlarm l_oAlarm;
			if (GetAlarmInfo(p_oRequest.m_oBody.m_strAlarmID, l_oAlarm))
			{
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmID = l_oAlarm.m_strID;
				p_oRespond.m_oBody.m_oFeedback.m_strActualOccurTime = l_oAlarm.m_strActualOccurTime;
				p_oRespond.m_oBody.m_oFeedback.m_strActualOccurAddr = l_oAlarm.m_strAddr;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmCalledNoType = l_oAlarm.m_strCalledNoType;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmFirstType = l_oAlarm.m_strFirstType;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmSecondType = l_oAlarm.m_strSecondType;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmThirdType = l_oAlarm.m_strThirdType;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmFourthType = l_oAlarm.m_strFourthType;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmAddrDeptName = l_oAlarm.m_strAdminDeptName;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmLatitude = l_oAlarm.m_strLatitude;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmLongitude = l_oAlarm.m_strLongitude;
				p_oRespond.m_oBody.m_oFeedback.m_strAlarmLongitude = l_oAlarm.m_strLongitude;
				p_oRespond.m_oBody.m_oFeedback.m_strFireBuildingFirstType = l_oAlarm.m_strFireBuildingType;
				p_oRespond.m_oBody.m_oFeedback.m_strTrafficVehicleNo = l_oAlarm.m_strVehicleNo;
				p_oRespond.m_oBody.m_oFeedback.m_strTrafficVehicleType = l_oAlarm.m_strVehicleType;
				p_oRespond.m_oBody.m_oFeedback.m_strEventType = l_oAlarm.m_strEventType;
			}
			p_oRespond.m_oBody.m_oFeedback.m_strProcessID = l_oProcess.m_strID;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessDeptCode = l_oProcess.m_strProcessDeptCode;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessDeptName = l_oProcess.m_strProcessDeptName;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessCode = l_oProcess.m_strProcessCode;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessName = l_oProcess.m_strProcessName;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessLeaderCode = l_oProcess.m_strProcessLeaderCode;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessLeaderName = l_oProcess.m_strProcessLeaderName;
			p_oRespond.m_oBody.m_oFeedback.m_strProcessDeptDistrictCode = l_oProcess.m_strProcessDeptDistrictCode;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchDeptCode = l_oProcess.m_strDispatchDeptCode;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchDeptName = l_oProcess.m_strDispatchDeptName;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchCode = l_oProcess.m_strDispatchCode;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchName = l_oProcess.m_strDispatchName;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchLeaderCode = l_oProcess.m_strDispatchLeaderCode;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchLeaderName = l_oProcess.m_strDispatchLeaderName;
			p_oRespond.m_oBody.m_oFeedback.m_strDispatchDeptDistrictCode = l_oProcess.m_strDispatchDeptDistrictCode;

			// 默认反馈信息
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackLeaderCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackLeaderName = l_oRespond.m_oBody.m_oStaff.m_strName;
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
			p_oRespond.m_oBody.m_oFeedback.m_strFeedbackName = l_oRespond.m_oBody.m_oStaff.m_strName;

			std::string l_strTime = m_pDateTime->CurrentDateTimeStr();
			p_oRespond.m_oBody.m_oFeedback.m_strTimePoliceDispatch = l_strTime;
			p_oRespond.m_oBody.m_oFeedback.m_strTimePoliceArrived = l_strTime;
		}
		//// 自动填写处警人，反馈人信息为当前登录用户的信息
		//p_oRespond.m_oBody.m_oFeedback.m_strProcessCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		//p_oRespond.m_oBody.m_oFeedback.m_strProcessName = l_oRespond.m_oBody.m_oStaff.m_strName;
		//p_oRespond.m_oBody.m_oFeedback.m_strProcessLeaderCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		//p_oRespond.m_oBody.m_oFeedback.m_strProcessLeaderName = l_oRespond.m_oBody.m_oStaff.m_strName;
		//p_oRespond.m_oBody.m_oFeedback.m_strFeedbackLeaderCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		//p_oRespond.m_oBody.m_oFeedback.m_strFeedbackLeaderName = l_oRespond.m_oBody.m_oStaff.m_strName;
		//p_oRespond.m_oBody.m_oFeedback.m_strFeedbackCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		//p_oRespond.m_oBody.m_oFeedback.m_strFeedbackName = l_oRespond.m_oBody.m_oStaff.m_strName;
	}
}

void CBusinessImpl::BuildWebAddAlarmFeedBackRequest(PROTOCOL::CWebAddAlarmFeedBackRequest& p_oRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "add_or_update_feedback_request", "queue_alarm");

	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);	

	p_oRequest.m_oBody.m_oFeedback.m_strFeedbackDeptDistrictCode = l_oRespond.m_oBody.m_oDept.m_strDistrictCode;
	//hyb 提的需求，webicc 可以修改反馈单位、反馈人、反馈领导
	if (p_oRequest.m_oBody.m_oFeedback.m_strFeedbackDeptCode.empty() && p_oRequest.m_oBody.m_oFeedback.m_strFeedbackDeptName.empty())
	{
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	}
	if (p_oRequest.m_oBody.m_oFeedback.m_strFeedbackCode.empty() && p_oRequest.m_oBody.m_oFeedback.m_strFeedbackName.empty())
	{
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackName = l_oRespond.m_oBody.m_oStaff.m_strName;
	}
	if (p_oRequest.m_oBody.m_oFeedback.m_strFeedbackLeaderCode.empty() && p_oRequest.m_oBody.m_oFeedback.m_strFeedbackLeaderName.empty())
	{
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackLeaderCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
		p_oRequest.m_oBody.m_oFeedback.m_strFeedbackLeaderName = l_oRespond.m_oBody.m_oStaff.m_strName;
	}

	p_oRequest.m_oBody.m_oFeedback.m_strUpdateUser = l_oRespond.m_oBody.m_oStaff.m_strCode;

	{// 根据处警单位代码与警情id查询处警id
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_alarm_process";
		l_SqlRequest.param["alarm_id"] = p_oRequest.m_oBody.m_oFeedback.m_strAlarmID;
		l_SqlRequest.param["process_dept_code"] = l_oRespond.m_oBody.m_oDept.m_strCode;

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
			return;
		}

		if (!l_pResult->Next())
		{
			ICC_LOG_DEBUG(m_pLog, "result is empty");
			return;
		}

		p_oRequest.m_oBody.m_oFeedback.m_strProcessID = l_pResult->GetValue("id");
	}
}

bool CBusinessImpl::BuildWebSignRequest(PROTOCOL::CWebSignRequest& p_oRequest)
{
	BuildRequestHeader(p_oRequest.m_oHeader, "add_alarm_sign_request", "queue_alarm");

	// 获取登录信息
	PROTOCOL::CWebLoginRespond l_oRespond;
	ReadLoginRespondInfo(p_oRequest.m_oBody.m_strToken, l_oRespond);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_process_sign";
	l_SqlRequest.param["alarm_id"] = p_oRequest.m_oBody.m_oProcess.m_strAlarmID;
	l_SqlRequest.param["process_dept_code"] = l_oRespond.m_oBody.m_oDept.m_strCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (!l_pResult->Next())
	{ // 查询结果为空
		ICC_LOG_DEBUG(m_pLog, "result is null");
		return false;
	}

	// 构建更新处警单信息
	p_oRequest.m_oBody.m_oProcess.m_strID = l_pResult->GetValue("id");
	p_oRequest.m_oBody.m_oProcess.m_strProcessDeptCode = l_oRespond.m_oBody.m_oDept.m_strCode;
	p_oRequest.m_oBody.m_oProcess.m_strProcessDeptName = l_oRespond.m_oBody.m_oDept.m_strName;
	p_oRequest.m_oBody.m_oProcess.m_strProcessCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
	p_oRequest.m_oBody.m_oProcess.m_strProcessName = l_oRespond.m_oBody.m_oStaff.m_strName;
	p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderCode = l_oRespond.m_oBody.m_oStaff.m_strCode;
	p_oRequest.m_oBody.m_oProcess.m_strProcessLeaderName = l_oRespond.m_oBody.m_oStaff.m_strName;

	return true;
}

bool CBusinessImpl::BuildWebProcessDept(std::string m_strAlarmID, std::vector<PROTOCOL::CProcessDept>& p_vecProcessDept)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_process_web";
	l_SqlRequest.param["alarm_id"] = m_strAlarmID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		PROTOCOL::CProcessDept l_oProcessDept;
		l_oProcessDept.m_strDeptCode = l_pResult->GetValue("process_dept_code");
		l_oProcessDept.m_strDeptName = l_pResult->GetValue("process_dept_name");
		l_oProcessDept.m_strParentDeptCode = l_pResult->GetValue("parent_dept_code");
		p_vecProcessDept.push_back(l_oProcessDept);
	}
	return true;
}

void CBusinessImpl::BuildWebBackAlarmSync(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, PROTOCOL::CWebBackAlarmSync& p_oSync)
{
	//消息头
	p_oSync.m_oHeader.m_strSystemID = "ICC";
	p_oSync.m_oHeader.m_strSubsystemID = "WEB";
	p_oSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	p_oSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oSync.m_oHeader.m_strCmd = "web_back_alarm_sync";
	p_oSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	p_oSync.m_oHeader.m_strRequestType = "1";
	p_oSync.m_oHeader.m_strResponse = "";
	p_oSync.m_oHeader.m_strResponseType = "";

	//消息体
	p_oSync.m_oBody.m_oData.m_strAlarmID = p_oRequest.m_oBody.m_oData.m_strAlarmID;
	p_oSync.m_oBody.m_oData.m_strProcessID = p_oRequest.m_oBody.m_oData.m_strProcessID;

	std::string l_strReceiptId; 
	std::string l_strReceiptName; 
	if (!SelectAlarmReciptInfo(p_oRequest.m_oBody.m_oData.m_strAlarmID, l_strReceiptId, l_strReceiptName))
	{
		ICC_LOG_ERROR(m_pLog, "SelectAlarmReciptInfo error");
	}

	p_oSync.m_oBody.m_oData.m_strDispatchID = l_strReceiptId;
	p_oSync.m_oBody.m_oData.m_strDispatchName = l_strReceiptName;
	p_oSync.m_oBody.m_oData.m_strBackUserID = p_oRequest.m_oBody.m_oData.m_strBackUserID;
	p_oSync.m_oBody.m_oData.m_strBackUserName = p_oRequest.m_oBody.m_oData.m_strBackUserName;
	p_oSync.m_oBody.m_oData.m_strBackDeptID = p_oRequest.m_oBody.m_oData.m_strBackDeptID;
	p_oSync.m_oBody.m_oData.m_strBackDeptName = p_oRequest.m_oBody.m_oData.m_strBackDeptName;
	p_oSync.m_oBody.m_oData.m_strReason = p_oRequest.m_oBody.m_oData.m_strReason;

}

bool ICC::CBusinessImpl::SelectAlarmReciptInfo(const std::string& p_strAlarmId, std::string& p_strReciptId, std::string& p_strReciptName)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm";
	l_SqlRequest.param["id"] = p_strAlarmId;
	
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_strReciptId = l_pResult->GetValue("receipt_code");
		p_strReciptName = l_pResult->GetValue("receipt_name");
	}

	return true;
}

bool ICC::CBusinessImpl::BackAlarmHandle(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	// 1.判断当前处警单状态是否为已签收及之前的状态
	if (!IsStateBeforAccepted(p_oRequest.m_oBody.m_oData.m_strProcessID, strTransGuid))
	{
		return false;
	}
	// 2.如果当前处警单状态为已签收之前的状态，则更新处警单状态为"已退警"
	if (!UpdateProcessState(p_oRequest, strTransGuid))
	{
		return false;
	}

	// 2.更新接警单状态为"处警中"
	if (!_UpdateAlarmState(p_oRequest, strTransGuid))
	{
		return false;
	}

	// 4.记录退单原因
	if (!_InsertBackReason(p_oRequest, strTransGuid))
	{
		return false;
	}

	// 4.记录流水并发送流水同步
	if (!AddAlarmLogInfo(p_oRequest, strTransGuid))
	{
		return false;
	}

	//// 4.判断是否为第三方警情
	//if (!IsThirdAlarm(p_oRequest.m_oBody.m_oData.m_strAlarmID))
	//{
	//	return true;
	//}

	//// 5.如果是第三方警情，则修改第三方警情表的处理标志位
	//if (!UpdateThirdAlarmState())
	//{
	//	return false;
	//}

	return true;
}

bool ICC::CBusinessImpl::IsStateBeforAccepted(std::string p_strProcessID, const std::string& strTransGuid)
{
	std::string l_strProcessState;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_process";
	l_SqlRequest.param["id"] = p_strProcessID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		l_strProcessState = l_pResult->GetValue("state");
	}

	if (l_strProcessState.empty() || l_strProcessState.compare(DIC020030) >= 0)
	{
		return false;
	}

	return true;
}

bool ICC::CBusinessImpl::UpdateProcessState(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "update_icc_t_alarm_process";
	l_tSQLRequest.param["id"] = p_oRequest.m_oBody.m_oData.m_strProcessID;
	l_tSQLRequest.set["state"] = DIC020073;
	l_tSQLRequest.set["update_user"] = p_oRequest.m_oBody.m_oData.m_strBackUserName;
	l_tSQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateProcess sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool ICC::CBusinessImpl::_UpdateAlarmState(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "update_icc_t_alarm";
	l_tSQLRequest.param["id"] = p_oRequest.m_oBody.m_oData.m_strAlarmID;
	l_tSQLRequest.param["state"] = "DIC019030";
	l_tSQLRequest.set["update_user"] = p_oRequest.m_oBody.m_oData.m_strBackUserName;
	l_tSQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateAlarm sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool ICC::CBusinessImpl::_InsertBackReason(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "insert_icc_t_backalarm_reason";
	l_tSQLRequest.param["guid"] = m_pString->CreateGuid();
	l_tSQLRequest.param["process_id"] = p_oRequest.m_oBody.m_oData.m_strProcessID;
	l_tSQLRequest.param["back_reason"] = p_oRequest.m_oBody.m_oData.m_strReason;	

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "web insert back reason sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "web insert back reason failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool ICC::CBusinessImpl::AddAlarmLogInfo(const PROTOCOL::CWebBackAlarmRequest& p_oRequest, const std::string& strTransGuid)
{
	std::string l_strGUID(m_pString->CreateGuid());
	std::string l_strCurTime(m_pDateTime->CurrentDateTimeStr());

	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strGUID = l_strGUID;
	l_oAlarmLogInfo.m_strAlarmID = p_oRequest.m_oBody.m_oData.m_strAlarmID;
	l_oAlarmLogInfo.m_strCode = BS001002028;
	{//l_oAlarmLogInfo.m_strContent
		std::vector<std::string> l_vecParamList;
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strBackUserName);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strBackUserID);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strBackDeptName);
		l_vecParamList.push_back(p_oRequest.m_oBody.m_oData.m_strReason);
		l_oAlarmLogInfo.m_strContent = BuildAlarmLogContent(l_vecParamList);
	}
	l_oAlarmLogInfo.m_strResourceID = "";
	l_oAlarmLogInfo.m_strResourceType = "";
	l_oAlarmLogInfo.m_strCreateUser = p_oRequest.m_oBody.m_oData.m_strBackUserName;
	l_oAlarmLogInfo.m_strCreateTime = l_strCurTime;	

	if (!InsertAlarmLogInfo(l_oAlarmLogInfo, strTransGuid))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log failed!");
		return false;
	}

	SyncAlarmLogInfo(l_oAlarmLogInfo);
	return true;
}

std::string ICC::CBusinessImpl::BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

bool CBusinessImpl::InsertAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_alarm_log";

	l_tSQLReqInsertAlarm.param["guid"] = p_AlarmLogInfo.m_strGUID;
	l_tSQLReqInsertAlarm.param["alarm_id"] = p_AlarmLogInfo.m_strAlarmID;
	l_tSQLReqInsertAlarm.param["code"] = p_AlarmLogInfo.m_strCode;
	l_tSQLReqInsertAlarm.param["content"] = p_AlarmLogInfo.m_strContent;
	l_tSQLReqInsertAlarm.param["resource_type"] = p_AlarmLogInfo.m_strResourceType;
	l_tSQLReqInsertAlarm.param["resource_id"] = p_AlarmLogInfo.m_strResourceID;

	l_tSQLReqInsertAlarm.param["create_user"] = p_AlarmLogInfo.m_strCreateUser;
	l_tSQLReqInsertAlarm.param["create_time"] = p_AlarmLogInfo.m_strCreateTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "WEB";
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strGUID = p_rAlarmLogToSync.m_strGUID;
	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strCode = p_rAlarmLogToSync.m_strCode;
	l_oAlarmLogSync.m_oBody.m_strContent = p_rAlarmLogToSync.m_strContent;
	l_oAlarmLogSync.m_oBody.m_strResourceType = p_rAlarmLogToSync.m_strResourceType;
	l_oAlarmLogSync.m_oBody.m_strResourceID = p_rAlarmLogToSync.m_strResourceID;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strUpdateUser = p_rAlarmLogToSync.m_strUpdateUser;
	l_oAlarmLogSync.m_oBody.m_strUpdateTime = p_rAlarmLogToSync.m_strUpdateTime;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}
bool CBusinessImpl::GetDeptGuid(const std::string& p_strInDeptCode, std::string& p_strOutDeptGuid, std::string& p_strOutParentDeptGuid)
{
	std::string l_strDeptCode = p_strInDeptCode;

	std::map<std::string, std::string> l_mapDeptInfo;
	m_pRedisClient->HGetAll(DEPT_INFO_KEY, l_mapDeptInfo);

	for (auto var : l_mapDeptInfo)
	{
		CDeptInfo l_oDeptInfo;
		l_oDeptInfo.Parse(var.second, m_pJsonFty->CreateJson());

		if (l_strDeptCode.compare(l_oDeptInfo.m_strCode) == 0)
		{
			p_strOutDeptGuid = l_oDeptInfo.m_strGuid;
			p_strOutParentDeptGuid = l_oDeptInfo.m_strParentGuid;

			break;
		}
	}

	if (!p_strOutParentDeptGuid.empty())
	{
		for (auto var : l_mapDeptInfo)
		{
			CDeptInfo l_oDeptInfo;
			l_oDeptInfo.Parse(var.second, m_pJsonFty->CreateJson());

			if (p_strOutParentDeptGuid.compare(l_oDeptInfo.m_strCode) == 0)
			{
				//如果上级的机构类型为市局，则不取其机构ID
				if (l_oDeptInfo.m_strType.compare("DIC014013") == 0)
				{
					p_strOutParentDeptGuid = "";
				}

				break;
			}
		}
	}


	return true;
}

/*
bool CBusinessImpl::LoadAllDept(std::map<std::string, std::string>& p_mapDeptInfo)
{
	bool loadtag = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dept";
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "ExecQuery success, sql:[%s]", l_result->GetSQL().c_str());

		while (l_result->Next())
		{
			PROTOCOL::CDeptSync pCDeptInfo;
			pCDeptInfo.m_oBody.m_strGuid = l_result->GetValue("to_guid");
			pCDeptInfo.m_oBody.m_strParentGuid = l_result->GetValue("parent_guid");
			pCDeptInfo.m_oBody.m_strCode = l_result->GetValue("dept_code");
			pCDeptInfo.m_oBody.m_strDistrictCode = l_result->GetValue("district_code");
			pCDeptInfo.m_oBody.m_strName = l_result->GetValue("name");
			pCDeptInfo.m_oBody.m_strType = l_result->GetValue("dept_type");
			pCDeptInfo.m_oBody.m_strPhone = l_result->GetValue("phone");
			pCDeptInfo.m_oBody.m_strLevel = l_result->GetValue("level");
			pCDeptInfo.m_oBody.m_strShortcut = l_result->GetValue("shortcut");
			pCDeptInfo.m_oBody.m_strSort = l_result->GetValue("sort");

			p_mapDeptInfo[pCDeptInfo.m_oBody.m_strGuid] = pCDeptInfo.ToString(m_pJsonFty->CreateJson());
		}

		ICC_LOG_DEBUG(m_pLog, "Get all dept success, dept size[%d]", p_mapDeptInfo.size());
	}
	
	return true;
}*/
