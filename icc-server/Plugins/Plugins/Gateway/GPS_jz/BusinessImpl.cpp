#include "Boost.h"
#include "BusinessImpl.h"

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
{
	g_pBusinessImpl = this;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_GPS_OBSERVER_CENTER);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
}

void CBusinessImpl::OnStart()
{
	m_strIsUsing = m_pConfig->GetValue("ICC/Plugin/GPSJZ/IsUsing", "0");
	if (m_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		//return;
	}

	m_strServerIp = m_pConfig->GetValue("ICC/Plugin/GPSJZ/ServerIP", "");
	m_strServerPort = m_pConfig->GetValue("ICC/Plugin/GPSJZ/ServerPort", "80");

	m_strAppID = m_pConfig->GetValue("ICC/Plugin/GPSJZ/AppID", "dev");
	m_strPassword = m_pConfig->GetValue("ICC/Plugin/GPSJZ/Password", "ppp");
	m_strKey = m_pConfig->GetValue("ICC/Plugin/GPSJZ/Key", "000000");
	m_strStationNo = m_pConfig->GetValue("ICC/Plugin/GPSJZ/Station", "000001");
	m_strMaxWaitSecondTime = m_pConfig->GetValue("ICC/Plugin/GPSJZ/MaxWaitSecondTime", "5");
	ICC_LOG_DEBUG(m_pLog, "get info from config GPS IP=[%s], Port=[%s], AppID=[%s], Passwd=[%s], Key=[%s], Station=[%s], MaxTime=[%s]", 
		m_strServerIp.c_str(), m_strServerPort.c_str(), m_strAppID.c_str(), m_strPassword.c_str(), m_strKey.c_str(), m_strStationNo.c_str(), m_strMaxWaitSecondTime.c_str());

	m_intTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/GPSJZ/TimeOut", "5"));

	m_strGisSdkServiceName = m_pConfig->GetValue("ICC/Plugin/GPSJZ/GisServiceName", "gis-sdk-server");


	// 注册观察者
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_gps_request", OnNotifiGetGpsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_phone_info_request", OnNotifiGetPhoneInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_phone_info_request", OnNotifiSetPhoneInfoRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_division_request", OnNotifiGetDivisionRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_division_request", OnNotifiSetDivisionRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bjrdh_gps_request", OnNotifiGetBJRDHGpsRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
	

}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stoped.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiGetGpsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSGetGpsRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSGetGpsRespond l_oRespond;
	PROTOCOL::CGpsSync l_oSync;
	bool l_bRes = BuildGetGpsRespondAndSync(l_oRespond, l_oSync, l_oRequest);
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	
	if (l_bRes)
	{
		std::string l_strSendSyncMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiGetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSGetPhoneInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSGetPhoneInfoRespond l_oRespond;
	PROTOCOL::CGPSPhoneInfoSync l_oSync;
	bool l_bRes = BuildGetPhoneInfoRespondAndSync(l_oRespond, l_oSync, l_oRequest);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		std::string l_strSendSyncMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiSetPhoneInfoRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSSetPhoneInfoRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSSetPhoneInfoRespond l_oRespond;
	PROTOCOL::CGPSPhoneInfoSync l_oSync;
	bool l_bRes = BuildSetPhoneInfoRespondAndSync(l_oRespond, l_oSync, l_oRequest);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		std::string l_strSendSyncMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiGetDivisionRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSGetDivisionRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSGetDivisionRespond l_oRespond;
	PROTOCOL::CGPSDivisionSync l_oSync;
	bool l_bRes = BuildGetDivisionRespondAndSync(l_oRespond, l_oSync, l_oRequest);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (l_bRes)
	{
		std::string l_strSendSyncMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiSetDivisionRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSSetDivisionRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSSetDivisionRespond l_oRespond;
	PROTOCOL::CGPSDivisionSync l_oSync;
	bool l_bRes = BuildSetDivisionRespondAndSync(l_oRespond, l_oSync, l_oRequest);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	if (l_bRes)
	{
		std::string l_strSendSyncMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendSyncMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendSyncMsg.c_str());
	}
}

void CBusinessImpl::OnNotifiGetBJRDHGpsRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGPSGetBJRDHGpsRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	if (m_strIsUsing != "1")
	{
		PROTOCOL::CPostespond l_oRespond;
		l_oRespond.m_oHeader = l_oRequest.m_oHeader;
		l_oRespond.m_oBody.m_strcode = "405";    // 接口不开放 
		l_oRespond.m_oBody.m_strmessage = "The interface is not open";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	// 发送响应消息
	PROTOCOL::CGPSGetBJRDHGpsRespond l_oRespond;

	do
	{
		if (!BuildGetBJRDHGpsRespond(l_oRespond, l_oRequest))
		{
			ICC_LOG_DEBUG(m_pLog, "BuildGetBJRDHGpsRespond  failed");
			break;
		}

		if (l_oRespond.m_oBody.m_vecData.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest m_vecData empty");
			break;
		}

		if (!l_oRespond.m_oBody.m_vecData.at(0).m_strAddress.empty()) 
		{
			ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest Address not empty");
			break;
		}

		if (l_oRespond.m_oBody.m_vecData.at(0).m_strLatitude.empty() || l_oRespond.m_oBody.m_vecData.at(0).m_strLongitude.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest Latitude or Longitude  empty");
			break;
		}

		PROTOCOL::CLonLoatByAddrInfo l_LonLoatByAddrInfo;
		if (!queryLonLoatByAddr(l_oRespond.m_oBody.m_vecData.at(0).m_strLongitude, l_oRespond.m_oBody.m_vecData.at(0).m_strLatitude, l_LonLoatByAddrInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest queryLonLoatByAddr  failed");
			break;
		}

		l_oRespond.m_oBody.m_vecData.at(0).m_strAddress = l_LonLoatByAddrInfo.m_oBody.strAddress;
		l_oRespond.m_oBody.m_vecData.at(0).m_strLongitude = l_LonLoatByAddrInfo.m_oBody.strLongitude;
		l_oRespond.m_oBody.m_vecData.at(0).m_strLatitude = l_LonLoatByAddrInfo.m_oBody.strLatitude;

	} while (false);

	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifiGetBJRDHGpsRequest send message:[%s]", l_strSendMsg.c_str());

}

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader, std::string p_strCmd)
{
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
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

void CBusinessImpl::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd)
{
	p_oRespondHeader.m_strSystemID = "ICC";
	p_oRespondHeader.m_strSubsystemID = "GPSGate";
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = "";
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = p_strCmd;
	p_oRespondHeader.m_strRequest = "topic_phone_position";
	p_oRespondHeader.m_strRequestType = "1";
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

bool CBusinessImpl::BuildGetGpsRespondAndSync(PROTOCOL::CGPSGetGpsRespond& p_oRespond, PROTOCOL::CGpsSync& p_oSync, const PROTOCOL::CGPSGetGpsRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader,"get_gps_respond");

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_gps_record";
	//l_SqlRequest.param["callref_id"] = p_oRequest.m_oBody.m_strCallrefID;
	l_SqlRequest.param["phone_no"] = p_oRequest.m_oBody.m_strPhoneNo;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	
	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}	
	if (!l_pResult->Next())
	{
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_gps_record have no data");
		return false;
	}	
	p_oRespond.m_oBody.m_strResult = "0";
	
	// 构建同步消息
	BuildRespondHeader(p_oSync.m_oHeader, "gps_sync");
	p_oSync.m_oBody.m_strCallrefID = p_oRequest.m_oBody.m_strCallrefID;
	p_oSync.m_oBody.m_strPhoneNo = l_pResult->GetValue("phone_no");
	p_oSync.m_oBody.m_strLongitude = l_pResult->GetValue("longitude");
	p_oSync.m_oBody.m_strLatitude = l_pResult->GetValue("latitude");

	// 更新接警表的经纬度信息
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "update_icc_t_jjdb";
		l_SqlRequest.param["source_id"] = p_oRequest.m_oBody.m_strCallrefID;
		l_SqlRequest.param["caller_no"] = p_oRequest.m_oBody.m_strPhoneNo;
		l_SqlRequest.set["longitude"] = p_oSync.m_oBody.m_strLongitude;
		l_SqlRequest.set["latitude"] = p_oSync.m_oBody.m_strLatitude;

		std::string strTime = m_pDateTime->GetCallRefIdTime(p_oRequest.m_oBody.m_strCallrefID);
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{ // 执行SQL失败
			p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}

	return true;
}

bool CBusinessImpl::BuildGetPhoneInfoRespondAndSync(PROTOCOL::CGPSGetPhoneInfoRespond& p_oRespond, PROTOCOL::CGPSPhoneInfoSync& p_oSync, const PROTOCOL::CGPSGetPhoneInfoRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader, "get_phone_info_respond");

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phoneinfo";
	l_SqlRequest.param["phone"] = p_oRequest.m_oBody.m_strPhoneNo;
	l_SqlRequest.param["is_delete"] = "false";
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_phoneinfo have no data");
		return false;
	}
	p_oRespond.m_oBody.m_strResult = "0";

	// 构建同步消息
	BuildRespondHeader(p_oSync.m_oHeader, "phone_info_sync");
	p_oSync.m_oBody.m_strCallrefID = p_oRequest.m_oBody.m_strCallrefID;
	p_oSync.m_oBody.m_strPhoneNo = l_pResult->GetValue("phone");
	p_oSync.m_oBody.m_strName = l_pResult->GetValue("name");
	p_oSync.m_oBody.m_strAddress = l_pResult->GetValue("address");

	return true;
}

bool CBusinessImpl::BuildSetPhoneInfoRespondAndSync(PROTOCOL::CGPSSetPhoneInfoRespond& p_oRespond, PROTOCOL::CGPSPhoneInfoSync& p_oSync, const PROTOCOL::CGPSSetPhoneInfoRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader, "set_phone_info_respond");

	//////////////////////////////////////////////////////////////////////////
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phoneinfo";
	l_SqlRequest.param["phone"] = p_oRequest.m_oBody.m_strPhoneNo;
	l_SqlRequest.param["is_delete"] = "false";
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{// 如果该电话号码不存在，则新增
		DataBase::SQLRequest l_SqlInsertRequest;
		l_SqlInsertRequest.sql_id = "insert_icc_t_phoneinfo";
		l_SqlInsertRequest.param["guid"] = m_pString->CreateGuid();
		l_SqlInsertRequest.param["phone"] = p_oRequest.m_oBody.m_strPhoneNo;
		l_SqlInsertRequest.param["name"] = p_oRequest.m_oBody.m_strName;
		l_SqlInsertRequest.param["address"] = p_oRequest.m_oBody.m_strAddress;
		l_SqlInsertRequest.param["is_delete"] = "false";
		DataBase::IResultSetPtr l_pInsertResult = m_pDBConn->Exec(l_SqlInsertRequest);

		if (!l_pInsertResult->IsValid())
		{ // 执行SQL失败
			p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pInsertResult->GetErrorMsg().c_str());
			return false;
		}
	}
	else
	{// 如果该电话号码已经存在，则更新
		DataBase::SQLRequest l_SqlUpdateRequest;
		l_SqlUpdateRequest.sql_id = "update_icc_t_phoneinfo";
		l_SqlUpdateRequest.param["phone"] = p_oRequest.m_oBody.m_strPhoneNo;
		l_SqlUpdateRequest.set["name"] = p_oRequest.m_oBody.m_strName;
		l_SqlUpdateRequest.set["address"] = p_oRequest.m_oBody.m_strAddress;
		DataBase::IResultSetPtr l_pUpdateResult = m_pDBConn->Exec(l_SqlUpdateRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pUpdateResult->GetSQL().c_str());

		if (!l_pUpdateResult->IsValid())
		{ // 执行SQL失败
			p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pUpdateResult->GetErrorMsg().c_str());
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	p_oRespond.m_oBody.m_strResult = "0";

	// 构建同步消息
	BuildRespondHeader(p_oSync.m_oHeader, "phone_info_sync");
	p_oSync.m_oBody.m_strPhoneNo = p_oRequest.m_oBody.m_strPhoneNo;
	p_oSync.m_oBody.m_strName = p_oRequest.m_oBody.m_strName;
	p_oSync.m_oBody.m_strAddress = p_oRequest.m_oBody.m_strAddress;

	return true;
}

bool CBusinessImpl::BuildGetDivisionRespondAndSync(PROTOCOL::CGPSGetDivisionRespond& p_oRespond, PROTOCOL::CGPSDivisionSync& p_oSync, const PROTOCOL::CGPSGetDivisionRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader, "get_division_respond");
	
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phone_division";
	l_SqlRequest.param["phone_pre"] = ParsePhonePre(p_oRequest.m_oBody.m_strPhone);
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_phone_division have no data");
		return false;
	}
	p_oRespond.m_oBody.m_strResult = "0";

	// 构建同步消息
	BuildRespondHeader(p_oSync.m_oHeader, "division_sync");
	p_oSync.m_oBody.m_strCallrefID = p_oRequest.m_oBody.m_strCallrefID;
	p_oSync.m_oBody.m_strPhone = p_oRequest.m_oBody.m_strPhone;
	p_oSync.m_oBody.m_strProvince = l_pResult->GetValue("province");
	p_oSync.m_oBody.m_strCity = l_pResult->GetValue("city");
	p_oSync.m_oBody.m_strOperator = l_pResult->GetValue("operator");

	return true;
}

bool CBusinessImpl::BuildSetDivisionRespondAndSync(PROTOCOL::CGPSSetDivisionRespond& p_oRespond, PROTOCOL::CGPSDivisionSync& p_oSync, const PROTOCOL::CGPSSetDivisionRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader, "set_division_respond");

	//////////////////////////////////////////////////////////////////////////
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_phone_division";
	l_SqlRequest.param["phone_pre"] = p_oRequest.m_oBody.m_strPhonePre;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{// 如果该电话号码不存在，则新增
		DataBase::SQLRequest l_SqlInsertRequest;
		l_SqlInsertRequest.sql_id = "insert_icc_t_phone_division";
		l_SqlInsertRequest.param["phone_pre"] = p_oRequest.m_oBody.m_strPhonePre;
		l_SqlInsertRequest.param["province"] = p_oRequest.m_oBody.m_strProvince;
		l_SqlInsertRequest.param["city"] = p_oRequest.m_oBody.m_strCity;
		l_SqlInsertRequest.param["operator"] = p_oRequest.m_oBody.m_strOperator;
		DataBase::IResultSetPtr l_pInsertResult = m_pDBConn->Exec(l_SqlInsertRequest);
		if (!l_pInsertResult->IsValid())
		{ // 执行SQL失败
			p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pInsertResult->GetErrorMsg().c_str());
			return false;
		}
	}
	else
	{// 如果该电话号码已存在，则更新
		DataBase::SQLRequest l_SqlUpdateRequest;
		l_SqlUpdateRequest.sql_id = "update_icc_t_phone_division";
		l_SqlUpdateRequest.param["phone_pre"] = p_oRequest.m_oBody.m_strPhonePre;
		l_SqlUpdateRequest.set["province"] = p_oRequest.m_oBody.m_strProvince;
		l_SqlUpdateRequest.set["city"] = p_oRequest.m_oBody.m_strCity;
		l_SqlUpdateRequest.set["operator"] = p_oRequest.m_oBody.m_strOperator;
		DataBase::IResultSetPtr l_pUpdateResult = m_pDBConn->Exec(l_SqlUpdateRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pUpdateResult->GetSQL().c_str());

		if (!l_pUpdateResult->IsValid())
		{ // 执行SQL失败
			p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pUpdateResult->GetErrorMsg().c_str());
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	p_oRespond.m_oBody.m_strResult = "0";

	// 构建同步消息
	BuildRespondHeader(p_oSync.m_oHeader, "division_sync");
	p_oSync.m_oBody.m_strPhonePre = p_oRequest.m_oBody.m_strPhonePre;
	p_oSync.m_oBody.m_strProvince = p_oRequest.m_oBody.m_strProvince;
	p_oSync.m_oBody.m_strCity = p_oRequest.m_oBody.m_strCity;
	p_oSync.m_oBody.m_strOperator = p_oRequest.m_oBody.m_strOperator;

	return true;
}

bool CBusinessImpl::BuildGetBJRDHGpsRespond(PROTOCOL::CGPSGetBJRDHGpsRespond& p_oRespond, const PROTOCOL::CGPSGetBJRDHGpsRequest& p_oRequest)
{
	// 构建响应消息
	BuildRespondHeader(p_oRespond.m_oHeader, p_oRequest.m_oHeader, "get_bjrdh_gps_respond");

	if (m_strServerIp.empty())
	{
		return BuildGetGpsRespond(p_oRespond, p_oRequest);
	}

	std::string strContent;
	std::string strErrorMessage;
	std::string strToken;
	//获取Position
	std::string urlPath = "/v3/location";
	std::map<std::string, std::string> mapHeaders;
	// 解析token
	PROTOCOL::CBJRDHTokenRespond tokenRes;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	DateTime::CDateTime l_oCurrentTime = m_pDateTime->CurrentDateTime();
	std::string strSoureA = "appid=" + m_strAppID + "&phone=" + p_oRequest.m_oBody.m_strPhoneNo + "&time=" + std::to_string(l_oCurrentTime);
	ICC_LOG_DEBUG(m_pLog, "strSoureA:[%s]", strSoureA.c_str());
	std::string SignTemp = strSoureA + "&key=" + m_strKey;
	ICC_LOG_DEBUG(m_pLog, "SignTemp:[%s]", SignTemp.c_str());
	// std::string SignTemp2 = m_pString->Format("appid=%s&phone=%s&time=%s&key=%s", "b930ea5d5a258f4f", "13800138000", "2988354366000", "250b4c09247ec02edce69f6a2d");
	//ICC_LOG_DEBUG(m_pLog, "SignTemp2:[%s]", SignTemp.c_str());
	// unsigned char *MD5(const unsigned char *d, size_t n, unsigned char *md);
	unsigned char buf[16];
	unsigned char szbuf[MD5_DIGEST_LENGTH * 2 + 1];
	memset(szbuf, 0, MD5_DIGEST_LENGTH * 2 + 1);
	MD5((const unsigned char*)SignTemp.c_str(), SignTemp.size(), buf);
	for (int i = 0; i < 16; i++)
	{
		printf("%x", (int)buf[i]);
		sprintf((char*)&szbuf[i * 2], "%02x", buf[i]);
	}
	std::string l_oSign = (char*)szbuf;
	ICC_LOG_DEBUG(m_pLog, "l_oSign:[%s]", l_oSign.c_str());
	std::string strTarget = m_pString->Format("%s?%s&station=%s&sign=%s", urlPath.c_str(), strSoureA.c_str(), m_strStationNo.c_str(), l_oSign.c_str());
	ICC_LOG_DEBUG(m_pLog, "strTarget:[%s]", strTarget.c_str());

	//std::string strReceive = m_pHttpClient->GetEx(m_strServerIp, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strServerIp, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_intTimeOut);

	//JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	//pJson->SetNodeValue("/status", "0");
	//pJson->SetNodeValue("/time", "1553830715800");
	//pJson->SetNodeValue("/caller", "13800138000");
	//pJson->SetNodeValue("/called", "110");
	//pJson->SetNodeValue("/lng", "39.990464");
	//pJson->SetNodeValue("/lat", "116.481488");
	//pJson->SetNodeValue("/callTime", "1553830710800");
	//pJson->SetNodeValue("/latLngType", "lbs");
	//pJson->SetNodeValue("/address", "广东省深圳市南山区蛇口网谷万维大厦附近");
	//pJson->SetNodeValue("/shortAddress", "南山区蛇口网谷万维大厦附近");
	//pJson->SetNodeValue("/organ", "CM");
	//strReceive =  pJson->ToString();
	ICC_LOG_DEBUG(m_pLog, "strsoure strReceive  info:[%s] ", strReceive.c_str());
	if (strReceive.empty())
	{
		//ICC_LOG_ERROR(m_pLog, "%s get position err[%s]", strTarget1.c_str(), strErrorMessage.c_str());
		p_oRespond.m_oHeader.m_strResult = "5";
		p_oRespond.m_oHeader.m_strMsg = "get position err";
		return false;
	}

	// 解析position
	PROTOCOL::CBJRDHPositionRespondJZ positionRes;
	strReceive.insert(0, "{\"data\":[");
	strReceive.append("]}");
	ICC_LOG_DEBUG(m_pLog, "strsoure strReceive  info:[%s] ", strReceive.c_str());
	if (!positionRes.ParseString(strReceive, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "position[%s] ParseString Failed", strReceive.c_str());
		p_oRespond.m_oHeader.m_strResult = "6";
		p_oRespond.m_oHeader.m_strMsg = "position ParseString Failed";
		return false;
	}

	p_oRespond.m_oHeader.m_strResult = positionRes.m_vecData[0].m_strStatus;
	p_oRespond.m_oHeader.m_strMsg = positionRes.m_vecData[0].m_strInfo;
	ICC_LOG_DEBUG(m_pLog, "strsoure strReceive  info:[%s] ", p_oRespond.m_oHeader.m_strMsg.c_str());
	std::vector<PROTOCOL::CBJRDHPositionRespondJZ::CPosition>::iterator it = positionRes.m_vecData.begin();
	for (; it != positionRes.m_vecData.end(); ++it)
	{
		PROTOCOL::CGPSGetBJRDHGpsRespond::CBJRDH data;
		data.m_strCallTime = it->m_strCallTime;
		data.m_strCallerNo = it->m_strCaller;
		data.m_strCalledNo = it->m_strCalled;
		//data.m_strArea = "";
		//data.m_strLac = "";
		//data.m_strCi = "";
		data.m_strLongitude = it->m_strLng;
		data.m_strLatitude = it->m_strLat;
		data.m_strAddress = it->m_strAddress;

		p_oRespond.m_oBody.m_vecData.push_back(data);
		p_oRespond.m_oBody.m_strCount = m_pString->Number(positionRes.m_vecData.size());
	}
	return true;
}

bool CBusinessImpl::BuildGetGpsRespond(PROTOCOL::CGPSGetBJRDHGpsRespond& p_oRespond, const PROTOCOL::CGPSGetBJRDHGpsRequest& p_oRequest)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_gps_record";
	l_SqlRequest.param["phone_no"] = p_oRequest.m_oBody.m_strPhoneNo;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	p_oRespond.m_oHeader.m_strResult = "0";
	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		p_oRespond.m_oHeader.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		p_oRespond.m_oHeader.m_strMsg = "exec sql fail";
		return false;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_gps_record have no data");
		return false;
	}

	PROTOCOL::CGPSGetBJRDHGpsRespond::CBJRDH data;
	data.m_strCallerNo = l_pResult->GetValue("phone_no");
	data.m_strLongitude = l_pResult->GetValue("longitude");
	data.m_strLatitude = l_pResult->GetValue("latitude");

	p_oRespond.m_oBody.m_vecData.push_back(data);
	p_oRespond.m_oBody.m_strCount = m_pString->Number(1);

	return true;
}

std::string CBusinessImpl::ParsePhonePre(std::string p_strPhone)
{
	/*
	* 【座机】+91前缀代表藏南
	* 【座机】8开头的3位前缀代表港澳台
	* 【座机】01、02开头3位前缀代表北京、上海、广州、深圳等特殊的市区
	* 【座机】+886前缀代表金门、马祖、乌丘等离岛地区
	* 【座机】03-09开头的4位前缀代表普通省市区
	* 【移动电话】1开头的7位前缀代表各市区
	*/
	if (m_pString->RegexMatch(p_strPhone, "(\\+91(\\d+)|8(\\d+)|0[1-2](\\d+))"))
	{
		return p_strPhone.substr(0, 3);
	}
	else if (m_pString->RegexMatch(p_strPhone, "(\\+886(\\d+)|0[3-9](\\d+))"))
	{
		return p_strPhone.substr(0, 4);
	}
	else if (m_pString->RegexMatch(p_strPhone, "1(\\d+)"))
	{
		return p_strPhone.substr(0, 7);
	}
	else
	{
		return "";
	}
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
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

bool CBusinessImpl::GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strServiceName = p_strServiceName;
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strVcsServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strVcsServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy send service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy send service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}


bool CBusinessImpl::queryLonLoatByAddr(std::string& p_strLon, std::string& p_strLoat, PROTOCOL::CLonLoatByAddrInfo& p_pLonLoatByAddrInfo)
{
	// 用经纬度 请求gis服务 获取对应地址 默认取第一条
 	// http://192.168.0.168:28020/geocode/regeo?x=Lon&y=Loat&location=lon,Loat&lonlat=[Lon,Loat]&radius=100
	// http://gis-sdk-server/geocode/regeo?x=104.07109&y=30.67508&location=104.07109%2C30.67508&lonlat=%5B104.07109%2C30.67508%5D&radius=100 

	do
	{
		std::string strTarget = m_pString->Format("/geocode/regeo?x=%s&y=%s&location=%s%s%s&lonlat=%s%s%s%s%s&radius=100", p_strLon.c_str(), p_strLoat.c_str(), 
			p_strLon.c_str(), "%2C", p_strLoat.c_str(),"%5B", p_strLon.c_str(), "%2C", p_strLoat.c_str(),"%5D");

		ICC_LOG_DEBUG(m_pLog, "queryLonLoatByAddr strTarget: %s", strTarget.c_str());

		std::string strErrorMessage;
		std::string strContent = "";
		std::map<std::string, std::string> mapHeaders;
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));


		std::string strGisSdkServerIp;
		std::string strGisSdkerverPort;
		if (GetServiceInfo(m_strGisSdkServiceName, strGisSdkServerIp, strGisSdkerverPort))
		{
			ICC_LOG_ERROR(m_pLog, "faile strWechatReceive empty strErrorMessage : %s", strErrorMessage.c_str());
			return false;
		}
		std::string strGisSdkReceive = m_pHttpClient->GetWithTimeout(strGisSdkServerIp, strGisSdkerverPort, strTarget, mapHeaders, strContent, strErrorMessage, m_intTimeOut);

		ICC_LOG_DEBUG(m_pLog, "strGisSdkReceive to strReceive: %s", strGisSdkReceive.c_str());

		if (strGisSdkReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "failed strGisSdkReceive empty strErrorMessage : %s", strErrorMessage.c_str());
			return false;
		}

		if (!p_pLonLoatByAddrInfo.ParseString(strGisSdkReceive, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "failed strGisSdkReceive ParseString ");
			return false;
		}
		if (0 == p_pLonLoatByAddrInfo.m_oBody.strCode.compare("200"))
		{
			ICC_LOG_ERROR(m_pLog, "failed code: %s", p_pLonLoatByAddrInfo.m_oBody.strCode.c_str());
			return false;
		}

		if (p_pLonLoatByAddrInfo.m_oBody.strAddress.empty() || p_pLonLoatByAddrInfo.m_oBody.strLatitude.empty()|| p_pLonLoatByAddrInfo.m_oBody.strLongitude.empty())
		{
			ICC_LOG_ERROR(m_pLog, "failed address or latitude or longitude empty");
			return false;
		}

	} while (false);

	return true;
}