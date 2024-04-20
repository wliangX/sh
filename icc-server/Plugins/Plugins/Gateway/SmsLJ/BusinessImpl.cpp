#include "Boost.h"
#include "BusinessImpl.h"

CBusinessImpl::CBusinessImpl()
{
	//
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	printf("smslj init begin!\n");
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_SMSLJ_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pJsonFty = ICCGetIJsonFactory();
	m_Mutex = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	// 读取配置
	ReadConfig();

	printf("smslj init end!\n");
}

void CBusinessImpl::OnStart()
{
	if (m_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}	
	
	// 注册观察者
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_STATE_SYNC, OnNotifiSetReadSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TA_CHANGEINFO_SYNC, OnNotifiSendSmsSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, SMS_GET_LOCATION_REQUEST, OnNotifiGetLocationRequest);

	ConnectServer();

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);	
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::ReadConfig()
{
	m_strIsUsing = m_pConfig->GetValue("ICC/Plugin/SmsLJ/IsUsing", "0");

	std::string l_strServiceIP = m_pConfig->GetValue("ICC/Plugin/SmsLJ/ServiceIP", "127.0.0.1");
	unsigned int l_uiServicePort = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/SmsLJ/ServicePort", "60000"));
	unsigned int l_uiProcessTime = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/SmsLJ/ProcessTime", "1"));
	unsigned int l_uiCheckLinkTime = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/SmsLJ/CheckLinkTime", "3"));
	unsigned int l_uiReconnectTime = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/SmsLJ/ReconnectTime", "3"));
	unsigned int l_uiIsChangeInfoTime = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/SmsLJ/IsChangeInfoTime", "7"));
	std::string l_strRespContent = m_pConfig->GetValue("ICC/Plugin/SmsLJ/RespContent", "{0}已受理您的短信报警");

	unsigned int l_nNodeCount = m_pConfig->GetNodeCount("ICC/Plugin/SmsLJ/Depts", "Dept");
	for (unsigned int i = 0; i < l_nNodeCount; ++i)
	{
		std::string l_strDeptCode = m_pConfig->GetValue("ICC/Plugin/SmsLJ/Depts/Dept[" + std::to_string(i) + "]/DeptCode", "");
		std::string l_strUserName = m_pConfig->GetValue("ICC/Plugin/SmsLJ/Depts/Dept[" + std::to_string(i) + "]/UserName", "icc");
		std::string l_strPassword = m_pConfig->GetValue("ICC/Plugin/SmsLJ/Depts/Dept[" + std::to_string(i) + "]/Password", "icc");
		std::string l_strDeptName;
		if (!SelectDeptNameByDeptCode(l_strDeptCode, l_strDeptName))
		{
			ICC_LOG_ERROR(m_pLog, "SelectDeptNameByDepeCode: [%s] Failed!", l_strDeptCode.c_str());
			continue;
		}

		ICC_LOG_DEBUG(m_pLog, "Load Dept, DeptCode: [%s], DeptName: [%s], UserName: [%s],Password: [%s]",
			l_strDeptCode.c_str(), l_strDeptName.c_str(), l_strUserName.c_str(), l_strPassword.c_str());

		boost::shared_ptr<CTcpHelper> l_pTcpHelper = boost::make_shared<CTcpHelper>();
		l_pTcpHelper->SetConnParam(l_strDeptCode, l_strDeptName, l_strServiceIP, l_uiServicePort, l_strUserName, l_strPassword);
		l_pTcpHelper->SetProcessParam(l_strRespContent, l_uiCheckLinkTime, l_uiProcessTime, l_uiIsChangeInfoTime);
		l_pTcpHelper->SetLogPtr(m_pLog);
		l_pTcpHelper->SetDBConnPtr(m_pDBConn);
		l_pTcpHelper->SetDateTimePtr(m_pDateTime);
		l_pTcpHelper->SetStringUtilPtr(m_pString);
		l_pTcpHelper->SetTimerManagerPtr(m_pTimerMgr);
		l_pTcpHelper->SetJsonFactoryPtr(m_pJsonFty);
		l_pTcpHelper->SetObserverCenterPtr(m_pObserverCenter);
		l_pTcpHelper->SetLockPtr(ICCGetILockFactory()->CreateLock(Lock::TypeMutex));
		l_pTcpHelper->SetSmsLockPtr(ICCGetILockFactory()->CreateLock(Lock::TypeMutex));
		l_pTcpHelper->SetTcpClientPtr(ICCGetITcpClientFactory()->CreateTcpClient());
		l_pTcpHelper->LoadUnReadSms();
		l_pTcpHelper->LoadOwnerSms();

		m_mapTcpHelper[l_strDeptCode] = l_pTcpHelper;
	}
}
bool ICC::CBusinessImpl::SelectDeptNameByDeptCode(std::string p_strDeptCode, std::string& p_strDeptName)
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
void CBusinessImpl::ConnectServer()
{
	Lock::AutoLock lock(m_Mutex);
	for (auto l_objTcpHelper : m_mapTcpHelper)
	{
		l_objTcpHelper.second->Connect();
	}
}

void CBusinessImpl::OnNotifiSetReadSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSmsSetReadSync l_oSync;
	if (!l_oSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse failed, message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	Lock::AutoLock lock(m_Mutex);
	std::string l_strGuid = l_oSync.m_oBody.m_strID;
	for (auto l_objTcpHelper : m_mapTcpHelper)
	{
		if (l_objTcpHelper.second->IsOwnerSms(l_strGuid))
		{
			ICC_LOG_DEBUG(m_pLog, "Found SmsOwner id:[%s]", l_strGuid.c_str());
			l_objTcpHelper.second->OnNotifiSetReadSync(p_pNotify);

			break;
		}
	}
}

void CBusinessImpl::OnNotifiSendSmsSync(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CSmsSendSmsSync l_oSync;
	if (!l_oSync.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Parse failed, message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if ("1" != l_oSync.m_oBody.m_strSyncType || l_oSync.m_oBody.m_strThirdType != THIRD_TYPE_SMS || l_oSync.m_oBody.m_strOrientation != ORIENTATION_POLICE_TO_CONTECT)
	{
		ICC_LOG_DEBUG(m_pLog, "Is not new changeinfo, no need to transmit to sms services!");
		return;
	}

	Lock::AutoLock lock(m_Mutex);
	std::string l_strGuid = l_oSync.m_oBody.m_strTAGuid;
	for (auto l_objTcpHelper : m_mapTcpHelper)
	{
		if (l_objTcpHelper.second->IsOwnerSms(l_strGuid))
		{
			ICC_LOG_DEBUG(m_pLog, "Found SmsOwner id:[%s]", l_strGuid.c_str());
			l_objTcpHelper.second->OnNotifiSendSmsSync(p_pNotify);

			break;
		}
	}
}

void CBusinessImpl::OnNotifiGetLocationRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSmsGetLocationRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	Lock::AutoLock lock(m_Mutex);
	std::string l_strPhone = l_oRequest.m_oBody.m_strTelnum;
	for (auto l_objTcpHelper : m_mapTcpHelper)
	{
		if (l_objTcpHelper.second->HasSmsByPhone(l_strPhone))
		{
			ICC_LOG_DEBUG(m_pLog, "Found phone id:[%s]", l_strPhone.c_str());
			l_objTcpHelper.second->OnNotifiGetLocationRequest(p_pNotify);

			break;
		}
	}
}
