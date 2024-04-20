#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000
#define ACD_INFO "acd_dept"

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

	//m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);


	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_acd_dept_relation", OnCNotifiAcdAddRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_acd_dept_relation", OnCNotifiAcdDeleteRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_acd_dept_relation", OnCNotifiAcdQueryRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_acd_dept_relation", OnCNotifiAcdUpdateRequest);

	
	this->SaveAcdData();


	ICC_LOG_INFO(m_pLog, "plugin basedata.acd start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "acd stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnCNotifiAcdAddRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CAcdAddRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;

	l_SQLRequest.sql_id = "insert_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = l_request.m_oBody.m_acd;
	l_SQLRequest.param["dept_code"] = l_request.m_oBody.m_dept_code;
	l_SQLRequest.param["dept_name"] = l_request.m_oBody.m_dept_name;
	l_SQLRequest.param["remark"] = l_request.m_oBody.m_remark;
	l_SQLRequest.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
	std::string strDeptInfo;
	if (!l_request.m_oBody.m_dept_code.empty())
	{
		if (!m_pRedisClient->HGet("DeptCodeInfoKey", l_request.m_oBody.m_dept_code, strDeptInfo))
		{
			ICC_LOG_ERROR(m_pLog, "get acd_dept_path failed");
		}
	}
	PROTOCOL::CDeptInfo l_DeptInfo;
	if(!l_DeptInfo.Parse(strDeptInfo, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse dept info failed");
	}
	l_request.m_oBody.m_dept_code_path = l_DeptInfo.m_strCodeLevel;
	l_SQLRequest.param["dept_code_path"] = l_request.m_oBody.m_dept_code_path;
	l_SQLRequest.param["dept_name_path"] = l_request.m_oBody.m_dept_name_path;


	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute insert_icc_t_acd_dept failed";
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_acd_dept,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}

	l_request.m_oBody.m_update_time = m_pDateTime->CurrentDateTimeStr();
	m_pRedisClient->HSet(ACD_INFO, l_request.m_oBody.m_acd.c_str(), l_request.ToString_NoHead(m_pJsonFac->CreateJson()).c_str());

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	PROTOCOL::CAcdSync::CBody data;
	data.m_data.m_acd = l_request.m_oBody.m_acd;
	data.m_data.m_dept_code = l_request.m_oBody.m_dept_code;
	data.m_data.m_dept_name = l_request.m_oBody.m_dept_name;
	data.m_data.m_dept_code_path = l_request.m_oBody.m_dept_code_path;
	data.m_data.m_dept_name_path = l_request.m_oBody.m_dept_name_path;
	data.m_data.m_remark = l_request.m_oBody.m_remark;
	data.m_data.m_update_type = "1";	//1表示新增 
	_SyncUpdate(data);
}

void CBusinessImpl::OnCNotifiAcdDeleteRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CAcdDeleteRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;

	l_SQLRequest.sql_id = "delete_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = l_request.m_oBody.m_acd;

	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute delete_icc_t_acd_dept failed";
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_acd_dept,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}
	m_pRedisClient->HDel(ACD_INFO, l_request.m_oBody.m_acd);
	//回复消息
;	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	PROTOCOL::CAcdSync::CBody data;
	data.m_data.m_acd = l_request.m_oBody.m_acd;
	data.m_data.m_dept_code = l_request.m_oBody.m_dept_code;
	data.m_data.m_dept_name = l_request.m_oBody.m_dept_name;
	data.m_data.m_dept_code_path = l_request.m_oBody.m_dept_code_path;
	data.m_data.m_dept_name_path = l_request.m_oBody.m_dept_name_path;
	data.m_data.m_remark = l_request.m_oBody.m_remark;
	data.m_data.m_update_type = "3";	//3表示删除
	_SyncUpdate(data);
}

void CBusinessImpl::OnCNotifiAcdUpdateRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CAcdUpdateRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	std::string strDeptInfo;
	if (!l_request.m_oBody.m_dept_code.empty())
	{
		if (!m_pRedisClient->HGet("DeptCodeInfoKey", l_request.m_oBody.m_dept_code, strDeptInfo))
		{
			ICC_LOG_ERROR(m_pLog, "get acd_dept_path failed");
		}
	}
	PROTOCOL::CDeptInfo l_DeptInfo;
	if (!l_DeptInfo.Parse(strDeptInfo, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse dept info failed");
	}
	l_request.m_oBody.m_dept_code_path = l_DeptInfo.m_strCodeLevel;
	DataBase::SQLRequest l_SQLRequest;

	l_SQLRequest.sql_id = "update_icc_t_acd_dept";
	l_SQLRequest.param["acd"] = l_request.m_oBody.m_acd;
	l_SQLRequest.set["dept_code"] = l_request.m_oBody.m_dept_code;
	l_SQLRequest.set["dept_name"] = l_request.m_oBody.m_dept_name;
	l_SQLRequest.set["remark"] = l_request.m_oBody.m_remark;
	l_SQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	l_SQLRequest.set["dept_code_path"] = l_request.m_oBody.m_dept_code_path;
	l_SQLRequest.set["dept_name_path"] = l_request.m_oBody.m_dept_name_path;


	PROTOCOL::CHeaderEx l_oRespond = l_request.m_oHeader;

	DataBase::IResultSetPtr l_Result = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute update_icc_t_acd_dept failed";
		ICC_LOG_ERROR(m_pLog, "update_icc_t_acd_dept,error msg:[%s]", l_Result->GetErrorMsg().c_str());
	}
	else
	{
		l_oRespond.m_strResult = "0";
	}
	l_request.m_oBody.m_update_time= m_pDateTime->CurrentDateTimeStr();
	m_pRedisClient->HSet(ACD_INFO, l_request.m_oBody.m_acd.c_str(), l_request.ToString_NoHead(m_pJsonFac->CreateJson()).c_str());

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	PROTOCOL::CAcdSync::CBody data;
	data.m_data.m_acd = l_request.m_oBody.m_acd;
	data.m_data.m_dept_code = l_request.m_oBody.m_dept_code;
	data.m_data.m_dept_name = l_request.m_oBody.m_dept_name;
	data.m_data.m_dept_code_path = l_request.m_oBody.m_dept_code_path;
	data.m_data.m_dept_name_path = l_request.m_oBody.m_dept_name_path;
	data.m_data.m_remark = l_request.m_oBody.m_remark;
	data.m_data.m_update_type = "2"; //2表示更新
	_SyncUpdate(data);
}

void CBusinessImpl::OnCNotifiAcdQueryRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_strMsg.c_str());
	PROTOCOL::CAcdQueryRequest l_request;
	if (!l_request.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	DataBase::SQLRequest l_SQLRequest;
	DataBase::IResultSetPtr l_Result;
	if (l_request.m_oBody.m_acd.empty())
	{
		l_SQLRequest.sql_id = "query_icc_t_acd_dept";

		l_Result = m_pIDBConn->Exec(l_SQLRequest,true);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());
	}
	else
	{
		l_SQLRequest.sql_id = "query_icc_t_acd_dept_by_acd";
		l_SQLRequest.param["acd"] = l_request.m_oBody.m_acd;
		l_Result = m_pIDBConn->Exec(l_SQLRequest, true);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());
	}

	//回复消息
	PROTOCOL::CAcdQueryResponse response;
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();
	while (l_Result->Next())
	{
		PROTOCOL::CAcdQueryResponse::CData data;
		data.m_acd= l_Result->GetValue("acd");
		data.m_dept_code = l_Result->GetValue("dept_code");
		data.m_dept_name = l_Result->GetValue("dept_name");
		data.m_remark = l_Result->GetValue("remark");
		data.m_dept_code_path = l_Result->GetValue("dept_code_path");
		data.m_dept_name_path = l_Result->GetValue("dept_name_path");
		
		response.m_oBody.m_vecData.push_back(data);

	}
	std::string l_strMessage = response.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::_SyncUpdate(PROTOCOL::CAcdSync::CBody& data)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAcdSync l_oAcdSync;
	l_oAcdSync.m_oHeader.m_strSystemID = "ICC";
	l_oAcdSync.m_oHeader.m_strSubsystemID = "icc-acd";
	l_oAcdSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAcdSync.m_oHeader.m_strRelatedID = "";
	l_oAcdSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAcdSync.m_oHeader.m_strCmd = "acd_dept_update_sync";
	l_oAcdSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_oAcdSync.m_oHeader.m_strRequestType = "1";  //1表示新增，2表示更新，3表示删除
	l_oAcdSync.m_oHeader.m_strResponse = "";
	l_oAcdSync.m_oHeader.m_strResponseType = "";

	l_oAcdSync.m_oBody = data;

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAcdSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}



void CBusinessImpl::SearchAcdRelationInDb(std::vector< std::map<std::string,std::string>>& m_vecAcdRelation)
{
	DataBase::SQLRequest l_oSQLRequest;
	l_oSQLRequest.sql_id = "query_icc_t_acd_dept";
	DataBase::IResultSetPtr	l_pResult = m_pIDBConn->Exec(l_oSQLRequest,true);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
		return ;
	}
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	std::vector<std::string> vecFiledNames = l_pResult->GetFieldNames();
	while (l_pResult->Next())
	{	
		std::map<std::string, std::string> mapTmps;
		unsigned int iCount = vecFiledNames.size();
		for (unsigned int i = 0; i < iCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecFiledNames[i], l_pResult->GetValue(vecFiledNames[i])));
		}
		m_vecAcdRelation.push_back(mapTmps);
	}
}

void CBusinessImpl::InsertAcdRelationInRedis(std::vector< std::map<std::string, std::string>>& m_vecAcdRelation)
{
	int i;
	int m_vecSize = m_vecAcdRelation.size();
	std::map<std::string, std::string>::const_iterator itr;
	for (i = 0; i < m_vecSize; i++)
	{
		for (itr = m_vecAcdRelation[i].begin(); itr != m_vecAcdRelation[i].end(); itr++)
		{
			PROTOCOL::CAcdInfo l_AcdInfo;
			l_AcdInfo.m_acd = m_vecAcdRelation[i]["acd"];
			l_AcdInfo.m_dept_code = m_vecAcdRelation[i]["dept_code"];
			l_AcdInfo.m_dept_code_path = m_vecAcdRelation[i]["dept_code_path"];
			l_AcdInfo.m_dept_name= m_vecAcdRelation[i]["dept_name"];
			l_AcdInfo.m_dept_name_path = m_vecAcdRelation[i]["dept_name_path"];
			l_AcdInfo.m_remark = m_vecAcdRelation[i]["remark"];
			l_AcdInfo.m_update_time = m_vecAcdRelation[i]["update_time"];

			std::string l_strVal = l_AcdInfo.ToJson(m_pJsonFac->CreateJson());
			m_pRedisClient->HSet(ACD_INFO, m_vecAcdRelation[i]["acd"], l_strVal);
		}
	}
	ICC_LOG_DEBUG(m_pLog, "Insert Acd Relation In Redis finished");
}

void CBusinessImpl::SaveAcdData()
{
	//存Redis之前先把Redis节点删除再存
	m_pRedisClient->Del(ACD_INFO);
	std::vector< std::map<std::string, std::string>> m_vecAcdRelation;
	this->SearchAcdRelationInDb(m_vecAcdRelation);
	this->InsertAcdRelationInRedis(m_vecAcdRelation);
	ICC_LOG_DEBUG(m_pLog, "Save Acd Data Success!!!");
}
