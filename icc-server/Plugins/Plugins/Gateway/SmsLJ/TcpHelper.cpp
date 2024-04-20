#include "Boost.h"
#include "TcpHelper.h"

#define MAX_SMS_COUNT	500

CTcpHelper::CTcpHelper()
	: m_pThreadCheckLink(nullptr),
	m_pThreadProcess(nullptr),
	m_bIsStopCheckLinkThread(false),
	m_bIsStopProcessThread(false),
	m_bSmsLoginStatus(false)
{
	m_TcpClient = nullptr;
	m_ITcpClientCallbackPtr = nullptr;

	m_uiCheckLinkTime = 3;
	m_uiProcessTime = 1;
	m_uiIsChangeInfoTime = 7;

	m_mapCmdName[SMS_GET_LOCATION_REQUEST] = SMS_GET_LOCATION_RESPOND;
}

CTcpHelper::~CTcpHelper()
{
	ClearOwerSms();
	EndProcessThread();
	DisConnect();
}

void CTcpHelper::SetConnParam(std::string& p_strDeptCode, std::string& p_strDeptName, std::string& p_strIp, unsigned short p_ushPort,
	std::string& p_strUserName, std::string& p_strPassword)
{
	m_strDeptCode = p_strDeptCode;
	m_strDeptName = p_strDeptName;

	m_strServiceIP = p_strIp;
	m_uiServicePort = p_ushPort;
	m_strUserName = p_strUserName;
	m_strPassword = p_strPassword;
}

void CTcpHelper::AddOwerSms(const std::string& strGuid, const std::string& strPhone)
{
	Lock::AutoLock lock(m_SmsMutex);
	if (m_mapOwnerSms.size() > MAX_SMS_COUNT)
	{
		m_mapOwnerSms.erase(m_mapOwnerSms.begin());
	}

	m_mapOwnerSms[strGuid] = strPhone;
}
void CTcpHelper::RemoveOwerSms(const std::string& strGuid)
{
	Lock::AutoLock lock(m_SmsMutex);
	m_mapOwnerSms.erase(strGuid);
}
void CTcpHelper::ClearOwerSms()
{
	Lock::AutoLock lock(m_SmsMutex);
	m_mapOwnerSms.clear();
}
bool CTcpHelper::IsOwnerSms(const std::string& p_strGuid)
{
	Lock::AutoLock lock(m_SmsMutex);
	auto it = m_mapOwnerSms.find(p_strGuid);
	if (it != m_mapOwnerSms.end())
	{
		return true;
	}

	return false;
}
bool CTcpHelper::HasSmsByPhone(const std::string& p_strPhone)
{
	Lock::AutoLock lock(m_SmsMutex);
	for (auto l_SmsObj : m_mapOwnerSms)
	{
		if (l_SmsObj.second.compare(p_strPhone) == 0)
		{
			return true;
		}
	}

	return false;
}

void CTcpHelper::SetProcessParam(std::string& p_strRespContent, unsigned int p_uiCheckLinkTime, unsigned int p_uiProcessTime, unsigned int p_uiIsChangeInfoTime)
{
	m_strRespContent = p_strRespContent;
	m_uiCheckLinkTime = p_uiCheckLinkTime;
	m_uiProcessTime = p_uiProcessTime;
	m_uiIsChangeInfoTime = p_uiIsChangeInfoTime;
}
void CTcpHelper::LoadUnReadSms()
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;
	l_Sql.param["type"] = THIRD_TYPE_SMS;
	l_Sql.param["state"] = STATE_UNREAD;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return;
	}

	// 获取第三方警情信息
	while (l_pResult->Next())
	{
		std::string l_strGuid = l_pResult->GetValue("guid");
		std::string l_strOrgId = l_pResult->GetValue("to_org_id");

		if (l_strOrgId.compare(m_strDeptCode) == 0)
		{
			// 缓存seqID信息
			Lock::AutoLock lock(m_Mutex);
			m_mapSmsResInfo[l_strGuid] = l_strGuid;
			ICC_LOG_DEBUG(m_pLog, "Load m_mapSmsResInfo, Id: [%s], [%d]", l_strGuid.c_str(), m_mapSmsResInfo.size());
		}
	}
}

void CTcpHelper::LoadOwnerSms()
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;
	l_Sql.param["type"] = THIRD_TYPE_SMS;
	l_Sql.param["to_org_id"] = m_strDeptCode;
	l_Sql.param["limit"] = std::to_string(MAX_SMS_COUNT);

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return;
	}

	// 获取第三方警情信息
	while (l_pResult->Next())
	{
		std::string l_strGuid = l_pResult->GetValue("guid");
		std::string l_strUserPhone = l_pResult->GetValue("user_phone");
		
		ICC_LOG_DEBUG(m_pLog, "Load owner sms, Guid:[%s], Phone[%s]", l_strGuid.c_str(), l_strUserPhone.c_str());
		AddOwerSms(l_strGuid, l_strUserPhone);
	}
}
bool CTcpHelper::Connect()
{
	if (m_ITcpClientCallbackPtr == nullptr)
	{
		m_ITcpClientCallbackPtr = boost::make_shared<TcpClientImpl>(this);
	}

	if (m_TcpClient != nullptr)
	{
		return m_TcpClient->Connect(m_strServiceIP, m_uiServicePort, m_ITcpClientCallbackPtr);
	}

	return false;
}
void CTcpHelper::DisConnect()
{
	if (m_TcpClient != nullptr)
	{
		m_TcpClient->Close();
	}
}

void CTcpHelper::OnNotifiSetReadSync(ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CSmsSetReadSync l_oSync;
	if (!l_oSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 查找是否为短信已读同步
	if (!IsSmsRead(l_oSync.m_oBody.m_strID))
	{
		ICC_LOG_DEBUG(m_pLog, "Is not sms read, no need to transmit to sms services!");
		return;
	}

	// 查找seqid是否存在，即：是否已经发送过已读请求
	auto it = m_mapSmsResInfo.find(l_oSync.m_oBody.m_strID);
	if (m_mapSmsResInfo.end() == it)
	{
		ICC_LOG_WARNING(m_pLog, "Is not sms id[%s], no need to transmit to sms services!", l_oSync.m_oBody.m_strID.c_str());
		return;
	}
	m_mapSmsResInfo.erase(it);
	ICC_LOG_DEBUG(m_pLog, "Del m_mapSmsResInfo, Id: [%s], [%d]", l_oSync.m_oBody.m_strID.c_str(), m_mapSmsResInfo.size());

	// 发送已读标记
	SendSetReaded(l_oSync.m_oBody.m_strID);

	// 发送已接警短信
	SendReceipted(l_oSync.m_oBody.m_strID);
}

void CTcpHelper::OnNotifiSendSmsSync(ObserverPattern::INotificationPtr p_pNotify)
{
	PROTOCOL::CSmsSendSmsSync l_oSync;
	if (!l_oSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送交互短信
	SendSmsInfo(l_oSync.m_oBody.m_strTelnum, l_oSync.m_oBody.m_strContent);
}

void CTcpHelper::OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	//ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSmsGetLocationRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 转发定位请求到SmsService
	bool l_bRes = SendGetLocation(l_oRequest.m_oBody.m_strTelnum);

	// 发送回复消息
	PROTOCOL::CSmsGetLocationRespond l_oRespond;
	BuildRespondHeader(l_oRespond.m_oHeader, l_oRequest.m_oHeader);
	l_oRespond.m_oBody.m_strResult = (l_bRes ? "0" : "1");

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::OnNotifiServerStatusSync(const SmsBodyServerStatusArrival& p_oServerStatus)
{
	PROTOCOL::CSmsServerStatusSync l_oSync;

	BuildRespondHeader(l_oSync.m_oHeader, SMS_SERVER_STATUS_SYNC,TOPIC);

	l_oSync.m_oBody.m_strType = m_pString->Number(p_oServerStatus.m_iType);
	l_oSync.m_oBody.m_strStatus = m_pString->Number(p_oServerStatus.m_bStatus);
	l_oSync.m_oBody.m_strDescription = SmsUtil::toString(p_oServerStatus.m_szDescription, sizeof(p_oServerStatus.m_szDescription));

	std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::OnNotifiUserStatusSync(const SmsBodyUserStatusArrival& p_oUserStatus)
{
	PROTOCOL::CSmsUserStatusSync l_oSync;

	BuildRespondHeader(l_oSync.m_oHeader, SMS_USER_STATUS_SYNC,TOPIC);

	l_oSync.m_oBody.m_strType = m_pString->Number(p_oUserStatus.m_iType);
	l_oSync.m_oBody.m_strCount = m_pString->Number(p_oUserStatus.m_iCount);
	for (int l_iIndex = 0; l_iIndex < p_oUserStatus.m_iCount; l_iIndex ++)
	{
		std::string l_strUnitID = SmsUtil::toString(p_oUserStatus.m_pUnitIDList + 16 * l_iIndex, 16);
		l_oSync.m_oBody.m_vecUnitIdList.push_back(l_strUnitID);
	}

	std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::OnNotifiLocationSync(const SmsBodyGetLocationResp& p_oGetLocation, std::string p_strTelNum)
{
	PROTOCOL::CSmsLocationSync l_oSync;

	BuildRespondHeader(l_oSync.m_oHeader, SMS_LOCATION_SYNC, TOPIC);

	l_oSync.m_oBody.m_strResult = m_pString->Number(p_oGetLocation.m_iResult);
	l_oSync.m_oBody.m_strTelnum = p_strTelNum;
	l_oSync.m_oBody.m_strLong = m_pString->Number(p_oGetLocation.m_dX);
	l_oSync.m_oBody.m_strLat = m_pString->Number(p_oGetLocation.m_dY);
	l_oSync.m_oBody.m_strVenderError = m_pString->Number(p_oGetLocation.m_iVenderError);
	l_oSync.m_oBody.m_strDescription = p_oGetLocation.m_szDescription;// m_pString->AnsiToUtf8(SmsUtil::toString(p_oGetLocation.m_szDescription, sizeof(p_oGetLocation.m_szDescription)));
	
	std::string l_strSendMsg = l_oSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::Transmit2ThirdAlarm(const SmsBodySmsArrival& p_oSmsAlarm)
{
	std::string l_strTAGuid;
	if (IsChangeInfo(SmsUtil::toString(p_oSmsAlarm.m_szSender, sizeof(p_oSmsAlarm.m_szSender)), l_strTAGuid))
	{
		TransmitChangeInfo(p_oSmsAlarm, l_strTAGuid);
	}
	else
	{
		TransmitSmsAlarm(p_oSmsAlarm);
	}
}

#define TIME_YYYYMMDDHHmmSS_LEN 14

std::string CTcpHelper::_TimeStandard(const std::string& strTmpTime)
{
	if (strTmpTime.empty() || strTmpTime.size() != TIME_YYYYMMDDHHmmSS_LEN)
	{
		return "";
	}

	std::string strYear = strTmpTime.substr(0, 4);
	std::string strMonth = strTmpTime.substr(4, 2);
	std::string strDay = strTmpTime.substr(6, 2);
	std::string strHour = strTmpTime.substr(8, 2);
	std::string strMinute = strTmpTime.substr(10, 2);
	std::string strSecond = strTmpTime.substr(12, 2);

	char szTmp[32] = { 0 };
	sprintf(szTmp, "%s-%s-%s %s:%s:%s", strYear.c_str(), strMonth.c_str(), strDay.c_str(), strHour.c_str(), strMinute.c_str(), strSecond.c_str());
	return szTmp;
}

void CTcpHelper::TransmitSmsAlarm(const SmsBodySmsArrival& p_oSmsAlarm)
{
	PROTOCOL::CSmsTransmitAlarmRequest l_oRequest;

	BuildRespondHeader(l_oRequest.m_oHeader, TA_ADD_ALARM_REQUEST, QUEUE);

	l_oRequest.m_oBody.m_strType = THIRD_TYPE_SMS;
	l_oRequest.m_oBody.m_strGuid = SmsUtil::toString(p_oSmsAlarm.m_szID, sizeof(p_oSmsAlarm.m_szID));
	l_oRequest.m_oBody.m_strUserPhone = SmsUtil::toString(p_oSmsAlarm.m_szSender, sizeof(p_oSmsAlarm.m_szSender));
	l_oRequest.m_oBody.m_strCrcontent = p_oSmsAlarm.m_pContent;// m_pString->AnsiToUtf8(SmsUtil::toString(p_oSmsAlarm.m_pContent, p_oSmsAlarm.m_iContentLen));
	std::string strTmpTime = SmsUtil::toString(p_oSmsAlarm.m_szTime, sizeof(p_oSmsAlarm.m_szTime));
	l_oRequest.m_oBody.m_strCreateDate = _TimeStandard(strTmpTime);
	l_oRequest.m_oBody.m_strToOrgID = m_strDeptCode;
	l_oRequest.m_oBody.m_strToOrgName = m_strDeptName;
	l_oRequest.m_oBody.m_strLong = m_pString->Number(p_oSmsAlarm.m_dX);
	l_oRequest.m_oBody.m_strLat = m_pString->Number(p_oSmsAlarm.m_dY);
	l_oRequest.m_oBody.m_strAddress = p_oSmsAlarm.m_szDescription;// m_pString->AnsiToUtf8(SmsUtil::toString(p_oSmsAlarm.m_szDescription, sizeof(p_oSmsAlarm.m_szDescription)));

	std::string l_strSendMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::TransmitChangeInfo(const SmsBodySmsArrival& p_oSmsAlarm, std::string p_strTAGuid)
{
	PROTOCOL::CSmsTransmitChangeRequest l_oRequest;

	BuildRespondHeader(l_oRequest.m_oHeader, TA_SET_CHANGEINFO_REQUEST, QUEUE);

	l_oRequest.m_oBody.m_strGuid = SmsUtil::toString(p_oSmsAlarm.m_szID, sizeof(p_oSmsAlarm.m_szID));
	l_oRequest.m_oBody.m_strThirdAlarmGuid = p_strTAGuid;
	l_oRequest.m_oBody.m_strOrientation = ORIENTATION_CONTECT_TO_POLICE;
	l_oRequest.m_oBody.m_strState = STATE_UNREAD;
	l_oRequest.m_oBody.m_strPhone = SmsUtil::toString(p_oSmsAlarm.m_szSender, sizeof(p_oSmsAlarm.m_szSender));
	l_oRequest.m_oBody.m_strChangeType = '1';
	l_oRequest.m_oBody.m_strChangeContent = p_oSmsAlarm.m_pContent;// m_pString->AnsiToUtf8(SmsUtil::toString(p_oSmsAlarm.m_pContent, p_oSmsAlarm.m_iContentLen));
	std::string strTmpTime = SmsUtil::toString(p_oSmsAlarm.m_szTime, sizeof(p_oSmsAlarm.m_szTime));
	l_oRequest.m_oBody.m_strChangeDate = _TimeStandard(strTmpTime);
	l_oRequest.m_oBody.m_strLong = m_pString->Number(p_oSmsAlarm.m_dX);
	l_oRequest.m_oBody.m_strLat = m_pString->Number(p_oSmsAlarm.m_dY);
	l_oRequest.m_oBody.m_strRemark1 = p_oSmsAlarm.m_szDescription;// m_pString->AnsiToUtf8(SmsUtil::toString(p_oSmsAlarm.m_szDescription, sizeof(p_oSmsAlarm.m_szDescription)));

	std::string l_strSendMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSendMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CTcpHelper::SendReceipted(std::string p_strSeqID)
{
	std::string l_strRespContent;
	std::string l_strTelNum;
	if (!BuildReceipted(l_strTelNum, l_strRespContent, p_strSeqID))
	{
		return;
	}

	SendSmsInfo(l_strTelNum, l_strRespContent);
}

bool CTcpHelper::BuildReceipted(std::string& p_strTelNum, std::string& p_strRespContent, std::string p_strSeqID)
{
	p_strRespContent = m_strRespContent;

	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;

	l_Sql.param["guid"] = p_strSeqID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{
		p_strTelNum = l_pResult->GetValue("user_phone");

		if (m_pString->Find(m_strRespContent,"{0}"))
		{
			p_strRespContent = m_pString->ReplaceAll(m_strRespContent, "{0}", l_pResult->GetValue("to_org_name"));
		}
		else if (m_pString->Find(m_strRespContent, "{1}"))
		{
			p_strRespContent = m_pString->ReplaceAll(m_strRespContent, "{1}", l_pResult->GetValue("to_org_user_name"));
		}
		else if (m_pString->Find(m_strRespContent, "{2}"))
		{
			p_strRespContent = m_pString->ReplaceAll(m_strRespContent, "{2}", l_pResult->GetValue("to_org_user_id"));
		}
		else if (m_pString->Find(m_strRespContent, "{3}"))
		{
			p_strRespContent = m_pString->ReplaceAll(m_strRespContent, "{3}", l_pResult->GetValue("caseid"));
		}
	}

	return true;
}

bool CTcpHelper::IsSmsRead(std::string p_strSeqID)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_THIRD_ALARM;
	l_Sql.param["guid"] = p_strSeqID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_THIRD_ALARM, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{
		std::string l_strType;
		std::string l_strState;
		l_strType = l_pResult->GetValue("type");
		l_strState = l_pResult->GetValue("state");

		if ((l_strType != THIRD_TYPE_SMS) || (l_strState != STATE_READ))
		{
			ICC_LOG_DEBUG(m_pLog, "Is Not Sms Read!");
			return false;
		}
	}

	return true;
}

bool CTcpHelper::IsChangeInfo(std::string p_strTelNum, std::string& p_strTAGuid)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_IS_CHANGEINFO;
	l_Sql.param["user_phone"] = p_strTelNum.c_str();
	l_Sql.param["ischangeinfotime"] = m_pString->Number(m_uiIsChangeInfoTime);

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_IS_CHANGEINFO, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{
		p_strTAGuid = l_pResult->GetValue("guid");
		return true;
	}

	return false;
}

bool ICC::CTcpHelper::SelectDeptNameByDeptCode(std::string p_strDeptCode, std::string& p_strDeptName)
{
	DataBase::SQLRequest l_Sql;
	l_Sql.sql_id = SELECT_ICC_T_DEPT;
	l_Sql.param["code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_Sql);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql[%s] fail[%s]", SELECT_ICC_T_DEPT, l_pResult->GetErrorMsg().c_str());
		return false;
	}

	// 获取第三方警情信息
	if (l_pResult->Next())
	{
		p_strDeptName = l_pResult->GetValue("name");
		return true;
	}

	return false;
}

void CTcpHelper::BuildRespondHeader(PROTOCOL::CHeader& p_oHeader, std::string p_strCmd, RequestType p_Type)
{
	p_oHeader.m_strSystemID = SYSTEMID;
	p_oHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oHeader.m_strMsgid = m_pString->CreateGuid();
	p_oHeader.m_strRelatedID = "";
	p_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oHeader.m_strCmd = p_strCmd;
	if (p_Type == TOPIC)
	{
		p_oHeader.m_strRequest = ICC_TOPIC_THIRD_ALARM;
		p_oHeader.m_strRequestType = "1";
	}
	else
	{
		p_oHeader.m_strRequest = ICC_QUEUE_THIRD_ALARM;
		p_oHeader.m_strRequestType = "0";
	}
	p_oHeader.m_strResponse = "";
	p_oHeader.m_strResponseType = "";
}

void CTcpHelper::BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader)
{
	p_oRespondHeader.m_strSystemID = SYSTEMID;
	p_oRespondHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oRespondHeader.m_strMsgid = m_pString->CreateGuid();
	p_oRespondHeader.m_strRelatedID = p_oRequestHeader.m_strMsgid;
	p_oRespondHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oRespondHeader.m_strCmd = m_mapCmdName[p_oRequestHeader.m_strCmd];
	p_oRespondHeader.m_strRequest = p_oRequestHeader.m_strResponse;
	p_oRespondHeader.m_strRequestType = p_oRequestHeader.m_strResponseType;
	p_oRespondHeader.m_strResponse = "";
	p_oRespondHeader.m_strResponseType = "";
}

void CTcpHelper::BeginProcessThread()
{
	try
	{
		// 创建链路检测线程
		if (m_pThreadCheckLink == nullptr)
		{
			m_bIsStopCheckLinkThread = false;
			m_pThreadCheckLink = boost::make_shared<boost::thread>(boost::bind(&CTcpHelper::S_RunCheckLink, this, m_uiCheckLinkTime));
		}

		// 创建短信处理线程
		if (m_pThreadProcess == nullptr)
		{
			m_bIsStopProcessThread = false;
			m_pThreadProcess = boost::make_shared<boost::thread>(boost::bind(&CTcpHelper::S_RunProcess, this, m_uiProcessTime));
		}
	}
	catch (...)
	{
		//
	}
}
void CTcpHelper::EndProcessThread()
{
	m_bIsStopCheckLinkThread = true;
	m_bIsStopProcessThread = true;

	if (m_pThreadCheckLink)
	{
		if (m_pThreadCheckLink->joinable())
		{
			m_pThreadCheckLink->join();
			m_pThreadCheckLink = nullptr;
		}
	}

	if (m_pThreadProcess)
	{
		if (m_pThreadProcess->joinable())
		{
			m_pThreadProcess->join();
			m_pThreadProcess = nullptr;
		}
	}
}
void CTcpHelper::S_RunCheckLink(CTcpHelper* p_pThread, unsigned int p_uiCheckLinkTime)
{
	p_pThread->CheckLink(p_uiCheckLinkTime);
}

void CTcpHelper::S_RunProcess(CTcpHelper* p_pThread, unsigned int p_uiProcessTime)
{
	p_pThread->Process(p_uiProcessTime);
}

void CTcpHelper::CheckLink(unsigned int p_uiCheckLinkTime)
{
	ICC_LOG_INFO(m_pLog, "CheckLinkThread Started.");

	while (!m_bIsStopCheckLinkThread)
	{
		if (GetSmsLoginStatus())
		{
			// 发送心跳包
			SendCheckLinkReq();
			
			// 确保在调用OnStop之后1秒内可以结束该函数
			for (int i = 0; i < p_uiCheckLinkTime;i++)
			{
				if (m_bIsStopCheckLinkThread)
				{
					return;
				}
				if (!GetSmsLoginStatus())
				{
					break;
				}
				try
				{
					m_pThreadCheckLink->sleep(boost::get_system_time() + boost::posix_time::seconds(1));
				}				
				catch (...)
				{
				}
				
			}
		}
		else
		{
			try
			{
				m_pThreadCheckLink->sleep(boost::get_system_time() + boost::posix_time::milliseconds(500));
			}			
			catch (...)
			{
				ICC_LOG_ERROR(m_pLog, "Sleep throw error !");
			}
			
		}
	}

	ICC_LOG_INFO(m_pLog, "Stop CheckLinkThread");
}

void CTcpHelper::Process(unsigned int p_uiProcessTime)
{
	ICC_LOG_INFO(m_pLog, "ProcessThread Started.");

	while (!m_bIsStopProcessThread)
	{
		ProcessSmsRecvBuf();

		// 确保在调用OnStop之后1秒内可以结束该函数
		for (int i = 0; i < p_uiProcessTime; i++)
		{
			if (m_bIsStopProcessThread)
			{
				return;
			}

			try
			{
				m_pThreadProcess->sleep(boost::get_system_time() + boost::posix_time::seconds(1));
			}			
			catch (...)
			{
				ICC_LOG_ERROR(m_pLog, "Sleep throw error !");
			}
		}
	}

	ICC_LOG_INFO(m_pLog, "ProcessThread Stop.");
}

bool CTcpHelper::ProcessSmsRecvBuf()
{
	// 取出一个完整的消息
	SmsHeader l_SmsHeader;
	// 仅仅存储消息体
	SmsBuffer l_SmsBuffer;
	{
		Lock::AutoLock lock(m_SmsMutex);
		// 获取消息头		
		if (!l_SmsHeader.SetBuffer(m_SmsBuffer.GetBuffer(), m_SmsBuffer.GetLength()))
		{
			//ICC_LOG_DEBUG("m_SmsBuffer.GetLength[%d], Not enough for SmsHeader", m_SmsBuffer.GetLength());
			return false;
		}
		ICC_LOG_LOWDEBUG(m_pLog, "SmsHeader[%s]", l_SmsHeader.GetInfo().c_str());

		// 判断消息的完整性
		if (l_SmsHeader.m_nTotalLength > m_SmsBuffer.GetLength())
		{
			//ICC_LOG_DEBUG("m_SmsBuffer.GetLength[%d], Not enough for SmsBody", m_SmsBuffer.GetLength());
			return false;
		}

		// 取出消息体
		l_SmsBuffer.Append(m_SmsBuffer.GetBuffer() + l_SmsHeader.GetLength(), l_SmsHeader.m_nTotalLength - l_SmsHeader.GetLength());
		m_SmsBuffer.OffsetLeft(l_SmsHeader.m_nTotalLength);

		ICC_LOG_LOWDEBUG(m_pLog, "SmsBody[%s]", SmsUtil::FormatBuffer(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength()).c_str());
	}

	// 处理消息
	switch (l_SmsHeader.m_nCommandID)
	{
	case CMD_LOGIN_RESP:
	{
		OnCmdLoginResp(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_CHECKALIVE_RESP:
	{
		OnCmdCheckAliveResp(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_GETLOCATION_RESP:
	{
		OnCmdGetLocationResp(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_SETREADED_RESP:
	{
		OnCmdSetReadedResp(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_SENDMSG_RESP:
	{
		OnCmdSendMSGResp(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_SERVERSTATUSARRIVAL:
	{
		OnCmdServerStatusArrival(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_SMSARRIVAL:
	{
		OnCmdSmsAlarmArrival(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	case CMD_USERSTATUSARRIVAL:
	{
		OnCmdUserStatusArrival(l_SmsBuffer.GetBuffer(), l_SmsBuffer.GetLength(), l_SmsHeader.m_nSequenceID);
		break;
	}
	default:
	{
		ICC_LOG_ERROR(m_pLog, "Unsupported CommandID[0x%04X]", (unsigned int)l_SmsHeader.m_nCommandID);
		break;
	}
	}

	return true;
}

bool CTcpHelper::GetSmsLoginStatus()
{
	Lock::AutoLock lock(m_Mutex);
	return m_bSmsLoginStatus;
}

void CTcpHelper::SetSmsLoginStatus(bool p_status)
{
	Lock::AutoLock lock(m_Mutex);
	m_bSmsLoginStatus = p_status;
}

bool CTcpHelper::SendCheckLinkReq()
{
	SmsHeader l_smsHeader;
	l_smsHeader.SetInfo(l_smsHeader.GetLength(), SmsUtil::NextSeqID4Heart(), CMD_CHECKALIVE);

	SmsBuffer l_smsManager;
	l_smsManager.Append(&l_smsHeader);

	ICC_LOG_LOWDEBUG(m_pLog, "Send HeartBeat l_smsHeader[%s], l_smsManager[%s]", l_smsHeader.GetInfo().c_str(),
		SmsUtil::FormatBuffer(l_smsManager.GetBuffer(), l_smsManager.GetLength()).c_str());

	if (0 == m_TcpClient->Send(l_smsManager.GetBuffer(), l_smsManager.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}
	return true;
}

bool CTcpHelper::SendLogin()
{
	SmsBodyLogin l_smsBodyLogin;
	l_smsBodyLogin.SetInfo(m_strUserName, m_strPassword);

	SmsHeader l_smsHeader;
	l_smsHeader.SetInfo(l_smsHeader.GetLength() + l_smsBodyLogin.GetLength(), SmsUtil::NextSeqID4Sms(), CMD_LOGIN);

	ICC_LOG_DEBUG(m_pLog, "l_smsHeader.GetLength[%d], l_smsHeader.GetInfo[%s], l_smsBodyLogin.GetLength[%d], l_smsBodyLogin.GetInfo[%s]",
		l_smsHeader.GetLength(), l_smsHeader.GetInfo().c_str(), l_smsBodyLogin.GetLength(), l_smsBodyLogin.GetInfo().c_str());

	SmsBuffer l_smsManager;
	l_smsManager.Append(&l_smsHeader);
	l_smsManager.Append(&l_smsBodyLogin);

	ICC_LOG_DEBUG(m_pLog, "Send Login : [%s]",
		SmsUtil::FormatBuffer(l_smsManager.GetBuffer(), l_smsManager.GetLength()).c_str());

	if (0 == m_TcpClient->Send(l_smsManager.GetBuffer(), l_smsManager.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Login Message Fail!");
		return false;
	}

	{ // 缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		std::map<std::string, std::string> tmp_info;
		tmp_info.insert(std::make_pair("User", SmsUtil::toString(l_smsBodyLogin.m_user, sizeof(l_smsBodyLogin.m_user))));
		m_mapSmsReqInfo.insert(std::make_pair(l_smsHeader.m_nSequenceID, tmp_info));
		ICC_LOG_DEBUG(m_pLog, "Add Login Request Id[%d], m_mapSmsReqInfo size[%d]", l_smsHeader.m_nSequenceID, m_mapSmsReqInfo.size());
	}

	return true;
}

bool CTcpHelper::SendSetReaded(std::string p_szTypeid)
{
	std::string t_szTypeid = p_szTypeid;

	// 发送已读标志
	SmsBodySetReaded l_smsBodySetReaded;
	l_smsBodySetReaded.SetInfo(t_szTypeid);

	SmsHeader l_smsHeader;
	l_smsHeader.SetInfo(l_smsHeader.GetLength() + l_smsBodySetReaded.GetLength(), SmsUtil::NextSeqID4Icc(), CMD_SETREADED);

	ICC_LOG_DEBUG(m_pLog, "l_smsHeader.GetLength[%d], l_smsHeader.GetInfo[%s], l_smsBodySetReaded.GetLength[%d], l_smsBodySetReaded.GetInfo[%s]",
		l_smsHeader.GetLength(), l_smsHeader.GetInfo().c_str(), l_smsBodySetReaded.GetLength(), l_smsBodySetReaded.GetInfo().c_str());

	SmsBuffer l_smsManager;
	l_smsManager.Append(&l_smsHeader);
	l_smsManager.Append(&l_smsBodySetReaded);

	ICC_LOG_DEBUG(m_pLog, "Send SetReaded [%s]",
		SmsUtil::FormatBuffer(l_smsManager.GetBuffer(), l_smsManager.GetLength()).c_str());

	if (0 == m_TcpClient->Send(l_smsManager.GetBuffer(), l_smsManager.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}

	{ // 缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		std::map<std::string, std::string> tmp_info;
		tmp_info.insert(std::make_pair("id", t_szTypeid));
		m_mapSmsReqInfo.insert(std::make_pair(l_smsHeader.m_nSequenceID, tmp_info));
		ICC_LOG_DEBUG(m_pLog, "Add SetReaded Request Id[%d], m_mapSmsReqInfo size[%d]", l_smsHeader.m_nSequenceID, m_mapSmsReqInfo.size());
	}

	return true;
}

bool CTcpHelper::SendGetLocation(std::string p_strTelNum)
{
	SmsBodyGetLocation l_smsBodyGetLocation;
	l_smsBodyGetLocation.SetInfo(p_strTelNum);

	SmsHeader l_smsHeader;
	l_smsHeader.SetInfo(l_smsHeader.GetLength() + l_smsBodyGetLocation.GetLength(), SmsUtil::NextSeqID4Icc(), CMD_GETLOCATION);

	ICC_LOG_DEBUG(m_pLog,"l_smsHeader.GetLength[%d], l_smsHeader.GetInfo[%s], l_smsBodyGetLocation.GetLength[%d], l_smsBodyGetLocation.GetInfo[%s]",
		l_smsHeader.GetLength(), l_smsHeader.GetInfo().c_str(), l_smsBodyGetLocation.GetLength(), l_smsBodyGetLocation.GetInfo().c_str());

	SmsBuffer l_smsManager;
	l_smsManager.Append(&l_smsHeader);
	l_smsManager.Append(&l_smsBodyGetLocation);

	ICC_LOG_DEBUG(m_pLog,"Send GetLocation [%s]",
		SmsUtil::FormatBuffer(l_smsManager.GetBuffer(), l_smsManager.GetLength()).c_str());

	if (0 == m_TcpClient->Send(l_smsManager.GetBuffer(), l_smsManager.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}

	{ // 缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		std::map<std::string, std::string> tmp_info;
		tmp_info.insert(std::make_pair("TelNum", SmsUtil::toString(l_smsBodyGetLocation.m_szTelNum, sizeof(l_smsBodyGetLocation.m_szTelNum))));
		m_mapSmsReqInfo.insert(std::make_pair(l_smsHeader.m_nSequenceID, tmp_info));
		ICC_LOG_DEBUG(m_pLog, "Add GetLocation Request Id[%d], m_mapSmsReqInfo size[%d]", l_smsHeader.m_nSequenceID, m_mapSmsReqInfo.size());
	}
	return true;
}

bool CTcpHelper::SendSmsInfo(std::string p_strTelNum, std::string p_strContent)
{
	SmsBodySendMsg l_smsBodySendSms;
	l_smsBodySendSms.SetInfo(p_strTelNum, p_strContent);

	SmsHeader l_smsHeader;
	l_smsHeader.SetInfo(l_smsHeader.GetLength() + l_smsBodySendSms.GetLength(), SmsUtil::NextSeqID4Icc(), CMD_SENDMSG);

	ICC_LOG_DEBUG(m_pLog, "l_smsHeader.GetLength[%d], l_smsHeader.GetInfo[%s], l_smsBodySendSms.GetLength[%d], l_smsBodySendSms.GetInfo[%s]",
		l_smsHeader.GetLength(), l_smsHeader.GetInfo().c_str(), l_smsBodySendSms.GetLength(), l_smsBodySendSms.GetInfo().c_str());

	SmsBuffer l_smsManager;
	l_smsManager.Append(&l_smsHeader);
	l_smsManager.Append(&l_smsBodySendSms);

	ICC_LOG_DEBUG(m_pLog, "Send Sms[%s]",
		SmsUtil::FormatBuffer(l_smsManager.GetBuffer(), l_smsManager.GetLength()).c_str());
	if (0 == m_TcpClient->Send(l_smsManager.GetBuffer(), l_smsManager.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}

	{ // 缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		std::map<std::string, std::string> tmp_info;
		tmp_info.insert(std::make_pair("TelNum", SmsUtil::toString(l_smsBodySendSms.m_szTelNum, sizeof(l_smsBodySendSms.m_szTelNum))));
		m_mapSmsReqInfo.insert(std::make_pair(l_smsHeader.m_nSequenceID, tmp_info));
		ICC_LOG_DEBUG(m_pLog, "Add Send Sms Request Id[%d], m_mapSmsReqInfo size[%d]", l_smsHeader.m_nSequenceID, m_mapSmsReqInfo.size());
	}
	return true;
}

bool CTcpHelper::SendServerStatusResp(int seqID)
{
	SmsHeader l_SmsHeader;
	l_SmsHeader.m_nTotalLength = l_SmsHeader.GetLength();
	l_SmsHeader.m_nSequenceID = seqID;
	l_SmsHeader.m_nCommandID = CMD_SERVERSTATUSARRIVAL_RESP;
	if (0 == m_TcpClient->Send(l_SmsHeader.GetBuffer(), l_SmsHeader.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "Send ServerStatusArrival Respond to Server: l_SmsHeader.GetInfo[%s]", l_SmsHeader.GetInfo().c_str());
	return true;
}

bool CTcpHelper::SendUserStatusResp(int seqID)
{
	SmsHeader l_SmsHeader;
	l_SmsHeader.m_nTotalLength = l_SmsHeader.GetLength();
	l_SmsHeader.m_nSequenceID = seqID;
	l_SmsHeader.m_nCommandID = CMD_USERSTATUSARRIVAL_RESP;
	if (0 == m_TcpClient->Send(l_SmsHeader.GetBuffer(), l_SmsHeader.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "Send UserStatusArrival Respond to Server: l_SmsHeader.GetInfo[%s]", l_SmsHeader.GetInfo().c_str());
	return true;
}

bool CTcpHelper::SendSmsAlarmResp(int seqID)
{
	SmsHeader l_SmsHeader;
	l_SmsHeader.m_nTotalLength = l_SmsHeader.GetLength();
	l_SmsHeader.m_nSequenceID = seqID;
	l_SmsHeader.m_nCommandID = CMD_SMSARRIVAL_RESP;
	if (0 == m_TcpClient->Send(l_SmsHeader.GetBuffer(), l_SmsHeader.GetLength()))
	{
		ICC_LOG_ERROR(m_pLog, "Send Message Fail!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "Send SmsArrival Respond to Server: l_SmsHeader.GetInfo[%s]", l_SmsHeader.GetInfo().c_str());
	return true;
}

void CTcpHelper::OnCmdCheckAliveResp(const char* buf, int nLen, int seqID)
{
	ICC_LOG_LOWDEBUG(m_pLog, "Receive HeartBeat Resp");
}

void CTcpHelper::OnCmdLoginResp(const char* buf, int nLen, int seqID)
{
	SmsBodyLoginResp resp;
	if (!resp.SetBuffer(buf, nLen))
	{
		ICC_LOG_ERROR(m_pLog, "resp.SetBuffer failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "resp.GetLength[%d], resp.GetInfo[%s]", resp.GetLength(), resp.GetInfo().c_str());

	std::string l_strUser;
	{ // 读取缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		auto it = m_mapSmsReqInfo.find(seqID);
		if (it == m_mapSmsReqInfo.end())
		{
			ICC_LOG_ERROR(m_pLog, "m_mapSmsReqInfo.find(%d) failed", seqID);
			return;
		}
		l_strUser = it->second["User"];

		m_mapSmsReqInfo.erase(it);
		ICC_LOG_DEBUG(m_pLog, "Del Req Id[%d], tmp_User[%s], m_mapSmsReqInfo size[%d]", seqID, l_strUser.c_str(), m_mapSmsReqInfo.size());
	}

	// 主题发送服务器状态
	SetSmsLoginStatus(0 == resp.m_iResult);
}

void CTcpHelper::OnCmdSetReadedResp(const char* buf, int nLen, int seqID)
{
	std::string tmp_id;
	{// 读取缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		auto it = m_mapSmsReqInfo.find(seqID);
		if (it == m_mapSmsReqInfo.end())
		{
			ICC_LOG_ERROR(m_pLog, "m_mapSmsReqInfo.find(%d) failed", seqID);
			return;
		}
		tmp_id = it->second["id"];

		m_mapSmsReqInfo.erase(it);
		ICC_LOG_DEBUG(m_pLog, "Del Req Id[%d], tmp_id[%s], m_mapSmsReqInfo size[%d]", seqID, tmp_id.c_str(), m_mapSmsReqInfo.size());
	}
}

void CTcpHelper::OnCmdGetLocationResp(const char* buf, int nLen, int seqID)
{
	SmsBodyGetLocationResp resp;
	if (!resp.SetBuffer(buf, nLen))
	{
		ICC_LOG_ERROR(m_pLog, "resp.SetBuffer failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "resp.GetLength[%d], resp.GetInfo[%s]", resp.GetLength(), resp.GetInfo().c_str());

	std::string l_strTelNum;

	{ // 读取缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		auto it = m_mapSmsReqInfo.find(seqID);
		if (it == m_mapSmsReqInfo.end())
		{
			ICC_LOG_ERROR(m_pLog, "m_mapSmsReqInfo.find(%d) failed", seqID);
			return;
		}
		l_strTelNum = it->second["TelNum"];

		m_mapSmsReqInfo.erase(it);
		ICC_LOG_DEBUG(m_pLog, "Del Req Id[%d], tmp_TelNum[%s], m_mapSmsReqInfo size[%d]", seqID, l_strTelNum.c_str(), m_mapSmsReqInfo.size());
	}

	// 发送定位信息同步
	OnNotifiLocationSync(resp, l_strTelNum);
}

void CTcpHelper::OnCmdSendMSGResp(const char* buf, int nLen, int seqID)
{
	std::string tmp_TelNum;
	{ // 读取缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		auto it = m_mapSmsReqInfo.find(seqID);
		if (it == m_mapSmsReqInfo.end())
		{
			ICC_LOG_ERROR(m_pLog, "m_mapSmsReqInfo.find(%d) failed", seqID);
			return;
		}
		tmp_TelNum = it->second["TelNum"];

		m_mapSmsReqInfo.erase(it);
		ICC_LOG_DEBUG(m_pLog, "Del Req Id[%d], tmp_TelNum[%s], m_mapSmsReqInfo size[%d] ", seqID, tmp_TelNum.c_str(), m_mapSmsReqInfo.size());
	}
}

void CTcpHelper::OnCmdServerStatusArrival(const char* buf, int nLen, int seqID)
{
	SmsBodyServerStatusArrival resp;
	if (!resp.SetBuffer(buf, nLen))
	{
		ICC_LOG_ERROR(m_pLog, "resp.SetBuffer failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "ServerStatusArrival, resp.GetLength[%d], resp.GetInfo[%s]", resp.GetLength(), resp.GetInfo().c_str());

	// 设置服务器状态
	SetSmsLoginStatus(resp.m_bStatus > 0);

	// 发送服务器状态同步
	OnNotifiServerStatusSync(resp);

	// 发送服务器状态应答
	SendServerStatusResp(seqID);	
}

void CTcpHelper::OnCmdUserStatusArrival(const char* buf, int nLen, int seqID)
{
	SmsBodyUserStatusArrival resp;
	if (!resp.SetBuffer(buf, nLen))
	{
		ICC_LOG_ERROR(m_pLog, "resp.SetBuffer failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "UserStatusArrival, resp.GetLength[%d], resp.GetInfo[%s]", resp.GetLength(), resp.GetInfo().c_str());

	// 发送单位状态同步
	OnNotifiUserStatusSync(resp);

	// 发送单位状态应答
	SendUserStatusResp(seqID);
}

void CTcpHelper::OnCmdSmsAlarmArrival(const char* buf, int nLen, int seqID)
{
	SmsBodySmsArrival resp;
	if (!resp.SetBuffer(buf, nLen))
	{
		ICC_LOG_ERROR(m_pLog, "resp.SetBuffer failed");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "SmsAlarmArrival, resp.GetLength[%d], resp.GetInfo[%s]", resp.GetLength(), resp.GetInfo().c_str());

	// 向客户端发送短信
	Transmit2ThirdAlarm(resp);

	// 发送收到短信应答
	SendSmsAlarmResp(seqID);

	// 缓存seqID信息
	{ // 缓存请求信息
		Lock::AutoLock lock(m_Mutex);
		std::string l_strID = SmsUtil::toString(resp.m_szID, sizeof(resp.m_szID)).c_str();
		m_mapSmsResInfo[l_strID] = l_strID;
		ICC_LOG_DEBUG(m_pLog, "Add m_mapSmsResInfo, Id: [%s], [%d]", l_strID.c_str(), m_mapSmsResInfo.size());
	}
	
	AddOwerSms(SmsUtil::toString(resp.m_szID, sizeof(resp.m_szID)).c_str(),
		SmsUtil::toString(resp.m_szSender, sizeof(resp.m_szSender)).c_str());
}

//////////////////////////////////////////////////////////////////////////
TcpClientImpl::TcpClientImpl(CTcpHelper* p_pTcpHelper)
	:m_pTcpHelper(p_pTcpHelper)
{
	//
}
TcpClientImpl::~TcpClientImpl()
{
	//
}
void TcpClientImpl::OnReceived(const char* p_pData, unsigned int p_iLength)
{
	Lock::AutoLock lock(m_pTcpHelper->m_SmsMutex);
	m_pTcpHelper->m_SmsBuffer.Append(p_pData, p_iLength);
}

void TcpClientImpl::OnDisconnected(std::string p_strError)
{
	ICC_LOG_ERROR(m_pTcpHelper->m_pLog, "Connect Sms Server Fail:[%s]", p_strError.c_str());

	if (m_pTcpHelper->GetSmsLoginStatus())
	{ // 主题发送服务器状态
		m_pTcpHelper->SetSmsLoginStatus(false);
	}
	m_pTcpHelper->EndProcessThread();
}

void TcpClientImpl::OnConnected(std::string p_strError)
{
	ICC_LOG_DEBUG(m_pTcpHelper->m_pLog, "Connect Sms Server Success:[%s]", p_strError.c_str());
	
	m_pTcpHelper->BeginProcessThread();
	m_pTcpHelper->SendLogin();
}

