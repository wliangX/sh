#include "Boost.h"
#include "BusinessImpl.h"

#define SYSTEMID ("icc_server")
#define SUBSYSTEMID ("icc_server_alarm_chat")
#define TIMER_CMD_NAME "alarm_chat_timer"

#define STAFF_INFO_MAP_KEY "StaffInfoMap"

#define GroupSyncTypeAdd "1"
#define GroupSyncTypeRemove "2"
#define GroupSyncTypeExit "3"
#define GroupSyncTypeEditName "4"


void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pLockFty = ICCGetILockFactory();
	m_pAlarmLock = m_pLockFty->CreateLock(Lock::TypeRecursiveMutex);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "init_chat_window_request", OnNotifiInitChatWindowRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_chat_group_request", OnNotifiAddChatGroupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "edit_chat_group_request", OnNotifiEditChatGroupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_chat_groups_request", OnNotifiGetChatGroupsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_chat_group_info_request", OnNotifiGetChatGroupInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_chat_group_member_request", OnNotifiAddChatGroupMemberRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "remove_chat_group_member_request", OnNotifiRemoveChatGroupMemberRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "exit_chat_group_request", OnNotifiExitChatGroupRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "send_chat_msg_request", OnNotifiSendChatMsgRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_chat_records_request", OnNotifiGetChatRecordsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_groupchat_records_request", OnNotifiGetGroupChatRecordsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_msg_state_request", OnNotifiSetMsgStateRequest);
												   
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);

    ICC_LOG_DEBUG(m_pLog, "chat start success!");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "chat stop success");
}

void CBusinessImpl::OnDestroy()
{

}

//定时器的响应
void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotify)
{
	
}

void CBusinessImpl::OnNotifiInitChatWindowRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CInitChatWindow l_oRequest;
	PROTOCOL::Chat::CInitChatWindow l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	InitChatWindow(l_oRequest.m_strStaffCode, m_pString->ToUInt(l_oRequest.m_strPageSize), m_pString->ToUInt(l_oRequest.m_strPageIndex), l_oRespond.m_oBody);
	
	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "init_chat_window_respond");	
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiAddChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CAddChatGroup l_oRequest;
	PROTOCOL::Chat::CAddChatGroup l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strGuid = m_pDBConn->BeginTransaction();

	bool l_result = false;
	l_oRespond.m_oBody.m_strGroupID = AddChatGroup(l_oRequest.m_strOperator, l_oRequest.m_strGroupName, l_strGuid);
	if (!l_oRespond.m_oBody.m_strGroupID.empty())
	{
		l_result = AddChatGroupMember(l_oRequest.m_strOperator, l_oRespond.m_oBody.m_strGroupID, l_oRequest.m_members, l_strGuid);

		if (l_result)
		{
			l_oRespond.m_oBody.m_strGroupID = l_oRespond.m_oBody.m_strGroupID;
			m_pDBConn->Commit(l_strGuid);
		}
		else
		{
			m_pDBConn->Rollback(l_strGuid);
		}
	}
	else
	{
		m_pDBConn->Rollback(l_strGuid);
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "add_chat_group_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	if (l_result)
	{
		//同步群信息
		CSyncGroupInfo l_syncGroupInfo;
		BuildSyncHeader("topic_alarm_chat_sync", "chat_group_sync", l_syncGroupInfo.m_oHeader);
		l_syncGroupInfo.m_oBody.m_strSyncType = GroupSyncTypeAdd;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupID = l_oRespond.m_oBody.m_strGroupID;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupName = l_oRequest.m_strGroupName;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_strAdminCode = l_oRequest.m_strOperator;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_vecMembers = l_oRequest.m_members;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_syncGroupInfo.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiEditChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CEditChatGroup l_oRequest;
	PROTOCOL::Chat::CEditChatGroup l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (l_oRequest.m_strGroupID.empty() || l_oRequest.m_strGroupName.empty())
	{
		BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "edit_chat_group_respond");
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);

		ICC_LOG_ERROR(m_pLog, "edit chat group, param error.");
		return;
	}

	if (!EditChatGroup(l_oRequest.m_strGroupID, l_oRequest.m_strGroupName))
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "edit_chat_group_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	//同步群信息
	CSyncGroupInfo l_syncGroupInfo;
	BuildSyncHeader("topic_alarm_chat_sync", "chat_group_sync", l_syncGroupInfo.m_oHeader);
	l_syncGroupInfo.m_oBody.m_strSyncType = GroupSyncTypeEditName;
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupID = l_oRequest.m_strGroupID;
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupName = l_oRequest.m_strGroupName;
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strAdminCode = l_oRequest.m_strOperator;

	ChatGroupInfo l_chatGroupInfo;
	GetChatGroupInfo(l_oRequest.m_strGroupID, l_chatGroupInfo);
	l_syncGroupInfo.m_oBody.m_groupInfo.m_vecMembers = l_chatGroupInfo.m_vecMembers;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_syncGroupInfo.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnNotifiGetChatGroupsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CGetChatGroups l_oRequest;
	PROTOCOL::Chat::CGetChatGroups l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	GetChatGroups(l_oRespond.m_oBody.m_vecGroups);
	
	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "get_chat_groups_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetChatGroupInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CGetChatGroupInfo l_oRequest;
	PROTOCOL::Chat::CGetChatGroupInfo l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	GetChatGroupInfo(l_oRequest.m_strGroupID, l_oRespond.m_oBody.m_groupInfo);

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "get_chat_groups_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiAddChatGroupMemberRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CAddChatGroupMember l_oRequest;
	PROTOCOL::Chat::CAddChatGroupMember l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (!AddChatGroupMember(l_oRequest.m_strOperator, l_oRequest.m_strGroupID, l_oRequest.m_members, ""))
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}
	
	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "add_chat_group_member_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	if (l_oRespond.m_oBody.m_strResult == "0")
	{
		//同步群信息
		CSyncGroupInfo l_syncGroupInfo;
		BuildSyncHeader("topic_alarm_chat_sync", "chat_group_sync", l_syncGroupInfo.m_oHeader);
		l_syncGroupInfo.m_oBody.m_strSyncType = GroupSyncTypeAdd;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupID = l_oRequest.m_strGroupID;

		ChatGroupInfo l_chatGroupInfo;
		GetChatGroupInfo(l_oRequest.m_strGroupID, l_chatGroupInfo);
		l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupName = l_chatGroupInfo.m_strGroupName;

		l_syncGroupInfo.m_oBody.m_groupInfo.m_strAdminCode = l_chatGroupInfo.m_strAdminCode;
		l_syncGroupInfo.m_oBody.m_groupInfo.m_vecMembers = l_oRequest.m_members;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_syncGroupInfo.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiRemoveChatGroupMemberRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CRemoveChatGroupMember l_oRequest;
	PROTOCOL::Chat::CRemoveChatGroupMember l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (!DelChatGroupMember(l_oRequest.m_strOperator, l_oRequest.m_strGroupID, l_oRequest.m_strStaffCode, ""))
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "remove_chat_group_member_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	//同步群信息
	CSyncGroupInfo l_syncGroupInfo;
	BuildSyncHeader("topic_alarm_chat_sync", "chat_group_sync", l_syncGroupInfo.m_oHeader);
	l_syncGroupInfo.m_oBody.m_strSyncType = GroupSyncTypeRemove;
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupID = l_oRequest.m_strGroupID;

	ChatGroupInfo l_chatGroupInfo;
	GetChatGroupInfo(l_oRequest.m_strGroupID, l_chatGroupInfo);
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupName = l_chatGroupInfo.m_strGroupName;

	Data::CStaffInfo l_staffInfo;
	GetStaffInfo(l_oRequest.m_strStaffCode, l_staffInfo);

	Member l_groupMember;
	l_groupMember.m_strStaffCode = l_oRequest.m_strStaffCode;
	l_groupMember.m_strStaffName = l_staffInfo.m_strName;

	l_syncGroupInfo.m_oBody.m_groupInfo.m_vecMembers.push_back(l_groupMember);

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_syncGroupInfo.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnNotifiExitChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CExitChatGroup l_oRequest;
	PROTOCOL::Chat::CExitChatGroup l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (!ExitChatGroup(l_oRequest.m_strGroupID, l_oRequest.m_strStaffCode))
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "remove_chat_group_member_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

	//同步群信息
	CSyncGroupInfo l_syncGroupInfo;
	BuildSyncHeader("topic_alarm_chat_sync", "chat_group_sync", l_syncGroupInfo.m_oHeader);
	l_syncGroupInfo.m_oBody.m_strSyncType = GroupSyncTypeExit;
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupID = l_oRequest.m_strGroupID;

	ChatGroupInfo l_chatGroupInfo;
	GetChatGroupInfo(l_oRequest.m_strGroupID, l_chatGroupInfo);
	l_syncGroupInfo.m_oBody.m_groupInfo.m_strGroupName = l_chatGroupInfo.m_strGroupName;

	Data::CStaffInfo l_staffInfo;
	GetStaffInfo(l_oRequest.m_strStaffCode, l_staffInfo);

	Member l_groupMember;
	l_groupMember.m_strStaffCode = l_oRequest.m_strStaffCode;
	l_groupMember.m_strStaffName = l_staffInfo.m_strName;

	l_syncGroupInfo.m_oBody.m_groupInfo.m_vecMembers.push_back(l_groupMember);

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_syncGroupInfo.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnNotifiSendChatMsgRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CSendChatMsg l_oRequest;
	PROTOCOL::Chat::CSendChatMsg l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "send_chat_msg_respond");

	std::string l_strMsgID = SaveMessage(l_oRequest);
	if (l_strMsgID.empty())
	{
		l_oRespond.m_oBody.m_strResult = "1";
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		return;
	}

	SendChatMessage(l_oRequest, l_strMsgID);

	l_oRespond.m_oBody.m_strResult = "0";
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetChatRecordsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CGetP2PChatRecords l_oRequest;
	PROTOCOL::Chat::CGetP2PChatRecords l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	GetP2PRecords(l_oRequest.m_strSender, 
		l_oRequest.m_strReceiver, 
		m_pString->ToUInt(l_oRequest.m_strPageSize), 
		m_pString->ToUInt(l_oRequest.m_strPageIndex), 
		l_oRespond.m_oBody.m_p2pRecords);

	//获取总数
	l_oRespond.m_oBody.m_strCount = m_pString->Number(GetP2PRecordsCount(l_oRequest.m_strSender, l_oRequest.m_strReceiver));
	//实际数量
	l_oRespond.m_oBody.m_strAllCount = m_pString->Number(l_oRespond.m_oBody.m_p2pRecords.size());

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "get_chat_records_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiGetGroupChatRecordsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CGetGroupChatRecords l_oRequest;
	PROTOCOL::Chat::CGetGroupChatRecords l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	unsigned int l_iUnreadCount = 0;
	GetGroupRecords(l_oRequest.m_strGroupID,
		m_pString->ToUInt(l_oRequest.m_strPageSize), 
		m_pString->ToUInt(l_oRequest.m_strPageIndex),
		l_oRespond.m_oBody.m_groupRecords);

	//获取总数
	l_oRespond.m_oBody.m_strCount = m_pString->Number(GetGroupRecordsCount(l_oRequest.m_strGroupID));
	//实际数量
	l_oRespond.m_oBody.m_strAllCount = m_pString->Number(l_oRespond.m_oBody.m_groupRecords.size());

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "get_groupchat_records_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnNotifiSetMsgStateRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::Chat::CSetMsgState l_oRequest;
	PROTOCOL::Chat::CSetMsgState l_oRespond;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (!SetMsgState(l_oRequest.m_strMsgID, l_oRequest.m_strReceiver))
	{
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
	}

	BuildRespondHeader(l_oRequest.m_oHeader, l_oRespond.m_oHeader, "set_smg_state_respond");
	std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "respond:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::InitChatWindow(std::string p_strStaffCode, unsigned int uiPageSize, unsigned int uiPageIndex, ChatInitWindow& pChatInitWindow)
{
	//获取总数
	unsigned int l_iCount = GetChatItemCount(p_strStaffCode);
	
	
	DataBase::SQLRequest l_oSelectInit;
	l_oSelectInit.sql_id = "get_chat_window";
	
	l_oSelectInit.param["page_size"] = m_pString->Number(uiPageSize);
	l_oSelectInit.param["page_index"] = m_pString->Number(uiPageIndex);
	l_oSelectInit.param["staff_code"] = p_strStaffCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectInit, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select operation failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	std::string str = l_oResult->GetSQL();

	std::string l_strCount;
	while (l_oResult->Next())
	{
		Record l_chatRecord;
		unsigned int l_iUnreadCount = 0;

		l_chatRecord.m_strIsGroupMsg = l_oResult->GetValue("is_group_msg");
		l_chatRecord.m_strLastTime = l_oResult->GetValue("time");

		if (l_chatRecord.m_strIsGroupMsg == "1")
		{
			l_chatRecord.m_strItemID = l_oResult->GetValue("group_id");
			l_chatRecord.m_strItemName = l_oResult->GetValue("group_name");

			GroupRecord l_lastChatRecord;
			if (!GetGroupRecordsEx(l_chatRecord.m_strItemID, p_strStaffCode, uiPageSize, 1, l_chatRecord.m_groupRecords, l_lastChatRecord, l_iUnreadCount))
			{
				continue;
			}

			l_chatRecord.m_strLastSender = l_lastChatRecord.m_strSender;
			l_chatRecord.m_strLastContent = l_lastChatRecord.m_strContent;			
		}
		else
		{
			std::string l_strSender = l_oResult->GetValue("sender");
			std::string l_strReceiver = l_oResult->GetValue("receiver");
			if (p_strStaffCode == l_strReceiver)
			{
				l_chatRecord.m_strItemID = l_strSender;
				l_chatRecord.m_strItemName = l_oResult->GetValue("sender_name");
			}
			else
			{
				l_chatRecord.m_strItemID = l_strReceiver;
				l_chatRecord.m_strItemName = l_oResult->GetValue("receiver_name");
			}

			P2PRecord l_lastChatRecord;
			if (!GetP2PRecordsEx(l_strSender, l_strReceiver, p_strStaffCode, uiPageSize, 1, l_chatRecord.m_p2pRecords, l_lastChatRecord, l_iUnreadCount))
			{
				continue;
			}

			l_chatRecord.m_strLastSender = l_lastChatRecord.m_strSender;
			l_chatRecord.m_strLastContent = l_lastChatRecord.m_strContent;
		}

		//未读数量
		l_chatRecord.m_strUnReadCount = m_pString->Number(l_iUnreadCount);

		auto l_iter = pChatInitWindow.m_mapRecords.find(l_chatRecord.m_strItemID);
		if (l_iter != pChatInitWindow.m_mapRecords.end())
		{
			--l_iCount;
		}
		else
		{
			pChatInitWindow.m_mapRecords[l_chatRecord.m_strItemID] = l_chatRecord;
		}			
	}

	//符合条件的记录数
	pChatInitWindow.m_strAllCount = m_pString->Number(pChatInitWindow.m_mapRecords.size());
	pChatInitWindow.m_strCount = m_pString->Number(l_iCount);
	return true;
}

unsigned int CBusinessImpl::GetChatItemCount(std::string p_strStaffCode)
{
	DataBase::SQLRequest l_oSelectCount;
	l_oSelectCount.sql_id = "get_chat_item_count";
	l_oSelectCount.param["staff_code"] = p_strStaffCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectCount, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get total count failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return 0;
	}

	unsigned int l_totalCount = 0;
	if (l_oResult->Next())
	{
		l_totalCount = m_pString->ToUInt(l_oResult->GetValue("total_count"));
	}

	return l_totalCount;
}

unsigned int CBusinessImpl::GetP2PRecordsCount(std::string p_strSender, std::string p_strReceiver)
{
	DataBase::SQLRequest l_oSelectCount;
	l_oSelectCount.sql_id = "get_p2p_records_count";
	l_oSelectCount.param["sender"] = p_strSender;
	l_oSelectCount.param["receiver"] = p_strReceiver;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectCount, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get total count failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return 0;
	}

	unsigned int l_totalCount = 0;
	if (l_oResult->Next())
	{
		l_totalCount = m_pString->ToUInt(l_oResult->GetValue("total_count"));
	}

	ICC_LOG_DEBUG(m_pLog, "get p2p chat, [%s]-[%s] total count [%d] sql:%s", p_strSender.c_str(), p_strSender.c_str(), l_totalCount, l_oResult->GetSQL().c_str());

	return l_totalCount;
}

unsigned int CBusinessImpl::GetGroupRecordsCount(std::string p_strGroupID)
{
	DataBase::SQLRequest l_oSelectCount;
	l_oSelectCount.sql_id = "get_group_records_count";
	l_oSelectCount.param["group_id"] = p_strGroupID;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectCount, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get total count failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return 0;
	}

	unsigned int l_totalCount = 0;
	if (l_oResult->Next())
	{
		l_totalCount = m_pString->ToUInt(l_oResult->GetValue("total_count"));
	}

	ICC_LOG_DEBUG(m_pLog, "get group chat [%s]  total count [%d] sql:%s", p_strGroupID.c_str(), l_totalCount, l_oResult->GetSQL().c_str());

	return l_totalCount;
}

bool CBusinessImpl::GetP2PRecords(std::string p_strSender,
	std::string p_strReceiver,
	unsigned int uiPageSize,
	unsigned int uiPageIndex,
	P2PRecords& pChatRecords)
{
	DataBase::SQLRequest l_oSelectRecord;
	l_oSelectRecord.sql_id = "get_p2p_records";
	l_oSelectRecord.param["sender"] = p_strSender;
	l_oSelectRecord.param["receiver"] = p_strReceiver;
	l_oSelectRecord.param["page_size"] = m_pString->Number(uiPageSize);
	l_oSelectRecord.param["page_index"] = m_pString->Number(uiPageIndex);

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectRecord, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get [%s]-[%s] records failed:[DB Error]:%s sql:%s", p_strSender.c_str(), p_strReceiver.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	while (l_oResult->Next())
	{
		P2PRecord l_chatRecord;
		l_chatRecord.m_strMsgID = l_oResult->GetValue("msg_id");
		l_chatRecord.m_strSender = l_oResult->GetValue("sender");
		l_chatRecord.m_strSenderName = l_oResult->GetValue("sender_name");
		l_chatRecord.m_strReceiver = l_oResult->GetValue("receiver");
		l_chatRecord.m_strReceiverName = l_oResult->GetValue("receiver_name");
		l_chatRecord.m_strSeatNo = l_oResult->GetValue("seat_no");
		l_chatRecord.m_strTime = l_oResult->GetValue("time");
		l_chatRecord.m_strContent = l_oResult->GetValue("content");
		l_chatRecord.m_strType = l_oResult->GetValue("type");
		l_chatRecord.m_strIsRead = l_oResult->GetValue("is_read").empty() ? "0" : l_oResult->GetValue("is_read");

		pChatRecords[l_chatRecord.m_strMsgID] = l_chatRecord;
	}

	ICC_LOG_DEBUG(m_pLog, "get [%s]-[%s] records success.", p_strSender.c_str(), p_strReceiver.c_str());
	return true;
}

//获取点对点聊天记录
bool CBusinessImpl::GetP2PRecordsEx(std::string p_strSender, 
	std::string p_strReceiver,
	std::string p_strStaffCode,
	unsigned int uiPageSize, 
	unsigned int uiPageIndex, 
	P2PRecords& pChatRecords, 
	P2PRecord& pLastChatRecord, 
	unsigned int & p_iUnreadCount)
{
	unsigned int l_iUnreadCount = 0;

	DataBase::SQLRequest l_oSelectRecord;
	l_oSelectRecord.sql_id = "get_p2p_records";
	l_oSelectRecord.param["sender"] = p_strSender;
	l_oSelectRecord.param["receiver"] = p_strReceiver;
	l_oSelectRecord.param["page_size"] = m_pString->Number(uiPageSize);
	l_oSelectRecord.param["page_index"] = m_pString->Number(uiPageIndex);

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectRecord, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get [%s]-[%s] records failed:[DB Error]:%s sql:%s", p_strSender.c_str(), p_strReceiver.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}	

	while (l_oResult->Next())
	{
		P2PRecord l_chatRecord;
		l_chatRecord.m_strMsgID = l_oResult->GetValue("msg_id");
		l_chatRecord.m_strSender = l_oResult->GetValue("sender");
		l_chatRecord.m_strSenderName = l_oResult->GetValue("sender_name");
		l_chatRecord.m_strReceiver = l_oResult->GetValue("receiver");
		l_chatRecord.m_strReceiverName = l_oResult->GetValue("receiver_name");
		l_chatRecord.m_strSeatNo = l_oResult->GetValue("seat_no");
		l_chatRecord.m_strTime = l_oResult->GetValue("time");
		l_chatRecord.m_strContent = l_oResult->GetValue("content");
		l_chatRecord.m_strType = l_oResult->GetValue("type");
		l_chatRecord.m_strIsRead = l_oResult->GetValue("is_read").empty() ? "0" : l_oResult->GetValue("is_read");

		//统计未读
		if (l_chatRecord.m_strIsRead == "0" && p_strStaffCode == l_chatRecord.m_strReceiver)
		{
			++l_iUnreadCount;
		}

		if (pChatRecords.size() == 0)
		{
			//单独取到最新一条记录
			pLastChatRecord = l_chatRecord;
		}

		pChatRecords[l_chatRecord.m_strMsgID] = l_chatRecord;	
	}

	p_iUnreadCount = l_iUnreadCount;

	ICC_LOG_DEBUG(m_pLog, "get [%s]-[%s] records success, unread[%d].", p_strSender.c_str(), p_strReceiver.c_str(), p_iUnreadCount);
	return true;
}

//获取群聊天记录
bool CBusinessImpl::GetGroupRecords(std::string p_strGroupID,
	unsigned int uiPageSize, 
	unsigned int uiPageIndex, 
	GroupRecords& pChatRecords)
{
	DataBase::SQLRequest l_oSelectRecord;
	l_oSelectRecord.sql_id = "get_group_records";

	l_oSelectRecord.param["receiver"] = p_strGroupID;
	l_oSelectRecord.param["page_size"] = m_pString->Number(uiPageSize);
	l_oSelectRecord.param["page_index"] = m_pString->Number(uiPageIndex);

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectRecord, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group[%s] records failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get group records, sql[%s].", l_oResult->GetSQL().c_str());

	unsigned int l_iUnreadCount = 0;
	std::string l_strLastMsgID, l_strCurrentMsgID;
	while (l_oResult->Next())
	{
		GroupRecord l_chatRecord;
		Member l_groupMember;
		l_strCurrentMsgID = l_oResult->GetValue("msg_id");
		bool l_isRead = l_oResult->GetValue("is_read") == "1" ? true : false;
		l_groupMember.m_strStaffCode = l_oResult->GetValue("receiver");
		l_groupMember.m_strStaffName = l_oResult->GetValue("receiver_name");

		if (l_strCurrentMsgID != l_strLastMsgID)
		{
			l_chatRecord.m_strMsgID = l_oResult->GetValue("msg_id");
			l_chatRecord.m_strSender = l_oResult->GetValue("sender");
			l_chatRecord.m_strSenderName = l_oResult->GetValue("sender_name");
			l_chatRecord.m_strGroupID = l_oResult->GetValue("group_id");
			l_chatRecord.m_strGroupName = l_oResult->GetValue("group_name");
			l_chatRecord.m_strSeatNo = l_oResult->GetValue("seat_no");
			l_chatRecord.m_strTime = l_oResult->GetValue("time");
			l_chatRecord.m_strContent = l_oResult->GetValue("content");
			l_chatRecord.m_strType = l_oResult->GetValue("type");

			pChatRecords[l_strCurrentMsgID] = l_chatRecord;
			l_strLastMsgID = l_strCurrentMsgID;
		}

		if (l_isRead)
		{
			pChatRecords[l_strCurrentMsgID].m_readList.push_back(l_groupMember);
		}
		else
		{
			pChatRecords[l_strCurrentMsgID].m_unReadList.push_back(l_groupMember);
		}

	}

	ICC_LOG_DEBUG(m_pLog, "get group chat records success, [%s], size[%d].", p_strGroupID.c_str(), pChatRecords.size());
	return true;
}

bool CBusinessImpl::GetGroupRecordsEx(std::string p_strGroupID,
	std::string p_strStaffCode,
	unsigned int uiPageSize,
	unsigned int uiPageIndex,
	GroupRecords& pChatRecords,
	GroupRecord& pLastChatRecord,
	unsigned int & p_iUnreadCount)
{
	DataBase::SQLRequest l_oSelectRecord;
	l_oSelectRecord.sql_id = "get_group_records";

	l_oSelectRecord.param["receiver"] = p_strGroupID;
	l_oSelectRecord.param["page_size"] = m_pString->Number(uiPageSize);
	l_oSelectRecord.param["page_index"] = m_pString->Number(uiPageIndex);

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSelectRecord, true);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group[%s] records failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get group records, sql[%s].", l_oResult->GetSQL().c_str());

	unsigned int l_iUnreadCount = 0;
	std::string l_strLastMsgID, l_strCurrentMsgID;
	while (l_oResult->Next())
	{
		GroupRecord l_chatRecord;
		Member l_groupMember;
		l_strCurrentMsgID = l_oResult->GetValue("msg_id");
		bool l_isRead = l_oResult->GetValue("is_read") == "1" ? true : false;
		l_groupMember.m_strStaffCode = l_oResult->GetValue("receiver");
		l_groupMember.m_strStaffName = l_oResult->GetValue("receiver_name");

		//统计未读
		if (!l_isRead && l_groupMember.m_strStaffCode == p_strStaffCode)
		{
			++l_iUnreadCount;
		}

		if (l_strCurrentMsgID != l_strLastMsgID)
		{
			l_chatRecord.m_strMsgID = l_oResult->GetValue("msg_id");
			l_chatRecord.m_strSender = l_oResult->GetValue("sender");
			l_chatRecord.m_strSenderName = l_oResult->GetValue("sender_name");
			l_chatRecord.m_strGroupID = l_oResult->GetValue("group_id");
			l_chatRecord.m_strGroupName = l_oResult->GetValue("group_name");
			l_chatRecord.m_strSeatNo = l_oResult->GetValue("seat_no");
			l_chatRecord.m_strTime = l_oResult->GetValue("time");
			l_chatRecord.m_strContent = l_oResult->GetValue("content");
			l_chatRecord.m_strType = l_oResult->GetValue("type");

			if (pChatRecords.size() == 0)
			{
				//单独取到最新一条记录
				pLastChatRecord = l_chatRecord;
			}

			pChatRecords[l_strCurrentMsgID] = l_chatRecord;
			l_strLastMsgID = l_strCurrentMsgID;
		}

		if (l_isRead)
		{
			pChatRecords[l_strCurrentMsgID].m_readList.push_back(l_groupMember);
		}
		else
		{
			pChatRecords[l_strCurrentMsgID].m_unReadList.push_back(l_groupMember);
		}

	}

	p_iUnreadCount = l_iUnreadCount;

	ICC_LOG_DEBUG(m_pLog, "get group chat records success, [%s], size[%d] unread[%d].", p_strGroupID.c_str(), pChatRecords.size(), p_iUnreadCount);
	return true;
}

void CBusinessImpl::BuildRespondHeader(const PROTOCOL::CHeader& p_oRequestHeader, PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd)
{
	p_oRespondHeader.m_strSystemID = p_oRequestHeader.m_strSystemID;
	p_oRespondHeader.m_strSubsystemID = p_oRequestHeader.m_strSubsystemID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = p_strCmd;
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

void CBusinessImpl::BuildSyncHeader(std::string p_strTopic, std::string p_strCmd, PROTOCOL::CHeader& p_oRespondHeader)
{
	p_oRespondHeader.m_strSystemID = "";
	p_oRespondHeader.m_strSubsystemID = "";
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = "";
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = p_strCmd;
	p_oRespondHeader.m_strRequest = p_strTopic;
	p_oRespondHeader.m_strRequestType = "1";
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

std::string CBusinessImpl::AddChatGroup(std::string p_strUserCode, std::string p_strGroupName, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "add_chat_group";
	std::string l_strGuid = m_pString->CreateGuid();
	l_SQLRequest.param["group_id"] = l_strGuid;
	l_SQLRequest.param["group_name"] = p_strGroupName;
	l_SQLRequest.param["admin_code"] = p_strUserCode;
	l_SQLRequest.param["is_delete"] = "0";

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "add group[%s] failed:[DB Error]:%s sql:%s", p_strGroupName.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return "";
	}

	return l_strGuid;
}

bool CBusinessImpl::EditChatGroup(std::string p_strGroupID, std::string p_strGroupName)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "edit_chat_group";
	l_SQLRequest.param["group_id"] = p_strGroupID;
	l_SQLRequest.set["group_name"] = p_strGroupName;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "edit group[%s] failed:[DB Error]:%s sql:%s", p_strGroupName.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	return true;
}

std::string CBusinessImpl::GetChatGroupAdmin(std::string p_strGroupID)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "get_chat_group_admin";
	l_SQLRequest.param["group_id"] = p_strGroupID;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group[%s] admin failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return "";
	}

	std::string l_strAdminCode;
	if (l_oResult->Next())
	{
		l_strAdminCode = l_oResult->GetValue("admin_code");
	}

	return l_strAdminCode;
}

std::string CBusinessImpl::GetNewGroupAdmin(std::string p_strGroupID, std::string p_strOldAdmin, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "get_new_group_admin";
	l_SQLRequest.param["group_id"] = p_strGroupID;
	l_SQLRequest.param["admin_code"] = p_strOldAdmin;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group[%s] admin failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return "";
	}

	std::string l_strAdminCode;
	if (l_oResult->Next())
	{
		l_strAdminCode = l_oResult->GetValue("admin_code");
	}

	return l_strAdminCode;
}

bool CBusinessImpl::UpgradeGroupAdmin(std::string p_strGroupID, std::string p_strNew, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "upgrade_group_admin";
	l_SQLRequest.param["group_id"] = p_strGroupID;
	l_SQLRequest.set["admin_code"] = p_strNew;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "upgrade group admin [%s]-[%s] failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), p_strNew.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "upgrade group admin[%s]-[%s] success.", p_strGroupID.c_str(), p_strNew.c_str());

//	SyncGroupInfo();
	return true;
}

bool CBusinessImpl::DissolveGroup(std::string p_strGroupID, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "del_chat_group";
	l_SQLRequest.param["group_id"] = p_strGroupID;
	l_SQLRequest.set["is_delete"] = "1";

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "del group[%s] failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "del group[%s] success.", p_strGroupID.c_str());

	return true;
}

bool CBusinessImpl::GetChatGroups(ChatGroups &p_chatGroups)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "get_chat_groups";

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group list failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	while (l_oResult->Next())
	{
		ChatGroup l_chatGroups;
		l_chatGroups.m_strGroupID = l_oResult->GetValue("group_id");
		l_chatGroups.m_strGroupName = l_oResult->GetValue("group_name");
		l_chatGroups.m_strGroupID = l_oResult->GetValue("admin_code");

		p_chatGroups.push_back(l_chatGroups);
	}

	return true;
}

bool CBusinessImpl::GetChatGroupInfo(std::string p_strGroupID, ChatGroupInfo& p_chatGroupInfo)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "get_chat_groupinfo";
	//如果为空，全取
	if (!p_strGroupID.empty())
	{
		l_SQLRequest.param["group_id"] = p_strGroupID;
	}
	
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get group list failed:[DB Error]:%s, sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	std::string l_groupID, l_lastGroupID;
	while (l_oResult->Next())
	{
		l_groupID = l_oResult->GetValue("group_id");
		if (l_groupID != l_lastGroupID)
		{
			p_chatGroupInfo.m_strGroupID = l_groupID;
			p_chatGroupInfo.m_strGroupName = l_oResult->GetValue("group_name");
			p_chatGroupInfo.m_strAdminCode = l_oResult->GetValue("admin_code");
			l_lastGroupID = l_groupID;
		}
		ChatGroupMember l_chatGroupMember;
		l_chatGroupMember.m_strStaffCode = l_oResult->GetValue("staff_code");
		l_chatGroupMember.m_strStaffName = l_oResult->GetValue("staff_name");

		p_chatGroupInfo.m_vecMembers.push_back(l_chatGroupMember);
	}

	return true;
}

bool CBusinessImpl::AddChatGroupMember(std::string p_strOperator, std::string p_strGroupID, const Members& p_groupMembers, const std::string& strTransGuid)
{
	for (auto var : p_groupMembers)
	{
		DataBase::SQLRequest l_SQLRequest;
		l_SQLRequest.sql_id = "add_chat_group_member";

		l_SQLRequest.param["group_id"] = p_strGroupID;
		l_SQLRequest.param["staff_code"] = var.m_strStaffCode;
		l_SQLRequest.param["staff_name"] = var.m_strStaffName;

		DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
		if (!l_oResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add group[%s] member[%s] failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), var.m_strStaffCode.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add group[%s] member[%s] success.", p_strGroupID.c_str(), var.m_strStaffCode.c_str());
	}
	
	return true;
}

bool CBusinessImpl::DelChatGroupMember(std::string p_strOperator, std::string p_strGroupID, std::string p_strStaffCode, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "delete_chat_group_member";
	l_SQLRequest.param["group_id"] = p_strGroupID;
	l_SQLRequest.param["staff_code"] = p_strStaffCode;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "del group[%s] member[%s] failed:[DB Error]:%s sql:%s", p_strGroupID.c_str(), p_strStaffCode.c_str(), l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "del group[%s] member[%s] success.", p_strGroupID.c_str(), p_strStaffCode.c_str());
	
	return true;
}

bool CBusinessImpl::ExitChatGroup(std::string p_strGroupID, std::string p_strStaffCode)
{
	std::string l_strAdminCode = GetChatGroupAdmin(p_strGroupID);

	std::string l_strGuid = m_pDBConn->BeginTransaction();
	if (l_strAdminCode == p_strStaffCode)
	{
		//如果是群主退出
		std::string l_strNewAdmin = GetNewGroupAdmin(p_strGroupID, p_strStaffCode, l_strGuid);
		if (l_strNewAdmin.empty())
		{
			//如果是最后一个获取不到了，群内已经没有成员，解散群
			if (!DissolveGroup(p_strGroupID, l_strGuid))
			{
				m_pDBConn->Rollback(l_strGuid);
				return false;
			}				
		}

		if (!UpgradeGroupAdmin(p_strGroupID, l_strNewAdmin, l_strGuid))
		{
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}	
	}

	if (!DelChatGroupMember(p_strStaffCode, p_strGroupID, p_strStaffCode, l_strGuid))
	{
		m_pDBConn->Rollback(l_strGuid);
		return false;
	}
	
	m_pDBConn->Commit(l_strGuid);

	//发送群成员同步信息
//	SyncGroupInfo();

	return true;
}

void CBusinessImpl::SendChatMessage(const CSendChatMsg& p_chatMessage, std::string l_strMsgID)
{
	if (p_chatMessage.m_strIsGroupMsg == "0")
	{
		CTransforP2PChatMsg l_oTransforP2P;
		BuildRespondHeader(p_chatMessage.m_oHeader, l_oTransforP2P.m_oHeader, "transfer_p2p_chat_msg");

		//点对点消息
		std::string l_strSeatNo = GetSeatByStaffCode(p_chatMessage.m_strReceiver);
		if (l_strSeatNo.empty())
		{
			return;
		}
		
		l_oTransforP2P.m_oHeader.m_strRequest = l_strSeatNo;
		l_oTransforP2P.m_oHeader.m_strRequestType = "0";

		l_oTransforP2P.m_oBody.m_strMsgID = l_strMsgID;
		l_oTransforP2P.m_oBody.m_strSender = p_chatMessage.m_strSender;

		Data::CStaffInfo l_senderInfo;
		GetStaffInfo(p_chatMessage.m_strSender, l_senderInfo);
		l_oTransforP2P.m_oBody.m_strSenderName = l_senderInfo.m_strName;

		Data::CStaffInfo l_receiverInfo;
		GetStaffInfo(p_chatMessage.m_strReceiver, l_receiverInfo);
		
		l_oTransforP2P.m_oBody.m_strReceiver = p_chatMessage.m_strReceiver;
		l_oTransforP2P.m_oBody.m_strReceiverName = l_receiverInfo.m_strName;

		l_oTransforP2P.m_oBody.m_strSeatNo = p_chatMessage.m_strSeatNo;
		l_oTransforP2P.m_oBody.m_strTime = p_chatMessage.m_strTime;
		l_oTransforP2P.m_oBody.m_strContent = p_chatMessage.m_strContent;
		l_oTransforP2P.m_oBody.m_strType = p_chatMessage.m_strType;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oTransforP2P.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
	else
	{
		ChatGroupInfo l_chatGroupInfo;
		if (!GetChatGroupInfo(p_chatMessage.m_strReceiver, l_chatGroupInfo))
		{
			ICC_LOG_ERROR(m_pLog, "get group info failed message:[%s]", p_chatMessage.m_strReceiver.c_str());
			return;
		}					
		
		for (auto l_member : l_chatGroupInfo.m_vecMembers)
		{
			CTransforGroupChatMsg l_oTransforGroup;
			BuildRespondHeader(p_chatMessage.m_oHeader, l_oTransforGroup.m_oHeader, "transfer_group_chat_msg");

			std::string l_strSeatNo = GetSeatByStaffCode(l_member.m_strStaffCode);
			if (l_strSeatNo.empty())
			{
				//
			}

			l_oTransforGroup.m_oHeader.m_strRequest = l_strSeatNo;
			l_oTransforGroup.m_oHeader.m_strRequestType = "0";

			l_oTransforGroup.m_oBody.m_strMsgID = l_strMsgID;
			l_oTransforGroup.m_oBody.m_strSender = p_chatMessage.m_strSender;

			Data::CStaffInfo l_senderInfo;
			GetStaffInfo(p_chatMessage.m_strSender, l_senderInfo);
			l_oTransforGroup.m_oBody.m_strSenderName = l_senderInfo.m_strName;
			l_oTransforGroup.m_oBody.m_strGroupID = l_chatGroupInfo.m_strGroupID;
			l_oTransforGroup.m_oBody.m_strGroupName = l_chatGroupInfo.m_strGroupName;

			l_oTransforGroup.m_oBody.m_strSeatNo = p_chatMessage.m_strSeatNo;
			l_oTransforGroup.m_oBody.m_strTime = p_chatMessage.m_strTime;
			l_oTransforGroup.m_oBody.m_strContent = p_chatMessage.m_strContent;
			l_oTransforGroup.m_oBody.m_strType = p_chatMessage.m_strType;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_oTransforGroup.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}

std::string CBusinessImpl::SaveMessage(const CSendChatMsg& p_chatMessage)
{
	std::string l_strMsgID = m_pString->CreateGuid();
	SaveMessageTag(p_chatMessage, l_strMsgID);
	
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "save_chat_message";
	l_SQLRequest.param["msg_id"] = l_strMsgID;
	l_SQLRequest.param["sender"] = p_chatMessage.m_strSender;
	l_SQLRequest.param["seat_no"] = p_chatMessage.m_strSeatNo;
	l_SQLRequest.param["receiver"] = p_chatMessage.m_strReceiver;
	l_SQLRequest.param["time"] = p_chatMessage.m_strTime;
	l_SQLRequest.param["is_group_msg"] = p_chatMessage.m_strIsGroupMsg;
	l_SQLRequest.param["content"] = p_chatMessage.m_strContent;
	l_SQLRequest.param["type"] = p_chatMessage.m_strType;

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "save chat message failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return "";
	}

	ICC_LOG_DEBUG(m_pLog, "save chat message success [%s]->[%s][%s] .", p_chatMessage.m_strSender.c_str(), p_chatMessage.m_strReceiver.c_str(), p_chatMessage.m_strContent.c_str());

	return l_strMsgID;
}

bool CBusinessImpl::SaveMessageTag(const CSendChatMsg& p_chatMessage, std::string p_strMsgID)
{
	if (p_chatMessage.m_strIsGroupMsg == "1")
	{
		ChatGroupInfo p_chatGroupInfo;
		GetChatGroupInfo(p_chatMessage.m_strReceiver, p_chatGroupInfo);

		for (auto var : p_chatGroupInfo.m_vecMembers)
		{
			if (var.m_strStaffCode == p_chatMessage.m_strSender)
			{
				continue;
			}

			DataBase::SQLRequest l_SQLRequest;
			l_SQLRequest.sql_id = "save_chat_message_tag";
			l_SQLRequest.param["msg_id"] = p_strMsgID;
			l_SQLRequest.param["receiver"] = var.m_strStaffCode;
			l_SQLRequest.param["is_read"] = "0";


			DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
			if (!l_oResult->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "save chat message tag failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
				return false;
			}
		}		
	}
	else
	{
		DataBase::SQLRequest l_SQLRequest;
		l_SQLRequest.sql_id = "save_chat_message_tag";
		l_SQLRequest.param["msg_id"] = p_strMsgID;
		l_SQLRequest.param["receiver"] = p_chatMessage.m_strReceiver;
		l_SQLRequest.param["is_read"] = "0";

		DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
		if (!l_oResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "save chat message tag failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
			return false;
		}
	}
	return true;
}

std::string CBusinessImpl::GetSeatByStaffCode(std::string p_strStaffCode)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "get_user_by_staff";
	l_SQLRequest.param["staff_code"] = p_strStaffCode;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get user by staff failed:[DB Error]:%s, sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return "";
	}

	std::string l_strUserCode, l_strSeatNo;
	if (l_oResult->Next())
	{
		l_strUserCode = l_oResult->GetValue("code");
	}

	if (l_strUserCode.empty())
	{
		ICC_LOG_ERROR(m_pLog, "get user by staff[%s] failed.", p_strStaffCode.c_str());
		return "";
	}	

	std::map<std::string, std::string> l_clientRegisters;
	m_pRedisClient->HGetAll("ClientRegisterInfo", l_clientRegisters);
	CRegister l_register;
	for (auto var : l_clientRegisters)
	{		
		if (!var.second.empty())
		{
			if (!l_register.Parse(var.second, m_pJsonFty->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse json failed");
				return "";
			}
		}
		if (l_register.m_strClientName == l_strUserCode)
		{
			l_strSeatNo = l_register.m_strClientID;
			ICC_LOG_DEBUG(m_pLog, "get seat by staff success.[%s]-[%s]", p_strStaffCode.c_str(), l_strSeatNo.c_str());
			return l_strSeatNo;
		}		
	}
	
	ICC_LOG_ERROR(m_pLog, "get seat by staff failed.[%s]-[%s], staff is offline", p_strStaffCode.c_str(), l_strSeatNo.c_str());
	return "";
}

bool CBusinessImpl::GetStaffInfo(std::string p_strStaffCode, Data::CStaffInfo& p_staffInfo)
{
	std::string l_strInfo;
	if (!m_pRedisClient->HGet(STAFF_INFO_MAP_KEY, p_strStaffCode, l_strInfo))
	{
		return false;
	}

	Data::CStaffInfo l_staffInfo;
	return l_staffInfo.Parse(l_strInfo, m_pJsonFty->CreateJson());
}

bool CBusinessImpl::SetMsgState(std::string p_strMsgID, std::string p_strReceiver)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "set_msg_state";
	l_SQLRequest.param["msg_id"] = p_strMsgID;
	l_SQLRequest.param["receiver"] = p_strReceiver;
	l_SQLRequest.set["is_read"] = "1";

	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_SQLRequest);
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "set msg state failed:[DB Error]:%s sql:%s", l_oResult->GetErrorMsg().c_str(), l_oResult->GetSQL().c_str());
		return false;
	}

	return true;
}