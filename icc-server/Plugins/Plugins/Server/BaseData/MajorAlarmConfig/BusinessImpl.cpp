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
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();

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

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_major_alarm_config", OnCNotifiMajorAlarmConfigAddRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_major_alarm_config", OnCNotifiMajorAlarmConfigQueryRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_major_alarm_config", OnNotifiMajorAlarmConfigDeleteRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);

	ICC_LOG_INFO(m_pLog, "plugin basedata.majoralarmconfig start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "frequentcontacts stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiMajorAlarmConfigAddRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CMajorAlarmConfigAddRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;

	std::string strGuid = m_pString->CreateGuid();
	l_SQLRequest.sql_id = "insert_icc_t_major_alarm_config";
	l_SQLRequest.param["guid"] = strGuid;
	l_SQLRequest.param["type_name"] = l_request.m_oBody.m_data.m_type_name;
	l_SQLRequest.param["type_value"] = l_request.m_oBody.m_data.m_type_value;
	l_SQLRequest.param["remark"] = l_request.m_oBody.m_data.m_remark;
	
	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute insert_icc_t_major_alarm_config failed";
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_major_alarm_config failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
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
	
	if (l_oRespond.m_strResult == "0")
	{
		PROTOCOL::CMajorAlarmConfigSync::CBody data;
		data.m_data.m_guid = strGuid;
		data.m_data.m_type_name = l_request.m_oBody.m_data.m_type_name;
		data.m_data.m_type_value = l_request.m_oBody.m_data.m_type_value;
		data.m_data.m_remark = l_request.m_oBody.m_data.m_remark;
		data.m_update_type = "1";
		_SyncUpdate(data);

		//SendMajorAlarmConfigMsgToMsgCenter(l_request, p_strMsg);
	}
}

void CBusinessImpl::OnCNotifiMajorAlarmConfigQueryRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CMajorAlarmConfigQueryRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "query_icc_t_major_alarm_config";	

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	//回复消息
	PROTOCOL::CMajorAlarmConfigQueryResponse response;
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	while (l_Result->Next())
	{
		PROTOCOL::CMajorAlarmConfigQueryResponse::CData data;
		data.m_guid = l_Result->GetValue("guid");
		data.m_type_name = l_Result->GetValue("type_name");
		data.m_type_value = l_Result->GetValue("type_value");
		data.m_remark = l_Result->GetValue("remark");		
		response.m_oBody.m_vecData.push_back(data);

	}
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
	
}

void CBusinessImpl::OnNotifiMajorAlarmConfigDeleteRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CMagorAlarmConfigDeleteRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "delete_icc_t_major_alarm_config";
	l_SQLRequest.param["guid"] = l_request.m_oBody.m_data.m_guid;
	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute delete_icc_t_major_alarm_config failed";
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_major_alarm_config failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
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

	if (l_oRespond.m_strResult == "0")
	{
		PROTOCOL::CMajorAlarmConfigSync::CBody data;
		data.m_data.m_guid = l_request.m_oBody.m_data.m_guid;
		data.m_data.m_type_name = l_request.m_oBody.m_data.m_type_name;
		data.m_data.m_type_value = l_request.m_oBody.m_data.m_type_value;
		data.m_data.m_remark = l_request.m_oBody.m_data.m_remark;
		data.m_update_type = "2";
		_SyncUpdate(data);
	}
}


void CBusinessImpl::_SyncUpdate(PROTOCOL::CMajorAlarmConfigSync::CBody& data)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CMajorAlarmConfigSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = "ICC";
	l_oAlarmSync.m_oHeader.m_strSubsystemID = "icc-majoralarmconfig";
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmSync.m_oHeader.m_strCmd = "major_alarm_config_update_sync";
	l_oAlarmSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	l_oAlarmSync.m_oBody = data;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}



