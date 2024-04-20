#include "Boost.h"
#include "BusinessImpl.h"
#include "ProcessSeatManager.h"
#include <fstream>

#define LOAD_WAITTIME 1000

#define MQ_DEST_QUEUE "0"
#define MQ_DEST_TOPIC "1"
#define QUEUE_BASEDATA			"queue_basedata"
#define BIND_TYPE_USER_STAFF	"user_bind_staff"		//绑定类型-用户绑定警员
#define BIND_TYPE_USER_ROLE		"user_bind_role"		//绑定类型-用户绑定角色
#define BIND_TYPE_USER_DEPT		"user_bind_dept"		//绑定类型-用户绑定单位
#define BIND_TYPE_ROLE_DATA		"role_bind_data"		//绑定类型-角色绑定数据
#define BIND_TYPE_ROLE_FUNC		"role_bind_func"		//绑定类型-角色绑定功能
//#define BIND_TYPE_ROLE_SYSTEM	"role_bind_sytem"		//绑定类型-角色绑定系统

#define SYNDATA_FAILED  -1
#define SYNDATA_WAITNEXTDATA 1
#define SYNDATA_SUCCESS 0

const std::string CODE_SUCCESS = "200";

enum THREADID
{
	THREADID_BASE = 10000,
	THREADID_REQUEST_LANGUAGE,
	THREADID_REQUEST_ORGN,
	THREADID_REQUEST_STAFF, 
	THREADID_REQUEST_DICT,
	THREADID_REQUEST_FUNC,
	THREADID_REQUEST_USER,
	THREADID_REQUEST_USER_BIND_STAFF,
	THREADID_REQUEST_SYS_ROLE,
	THREADID_REQUEST_USER_ROLE,

	THREADID_RECEIVE_CHANGED_NOTIFY_DICT,
	THREADID_RECEIVE_CHANGED_NOTIFY_USER,
	THREADID_RECEIVE_CHANGED_NOTIFY_STAFF,
	THREADID_RECEIVE_CHANGED_NOTIFY_FUNC,
	THREADID_RECEIVE_CHANGED_NOTIFY_ROLE,
	THREADID_RECEIVE_CHANGED_NOTIFY_DEPT
};

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_SMP_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pLicense = ICCGetLicenseFactory()->CreateLicense();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

	m_bStartedTimerFlag = false;
	m_bFirstSynDataFlag = true;	

	m_bNeedSynData = true;
	m_bIsSyning = false;

	m_bSynDataSuccess = false;

	_InitDataAnalyzers();
}

void CBusinessImpl::OnStart()
{		
	m_strNacosServerIp = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerIp", "127.0.0.1");
	m_strNacosServerPort = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerPort", "8848");
	m_strNacosNamespace = m_pConfig->GetValue("ICC/Component/HttpServer/NacosNamespace", "dev");
	m_strSmpServiceName = m_pConfig->GetValue("ICC/Plugin/SMP/smpservicename", "center-smp-northface-service");
	m_strNacosGroupName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosGroupName", "master");
	m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");	
	m_strNacosSuccessFlag = m_pConfig->GetValue("ICC/Component/HttpServer/successflag", "ok");
	m_strSmpServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/SMP/servicehealthyflag", "1");

	m_strSyncProcessSeatFlag = m_pConfig->GetValue("ICC/Plugin/SMP/SynProcessSeatFlag", "0");

	ICC_LOG_DEBUG(m_pLog, "sync process seat flag:%s!!", m_strSyncProcessSeatFlag.c_str());

	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/SMP/IsUsing", "1");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}	

	m_strSmpServerIp = m_pConfig->GetValue("ICC/Plugin/SMP/serverip", "");
	if (m_strSmpServerIp.empty())
	{
		m_strSmpServerIp = "10.161.21.49";
	}
	m_strSmpPort = m_pConfig->GetValue("ICC/Plugin/SMP/serverport", "");
	if (m_strSmpPort.empty())
	{
		m_strSmpPort = "26000";
	}
	m_strUser = m_pConfig->GetValue("ICC/Plugin/SMP/user", "");
	if (m_strUser.empty())
	{
		m_strUser = "test5";
	}
	m_strPassword = m_pConfig->GetValue("ICC/Plugin/SMP/password", "");
	if (m_strPassword.empty())
	{
		m_strPassword = "123456";
	}
	m_strLoginType = m_pConfig->GetValue("ICC/Plugin/SMP/logintype", "");
	if (m_strLoginType.empty())
	{
		m_strLoginType = "0";
	}
	m_strSystemCode = m_pConfig->GetValue("ICC/Plugin/SMP/systemcode", "");
	if (m_strSystemCode.empty())
	{
		m_strSystemCode = "SYS_CODE_ICC";
	}

	m_strPublicCode = m_pConfig->GetValue("ICC/Plugin/SMP/publiccode", "");
	if (m_strPublicCode.empty())
	{
		m_strPublicCode = "SYS_CODE_PUBLIC";
	}

	m_strGatewayServiceName = m_pConfig->GetValue("ICC/Plugin/SMP/gatewayservicename", "");
	if (m_strGatewayServiceName.empty())
	{
		//m_strGatewayServiceName = "/center-smp-northface-service";
	}
	std::string strPackageSize = m_pConfig->GetValue("ICC/Plugin/SMP/syndatapackagesize", "");
	if (strPackageSize.empty())
	{
		strPackageSize = "200";
	}
	m_iPackageSize = std::stoi(strPackageSize);

	m_strIgnoreHttpError = m_pConfig->GetValue("ICC/Plugin/SMP/ignorehttperror", "");


	m_nTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/SMP/TimeOut", "3"));

	ICC_LOG_DEBUG(m_pLog, "begin load base data!!");

	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();

	/*while (!_LoadBaseData())
	{
		ICC_LOG_ERROR(m_pLog, "smp load base data failed!!");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}*/

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullBeginTime2 = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

	ICC_LOG_DEBUG(m_pLog, "end load base data!! user time[%lld]", ullBeginTime2 - ullBeginTime);

	LoadDeptMapInfo();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dict_language_info", OnGetDictLanguageInfo);//1
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_organ_Infor", OnGetOrganInfor);//4
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_staff_infor", OnGetStaffInfor);//5
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_dict_infor", OnGetDictInfor);//2
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_smp_user_info", OnGetSmpUserInfo);//6
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_auth_infor", OnGetAuthInfor);//3
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_smp_user_staff_bind", OnGetSmpUserStaffBind);//7
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_sys_role_info", OnGetSysRoleInfo);//9
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_user_role_info", OnGetUserRoleInfo);//8

	// 接收SMP推送消息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "send_smp_language_notify", OnSmpLanguageNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpOrg", OnSmpOrgNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpStaff", OnSmpStaffNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpDict", OnSmpDictNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpUser", OnSmpUserNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpRole", OnSmpRoleNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpFunction", OnSmpAuthNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smpSeats", OnSmpSeatNotify);

	//客户端获取license信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_expiration_request", OnGetExpiration);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smp_timer", OnRequestInfo);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "smp_license", OnCheckLicense);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_server_lock", OnNotifiDoubleServerSync);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_syndata_result_request", OnGetSynDataResult);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);

	m_strSyncTime = m_pConfig->GetValue("ICC/Plugin/SMP/SyncTime", "86400");
	m_strSyncHour = m_pConfig->GetValue("ICC/Plugin/SMP/SyncHour", "3");
	if (m_pString->ToInt(m_strSyncHour) < 0 || m_pString->ToInt(m_strSyncHour) >= 24)
	{
		m_strSyncHour = "3";
	}
	m_strSyncMinute = m_pConfig->GetValue("ICC/Plugin/SMP/SyncMinute", "0");
	if (m_pString->ToInt(m_strSyncMinute) < 0 || m_pString->ToInt(m_strSyncMinute) >= 60)
	{
		m_strSyncMinute = "0";
	}
	m_strCheckLicense = m_pConfig->GetValue("ICC/Plugin/SMP/CheckLicense", "60");
	m_strAutoSyncUserStaffBind = m_pConfig->GetValue("ICC/Plugin/SMP/AutoSyncUserStaffBind", "1");	

	std::string l_strFirstSynData = m_pConfig->GetValue("ICC/Plugin/SMP/FirstSyncData", "1");
	m_bFirstSynDataFlag = (l_strFirstSynData.compare("1") == 0 ? true : false);
	if(!m_bFirstSynDataFlag)
	{
		ICC_LOG_DEBUG(m_pLog, "Don't get smp data!");
	}

	ICC_LOG_DEBUG(m_pLog, "syn data time hour = %s, minute = %s.", m_strSyncHour.c_str(), m_strSyncMinute.c_str());

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

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

	_SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncServerLock l_oSyncObj;
	if (!l_oSyncObj.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	std::string strServerId(m_pConfig->ServerFlag());

	ICC_LOG_DEBUG(m_pLog, "Server Role Sync,Config ServerFalg: %s, clientid: %s", strServerId.c_str(), l_oSyncObj.m_oBody.m_strClientID.c_str());

	if (l_oSyncObj.m_oBody.m_strClientID.find(strServerId) != std::string::npos && l_oSyncObj.m_oBody.m_strServiceState == "master")
	{
		ICC_LOG_DEBUG(m_pLog, "receive sync lock message. %s will start get data!", MODULE_NAME);

		if (!m_bStartedTimerFlag)
		{
			m_bStartedTimerFlag = true;

			std::string strSynCheckInterval = m_pConfig->GetValue("ICC/Plugin/SMP/SynCheckInterval", "600");
			if (strSynCheckInterval.empty())
			{
				strSynCheckInterval = "600";
			}
			int iSynCheckInterval = std::stoi(strSynCheckInterval);
			std::string l_strRequestInfoTimerName = m_pTimerMgr->AddTimer("smp_timer", iSynCheckInterval, 10);
			std::string l_strCheckLicenseTimerName = m_pTimerMgr->AddTimer("smp_license", m_pString->ToInt(m_strCheckLicense), 10);

			ICC_LOG_DEBUG(m_pLog, "start get data timer complete!", MODULE_NAME);
		}
	}	
}

void CBusinessImpl::RequestInfoFunc()
{

	m_threadSynData.reset(new boost::thread(boost::bind(&CBusinessImpl::_SynData, this)));

	
	

	//m_threadProc.AddMessage(this, THREADID_REQUEST_LANGUAGE);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_FUNC);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_DICT);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_ORGN);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_STAFF);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_USER);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_USER_BIND_STAFF);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_USER_ROLE);
	//m_threadProc.AddMessage(this, THREADID_REQUEST_SYS_ROLE);
}

void CBusinessImpl::_RequestLanguageInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_dict_language_info", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strLanguageVersion;
	l_oCSmpSyncRequest.m_strType = TYPE_TIME;
	std::string strGetLanguageInforMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetLanguageInforMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetLanguageInforMsg.c_str());
}

void CBusinessImpl::_RequestOrgnInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_organ_Infor", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strDeptVersion;
	l_oCSmpSyncRequest.m_oBody.m_strPucSysType = m_strSystemType;
	l_oCSmpSyncRequest.m_strType = TYPE_TIME;
	std::string strGetOrganInforMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetOrganInforMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetOrganInforMsg.c_str());
}

void CBusinessImpl::_RequestStaffInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_staff_infor", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strStaffVersion;
	l_oCSmpSyncRequest.m_strType = TYPE_TIME;
	std::string strGetStaffInfoMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetStaffInfoMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetStaffInfoMsg.c_str());
}

void CBusinessImpl::_RequestDictInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_dict_infor", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strDictVersion;
	l_oCSmpSyncRequest.m_oBody.m_strSystemNo = "ICC";
	l_oCSmpSyncRequest.m_strType = TYPE_SYSTEM_TIME;
	std::string strGetDictInfoMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetDictInfoMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetDictInfoMsg.c_str());
}

void CBusinessImpl::_RequestFuncInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_auth_infor", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strAuthVersion;
	l_oCSmpSyncRequest.m_oBody.m_strSystemNo = "ICC";
	l_oCSmpSyncRequest.m_strType = TYPE_SYSTEM;
	std::string strGetAuthInfoMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetAuthInfoMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetAuthInfoMsg.c_str());
}

void CBusinessImpl::_RequestUserInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_smp_user_info", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strUserVersion;
	l_oCSmpSyncRequest.m_strType = TYPE_TIME;
	std::string strGetUserInforMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetUserInforMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetUserInforMsg.c_str());
}

void CBusinessImpl::_RequestUserStaffBindInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_smp_user_staff_bind", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strUserStaffBindVersion;
	if (0 == m_strAutoSyncUserStaffBind.compare("0"))
	{
		l_oCSmpSyncRequest.m_oBody.m_strSystemNo = "ICC";
		l_oCSmpSyncRequest.m_strType = TYPE_SYSTEM_TIME;
	}
	else
	{
		l_oCSmpSyncRequest.m_strType = TYPE_ALL;
	}
	std::string strGetSmpUserStaffBindMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetSmpUserStaffBindMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetSmpUserStaffBindMsg.c_str());
}

void CBusinessImpl::_RequestSysRoleInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_sys_role_info", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strRoleAuthorityBindVersion;
	l_oCSmpSyncRequest.m_oBody.m_strSystemNo = "ICC";
	l_oCSmpSyncRequest.m_strType = TYPE_SYSTEM_TIME;
	std::string strGetSmpRoleFuncBindMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetSmpRoleFuncBindMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetSmpRoleFuncBindMsg.c_str());
}

void CBusinessImpl::_RequestUserRoleInfo()
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = "queue_smp_sharedata";
	l_pRequestHeaderInit.m_strResponseType = "0";

	PROTOCOL::CSmpSyncRequest l_oCSmpSyncRequest;

	GeneralHeader("get_user_role_info", l_pRequestHeaderInit, l_oCSmpSyncRequest.m_oHeader);
	l_oCSmpSyncRequest.m_oBody.m_strVersion = m_dataVersion.m_strUserRoleBindVersion;
	l_oCSmpSyncRequest.m_oBody.m_strSystemNo = "ICC";
	l_oCSmpSyncRequest.m_strType = TYPE_SYSTEM_TIME;
	std::string strGetSmpUserRoleBindMsg = l_oCSmpSyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strGetSmpUserRoleBindMsg, ObserverPattern::ERequestMode::Request_Respond));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", strGetSmpUserRoleBindMsg.c_str());
}

int CBusinessImpl::_ProcReceiveOrgnInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataOrg orgDatas;

	//解析协议
	if (!orgDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive Dept from smp, islast:package:packagesize:count[%s:%s:%s:%d]", bIsLast ? "true" : "false", orgDatas.m_strCurrentPackageCount.c_str(), orgDatas.m_strPackageSize.c_str(), orgDatas.m_vecOrgs.size());

	if (orgDatas.m_vecOrgs.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no department info received from smp:[%d],return", orgDatas.m_vecOrgs.size());
		return SYNDATA_SUCCESS;
	}	

	std::vector<Data::CDeptInfo> l_vecDeptInfo;
	for (auto l_dept : orgDatas.m_vecOrgs)
	{
		//过滤系统类型，单位状态为有效
		if (l_dept.m_strEnableFlag == "1")
		{
			if (l_dept.m_strOrgGovCode.empty())
			{
				ICC_LOG_WARNING(m_pLog, "[smp] OrgInfo No OrgGovCode,do not send sync msg:name[%s] guid[%s] parent_guid[%s] district_code[%s]",
					l_dept.m_strOrgName.c_str(), l_dept.m_strGuid.c_str(), l_dept.m_strParentGuid.c_str(), l_dept.m_strOrgRegionType.c_str());
				continue;
			}			

			Data::CDeptInfo l_deptInfo;

			_TransSmpOrgToIcc(l_dept, l_deptInfo);			

			l_vecDeptInfo.push_back(l_deptInfo);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid dept data!! size[%d]", l_vecDeptInfo.size());

	boost::shared_ptr<Data::CDepartment> pProcesser = boost::dynamic_pointer_cast<Data::CDepartment>(m_pOrg);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "dept object is null!!!");
		return SYNDATA_FAILED;
	}
	
	pProcesser->Append(l_vecDeptInfo);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append dept success!! size[%d], total size[%d]", l_vecDeptInfo.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (bIsLast)
	{
		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update language to db failed!");
			return SYNDATA_FAILED;
		}		

		ICC_LOG_DEBUG(m_pLog, "update dept info success, valid[%d], update size[%d]",  l_vecDeptInfo.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "language is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendOrganRequest(l_CGetOrganInfoRequest.m_Body.m_vecData);

	return iRs;


	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetDeptInfoRequest l_CGetOrganInfoRequest;

	////解析协议
	//if (!l_CGetOrganInfoRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive dept from smp, count:value[%s : %d]", l_CGetOrganInfoRequest.m_Body.m_strCount.c_str(), l_CGetOrganInfoRequest.m_Body.m_vecData.size());

	//if (l_CGetOrganInfoRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no department info received from smp,return", l_CGetOrganInfoRequest.m_Body.m_vecData.size());
	//	return SYNDATA_SUCCESS;
	//}

	//int iTotalCount = 0;
	//if (!l_CGetOrganInfoRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetOrganInfoRequest.m_Body.m_strCount);
	//}

	//std::vector<Data::CDeptInfo> l_vecDeptInfo;
	//int l_iValidSize = 0;
	//for (auto l_dept : l_CGetOrganInfoRequest.m_Body.m_vecData)
	//{
	//	//过滤系统类型，单位状态为有效
	//	if ((l_dept.m_strPucSysType == "undef" || l_dept.m_strPucSysType == m_strSystemType) && l_dept.m_strEnableFlag == "1")
	//	{
	//		if (l_dept.m_strOrgGovCode.empty())
	//		{
	//			ICC_LOG_WARNING(m_pLog, "[smp] OrgInfo No OrgGovCode,do not send sync msg:name[%s] guid[%s] parent_guid[%s] district_code[%s]",
	//				l_dept.m_strOrgName.c_str(), l_dept.m_strGuid.c_str(), l_dept.m_strParentOrgGuid.c_str(), l_dept.m_strDistrictCode.c_str());
	//			continue;
	//		}
	//		++l_iValidSize;

	//		Data::CDeptInfo l_deptInfo;
	//		l_deptInfo.m_strGuid = l_dept.m_strGuid;

	//		l_deptInfo.m_strParentGuid = ((l_dept.m_strOrgGovCode == m_strRootDeptCode) ? l_dept.m_strGuid : l_dept.m_strParentOrgGuid);

	//		l_deptInfo.m_strCode = l_dept.m_strOrgGovCode;
	//		l_deptInfo.m_strDistrictCode = l_dept.m_strDistrictCode;
	//		l_deptInfo.m_strType = TransformDeptTypeCode(l_dept.m_strOrgType);
	//		l_deptInfo.m_strName = l_dept.m_strOrgName;
	//		l_deptInfo.m_strPhone = l_dept.m_strContactno;
	//		l_deptInfo.m_strLevel = "";
	//		l_deptInfo.m_strShortcut = l_dept.m_strOrgShortName;
	//		l_deptInfo.m_strSort = l_dept.m_strSeq;

	//		l_vecDeptInfo.push_back(l_deptInfo);
	//	}
	//}

	//ICC_LOG_DEBUG(m_pLog, "valid dept data!! size[%d]", l_vecDeptInfo.size());

	//boost::shared_ptr<Data::CDepartment> pProcesser = boost::dynamic_pointer_cast<Data::CDepartment>(m_pOrg);
	//if (!pProcesser)
	//{
	//	ICC_LOG_DEBUG(m_pLog, "dept object is null!!!");
	//	return SYNDATA_FAILED;
	//}

	//pProcesser->AppendReceiveDataSize(l_CGetOrganInfoRequest.m_Body.m_vecData.size());
	//pProcesser->Append(l_vecDeptInfo);
	//int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//ICC_LOG_DEBUG(m_pLog, "append dept success!! size[%d], total size[%d]", l_vecDeptInfo.size(), iTotalDataSize);

	//int iRs = SYNDATA_FAILED;
	//if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//{
	//	/*if (m_dataVersion.m_strLanguageVersion == l_CGetLanguageInfoRequest.m_Body.m_strVersionData)
	//	{
	//		ICC_LOG_ERROR(m_pLog, "language version is same, not need update");
	//		return true;
	//	}*/

	//	//批量更新
	//	if (!pProcesser->SynUpdate())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update language to db failed!");
	//		return SYNDATA_FAILED;
	//	}

	//	m_dataVersion.m_strLanguageVersion = l_CGetOrganInfoRequest.m_Body.m_strVersionData;
	//	
	//	ICC_LOG_DEBUG(m_pLog, "update dept info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecDeptInfo.size(), iTotalDataSize);

	//	iRs = SYNDATA_SUCCESS;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "language is not enough, wait next data!!");
	//	iRs = SYNDATA_WAITNEXTDATA;
	//}

	////发送通知到base服务
	////SendOrganRequest(l_CGetOrganInfoRequest.m_Body.m_vecData);

	//return iRs;	
}

void CBusinessImpl::OnGetOrganInfor(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive dept info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive dept from smp, [%s]", strTmp.c_str());
	
	int iRs = _ProcReceiveOrgnInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn dept data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_STAFF);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn dept data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn dept data failed!");
	}
}

bool CBusinessImpl::_ProcReceiveOrgnInfoNotify(const std::string& strMessage)
{

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse dept notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CDeptInfo> vecAddDatas;
	std::vector<Data::CDeptInfo> vecUpdateDatas;
	std::vector<Data::CDeptInfo> vecDeleteDatas;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CDeptInfo dataInfo;
			dataInfo.m_strGuid = data.strGuid;
			vecDeleteDatas.push_back(dataInfo);
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "org notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataOrgNotify dataNotify;
			if (!dataNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dataNotify.m_dataOrg.m_strEnableFlag == "1")
			{
				
				Data::CDeptInfo dataInfo;

				_TransSmpOrgToIcc(dataNotify.m_dataOrg, dataInfo);				

				//组织关系未处理

				std::string l_strVal;
				if ((data.strAction == ACTION_UPDATE) && (m_pRedisClient->HGet(DEPT_INFO_KEY, dataInfo.m_strGuid, l_strVal)))
				{
					ICC_LOG_ERROR(m_pLog, "update org guid:[%s]", dataInfo.m_strGuid.c_str());
					vecUpdateDatas.push_back(dataInfo);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "can not find org guid:[%s]", dataInfo.m_strGuid.c_str());
					vecAddDatas.push_back(dataInfo);
				}
			}			
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown dept action, action[%s]", data.strAction.c_str());
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CDepartment>(vecDeleteDatas, GetResourceManager());
		//批量删除数据库
		if (!pBaseData->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete dept from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CDepartment>(vecAddDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add dept from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CDepartment>(vecUpdateDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "add dept from db failed");
			return false;
		}
	}


	return true;




	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CDeptInfoChangeRequest l_oDeptInfoChangeRequest;
	////解析协议
	//if (!l_oDeptInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive department change message, size[%d]", l_oDeptInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oDeptInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no dept info changed");
	//	return false;
	//}	

	//std::vector<Data::CDeptInfo> l_vecDeptInfo;
	//if (l_oDeptInfoChangeRequest.m_Header.m_strAction == ACTION_DELETE)
	//{
	//	std::vector<Data::CBindInfo> vecRoleBindDepts;

	//	for (auto l_dept : l_oDeptInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDeptInfo l_deptInfo;
	//		l_deptInfo.m_strGuid = l_dept.m_strGuid;
	//		l_vecDeptInfo.push_back(l_deptInfo);

	//		Data::CBindInfo roleBindDept;
	//		roleBindDept.m_strToGuid = l_dept.m_strGuid;
	//		roleBindDept.m_strType = BIND_TYPE_ROLE_DATA;
	//		vecRoleBindDepts.push_back(roleBindDept);
	//	}

	//	if (!l_vecDeptInfo.empty())
	//	{
	//		Data::IBaseDataPtr l_pDepartment = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());
	//		//批量删除数据库
	//		if (!l_pDepartment->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete department from db failed");
	//			return false;
	//		}
	//	}

	//	if (!vecRoleBindDepts.empty())
	//	{
	//		Data::IBaseDataPtr pRoleBindDept = boost::make_shared<Data::CBind>(vecRoleBindDepts, GetResourceManager(), BIND_TYPE_ROLE_DATA);
	//		//批量删除数据库
	//		if (!pRoleBindDept->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind department from db failed");
	//			return false;
	//		}
	//	}	

	//	ICC_LOG_DEBUG(m_pLog, "delete dept info success, size[%d]", l_vecDeptInfo.size());		

	//	return true;
	//}
	//else if (l_oDeptInfoChangeRequest.m_Header.m_strAction == ACTION_ADD)
	//{
	//	for (auto l_dept : l_oDeptInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDeptInfo l_deptInfo;
	//		l_deptInfo.m_strGuid = l_dept.m_strGuid;
	//		l_deptInfo.m_strParentGuid = ((l_dept.m_strOrgGovCode == m_strRootDeptCode) ? l_dept.m_strGuid : l_dept.m_strParentOrgGuid);
	//		l_deptInfo.m_strCode = l_dept.m_strOrgGovCode;
	//		l_deptInfo.m_strDistrictCode = l_dept.m_strDistrictCode;
	//		l_deptInfo.m_strType = TransformDeptTypeCode(l_dept.m_strOrgType);
	//		l_deptInfo.m_strName = l_dept.m_strOrgName;
	//		l_deptInfo.m_strPhone = l_dept.m_strContactno;
	//		//l_deptInfo.m_strLevel = "";
	//		l_deptInfo.m_strShortcut = l_dept.m_strOrgShortName;
	//		l_deptInfo.m_strSort = l_dept.m_strSeq;

	//		l_vecDeptInfo.push_back(l_deptInfo);
	//	}

	//	Data::IBaseDataPtr l_pDepartment = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());
	//	//批量更新
	//	if (!l_pDepartment->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add department failed!");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "add dept info to db, size[%d]", l_vecDeptInfo.size());
	//	return true;
	//}
	//else if (l_oDeptInfoChangeRequest.m_Header.m_strAction == ACTION_UPDATE)
	//{
	//	for (auto l_dept : l_oDeptInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDeptInfo l_deptInfo;
	//		l_deptInfo.m_strGuid = l_dept.m_strGuid;
	//		l_deptInfo.m_strParentGuid = ((l_dept.m_strOrgGovCode == m_strRootDeptCode) ? l_dept.m_strGuid : l_dept.m_strParentOrgGuid);
	//		l_deptInfo.m_strCode = l_dept.m_strOrgGovCode;
	//		l_deptInfo.m_strDistrictCode = l_dept.m_strDistrictCode;
	//		l_deptInfo.m_strType = TransformDeptTypeCode(l_dept.m_strOrgType);
	//		l_deptInfo.m_strName = l_dept.m_strOrgName;
	//		l_deptInfo.m_strPhone = l_dept.m_strContactno;
	//		//l_deptInfo.m_strLevel = "";
	//		l_deptInfo.m_strShortcut = l_dept.m_strOrgShortName;
	//		l_deptInfo.m_strSort = l_dept.m_strSeq;

	//		l_vecDeptInfo.push_back(l_deptInfo);
	//	}

	//	Data::IBaseDataPtr l_pDepartment = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());
	//	//批量更新
	//	if (!l_pDepartment->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update department failed!");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "update dept info to db, size[%d]", l_vecDeptInfo.size());
	//	return true;
	//}
	//else
	//{
	//	ICC_LOG_WARNING(m_pLog, "unknown department action!!action[%s]", l_oDeptInfoChangeRequest.m_Header.m_strAction.c_str());
	//}
	//return true;
}

void CBusinessImpl::OnSmpOrgNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		return;
	}
	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive dept change message, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_DEPT, 0, 0, 0, strTmp);

	/*if (!_ProcReceiveOrgnInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process dept change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process dept change success!");
	}*/
}

void CBusinessImpl::SendOrganRequest(const std::vector<PROTOCOL::CGetDeptInfoRequest::CBody::CData>& p_DeptInfo)
{
	for (auto l_dept :  p_DeptInfo)
	{
		//过滤系统类型，单位状态为有效
		if ((l_dept.m_strPucSysType == "undef" || l_dept.m_strPucSysType == m_strSystemType) && l_dept.m_strEnableFlag == "1")
		{
			if (l_dept.m_strOrgGovCode.empty())
			{
				ICC_LOG_WARNING(m_pLog, "[smp] OrgInfo No OrgGovCode,do not send sync msg:name[%s] guid[%s] parent_guid[%s] district_code[%s]",
					l_dept.m_strOrgName.c_str(), l_dept.m_strGuid.c_str(), l_dept.m_strParentOrgGuid.c_str(), l_dept.m_strDistrictCode.c_str());
				continue;
			}

			PROTOCOL::CHeader l_pRequestHeaderInit;
			l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
			l_pRequestHeaderInit.m_strResponseType = "0";
			PROTOCOL::CSetDeptRequest l_oSetDeptRequest;

			l_oSetDeptRequest.m_oBody.m_strGuid = m_pString->CreateGuid();

			GeneralHeader("set_dept_request", l_pRequestHeaderInit, l_oSetDeptRequest.m_oHeader);
			l_oSetDeptRequest.m_oBody.m_strGuid = l_dept.m_strGuid;
			l_oSetDeptRequest.m_oBody.m_strParentGuid = ((l_dept.m_strOrgGovCode == m_strRootDeptCode) ? l_dept.m_strGuid : l_dept.m_strParentOrgGuid);
			l_oSetDeptRequest.m_oBody.m_strCode = l_dept.m_strOrgGovCode;
			l_oSetDeptRequest.m_oBody.m_strDistrictCode = l_dept.m_strDistrictCode;
			l_oSetDeptRequest.m_oBody.m_strType = TransformDeptTypeCode(l_dept.m_strOrgType);
			l_oSetDeptRequest.m_oBody.m_strName = l_dept.m_strOrgName;
			l_oSetDeptRequest.m_oBody.m_strPhone = l_dept.m_strContactno;
			l_oSetDeptRequest.m_oBody.m_strShortcut = l_dept.m_strOrgShortName;
			l_oSetDeptRequest.m_oBody.m_strSort = l_dept.m_strSeq;

			std::string l_strMsg = l_oSetDeptRequest.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}

void CBusinessImpl::SendOrganChangeRequest(const std::vector<PROTOCOL::CDeptInfoChangeRequest::CBody::CData>& p_DeptInfo, const std::string& p_strSyncType)
{
	for (auto l_dept :  p_DeptInfo)
	{
		PROTOCOL::CSetDeptRequest l_oSetDeptRequest;
		PROTOCOL::CHeader l_pRequestHeaderInit;
		l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
		l_pRequestHeaderInit.m_strResponseType = "0";
		l_pRequestHeaderInit.m_strMsgid = m_pString->CreateGuid();
		GeneralHeader("set_dept_request", l_pRequestHeaderInit, l_oSetDeptRequest.m_oHeader);
		l_oSetDeptRequest.m_oBody.m_strGuid = l_dept.m_strGuid;
		if (l_dept.m_strOrgGovCode == m_strRootDeptCode)
		{
			ICC_LOG_DEBUG(m_pLog, "root department set request:%s", l_dept.m_strOrgGovCode.c_str());
		}
		l_oSetDeptRequest.m_oBody.m_strParentGuid = ((l_dept.m_strOrgGovCode == m_strRootDeptCode) ? l_dept.m_strGuid : l_dept.m_strParentOrgGuid);
		l_oSetDeptRequest.m_oBody.m_strCode = l_dept.m_strOrgGovCode;
		l_oSetDeptRequest.m_oBody.m_strDistrictCode = l_dept.m_strDistrictCode;
		l_oSetDeptRequest.m_oBody.m_strType = TransformDeptTypeCode(l_dept.m_strOrgType);
		l_oSetDeptRequest.m_oBody.m_strName = l_dept.m_strOrgName;
		l_oSetDeptRequest.m_oBody.m_strPhone = l_dept.m_strContactno;
		l_oSetDeptRequest.m_oBody.m_strShortcut = l_dept.m_strOrgShortName;
		l_oSetDeptRequest.m_oBody.m_strSort = l_dept.m_strSeq;
		l_oSetDeptRequest.m_oBody.m_strSyncType = p_strSyncType;

		std::string l_strMsg = l_oSetDeptRequest.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}


int CBusinessImpl::_ProcReceiveStaffInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataStaff staffDatas;
	//协议解析
	if (!staffDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive staff from smp, islast:package:packagesize:count[%s:%s:%s:%d]", bIsLast ? "true" : "false", staffDatas.m_strCurrentPackageCount.c_str(), staffDatas.m_strPackageSize.c_str(), staffDatas.m_vecStaffs.size());

	if (staffDatas.m_vecStaffs.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff info received from smp,return");
		return SYNDATA_SUCCESS;
	}	

	std::vector<Data::CStaffInfo> l_vecStaffInfo;
	int l_iValidSize = 0;
	for (auto l_staff : staffDatas.m_vecStaffs)
	{
		//SMP只同步有效的警员数据到基础数据服务   m_strEnableFlag??????
		if (l_staff.m_strEnableFlag == "1")
		{
			++l_iValidSize;
			Data::CStaffInfo l_staffInfo;

			_TransSmpStaffToIcc(l_staff, l_staffInfo);			

			l_vecStaffInfo.push_back(l_staffInfo);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid staff data!! size[%d]", l_vecStaffInfo.size());

	boost::shared_ptr<Data::CStaff> pProcesser = boost::dynamic_pointer_cast<Data::CStaff>(m_pStaff);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "staff object is null!!!");
		return SYNDATA_FAILED;
	}
	
	pProcesser->Append(l_vecStaffInfo);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append staff success!! size[%d], total size[%d]", l_vecStaffInfo.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (bIsLast)
	{
		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update staff to db failed!");
			return SYNDATA_FAILED;
		}	

		ICC_LOG_DEBUG(m_pLog, "update staff info success, valid[%d], update size[%d]", l_vecStaffInfo.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "staff is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务


	return iRs;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetStaffInfoRequest l_CGetStaffInfoRequest;
	////协议解析
	//if (!l_CGetStaffInfoRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive staff from smp, count:value[%s : %d]", l_CGetStaffInfoRequest.m_Body.m_strCount.c_str(), l_CGetStaffInfoRequest.m_Body.m_vecData.size());


	//if (l_CGetStaffInfoRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no staff info received from smp,return");
	//	return SYNDATA_SUCCESS;
	//}

	//int iTotalCount = 0;
	//if (!l_CGetStaffInfoRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetStaffInfoRequest.m_Body.m_strCount);
	//}

	//std::vector<Data::CStaffInfo> l_vecStaffInfo;
	//int l_iValidSize = 0;
	//for (auto l_staff : l_CGetStaffInfoRequest.m_Body.m_vecData)
	//{
	//	//SMP只同步有效的警员数据到基础数据服务   m_strEnableFlag??????
	//	if (l_staff.m_strEnableFlag != "0")
	//	{
	//		++l_iValidSize;
	//		Data::CStaffInfo l_staffInfo;
	//		l_staffInfo.m_strGuid = l_staff.m_strGuid;
	//		l_staffInfo.m_strDeptGuid = l_staff.m_strOrgGuid;
	//		l_staffInfo.m_strCode = l_staff.m_strStaffCode;
	//		l_staffInfo.m_strName = l_staff.m_strStaffName;
	//		l_staffInfo.m_strSex = l_staff.m_strSex;
	//		l_staffInfo.m_strType = l_staff.m_strStaffType;
	//		l_staffInfo.m_strPosition = l_staff.m_strPosition;
	//		l_staffInfo.m_strMobile = l_staff.m_strMobile;
	//		l_staffInfo.m_strPhone = l_staff.m_strTelephone;
	//		l_staffInfo.m_strSort = l_staff.m_strSeq;
	//		l_staffInfo.m_strIsLeader = l_staff.m_strIsLeader;

	//		l_vecStaffInfo.push_back(l_staffInfo);
	//	}
	//}

	//ICC_LOG_DEBUG(m_pLog, "valid staff data!! size[%d]", l_vecStaffInfo.size());

	//boost::shared_ptr<Data::CStaff> pProcesser = boost::dynamic_pointer_cast<Data::CStaff>(m_pStaff);
	//if (!pProcesser)
	//{
	//	ICC_LOG_DEBUG(m_pLog, "staff object is null!!!");
	//	return SYNDATA_FAILED;
	//}

	//pProcesser->AppendReceiveDataSize(l_CGetStaffInfoRequest.m_Body.m_vecData.size());
	//pProcesser->Append(l_vecStaffInfo);
	//int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//ICC_LOG_DEBUG(m_pLog, "append staff success!! size[%d], total size[%d]", l_vecStaffInfo.size(), iTotalDataSize);

	//int iRs = SYNDATA_FAILED;
	//if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//{
	//	//批量更新
	//	if (!pProcesser->SynUpdate())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update staff to db failed!");
	//		return SYNDATA_FAILED;
	//	}

	//	m_dataVersion.m_strStaffVersion = l_CGetStaffInfoRequest.m_Body.m_strVersionData;
	//	
	//	ICC_LOG_DEBUG(m_pLog, "update staff info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecStaffInfo.size(), iTotalDataSize);

	//	iRs = SYNDATA_SUCCESS;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "staff is not enough, wait next data!!");
	//	iRs = SYNDATA_WAITNEXTDATA;
	//}

	////发送通知到base服务
	//

	//return iRs;	
}

void CBusinessImpl::OnGetStaffInfor(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive staff info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive staff from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveStaffInfo(strTmp);

	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn staff data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_USER);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn staff data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn staff data failed!");
	}
}

bool CBusinessImpl::_ProcReceiveStaffInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse staff notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CStaffInfo> vecAddDatas;
	std::vector<Data::CStaffInfo> vecUpdateDatas;
	std::vector<Data::CStaffInfo> vecDeleteDatas;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CStaffInfo dataInfo;
			dataInfo.m_strGuid = data.strGuid;
			vecDeleteDatas.push_back(dataInfo);
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "staff notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataStaffNotify dataNotify;
			if (!dataNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dataNotify.m_dataStaff.m_strEnableFlag == "1")
			{
				Data::CStaffInfo dataInfo;

				_TransSmpStaffToIcc(dataNotify.m_dataStaff, dataInfo);

				std::string l_strVal;
				if ((data.strAction == ACTION_UPDATE) && (m_pRedisClient->HGet(STAFF_INFO_KEY, dataInfo.m_strGuid, l_strVal)))
				{
					ICC_LOG_ERROR(m_pLog, "update staff guid:[%s]", dataInfo.m_strGuid.c_str());
					vecUpdateDatas.push_back(dataInfo);
				}				
				else
				{
					ICC_LOG_ERROR(m_pLog, "can not find staff guid:[%s]", dataInfo.m_strGuid.c_str());
					vecAddDatas.push_back(dataInfo);
				}
			}			
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown staff action, action[%s]", data.strAction.c_str());
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CStaff>(vecDeleteDatas, GetResourceManager());
		//批量删除数据库
		if (!pBaseData->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete staff from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CStaff>(vecAddDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add staff from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CStaff>(vecUpdateDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "add staff from db failed");
			return false;
		}
	}


	return true;


	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CStaffInfoChangeRequest l_oStaffInfoChangeRequest;
	////协议解析
	//if (!l_oStaffInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive staff change message, size[%d]", l_oStaffInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oStaffInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no staff info changed");
	//	return true;
	//}	

	//std::vector<Data::CStaffInfo> l_vecStaffInfo;
	//
	//if (l_oStaffInfoChangeRequest.m_Header.m_strAction == ACTION_DELETE)
	//{
	//	std::vector<Data::CBindInfo> vecBinds;

	//	for (auto l_staff : l_oStaffInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CStaffInfo l_staffInfo;
	//		l_staffInfo.m_strGuid = l_staff.m_strGuid;
	//		l_vecStaffInfo.push_back(l_staffInfo);

	//		Data::CBindInfo bindInfo;
	//		bindInfo.m_strToGuid = l_staff.m_strGuid;
	//		bindInfo.m_strType = BIND_TYPE_USER_STAFF;
	//		vecBinds.push_back(bindInfo);
	//	}

	//	Data::IBaseDataPtr l_pStaff = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());
	//	//批量删除数据库
	//	if (!l_pStaff->SingleDelete())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete staff from db failed");
	//		return false;
	//	}

	//	if (!vecBinds.empty())
	//	{
	//		Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(vecBinds, GetResourceManager(), BIND_TYPE_USER_STAFF);
	//		//批量删除数据库
	//		if (!l_pBind->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete bind from db failed");
	//			return false;
	//		}
	//	}	

	//	ICC_LOG_DEBUG(m_pLog, "delete staff info, size[%d]", l_vecStaffInfo.size());	

	//	return true;
	//}
	//else if (l_oStaffInfoChangeRequest.m_Header.m_strAction == ACTION_ADD)
	//{
	//	for (auto l_staff : l_oStaffInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CStaffInfo l_staffInfo;
	//		l_staffInfo.m_strGuid = l_staff.m_strGuid;
	//		l_staffInfo.m_strDeptGuid = l_staff.m_strOrgGuid;
	//		l_staffInfo.m_strCode = l_staff.m_strStaffCode;
	//		l_staffInfo.m_strName = l_staff.m_strStaffName;
	//		l_staffInfo.m_strSex = l_staff.m_strSex;
	//		l_staffInfo.m_strType = l_staff.m_strStaffType;
	//		l_staffInfo.m_strPosition = l_staff.m_strPosition;
	//		l_staffInfo.m_strMobile = l_staff.m_strMobile;
	//		l_staffInfo.m_strPhone = l_staff.m_strTelephone;
	//		l_staffInfo.m_strSort = l_staff.m_strSeq;
	//		l_staffInfo.m_strIsLeader = l_staff.m_strIsLeader;

	//		l_vecStaffInfo.push_back(l_staffInfo);
	//	}

	//	Data::IBaseDataPtr l_pStaff = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());
	//	//批量删除数据库
	//	if (!l_pStaff->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add staff info failed");
	//		return false;
	//	}
	//	ICC_LOG_DEBUG(m_pLog, "add staff info success, size[%d]", l_vecStaffInfo.size());
	//	return true;
	//}
	//else if (l_oStaffInfoChangeRequest.m_Header.m_strAction == ACTION_UPDATE)
	//{
	//	for (auto l_staff : l_oStaffInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CStaffInfo l_staffInfo;
	//		l_staffInfo.m_strGuid = l_staff.m_strGuid;
	//		l_staffInfo.m_strDeptGuid = l_staff.m_strOrgGuid;
	//		l_staffInfo.m_strCode = l_staff.m_strStaffCode;
	//		l_staffInfo.m_strName = l_staff.m_strStaffName;
	//		l_staffInfo.m_strSex = l_staff.m_strSex;
	//		l_staffInfo.m_strType = l_staff.m_strStaffType;
	//		l_staffInfo.m_strPosition = l_staff.m_strPosition;
	//		l_staffInfo.m_strMobile = l_staff.m_strMobile;
	//		l_staffInfo.m_strPhone = l_staff.m_strTelephone;
	//		l_staffInfo.m_strSort = l_staff.m_strSeq;
	//		l_staffInfo.m_strIsLeader = l_staff.m_strIsLeader;

	//		l_vecStaffInfo.push_back(l_staffInfo);
	//	}

	//	Data::IBaseDataPtr l_pStaff = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());
	//	//批量删除数据库
	//	if (!l_pStaff->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "Update staff info failed");
	//		return false;
	//	}
	//	ICC_LOG_DEBUG(m_pLog, "update staff info success, size[%d]", l_vecStaffInfo.size());
	//	return true;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "unknown staff action, action[%d]", l_oStaffInfoChangeRequest.m_Header.m_strAction.c_str());
	//}
	//return true;
}

void CBusinessImpl::OnSmpStaffNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive dept info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive staff change from smp, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_STAFF, 0, 0, 0, strTmp);

	/*if (!_ProcReceiveStaffInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process staff change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process staff change success!");
	}*/
}

void CBusinessImpl::SendStaffRequest(const std::vector<PROTOCOL::CGetStaffInfoRequest::CBody::CData>& p_StaffInfo)
{
	for (auto l_staff :  p_StaffInfo)
	{
		if (l_staff.m_strEnableFlag != "0")
		{
			PROTOCOL::CHeader l_pRequestHeader;
			PROTOCOL::CSetStaffRequest l_oSetStaffRequest;
			l_pRequestHeader.m_strResponse = QUEUE_BASEDATA;
			l_pRequestHeader.m_strResponseType = "0";
			GeneralHeader("set_staff_request", l_pRequestHeader, l_oSetStaffRequest.m_oHeader);

			l_oSetStaffRequest.m_oBody.m_strGuid = l_staff.m_strGuid;
			l_oSetStaffRequest.m_oBody.m_strDeptGuid = l_staff.m_strOrgGuid;
			l_oSetStaffRequest.m_oBody.m_strCode = l_staff.m_strStaffCode;
			l_oSetStaffRequest.m_oBody.m_strName = l_staff.m_strStaffName;
			l_oSetStaffRequest.m_oBody.m_strSex = l_staff.m_strSex;
			l_oSetStaffRequest.m_oBody.m_strType = l_staff.m_strStaffType;
			l_oSetStaffRequest.m_oBody.m_strPosition = l_staff.m_strPosition;
			l_oSetStaffRequest.m_oBody.m_strMobile = l_staff.m_strMobile;
			l_oSetStaffRequest.m_oBody.m_strPhone = l_staff.m_strTelephone;
			l_oSetStaffRequest.m_oBody.m_strShortcut = l_staff.m_strStaffName;
			l_oSetStaffRequest.m_oBody.m_strSort = l_staff.m_strSeq;
			l_oSetStaffRequest.m_oBody.m_strIsLeader = l_staff.m_strIsLeader;

			std::string l_strMsg = l_oSetStaffRequest.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}

void CBusinessImpl::SendStaffChangeRequest(const std::vector<PROTOCOL::CStaffInfoChangeRequest::CBody::CData>& p_StaffInfo, const std::string& p_strSyncType)
{
	for (auto l_staff :  p_StaffInfo)
	{
		PROTOCOL::CSetStaffRequest l_oSetStaffRequest;
		PROTOCOL::CHeader l_pRequestHeaderInit;
		l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
		l_pRequestHeaderInit.m_strResponseType = "0";

		GeneralHeader("set_staff_request", l_pRequestHeaderInit, l_oSetStaffRequest.m_oHeader);

		l_oSetStaffRequest.m_oHeader.m_strRelatedID = "Staff_Notify";
		l_oSetStaffRequest.m_oBody.m_strGuid = l_staff.m_strGuid;
		l_oSetStaffRequest.m_oBody.m_strDeptGuid = l_staff.m_strOrgGuid;
		l_oSetStaffRequest.m_oBody.m_strCode = l_staff.m_strStaffCode;
		l_oSetStaffRequest.m_oBody.m_strName = l_staff.m_strStaffName;
		l_oSetStaffRequest.m_oBody.m_strSex = l_staff.m_strSex;
		l_oSetStaffRequest.m_oBody.m_strType = l_staff.m_strStaffType;
		l_oSetStaffRequest.m_oBody.m_strPosition = l_staff.m_strPosition;
		l_oSetStaffRequest.m_oBody.m_strMobile = l_staff.m_strMobile;
		l_oSetStaffRequest.m_oBody.m_strPhone = l_staff.m_strTelephone;
		l_oSetStaffRequest.m_oBody.m_strShortcut = l_staff.m_strStaffName;
		l_oSetStaffRequest.m_oBody.m_strSort = l_staff.m_strSeq;
		l_oSetStaffRequest.m_oBody.m_strIsLeader = l_staff.m_strIsLeader;
		l_oSetStaffRequest.m_oBody.m_strSyncType = p_strSyncType;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetStaffRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

int CBusinessImpl::_ProcReceiveDictInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	JsonParser::IJsonPtr l_pIJsonEx = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataDict dictDatas;

	//协议解析
	if (!dictDatas.ParseStringEx(strMessage, l_pIJson, l_pIJsonEx))
	{
		ICC_LOG_ERROR(m_pLog, "is not json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive dict from smp, islast:package:packagesize:count[%s:%s:%s:%d]",bIsLast?"true":"false", dictDatas.m_strCurrentPackageCount.c_str(), dictDatas.m_strPackageSize.c_str(), dictDatas.m_vecDicts.size());

	if (dictDatas.m_vecDicts.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict info received from smp,return");
		return SYNDATA_SUCCESS;
	}	

	int l_iValid = 0;
	std::vector<Data::CDictInfo> l_vecDict;
	for (auto l_dict : dictDatas.m_vecDicts)
	{
		//ICC的数据字典是以DIC开头的,职务类型以 JYZW 开头
		if (l_dict.m_strEnableFlag == "1" && (l_dict.m_strSystemCode == m_strSystemCode || l_dict.m_strSystemCode == m_strPublicCode))
		{
			++l_iValid;
			//SMP只同步有效的字典数据到基础数据服务
			Data::CDictInfo l_dictInfo;
			
			_TransSmpDictToIcc(l_dict, l_dictInfo);

			l_vecDict.push_back(l_dictInfo);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid dict data!! size[%d], Valid[%d]", l_vecDict.size(), l_iValid);

	boost::shared_ptr<Data::CDictionary> pProcesser = boost::dynamic_pointer_cast<Data::CDictionary>(m_pDict);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "dict object is null!!!");
		return SYNDATA_FAILED;
	}
	
	pProcesser->Append(l_vecDict);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append dict success!! size[%d], total size[%d]", l_vecDict.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (bIsLast)
	{
		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update dict to db failed!");
			return SYNDATA_FAILED;
		}

		//m_dataVersion.m_strDictVersion = l_CGetDictInfoRequest.m_Body.m_strVersionData;

		//ICC_LOG_DEBUG(m_pLog, "update dict info success, total package[%s], current package[%s]", iTotalCount, l_vecDict.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "dict is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendDictRequest(l_CGetDictInfoRequest.m_Body.m_vecData);

	return iRs;


	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetDictInfoRequest l_CGetDictInfoRequest;
	////协议解析
	//if (!l_CGetDictInfoRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "is not json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive dict from smp, count:value[%s : %d]", l_CGetDictInfoRequest.m_Body.m_strCount.c_str(), l_CGetDictInfoRequest.m_Body.m_vecData.size());


	//if (l_CGetDictInfoRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no dict info received from smp,return");
	//	return SYNDATA_SUCCESS;
	//}	

	//int iTotalCount = 0;
	//if (!l_CGetDictInfoRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetDictInfoRequest.m_Body.m_strCount);
	//}

	//int l_iValid = 0;
	//std::vector<Data::CDictInfo> l_vecDict;
	//for (auto l_dict : l_CGetDictInfoRequest.m_Body.m_vecData)
	//{
	//	//ICC的数据字典是以DIC开头的,职务类型以 JYZW 开头
	//	if (l_dict.m_strStatus == "Y" && (m_pString->StartsWith(l_dict.m_strDictInfoKey, "DIC") || m_pString->StartsWith(l_dict.m_strDictInfoKey, "JYZW")))
	//	{
	//		++l_iValid;
	//		//SMP只同步有效的字典数据到基础数据服务
	//		Data::CDictInfo l_dictInfo;
	//		l_dictInfo.m_strGuid = l_dict.m_strGuid;
	//		l_dictInfo.m_strParentGuid = l_dict.m_strParentGuid;
	//		l_dictInfo.m_strCode = l_dict.m_strDictInfoKey;
	//		l_dictInfo.m_strSort = l_dict.m_strSort;

	//		for (auto var : l_dict.m_vecDictValue)
	//		{
	//			Data::CValue l_value;
	//			l_value.m_strGuid = var.m_strGuid;
	//			l_value.m_strLangGuid = var.m_strLanguageGuid;
	//			l_value.m_strValue = var.m_strValue;
	//			l_dictInfo.m_vecValue.push_back(l_value);
	//		}

	//		l_vecDict.push_back(l_dictInfo);
	//	}
	//}

	//ICC_LOG_DEBUG(m_pLog, "valid dict data!! size[%d]", l_vecDict.size());

	//boost::shared_ptr<Data::CDictionary> pProcesser = boost::dynamic_pointer_cast<Data::CDictionary>(m_pDict);
	//if (!pProcesser)
	//{
	//	ICC_LOG_DEBUG(m_pLog, "dict object is null!!!");
	//	return SYNDATA_FAILED;
	//}

	//pProcesser->AppendReceiveDataSize(l_CGetDictInfoRequest.m_Body.m_vecData.size());
	//pProcesser->Append(l_vecDict);
	//int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//ICC_LOG_DEBUG(m_pLog, "append dict success!! size[%d], total size[%d]", l_vecDict.size(), iTotalDataSize);

	//int iRs = SYNDATA_FAILED;
	//if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//{
	//	//批量更新
	//	if (!pProcesser->SynUpdate())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update dict to db failed!");
	//		return SYNDATA_FAILED;
	//	}

	//	m_dataVersion.m_strDictVersion = l_CGetDictInfoRequest.m_Body.m_strVersionData;

	//	ICC_LOG_DEBUG(m_pLog, "update dict info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecDict.size(), iTotalDataSize);

	//	iRs = SYNDATA_SUCCESS;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "dict is not enough, wait next data!!");
	//	iRs = SYNDATA_WAITNEXTDATA;
	//}

	////发送通知到base服务
	////SendDictRequest(l_CGetDictInfoRequest.m_Body.m_vecData);

	//return iRs;	



}

void CBusinessImpl::OnGetDictInfor(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive dict info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive dict from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveDictInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn dict data success!");

		m_threadProc.AddMessage(this, THREADID_REQUEST_FUNC);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn dict data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn dict data failed!");
	}
}

bool CBusinessImpl::_ProcReceiveDictInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas ;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse dict notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CDictInfo> vecAddDicts;
	std::vector<Data::CDictInfo> vecUpdateDicts;
	std::vector<Data::CDictInfo> vecDeleteDicts;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CDictInfo l_dictInfo;
			l_dictInfo.m_strGuid = data.strGuid;
			vecDeleteDicts.push_back(l_dictInfo);			
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "dict notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(),strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataDictNotify dicNotify;
			if (!dicNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dicNotify.m_dataDict.m_strEnableFlag == "1" && (dicNotify.m_dataDict.m_strSystemCode == m_strSystemCode || dicNotify.m_dataDict.m_strSystemCode == m_strPublicCode))
			{
				Data::CDictInfo l_dictInfo;

				_TransSmpDictToIcc(dicNotify.m_dataDict, l_dictInfo);

				std::string l_strVal;
				if ((data.strAction == ACTION_UPDATE) && (m_pRedisClient->HGet(DICT_KEY, l_dictInfo.m_strGuid, l_strVal)))
				{
					ICC_LOG_ERROR(m_pLog, "update dict guid:[%s]", l_dictInfo.m_strGuid.c_str());
					vecUpdateDicts.push_back(l_dictInfo);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "can not find dict guid:[%s]", l_dictInfo.m_strGuid.c_str());
					vecAddDicts.push_back(l_dictInfo);
				}
			}			
		}		
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown dictionary action, action[%s]", data.strAction.c_str());
		}
	}	

	{
		Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(vecDeleteDicts, GetResourceManager());
		//批量删除数据库
		if (!l_pDictionary->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete dictionary from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(vecAddDicts, GetResourceManager());
		//批量更新数据库
		if (!l_pDictionary->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add dictionary from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(vecUpdateDicts, GetResourceManager());
		//批量更新数据库
		if (!l_pDictionary->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "add dictionary from db failed");
			return false;
		}
	}
		

	return true;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CDictInfoChangeRequest l_oDictInfoChangeRequest;
	////协议解析
	//if (!l_oDictInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive dict change message, size[%s]", l_oDictInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oDictInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no dict info changed");
	//	return true;
	//}
	//

	//std::vector<Data::CDictInfo> l_vecDict;
	//if (l_oDictInfoChangeRequest.m_Header.m_strAction == ACTION_DELETE)
	//{
	//	for (auto l_dict : l_oDictInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDictInfo l_dictInfo;
	//		l_dictInfo.m_strGuid = l_dict.m_strGuid;
	//		l_vecDict.push_back(l_dictInfo);
	//	}

	//	Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());
	//	//批量删除数据库
	//	if (!l_pDictionary->SingleDelete())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete dictionary from db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "delete dictionary info, size[%d]", l_vecDict.size());	
	//	return true;
	//}
	//else if (l_oDictInfoChangeRequest.m_Header.m_strAction == ACTION_ADD)
	//{
	//	for (auto l_dict : l_oDictInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDictInfo l_dictInfo;
	//		l_dictInfo.m_strGuid = l_dict.m_strGuid;
	//		l_dictInfo.m_strParentKey = l_dict.m_strParentGuid;
	//		l_dictInfo.m_strKey = l_dict.m_strDictInfoKey;
	//		l_dictInfo.m_strSort = l_dict.m_strSort;

	//		for (auto var : l_dict.m_vecDictValue)
	//		{
	//			Data::CValue l_value;
	//			l_value.m_strGuid = var.m_strGuid;
	//			l_value.m_strLangGuid = var.m_strLanguageGuid;
	//			l_value.m_strValue = var.m_strValue;
	//			l_dictInfo.m_vecValue.push_back(l_value);
	//		}

	//		l_vecDict.push_back(l_dictInfo);
	//	}

	//	Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());
	//	//批量更新数据库
	//	if (!l_pDictionary->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add dictionary from db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "add dictionary info success, size[%d]", l_vecDict.size());
	//	return true;
	//}
	//else if (l_oDictInfoChangeRequest.m_Header.m_strAction == ACTION_UPDATE)
	//{
	//	for (auto l_dict : l_oDictInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CDictInfo l_dictInfo;
	//		l_dictInfo.m_strGuid = l_dict.m_strGuid;
	//		l_dictInfo.m_strParentKey = l_dict.m_strParentGuid;
	//		l_dictInfo.m_strKey = l_dict.m_strDictInfoKey;
	//		l_dictInfo.m_strSort = l_dict.m_strSort;

	//		for (auto var : l_dict.m_vecDictValue)
	//		{
	//			Data::CValue l_value;
	//			l_value.m_strGuid = var.m_strGuid;
	//			l_value.m_strLangGuid = var.m_strLanguageGuid;
	//			l_value.m_strValue = var.m_strValue;
	//			l_dictInfo.m_vecValue.push_back(l_value);
	//		}

	//		l_vecDict.push_back(l_dictInfo);
	//	}

	//	Data::IBaseDataPtr l_pDictionary = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());
	//	//批量更新数据库
	//	if (!l_pDictionary->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update dictionary from db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "update dictionary info success, size[%d]", l_vecDict.size());

	//	return true;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "unknown dictionary action, action[%s]", l_oDictInfoChangeRequest.m_Header.m_strAction.c_str());
	//}

	//return true;
}

void CBusinessImpl::OnSmpDictNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive dict info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive dict change from smp, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_DICT, 0, 0, 0, strTmp);

	_SyncDictUpdateToClient();

	ICC_LOG_DEBUG(m_pLog, "sync dict change to client complete, [%s]", strTmp.c_str());

	/*if (!_ProcReceiveDictInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process dict change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process dict change success!");
	}*/
}

//发送字典信息通知
void CBusinessImpl::SendDictRequest(const std::vector<PROTOCOL::CGetDictInfoRequest::CBody::CData>& p_DictInfo)
{
	for (auto l_dict :  p_DictInfo)
	{
		//SMP只同步有效的字典数据到基础数据服务
		if (l_dict.m_strStatus != "0")
		{
			PROTOCOL::CSetDictRequest l_CSetDictRequest;
			PROTOCOL::CHeader l_pRequestHeaderInit;
			l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
			l_pRequestHeaderInit.m_strResponseType = "0";

			GeneralHeader("set_dict_request", l_pRequestHeaderInit, l_CSetDictRequest.m_oHeader);
			for (unsigned int i = 0; i < l_dict.m_vecDictValue.size(); i++)
			{
				PROTOCOL::CSetDictRequest::CBody::CValue l_CValue;
				l_CValue.m_strGuid = l_dict.m_vecDictValue.at(i).m_strGuid;
				l_CValue.m_strValue = l_dict.m_vecDictValue.at(i).m_strValue;
				l_CValue.m_strLangGuid = l_dict.m_vecDictValue.at(i).m_strLanguageGuid;
				l_CSetDictRequest.m_oBody.m_vecValue.push_back(l_CValue);
			}
			l_CSetDictRequest.m_oBody.m_strGuid = l_dict.m_strGuid;
			l_CSetDictRequest.m_oBody.m_strParentGuid = l_dict.m_strParentGuid;
			l_CSetDictRequest.m_oBody.m_strCode = l_dict.m_strDictInfoKey;
			l_CSetDictRequest.m_oBody.m_strSort = l_dict.m_strSort;
			l_CSetDictRequest.m_oBody.m_strShortCut = "";

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_CSetDictRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}

void CBusinessImpl::SendDictChangeRequest(const std::vector<PROTOCOL::CDictInfoChangeRequest::CBody::CData>& p_DictInfo, const std::string& p_strSyncType)
{
	for (auto l_dict :  p_DictInfo)
	{
		PROTOCOL::CSetDictRequest l_CSetDictRequest;
		PROTOCOL::CHeader l_pRequestHeaderInit;
		l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
		l_pRequestHeaderInit.m_strResponseType = "0";

		GeneralHeader("set_dict_request", l_pRequestHeaderInit, l_CSetDictRequest.m_oHeader);
		l_CSetDictRequest.m_oHeader.m_strRelatedID = "Dict_Notify";

		for (auto l_value :  l_dict.m_vecDictValue)
		{
			PROTOCOL::CSetDictRequest::CBody::CValue l_CValue;
			l_CValue.m_strGuid = l_value.m_strGuid;
			l_CValue.m_strValue = l_value.m_strValue;
			l_CValue.m_strLangGuid = l_value.m_strLanguageGuid;
			l_CSetDictRequest.m_oBody.m_vecValue.push_back(l_CValue);
		}

		l_CSetDictRequest.m_oBody.m_strGuid = l_dict.m_strGuid;
		l_CSetDictRequest.m_oBody.m_strParentGuid = l_dict.m_strParentGuid;
		l_CSetDictRequest.m_oBody.m_strCode = l_dict.m_strDictInfoKey;
		l_CSetDictRequest.m_oBody.m_strSort = l_dict.m_strSort;
		l_CSetDictRequest.m_oBody.m_strShortCut = "";
		l_CSetDictRequest.m_oBody.m_strSyncType = p_strSyncType;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_CSetDictRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::SendDictDelRequest(const std::vector<PROTOCOL::CDictInfoChangeRequest::CBody::CData>& p_DictInfo)
{
	for (auto l_dict :  p_DictInfo)
	{
		PROTOCOL::CDeleteDictRequest l_oDeleteDictRequest;
		PROTOCOL::CHeader l_pRequestHeaderInit;
		l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
		l_pRequestHeaderInit.m_strResponseType = "0";
		GeneralHeader("delete_dict_request", l_pRequestHeaderInit, l_oDeleteDictRequest.m_oHeader);
		l_oDeleteDictRequest.m_oBody.m_strGuid = l_dict.m_strGuid;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oDeleteDictRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

int CBusinessImpl::_ProcReceiveUserInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataUser userDatas;
	if (!userDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive user from smp, islast:package:packagesize:count[%s:%s:%s:%d]", bIsLast ? "true" : "false", userDatas.m_strCurrentPackageCount.c_str(), userDatas.m_strPackageSize.c_str(), userDatas.m_vecUsers.size());
	
	if (userDatas.m_vecUsers.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user info received from smp,return");
		return SYNDATA_SUCCESS;
	}	

	std::vector<Data::CUserInfo> l_vecUserInfo;
	int l_iValidSize = 0;
	for (auto l_user : userDatas.m_vecUsers)
	{
		if (l_user.m_strEnableFlag == "1")
		{
			Data::CUserInfo l_userInfo;
			
			_TransSmpUserToIcc(l_user, l_userInfo);

			l_vecUserInfo.push_back(l_userInfo);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid user data!! size[%d]", l_vecUserInfo.size());

	boost::shared_ptr<Data::CUser> pProcesser = boost::dynamic_pointer_cast<Data::CUser>(m_pUser);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "user object is null!!!");
		return SYNDATA_FAILED;
	}
	
	pProcesser->Append(l_vecUserInfo);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append user success!! size[%d], total size[%d]", l_vecUserInfo.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (bIsLast)
	{
		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update user to db failed!");
			return SYNDATA_FAILED;
		}		

		ICC_LOG_DEBUG(m_pLog, "update user info success, valid[%d], update size[%d]", l_vecUserInfo.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "user info is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendUserRequest(l_CGetUserInfoRequest.m_Body.m_vecData);

	return iRs;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetUserInfoRequest l_CGetUserInfoRequest;
	//if (!l_CGetUserInfoRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive user from smp, count:value[%s : %d]", l_CGetUserInfoRequest.m_Body.m_strCount.c_str(), l_CGetUserInfoRequest.m_Body.m_vecData.size());

	//if (l_CGetUserInfoRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no user info received from smp,return");
	//	return SYNDATA_SUCCESS;
	//}

	//int iTotalCount = 0;
	//if (!l_CGetUserInfoRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetUserInfoRequest.m_Body.m_strCount);
	//}		

	//std::vector<Data::CUserInfo> l_vecUserInfo;
	//int l_iValidSize = 0;
	//for (auto l_user : l_CGetUserInfoRequest.m_Body.m_vecData)
	//{
	//	if (l_user.m_IsValid == "Y" || l_user.m_IsValid == "y")
	//	{
	//		l_iValidSize++;

	//		Data::CUserInfo l_userInfo;
	//		l_userInfo.m_strGuid = l_user.m_strGuid;
	//		l_userInfo.m_strCode = l_user.m_strUserCode;
	//		l_userInfo.m_strName = l_user.m_strName;
	//		l_userInfo.m_strPwd = l_user.m_strPassword;
	//		l_userInfo.m_strRemark = l_user.m_strRemark;
	//		l_userInfo.m_strIsDelete = "false";

	//		l_vecUserInfo.push_back(l_userInfo);
	//	}		
	//}

	//ICC_LOG_DEBUG(m_pLog, "valid user data!! size[%d]", l_vecUserInfo.size());

	//boost::shared_ptr<Data::CUser> pProcesser = boost::dynamic_pointer_cast<Data::CUser>(m_pUser);
	//if (!pProcesser)
	//{
	//	ICC_LOG_DEBUG(m_pLog, "user object is null!!!");
	//	return SYNDATA_FAILED;
	//}

	//pProcesser->AppendReceiveDataSize(l_CGetUserInfoRequest.m_Body.m_vecData.size());
	//pProcesser->Append(l_vecUserInfo);
	//int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//ICC_LOG_DEBUG(m_pLog, "append user success!! size[%d], total size[%d]", l_vecUserInfo.size(), iTotalDataSize);

	//int iRs = SYNDATA_FAILED;
	//if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//{
	//	//批量更新
	//	if (!pProcesser->SynUpdate())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update user to db failed!");
	//		return SYNDATA_FAILED;
	//	}

	//	m_dataVersion.m_strUserVersion = l_CGetUserInfoRequest.m_Body.m_strVersionData;

	//	ICC_LOG_DEBUG(m_pLog, "update user info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecUserInfo.size(), iTotalDataSize);

	//	iRs = SYNDATA_SUCCESS;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "user info is not enough, wait next data!!");
	//	iRs = SYNDATA_WAITNEXTDATA;
	//}

	////发送通知到base服务
	////SendUserRequest(l_CGetUserInfoRequest.m_Body.m_vecData);

	//return iRs;	
}

void CBusinessImpl::OnGetSmpUserInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive user info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive user from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveUserInfo(strTmp);

	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn user data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_USER_BIND_STAFF);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn user data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn user data failed!");
	}
}

void CBusinessImpl::SendUserRequest(const std::vector<PROTOCOL::CGetUserInfoRequest::CBody::CData>& p_UserInfo)
{
	for (auto l_user :  p_UserInfo)
	{
		PROTOCOL::CPushUserRequest l_CPushUserRequest;
		PROTOCOL::CHeader l_pRequestHeaderInit;
		l_pRequestHeaderInit.m_strResponse = "queue_auth";
		l_pRequestHeaderInit.m_strResponseType = "0";

		GeneralHeader("set_user_request", l_pRequestHeaderInit, l_CPushUserRequest.m_oHeader);
		l_CPushUserRequest.m_oBody.m_strGuid = l_user.m_strGuid;
		l_CPushUserRequest.m_oBody.m_strCode = l_user.m_strUserCode;
		l_CPushUserRequest.m_oBody.m_strName = l_user.m_strName;
		l_CPushUserRequest.m_oBody.m_strPwd = l_user.m_strPassword;
		l_CPushUserRequest.m_oBody.m_strRemark = l_user.m_strRemark;

		std::string l_strMsg = l_CPushUserRequest.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

bool CBusinessImpl::_ProcReceiveUserInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse user notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CUserInfo> vecAddDatas;
	std::vector<Data::CUserInfo> vecUpdateDatas;
	std::vector<Data::CUserInfo> vecDeleteDatas;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CUserInfo dataInfo;
			dataInfo.m_strGuid = data.strGuid;
			vecDeleteDatas.push_back(dataInfo);
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "dict notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataUserNotify dataNotify;
			if (!dataNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dataNotify.m_dataUser.m_strEnableFlag == "1")
			{
				Data::CUserInfo dataInfo;

				_TransSmpUserToIcc(dataNotify.m_dataUser, dataInfo);
				
				std::string l_strVal;
				if ((data.strAction == ACTION_UPDATE) && (m_pRedisClient->HGet(USER_INFO, dataInfo.m_strGuid, l_strVal)))
				{
					ICC_LOG_ERROR(m_pLog, "update guid:[%s]", dataInfo.m_strGuid.c_str());
					vecUpdateDatas.push_back(dataInfo);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "can not find guid:[%s]", dataInfo.m_strGuid.c_str());
					vecAddDatas.push_back(dataInfo);							
				}
			}			
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown staff action, action[%s]", data.strAction.c_str());
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CUser>(vecDeleteDatas, GetResourceManager());
		//批量删除数据库
		if (!pBaseData->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete user from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CUser>(vecAddDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add user from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CUser>(vecUpdateDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "update user from db failed");
			return false;
		}
	}


	return true;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CUserInfoChangeRequest l_oUserInfoChangeRequest;
	//if (!l_oUserInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "parse user info failed!! content[%s]", strMessage.c_str());
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive user change message, size[%d]", l_oUserInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oUserInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no user info changed");
	//	return true;
	//}

	//std::vector<Data::CUserInfo> l_vecUser;
	//std::string l_strAction = l_oUserInfoChangeRequest.m_Header.m_strAction;
	//if (l_strAction == ACTION_DELETE)
	//{
	//	std::vector<Data::CBindInfo> vecUserBindStaffs;
	//	std::vector<Data::CBindInfo> vecUserBindRoles;

	//	for (auto l_user : l_oUserInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CUserInfo l_userInfo;
	//		l_userInfo.m_strGuid = l_user.m_strGuid;
	//		//l_userInfo.m_strIsDelete = "false";
	//		l_vecUser.push_back(l_userInfo);

	//		Data::CBindInfo userBindStaff;
	//		userBindStaff.m_strFromGuid = l_user.m_strGuid;
	//		userBindStaff.m_strType = BIND_TYPE_USER_STAFF;
	//		vecUserBindStaffs.push_back(userBindStaff);

	//		Data::CBindInfo userBindRole;
	//		userBindRole.m_strFromGuid = l_user.m_strGuid;
	//		userBindRole.m_strType = BIND_TYPE_USER_ROLE;
	//		vecUserBindRoles.push_back(userBindRole);
	//	}

	//	Data::IBaseDataPtr l_pUser = boost::make_shared<Data::CUser>(l_vecUser, GetResourceManager());
	//	//批量更新
	//	if (!l_pUser->SingleDelete())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete user to db failed");
	//		return false;
	//	}

	//	if (!vecUserBindStaffs.empty())
	//	{
	//		Data::IBaseDataPtr pUserBindStaff = boost::make_shared<Data::CBind>(vecUserBindStaffs, GetResourceManager(), BIND_TYPE_USER_STAFF);
	//		//批量更新
	//		if (!pUserBindStaff->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete user bind staff to db failed");
	//			return false;
	//		}
	//	}

	//	if (!vecUserBindRoles.empty())
	//	{
	//		Data::IBaseDataPtr pUserBindRole = boost::make_shared<Data::CBind>(vecUserBindRoles, GetResourceManager(), BIND_TYPE_USER_ROLE);
	//		//批量更新
	//		if (!pUserBindRole->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete user bind role to db failed");
	//			return false;
	//		}
	//	}		

	//	ICC_LOG_DEBUG(m_pLog, "delete user info success, size[%d]", l_vecUser.size());
	//	return true;
	//}
	//else if (l_strAction == ACTION_ADD)
	//{
	//	for (auto l_user : l_oUserInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		if (l_user.m_IsValid == "Y" || l_user.m_IsValid == "y")
	//		{
	//			Data::CUserInfo l_userInfo;
	//			l_userInfo.m_strGuid = l_user.m_strGuid;
	//			l_userInfo.m_strCode = l_user.m_strUserCode;
	//			l_userInfo.m_strName = l_user.m_strName;
	//			l_userInfo.m_strPwd = l_user.m_strPassword;
	//			l_userInfo.m_strIsDelete = l_user.m_IsValid = "true";
	//			l_userInfo.m_strRemark = l_user.m_strRemark;
	//			l_userInfo.m_strCreateUser = "AuthBusiness";
	//			l_userInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	//			l_vecUser.push_back(l_userInfo);
	//		}
	//		else
	//		{
	//			ICC_LOG_DEBUG(m_pLog, "invalid add user info , guid[%s]", l_user.m_strGuid.c_str());
	//		}
	//	}

	//	Data::IBaseDataPtr l_pUser = boost::make_shared<Data::CUser>(l_vecUser, GetResourceManager());
	//	//批量更新
	//	if (!l_pUser->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add user to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "add user info success, size[%d]", l_vecUser.size());
	//	return true;		
	//}
	//else if (l_strAction == ACTION_UPDATE)
	//{
	//	for (auto l_user : l_oUserInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		if (l_user.m_IsValid == "Y" || l_user.m_IsValid == "y")
	//		{
	//			Data::CUserInfo l_userInfo;
	//			l_userInfo.m_strGuid = l_user.m_strGuid;
	//			l_userInfo.m_strCode = l_user.m_strUserCode;
	//			l_userInfo.m_strName = l_user.m_strName;
	//			l_userInfo.m_strPwd = l_user.m_strPassword;
	//			l_userInfo.m_strIsDelete = l_user.m_IsValid = "true";
	//			l_userInfo.m_strRemark = l_user.m_strRemark;

	//			l_userInfo.m_strUpdateUser = "AuthBusiness";
	//			l_userInfo.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

	//			l_vecUser.push_back(l_userInfo);
	//		}
	//		else
	//		{
	//			ICC_LOG_DEBUG(m_pLog, "invalid update user info , guid[%s]", l_user.m_strGuid.c_str());
	//		}
	//	}

	//	Data::IBaseDataPtr l_pUser = boost::make_shared<Data::CUser>(l_vecUser, GetResourceManager());
	//	//批量更新
	//	if (!l_pUser->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update user to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "update user info success, size[%d]", l_vecUser.size());
	//	return true;
	//}
	//else
	//{
	//	std::string l_strType;
	//	std::vector<Data::CBindInfo> l_vecBindData;
	//	if (m_pString->StartsWith(l_strAction, ACTION_BIND))//绑定操作
	//	{
	//		if (l_strAction == ACTION_BIND_STAFF)//绑人
	//			l_strType = BIND_TYPE_USER_STAFF;
	//		else if (l_strAction == ACTION_BIND_ROLE)//绑角色
	//			l_strType = BIND_TYPE_USER_ROLE;
	//		else if (l_strAction == ACTION_BIND_ORG)//绑单位
	//			l_strType = BIND_TYPE_USER_DEPT;
	//		else
	//		{
	//			ICC_LOG_WARNING(m_pLog, "unknown bind type [%s]", l_strAction.c_str());
	//			return true;
	//		}

	//		std::vector<Data::CBindInfo> l_vecBindStaff;
	//		std::vector<Data::CBindInfo> l_vecBindRole;
	//		std::vector<Data::CBindInfo> l_vecBindDept;

	//		for (auto l_bind : l_oUserInfoChangeRequest.m_Body.m_vecData)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strType = l_strType;
	//			l_bindInfo.m_strFromGuid = l_bind.m_strUserGuid;
	//			if (l_strAction == ACTION_BIND_STAFF) //绑人
	//			{
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strStaffGuid;//绑定关系的guid
	//				l_bindInfo.m_strToGuid = l_bind.m_strStaffGuid;
	//				l_vecBindStaff.push_back(l_bindInfo);
	//			}
	//			else if (l_strAction == ACTION_BIND_ROLE)//绑角色
	//			{
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strRoleGuid;//绑定关系的guid
	//				l_bindInfo.m_strToGuid = l_bind.m_strRoleGuid;
	//				l_vecBindRole.push_back(l_bindInfo);
	//			}
	//			else if (l_strAction == ACTION_BIND_ORG)//绑单位
	//			{
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strOrgGuid;//绑定关系的guid
	//				l_bindInfo.m_strToGuid = l_bind.m_strOrgGuid;
	//				l_vecBindDept.push_back(l_bindInfo);

	//				ICC_LOG_WARNING(m_pLog, "user not bind dept. guid[%s:%s]", l_bind.m_strUserGuid.c_str(), l_bind.m_strOrgGuid.c_str());
	//			}				
	//		}

	//		if (!l_vecBindStaff.empty())
	//		{
	//			Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(l_vecBindStaff, GetResourceManager(), BIND_TYPE_USER_STAFF);
	//			//批量更新
	//			if (!l_pBind->SingleAdd())
	//			{
	//				ICC_LOG_ERROR(m_pLog, "add user bind staff to db failed");
	//				return false;
	//			}

	//			ICC_LOG_DEBUG(m_pLog, "add user bind staff success. size[%d]", l_vecBindStaff.size());
	//		}

	//		if (!l_vecBindRole.empty())
	//		{
	//			Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(l_vecBindRole, GetResourceManager(), BIND_TYPE_USER_ROLE);
	//			//批量更新
	//			if (!l_pBind->SingleAdd())
	//			{
	//				ICC_LOG_ERROR(m_pLog, "add user bind role to db failed");
	//				return false;
	//			}

	//			ICC_LOG_DEBUG(m_pLog, "add user bind role success. size[%d]", l_vecBindRole.size());
	//		}			

	//		return true;
	//	}
	//	else if (m_pString->StartsWith(l_strAction, ACTION_UNBIND))//解绑操作
	//	{
	//		if (l_strAction == ACTION_UNBIND_STAFF) //解绑人
	//			l_strType = BIND_TYPE_USER_STAFF;
	//		else if (l_strAction == ACTION_UNBIND_ROLE)//解绑角色
	//			l_strType = BIND_TYPE_USER_ROLE;
	//		else if (l_strAction == ACTION_UNBIND_ORG)//解绑单位
	//			l_strType = BIND_TYPE_USER_DEPT;
	//		else
	//		{
	//			ICC_LOG_WARNING(m_pLog, "unknown unbind type [%s]", l_strAction.c_str());
	//			return true;
	//		}

	//		std::vector<Data::CBindInfo> l_vecBindStaff;
	//		std::vector<Data::CBindInfo> l_vecBindRole;
	//		std::vector<Data::CBindInfo> l_vecBindDept;
	//		for (auto l_bind : l_oUserInfoChangeRequest.m_Body.m_vecData)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strType = l_strType;
	//			l_bindInfo.m_strFromGuid = l_bind.m_strUserGuid;

	//			if (l_strAction == ACTION_UNBIND_STAFF) //解绑人
	//			{
	//				//SMP 发过来的解绑协议 ToGuid 字段为空的
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strStaffGuid;//绑定关系的guid

	//				l_bindInfo.m_strToGuid = l_bind.m_strStaffGuid;
	//				l_vecBindStaff.push_back(l_bindInfo);
	//			}
	//			else if (l_strAction == ACTION_UNBIND_ROLE)//解绑角色
	//			{
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strRoleGuid;//绑定关系的guid

	//				l_bindInfo.m_strToGuid = l_bind.m_strRoleGuid;
	//				l_vecBindRole.push_back(l_bindInfo);
	//			}
	//			else if (l_strAction == ACTION_UNBIND_ORG)//解绑单位
	//			{
	//				l_bindInfo.m_strGuid = l_bind.m_strUserGuid + std::string("_") + l_bind.m_strOrgGuid;//绑定关系的guid

	//				l_bindInfo.m_strToGuid = l_bind.m_strOrgGuid;
	//				l_vecBindDept.push_back(l_bindInfo);

	//				ICC_LOG_WARNING(m_pLog, "user not bind dept. guid[%s:%s]", l_bind.m_strUserGuid.c_str(), l_bind.m_strOrgGuid.c_str());
	//			}				
	//		}

	//		if (!l_vecBindStaff.empty())
	//		{
	//			Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(l_vecBindStaff, GetResourceManager(), BIND_TYPE_USER_STAFF);
	//			//批量更新
	//			if (!l_pBind->SingleDelete())
	//			{
	//				ICC_LOG_ERROR(m_pLog, "delete bindinfo to db failed");
	//				return false;
	//			}

	//			ICC_LOG_DEBUG(m_pLog, "delete user bind staff success! size[%d]", l_vecBindStaff.size());
	//		}

	//		if (!l_vecBindRole.empty())
	//		{
	//			Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(l_vecBindRole, GetResourceManager(), BIND_TYPE_USER_ROLE);
	//			//批量更新
	//			if (!l_pBind->SingleDelete())
	//			{
	//				ICC_LOG_ERROR(m_pLog, "delete bindinfo to db failed");
	//				return false;
	//			}

	//			ICC_LOG_DEBUG(m_pLog, "delete user bind role success! size[%d]", l_vecBindRole.size());
	//		}
	//		
	//		return true;
	//	}
	//	else
	//	{
	//		ICC_LOG_WARNING(m_pLog, "unknown action [%s]", l_strAction.c_str());
	//		return true;
	//	}		
	//}

	return true;
}

void CBusinessImpl::OnSmpUserNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive user info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive user change from smp, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_USER, 0, 0, 0, strTmp);

	/*if (!_ProcReceiveUserInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process user change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process user change success!");
	}*/
}

void CBusinessImpl::UserDeleteNotify(const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData)
{
	PROTOCOL::CAuthDeleteRequest l_oDeleteUserRequest;
	l_oDeleteUserRequest.m_oHeader.m_strCmd = "delete_user_request";
	l_oDeleteUserRequest.m_oHeader.m_strRequest = "queue_auth";
	l_oDeleteUserRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
	l_oDeleteUserRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

	for (auto it :  p_rvecData)
	{
		l_oDeleteUserRequest.m_oBody.m_strGuid = it.m_strGuid;
		l_oDeleteUserRequest.m_oBody.m_strCode = it.m_strUserCode;
		l_oDeleteUserRequest.m_oBody.m_strName = it.m_strName;
		l_oDeleteUserRequest.m_oBody.m_strPwd = it.m_strPassword;
		l_oDeleteUserRequest.m_oBody.m_strRemark = it.m_strRemark;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oDeleteUserRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::UserChangeNotify(const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData, const std::string& p_strSyncType)
{
	PROTOCOL::CPushUserRequest l_CPushUserRequest;
	l_CPushUserRequest.m_oHeader.m_strCmd = "set_user_request";//"set_bind_request";
	l_CPushUserRequest.m_oHeader.m_strRequest = "queue_auth";
	l_CPushUserRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
	l_CPushUserRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

	for (auto it = p_rvecData.begin(); it != p_rvecData.end(); ++it)
	{
		l_CPushUserRequest.m_oBody.m_strGuid = it->m_strGuid;
		l_CPushUserRequest.m_oBody.m_strCode = it->m_strUserCode;
		l_CPushUserRequest.m_oBody.m_strName = it->m_strName;
		l_CPushUserRequest.m_oBody.m_strPwd = it->m_strPassword;
		l_CPushUserRequest.m_oBody.m_strRemark = it->m_strRemark;
		l_CPushUserRequest.m_oBody.m_strSyncType = p_strSyncType;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_CPushUserRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::UserBindChangeNotify(std::string p_strBindType, const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData)
{
	PROTOCOL::CSetBindRequest l_oSetBindRequest;
	l_oSetBindRequest.m_oHeader.m_strCmd = "set_bind_request";
	l_oSetBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
	l_oSetBindRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
	l_oSetBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	//l_oSetBindRequest.m_oBody.m_strGuid = m_pString->CreateGuid();
	l_oSetBindRequest.m_oBody.m_strType = p_strBindType;

	for (auto it = p_rvecData.begin(); it != p_rvecData.end(); ++it)
	{
		if (p_strBindType == BIND_TYPE_USER_STAFF)
		{
			l_oSetBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oSetBindRequest.m_oBody.m_strToGuid = it->m_strStaffGuid;
			l_oSetBindRequest.m_oBody.m_strGuid = it->m_strUserGuid + std::string("_") + it->m_strStaffGuid;
		}
		else if (p_strBindType == BIND_TYPE_USER_ROLE)
		{
			l_oSetBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oSetBindRequest.m_oBody.m_strToGuid = it->m_strRoleGuid;
			l_oSetBindRequest.m_oBody.m_strGuid = it->m_strUserGuid + std::string("_") + it->m_strRoleGuid;
		}
		else if (p_strBindType == BIND_TYPE_USER_DEPT)
		{
			l_oSetBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oSetBindRequest.m_oBody.m_strToGuid = it->m_strOrgGuid;
			l_oSetBindRequest.m_oBody.m_strGuid = it->m_strUserGuid + std::string("_") + it->m_strOrgGuid;
		}

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetBindRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void CBusinessImpl::UserUnBindChangeNotify(std::string p_strBindType, const std::vector<Data::CBindInfo>& p_rvecData)
{
	PROTOCOL::CDeleteBindRequest l_oDeleteBindRequest;
	l_oDeleteBindRequest.m_oHeader.m_strCmd = "delete_bind_request";
	l_oDeleteBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
	l_oDeleteBindRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
	l_oDeleteBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

	l_oDeleteBindRequest.m_oBody.m_strType = p_strBindType;

	for (auto it = p_rvecData.begin(); it != p_rvecData.end(); ++it)
	{
		/*if (p_strBindType == BIND_TYPE_USER_STAFF)
		{
			l_oDeleteBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oDeleteBindRequest.m_oBody.m_strToGuid = it->m_strStaffGuid;
		}
		else if (p_strBindType == BIND_TYPE_USER_ROLE)
		{
			l_oDeleteBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oDeleteBindRequest.m_oBody.m_strToGuid = it->m_strRoleGuid;
		}
		else if (p_strBindType == BIND_TYPE_USER_DEPT)
		{
			l_oDeleteBindRequest.m_oBody.m_strFromGuid = it->m_strUserGuid;
			l_oDeleteBindRequest.m_oBody.m_strToGuid = it->m_strOrgGuid;
		}*/
		l_oDeleteBindRequest.m_oBody.m_strGuid = it->m_strGuid;
		l_oDeleteBindRequest.m_oBody.m_strFromGuid = it->m_strFromGuid;
		l_oDeleteBindRequest.m_oBody.m_strToGuid = it->m_strToGuid;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oDeleteBindRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

//功能权限同步响应消息
void CBusinessImpl::OnGetAuthInfor(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive func info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive func from smp, [%s]", strTmp.c_str());		

	int iRs = _ProcReceiveFuncInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn func data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_ORGN);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn func data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn func data failed!");
	}
}

int CBusinessImpl::_ProcReceiveFuncInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataFunc funcDatas;
	if (!funcDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	if (funcDatas.m_vecFuncs.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func info received from smp,return");
		return SYNDATA_SUCCESS;
	}

	ICC_LOG_DEBUG(m_pLog, "receive func from smp, islast:package:packagesize:count[%s:%s:%s:%d]", bIsLast ? "true" : "false", funcDatas.m_strCurrentPackageCount.c_str(), funcDatas.m_strPackageSize.c_str(), funcDatas.m_vecFuncs.size());
		

	int l_iValid = 0;
	std::vector<Data::CAuthInfo> l_vecDatas;
	for (auto func : funcDatas.m_vecFuncs)
	{
		if (func.m_strEnableFlag == "1" && func.m_strSystemCode == m_strSystemCode)
		{			
			Data::CAuthInfo l_authInfo;
			l_authInfo.m_strGuid = func.m_strGuid;
			l_authInfo.m_strCode = func.m_strFunctionCode;
			l_authInfo.m_strName = func.m_strFunctionName;
			l_authInfo.m_strParentGuid = func.m_strFunctionParentGuid;
			l_authInfo.m_strSort = func.m_strSort;
			l_vecDatas.push_back(l_authInfo);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid func data!! size[%d]", l_vecDatas.size());

	boost::shared_ptr<Data::CAuth> pProcesser = boost::dynamic_pointer_cast<Data::CAuth>(m_pFunc);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "func object is null!!!");
		return SYNDATA_FAILED;
	}

	pProcesser->Append(l_vecDatas);
	
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append func success!! size[%d], total size[%d]", l_vecDatas.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (bIsLast)
	{
		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update func to db failed!");
			return SYNDATA_FAILED;
		}		

		ICC_LOG_DEBUG(m_pLog, "update func info success, valid[%d], update size[%d]", l_vecDatas.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "func is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendAuthRequest(l_CGetAuthInfoRequest.m_Body.m_vecData);	

	return iRs;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetAuthInfoRequest l_CGetAuthInfoRequest;
	//if (!l_CGetAuthInfoRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//if (l_CGetAuthInfoRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no func info received from smp,return");
	//	return SYNDATA_SUCCESS;
	//}
	//
	//ICC_LOG_DEBUG(m_pLog, "receive language from smp, count:value[%s : %d]", l_CGetAuthInfoRequest.m_Body.m_strCount.c_str(), l_CGetAuthInfoRequest.m_Body.m_vecData.size());

	//int iTotalCount = 0;
	//if (!l_CGetAuthInfoRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetAuthInfoRequest.m_Body.m_strCount);
	//}
	//
	//int l_iValid = 0;
	//std::vector<Data::CAuthInfo> l_vecDatas;
	//for (auto func : l_CGetAuthInfoRequest.m_Body.m_vecData)
	//{
	//	if (func.m_strIsAvailable == "Y")
	//	{
	//		++l_iValid;
	//		Data::CAuthInfo l_authInfo;
	//		l_authInfo.m_strGuid = func.m_strGuid;
	//		l_authInfo.m_strCode = func.m_strCode;
	//		l_authInfo.m_strName = func.m_strName;
	//		l_authInfo.m_strParentGuid = func.m_strParentCode;
	//		l_authInfo.m_strSort = func.m_strSort;
	//		l_vecDatas.push_back(l_authInfo);
	//	}
	//}

	//ICC_LOG_DEBUG(m_pLog, "valid func data!! size[%d]", l_vecDatas.size());

	//boost::shared_ptr<Data::CAuth> pProcesser = boost::dynamic_pointer_cast<Data::CAuth>(m_pFunc);
	//if (!pProcesser)
	//{
	//	ICC_LOG_DEBUG(m_pLog, "func object is null!!!");
	//	return SYNDATA_FAILED;
	//}

	//pProcesser->Append(l_vecDatas);
	//pProcesser->AppendReceiveDataSize(l_CGetAuthInfoRequest.m_Body.m_vecData.size());
	//int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//ICC_LOG_DEBUG(m_pLog, "append func success!! size[%d], total size[%d]", l_vecDatas.size(), iTotalDataSize);

	//int iRs = SYNDATA_FAILED;
	//if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//{
	//	//批量更新
	//	if (!pProcesser->SynUpdate())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update func to db failed!");
	//		return SYNDATA_FAILED;
	//	}	

	//	m_dataVersion.m_strAuthVersion = l_CGetAuthInfoRequest.m_Body.m_strVersionData;
	//	
	//	ICC_LOG_DEBUG(m_pLog, "update func info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecDatas.size(), iTotalDataSize);

	//	iRs = SYNDATA_SUCCESS;
	//}
	//else
	//{
	//	ICC_LOG_DEBUG(m_pLog, "func is not enough, wait next data!!");
	//	iRs = SYNDATA_WAITNEXTDATA;
	//}	

	////发送通知到base服务
	////SendAuthRequest(l_CGetAuthInfoRequest.m_Body.m_vecData);	

	//return iRs;
}

void CBusinessImpl::SendAuthRequest(const std::vector<PROTOCOL::CGetAuthInfoRequest::CBody::CData>& p_AuthInfo)
{
	for (auto func :  p_AuthInfo)
	{	//SMP只同步有效的用户数据到基础数据服务
		if (func.m_strIsAvailable == "Y")
		{
			std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
			PROTOCOL::CHeader l_pRequestHeaderInit;
			l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
			l_pRequestHeaderInit.m_strResponseType = "0";

			PROTOCOL::CSetFuncRequest l_CSetFuncRequest;
			GeneralHeader("set_func_request", l_pRequestHeaderInit, l_CSetFuncRequest.m_oHeader);
			l_CSetFuncRequest.m_oBody.m_strGuid = func.m_strGuid;
			l_CSetFuncRequest.m_oBody.m_strCode = func.m_strCode;
			l_CSetFuncRequest.m_oBody.m_strName = func.m_strName;
			l_CSetFuncRequest.m_oBody.m_strParentGuid = func.m_strParentCode;
			l_CSetFuncRequest.m_oBody.m_strSort = func.m_strSort;
			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_CSetFuncRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}

bool CBusinessImpl::_ProcReceiveFuncInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse func notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CAuthInfo> vecAddDatas;
	std::vector<Data::CAuthInfo> vecUpdateDatas;
	std::vector<Data::CAuthInfo> vecDeleteDatas;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CAuthInfo dataInfo;
			dataInfo.m_strGuid = data.strGuid;
			vecDeleteDatas.push_back(dataInfo);
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "dict notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataFuncNofify dataNotify;
			if (!dataNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dataNotify.m_dataFunc.m_strEnableFlag == "1" && dataNotify.m_dataFunc.m_strSystemCode == m_strSystemCode)
			{
				Data::CAuthInfo dataInfo;
				dataInfo.m_strGuid = dataNotify.m_dataFunc.m_strGuid;
				dataInfo.m_strCode = dataNotify.m_dataFunc.m_strFunctionCode;
				dataInfo.m_strName = dataNotify.m_dataFunc.m_strFunctionName;
				dataInfo.m_strParentGuid = dataNotify.m_dataFunc.m_strFunctionParentGuid;
				dataInfo.m_strSort = dataNotify.m_dataFunc.m_strSort;

				if (data.strAction == ACTION_ADD)
				{
					vecAddDatas.push_back(dataInfo);
				}
				else
				{
					vecUpdateDatas.push_back(dataInfo);
				}
			}			
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown func action, action[%s]", data.strAction.c_str());
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CAuth>(vecDeleteDatas, GetResourceManager());
		//批量删除数据库
		if (!pBaseData->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete func from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CAuth>(vecAddDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add func from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CAuth>(vecUpdateDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "add func from db failed");
			return false;
		}
	}


	return true;


	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CSMPFuncInfoChangeNotify l_oFuncInfoChangeRequest;
	//if (!l_oFuncInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive func change message, size[%d]", l_oFuncInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oFuncInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no func info changed");
	//	return true;
	//}	

	//int l_iValid = 0;
	//std::string l_strAction = l_oFuncInfoChangeRequest.m_Header.m_strAction;
	//std::vector<Data::CAuthInfo> l_vecAuth;
	//if (l_strAction == ACTION_ADD)
	//{
	//	for (auto func : l_oFuncInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		if (func.m_strIsAvailabe == "Y")
	//		{
	//			++l_iValid;
	//			Data::CAuthInfo l_authInfo;
	//			l_authInfo.m_strGuid = func.m_strGuid;
	//			l_authInfo.m_strCode = func.m_strCode;
	//			l_authInfo.m_strName = func.m_strName;
	//			l_authInfo.m_strParentGuid = func.m_strParentCode;
	//			l_authInfo.m_strSort = func.m_strSort;
	//			l_vecAuth.push_back(l_authInfo);
	//		}
	//	}

	//	Data::IBaseDataPtr l_pAuthInfo = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());
	//	//批量更新
	//	if (!l_pAuthInfo->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add func to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "add func info success, size[%d], valid[%d]", l_oFuncInfoChangeRequest.m_Body.m_vecData.size(), l_vecAuth.size());
	//	return true;
	//}
	//else if (l_strAction == ACTION_UPDATE)
	//{
	//	for (auto func : l_oFuncInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		if (func.m_strIsAvailabe == "Y")
	//		{
	//			++l_iValid;
	//			Data::CAuthInfo l_authInfo;
	//			l_authInfo.m_strGuid = func.m_strGuid;
	//			l_authInfo.m_strCode = func.m_strCode;
	//			l_authInfo.m_strName = func.m_strName;
	//			l_authInfo.m_strParentGuid = func.m_strParentCode;
	//			l_authInfo.m_strSort = func.m_strSort;
	//			l_vecAuth.push_back(l_authInfo);
	//		}
	//	}

	//	Data::IBaseDataPtr l_pAuthInfo = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());
	//	//批量更新
	//	if (!l_pAuthInfo->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update func to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "update func info success, size[%d], valid[%d]", l_oFuncInfoChangeRequest.m_Body.m_vecData.size(), l_vecAuth.size());
	//	return true;
	//}
	//else if (l_strAction == ACTION_DELETE)
	//{
	//	std::vector<Data::CBindInfo> vecRoleBindFuncs;

	//	for (auto func : l_oFuncInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CAuthInfo l_authInfo;
	//		l_authInfo.m_strGuid = func.m_strGuid;
	//		l_vecAuth.push_back(l_authInfo);

	//		Data::CBindInfo roleBindFuncs;
	//		roleBindFuncs.m_strToGuid = func.m_strGuid;
	//		roleBindFuncs.m_strType = BIND_TYPE_ROLE_FUNC;
	//		vecRoleBindFuncs.push_back(roleBindFuncs);
	//	}

	//	Data::IBaseDataPtr l_pAuthInfo = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());
	//	//批量更新
	//	if (!l_pAuthInfo->SingleDelete())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete func to db failed");
	//		return false;
	//	}

	//	if (!vecRoleBindFuncs.empty())
	//	{
	//		Data::IBaseDataPtr pRoleBindFunc = boost::make_shared<Data::CBind>(vecRoleBindFuncs, GetResourceManager(), BIND_TYPE_ROLE_FUNC);
	//		//批量更新
	//		if (!pRoleBindFunc->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind func to db failed");
	//			return false;
	//		}
	//	}		

	//	ICC_LOG_DEBUG(m_pLog, "delete func info success, size[%d]", l_vecAuth.size());
	//	return true;
	//}
	//else
	//{
	//	ICC_LOG_ERROR(m_pLog, "unknown action [%s]", l_strAction.c_str());
	//}
	//return true;
}

void CBusinessImpl::OnSmpAuthNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive func info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive func change from smp, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_FUNC, 0, 0, 0, strTmp);

	/*if (!_ProcReceiveFuncInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process func change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process func change success!");
	}*/
}

void CBusinessImpl::OnSmpSeatNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive func info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive func change from smp, [%s]", strTmp.c_str());

	if (_ProcSynDataEx() == 0)
	{
		ICC_LOG_DEBUG(m_pLog, " smp syn complete ");
	}
	else {
		ICC_LOG_DEBUG(m_pLog, " smp syn failed ");
	}

}

void ICC::CBusinessImpl::FuncChangeNotify(const std::vector<PROTOCOL::CSMPFuncInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CSetFuncRequest l_oSetFuncRequest;
		l_oSetFuncRequest.m_oHeader.m_strCmd = "set_func_request";
		l_oSetFuncRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetFuncRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oSetFuncRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		l_oSetFuncRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

		l_oSetFuncRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		l_oSetFuncRequest.m_oBody.m_strParentGuid = p_rData.m_strParentCode;//ICC协议中的parent_guid实际在数据库里存放的是parent_code
		l_oSetFuncRequest.m_oBody.m_strCode = p_rData.m_strCode;
		l_oSetFuncRequest.m_oBody.m_strName = p_rData.m_strName;
		l_oSetFuncRequest.m_oBody.m_strSort = p_rData.m_strSort;
		l_oSetFuncRequest.m_oBody.m_strSyncType = p_strSyncType;

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetFuncRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

void ICC::CBusinessImpl::FuncDeleteNotify(const std::vector<PROTOCOL::CSMPFuncInfoChangeNotify::CBody::CData>& p_rvecData)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CDeleteFuncRequest l_oDeleteFuncRequest;
		l_oDeleteFuncRequest.m_oHeader.m_strCmd = "delete_func_request";
		l_oDeleteFuncRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oDeleteFuncRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oDeleteFuncRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		l_oDeleteFuncRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

		l_oDeleteFuncRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oDeleteFuncRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

int CBusinessImpl::_ProcReceiveLanguageInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetLanguageInfoRequest l_CGetLanguageInfoRequest;
	if (!l_CGetLanguageInfoRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive language from smp, count:value[%s : %d]", l_CGetLanguageInfoRequest.m_Body.m_strCount.c_str(), l_CGetLanguageInfoRequest.m_Body.m_vecData.size());

	if (l_CGetLanguageInfoRequest.m_Body.m_vecData.empty())
	{
		ICC_LOG_INFO(m_pLog, "no language info received from smp,return");
		return SYNDATA_SUCCESS;
	}

	int iTotalCount = 0;
	if (!l_CGetLanguageInfoRequest.m_Body.m_strCount.empty())
	{
		iTotalCount = std::stoi(l_CGetLanguageInfoRequest.m_Body.m_strCount);
	}



	int l_iValid = 0;
	std::vector<Data::CLanguageInfo> l_vecDatas;
	for (auto lang : l_CGetLanguageInfoRequest.m_Body.m_vecData)
	{
		if (lang.m_strStatus == "Y" || lang.m_strStatus == "y")
		{
			++l_iValid;
			Data::CLanguageInfo l_language;
			l_language.m_strGuid = lang.m_strGuid;
			l_language.m_strCode = lang.m_strLanguageCode;
			l_language.m_strName = lang.m_strDescribeName;
			l_language.m_strSort = lang.m_strSort;
			l_vecDatas.push_back(l_language);
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid language data!! size[%d]", l_vecDatas.size());

	boost::shared_ptr<Data::CLanguage> pProcesser = boost::dynamic_pointer_cast<Data::CLanguage>(m_pLanguage);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "language object is null!!!");
		return SYNDATA_FAILED;
	}

	pProcesser->AppendReceiveDataSize(l_CGetLanguageInfoRequest.m_Body.m_vecData.size());
	pProcesser->Append(l_vecDatas);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append language success!! size[%d], total size[%d]", l_vecDatas.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	{
		/*if (m_dataVersion.m_strLanguageVersion == l_CGetLanguageInfoRequest.m_Body.m_strVersionData)
		{
			ICC_LOG_ERROR(m_pLog, "language version is same, not need update");
			return true;
		}*/

		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update language to db failed!");
			return SYNDATA_FAILED;
		}		

		m_dataVersion.m_strLanguageVersion = l_CGetLanguageInfoRequest.m_Body.m_strVersionData;

		ICC_LOG_DEBUG(m_pLog, "update language info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecDatas.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "language is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendLanguageRequest(l_CGetLanguageInfoRequest.m_Body.m_vecData);

	return iRs;
}

void CBusinessImpl::OnGetDictLanguageInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive language info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive language from smp, [%s]", strTmp.c_str());
	
	int iRs = _ProcReceiveLanguageInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn language data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_DICT);
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn language data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn language data failed!");
	}
}

void CBusinessImpl::SendLanguageRequest(const std::vector<PROTOCOL::CGetLanguageInfoRequest::CBody::CData>& p_LanguageInfo)
{
	for (auto & lang :  p_LanguageInfo)
	{
		//SMP只同步有效的语言数据到基础数据服务
		if (lang.m_strStatus == "Y" || lang.m_strStatus == "y")
		{
			PROTOCOL::CHeader l_pRequestHeaderInit;
			l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
			l_pRequestHeaderInit.m_strResponseType = "0";
			PROTOCOL::CSetLanguageRequest l_CSetLanguageRequest;
			GeneralHeader("set_language_request", l_pRequestHeaderInit, l_CSetLanguageRequest.m_oHeader);

			l_CSetLanguageRequest.m_oBody.m_strGuid = lang.m_strGuid;
			l_CSetLanguageRequest.m_oBody.m_strCode = lang.m_strLanguageCode;
			l_CSetLanguageRequest.m_oBody.m_strName = lang.m_strDescribeName;
			l_CSetLanguageRequest.m_oBody.m_strSort = lang.m_strSort;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_CSetLanguageRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}
	}
}


bool CBusinessImpl::_ProcReceiveLanguageInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSMPLangInfoChangeNotify l_oLangInfoChangeRequest;
	if (!l_oLangInfoChangeRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_DEBUG(m_pLog, "parse language info failed!! constent[%s]", strMessage.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive language change from smp, count[ %d]", l_oLangInfoChangeRequest.m_Body.m_vecData.size());

	if (l_oLangInfoChangeRequest.m_Body.m_vecData.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "there is no language info changed");
		return true;
	}	

	int l_iValid = 0;
	std::vector<Data::CLanguageInfo> l_vecLanguage;
	std::string l_strAction = l_oLangInfoChangeRequest.m_Header.m_strAction;
	if (l_strAction == ACTION_ADD)
	{
		for (auto lang : l_oLangInfoChangeRequest.m_Body.m_vecData)
		{
			if (lang.m_strStatus == "Y" || lang.m_strStatus == "y")
			{
				++l_iValid;
				Data::CLanguageInfo l_language;
				l_language.m_strGuid = lang.m_strGuid;
				l_language.m_strCode = lang.m_strLangCode;
				l_language.m_strName = lang.m_strLangName;
				l_language.m_strSort = lang.m_strSort;
				l_vecLanguage.push_back(l_language);
			}
		}

		Data::IBaseDataPtr l_pLanguage = boost::make_shared<Data::CLanguage>(l_vecLanguage, GetResourceManager());
		if (l_pLanguage == nullptr)
		{
			return false;
		}

		if (!l_pLanguage->SingleAdd())
		{
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add language info success, size[%d]", l_vecLanguage.size());

		//发送通知到base服务
		std::string l_strSyncType = (l_oLangInfoChangeRequest.m_Header.m_strAction == ACTION_ADD ? SYNC_TYPE_ADD : SYNC_TYPE_UPDATE);
		LanguageChangeNotify(l_oLangInfoChangeRequest.m_Body.m_vecData, l_strSyncType);

		return true;
	}
	else if (l_strAction == ACTION_UPDATE)
	{
		for (auto lang : l_oLangInfoChangeRequest.m_Body.m_vecData)
		{
			if (lang.m_strStatus == "Y" || lang.m_strStatus == "y")
			{
				++l_iValid;
				Data::CLanguageInfo l_language;
				l_language.m_strGuid = lang.m_strGuid;
				l_language.m_strCode = lang.m_strLangCode;
				l_language.m_strName = lang.m_strLangName;
				l_language.m_strSort = lang.m_strSort;
				l_vecLanguage.push_back(l_language);
			}
		}

		Data::IBaseDataPtr l_pLanguage = boost::make_shared<Data::CLanguage>(l_vecLanguage, GetResourceManager());
		if (l_pLanguage == nullptr)
		{
			return false;
		}

		if (!l_pLanguage->SingleModify())
		{
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update language info success, size[%d]", l_vecLanguage.size());

		//发送通知到base服务
		std::string l_strSyncType = (l_oLangInfoChangeRequest.m_Header.m_strAction == ACTION_ADD ? SYNC_TYPE_ADD : SYNC_TYPE_UPDATE);
		LanguageChangeNotify(l_oLangInfoChangeRequest.m_Body.m_vecData, l_strSyncType);

		return true;
	}
	else if (l_strAction == ACTION_DELETE)
	{
		for (auto lang : l_oLangInfoChangeRequest.m_Body.m_vecData)
		{
			Data::CLanguageInfo l_language;
			l_language.m_strGuid = lang.m_strGuid;
			l_vecLanguage.push_back(l_language);
		}

		Data::IBaseDataPtr l_pLanguage = boost::make_shared<Data::CLanguage>(l_vecLanguage, GetResourceManager());
		//批量删除
		if (!l_pLanguage->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete language to db failed");
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete language info success, size[%d]", l_vecLanguage.size());		
		//发送通知到base服务
		LanguageDeleteNotify(l_oLangInfoChangeRequest.m_Body.m_vecData);
		return true;

	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "unknown action [%s]", l_strAction.c_str());
	}

	return true;
}

void CBusinessImpl::OnSmpLanguageNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		return;
	}
	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive language change message, [%s]", strTmp.c_str());

	if (!_ProcReceiveLanguageInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process language change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process language change success!");
	}
}

void ICC::CBusinessImpl::LanguageChangeNotify(const std::vector<PROTOCOL::CSMPLangInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CSetLanguageRequest l_oSetLangRequest;
		l_oSetLangRequest.m_oHeader.m_strCmd = "set_language_request";
		l_oSetLangRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetLangRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetLangRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		l_oSetLangRequest.m_oBody.m_strCode = p_rData.m_strLangCode;
		l_oSetLangRequest.m_oBody.m_strName = p_rData.m_strLangName;
		l_oSetLangRequest.m_oBody.m_strSort = p_rData.m_strSort;
		l_oSetLangRequest.m_oBody.m_strSyncType = p_strSyncType;
		l_oSetLangRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oSetLangRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetLangRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}	
}

void ICC::CBusinessImpl::LanguageDeleteNotify(const std::vector<PROTOCOL::CSMPLangInfoChangeNotify::CBody::CData>& p_rvecData)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CSetLanguageRequest l_oSetLangRequest;
		l_oSetLangRequest.m_oHeader.m_strCmd = "delete_language_request";
		l_oSetLangRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oSetLangRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		l_oSetLangRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetLangRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetLangRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetLangRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

bool CBusinessImpl::_ProcReceiveSysRoleInfoNotify(const std::string& strMessage)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynNotifyDatas notifyDatas;
	//协议解析
	if (!notifyDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse dict notify failed! [%s]", strMessage.c_str());
		return false;
	}

	std::vector<Data::CRoleInfo> vecAddDatas;
	std::vector<Data::CRoleInfo> vecUpdateDatas;
	std::vector<Data::CRoleInfo> vecDeleteDatas;

	int iCount = notifyDatas.m_vecDatas.size();
	for (PROTOCOL::SmpNotifyData data : notifyDatas.m_vecDatas)
	{
		if (data.strAction == ACTION_DELETE)
		{
			Data::CRoleInfo dataInfo;
			dataInfo.m_strGuid = data.strGuid;
			vecDeleteDatas.push_back(dataInfo);
		}
		else if (data.strAction == ACTION_ADD || data.strAction == ACTION_UPDATE)
		{
			std::string strContent;
			std::map<std::string, std::string> mapHeaders;
			std::string strTarget;
			std::string strErrorMessage;
			std::string strReceive;
			std::string strToken;
			if (!_LoginSmp(strToken))
			{
				ICC_LOG_ERROR(m_pLog, "dict notify login smp failed!");
				return false;
			}

			mapHeaders.insert(std::make_pair("Authorization", strToken));

			strTarget = m_pString->Format("/sync/smp/data-info?cmd=%s&dataGuid=%s", data.strCmd.c_str(), data.strGuid.c_str());
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return false;
			}

			PROTOCOL::CSmpSynDataRoleNotify dataNotify;
			if (!dataNotify.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s  failed!!!!", strTarget.c_str());
				return false;
			}

			if (dataNotify.m_dataRole.m_strEnableFlag == "1")
			{
				Data::CRoleInfo dataInfo;

				_TransSmpRoleToIcc(dataNotify.m_dataRole, dataInfo);

				std::string l_strVal;
				if ((data.strAction == ACTION_UPDATE) && (m_pRedisClient->HGet(ROLE_INFO, dataInfo.m_strGuid, l_strVal)))
				{
					ICC_LOG_ERROR(m_pLog, "update staff guid:[%s]", dataInfo.m_strGuid.c_str());
					vecUpdateDatas.push_back(dataInfo);
				}
				else
				{
					ICC_LOG_ERROR(m_pLog, "can not find staff guid:[%s]", dataInfo.m_strGuid.c_str());
					vecAddDatas.push_back(dataInfo);
				}
			}			
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "unknown role action, action[%s]", data.strAction.c_str());
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CRole>(vecDeleteDatas, GetResourceManager());
		//批量删除数据库
		if (!pBaseData->SingleDelete())
		{
			ICC_LOG_ERROR(m_pLog, "delete role from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CRole>(vecAddDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleAdd())
		{
			ICC_LOG_ERROR(m_pLog, "add role from db failed");
			return false;
		}
	}

	{
		Data::IBaseDataPtr pBaseData = boost::make_shared<Data::CRole>(vecUpdateDatas, GetResourceManager());
		//批量更新数据库
		if (!pBaseData->SingleModify())
		{
			ICC_LOG_ERROR(m_pLog, "update role from db failed");
			return false;
		}
	}


	return true;

	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CSMPRoleInfoChangeNotify l_oRoleInfoChangeRequest;
	//if (!l_oRoleInfoChangeRequest.ParseString(strMessage, l_pIJson))
	//{
	//	return false;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive role change message, size[%d]", l_oRoleInfoChangeRequest.m_Body.m_vecData.size());

	//if (l_oRoleInfoChangeRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "there is no role info changed");
	//	return true;
	//}	

	//int l_iValid = 0;
	//std::vector<Data::CRoleInfo> l_vecRole;
	//std::string l_strAction = l_oRoleInfoChangeRequest.m_Header.m_strAction;
	//if (l_strAction == ACTION_ADD)
	//{
	//	for (auto l_role : l_oRoleInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CRoleInfo l_roleInfo;
	//		l_roleInfo.m_strGuid = l_role.m_strGuid;
	//		l_roleInfo.m_strCode = l_role.m_strCode;
	//		l_roleInfo.m_strName = l_role.m_strName;
	//		l_vecRole.push_back(l_roleInfo);
	//	}

	//	Data::IBaseDataPtr l_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());
	//	//批量更新
	//	if (!l_pRole->SingleAdd())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "add role to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "add role info success, size[%d], valid[%d]", l_vecRole.size(), l_iValid);	
	//	return true;
	//}
	//else if (l_strAction == ACTION_UPDATE)
	//{
	//	for (auto l_role : l_oRoleInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CRoleInfo l_roleInfo;
	//		l_roleInfo.m_strGuid = l_role.m_strGuid;
	//		l_roleInfo.m_strCode = l_role.m_strCode;
	//		l_roleInfo.m_strName = l_role.m_strName;
	//		l_vecRole.push_back(l_roleInfo);
	//	}

	//	Data::IBaseDataPtr l_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());
	//	//批量更新
	//	if (!l_pRole->SingleModify())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "update role to db failed");
	//		return false;
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "update role info, size[%d], valid[%d]", l_vecRole.size(), l_iValid);
	//	return true;
	//}
	//else if (l_strAction == ACTION_DELETE)
	//{
	//	//删除角色绑定数据(功能，数据)
	//	std::vector<Data::CBindInfo> l_vecRoleBindFunc;
	//	std::vector<Data::CBindInfo> l_vecRoleBindData;
	//	std::vector<Data::CBindInfo> vecUserBindRole;

	//	for (auto l_role : l_oRoleInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		Data::CRoleInfo l_roleInfo;
	//		l_roleInfo.m_strGuid = l_role.m_strGuid;
	//		l_vecRole.push_back(l_roleInfo);

	//		//删除角色绑定的功能信息
	//		Data::CBindInfo roleBindFunc;
	//		roleBindFunc.m_strType = BIND_TYPE_ROLE_FUNC;
	//		roleBindFunc.m_strFromGuid = l_role.m_strGuid;
	//		l_vecRoleBindFunc.push_back(roleBindFunc);

	//		//删除角色绑定的数据信息
	//		Data::CBindInfo roleBindDept;
	//		roleBindDept.m_strType = BIND_TYPE_ROLE_DATA;
	//		roleBindDept.m_strFromGuid = l_role.m_strGuid;
	//		l_vecRoleBindData.push_back(roleBindDept);

	//		Data::CBindInfo userBindRole;
	//		userBindRole.m_strType = BIND_TYPE_USER_ROLE;
	//		userBindRole.m_strToGuid = l_role.m_strGuid;
	//		vecUserBindRole.push_back(userBindRole);			
	//	}

	//	Data::IBaseDataPtr l_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());
	//	//批量删除
	//	if (!l_pRole->SingleDelete())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete role to db failed");
	//		return false;
	//	}		

	//	if (!l_vecRoleBindFunc.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecRoleBindFunc, GetResourceManager(), BIND_TYPE_ROLE_FUNC);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind func info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "delete role bind func info success, size[%d]", l_vecRoleBindFunc.size());
	//	}

	//	if (!l_vecRoleBindData.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecRoleBindData, GetResourceManager(), BIND_TYPE_ROLE_DATA);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind data info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "delete role bind data info success, size[%d]", l_vecRoleBindData.size());
	//	}	

	//	if (!vecUserBindRole.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(vecUserBindRole, GetResourceManager(), BIND_TYPE_USER_ROLE);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete user bind role info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "delete user bind role info success, size[%d]", l_vecRoleBindData.size());
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "delete role info success, size[%d]", l_vecRole.size());
	//}
	//else if (l_strAction == ACTION_BIND)
	//{
	//	//1、先删除角色绑定数据(功能，数据)
	//	std::vector<Data::CBindInfo> l_vecDeleteRoleBindFunc;
	//	std::vector<Data::CBindInfo> l_vecDeleteRoleBindData;
	//	for (auto l_role : l_oRoleInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		{
	//			//删除角色绑定的功能信息
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strType = BIND_TYPE_ROLE_FUNC;
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_vecDeleteRoleBindFunc.push_back(l_bindInfo);
	//		}
	//		{
	//			//删除角色绑定的数据信息
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strType = BIND_TYPE_ROLE_DATA;
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_vecDeleteRoleBindData.push_back(l_bindInfo);
	//		}

	//	}

	//	if (!l_vecDeleteRoleBindFunc.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecDeleteRoleBindFunc, GetResourceManager(), BIND_TYPE_ROLE_FUNC);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind func info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "delete role bind func info success, size[%d]", l_vecDeleteRoleBindFunc.size());
	//	}

	//	if (!l_vecDeleteRoleBindData.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecDeleteRoleBindData, GetResourceManager(), BIND_TYPE_ROLE_DATA);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleDelete())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "delete role bind data info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "delete role bind data info success, size[%d]", l_vecDeleteRoleBindData.size());
	//	}	


	//	std::vector<Data::CBindInfo> l_vecAddRoleBindFunc;
	//	std::vector<Data::CBindInfo> l_vecAddRoleBindData;
	//	//2、更新角色绑定的数据
	//	for (auto l_role : l_oRoleInfoChangeRequest.m_Body.m_vecData)
	//	{
	//		//更新角色绑定的数据信息
	//		for (std::string guid : l_role.m_vecDataAuthority)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strGuid = l_role.m_strGuid + std::string("_") + guid; //绑定关系的guid
	//			l_bindInfo.m_strType = BIND_TYPE_ROLE_DATA;
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_bindInfo.m_strToGuid = guid;
	//			l_vecAddRoleBindData.push_back(l_bindInfo);
	//		}
	//		//更新角色绑定的功能数据信息
	//		for (std::string guid : l_role.m_vecFuncGuid)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strGuid = l_role.m_strGuid + std::string("_") + guid; //绑定关系的guid
	//			l_bindInfo.m_strType = BIND_TYPE_ROLE_FUNC;
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_bindInfo.m_strToGuid = guid;
	//			l_vecAddRoleBindFunc.push_back(l_bindInfo);
	//		}

	//		//角色绑定系统权限 这里不需要关心

	//	}

	//	if (!l_vecAddRoleBindFunc.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecAddRoleBindFunc, GetResourceManager(), BIND_TYPE_ROLE_FUNC);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleAdd())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "add role bind func info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "add role bind func info success, size[%d]", l_vecAddRoleBindFunc.size());
	//	}

	//	if (!l_vecAddRoleBindData.empty())
	//	{
	//		Data::IBaseDataPtr l_pBindDel = boost::make_shared<Data::CBind>(l_vecAddRoleBindData, GetResourceManager(), BIND_TYPE_ROLE_DATA);
	//		//1.1批量删除
	//		if (!l_pBindDel->SingleAdd())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "add role bind data info to db failed");
	//			return false;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "add role bind data info success, size[%d]", l_vecAddRoleBindData.size());
	//	}
	//}
	//else
	//{
	//	ICC_LOG_WARNING(m_pLog, "Unknown Action [%s]", l_strAction.c_str());
	//}

	//return true;
}



//处理SMP推送来的角色信息变更消息
void ICC::CBusinessImpl::OnSmpRoleNotify(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive role info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive role change from smp, [%s]", strTmp.c_str());

	m_threadProc.AddMessage(this, THREADID_RECEIVE_CHANGED_NOTIFY_ROLE, 0, 0, 0, strTmp);

	/*if (!_ProcReceiveSysRoleInfoNotify(strTmp))
	{
		ICC_LOG_DEBUG(m_pLog, "process role change failed, [%s]", strTmp.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "process role change success!");
	}*/
}

//发送角色信息设置请求至基础数据服务
void ICC::CBusinessImpl::RoleChangeNotify(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CSetRoleRequest l_oSetRoleRequest;
		l_oSetRoleRequest.m_oHeader.m_strCmd = "set_role_request";
		l_oSetRoleRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetRoleRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetRoleRequest.m_oBody.m_strCode = p_rData.m_strCode;
		l_oSetRoleRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		l_oSetRoleRequest.m_oBody.m_strName = p_rData.m_strName;
		l_oSetRoleRequest.m_oBody.m_strSyncType = p_strSyncType;
		l_oSetRoleRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oSetRoleRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oSetRoleRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

//发送角色信息删除请求至基础数据服务
void ICC::CBusinessImpl::RoleDeleteNotify(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData)
{
	for (auto p_rData :  p_rvecData)
	{
		PROTOCOL::CDeleteRoleRequest l_oDeleteRoleRequest;
		l_oDeleteRoleRequest.m_oHeader.m_strCmd = "delete_role_request";
		l_oDeleteRoleRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oDeleteRoleRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oDeleteRoleRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		l_oDeleteRoleRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oDeleteRoleRequest.m_oBody.m_strGuid = p_rData.m_strGuid;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_oDeleteRoleRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}

//推送角色绑定权限变更消息至基础数据服务
void ICC::CBusinessImpl::RoleBindFuncRequest(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData)
{
	for (auto p_rData :  p_rvecData)
	{
		//先删除该角色的绑定关系，再发送设置请求；因为直接发送时，若某个角色的绑定关系比以前更少，则基础数据无法删除旧的绑定关系
		{
			PROTOCOL::CDeleteBindRequest l_oDeleteBindRequest;
			l_oDeleteBindRequest.m_oHeader.m_strCmd = "delete_bind_request";
			l_oDeleteBindRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oDeleteBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
			l_oDeleteBindRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
			l_oDeleteBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

			l_oDeleteBindRequest.m_oBody.m_strFromGuid = p_rData.m_strGuid;
			l_oDeleteBindRequest.m_oBody.m_strType = "role_bind_func";

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_oDeleteBindRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}

		{
			PROTOCOL::CDeleteBindRequest l_oDeleteBindRequest;
			l_oDeleteBindRequest.m_oHeader.m_strCmd = "delete_bind_request";
			l_oDeleteBindRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oDeleteBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
			l_oDeleteBindRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
			l_oDeleteBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();

			l_oDeleteBindRequest.m_oBody.m_strFromGuid = p_rData.m_strGuid;
			l_oDeleteBindRequest.m_oBody.m_strType = "role_bind_data";

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_oDeleteBindRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}

		PROTOCOL::CSetBindRequest l_oSetBindRequest;
		l_oSetBindRequest.m_oHeader.m_strCmd = "set_bind_request";
		l_oSetBindRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
		l_oSetBindRequest.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
		l_oSetBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetBindRequest.m_oBody.m_strFromGuid = p_rData.m_strGuid;

		//发送角色绑定数据权限消息
		for (std::string guid :  p_rData.m_vecDataAuthority)
		{
			l_oSetBindRequest.m_oBody.m_strGuid = p_rData.m_strGuid + std::string("_") + guid;//m_pString->CreateGuid();
			l_oSetBindRequest.m_oBody.m_strToGuid = guid;
			l_oSetBindRequest.m_oBody.m_strType = BIND_TYPE_ROLE_DATA;
			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_oSetBindRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}

		//发送角色绑定功能权限消息
		for (std::string guid :  p_rData.m_vecFuncGuid)
		{
			l_oSetBindRequest.m_oBody.m_strGuid = p_rData.m_strGuid + std::string("_") + guid;//m_pString->CreateGuid();
			l_oSetBindRequest.m_oBody.m_strToGuid = guid;
			l_oSetBindRequest.m_oBody.m_strType = BIND_TYPE_ROLE_FUNC;
			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMsg = l_oSetBindRequest.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
		}

		//角色绑定系统权限 - ICC 系统无该权限
		/*for (std::string guid :  p_rData.m_vecSystemGuid)
		{
		l_oSetBindRequest.m_oBody.m_strGuid = m_pString->CreateGuid();
		l_oSetBindRequest.m_oBody.m_strToGuid = guid;
		l_oSetBindRequest.m_oBody.m_strType = BIND_TYPE_ROLE_SYSTEM;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_oSetBindRequest.ToString(l_pIJson),
		ObserverPattern::ERequestMode::Request_Respond));
		}*/
	}
}

int CBusinessImpl::_ProcReceiveUserStaffBindInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetUserStaffInfoRequest l_CGetUserStaffInfoRequest;
	if (!l_CGetUserStaffInfoRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive user bind staff from smp, count:value[%s : %d]", l_CGetUserStaffInfoRequest.m_Body.m_strCount.c_str(), l_CGetUserStaffInfoRequest.m_Body.m_vecData.size());

	if (l_CGetUserStaffInfoRequest.m_Body.m_vecData.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user bind staff info received from smp,return");
		return SYNDATA_SUCCESS;
	}

	int iTotalCount = 0;
	if (!l_CGetUserStaffInfoRequest.m_Body.m_strCount.empty())
	{
		iTotalCount = std::stoi(l_CGetUserStaffInfoRequest.m_Body.m_strCount);
	}	

	std::vector<Data::CBindInfo> l_vecBind;
	for (auto var : l_CGetUserStaffInfoRequest.m_Body.m_vecData)
	{
		Data::CBindInfo l_bindInfo;
		l_bindInfo.m_strGuid = var.m_strGuid + std::string("_") + var.m_strStaffGuid; //绑定关系的guid
		l_bindInfo.m_strType = "user_bind_staff";
		l_bindInfo.m_strFromGuid = var.m_strGuid;
		l_bindInfo.m_strToGuid = var.m_strStaffGuid;

		l_vecBind.push_back(l_bindInfo);
	}

	ICC_LOG_DEBUG(m_pLog, "valid user bind staff data!! size[%d]", l_vecBind.size());

	boost::shared_ptr<Data::CBind> pProcesser = boost::dynamic_pointer_cast<Data::CBind>(m_pUserBindStaff);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "user bind staff object is null!!!");
		return SYNDATA_FAILED;
	}

	pProcesser->AppendReceiveDataSize(l_CGetUserStaffInfoRequest.m_Body.m_vecData.size());
	pProcesser->Append(l_vecBind);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append user bind staff success!! size[%d], total size[%d]", l_vecBind.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	{
		/*if (m_dataVersion.m_strLanguageVersion == l_CGetLanguageInfoRequest.m_Body.m_strVersionData)
		{
			ICC_LOG_ERROR(m_pLog, "language version is same, not need update");
			return true;
		}*/

		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update user bind staff info to db failed!");
			return SYNDATA_FAILED;
		}

		m_dataVersion.m_strUserStaffBindVersion = l_CGetUserStaffInfoRequest.m_Body.m_strVersionData;
		
		ICC_LOG_DEBUG(m_pLog, "update user bind staff info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecBind.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "user bind staff is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendUserStaffBindRequest(l_CGetUserStaffInfoRequest.m_Body.m_vecData);

	return iRs;	
}

void CBusinessImpl::OnGetSmpUserStaffBind(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive user bind staff info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive user bind staff from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveUserStaffBindInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn user bind staff data success!");
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_USER_ROLE);	
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn user bind staff data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn user bind staff data failed!");
	}
}

void CBusinessImpl::SendUserStaffBindRequest(const std::vector<PROTOCOL::CGetUserStaffInfoRequest::CBody::CData>& p_BindInfo)
{
	PROTOCOL::CSetBindRequest l_CSetBindRequest;
	l_CSetBindRequest.m_oHeader.m_strCmd = "set_bind_request";
	l_CSetBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
	l_CSetBindRequest.m_oHeader.m_strRequestType = "0";
	l_CSetBindRequest.m_oHeader.m_strRelatedID = "user_bind_staff";
	int l_iValidSize = 0;

	for (auto & item :  p_BindInfo)
	{
		++l_iValidSize;
		l_CSetBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_CSetBindRequest.m_oBody.m_strGuid = item.m_strGuid + std::string("_") + item.m_strStaffGuid;//m_pString->CreateGuid();
		l_CSetBindRequest.m_oBody.m_strType = "user_bind_staff";
		l_CSetBindRequest.m_oBody.m_strFromGuid = item.m_strGuid;
		l_CSetBindRequest.m_oBody.m_strToGuid = item.m_strStaffGuid;
		l_CSetBindRequest.m_oBody.m_strSort = "";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMsg = l_CSetBindRequest.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
	}
}


int CBusinessImpl::_ProcReceiveUserRoleInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetUserRoleInfoRequest l_CGetUserRoleInfoRequest;
	if (!l_CGetUserRoleInfoRequest.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive language from smp, count:value[%s : %d]", l_CGetUserRoleInfoRequest.m_Body.m_strCount.c_str(), l_CGetUserRoleInfoRequest.m_Body.m_vecData.size());

	if (l_CGetUserRoleInfoRequest.m_Body.m_vecData.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user bind role info received from smp,return");
		return SYNDATA_SUCCESS;
	}

	int iTotalCount = 0;
	if (!l_CGetUserRoleInfoRequest.m_Body.m_strCount.empty())
	{
		iTotalCount = std::stoi(l_CGetUserRoleInfoRequest.m_Body.m_strCount);
	}	

	std::vector<Data::CBindInfo> l_vecBind;
	for (auto l_userInfo : l_CGetUserRoleInfoRequest.m_Body.m_vecData)
	{
		if (l_userInfo.m_IsValid == "Y")
		{
			Data::CBindInfo l_bindInfo;
			l_bindInfo.m_strType = "user_bind_role";
			l_bindInfo.m_strFromGuid = l_userInfo.m_strUserGuid;

			for (auto l_role : l_userInfo.m_vecRoleInfo)
			{
				if (l_role.m_IsValid == "Y")
				{
					l_bindInfo.m_strToGuid = l_role.m_strGuid;
					l_bindInfo.m_strGuid = l_userInfo.m_strUserGuid + std::string("_") + l_role.m_strGuid;//绑定关系的guid
					l_vecBind.push_back(l_bindInfo);
				}
			}
		}
	}

	ICC_LOG_DEBUG(m_pLog, "valid user bind role data!! size[%d]", l_vecBind.size());

	boost::shared_ptr<Data::CBind> pProcesser = boost::dynamic_pointer_cast<Data::CBind>(m_pUserBindRole);
	if (!pProcesser)
	{
		ICC_LOG_DEBUG(m_pLog, "language object is null!!!");
		return SYNDATA_FAILED;
	}

	pProcesser->AppendReceiveDataSize(l_CGetUserRoleInfoRequest.m_Body.m_vecData.size());
	pProcesser->Append(l_vecBind);
	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	ICC_LOG_DEBUG(m_pLog, "append language success!! size[%d], total size[%d]", l_vecBind.size(), iTotalDataSize);

	int iRs = SYNDATA_FAILED;
	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	{
		/*if (m_dataVersion.m_strLanguageVersion == l_CGetLanguageInfoRequest.m_Body.m_strVersionData)
		{
			ICC_LOG_ERROR(m_pLog, "language version is same, not need update");
			return true;
		}*/

		//批量更新
		if (!pProcesser->SynUpdate())
		{
			ICC_LOG_ERROR(m_pLog, "update language to db failed!");
			return SYNDATA_FAILED;
		}

		m_dataVersion.m_strUserRoleBindVersion= l_CGetUserRoleInfoRequest.m_Body.m_strVersionData;

		ICC_LOG_DEBUG(m_pLog, "update user bind role info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecBind.size(), iTotalDataSize);

		iRs = SYNDATA_SUCCESS;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "user bind role is not enough, wait next data!!");
		iRs = SYNDATA_WAITNEXTDATA;
	}

	//发送通知到base服务
	//SendUserRoleRequest(l_CGetUserRoleInfoRequest.m_Body.m_vecData);

	return iRs;
}

void CBusinessImpl::OnGetUserRoleInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive language info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive user bind role from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveUserRoleInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn user bind role data success!");		
		
	    m_threadProc.AddMessage(this, THREADID_REQUEST_SYS_ROLE);
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn user bind role data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn user bind role data failed!");
	}
}

void CBusinessImpl::SendUserRoleRequest(const std::vector<PROTOCOL::CGetUserRoleInfoRequest::CBody::CData>& p_rUserRole)
{
	for (auto l_userInfo : p_rUserRole)
	{
		if (l_userInfo.m_IsValid == "Y" || l_userInfo.m_IsValid == "y")
		{
			std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
			std::string l_strGuid = m_pString->CreateGuid();

			PROTOCOL::CSetBindRequest l_CSetBindRequest;
			l_CSetBindRequest.m_oHeader.m_strCmd = "set_bind_request";
			l_CSetBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
			l_CSetBindRequest.m_oHeader.m_strRequestType = "0";
			l_CSetBindRequest.m_oHeader.m_strRelatedID = "user_bind_role";
			//l_CSetBindRequest.m_oBody.m_strGuid = l_userInfo.m_strUserGuid + std::string("_") + l_role.m_strGuid;//m_pString->CreateGuid();;
			l_CSetBindRequest.m_oBody.m_strType = "user_bind_role";
			l_CSetBindRequest.m_oBody.m_strFromGuid = l_userInfo.m_strUserGuid;

			for (auto & role : l_userInfo.m_vecRoleInfo)
			{
				l_CSetBindRequest.m_oBody.m_strGuid = l_userInfo.m_strUserGuid + std::string("_") + role.m_strGuid;//m_pString->CreateGuid();;
				l_CSetBindRequest.m_oBody.m_strToGuid = role.m_strGuid;
				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMsg = l_CSetBindRequest.ToString(l_pIJson);
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
			}

			for (auto l_roleInfo : l_userInfo.m_vecRoleInfo)
			{
				//SMP只同步有效数据到基础数据服务
				if (l_roleInfo.m_IsValid == "Y" || l_roleInfo.m_IsValid == "y")
				{
					SendRoleRequest(l_roleInfo);
				}
			}
		}
	}
}

void CBusinessImpl::SendRoleRequest(const PROTOCOL::CGetUserRoleInfoRequest::CBody::CData::CRoleInfo& p_rRoleInfo)
{
	PROTOCOL::CHeader l_pRequestHeaderInit;
	l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
	l_pRequestHeaderInit.m_strResponseType = "0";
	PROTOCOL::CSetRoleRequest l_oSetFuncRoleRequest;
	GeneralHeader("set_role_request", l_pRequestHeaderInit, l_oSetFuncRoleRequest.m_oHeader);

	l_oSetFuncRoleRequest.m_oBody.m_strGuid = p_rRoleInfo.m_strGuid;
	l_oSetFuncRoleRequest.m_oBody.m_strParentGuid = "";
	l_oSetFuncRoleRequest.m_oBody.m_strCode = "";
	l_oSetFuncRoleRequest.m_oBody.m_strName = p_rRoleInfo.m_strName;
	l_oSetFuncRoleRequest.m_oBody.m_strSort = "";

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_oSetFuncRoleRequest.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

int CBusinessImpl::_ProcReceiveSysRoleInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSmpSynDataRole roleDatas;
	if (!roleDatas.ParseString(strMessage, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
		return SYNDATA_FAILED;
	}

	ICC_LOG_DEBUG(m_pLog, "receive role from smp, islast:package:packagesize:count[%s:%s:%s:%d]", bIsLast ? "true" : "false", roleDatas.m_strCurrentPackageCount.c_str(), roleDatas.m_strPackageSize.c_str(), roleDatas.m_vecRoles.size());

	if (roleDatas.m_vecRoles.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role bind func info received from smp,return");
		return SYNDATA_SUCCESS;
	}

	std::vector<Data::CRoleInfo> l_vecRole;

	for (auto l_role : roleDatas.m_vecRoles)
	{
		if (l_role.m_strEnableFlag == "1")
		{
			Data::CRoleInfo l_roleInfo;
			
			_TransSmpRoleToIcc(l_role, l_roleInfo);

			l_vecRole.push_back(l_roleInfo);
		}
	}

	int iRs = SYNDATA_FAILED;

	{
		boost::shared_ptr<Data::CRole> pProcesser = boost::dynamic_pointer_cast<Data::CRole>(m_pRole);
		if (!pProcesser)
		{
			ICC_LOG_DEBUG(m_pLog, "role object is null!!!");
			return SYNDATA_FAILED;
		}
		
		pProcesser->Append(l_vecRole);
		int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
		ICC_LOG_DEBUG(m_pLog, "append role success!! size[%d], total size[%d]", l_vecRole.size(), iTotalDataSize);

		if (bIsLast)
		{
			//批量更新
			if (!pProcesser->SynUpdate())
			{
				ICC_LOG_ERROR(m_pLog, "update role to db failed!");
				return SYNDATA_FAILED;
			}			

			ICC_LOG_DEBUG(m_pLog, "update role info success, valid[%d], update size[%d]", l_vecRole.size(), iTotalDataSize);

			iRs = SYNDATA_SUCCESS;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "role is not enough, wait next data!!");
			iRs = SYNDATA_WAITNEXTDATA;
		}
	}	

	return iRs;


	//JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	//PROTOCOL::CGetRoleFuncBindRequest l_CGetRoleFuncBindRequest;
	//if (!l_CGetRoleFuncBindRequest.ParseString(strMessage, l_pIJson))
	//{
	//	ICC_LOG_ERROR(m_pLog, "Is Not Json: [%s]", strMessage.c_str());
	//	return SYNDATA_FAILED;
	//}

	//ICC_LOG_DEBUG(m_pLog, "receive role info from smp, count:value[%s : %d]", l_CGetRoleFuncBindRequest.m_Body.m_strCount.c_str(), l_CGetRoleFuncBindRequest.m_Body.m_vecData.size());

	//if (l_CGetRoleFuncBindRequest.m_Body.m_vecData.empty())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "no role bind func info received from smp,return");
	//	return SYNDATA_SUCCESS;
	//}

	//int iTotalCount = 0;
	//if (!l_CGetRoleFuncBindRequest.m_Body.m_strCount.empty())
	//{
	//	iTotalCount = std::stoi(l_CGetRoleFuncBindRequest.m_Body.m_strCount);
	//}

	//std::vector<Data::CBindInfo> l_vecRoleBindFunc;
	//std::vector<Data::CBindInfo> l_vecRoleBindDept;
	//std::vector<Data::CRoleInfo> l_vecRole;

	//for (auto l_role : l_CGetRoleFuncBindRequest.m_Body.m_vecData)
	//{
	//	if (l_role.m_IsValid == "Y" || l_role.m_IsValid == "y")
	//	{
	//		//角色绑定权限
	//		for (auto l_func : l_role.m_vecFuncInfo)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strGuid = l_role.m_strGuid + std::string("_") + l_func.m_strGuid;//绑定关系的guid
	//			l_bindInfo.m_strType = "role_bind_func";
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_bindInfo.m_strToGuid = l_func.m_strGuid;
	//			l_vecRoleBindFunc.push_back(l_bindInfo);
	//		}

	//		//角色绑定数据
	//		for (auto l_data : l_role.m_vecDataInfo)
	//		{
	//			Data::CBindInfo l_bindInfo;
	//			l_bindInfo.m_strGuid = l_role.m_strGuid + std::string("_") + l_data.m_strGuid;//绑定关系的guid
	//			l_bindInfo.m_strType = "role_bind_data";
	//			l_bindInfo.m_strFromGuid = l_role.m_strGuid;
	//			l_bindInfo.m_strToGuid = l_data.m_strGuid;
	//			l_vecRoleBindDept.push_back(l_bindInfo);
	//		}

	//		Data::CRoleInfo l_roleInfo;
	//		l_roleInfo.m_strGuid = l_role.m_strGuid;
	//		l_roleInfo.m_strCode = l_role.m_strCode;
	//		l_roleInfo.m_strName = l_role.m_strName;
	//		l_vecRole.push_back(l_roleInfo);
	//	}
	//}

	//int iRs = SYNDATA_FAILED;

	//{
	//	boost::shared_ptr<Data::CRole> pProcesser = boost::dynamic_pointer_cast<Data::CRole>(m_pRole);
	//	if (!pProcesser)
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role object is null!!!");
	//		return SYNDATA_FAILED;
	//	}

	//	pProcesser->AppendReceiveDataSize(l_CGetRoleFuncBindRequest.m_Body.m_vecData.size());
	//	pProcesser->Append(l_vecRole);
	//	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//	ICC_LOG_DEBUG(m_pLog, "append role success!! size[%d], total size[%d]", l_vecRole.size(), iTotalDataSize);
	//	
	//	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//	{
	//		//批量更新
	//		if (!pProcesser->SynUpdate())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "update role to db failed!");
	//			return SYNDATA_FAILED;
	//		}

	//		m_dataVersion.m_strLanguageVersion = l_CGetRoleFuncBindRequest.m_Body.m_strVersionData;

	//		ICC_LOG_DEBUG(m_pLog, "update role info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecRole.size(), iTotalDataSize);

	//		iRs = SYNDATA_SUCCESS;
	//	}
	//	else
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role is not enough, wait next data!!");
	//		iRs = SYNDATA_WAITNEXTDATA;
	//	}
	//}

	//

	//{
	//	boost::shared_ptr<Data::CBind> pProcesser = boost::dynamic_pointer_cast<Data::CBind>(m_pRoleBindFunc);
	//	if (!pProcesser)
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role bind func object is null!!!");
	//		return SYNDATA_FAILED;
	//	}

	//	pProcesser->AppendReceiveDataSize(l_CGetRoleFuncBindRequest.m_Body.m_vecData.size());
	//	pProcesser->Append(l_vecRoleBindFunc);
	//	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//	ICC_LOG_DEBUG(m_pLog, "append role bind func success!! size[%d], total size[%d]", l_vecRoleBindFunc.size(), iTotalDataSize);
	//	
	//	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//	{
	//		//批量更新
	//		if (!pProcesser->SynUpdate())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "update role bind func to db failed!");
	//			return SYNDATA_FAILED;
	//		}

	//		m_dataVersion.m_strLanguageVersion = l_CGetRoleFuncBindRequest.m_Body.m_strVersionData;
	//		
	//		ICC_LOG_DEBUG(m_pLog, "update role bind func info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecRoleBindFunc.size(), iTotalDataSize);

	//		iRs = SYNDATA_SUCCESS;
	//	}
	//	else
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role bind func is not enough, wait next data!!");
	//		iRs = SYNDATA_WAITNEXTDATA;
	//	}
	//}

	//{
	//	boost::shared_ptr<Data::CBind> pProcesser = boost::dynamic_pointer_cast<Data::CBind>(m_pRoleBindData);
	//	if (!pProcesser)
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role bind dept object is null!!!");
	//		return SYNDATA_FAILED;
	//	}

	//	pProcesser->AppendReceiveDataSize(l_CGetRoleFuncBindRequest.m_Body.m_vecData.size());
	//	pProcesser->Append(l_vecRoleBindDept);
	//	int iTotalDataSize = pProcesser->ReceiveTotalDataSize();
	//	ICC_LOG_DEBUG(m_pLog, "append role bind dept success!! size[%d], total size[%d]", l_vecRoleBindDept.size(), iTotalDataSize);
	//	
	//	if (iTotalCount == pProcesser->ReceiveTotalDataSize())
	//	{
	//		//批量更新
	//		if (!pProcesser->SynUpdate())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "update role bind dept to db failed!");
	//			return SYNDATA_FAILED;
	//		}

	//		m_dataVersion.m_strRoleAuthorityBindVersion = l_CGetRoleFuncBindRequest.m_Body.m_strVersionData;

	//		ICC_LOG_DEBUG(m_pLog, "update role bind dept info success, total count[%d], valid[%d], update size[%d]", iTotalCount, l_vecRoleBindDept.size(), iTotalDataSize);

	//		iRs = SYNDATA_SUCCESS;
	//	}
	//	else
	//	{
	//		ICC_LOG_DEBUG(m_pLog, "role bind dept is not enough, wait next data!!");
	//		iRs = SYNDATA_WAITNEXTDATA;
	//	}
	//}

	////SendSysRoleInfoRequest(l_CGetRoleFuncBindRequest.m_Body.m_vecData);

	//return iRs;	
}

void CBusinessImpl::OnGetSysRoleInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "receive role info is null!");
		return;
	}

	std::string strTmp = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive role from smp, [%s]", strTmp.c_str());

	int iRs = _ProcReceiveSysRoleInfo(strTmp);
	if (iRs == SYNDATA_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "syn role data success!");
		m_bNeedSynData = false;
		m_bSynDataSuccess = true;
		_SendSynDataSuccess();
	}
	else if (iRs == SYNDATA_WAITNEXTDATA)
	{
		ICC_LOG_DEBUG(m_pLog, "syn role data wait next data!");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn role data failed!");
	}	
}

void CBusinessImpl::SendSysRoleInfoRequest(const std::vector<PROTOCOL::CGetRoleFuncBindRequest::CBody::CData>& p_rSysRoleInfo)
{
	int l_iValidSize = 0;
	for (auto l_sysSoleInfo :  p_rSysRoleInfo)
	{
		//SMP只同步有效数据到基础数据服务
		if (l_sysSoleInfo.m_IsValid == "Y")
		{
			++l_iValidSize;
			PROTOCOL::CSetBindRequest l_CSetBindRequest;
			l_CSetBindRequest.m_oHeader.m_strCmd = "set_bind_request";
			l_CSetBindRequest.m_oHeader.m_strRequest = QUEUE_BASEDATA;
			l_CSetBindRequest.m_oHeader.m_strRequestType = "0";
			l_CSetBindRequest.m_oHeader.m_strRelatedID = "role_bind_func";

			//角色绑定权限
			for (auto func :  l_sysSoleInfo.m_vecFuncInfo)
			{
				l_CSetBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
				l_CSetBindRequest.m_oBody.m_strGuid = l_sysSoleInfo.m_strGuid + std::string("_") + func.m_strGuid;//m_pString->CreateGuid();
				l_CSetBindRequest.m_oBody.m_strType = "role_bind_func";
				l_CSetBindRequest.m_oBody.m_strFromGuid = l_sysSoleInfo.m_strGuid;
				l_CSetBindRequest.m_oBody.m_strToGuid = func.m_strGuid;
				l_CSetBindRequest.m_oBody.m_strSort = "";
				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strSetBindRequest = l_CSetBindRequest.ToString(l_pIJson);
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSetBindRequest));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSetBindRequest.c_str());

				//权限设置请求
				PROTOCOL::CHeader l_pRequestHeaderInit;
				l_pRequestHeaderInit.m_strResponse = QUEUE_BASEDATA;
				l_pRequestHeaderInit.m_strResponseType = "0";
				PROTOCOL::CSetFuncRequest l_oSetFuncRequest;
				GeneralHeader("set_func_request", l_pRequestHeaderInit, l_oSetFuncRequest.m_oHeader);
				l_oSetFuncRequest.m_oBody.m_strGuid = func.m_strGuid;
				l_oSetFuncRequest.m_oBody.m_strParentGuid = func.m_strParentCode;
				l_oSetFuncRequest.m_oBody.m_strCode = func.m_strCode;
				l_oSetFuncRequest.m_oBody.m_strName = func.m_strName;
				l_oSetFuncRequest.m_oBody.m_strSort = "";
				l_pIJson = ICCGetIJsonFactory()->CreateJson();
				l_strSetBindRequest = l_oSetFuncRequest.ToString(l_pIJson);
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSetBindRequest));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSetBindRequest.c_str());
			}

			//角色绑定数据
			l_CSetBindRequest.m_oHeader.m_strRelatedID = "role_bind_data";
			for (auto &data :  l_sysSoleInfo.m_vecDataInfo)
			{
				l_CSetBindRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
				l_CSetBindRequest.m_oBody.m_strGuid = l_sysSoleInfo.m_strGuid + std::string("_") + data.m_strGuid;//m_pString->CreateGuid();
				l_CSetBindRequest.m_oBody.m_strType = "role_bind_data";
				l_CSetBindRequest.m_oBody.m_strFromGuid = l_sysSoleInfo.m_strGuid;
				l_CSetBindRequest.m_oBody.m_strToGuid = data.m_strGuid;
				l_CSetBindRequest.m_oBody.m_strSort = "";
				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMessage = l_CSetBindRequest.ToString(l_pIJson);
				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
			}
		}
	}
}


int CBusinessImpl::_ProcReceiveSeatsInfo(const std::string& strMessage, bool bIsLast /* = false */)
{
	ICC_LOG_DEBUG(m_pLog, "receive seatsInfo [%s]", strMessage.c_str());
	return 0;
}



void CBusinessImpl::OnGetExpiration(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive client to get license request");

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::GetLicenseInfo l_getLicenseInfoReq;
	if (!l_getLicenseInfoReq.ParseString(p_pNotifiRequest->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "is not json: [%s]", p_pNotifiRequest->GetMessages().c_str());
		return;
	}

	std::string l_strLicenseGenDate = m_pLicense->GetGenDate();//license生成时间
	unsigned int l_iLicenseVaildDays = m_pLicense->GetValidDays();//license有效天数
	DateTime::CDateTime l_start = m_pDateTime->FromString(l_strLicenseGenDate);
	DateTime::CDateTime l_end = m_pDateTime->AddDays(l_start, l_iLicenseVaildDays);
	std::string l_strLicenseEndDate = m_pDateTime->ToString(l_end);//license截止时间

	ICC_LOG_DEBUG(m_pLog, "##license gen time[%s], expiration time[%s]", l_strLicenseGenDate.c_str(), l_strLicenseEndDate.c_str());

	DateTime::CDateTime l_current = m_pDateTime->CurrentDateTime();
	int l_iDays = m_pDateTime->DaysDifference(l_end, l_current);

	PROTOCOL::GetLicenseInfo l_getLicenseInfoResp;	
	l_getLicenseInfoResp.m_oHeader.m_strSystemID = SYSTEMID;
	l_getLicenseInfoResp.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_getLicenseInfoResp.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_getLicenseInfoResp.m_oHeader.m_strRelatedID = l_getLicenseInfoReq.m_oHeader.m_strMsgid;
	l_getLicenseInfoResp.m_oHeader.m_strCmd = "get_expiration_respond";
	l_getLicenseInfoResp.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_getLicenseInfoResp.m_oHeader.m_strRequest = l_getLicenseInfoReq.m_oHeader.m_strResponse;
	l_getLicenseInfoResp.m_oHeader.m_strRequestType = l_getLicenseInfoReq.m_oHeader.m_strResponseType;

	l_getLicenseInfoResp.m_oBody.m_strGenDate = m_pLicense->GetGenDate();
	l_getLicenseInfoResp.m_oBody.m_strValidDayse = m_pString->Number(l_iDays);
	l_getLicenseInfoResp.m_oBody.m_strExpirationDate = l_strLicenseEndDate;
	l_getLicenseInfoResp.m_oBody.m_strClientNum = m_pString->Number(m_pLicense->GetClientNum());
	l_getLicenseInfoResp.m_oBody.m_strLicenseState = l_iDays > 0 ? "1" : "0";

	std::string l_strMsg = l_getLicenseInfoResp.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotifiRequest->Response(l_strMsg);
	ICC_LOG_DEBUG(m_pLog, "respond license info: %s", l_strMsg.c_str());
}

void CBusinessImpl::GeneralHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader)
{
	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = "";
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
	p_pRespHeader.m_strResponse = "";
	p_pRespHeader.m_strResponseType = "";
}

void CBusinessImpl::OnRequestInfo(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (m_bFirstSynDataFlag)
	{
		m_bFirstSynDataFlag = false;
		ICC_LOG_DEBUG(m_pLog, "server start, first syn data!");
		RequestInfoFunc();
	}
	else
	{
		if (!m_bSynDataSuccess)
		{
			ICC_LOG_DEBUG(m_pLog, "server start, first syn not success!!! will start syn data!");
			RequestInfoFunc();
		}
		else
		{
			m_strSyncHour = m_pConfig->GetValue("ICC/Plugin/SMP/SyncHour", "3");
			if (m_pString->ToInt(m_strSyncHour) < 0 || m_pString->ToInt(m_strSyncHour) >= 24)
			{
				m_strSyncHour = "3";
			}
			m_strSyncMinute = m_pConfig->GetValue("ICC/Plugin/SMP/SyncMinute", "0");
			if (m_pString->ToInt(m_strSyncMinute) < 0 || m_pString->ToInt(m_strSyncMinute) >= 60)
			{
				m_strSyncMinute = "0";
			}

			std::string strCurDate = m_pDateTime->CurrentDateTimeStr();
			ICC_LOG_DEBUG(m_pLog, "smp current time [%s]", strCurDate.c_str());
			if (strCurDate.length() >= 16 && m_pString->ToInt(m_strSyncHour) == m_pString->ToInt(strCurDate.substr(11, 2)) && m_pString->ToInt(m_strSyncMinute) == m_pString->ToInt(strCurDate.substr(14, 2)))
			{
				ICC_LOG_DEBUG(m_pLog, "will begin syn data, cur time: %s", strCurDate.c_str());
				RequestInfoFunc();
			}
		}		
	}
	

		
}

void CBusinessImpl::OnCheckLicense(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	CheckLicense();
}

void CBusinessImpl::CheckLicense()
{
	std::string l_strLicenseGenDate = m_pLicense->GetGenDate();//license生成时间
	unsigned int l_iLicenseVaildDays = m_pLicense->GetValidDays();//license有效天数
	DateTime::CDateTime l_start = m_pDateTime->FromString(l_strLicenseGenDate);
	DateTime::CDateTime l_end = m_pDateTime->AddDays(l_start, l_iLicenseVaildDays);
	std::string l_strLicenseEndDate = m_pDateTime->ToString(l_end);//license截止时间

	ICC_LOG_DEBUG(m_pLog, "##license gen time[%s], expiration time[%s]", l_strLicenseGenDate.c_str(), l_strLicenseEndDate.c_str());

	DateTime::CDateTime l_current = m_pDateTime->CurrentDateTime();
	int l_iDays = m_pDateTime->DaysDifference(l_end, l_current);
	if (l_iDays < 30)
	{
		PROTOCOL::CSyncLicense l_SyncLicense;
		l_SyncLicense.m_oHeader.m_strCmd = "expiration_sync";
		l_SyncLicense.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_SyncLicense.m_oHeader.m_strRequest = "topic_license_sync";
		l_SyncLicense.m_oHeader.m_strRequestType = MQ_DEST_TOPIC;
		l_SyncLicense.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		
		l_SyncLicense.m_oBody.m_strGenDate = m_pLicense->GetGenDate();
		l_SyncLicense.m_oBody.m_strValidDayse = m_pString->Number(l_iDays);
		l_SyncLicense.m_oBody.m_strExpirationDate = l_strLicenseEndDate;

		l_SyncLicense.m_oBody.m_strClientNum = m_pString->Number(m_pLicense->GetClientNum());
		l_SyncLicense.m_oBody.m_strLicenseState = l_iDays > 0 ? "1" : "0";

		std::string l_strMsg = l_SyncLicense.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "sync license info: %s", l_strMsg.c_str());
	}
}

void ICC::CBusinessImpl::LoadDeptMapInfo()
{
	const unsigned int l_iCorrectSize = 3;
	m_strSystemType = m_pConfig->GetValue("ICC/Plugin/SMP/SystemType", "");
	m_strRootDeptCode = m_pConfig->GetValue("ICC/Plugin/SMP/SystemRootDeptCode", "");
	std::string l_strMapDeptInfo = m_pConfig->GetValue("ICC/Plugin/SMP/DeptCodeMappings", "");
	if (!l_strMapDeptInfo.empty())
	{
		std::vector<std::string>l_vSplitInfo;
		m_pString->Split(m_pString->ReplaceAll(l_strMapDeptInfo, " ", ""), ";", l_vSplitInfo, 1);
		for (auto strSplit :  l_vSplitInfo)
		{
			if (!strSplit.empty())
			{
				std::vector<std::string> l_vecSubSplit;
				m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
				if (l_iCorrectSize == l_vecSubSplit.size())
				{
					m_mapDeptCode[l_vecSubSplit.at(0)] = l_vecSubSplit.at(1);
				}
			}
		}
	}
	else
	{
		m_mapDeptCode["BMLX001"] = "DIC014013";//市局
		m_mapDeptCode["BMLX002"] = "DIC014001";//分局
		m_mapDeptCode["BMLX003"] = "DIC014002";//派出所
		m_mapDeptCode["BMLX004"] = "DIC014015";//内部机构
		m_mapDeptCode["BMLX005"] = "DIC014014";//直属单位
		m_mapDeptCode["BMLX006"] = "DIC014016";//机构领导
		m_mapDeptCode["BMLX007"] = "DIC014006";//联动单位
		m_mapDeptCode["BMLX008"] = "DIC014007";//警务站
		ICC_LOG_WARNING(m_pLog, "no dept type code mapping info found in config file,loading default mapping relation.")
	}
}

std::string ICC::CBusinessImpl::TransformDeptTypeCode(std::string p_strSMPDeptTypeCode)
{
	auto it = m_mapDeptCode.find(p_strSMPDeptTypeCode);
	if (it != m_mapDeptCode.end())
	{
		return it->second;
	}
	return "";
}

void CBusinessImpl::_SendSynDataSuccess()
{
	PROTOCOL::CSynBaseDataResultNotify l_oSynBaseDataSuccessNotify;
	l_oSynBaseDataSuccessNotify.m_oHeader.m_strCmd = "syndata_success_notify";
	l_oSynBaseDataSuccessNotify.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oSynBaseDataSuccessNotify.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oSynBaseDataSuccessNotify.m_oBody.m_strSynResult = "0";
	
	l_oSynBaseDataSuccessNotify.m_oHeader.m_strRequest = QUEUE_BASEDATA;
	l_oSynBaseDataSuccessNotify.m_oHeader.m_strRequestType = MQ_DEST_QUEUE;
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMsg = l_oSynBaseDataSuccessNotify.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::OnGetSynDataResult(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive get syndata result request");

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CSynBaseDataResultRequest l_getSynDataResultReq;
	if (!l_getSynDataResultReq.ParseString(p_pNotifiRequest->GetMessages(), l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "is not json: [%s]", p_pNotifiRequest->GetMessages().c_str());
		return;
	}

	PROTOCOL::CSynBaseDataResultNotify l_oSynBaseDataResult;
	l_oSynBaseDataResult.m_oHeader.m_strSystemID = SYSTEMID;
	l_oSynBaseDataResult.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oSynBaseDataResult.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oSynBaseDataResult.m_oHeader.m_strRelatedID = l_getSynDataResultReq.m_oHeader.m_strMsgid;
	l_oSynBaseDataResult.m_oHeader.m_strCmd = "get_syndata_result_respond";
	l_oSynBaseDataResult.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oSynBaseDataResult.m_oHeader.m_strRequest = l_getSynDataResultReq.m_oHeader.m_strResponse;
	l_oSynBaseDataResult.m_oHeader.m_strRequestType = l_getSynDataResultReq.m_oHeader.m_strResponseType;

	l_oSynBaseDataResult.m_oBody.m_strSynResult = m_bSynDataSuccess?"0":"false";
	

	std::string l_strMsg = l_oSynBaseDataResult.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotifiRequest->Response(l_strMsg);
	ICC_LOG_DEBUG(m_pLog, "respond get syn data result info: %s", l_strMsg.c_str());
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_RECEIVE_CHANGED_NOTIFY_DICT:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive dict info begin---!");
			_ProcReceiveDictInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive dict info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;	
	case THREADID_RECEIVE_CHANGED_NOTIFY_FUNC:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive func info begin---!");
			_ProcReceiveFuncInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive func info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	case THREADID_RECEIVE_CHANGED_NOTIFY_USER:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive user info begin---!");
			_ProcReceiveUserInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive user info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	case THREADID_RECEIVE_CHANGED_NOTIFY_STAFF:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive staff info begin---!");
			_ProcReceiveStaffInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive staff info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	case THREADID_RECEIVE_CHANGED_NOTIFY_ROLE:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive role info begin---!");
			_ProcReceiveSysRoleInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive role info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	case THREADID_RECEIVE_CHANGED_NOTIFY_DEPT:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "receive dept info begin---!");
			_ProcReceiveOrgnInfoNotify(msg_data.str_msg);
			ICC_LOG_INFO(m_pLog, "receive dept info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	case THREADID_REQUEST_USER_BIND_STAFF:
	{
		try
		{
			ICC_LOG_INFO(m_pLog, "request user staff bind info begin---!");
			_RequestUserStaffBindInfo();
			ICC_LOG_INFO(m_pLog, "request user staff bind info end---!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;	
	default:
		break;
	}
}

bool CBusinessImpl::_LoadBaseData()
{
	bool bLoad = false;

	/*std::vector<Data::CLanguageInfo> l_vecLanguage;
	Data::IBaseDataPtr l_pLanguage = boost::make_shared<Data::CLanguage>(l_vecLanguage, GetResourceManager());
	bLoad = l_pLanguage->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load language info failed!!!");
		return false;
	}*/

	std::vector<Data::CDeptInfo> l_vecDeptInfo;
	Data::IBaseDataPtr l_pDeptInfo = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());
	bLoad = l_pDeptInfo->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load dept info failed!!!");
		return false;
	}

	std::vector<Data::CStaffInfo> l_vecStaffInfo;
	Data::IBaseDataPtr l_pStaffInfo = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());
	bLoad = l_pStaffInfo->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load staff info failed!!!");
		return false;
	}

	std::vector<Data::CDictInfo> l_vecDict;	
	Data::IBaseDataPtr l_pDictInfo = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());
	bLoad = l_pDictInfo->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load dict info failed!!!");
		return false;
	}

	std::vector<Data::CUserInfo> l_vecUserInfo;
	Data::IBaseDataPtr l_pUser = boost::make_shared<Data::CUser>(l_vecUserInfo, GetResourceManager());
	bLoad = l_pUser->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load user info failed!!!");
		return false;
	}

	std::vector<Data::CAuthInfo> l_vecAuth;	
	Data::IBaseDataPtr l_pAuth = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());
	bLoad = l_pAuth->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load func info failed!!!");
		return false;
	}

	/*std::vector<Data::CBindInfo> l_vecBind;	
	Data::IBaseDataPtr l_pBind = boost::make_shared<Data::CBind>(l_vecBind, GetResourceManager(), "");
	bLoad = l_pBind->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load bind info failed!!!");
		return false;
	}*/

	std::vector<Data::CRoleInfo> l_vecRole;
	Data::IBaseDataPtr l_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());
	bLoad = l_pRole->LoadData();
	if (!bLoad)
	{
		ICC_LOG_ERROR(m_pLog, "load role info failed!!!");
		return false;
	}

	return true;

}

bool CBusinessImpl::_AnalyzeValidSynCmds(const std::string& strSynCmds)
{
	PROTOCOL::CSmpSynCmds syncmds;
	if (syncmds.ParseString(strSynCmds, ICCGetIJsonFactory()->CreateJson()))
	{
		m_mapValidSynCmds = syncmds.m_mapCmds;
		return true;
	}
	return false;
}

bool CBusinessImpl::_AnalyzeSynPackages(const std::string& strMessage, std::string& strPackageCount, std::string& strVersion)
{
	PROTOCOL::CSmpSynPackages packages;
	if (packages.ParseString(strMessage, ICCGetIJsonFactory()->CreateJson()))
	{
		strPackageCount = packages.m_strPackageCount;
		strVersion = packages.m_strMaxVersion;
		return true;
	}
	return false;
}

bool CBusinessImpl::_AnalyzeSmpDataChangedNotify(const std::string& strMessage)
{
	PROTOCOL::CSmpDataChangedNotify dataChangedNotify;
	if (dataChangedNotify.ParseString(strMessage, ICCGetIJsonFactory()->CreateJson()))
	{
		return true;
	}
	return false;
}

std::string CBusinessImpl::_ReadTmpResults(const std::string& strFile)
{
	char szTmp[128 * 1024] = { 0 };
	std::ifstream iFile;
	iFile.open(strFile.c_str(), std::ios_base::binary);
	if (iFile.is_open())
	{
		iFile.read(szTmp, 128 * 1024);
	}
	iFile.close();
	return szTmp;
}

bool CBusinessImpl::_LogoutSmp(const std::string& strToken)
{
	return true;

	std::string strContent;
	std::map<std::string, std::string> mapHeaders;	
	mapHeaders.insert(std::make_pair("Authorization", strToken));
	std::string strErrorMessage;
	std::string strTarget = "/auth/logout";
	std::string strReceive = m_pHttpClient->PostWithTimeout(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());

	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());
	}

	return true;
}

bool CBusinessImpl::_LoginSmp(std::string& strToken)
{
	return true;

	PROTOCOL::CSmpLogin login;
	login.m_strUser = m_strUser;
	login.m_strPassword = m_strPassword;
	login.m_strLoginType = m_strLoginType;
	login.m_strSystemCode = m_strSystemCode;

	std::string strContent = login.ToString(ICCGetIJsonFactory()->CreateJson());
	if (strContent.empty())
	{
		ICC_LOG_ERROR(m_pLog, "packet login info failed!!!!");
		return false;
	}
	
	std::map<std::string, std::string> mapHeaders;
	std::string strTarget = "/auth/login";
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->PostWithTimeout(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, strContent, strErrorMessage, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "/auth/login receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "/auth/login receive [%s]", strReceive.c_str());

	PROTOCOL::CSmpLoginResult loginResult;
	if (!loginResult.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse smp login response failed!!!!");
		return false;
	}

	if (loginResult.m_strCode != CODE_SUCCESS)
	{
		ICC_LOG_ERROR(m_pLog, "smp login failed!!!! code[%s : %s]", loginResult.m_strCode.c_str(), loginResult.m_strMessage.c_str());
		return false;
	}

	strToken = loginResult.m_strToken;

	return true;
}

int CBusinessImpl::_ProcSynData()
{	
	m_pOrg = nullptr;
	m_pStaff = nullptr;
	m_pDict = nullptr;
	m_pFunc = nullptr;
	m_pUser = nullptr;	
	m_pRole = nullptr;	

	std::vector<Data::CDeptInfo> l_vecDeptInfo;
	m_pOrg = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());

	std::vector<Data::CStaffInfo> l_vecStaffInfo;
	m_pStaff = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());

	std::vector<Data::CDictInfo> l_vecDict;
	m_pDict = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());

	std::vector<Data::CUserInfo> l_vecUserInfo;
	m_pUser = boost::make_shared<Data::CUser>(l_vecUserInfo, GetResourceManager());

	std::vector<Data::CAuthInfo> l_vecAuth;
	m_pFunc = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());	

	std::vector<Data::CRoleInfo> l_vecRole;
	m_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());	

	

	/*std::map<std::string, std::string> mapHeaders;
	std::string strTarget = "/auth/login";
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->PostEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, strContent, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "/auth/login receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return -1;
	}

	ICC_LOG_DEBUG(m_pLog, "/auth/login receive [%s]", strReceive.c_str());

	PROTOCOL::CSmpLoginResult loginResult;
	if (!loginResult.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse smp login response failed!!!!");
		return -1;
	}

	if (loginResult.m_strCode != CODE_SUCCESS)
	{
		ICC_LOG_ERROR(m_pLog, "smp login failed!!!! code[%s : %s]", loginResult.m_strCode.c_str(), loginResult.m_strMessage.c_str());
		return -1;
	}*/

	ICC_LOG_DEBUG(m_pLog, "_ProcSynData enter!!!!");
	ICC_LOG_DEBUG(m_pLog, "nacos server address: %s:%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());	
	ICC_LOG_DEBUG(m_pLog, "smp service info: servicename:%s, healthyflag:%s", m_strSmpServiceName.c_str(), m_strSmpServiceHealthyFlag.c_str());

	if (!_QuerySmpService())
	{
		return -1;
	}

	ICC_LOG_DEBUG(m_pLog, "will syn data, smp service info [%s:%s]", m_strSmpServerIp.c_str(), m_strSmpPort.c_str());

	std::map<std::string, std::string> mapHeaders;
	std::string strTarget;
	std::string strErrorMessage;
	std::string strReceive;

	std::string strToken;
	if (!_LoginSmp(strToken))
	{
		ICC_LOG_ERROR(m_pLog, "login smp failed!!!!");
		return -1;
	}

	mapHeaders.insert(std::make_pair("Authorization", strToken));

	strTarget = "/sync/smp/cmds";
	strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "/sync/smp/cmds receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return -1;
	}

	ICC_LOG_DEBUG(m_pLog, "/sync/smp/cmds receive [%s]", strReceive.c_str());

	PROTOCOL::CSmpSynCmds syncmds;
	if (!syncmds.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse /sync/smp/cmds login response failed!!!!");
		return -1;
	}

	if (syncmds.m_strCode != CODE_SUCCESS)
	{
		ICC_LOG_ERROR(m_pLog, "/sync/smp/cmds failed!!!! code[%s : %s]", syncmds.m_strCode.c_str(), syncmds.m_strMessage.c_str());
		return -1;
	}

	int iCmdCount = m_vecSmpSynCmds.size();
	for (int i = 0; i < iCmdCount; ++i)
	{
		strTarget = m_pString->Format("/sync/smp/packages?cmd=%s&step=%d&version=0", m_vecSmpSynCmds[i].c_str(), m_iPackageSize);

		std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
		long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();	
		std::string strTmpTarget = strTarget;

		strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
			return -1;
		}

		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());

		PROTOCOL::CSmpSynPackages packages;
		if (!packages.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", strTarget.c_str());
			return -1;
		}

		if (packages.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", strTarget.c_str(), packages.m_strCode.c_str(), packages.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", strTarget.c_str(), packages.m_strCode.c_str(), packages.m_strMessage.c_str());
			return -1;
		}

		if (packages.m_strPackageCount.empty())
		{
			ICC_LOG_ERROR(m_pLog, " %s package count is empty!!!!", strTarget.c_str());
			return -1;
		}

		int iPackageCount = std::stoi(packages.m_strPackageCount);
		for (int m = 0; m < iPackageCount; ++m)
		{
			bool bIsLast = false;
			if (m == iPackageCount - 1)
			{
				bIsLast = true;
			}

			strTarget = m_pString->Format("/sync/smp/datas?cmd=%s&step=%d&version=0&currentPackage=%d", m_vecSmpSynCmds[i].c_str(), m_iPackageSize, m + 1);
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return -1;
			}

			ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());

			PROTOCOL::CSmpSynDatas synDatas;
			if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", strTarget.c_str());
				return -1;
			}

			if (synDatas.m_strCode != CODE_SUCCESS)
			{
				if (m_strIgnoreHttpError == "1")
				{
					ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
					continue;
				}
				ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				return -1;
			}

			std::map<std::string, SmpDataAnalyze>::const_iterator itr_const;
			itr_const = m_mapSmpDataAnalyzers.find(m_vecSmpSynCmds[i]);
			if (itr_const != m_mapSmpDataAnalyzers.end())
			{
				int iRs = (this->*itr_const->second)(synDatas.m_strData, bIsLast);
				if (iRs != SYNDATA_SUCCESS && iRs != SYNDATA_WAITNEXTDATA)
				{
					ICC_LOG_ERROR(m_pLog, "%s analyze failed!!! content[%s]", m_vecSmpSynCmds[i].c_str(), synDatas.m_strData.c_str());
					return -1;
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "not find analyzer:[%s]", m_vecSmpSynCmds[i].c_str());
			}
		}		

		std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
		long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

		ICC_LOG_DEBUG(m_pLog, "%s syn complete, time[%lld]", strTmpTarget.c_str(), ullEndTime - ullBeginTime);
	}

	if (m_strSyncProcessSeatFlag == "1")
	{
		int iResult = _ProcSynDataEx();
		if (iResult != 0)
		{
			return iResult;
		}
	}	

	_LogoutSmp(strToken);

	/*strTarget = "/auth/logout";
	strReceive = m_pHttpClient->PostEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(),strErrorMessage.c_str());
		
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());
	}	*/

	return 0;
}

int CBusinessImpl::_ProcSynDataEx()
{

	/*m_pOrg = nullptr;
	m_pStaff = nullptr;
	m_pDict = nullptr;
	m_pFunc = nullptr;
	m_pUser = nullptr;
	m_pRole = nullptr;

	std::vector<Data::CDeptInfo> l_vecDeptInfo;
	m_pOrg = boost::make_shared<Data::CDepartment>(l_vecDeptInfo, GetResourceManager());

	std::vector<Data::CStaffInfo> l_vecStaffInfo;
	m_pStaff = boost::make_shared<Data::CStaff>(l_vecStaffInfo, GetResourceManager());

	std::vector<Data::CDictInfo> l_vecDict;
	m_pDict = boost::make_shared<Data::CDictionary>(l_vecDict, GetResourceManager());

	std::vector<Data::CUserInfo> l_vecUserInfo;
	m_pUser = boost::make_shared<Data::CUser>(l_vecUserInfo, GetResourceManager());

	std::vector<Data::CAuthInfo> l_vecAuth;
	m_pFunc = boost::make_shared<Data::CAuth>(l_vecAuth, GetResourceManager());

	std::vector<Data::CRoleInfo> l_vecRole;
	m_pRole = boost::make_shared<Data::CRole>(l_vecRole, GetResourceManager());*/
	

	std::map<std::string, std::string> mapHeaders;
	std::string strTarget;
	std::string strErrorMessage;
	std::string strReceive;

	std::string strToken;	

	mapHeaders.insert(std::make_pair("Authorization", strToken));	

	int iCmdCount = m_vecSmpSynCmdsEx.size();
	for (int i = 0; i < iCmdCount; ++i)
	{
		strTarget = m_pString->Format("/basic/%s/list", m_vecSmpSynCmdsEx[i].c_str());

		std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
		long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();
		std::string strTmpTarget = strTarget;

		strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
			return -1;
		}

		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());

		PROTOCOL::CSmpSynListResult packages;
		if (!packages.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", strTarget.c_str());
			return -1;
		}

		if (packages.m_strCode != CODE_SUCCESS)
		{
			if (m_strIgnoreHttpError == "1")
			{
				ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", strTarget.c_str(), packages.m_strCode.c_str(), packages.m_strMessage.c_str());
				continue;
			}
			ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", strTarget.c_str(), packages.m_strCode.c_str(), packages.m_strMessage.c_str());
			return -1;
		}

		Data::IBasicDataManagerPtr pManager = nullptr;
		std::map<std::string, Data::IBasicDataManagerPtr>::const_iterator itr;
		itr = m_mapSmpDataAnalyzersEx.find(m_vecSmpSynCmdsEx[i]);
		if (itr != m_mapSmpDataAnalyzersEx.end())
		{
			itr->second->AppendData(packages.vecDatas);
			
			if (packages.m_strCurrent.empty() || packages.m_strCurrent == packages.m_strTotalPage)
			{
				if (!itr->second->AnalyzeData())
				{
					ICC_LOG_ERROR(m_pLog, "parse %s response data failed!!!!", strTarget.c_str());
					return -1;
				}				
			}
			
		}
		

		/*if (packages.m_strPackageCount.empty())
		{
			ICC_LOG_ERROR(m_pLog, " %s package count is empty!!!!", strTarget.c_str());
			return -1;
		}

		int iPackageCount = std::stoi(packages.m_strPackageCount);
		for (int m = 0; m < iPackageCount; ++m)
		{
			bool bIsLast = false;
			if (m == iPackageCount - 1)
			{
				bIsLast = true;
			}

			strTarget = m_pString->Format("/sync/smp/datas?cmd=%s&step=%d&version=0&currentPackage=%d", m_vecSmpSynCmds[i].c_str(), m_iPackageSize, m + 1);
			strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", strTarget.c_str(), strErrorMessage.c_str());
				return -1;
			}

			ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", strTarget.c_str(), strReceive.c_str());

			PROTOCOL::CSmpSynDatas synDatas;
			if (!synDatas.ParseString(strReceive, ICCGetIJsonFactory()->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse %s response failed!!!!", strTarget.c_str());
				return -1;
			}

			if (synDatas.m_strCode != CODE_SUCCESS)
			{
				if (m_strIgnoreHttpError == "1")
				{
					ICC_LOG_WARNING(m_pLog, "ignore %s failed!!!! code[%s : %s]", strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
					continue;
				}
				ICC_LOG_ERROR(m_pLog, "%s failed!!!! code[%s : %s]", strTarget.c_str(), synDatas.m_strCode.c_str(), synDatas.m_strMessage.c_str());
				return -1;
			}

			std::map<std::string, SmpDataAnalyze>::const_iterator itr_const;
			itr_const = m_mapSmpDataAnalyzers.find(m_vecSmpSynCmds[i]);
			if (itr_const != m_mapSmpDataAnalyzers.end())
			{
				int iRs = (this->*itr_const->second)(synDatas.m_strData, bIsLast);
				if (iRs != SYNDATA_SUCCESS && iRs != SYNDATA_WAITNEXTDATA)
				{
					ICC_LOG_ERROR(m_pLog, "%s analyze failed!!! content[%s]", m_vecSmpSynCmds[i].c_str(), synDatas.m_strData.c_str());
					return -1;
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "not find analyzer:[%s]", m_vecSmpSynCmds[i].c_str());
			}
		}*/

		std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
		long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

		ICC_LOG_DEBUG(m_pLog, "%s syn complete, time[%lld]", strTmpTarget.c_str(), ullEndTime - ullBeginTime);
	}

	return 0;
}

void CBusinessImpl::_SynData()
{
	if (m_bIsSyning)
	{
		ICC_LOG_WARNING(m_pLog, "is syning!!!");
		return;
	}

	{
		boost::lock_guard<boost::mutex> lock(m_mutexSyning);
		if (m_bIsSyning)
		{
			ICC_LOG_WARNING(m_pLog, "is syning!!!");
			return;
		}
		m_bIsSyning = true;
	}

	ICC_LOG_DEBUG(m_pLog, "syn data begin!!!!!!!");

	if(!_LoadBaseData())
	{
		ICC_LOG_ERROR(m_pLog, "smp load base data failed!!");
		return;
	}


	std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
	long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();

	int iRs = _ProcSynData();
	if (iRs == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "syn data success!!!!!!!");
		_SendSynDataSuccess();
		m_bSynDataSuccess = true;
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "syn data failed!!!!!!!");
		m_bSynDataSuccess = false;
	}

	m_bIsSyning = false;

	std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
	long long ullEndTime = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

	ICC_LOG_DEBUG(m_pLog, "syn data end!!!!!!! time[%lld]", ullEndTime - ullBeginTime);
}

void CBusinessImpl::_InitDataAnalyzers()
{
	m_vecSmpSynCmds.push_back("smpDict");
	m_vecSmpSynCmds.push_back("smpFunction");
	m_vecSmpSynCmds.push_back("smpRole");
	m_vecSmpSynCmds.push_back("smpUser");
	m_vecSmpSynCmds.push_back("smpStaff");
	m_vecSmpSynCmds.push_back("smpOrg");

	m_mapSmpDataAnalyzers.insert(std::make_pair("smpFunction", &CBusinessImpl::_ProcReceiveFuncInfo));
	m_mapSmpDataAnalyzers.insert(std::make_pair("smpRole", &CBusinessImpl::_ProcReceiveSysRoleInfo));
	m_mapSmpDataAnalyzers.insert(std::make_pair("smpUser", &CBusinessImpl::_ProcReceiveUserInfo));
	m_mapSmpDataAnalyzers.insert(std::make_pair("smpDict", &CBusinessImpl::_ProcReceiveDictInfo));
	m_mapSmpDataAnalyzers.insert(std::make_pair("smpStaff", &CBusinessImpl::_ProcReceiveStaffInfo));
	m_mapSmpDataAnalyzers.insert(std::make_pair("smpOrg", &CBusinessImpl::_ProcReceiveOrgnInfo));
	

	m_vecSmpSynCmdsEx.push_back("seats");
	ICC::Data::IBasicDataManagerPtr pProcessSeats = boost::make_shared<Data::CProcessSeatManager>(m_pLog, m_pString, m_pDBConn, ICCGetIJsonFactory(), m_pDateTime);
	if (pProcessSeats)
	{
		m_mapSmpDataAnalyzersEx.insert(std::make_pair("seats", pProcessSeats));
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "add process seat processer failed!!!!!");
	}
}


void CBusinessImpl::_TransSmpOrgToIcc(const PROTOCOL::SmpDataOrg& org, Data::CDeptInfo& deptInfo)
{
	deptInfo.m_strGuid = org.m_strGuid;
	deptInfo.m_strParentGuid = org.m_strParentGuid;
	deptInfo.m_strCode = org.m_strOrgGovCode;
	deptInfo.m_strDistrictCode = org.m_strDistrictCode;
	deptInfo.m_strType = org.m_strOrgType;
	deptInfo.m_strName = org.m_strOrgName;
	deptInfo.m_strPhone = org.m_strOrgContactNo;
	deptInfo.m_strShortcut = org.m_strOrgShortName;
	deptInfo.m_strSort = org.m_strSort;
	deptInfo.m_strPucOrgIdentifier = org.m_strPucOrgIdentifier;
}

void CBusinessImpl::_TransSmpStaffToIcc(const PROTOCOL::SmpDataStaff& staff, Data::CStaffInfo& staffInfo)
{
	staffInfo.m_strGuid = staff.m_strGuid;
	if (staff.m_strbusinessOrgGuid.empty() || staff.m_strbusinessOrgGuid == "null")
	{
		staffInfo.m_strDeptGuid = staff.m_strBelongOrgGuid;
	}
	else
	{
		staffInfo.m_strDeptGuid = staff.m_strbusinessOrgGuid;
	}
	staffInfo.m_strCode = staff.m_strStaffCode;
	staffInfo.m_strName = staff.m_strStaffName;
	staffInfo.m_strSex = staff.m_strStaffSex;
	staffInfo.m_strType = staff.m_strStaffType;
	staffInfo.m_strPosition = staff.m_strStaffPosition;
	staffInfo.m_strMobile = staff.m_strStaffMobile;
	staffInfo.m_strPhone = staff.m_strTelephone;
	staffInfo.m_strSort = staff.m_strSort;
	staffInfo.m_strIsLeader = staff.m_strIsLeader;
	staffInfo.m_strOrgIdentifier = staff.m_strPucOrgIdentifier;
	staffInfo.m_strStaffIdNo = staff.m_strStaffIdNo;
}

void CBusinessImpl::_TransSmpDictToIcc(const PROTOCOL::SmpDataDict& dict, Data::CDictInfo& dictInfo)
{
	dictInfo.m_strGuid = dict.m_strGuid;
	dictInfo.m_strParentKey = dict.m_strParentDictKey;
	dictInfo.m_strKey = dict.m_strDictKey;
	dictInfo.m_strSort = dict.m_strSort;
	dictInfo.m_strValue = dict.m_strValue;
	dictInfo.m_strSort = dict.m_strSort;
	dictInfo.m_strDictType = dict.m_strDictType;
	dictInfo.m_strDictCode = dict.m_strDictCode;
	dictInfo.m_strLanguage = dict.m_strLanguage;
	dictInfo.m_strUnionKey = dict.m_strUnionKey;
	dictInfo.m_strParentUnionKey = dict.m_strParentUnionKey;
	dictInfo.m_strDictValueJson = dict.m_strDictValueJson;
}

void CBusinessImpl::_TransSmpFuncToIcc(const PROTOCOL::SmpDataFunc& func, Data::CAuthInfo& funcInfo)
{
	funcInfo.m_strGuid = func.m_strGuid;
	funcInfo.m_strCode = func.m_strFunctionCode;
	funcInfo.m_strName = func.m_strFunctionName;
	funcInfo.m_strParentGuid = func.m_strFunctionParentGuid;
	funcInfo.m_strSort = func.m_strSort;
}

void CBusinessImpl::_TransSmpUserToIcc(const PROTOCOL::SmpDataUser& user, Data::CUserInfo& userInfo)
{
	userInfo.m_strGuid = user.m_strGuid;
	userInfo.m_strName = user.m_strUserName;
	userInfo.m_strPwd = user.m_strUserPassword;
	userInfo.m_strOnlineStatus = user.m_strOnlineStatus;
	userInfo.m_strLoginTime = user.m_strLoginTime;
	userInfo.m_strUserType = user.m_strUserType;
	userInfo.m_strStaffGuid = user.m_strStaffGuid;
	userInfo.m_strOrgGuid = user.m_strBelongOrgGuid;
	userInfo.m_strCreateUser = user.m_strCreateUser;
	userInfo.m_strCreateTime = user.m_strCreateTime;
	userInfo.m_strUpdateUser = user.m_strUpdateUser;
	userInfo.m_strUpdateTime = user.m_strUpdateTime;
	userInfo.m_strRemark = user.m_strRemark;
}

void CBusinessImpl::_TransSmpRoleToIcc(const PROTOCOL::SmpDataRole& role, Data::CRoleInfo& roleInfo)
{
	roleInfo.m_strGuid = role.m_strGuid;
	roleInfo.m_strCode = role.m_strRoleCode;
	roleInfo.m_strName = role.m_strRoleName;
	roleInfo.m_strOrgGuid = role.m_strBelongOrgGuid;
}

void CBusinessImpl::_SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosNamespace = strNameSpace;
	m_strNacosGroupName = strGroupName;
}

void CBusinessImpl::_GetNacosParams(std::string& strNacosIp, std::string& strNacosPort, std::string& strNameSpace, std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	strNacosIp = m_strNacosServerIp;
	strNacosPort = m_strNacosServerPort;
	strNameSpace = m_strNacosNamespace;
	strGroupName = m_strNacosGroupName;
}

bool CBusinessImpl::_QuerySmpService()
{
	std::string strNacosIp;
	std::string strNacosPort;
	std::string strNameSpace;
	std::string strGroupName;

	_GetNacosParams(strNacosIp, strNacosPort, strNameSpace, strGroupName);

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), strNameSpace.c_str(),
		strGroupName.c_str(), m_strSmpServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->GetEx(strNacosIp, strNacosPort, strTarget, mapHeaders, strContent, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response. ");
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
		if (strHealthy == m_strSmpServiceHealthyFlag)
		{
			m_strSmpServerIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			m_strSmpPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy smp service : [%s:%s]. ", m_strSmpServerIp.c_str(), m_strSmpPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy smp service!!!");

	return false;
}

#define DICT_SYNC "dict_sync"
void CBusinessImpl::_SyncDictUpdateToClient()
{
	PROTOCOL::CDictSync l_CDictSync;
	l_CDictSync.m_oHeader.m_strSystemID = "ICC";
	l_CDictSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_CDictSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CDictSync.m_oHeader.m_strRelatedID = "";
	l_CDictSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CDictSync.m_oHeader.m_strCmd = DICT_SYNC;
	l_CDictSync.m_oHeader.m_strRequest = "topic_basedata_sync";
	l_CDictSync.m_oHeader.m_strRequestType = "1";
	l_CDictSync.m_oHeader.m_strResponse = "";
	l_CDictSync.m_oHeader.m_strResponseType = "";	

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_CDictSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}


bool CBusinessImpl::_ProcReceiveSeatNotify(const std::string& strMessage)
{
	std::vector<DataNotify> vecNotifyDatas;
	if (!_AnalyzeNotify(strMessage, vecNotifyDatas))
	{
		ICC_LOG_DEBUG(m_pLog, "_AnalyzeNotify failed:[%s]", strMessage.c_str());
		return false;
	}

	int iCount = vecNotifyDatas.size();
	for (size_t i = 0; i < iCount; i++)
	{
		std::string strResult = _GetNotifyData(vecNotifyDatas[i]);
		if (strResult.empty())
		{
			ICC_LOG_ERROR(m_pLog, "_GetNotifyData result is empty:[%s]", vecNotifyDatas[i].strDataGuid.c_str());
			return false;
		}

		PROTOCOL::CSmpSynListResult result;
		if (!result.ParseStringEx(strResult, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "ParseStringEx failed:[%s]", strResult.c_str());
			return false;
		}

		if (result.m_strCode != CODE_SUCCESS)
		{
			ICC_LOG_ERROR(m_pLog, "response result is error:[%s]", result.m_strCode.c_str());
			return false;
		}

		ICC::Data::IBasicDataManagerPtr pBasicManager = _CreateIBasicDataManager(vecNotifyDatas[i].strCmd);
		if (!pBasicManager)
		{
			ICC_LOG_ERROR(m_pLog, "create basic manager failed: cmd[%s]", vecNotifyDatas[i].strCmd.c_str());
			return false;
		}

		std::map<std::string, Data::IBasicDataPtr> mapDatas;
		if (!pBasicManager->_ParseData(result.m_strData, mapDatas))
		{
			ICC_LOG_ERROR(m_pLog, "_ParseData data failed!! data[%s]", result.m_strData.c_str());
			return false;
		}

		std::vector< Data::IBasicDataPtr> vecDatas;
		std::map<std::string, Data::IBasicDataPtr>::const_iterator itr;
		for (itr = mapDatas.begin(); itr != mapDatas.end(); ++itr)
		{
			vecDatas.push_back(itr->second);
		}

		if (vecNotifyDatas[i].strAction == "i")
		{
			if (!pBasicManager->_AddData(vecDatas))
			{
				ICC_LOG_ERROR(m_pLog, "_AddData failed!!");
				return false;
			}
		}
		else if (vecNotifyDatas[i].strAction == "u")
		{
			if (!pBasicManager->_UpdateData(vecDatas))
			{
				ICC_LOG_ERROR(m_pLog, "_UpdateData failed!!");
				return false;
			}
		}
		else if (vecNotifyDatas[i].strAction == "d")
		{
			if (!pBasicManager->_DeleteData(vecDatas))
			{
				ICC_LOG_ERROR(m_pLog, "_DeleteData failed!!");
				return false;
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "unknown action!![%s]", vecNotifyDatas[i].strAction.c_str());
		}
		
	}

	return true;
}

bool CBusinessImpl::_AnalyzeNotify(const std::string& strMessage, std::vector<DataNotify>& notifyDatas)
{
	ICC_LOG_DEBUG(m_pLog, "analyze notify  begin!![%s]", strMessage.c_str());
	
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (nullptr == pJson)
	{
		return false;
	}

	int iCount = pJson->GetCount("/body/dataList");
	for (int i = 0; i < iCount; ++i)
	{
		DataNotify data;

		std::string l_strPrefixPath("/body/dataList/" + std::to_string(i));

		data.strAction = pJson->GetNodeValue(l_strPrefixPath + "/action", "");
		data.strCallback = pJson->GetNodeValue(l_strPrefixPath + "/callback", "");
		data.strCmd = pJson->GetNodeValue(l_strPrefixPath + "/cmd", "");
		data.strDataGuid = pJson->GetNodeValue(l_strPrefixPath + "/dataGuid", "");

		notifyDatas.push_back(data);
	}

	return true;
}

std::string CBusinessImpl::_GetNotifyData(const DataNotify& notifyData)
{
	std::map<std::string, std::string> mapHeaders;
	std::string strTarget;
	std::string strErrorMessage;
	std::string strReceive;	

	strTarget = _PacketTarget(notifyData.strCmd, notifyData.strCallback, notifyData.strDataGuid);
	strReceive = m_pHttpClient->GetEx(m_strSmpServerIp, m_strSmpPort, strTarget, mapHeaders, "", strErrorMessage);
	
	return strReceive;
}

Data::IBasicDataManagerPtr CBusinessImpl::_CreateIBasicDataManager(const std::string& strCmd)
{
	ICC::Data::IBasicDataManagerPtr pBasicManager = nullptr;
	if (strCmd == "smpSeats")
	{
		pBasicManager = boost::make_shared<Data::CProcessSeatManager>(m_pLog, m_pString, m_pDBConn, ICCGetIJsonFactory(), m_pDateTime);
	}

	return pBasicManager;
}

std::string CBusinessImpl::_PacketTarget(const std::string& strCmd, const std::string& strCallback, const std::string& strData)
{
	if (strCmd == "smpSeats")
	{
		return "/seats/info?orgIdentifier=" + strData;
	}
	return "";
}



