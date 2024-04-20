#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

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

	//m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);	

	/*while (!_LoadKeyWordFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.keyword failed to load keyword info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_frequent_contact", OnCNotifiAddRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_frequent_contact", OnCNotifiQueryRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_frequent_contact", OnNotifiDeleteRequest);

	ICC_LOG_INFO(m_pLog, "plugin basedata.frequentcontacts start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "frequentcontacts stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiAddRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CAddContactsRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;

	l_SQLRequest.param["staff_code"] = l_request.m_oBody.m_staff_code;
	l_SQLRequest.param["contact_number"] = l_request.m_oBody.m_contact_number;
	l_SQLRequest.param["contact_name"] = l_request.m_oBody.m_contact_name;
	l_SQLRequest.param["contact_dept"] = l_request.m_oBody.m_contact_dept;
	l_SQLRequest.param["contact_number2"] = l_request.m_oBody.m_contact_number2;
	l_SQLRequest.param["contact_number3"] = l_request.m_oBody.m_contact_number3;
	l_SQLRequest.param["contact_number4"] = l_request.m_oBody.m_contact_number4;
	l_SQLRequest.param["contact_guid"] = l_request.m_oBody.m_contact_guid;

	if (l_request.m_oBody.m_guid.empty())
	{
		l_SQLRequest.sql_id = "insert_icc_t_frequent_contacts";
		l_SQLRequest.param["guid"] = m_pString->CreateGuid();
		l_SQLRequest.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else
	{
		l_SQLRequest.sql_id = "update_icc_t_frequent_contacts";
		l_SQLRequest.param["guid"] = l_request.m_oBody.m_guid;
		l_SQLRequest.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}	
	
	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute insert_icc_t_frequent_contacts failed";
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_frequent_contacts failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
	
}

void CBusinessImpl::OnCNotifiQueryRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CQueryContactsRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_icc_t_frequent_contacts";
	l_SQLRequest.param["staff_code"] = l_request.m_oBody.m_staff_code;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	//回复消息
	PROTOCOL::CQueryContactsResponse response;
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	while (l_Result->Next())
	{
		PROTOCOL::CQueryContactsResponse::CData data;
		data.m_guid = l_Result->GetValue("guid");
		data.m_staff_code = l_Result->GetValue("staff_code");
		data.m_contact_number = l_Result->GetValue("contact_number");
		data.m_contact_name = l_Result->GetValue("contact_name");
		data.m_contact_dept = l_Result->GetValue("contact_dept");
		data.m_contact_number2 = l_Result->GetValue("contact_number2");
		data.m_contact_number3 = l_Result->GetValue("contact_number3");
		data.m_contact_number4 = l_Result->GetValue("contact_number4");
		data.m_contact_guid = l_Result->GetValue("contact_guid");
		
		response.m_oBody.m_vecData.push_back(data);

	}
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
	
}

void CBusinessImpl::OnNotifiDeleteRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CDeleteContactsRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "delete_icc_t_frequent_contacts";
	l_SQLRequest.param["guid"] = l_request.m_oBody.m_guid;
	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute delete_icc_t_frequent_contacts failed";
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_frequent_contacts failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}



