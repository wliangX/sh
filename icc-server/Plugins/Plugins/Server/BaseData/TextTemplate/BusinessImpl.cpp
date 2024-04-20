#include "Boost.h"
#include "BusinessImpl.h"
#include <iostream>
#include <thread>

#define LOAD_WAITTIME 1000

const std::string CBusinessImpl::TextTemplate("TextTemplates");

#define SYSTEMID ("icc_server")
#define SUBSYSTEMID ("icc_server_basedata_TextTemplate")

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

	m_isConnectDB = false;
	m_isConnectMQ = false;

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	while (!LoadTextTemplateInfo())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.texttemplate failed to load texttemplate info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_text_template_request", OnNotifiGetAllTextTemplate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_text_template_request", OnNotifiSetOneTextTemplate);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_text_template_request", OnNotifiDelTextTemplate);

	//等待数据库连接成功后再进行后续操作

	//LoadTextTemplateInfo();

	ICC_LOG_INFO(m_pLog, "plugin basedata.texttemplate start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	//m_pRedisClient->Disconnect();

	ICC_LOG_INFO(m_pLog, "texttemplate stop success");
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
bool CBusinessImpl::LoadTextTemplateInfo()
{
	m_pRedisClient->Del(TextTemplate);

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_text_template";

	ICC_LOG_DEBUG(m_pLog, "LodingTextTemplateInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	if (l_result->RecordSize() > 0)
	{
		//初始化文本模板信息缓存
		while (l_result->Next())
		{
			CTextTemplate l_oTextTemplateInfo;
			l_oTextTemplateInfo.m_strGuid = l_result->GetValue("guid");
			l_oTextTemplateInfo.m_strType = l_result->GetValue("type");
			l_oTextTemplateInfo.m_strContent = l_result->GetValue("content");
			l_oTextTemplateInfo.m_strAlarm_type = l_result->GetValue("alarm_type");
			l_oTextTemplateInfo.m_strHandly_type = l_result->GetValue("handly_type");
			std::string l_strVal = l_oTextTemplateInfo.ToJson(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(TextTemplate, l_oTextTemplateInfo.m_strGuid, l_strVal);
		}
	}
	ICC_LOG_DEBUG(m_pLog, "Loding TextTemplateInfo Success,TextTemplate Size[%d]", l_result->RecordSize());
	return true;
}

//获取所有文本模板信息
void CBusinessImpl::OnNotifiGetAllTextTemplate(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CTextTemplate request;
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnNotifiGetAllCanTextTemplateDept]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	std::string l_strPageSize = request.m_oBody.m_strPageSize;
	std::string l_strPageIndex = request.m_oBody.m_strPageIndex;
	int l_nPageSize = std::atoi(l_strPageSize.c_str());
	int l_nPageIndex = std::atoi(l_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;

	std::map<std::string, std::string> l_mapTextTemplate;
	m_pRedisClient->HGetAll(TextTemplate, l_mapTextTemplate);
	ICC_LOG_DEBUG(m_pLog, "[OnNotifiGetAllTextTemplate] HGetAll complete! msgid = %s, count = %d", request.m_oHeader.m_strMsgId.c_str(), l_mapTextTemplate.size());

	PROTOCOL::CTextTemplate response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;
	response.m_oBody.m_strPageIndex = l_strPageIndex;

	if (l_mapTextTemplate.size() == 0)
	{
		ICC_LOG_WARNING(m_pLog, "GetAllTextTemplate Error : No Search Data");

		response.m_oBody.m_strCount = "0";
		response.m_oBody.m_strAllCount = "0";
		PROTOCOL::CTextTemplate::Text l_oData;
		l_oData.m_strContent = "";
		l_oData.m_strGuid = "";
		l_oData.m_strType = "";
		l_oData.m_strAlarm_type = "";
		l_oData.m_strHandly_type = "";
		response.m_oBody.m_vecData.push_back(l_oData);
	}
	else
	{
		int l_nBeginIndex = (l_nPageIndex - 1) * l_nPageSize;
		int l_nEndIndex = l_nPageIndex * l_nPageSize;
		if (l_nBeginIndex > l_mapTextTemplate.size())
		{
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "Out of range";
			response.m_oBody.m_strCount = "0";
		}
		else
		{
			if (l_nEndIndex > l_mapTextTemplate.size())
			{
				l_nEndIndex = l_mapTextTemplate.size();
			}

			std::vector<PROTOCOL::CTextTemplate::Text> l_vecTempData;

			CTextTemplate l_oMapTextTemplate;
			for (auto iter = l_mapTextTemplate.begin(); iter != l_mapTextTemplate.end(); ++iter)
			{
				if (l_oMapTextTemplate.Parse(iter->second, l_pJson))
				{
					if (request.m_oBody.m_data.m_strType.empty())
					{
						PROTOCOL::CTextTemplate::Text l_oData;

						l_oData.m_strGuid = l_oMapTextTemplate.m_strGuid;
						l_oData.m_strType = l_oMapTextTemplate.m_strType;
						l_oData.m_strContent = l_oMapTextTemplate.m_strContent;
						l_oData.m_strAlarm_type = l_oMapTextTemplate.m_strAlarm_type;
						l_oData.m_strHandly_type = l_oMapTextTemplate.m_strHandly_type;
						l_vecTempData.push_back(l_oData);
					}
					else
					{
						if (request.m_oBody.m_data.m_strType == l_oMapTextTemplate.m_strType)
						{
							PROTOCOL::CTextTemplate::Text l_oData;

							l_oData.m_strGuid = l_oMapTextTemplate.m_strGuid;
							l_oData.m_strType = l_oMapTextTemplate.m_strType;
							l_oData.m_strContent = l_oMapTextTemplate.m_strContent;
							l_oData.m_strAlarm_type = l_oMapTextTemplate.m_strAlarm_type;
							l_oData.m_strHandly_type = l_oMapTextTemplate.m_strHandly_type;
							l_vecTempData.push_back(l_oData);
						}
					}				

				}
			}

			for (auto it = l_vecTempData.begin() + l_nBeginIndex;
				it < l_vecTempData.begin() + l_nEndIndex && it != l_vecTempData.end(); it++)
			{
				response.m_oBody.m_vecData.push_back(*it);
			}

			response.m_oBody.m_strCount = std::to_string(response.m_oBody.m_vecData.size());
			response.m_oBody.m_strAllCount = std::to_string(l_vecTempData.size());
		}
	}

	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send GetAllTextTemplateRespond [%s]", l_strMessage.c_str());
}

//设置某一个文本模板信息
void CBusinessImpl::OnNotifiSetOneTextTemplate(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CTextTemplate request;
	JsonParser::IJsonPtr l_IJson = m_pJsonFty->CreateJson();
	if (!request.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "SetOneTextTemplate Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	ICC_LOG_LOWDEBUG(m_pLog, "receive SetOneTextTemplateRequest [%s]", p_pNotify->GetMessages().c_str());
	//GenRespHeader("set_text_template_respond", request.m_oHeader, response.m_oHeader);
	bool l_bExistInCache = false;
	std::map<std::string, std::string> l_mapTextTemplate;
	m_pRedisClient->HGetAll(TextTemplate, l_mapTextTemplate);

	PROTOCOL::CTextTemplate response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;

	CTextTemplate l_oTextTemplate;
	for (auto iter = l_mapTextTemplate.begin(); iter != l_mapTextTemplate.end(); ++iter)
	{
		if (l_oTextTemplate.Parse(iter->second, m_pJsonFty->CreateJson()))
		{
			if (l_oTextTemplate.m_strGuid == request.m_oBody.m_data.m_strGuid)
			{
				//更新操作
				DataBase::SQLRequest l_oUpDate;

				l_oUpDate.sql_id = "update_icc_t_text_template";
				l_oUpDate.param["guid"] = request.m_oBody.m_data.m_strGuid;
				l_oUpDate.set["type"] = request.m_oBody.m_data.m_strType;
				l_oUpDate.set["content"] = request.m_oBody.m_data.m_strContent;
				l_oUpDate.set["alarm_type"] = request.m_oBody.m_data.m_strAlarm_type;
				l_oUpDate.set["handly_type"] = request.m_oBody.m_data.m_strHandly_type;
				l_oUpDate.set["update_user"] = "BaseData Server";
				l_oUpDate.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
				DataBase::IResultSetPtr l_oSQLptr = m_pDBConn->Exec(l_oUpDate);
				ICC_LOG_INFO(m_pLog, "sql:[%s]", l_oSQLptr->GetSQL().c_str());
				if (!l_oSQLptr->IsValid())
				{
					ICC_LOG_ERROR(m_pLog, "data is invalid,update operation failed[DB Error:%s]", l_oSQLptr->GetErrorMsg().c_str());
					//ICC_LOG_ERROR(m_pLog, "data is invalid,update operation failed");
					//response.m_oBody.m_strResult = "1";
					response.m_oHeader.m_strResult = "1";
					response.m_oHeader.m_strMsg = "data is invalid,update operation failed";
					std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
					p_pNotify->Response(l_strMessage);
					ICC_LOG_DEBUG(m_pLog, "send SetOneTextTemplateRespond [%s]", l_strMessage.c_str());
				}
				else
				{
					l_oTextTemplate.m_strContent = request.m_oBody.m_data.m_strContent;
					l_oTextTemplate.m_strType = request.m_oBody.m_data.m_strType;
					l_oTextTemplate.m_strAlarm_type = request.m_oBody.m_data.m_strAlarm_type;
					l_oTextTemplate.m_strHandly_type = request.m_oBody.m_data.m_strHandly_type;

					std::string l_strVal = l_oTextTemplate.ToJson(m_pJsonFty->CreateJson());
					m_pRedisClient->HSet(TextTemplate, l_oTextTemplate.m_strGuid, l_strVal);
					ICC_LOG_DEBUG(m_pLog, "update operation success");

					std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
					p_pNotify->Response(l_strMessage);
					ICC_LOG_DEBUG(m_pLog, "send SetOneTextTemplateRespond [%s]", l_strMessage.c_str());

					//response.m_oBody.m_strResult = "0";
					l_bExistInCache = true;
					//发送同步
					PROTOCOL::CTextTemplateSync l_oTextTemplateSync;
					l_oTextTemplateSync.m_oHeader.m_strSystemID = SYSTEMID;
					l_oTextTemplateSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
					l_oTextTemplateSync.m_oHeader.m_strMsgid = request.m_oBody.m_data.m_strGuid;
					l_oTextTemplateSync.m_oHeader.m_strRelatedID = "";
					l_oTextTemplateSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
					l_oTextTemplateSync.m_oHeader.m_strCmd = "text_template_sync";
					l_oTextTemplateSync.m_oHeader.m_strRequest = "topic_basedata_sync";
					l_oTextTemplateSync.m_oHeader.m_strRequestType = "1";
					l_oTextTemplateSync.m_oHeader.m_strResponse = "";
					l_oTextTemplateSync.m_oHeader.m_strResponseType = "";
					l_oTextTemplateSync.m_oBody.m_strSyncType = "2";
					l_oTextTemplateSync.m_oBody.m_strGuid = request.m_oBody.m_data.m_strGuid;
					l_oTextTemplateSync.m_oBody.m_strType = request.m_oBody.m_data.m_strType;
					l_oTextTemplateSync.m_oBody.m_strContent = request.m_oBody.m_data.m_strContent;
					l_oTextTemplateSync.m_oBody.m_strAlarm_type = request.m_oBody.m_data.m_strAlarm_type;
					l_oTextTemplateSync.m_oBody.m_strHandly_type = request.m_oBody.m_data.m_strHandly_type;

					JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
					std::string l_strSyncMessage = l_oTextTemplateSync.ToString(l_pIJson);
					m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
					ICC_LOG_DEBUG(m_pLog, "Send TextTemplateSync [%s]", l_strSyncMessage.c_str());
					break;
				}
			}
		}
	}
	if (!l_bExistInCache)
	{
		//新增操作
		DataBase::SQLRequest l_oAdd;
		std::string l_strGuid = m_pString->CreateGuid();
		l_oAdd.sql_id = "insert_icc_t_text_template";
		l_oAdd.param["guid"] = l_strGuid;
		l_oAdd.param["type"] = request.m_oBody.m_data.m_strType;
		l_oAdd.param["content"] = request.m_oBody.m_data.m_strContent;
		l_oAdd.param["alarm_type"] = request.m_oBody.m_data.m_strAlarm_type;
		l_oAdd.param["handly_type"] = request.m_oBody.m_data.m_strHandly_type;
		l_oAdd.param["create_user"] = "BaseData Server";
		l_oAdd.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oAdd.param["update_user"] = "BaseData Server";
		l_oAdd.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
		
		DataBase::IResultSetPtr p_Add = m_pDBConn->Exec(l_oAdd);
		ICC_LOG_INFO(m_pLog, "sql:[%s]", p_Add->GetSQL().c_str());
		if (!p_Add->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "data is invalid,insert operation failed [DB Error]:%s", p_Add->GetErrorMsg().c_str());
			//response.m_oBody.m_strResult = "1";
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "data is invalid,update operation failed";
			std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			//ICC_LOG_ERROR(m_pLog, "data is invalid,set operation failed[DB Error:%s]", p_Add->GetErrorMsg().c_str());
			ICC_LOG_DEBUG(m_pLog, "send SetOneTextTemplateRespond [%s]", l_strMessage.c_str());

		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "insert operation success");
			l_oTextTemplate.m_strGuid = l_strGuid;
			l_oTextTemplate.m_strType = request.m_oBody.m_data.m_strType;
			l_oTextTemplate.m_strContent = request.m_oBody.m_data.m_strContent;
			l_oTextTemplate.m_strAlarm_type = request.m_oBody.m_data.m_strAlarm_type;
			l_oTextTemplate.m_strHandly_type = request.m_oBody.m_data.m_strHandly_type;

			std::string l_strVal = l_oTextTemplate.ToJson(m_pJsonFty->CreateJson());
			m_pRedisClient->HSet(TextTemplate.c_str(), l_oTextTemplate.m_strGuid.c_str(), l_strVal);
			//response.m_oBody.m_strResult = "0";

			std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send SetOneTextTemplateRespond [%s]", l_strMessage.c_str());

			//发送同步
			PROTOCOL::CTextTemplateSync l_oTextTemplateSync;
			l_oTextTemplateSync.m_oHeader.m_strSystemID = SYSTEMID;
			l_oTextTemplateSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
			l_oTextTemplateSync.m_oHeader.m_strMsgid = request.m_oBody.m_data.m_strGuid;
			l_oTextTemplateSync.m_oHeader.m_strRelatedID = "";
			l_oTextTemplateSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oTextTemplateSync.m_oHeader.m_strCmd = "text_template_sync";
			l_oTextTemplateSync.m_oHeader.m_strRequest = "topic_basedata_sync";
			l_oTextTemplateSync.m_oHeader.m_strRequestType = "1";
			l_oTextTemplateSync.m_oHeader.m_strResponse = "";
			l_oTextTemplateSync.m_oHeader.m_strResponseType = "";
			l_oTextTemplateSync.m_oBody.m_strSyncType = "1";
			l_oTextTemplateSync.m_oBody.m_strGuid = l_strGuid;
			l_oTextTemplateSync.m_oBody.m_strType = request.m_oBody.m_data.m_strType;
			l_oTextTemplateSync.m_oBody.m_strContent = request.m_oBody.m_data.m_strContent;
			l_oTextTemplateSync.m_oBody.m_strAlarm_type = request.m_oBody.m_data.m_strAlarm_type;
			l_oTextTemplateSync.m_oBody.m_strHandly_type = request.m_oBody.m_data.m_strHandly_type;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strSyncMessage = l_oTextTemplateSync.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
			ICC_LOG_DEBUG(m_pLog, "Send TextTemplateSync [%s]", l_strSyncMessage.c_str());
		}
	}
}

//删除某一个文本模板信息
void CBusinessImpl::OnNotifiDelTextTemplate(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	JsonParser::IJsonPtr l_pJsonPtr = m_pJsonFty->CreateJson();
	PROTOCOL::CTextTemplate request;
	if (!request.ParseString(p_pNotify->GetMessages(), l_pJsonPtr))
	{
		ICC_LOG_ERROR(m_pLog, "DelTextTemplate Request Not Json:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	//GenRespHeader("delete_text_template_respond", request.m_oHeader, response.m_oHeader);
	//std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	PROTOCOL::CTextTemplate response;
	response.m_oHeader.m_strMsgId = request.m_oHeader.m_strMsgId;

	std::string l_strTextTemplate;
	m_pRedisClient->HGet(TextTemplate, request.m_oBody.m_data.m_strGuid, l_strTextTemplate);
	CTextTemplate l_oTextTemplate;
	if (!l_oTextTemplate.Parse(l_strTextTemplate, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse json failed");

		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "parse json failed";

		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DelTextTemplateRespond [%s]", l_strMessage.c_str());

		return;
	}

	//删除操作
	DataBase::SQLRequest l_oDelete;
	l_oDelete.sql_id = "delete_icc_t_text_template";
	l_oDelete.param["guid"] = request.m_oBody.m_data.m_strGuid;
	DataBase::IResultSetPtr l_pAdd = m_pDBConn->Exec(l_oDelete);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pAdd->GetSQL().c_str());

	if (!l_pAdd->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "data is invalid,delete operation failed:[DB Error]:%s", l_pAdd->GetErrorMsg().c_str());

		response.m_oHeader.m_strResult = "1";
		response.m_oHeader.m_strMsg = "data is invalid,delete operation failed";

		std::string l_strMessage = response.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send DelTextTemplateRespond [%s]", l_strMessage.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "delete operation success");
		//response.m_oBody.m_strResult = "0";

		//删除缓存
		if (m_pRedisClient->HDel(TextTemplate, l_oTextTemplate.m_strGuid))
		{
			std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send DelTextTemplateRespond [%s]", l_strMessage.c_str());

			//发送同步
			PROTOCOL::CTextTemplateSync l_oTextTemplateSync;
			l_oTextTemplateSync.m_oHeader.m_strSystemID = SYSTEMID;
			l_oTextTemplateSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
			l_oTextTemplateSync.m_oHeader.m_strMsgid = request.m_oBody.m_data.m_strGuid;
			l_oTextTemplateSync.m_oHeader.m_strRelatedID = "";
			l_oTextTemplateSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oTextTemplateSync.m_oHeader.m_strCmd = "text_template_sync";
			l_oTextTemplateSync.m_oHeader.m_strRequest = "topic_basedata_sync";
			l_oTextTemplateSync.m_oHeader.m_strRequestType = "1";
			l_oTextTemplateSync.m_oHeader.m_strResponse = "";
			l_oTextTemplateSync.m_oHeader.m_strResponseType = "";
			l_oTextTemplateSync.m_oBody.m_strSyncType = "3";
			l_oTextTemplateSync.m_oBody.m_strGuid = request.m_oBody.m_data.m_strGuid;
			l_oTextTemplateSync.m_oBody.m_strType = l_oTextTemplate.m_strType;
			l_oTextTemplateSync.m_oBody.m_strContent = l_oTextTemplate.m_strContent;
			l_oTextTemplateSync.m_oBody.m_strAlarm_type = request.m_oBody.m_data.m_strAlarm_type;
			l_oTextTemplateSync.m_oBody.m_strHandly_type = request.m_oBody.m_data.m_strHandly_type;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strSyncMessage = l_oTextTemplateSync.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));
			ICC_LOG_DEBUG(m_pLog, "send DelTextTemplateSync [%s]", l_strSyncMessage.c_str());
		}
		else
		{
			response.m_oHeader.m_strResult = "1";
			response.m_oHeader.m_strMsg = "delete cache failed";

			std::string l_strMessage = response.ToStringResponse(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send DelTextTemplateRespond [%s]", l_strMessage.c_str());
		}
	}
}

