#include "Boost.h"
#include "BusinessImpl.h"

#define CMD_ADD_NOTICE_REQUEST	"add_notice_request"
#define CMD_ADD_NOTICE_RESPOND	"add_notice_respond"
#define CMD_ADD_NOTICE_SYNC		"add_notice_sync"
#define CMD_GET_NOTICES_REQUEST	"get_notices_request"
#define CMD_GET_NOTICES_RESPOND	"get_notices_respond"

#define CMD_ADD_TIPS_REQUEST	"add_tips_request"
#define CMD_ADD_TIPS_RESPOND	"add_tips_respond"
//#define CMD_ADD_TIPS_SYNC		"add_tipse_sync"
#define CMD_MODIFY_TIPS_REQUEST	"modify_tips_request"
#define CMD_MODIFY_TIPS_RESPOND	"modify_tips_respond"
//#define CMD_MODIFY_TIPS_SYNC	"modify_tips_sync"
#define CMD_DEL_TIPS_REQUEST	"delete_tips_request"
#define CMD_DEL_TIPS_RESPOND	"delete_tips_respond"
//#define CMD_DEL_TIPS_SYNC		"delete_tips_sync"
#define CMD_GET_TIPS_REQUEST	"get_tips_request"
#define CMD_GET_TIPS_RESPOND	"get_tips_respond"
#define CMD_TOPIC_TIPS_SYNC		"topic_tips_sync"

#define CMD_ADD_URGES_REQUEST	"add_urges_request"
#define CMD_ADD_URGES_RESPOND	"add_urges_respond"
#define CMD_ADD_URGES_SYNC		"add_urges_sync"
#define CMD_ALARM_LOG_SYNC		"alarm_log_sync"

#define CMD_CALL_OVER_SYNC		"call_over_sync"
#define CMD_TIMEOUT_EVENT_SYNC	"timeout_event_sync"
#define CMD_DEL_TIMEOUT_EVENT_REQUEST	"delete_timeout_event_request"
#define CMD_DEL_TIMEOUT_EVENT_RESPOND	"delete_timeout_event_respond"
#define CMD_GET_TIMEOUT_EVENT_REQUEST	"get_timeout_event_request"
#define CMD_GET_TIMEOUT_EVENT_RESPOND	"get_timeout_event_respond"

#define LOG_URGES_ALARM			"BS001007002"
#define LOG_ALARM_RESOURCETYPE	"2"			//流水资源类型，处警

#define QUEUE_NOTICE			"queue_notice"
#define TOPIC_NOTICE			"topic_notice"
#define QUEUE_ALARM				"queue_alarm"
#define TOPIC_ALARM				"topic_alarm"
#define ICC_TOPIC_WEB			"icc_topic_web"
#define ICC_QUEUE_WEB			"icc_queue_web"

#define SERVER_NAME				"Server_Alarm_Notice"
#define NOTICE_INFO				"NoticeInfo"
#define TIPS_INFO				"TipsInfo"
#define TIMEOUT_EVENT_INFO		"TimeOutEventInfo"

#define SYNC_TYPE_ADD			"1"
#define SYNC_TYPE_MODIFY		"2"
#define SYNC_TYPE_DEL			"3"

#define Result_Success			"0"
#define Result_Failed			"1"

void CBusinessImpl::OnInit()
{
	m_nRingTimeOut = 25;
	m_nTalkTimeOut = 10;

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	ICC_LOG_DEBUG(m_pLog, "Notice OnStart 1");

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_ADD_NOTICE_REQUEST, OnNotifiAddNoticeRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_GET_NOTICES_REQUEST, OnNotifiGetNoticeRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_ADD_TIPS_REQUEST, OnNotifiAddTipsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_MODIFY_TIPS_REQUEST, OnNotifiModifyTipsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_DEL_TIPS_REQUEST, OnNotifiDelTipsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_GET_TIPS_REQUEST, OnNotifiGetTipsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_ADD_URGES_REQUEST, OnNotifiAddUrgesRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_CALL_OVER_SYNC, OnNotifiCallOverSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_DEL_TIMEOUT_EVENT_REQUEST, OnNotifiDelTimeOutEventRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, CMD_GET_TIMEOUT_EVENT_REQUEST, OnNotifiGetTimeOutEventRequest);

	LoadParams();
	LoadAllNotices();
	LoadAllTips();
	LoadTimeOutEvents();

    ICC_LOG_DEBUG(m_pLog, "Notice start success!");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "Notice stop success");
}

void CBusinessImpl::OnDestroy()
{

}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadParams()
{
	std::string l_strRingTimeOut = m_pConfig->GetValue("ICC/Plugin/Notice/RingTimeOut", "30");
	m_nRingTimeOut = atoi(l_strRingTimeOut.c_str());

	std::string l_strTalkTimeOut = m_pConfig->GetValue("ICC/Plugin/Notice/TalkTimeOut", "10");
	m_nTalkTimeOut = atoi(l_strTalkTimeOut.c_str());
}

void CBusinessImpl::LoadAllNotices()
{
	DataBase::SQLRequest l_tReqSelectSQL;
	l_tReqSelectSQL.sql_id = "select_icc_t_notice";
	l_tReqSelectSQL.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tReqSelectSQL, true);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_notice fialed, sql:[%s], ErrorMsg:[%s]",
			l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pResult->Next())
		{
			unsigned int l_iRowNum = l_pResult->RecordSize();
			for (unsigned int i = 0; i < l_iRowNum; i++)
			{
				PROTOCOL::CNoticeInfo l_oNoticeInfo;
				std::string l_strNoticeId = l_pResult->GetValue(i, "notice_id");
				l_oNoticeInfo.m_strUserCode = l_pResult->GetValue(i, "user_code");
				l_oNoticeInfo.m_strUserName = l_pResult->GetValue(i, "user_name");
				l_oNoticeInfo.m_strSeatNo = l_pResult->GetValue(i, "seat_no");

				l_oNoticeInfo.m_strDeptCode = l_pResult->GetValue(i, "dept_code");
				l_oNoticeInfo.m_strDeptName = l_pResult->GetValue(i, "dept_name");
				l_oNoticeInfo.m_strTargetDeptCode = l_pResult->GetValue(i, "target_dept_code");
				l_oNoticeInfo.m_strTargetDeptName = l_pResult->GetValue(i, "target_dept_name");

				l_oNoticeInfo.m_strNoticeType = l_pResult->GetValue(i, "notice_type");
				l_oNoticeInfo.m_strTitle = l_pResult->GetValue(i, "title");
				l_oNoticeInfo.m_strContent = l_pResult->GetValue(i, "content");
				l_oNoticeInfo.m_strTime = l_pResult->GetValue(i, "time");
				l_oNoticeInfo.m_strNoticeId = l_strNoticeId;

				std::string l_strMsg = l_oNoticeInfo.ToString(m_pJsonFty->CreateJson());
				m_pRedisClient->HSet(NOTICE_INFO, l_strNoticeId, l_strMsg);

				ICC_LOG_DEBUG(m_pLog, "Load notice: [%s]", l_strMsg.c_str());
			}
		}
	}
}
void CBusinessImpl::OnNotifiAddNoticeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Add Notice Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CAddNoticeRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}
		
		bool l_bResult = false;
		std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		std::string l_strNoticeType = l_oRequest.m_oBody.m_strNoticeType;			//通知类型（1：公告，2：通知）
		
		if (DeleteNotice(l_strNoticeType))
		{
			std::string l_strNoticeId = m_pString->CreateGuid();
			std::string l_strVal = GetNoticeInfo(l_oRequest, l_strNoticeId);
			if (m_pRedisClient->HSet(NOTICE_INFO, l_strNoticeId, l_strVal))
			{
				l_bResult = InsertNotice(l_oRequest, l_strNoticeId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "Set redis HSet failed, Value: [%s]", l_strVal.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Del notices failed, NoticeType: [%s]", l_strNoticeType.c_str());
		}

		PROTOCOL::CAddNoticeRespond l_oRespond;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_ADD_NOTICE_RESPOND, QUEUE_NOTICE, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send AddNotice Respond:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			SendAddNoticeSync(l_oRequest);
		}
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

		std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		PROTOCOL::CGetNoticesRespond l_oRespond;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_GET_NOTICES_RESPOND, QUEUE_NOTICE, SENDTYPE_QUEUE, l_strRelatedId);

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
//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadAllTips()
{
	m_pRedisClient->Del(TIPS_INFO);

	DataBase::SQLRequest l_tReqSelectSQL;
	l_tReqSelectSQL.sql_id = "select_icc_t_tips";
	l_tReqSelectSQL.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tReqSelectSQL, true);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_tips fialed, sql:[%s], ErrorMsg:[%s]",
			l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pResult->Next())
		{
			unsigned int l_iRowNum = l_pResult->RecordSize();
			for (unsigned int i = 0; i < l_iRowNum; i++)
			{
				PROTOCOL::CTipsInfo l_oTipsInfo;
				std::string l_strTipsId = l_pResult->GetValue(i, "tips_id");
				l_oTipsInfo.m_strCalledNoType= l_pResult->GetValue(i, "called_no_type");
				l_oTipsInfo.m_strLevel = l_pResult->GetValue(i, "level");
				l_oTipsInfo.m_strFirstType = l_pResult->GetValue(i, "first_type");
				l_oTipsInfo.m_strContent = l_pResult->GetValue(i, "tips_content");
				l_oTipsInfo.m_strTipsId = l_strTipsId;

				std::string l_strMsg = l_oTipsInfo.ToString(m_pJsonFty->CreateJson());
				m_pRedisClient->HSet(TIPS_INFO, l_strTipsId, l_strMsg);

				ICC_LOG_DEBUG(m_pLog, "Load tips: [%s]", l_strMsg.c_str());
			}
		}
	}
}

void CBusinessImpl::OnNotifiAddTipsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Add Tips Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CSetTipsRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		bool l_bResult = false;
		std::string l_strTipsId = m_pString->CreateGuid();
		std::string l_strVal = GetTipsInfo(l_oRequest, l_strTipsId);

		l_bResult = m_pRedisClient->HSet(TIPS_INFO, l_strTipsId, l_strVal);
		if (l_bResult)
		{
			l_bResult = InsertTips(l_oRequest, l_strTipsId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Set redis HSet failed, Value: [%s]", l_strVal.c_str());
		}

		PROTOCOL::CBaseRespond l_oRespond;
		l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;
		l_oRespond.m_oHeader.m_strResult = l_bResult ? Result_Success : Result_Failed;
		/*std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_ADD_TIPS_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;*/
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send AddTips Respond:[%s]", l_strMessage.c_str());
		
		if (l_bResult)
		{
			SendTipsSync(l_oRequest, l_strTipsId, SYNC_TYPE_ADD);
		}
	}
}
void CBusinessImpl::OnNotifiModifyTipsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Modify Tips Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CSetTipsRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		bool l_bResult = false;
		std::string l_strTipsId = l_oRequest.m_oBody.m_strTipsId;
		std::string l_strVal = GetTipsInfo(l_oRequest, l_strTipsId);

		l_bResult = m_pRedisClient->HSet(TIPS_INFO, l_strTipsId, l_strVal);
		if (l_bResult)
		{
			l_bResult = UpdateTips(l_oRequest, l_strTipsId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Set redis HSet failed, Value: [%s]", l_strVal.c_str());
		}

		PROTOCOL::CBaseRespond l_oRespond;
		l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;
		l_oRespond.m_oHeader.m_strResult = l_bResult ? Result_Success : Result_Failed;
		/*std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_MODIFY_TIPS_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;*/
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send ModifyTips Respond:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			SendTipsSync(l_oRequest, l_strTipsId, SYNC_TYPE_MODIFY);
		}
	}
}
void CBusinessImpl::OnNotifiDelTipsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Del Tips Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CSetTipsRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		bool l_bResult = false;
		std::string l_strTipsId = l_oRequest.m_oBody.m_strTipsId;
		std::string l_strVal = GetTipsInfo(l_oRequest, l_strTipsId);

		l_bResult = m_pRedisClient->HDel(TIPS_INFO, l_strTipsId);
		if (l_bResult)
		{
			l_bResult = DeleteTips(l_strTipsId);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Del redis HSet failed, Key: [%s]", l_strTipsId.c_str());
		}

		PROTOCOL::CBaseRespond l_oRespond;
		l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;
		l_oRespond.m_oHeader.m_strResult = l_bResult ? Result_Success : Result_Failed;
		/*std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_DEL_TIPS_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;*/
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send DelTips Respond:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			SendTipsSync(l_oRequest, l_strTipsId, SYNC_TYPE_DEL);
		}
	}
}
void CBusinessImpl::OnNotifiGetTipsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		ICC_LOG_DEBUG(m_pLog, "Receive Get Tips Request:[%s]", p_pNotify->GetMessages().c_str());

		// 解析请求消息
		PROTOCOL::CBaseRequest l_oRequest;
		if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
			return;
		}

		/*std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		PROTOCOL::CGetTipsRespond l_oRespond;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_GET_TIPS_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);*/
		PROTOCOL::CGetTipsRespond l_oRespond;
		l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

		std::map<std::string, std::string> l_mapTips;
		if (m_pRedisClient->HGetAll(TIPS_INFO, l_mapTips))
		{
			for (auto l_oTemp : l_mapTips)
			{
				PROTOCOL::CTipsInfo l_oTipsInfo;
				if (l_oTipsInfo.ParseString(l_oTemp.second, m_pJsonFty->CreateJson()))
				{
					PROTOCOL::CGetTipsRespond::CData l_oData;
					l_oData.m_strTipsId = l_oTipsInfo.m_strTipsId;
					l_oData.m_strCalledNoType = l_oTipsInfo.m_strCalledNoType;
					l_oData.m_strLevel = l_oTipsInfo.m_strLevel;
					l_oData.m_strFirstType = l_oTipsInfo.m_strFirstType;
					l_oData.m_strContent = l_oTipsInfo.m_strContent;

					l_oRespond.m_oBody.m_vecData.push_back(l_oData);
				}
			}

			l_oRespond.m_oBody.m_strCount = std::to_string(l_mapTips.size());
		}

		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send GetTips Respond:[%s]", l_strMessage.c_str());
	}
}
//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::OnNotifiAddUrgesRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	/*
	1、根据 警情ID 查找调派单位信息
	2、向 Web 网关发送催促同步信息
	3、写流水
	*/
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

		bool l_bResult = false;
		std::string l_strAlarmId = l_oRequest.m_oBody.m_strAlarmId;
		std::string l_strContent = l_oRequest.m_oBody.m_strContent;
		std::vector<PROTOCOL::CUrgesRequest::CBody> l_outVecData;
		if (SelectProcessAlarmByAlarmID(l_outVecData, l_strAlarmId))
		{
			l_bResult = true;
		}


		std::string l_strRelatedId = l_oRequest.m_oHeader.m_strMsgid;
		PROTOCOL::CBaseRespond l_oRespond;
		//l_oRespond.m_oHeader = CreateProtocolHeader(CMD_ADD_URGES_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;

		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send AddUrges Respond:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			SendUrgesSync(l_outVecData, l_strContent);

			AddUrgesAlarmLog(l_outVecData, l_strContent);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CBusinessImpl::LoadTimeOutEvents()
{
	DataBase::SQLRequest l_tReqSelectSQL;
	l_tReqSelectSQL.sql_id = "select_icc_t_timeout_event";
	l_tReqSelectSQL.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tReqSelectSQL, true);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_timeout_event fialed, sql:[%s], ErrorMsg:[%s]",
			l_pResult->GetSQL().c_str(), l_pResult->GetErrorMsg().c_str());
	}
	else
	{
		if (l_pResult->Next())
		{
			unsigned int l_iRowNum = l_pResult->RecordSize();
			for (unsigned int i = 0; i < l_iRowNum; i++)
			{
				PROTOCOL::CTimeOutEvent l_oEventData;
				std::string l_strGuid = l_pResult->GetValue(i, "guid");
				l_oEventData.m_oBody.m_strAlarmId = l_pResult->GetValue(i, "alarm_id");
				l_oEventData.m_oBody.m_strCallRefId = l_pResult->GetValue(i, "callref_id");
				l_oEventData.m_oBody.m_strReceiptDeptCode = l_pResult->GetValue(i, "receipt_dept_code");

				l_oEventData.m_oBody.m_strReceiptDeptName = l_pResult->GetValue(i, "receipt_dept_name");
				l_oEventData.m_oBody.m_strReceiptCode = l_pResult->GetValue(i, "receipt_code");
				l_oEventData.m_oBody.m_strReceiptName = l_pResult->GetValue(i, "receipt_name");
				l_oEventData.m_oBody.m_strReceiptSeatNo = l_pResult->GetValue(i, "receipt_seatno");

				l_oEventData.m_oBody.m_strStartTime = l_pResult->GetValue(i, "start_time");
				l_oEventData.m_oBody.m_strEndTime = l_pResult->GetValue(i, "end_time");
				l_oEventData.m_oBody.m_strAlarmType = l_pResult->GetValue(i, "alarm_type");
				l_oEventData.m_oBody.m_strGuid = l_strGuid;

				std::string l_strMsg = l_oEventData.ToString(m_pJsonFty->CreateJson());
				m_pRedisClient->HSet(TIMEOUT_EVENT_INFO, l_strGuid, l_strMsg);

				ICC_LOG_DEBUG(m_pLog, "Load TimeOutEvent: [%s]", l_strMsg.c_str());
			}
		}
	}
}
void CBusinessImpl::OnNotifiCallOverSync(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		std::string l_strNetMsg = p_pNotify->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive Call Over Sync:[%s]", l_strNetMsg.c_str());

		PROTOCOL::CCallOverEvent l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strNetMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid Call Over Sync protocol: [%s]", l_strNetMsg.c_str());

			return;
		}


		std::string l_strACD = l_oRequestObj.m_oBody.m_strACD;
		if (!l_strACD.empty())
		{
			//报警话务，检测是否振铃超时、通话超时
			std::string l_strCallRefId = l_oRequestObj.m_oBody.m_strCallRefId;
			std::string l_strRingTime = l_oRequestObj.m_oBody.m_strRingTime;
			std::string l_strTalkTime = l_oRequestObj.m_oBody.m_strTalkTime;
			std::string l_strHangupTime = l_oRequestObj.m_oBody.m_strHangupTime;

			if (!l_strRingTime.empty() && !l_strTalkTime.empty())
			{
				DateTime::CDateTime l_oRingTime = m_pDateTime->FromString(l_strRingTime);
				DateTime::CDateTime l_oTalkTime = m_pDateTime->FromString(l_strTalkTime);
				unsigned int l_unTimeSpan = m_pDateTime->SecondsDifference(l_oRingTime, l_oTalkTime);

				if (l_unTimeSpan > m_nRingTimeOut)
				{
					ProcessTimeOutEvent(l_strCallRefId, l_strRingTime, l_strTalkTime, "0");
				}
			}

			if (!l_strTalkTime.empty() && !l_strHangupTime.empty())
			{
				DateTime::CDateTime l_oTalkTime = m_pDateTime->FromString(l_strTalkTime);
				DateTime::CDateTime l_oHangupTime = m_pDateTime->FromString(l_strHangupTime);
				unsigned int l_unTimeSpan = m_pDateTime->SecondsDifference(l_oTalkTime, l_oHangupTime);

				if (l_unTimeSpan > m_nTalkTimeOut * 60)
				{
					ProcessTimeOutEvent(l_strCallRefId, l_strTalkTime, l_strHangupTime, "1");
				}
			}
		}
	}
}
void CBusinessImpl::OnNotifiDelTimeOutEventRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		std::string l_strNetMsg = p_pNotify->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive Del TimeOut Event Request:[%s]", l_strNetMsg.c_str());

		PROTOCOL::CTimeOutEvent l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strNetMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid Get TimeOut Event Request protocol: [%s]", l_strNetMsg.c_str());

			return;
		}

		std::string l_strGuid = l_oRequestObj.m_oBody.m_strGuid;
		bool l_bResult = m_pRedisClient->HDel(TIMEOUT_EVENT_INFO, l_strGuid);
		if (l_bResult)
		{
			l_bResult = DeleteTimeOutEvent(l_strGuid);
		} 
		else
		{
			ICC_LOG_ERROR(m_pLog, "Del TimeOutEvent Faied, Guid: [%s]", l_strGuid.c_str());
		}

		PROTOCOL::CBaseRespond l_oRespond;
		std::string l_strRelatedId = l_oRequestObj.m_oHeader.m_strMsgid;
		//l_oRespond.m_oHeader = CreateProtocolHeader(CMD_DEL_TIMEOUT_EVENT_RESPOND, QUEUE_ALARM, SENDTYPE_QUEUE, l_strRelatedId);
		l_oRespond.m_oBody.m_strResult = l_bResult ? Result_Success : Result_Failed;
		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send DelTimeOutEvent Respond:[%s]", l_strMessage.c_str());

		if (l_bResult)
		{
			PROTOCOL::CTimeOutEvent::CBody p_oTimeOutEvent;
			p_oTimeOutEvent.m_strGuid = l_strGuid;

			SendTimeOutEventSync(p_oTimeOutEvent, SYNC_TYPE_DEL);
		}
	}
}
void CBusinessImpl::OnNotifiGetTimeOutEventRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	if (p_pNotify)
	{
		std::string l_strNetMsg = p_pNotify->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Receive Get TimeOut Event Request:[%s]", l_strNetMsg.c_str());

		PROTOCOL::CBaseRequest l_oRequestObj;
		if (!l_oRequestObj.ParseString(l_strNetMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "Invalid Get TimeOut Event Request protocol: [%s]", l_strNetMsg.c_str());

			return;
		}

		std::string l_strRelatedId = l_oRequestObj.m_oHeader.m_strMsgid;
		PROTOCOL::CGetTimeOutEventRespond l_oRespond;
		l_oRespond.m_oHeader = CreateProtocolHeader(CMD_GET_TIMEOUT_EVENT_RESPOND, QUEUE_NOTICE, SENDTYPE_QUEUE, l_strRelatedId);

		std::map<std::string, std::string> l_mapEvent;
		if (m_pRedisClient->HGetAll(TIMEOUT_EVENT_INFO, l_mapEvent))
		{
			for (auto l_oTemp : l_mapEvent)
			{
				PROTOCOL::CTimeOutEvent l_oEventInfo;
				if (l_oEventInfo.ParseString(l_oTemp.second, m_pJsonFty->CreateJson()))
				{
					PROTOCOL::CGetTimeOutEventRespond::CData l_oData;
					l_oData.m_strGuid = l_oEventInfo.m_oBody.m_strGuid;
					l_oData.m_strAlarmId = l_oEventInfo.m_oBody.m_strAlarmId;
					l_oData.m_strCallRefId = l_oEventInfo.m_oBody.m_strCallRefId;
					l_oData.m_strReceiptDeptCode = l_oEventInfo.m_oBody.m_strReceiptDeptCode;

					l_oData.m_strReceiptDeptName = l_oEventInfo.m_oBody.m_strReceiptDeptName;
					l_oData.m_strReceiptCode = l_oEventInfo.m_oBody.m_strReceiptCode;
					l_oData.m_strReceiptName = l_oEventInfo.m_oBody.m_strReceiptName;
					l_oData.m_strReceiptSeatNo = l_oEventInfo.m_oBody.m_strReceiptSeatNo;

					l_oData.m_strStartTime = l_oEventInfo.m_oBody.m_strStartTime;
					l_oData.m_strEndTime = l_oEventInfo.m_oBody.m_strEndTime;
					l_oData.m_strAlarmType = l_oEventInfo.m_oBody.m_strAlarmType;

					l_oRespond.m_oBody.m_vecData.push_back(l_oData);
				}
			}

			l_oRespond.m_oBody.m_strCount = std::to_string(l_mapEvent.size());
		}

		std::string l_strMessage = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "Send GetTimeOutEvent Respond:[%s]", l_strMessage.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PROTOCOL::CHeader CBusinessImpl::CreateProtocolHeader(const std::string& p_strCmd, const std::string& p_strRequest,
	const std::string& p_strRequestType, const std::string& p_strRelatedId)
{
	PROTOCOL::CHeader l_oHeader;

	l_oHeader.m_strSystemID = "ICC";
	l_oHeader.m_strSubsystemID = SERVER_NAME;
	l_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oHeader.m_strRelatedID = p_strRelatedId;
	l_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oHeader.m_strCmd = p_strCmd;
	l_oHeader.m_strRequest = p_strRequest;
	l_oHeader.m_strRequestType = p_strRequestType;
	l_oHeader.m_strResponse = "";
	l_oHeader.m_strResponseType = "";

	return l_oHeader;
}

std::string CBusinessImpl::GetNoticeType(const std::string& p_strInfo)
{
	std::string l_strType;

	PROTOCOL::CNoticeInfo l_oNoticeInfo;
	if (!l_oNoticeInfo.ParseString(p_strInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse string failed, Msg:[%s]", p_strInfo.c_str());

		return l_strType;
	}

	l_strType = l_oNoticeInfo.m_strNoticeType;
	
	return l_strType;
}
std::string CBusinessImpl::GetNoticeInfo(const PROTOCOL::CAddNoticeRequest& p_oRequest, const std::string& p_strNoticeId)
{
	PROTOCOL::CNoticeInfo l_oNoticeInfo;
	l_oNoticeInfo.m_strNoticeId = p_strNoticeId;
	l_oNoticeInfo.m_strUserCode = p_oRequest.m_oBody.m_strUserCode;
	l_oNoticeInfo.m_strUserName = p_oRequest.m_oBody.m_strUserName;
	l_oNoticeInfo.m_strSeatNo = p_oRequest.m_oBody.m_strSeatNo;

	l_oNoticeInfo.m_strDeptCode = p_oRequest.m_oBody.m_strDeptCode;
	l_oNoticeInfo.m_strDeptName = p_oRequest.m_oBody.m_strDeptName;
	l_oNoticeInfo.m_strTargetDeptCode = p_oRequest.m_oBody.m_strTargetDeptCode;
	l_oNoticeInfo.m_strTargetDeptName = p_oRequest.m_oBody.m_strTargetDeptName;

	l_oNoticeInfo.m_strNoticeType = p_oRequest.m_oBody.m_strNoticeType;
	l_oNoticeInfo.m_strTitle = p_oRequest.m_oBody.m_strTitle;
	l_oNoticeInfo.m_strContent = p_oRequest.m_oBody.m_strContent;
	l_oNoticeInfo.m_strTime = p_oRequest.m_oBody.m_strTime;

	return l_oNoticeInfo.ToString(ICCGetIJsonFactory()->CreateJson());
}
bool CBusinessImpl::InsertNotice(const PROTOCOL::CAddNoticeRequest& p_oRequest, const std::string& p_strNoticeId)
{
	DataBase::SQLRequest l_tReqInsertSQL;
	l_tReqInsertSQL.sql_id = "insert_icc_t_notice";

	l_tReqInsertSQL.param["notice_id"] = p_strNoticeId;
	l_tReqInsertSQL.param["user_code"] = p_oRequest.m_oBody.m_strUserCode;
	l_tReqInsertSQL.param["user_name"] = p_oRequest.m_oBody.m_strUserName;
	l_tReqInsertSQL.param["seat_no"] = p_oRequest.m_oBody.m_strSeatNo;

	l_tReqInsertSQL.param["dept_code"] = p_oRequest.m_oBody.m_strDeptCode;
	l_tReqInsertSQL.param["dept_name"] = p_oRequest.m_oBody.m_strDeptName;
	l_tReqInsertSQL.param["target_dept_code"] = p_oRequest.m_oBody.m_strTargetDeptCode;
	l_tReqInsertSQL.param["target_dept_name"] = p_oRequest.m_oBody.m_strTargetDeptName;

	l_tReqInsertSQL.param["notice_type"] = p_oRequest.m_oBody.m_strNoticeType;
	l_tReqInsertSQL.param["title"] = p_oRequest.m_oBody.m_strTitle;
	l_tReqInsertSQL.param["content"] = p_oRequest.m_oBody.m_strContent;
	l_tReqInsertSQL.param["time"] = p_oRequest.m_oBody.m_strTime;
	l_tReqInsertSQL.param["is_delete"] = "false";
	l_tReqInsertSQL.param["create_user"] = SERVER_NAME;
	l_tReqInsertSQL.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tReqInsertSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_notice fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "insert_icc_t_notice success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
bool CBusinessImpl::UpdateNotice(const std::string& p_strNoticeId)
{
	DataBase::SQLRequest l_tUpdateSQL;
	l_tUpdateSQL.sql_id = "update_icc_t_notice";

	l_tUpdateSQL.param["notice_id"] = p_strNoticeId;
	l_tUpdateSQL.set["is_delete"] = "true";
	l_tUpdateSQL.set["update_user"] = SERVER_NAME;
	l_tUpdateSQL.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tUpdateSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_notice fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "update_icc_t_notice success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
bool CBusinessImpl::DeleteNotice(const std::string& p_strNoticeType)
{
	std::map<std::string, std::string> l_mapItem;
	if (m_pRedisClient->HGetAll(NOTICE_INFO, l_mapItem))
	{
		//删除已有的公告/通知
		for (auto l_oTemp : l_mapItem)
		{
			std::string l_strNoticeId = l_oTemp.first;
			std::string l_strNoticeType = GetNoticeType(l_oTemp.second);

			if (l_strNoticeType.compare(p_strNoticeType) == 0)
			{
				if (!UpdateNotice(l_strNoticeId))
				{
					return false;
				}

				m_pRedisClient->HDel(NOTICE_INFO, l_strNoticeId);
			}
		}
	}

	return true;
}
void CBusinessImpl::SendAddNoticeSync(const PROTOCOL::CAddNoticeRequest& p_oRequest)
{
	PROTOCOL::CAddNoticeSync l_oSync;
	l_oSync.m_oHeader = CreateProtocolHeader(CMD_ADD_NOTICE_SYNC, TOPIC_NOTICE, SENDTYPE_TOPIC);

	l_oSync.m_oBody.m_strNoticeId = p_oRequest.m_oHeader.m_strMsgid;
	l_oSync.m_oBody.m_strUserCode = p_oRequest.m_oBody.m_strUserCode;
	l_oSync.m_oBody.m_strUserName = p_oRequest.m_oBody.m_strUserName;
	l_oSync.m_oBody.m_strSeatNo = p_oRequest.m_oBody.m_strSeatNo;
	l_oSync.m_oBody.m_strDeptCode = p_oRequest.m_oBody.m_strDeptCode;
	l_oSync.m_oBody.m_strDeptName = p_oRequest.m_oBody.m_strDeptName;
	l_oSync.m_oBody.m_strTargetDeptCode = p_oRequest.m_oBody.m_strTargetDeptCode;
	l_oSync.m_oBody.m_strTargetDeptName = p_oRequest.m_oBody.m_strTargetDeptName;
	l_oSync.m_oBody.m_strNoticeType = p_oRequest.m_oBody.m_strNoticeType;
	l_oSync.m_oBody.m_strTitle = p_oRequest.m_oBody.m_strTitle;
	l_oSync.m_oBody.m_strContent = p_oRequest.m_oBody.m_strContent;
	l_oSync.m_oBody.m_strTime = p_oRequest.m_oBody.m_strTime;

	std::string l_strMessage = l_oSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "Send AddNoticeSync:[%s]", l_strMessage.c_str());
}

//////////////////////////////////////////////////////////////////////////
std::string CBusinessImpl::GetTipsInfo(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId)
{
	PROTOCOL::CTipsInfo l_oTipsInfo;
	l_oTipsInfo.m_strTipsId = p_strTipsId;
	l_oTipsInfo.m_strCalledNoType = p_oRequest.m_oBody.m_strCalledNoType;
	l_oTipsInfo.m_strLevel = p_oRequest.m_oBody.m_strLevel;
	l_oTipsInfo.m_strFirstType = p_oRequest.m_oBody.m_strFirstType;
	l_oTipsInfo.m_strContent = p_oRequest.m_oBody.m_strContent;

	return l_oTipsInfo.ToString(ICCGetIJsonFactory()->CreateJson());
}
bool CBusinessImpl::InsertTips(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId)
{
	DataBase::SQLRequest l_tReqInsertSQL;
	l_tReqInsertSQL.sql_id = "insert_icc_t_tips";

	l_tReqInsertSQL.param["tips_id"] = p_strTipsId;
	l_tReqInsertSQL.param["called_no_type"] = p_oRequest.m_oBody.m_strCalledNoType;
	l_tReqInsertSQL.param["level"] = p_oRequest.m_oBody.m_strLevel;
	l_tReqInsertSQL.param["first_type"] = p_oRequest.m_oBody.m_strFirstType;
	l_tReqInsertSQL.param["tips_content"] = p_oRequest.m_oBody.m_strContent;

	l_tReqInsertSQL.param["is_delete"] = "false";
	l_tReqInsertSQL.param["create_user"] = SERVER_NAME;
	l_tReqInsertSQL.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tReqInsertSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_tips fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "insert_icc_t_tips success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
bool CBusinessImpl::UpdateTips(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId)
{
	DataBase::SQLRequest l_tUpdateSQL;
	l_tUpdateSQL.sql_id = "update_icc_t_tips";

	l_tUpdateSQL.param["tips_id"] = p_strTipsId;
	l_tUpdateSQL.set["called_no_type"] = p_oRequest.m_oBody.m_strCalledNoType;
	l_tUpdateSQL.set["level"] = p_oRequest.m_oBody.m_strLevel;
	l_tUpdateSQL.set["first_type"] = p_oRequest.m_oBody.m_strFirstType;
	l_tUpdateSQL.set["tips_content"] = p_oRequest.m_oBody.m_strContent;
	l_tUpdateSQL.set["update_user"] = SERVER_NAME;
	l_tUpdateSQL.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tUpdateSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_tips fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "update_icc_t_tips success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
bool CBusinessImpl::DeleteTips(const std::string& p_strTipsId)
{
	DataBase::SQLRequest l_tUpdateSQL;
	l_tUpdateSQL.sql_id = "update_icc_t_tips";

	l_tUpdateSQL.param["tips_id"] = p_strTipsId;
	l_tUpdateSQL.set["is_delete"] = "true";
	l_tUpdateSQL.set["update_user"] = SERVER_NAME;
	l_tUpdateSQL.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tUpdateSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_tips fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "delete_icc_t_tips success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
void CBusinessImpl::SendTipsSync(const PROTOCOL::CSetTipsRequest& p_oRequest, const std::string& p_strTipsId, const std::string& p_strSynType)
{
	PROTOCOL::CSetTipsSync l_oSync;
	l_oSync.m_oHeader = CreateProtocolHeader(CMD_TOPIC_TIPS_SYNC, TOPIC_NOTICE, SENDTYPE_TOPIC);

	l_oSync.m_oBody.m_strTipsId = p_strTipsId;
	l_oSync.m_oBody.m_strCalledNoType = p_oRequest.m_oBody.m_strCalledNoType;
	l_oSync.m_oBody.m_strLevel = p_oRequest.m_oBody.m_strLevel;
	l_oSync.m_oBody.m_strFirstType = p_oRequest.m_oBody.m_strFirstType;
	l_oSync.m_oBody.m_strContent = p_oRequest.m_oBody.m_strContent;
	l_oSync.m_oBody.m_strSyncType = p_strSynType;

	std::string l_strMessage = l_oSync.ToString(ICCGetIJsonFactory()->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "Send TipsSync:[%s]", l_strMessage.c_str());
}
//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::SelectProcessAlarmByAlarmID(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strInAlarmId)
{
	//根据 警情ID 查找调派单位信息

	if (p_strInAlarmId.empty())
	{
		ICC_LOG_ERROR(m_pLog, "AlarmId is Null");

		return false;
	}

	DataBase::SQLRequest l_oSelectAlarmProcess;

	l_oSelectAlarmProcess.sql_id = "select_icc_t_pjdb";
	l_oSelectAlarmProcess.param["alarm_id"] = p_strInAlarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strInAlarmId);
	if (strTime != "")
	{
		l_oSelectAlarmProcess.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_oSelectAlarmProcess.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_oSelectAlarmProcess);
	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery select_icc_t_pjdb Failed ,Error Message :[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}
	while (l_Result->Next())
	{
		PROTOCOL::CUrgesRequest::CBody l_oData;
		l_oData.m_strAlarmId = l_Result->GetValue("alarm_id");
		l_oData.m_strProcessId = l_Result->GetValue("id");
		l_oData.m_strDispatchDeptCode = l_Result->GetValue("dispatch_dept_code");
		l_oData.m_strDispatchDeptName = l_Result->GetValue("dispatch_dept_name");
		l_oData.m_strProcessDeptCode = l_Result->GetValue("process_dept_code");
		l_oData.m_strProcessDeptName = l_Result->GetValue("process_dept_name");

		p_outVecData.push_back(l_oData);
	}

	return true;
}
void CBusinessImpl::SendUrgesSync(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strContent)
{
	for (auto l_oData : p_outVecData)
	{
		PROTOCOL::CUrgesRequest l_oSync;
		l_oSync.m_oHeader = CreateProtocolHeader(CMD_ADD_URGES_SYNC, TOPIC_NOTICE, SENDTYPE_TOPIC);

		l_oSync.m_oBody.m_strAlarmId = l_oData.m_strAlarmId;
		l_oSync.m_oBody.m_strProcessId = l_oData.m_strProcessId;
		l_oSync.m_oBody.m_strDispatchDeptCode = l_oData.m_strDispatchDeptCode; 
		l_oSync.m_oBody.m_strDispatchDeptName = l_oData.m_strDispatchDeptName;
		l_oSync.m_oBody.m_strProcessDeptCode = l_oData.m_strProcessDeptCode;
		l_oSync.m_oBody.m_strProcessDeptName = l_oData.m_strProcessDeptName;
		l_oSync.m_oBody.m_strContent = p_strContent;

		std::string l_strMessage = l_oSync.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "Send UrgesSync:[%s]", l_strMessage.c_str());
	}
}
void CBusinessImpl::AddUrgesAlarmLog(std::vector<PROTOCOL::CUrgesRequest::CBody> &p_outVecData, const std::string& p_strContent)
{
	for (auto l_oData : p_outVecData)
	{
		//<Resource Key = "BS001007002" Value = "[调派单位-{0}]发起催促[处警单位-{1}, 处警单-{2}, 催促内容-{3}]"/>
		PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
		
		std::vector<std::string> l_vecParamList;
		l_vecParamList.push_back(l_oData.m_strDispatchDeptName);
		l_vecParamList.push_back(l_oData.m_strProcessDeptName);
		l_vecParamList.push_back(l_oData.m_strProcessId);
		l_vecParamList.push_back(p_strContent);
		l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
		l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
		l_oAlarmLogInfo.m_strAlarmID = l_oData.m_strAlarmId;
		l_oAlarmLogInfo.m_strOperate = LOG_URGES_ALARM;
		std::string type = LOG_ALARM_RESOURCETYPE;
		l_oAlarmLogInfo.m_strOperateAttachDesc = GenAlarmLogAttach(type, l_oData.m_strProcessId);
		l_oAlarmLogInfo.m_strCreateUser = l_oData.m_strDispatchDeptName;
		l_oAlarmLogInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
		l_oAlarmLogInfo.m_strSourceName = "icc";

		if (InsertDBAlarmLogInfo(l_oAlarmLogInfo))
		{
			SendAlarmLogInfoSync(l_oAlarmLogInfo);
		} 
	}
}

std::string CBusinessImpl::GenAlarmLogContent(std::vector<std::string> p_vecParamList)
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

std::string CBusinessImpl::GenAlarmLogAttach(std::string& type, std::string& id)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

	std::string l_strPath = "/";
	std::string l_strNum = std::to_string(0);

	if (0 == type.compare("7"))
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", "jjlyh");
	}
	else
	{
		l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", type);
	}

	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/id", id);
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/name", "");
	l_pIJson->SetNodeValue(l_strPath + l_strNum + "/path", "");

	return l_pIJson->ToString();
}

bool CBusinessImpl::InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		//TODO::select_icc_t_jjdb_jjsj 查询jjsj
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_strAlarmID);

		if (strTime != "")
		{
			DataBase::SQLRequest l_sqlReqeust;
			l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
			l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
			DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
			if (!l_pRSetPtr->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
			}
			if (l_pRSetPtr->Next())
			{
				p_AlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
			}
		}
	}

	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		p_AlarmLogInfo.m_strReceivedTime = l_strCurTime;
	}

	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		return false;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	return true;
}

void CBusinessImpl::SendAlarmLogInfoSync(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader = CreateProtocolHeader(CMD_ALARM_LOG_SYNC, TOPIC_ALARM, SENDTYPE_TOPIC);

	l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
	l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
	l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
	l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
	l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
	l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
	l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
	l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
	l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
	l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
	l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
	l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
	l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
	l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
	l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
	l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
	l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "Send AlarmLogSync: [%s]", l_strMessage.c_str());
}

//////////////////////////////////////////////////////////////////////////
bool CBusinessImpl::SelectAlarmByCallRefId(PROTOCOL::CTimeOutEvent::CBody& p_oOutAlarm, const std::string& p_strInCallRefId, std::string p_strTalkTime)
{
	DataBase::SQLRequest l_oSelectAlarm;

	l_oSelectAlarm.sql_id = "select_icc_t_jjdb";
	l_oSelectAlarm.param["source_id"] = p_strInCallRefId;

	l_oSelectAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(p_strTalkTime, 172800);
	l_oSelectAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(p_strTalkTime, 172800);

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_oSelectAlarm);
	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery select_icc_t_alarm Failed ,Error Message :[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		p_oOutAlarm.m_strAlarmId = l_Result->GetValue("id");
		p_oOutAlarm.m_strReceiptDeptCode = l_Result->GetValue("receipt_dept_code");
		p_oOutAlarm.m_strReceiptDeptName = l_Result->GetValue("receipt_dept_name");
		p_oOutAlarm.m_strReceiptCode = l_Result->GetValue("receipt_code");
		p_oOutAlarm.m_strReceiptName = l_Result->GetValue("receipt_name");
		p_oOutAlarm.m_strReceiptSeatNo = l_Result->GetValue("receipt_seatno");
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "Not found alarm by callrefid: [%s], SQL :[%s]", p_strInCallRefId.c_str(), l_Result->GetSQL().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::SelectAlarmByCallRefId(PROTOCOL::CTimeOutEvent::CBody& p_oOutAlarm, const std::string& p_strInCallRefId)
{
	DataBase::SQLRequest l_oSelectAlarm;

	l_oSelectAlarm.sql_id = "select_icc_t_jjdb";
	l_oSelectAlarm.param["source_id"] = p_strInCallRefId;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_oSelectAlarm);
	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery select_icc_t_alarm Failed ,Error Message :[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}

	if (l_Result->Next())
	{
		p_oOutAlarm.m_strAlarmId = l_Result->GetValue("id");
		p_oOutAlarm.m_strReceiptDeptCode = l_Result->GetValue("receipt_dept_code");
		p_oOutAlarm.m_strReceiptDeptName = l_Result->GetValue("receipt_dept_name");
		p_oOutAlarm.m_strReceiptCode = l_Result->GetValue("receipt_code");
		p_oOutAlarm.m_strReceiptName = l_Result->GetValue("receipt_name");
		p_oOutAlarm.m_strReceiptSeatNo = l_Result->GetValue("receipt_seatno");
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "Not found alarm by callrefid: [%s], SQL :[%s]", p_strInCallRefId.c_str(), l_Result->GetSQL().c_str());
		return false;
	}

	return true;
}

std::string CBusinessImpl::GetTimeOutEventInfo(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent)
{
	PROTOCOL::CTimeOutEvent l_oAlarmInfo;

	l_oAlarmInfo.m_oBody.m_strGuid = p_oTimeOutEvent.m_strGuid;
	l_oAlarmInfo.m_oBody.m_strAlarmId = p_oTimeOutEvent.m_strAlarmId;
	l_oAlarmInfo.m_oBody.m_strCallRefId = p_oTimeOutEvent.m_strCallRefId;
	l_oAlarmInfo.m_oBody.m_strReceiptDeptCode = p_oTimeOutEvent.m_strReceiptDeptCode;
	l_oAlarmInfo.m_oBody.m_strReceiptDeptName = p_oTimeOutEvent.m_strReceiptDeptName;
	l_oAlarmInfo.m_oBody.m_strReceiptCode = p_oTimeOutEvent.m_strReceiptCode;
	l_oAlarmInfo.m_oBody.m_strReceiptName = p_oTimeOutEvent.m_strReceiptName;
	l_oAlarmInfo.m_oBody.m_strReceiptSeatNo = p_oTimeOutEvent.m_strReceiptSeatNo;
	l_oAlarmInfo.m_oBody.m_strStartTime = p_oTimeOutEvent.m_strStartTime;
	l_oAlarmInfo.m_oBody.m_strEndTime = p_oTimeOutEvent.m_strEndTime;
	l_oAlarmInfo.m_oBody.m_strAlarmType = p_oTimeOutEvent.m_strAlarmType;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmInfo.ToString(l_pIJson);

	return l_strMessage;
}
bool CBusinessImpl::InsertTimeOutEvent(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent)
{
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_timeout_event";

	l_tSQLReqInsertAlarm.param["guid"] = p_oTimeOutEvent.m_strGuid;
	l_tSQLReqInsertAlarm.param["alarm_id"] = p_oTimeOutEvent.m_strAlarmId;
	l_tSQLReqInsertAlarm.param["callref_id"] = p_oTimeOutEvent.m_strCallRefId;
	l_tSQLReqInsertAlarm.param["receipt_dept_code"] = p_oTimeOutEvent.m_strReceiptDeptCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_name"] = p_oTimeOutEvent.m_strReceiptDeptName;
	l_tSQLReqInsertAlarm.param["receipt_code"] = p_oTimeOutEvent.m_strReceiptCode;
	l_tSQLReqInsertAlarm.param["receipt_name"] = p_oTimeOutEvent.m_strReceiptName;
	l_tSQLReqInsertAlarm.param["receipt_seatno"] = p_oTimeOutEvent.m_strReceiptSeatNo;
	l_tSQLReqInsertAlarm.param["start_time"] = p_oTimeOutEvent.m_strStartTime;
	l_tSQLReqInsertAlarm.param["end_time"] = p_oTimeOutEvent.m_strEndTime;
	l_tSQLReqInsertAlarm.param["alarm_type"] = p_oTimeOutEvent.m_strAlarmType;

	l_tSQLReqInsertAlarm.param["create_user"] = SERVER_NAME;
	l_tSQLReqInsertAlarm.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	//l_tSQLReqInsertAlarm.param["update_user"] = SERVER_NAME;
	//l_tSQLReqInsertAlarm.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Insert icc_t_timeout_event failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_timeout_event success,sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
bool CBusinessImpl::DeleteTimeOutEvent(const std::string& p_strTimeOutEventId)
{
	DataBase::SQLRequest l_tUpdateSQL;
	l_tUpdateSQL.sql_id = "update_icc_t_timeout_event";

	l_tUpdateSQL.param["guid"] = p_strTimeOutEventId;
	l_tUpdateSQL.set["is_delete"] = "true";
	l_tUpdateSQL.set["update_user"] = SERVER_NAME;
	l_tUpdateSQL.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tUpdateSQL);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_timeout_event fialed, sql:[%s], ErrorMsg:[%s]",
			l_pRSet->GetSQL().c_str(), l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "delete_icc_t_timeout_event success, sql:[%s]", l_pRSet->GetSQL().c_str());
	}

	return true;
}
void CBusinessImpl::SendTimeOutEventSync(const PROTOCOL::CTimeOutEvent::CBody& p_oTimeOutEvent, const std::string& p_strSynType)
{
	PROTOCOL::CTimeOutEvent l_oAlarmSync;
	l_oAlarmSync.m_oHeader = CreateProtocolHeader(CMD_TIMEOUT_EVENT_SYNC, TOPIC_NOTICE, SENDTYPE_TOPIC);

	l_oAlarmSync.m_oBody.m_strGuid = p_oTimeOutEvent.m_strGuid;
	l_oAlarmSync.m_oBody.m_strAlarmId = p_oTimeOutEvent.m_strAlarmId;
	l_oAlarmSync.m_oBody.m_strCallRefId = p_oTimeOutEvent.m_strCallRefId;
	l_oAlarmSync.m_oBody.m_strReceiptDeptCode = p_oTimeOutEvent.m_strReceiptDeptCode;
	l_oAlarmSync.m_oBody.m_strReceiptDeptName = p_oTimeOutEvent.m_strReceiptDeptName;
	l_oAlarmSync.m_oBody.m_strReceiptCode = p_oTimeOutEvent.m_strReceiptCode;
	l_oAlarmSync.m_oBody.m_strReceiptName = p_oTimeOutEvent.m_strReceiptName;
	l_oAlarmSync.m_oBody.m_strReceiptSeatNo = p_oTimeOutEvent.m_strReceiptSeatNo;
	l_oAlarmSync.m_oBody.m_strStartTime = p_oTimeOutEvent.m_strStartTime;
	l_oAlarmSync.m_oBody.m_strEndTime = p_oTimeOutEvent.m_strEndTime;
	l_oAlarmSync.m_oBody.m_strAlarmType = p_oTimeOutEvent.m_strAlarmType;
	l_oAlarmSync.m_oBody.m_strSyncType = p_strSynType;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "Send TimeOutEventSync: [%s]", l_strMessage.c_str());
}
void CBusinessImpl::ProcessTimeOutEvent(const std::string& p_strCallRefId, const std::string& p_strStartTime, const std::string& p_strEndTime, const std::string& p_strAlarmType/* = "0"*/)
{
	PROTOCOL::CTimeOutEvent::CBody l_oAlarmData;
	if (SelectAlarmByCallRefId(l_oAlarmData, p_strCallRefId, p_strStartTime))
	{
		std::string l_strGuid = m_pString->CreateGuid();

		l_oAlarmData.m_strGuid = l_strGuid;
		l_oAlarmData.m_strCallRefId = p_strCallRefId;
		l_oAlarmData.m_strStartTime = p_strStartTime;
		l_oAlarmData.m_strEndTime = p_strEndTime;
		l_oAlarmData.m_strAlarmType = p_strAlarmType;

		std::string l_strVal = GetTimeOutEventInfo(l_oAlarmData);
		if (m_pRedisClient->HSet(TIMEOUT_EVENT_INFO, l_strGuid, l_strVal))
		{
			if (InsertTimeOutEvent(l_oAlarmData))
			{
				SendTimeOutEventSync(l_oAlarmData, SYNC_TYPE_ADD);
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Insert HSet failed, CallRefId: [%s]", p_strCallRefId.c_str());
		}
	}
}
