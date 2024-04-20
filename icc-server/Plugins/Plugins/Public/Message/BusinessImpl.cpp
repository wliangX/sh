#include "Boost.h"
#include "BusinessImpl.h"
#include <iostream>
#include <fstream>
#include "CHeartBeatData.h"

#ifndef _WIN32
#include <memory>
#include <thread>
#include <unistd.h>
#include "factory/NacosServiceFactory.h"
#include "ResourceGuard.h"
#include "naming/Instance.h"
#include "NacosString.h"
#include "Properties.h"
#include "constant/PropertyKeyConst.h"

using namespace nacos;
#endif

const unsigned long TIMERID_GETSERVICELOCK = 20001;
const unsigned long TIMERID_CHECKHTTPSERVER = 20002;
const unsigned long TIMERID_RELOADCONFIG = 20003;
const unsigned long TIMERID_INSPECTSHARDTABLE = 20004;

const unsigned long TIMERID_ACTIVEMQ_CHECK = 20005;
const unsigned long TIMERID_AMQ = 20006;
#define MESSAGE_TIMER_CMD_NAME "message_timer"
#define MAX_UNSIGNED_INT	0xfffffff
#define NacosParams "NacosParams"

#define REDIS_HEALTH "redis_health"

const unsigned long HEART_BEAT_MSG = 0x01;
const unsigned long THREADID_RELOADCONFIG = 30001;
const unsigned long THREADID_INSPECTSHARDTABLE = 30002;

const unsigned long THREADID_ACTIVEMQ_CHECK = 30003;

const unsigned long THREADID_SENDRESPONSE = 10000;
const unsigned long THREADID_AMQ = 10088;

class CReceiveCallback : public IAsyncCallback
{
public:
	CReceiveCallback(CBusinessImpl* p_MSQBusiness);
	virtual ~CReceiveCallback();
public:
	virtual void OnMessage(std::string p_strMsg, IResCallbackPtr p_callbackPtr = nullptr);
private:
	CBusinessImpl* m_pMSQBusiness;
};


//一次重入类
class crunprocessguard
{
public:
	crunprocessguard(bool* binprocess, bool oldval)
	{
		_binprocess = binprocess;
		boldval = oldval;
	}
	~crunprocessguard()
	{
		if (NULL != _binprocess && boldval == false)
			*_binprocess = false;
	}
	bool* _binprocess;
	bool boldval;
};


#define PROCESSGUARD	\
	bool oldval = m_bDisposeHeartMsg;\
	if (oldval)\
    {\
         if (NULL != p_pNotifiRequest.get()) \
		 { \
			 ICC_LOG_DEBUG(m_pLog, "It's do thing now, Msg: %s", p_pNotifiRequest->GetMessages().c_str());  \
		 }\
        return; \
	}\
	m_bDisposeHeartMsg = true;


#define PROCESSGUARD_EX	\
	bool oldval = m_bDisposeHeartMsg;\
	if (oldval)\
    {\
        return; \
	}\
	m_bDisposeHeartMsg = true;

CReceiveCallback::CReceiveCallback(CBusinessImpl* p_MSQBusiness) : m_pMSQBusiness(p_MSQBusiness)
{

}

CReceiveCallback::~CReceiveCallback()
{

}

void CReceiveCallback::OnMessage(std::string p_strMsg, IResCallbackPtr p_callbackPtr /* = nullptr */)
{
	m_pMSQBusiness->OnMessage(p_strMsg, p_callbackPtr);
}

//////////////////////////////////////////////////////////////////////////
CBusinessImpl::CBusinessImpl()
{
	m_strNeedNacos = "1";

	m_uiHeartBeatTime = 0;
	m_uiCountTimes = 0;

	m_uiHeartBeatCount = 0;
	m_bNacosWorkCheck = true;
	m_bDisposeHeartMsg = false; //默认未在处理心跳

	m_respondHttpCount = 0;
	m_uIndex = 0;

	m_nActiveCheckCount = 0;
	m_nActiveCheckMaxValue = 3;

}

CBusinessImpl::~CBusinessImpl()
{

}

void CBusinessImpl::OnInit()
{
	printf("message plugin init begin\n");
	m_pString = ICCGetIStringFactory()->CreateString();
	//printf("message plugin init stringutil success!\n");
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	//printf("message plugin init datetime success!\n");
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	//printf("message plugin init config success!\n");
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	//printf("message plugin init staticconfig success!\n");
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	//m_pConsumer = ICCGetAmqClient()->CreateAsyncConsumer();
	//m_pProducer = ICCGetAmqClient()->CreateProducer();
	//printf("message plugin init logger success!\n");
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);
	m_pObserverCenterList = ICCGetIObserverFactory()->GetObserverCenterList();

	m_pObserverCenteHealthActivemq = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE_HEALTHACTIVEMQ);

	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);


	//printf("message plugin init observercenter success!\n");
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	//printf("message plugin init timemgr success!\n");
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	//printf("message plugin init redisclient success!\n");
	m_pJsonFty = ICCGetIJsonFactory();
	//printf("message plugin init json success!\n");
	m_callbackMutex = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);
	//printf("message plugin init callbackmutex success!\n");
	m_notificationMutex = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);
	//printf("message plugin init notificationmutex success!\n");
	//m_pLicense = ICCGetLicenseFactory()->CreateLicense();
	//printf("message plugin init license success!\n");

	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();	

	m_bIsMaster = false;
	m_bStopNacos = false;
	m_bHealthActivemq = false;
	m_strStatus = "UP";
	m_strActivemqStatus = "DOWN";
	m_strpostgreSqlStatus = "DOWN";
	m_strRedisStatus = "DOWN";

	printf("message plugin init end\n");
}

bool CBusinessImpl::ServiceLock(std::string p_strApplicationID, std::string& p_strLockValue, std::string& p_strError)
{
	std::string l_strLockTTL = m_pConfig->GetValue("ICC/Plugin/Message/ServiceLockTTL", "15");
	int l_iLockTTL = m_pString->ToUInt(l_strLockTTL);
	return m_pRedisClient->ServiceLockEx(p_strApplicationID, p_strLockValue, l_iLockTTL * 1000, 3000, (!m_bIsMaster), p_strError);
}

void CBusinessImpl::OnStart()
{
/*
	ICC_LOG_DEBUG(m_pLog, "license info: enabletimelimit[%s] enablesuperdogcode[%s] enablemachinecode[%s] systemcode[%s] systemname[%s] superdogcode[%s] machinecode[%s] gendate[%s] vendercode[%s] validdays[%d] status[%s] clientnum[%d] validdate[%s] licenseDesp[%s]",
								m_pLicense->IsEnableTimeLimit() ? "true" : "false",
								m_pLicense->IsEnableSuperdogCode() ? "true" : "false",
								m_pLicense->IsEnableMachineCode() ? "true" : "false",
								m_pLicense->GetSystemCode().c_str(),
								m_pLicense->GetSystemName().c_str(),
								m_pLicense->GetSuperdogCode().c_str(),
								m_pLicense->GetMachineCode().c_str(),
								m_pLicense->GetGenDate().c_str(),
								m_pLicense->GetVenderCode().c_str(),
								m_pLicense->GetValidDays(),
								m_pLicense->GetStatus().c_str(),
								m_pLicense->GetClientNum(),
								m_pLicense->GetValidDate().c_str(),
								m_pLicense->GetLicenseDesp().c_str());*/
	printf("message plugin OnStart begin\n");

	m_strStartTime = m_pDateTime->CurrentLocalDateTimeStr();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, NOTIFI_SEND_REQUEST, OnNotifiSendRequest);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "health", OnNotifiHealthRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "health_activemq", OnNotifiHealthActivemqRequest);
	
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, MESSAGE_TIMER_CMD_NAME, OnTimer);
	
	m_strbrokerURI = m_pConfig->GetValue("ICC/Component/AmqClient/BrokerURI", "failover:(tcp://127.0.0.1:22000)");
	m_strClientID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "ICC");
	m_strApplicationID = m_strClientID;
	std::string strClientGuid = m_pString->CreateGuid();
	m_strClientID += "." + strClientGuid;

	//m_pConfig->SetServerFlag(m_strClientID);

	m_strUserName = m_pConfig->GetValue("ICC/Component/AmqClient/UserName", "admin");
	m_strPassword = m_pConfig->GetValue("ICC/Component/AmqClient/Password", "admin");
	m_strTopics = m_pStaticConfig->GetValue("ICC/Component/AmqClient/Topics", "");
	m_strQueues = m_pStaticConfig->GetValue("ICC/Component/AmqClient/Queues", "");

	m_strShardCheckTime = m_pStaticConfig->GetValue("ICC/Plugin/Message/ShardCheckTime", "1");
	m_strShardUpdateTime = m_pStaticConfig->GetValue("ICC/Plugin/Message/ShardUpdateTime", "23");

	m_strHttpLocalAddress = m_pConfig->GetValue("ICC/Component/HttpServer/LocalAddress", "0.0.0.0");
	m_strHttpLocalPort = m_pConfig->GetValue("ICC/Component/HttpServer/LocalPort", "29090");

	m_strNacosLocalIP = m_pConfig->GetValue("ICC/Component/HttpServer/NacosLocalIP", ""); //不使用
	m_strNacosServerIp = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerIp", "127.0.0.1");
	m_strNacosServerPort = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServerPort", "8848");
	m_strNacosNamespace = m_pConfig->GetValue("ICC/Component/HttpServer/NacosNamespace", "dev");
	m_strNacosServiceName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosServiceName", "icc-incident-service");
	m_strNacosGroupName = m_pConfig->GetValue("ICC/Component/HttpServer/NacosGroupName", "master");
	m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	m_strNacosRegistUrl = m_pConfig->GetValue("ICC/Component/HttpServer/registurl", "/nacos/v1/ns/instance");
	m_strNacosSuccessFlag = m_pConfig->GetValue("ICC/Component/HttpServer/successflag", "ok");	

	m_strNeedNacos = m_pConfig->GetValue("ICC/Component/HttpServer/NeedNacos", "1");

	std::string l_strNacosWorkCheck = m_pConfig->GetValue("ICC/Component/HttpServer/NacosWorkCheck", "true");
	if (l_strNacosWorkCheck == "false")
	{
		m_bNacosWorkCheck = false;
	}
	//m_strClientID += "." + m_strNacosLocalIP;

	//如下参数注册到naocs中。
	/*# 删除IP时间
		spring.cloud.nacos.discovery.metadata.preserved.ip.delete.timeout = 129600000
		# 心跳间隔时间
		spring.cloud.nacos.discovery.metadata.preserved.heart.beat.interval = 5000
		# 心跳超时时间（15秒没收心跳信息变为不健康状态）
		spring.cloud.nacos.discovery.metadata.preserved.heart.beat.timeout = 15000
	
		spring.cloud.nacos.discovery.metadata.preserved.ip.delete.timeout=2592000000 这个时间建议设置为一个月其他不变*/
	m_oNacosMetaData.m_strBeatTimeOut = m_pConfig->GetValue("ICC/Component/HttpServer/NacosBeatTimeOut", "15000"); //单位ms
	m_oNacosMetaData.m_strDeleteTimeOut = m_pConfig->GetValue("ICC/Component/HttpServer/NacosIpDeleteTimeOut", "2592000000"); //单位ms
	m_oNacosMetaData.m_strBeatInterval = m_pConfig->GetValue("ICC/Component/HttpServer/NacosBeatInterval", "5000");  //单位ms

	m_uiHeartBeatTime = m_pString->ToInt(m_oNacosMetaData.m_strBeatInterval) / 1000; //得到秒

	std::string strCheckInterval = m_pConfig->GetValue("ICC/Component/HttpServer/checkinterval", "1");
	if (strCheckInterval.empty())
	{
		strCheckInterval = "60";
	}
	m_lCheckInterval = std::stol(strCheckInterval);
	m_lCheckInterval *= 1000;
	m_strCheckNum = 0;

	std::string l_strLockTimeOut = m_pConfig->GetValue("ICC/Plugin/Message/ServiceLockTimeOut", "5");
	m_bServiceState = false;

	if (m_strApplicationID == "ICC.Server.Alarm")
	{
		m_strSyncCmdList = m_pStaticConfig->GetValue("ICC/Component/Message/SyncCmdList", "add_alarm_request;add_or_update_alarm_and_process_request;add_or_update_process_request");
	}
	else
	{
		m_strSyncCmdList = m_pStaticConfig->GetValue("ICC/Component/Message/SyncCmdList", "");
	}
	
	if (!m_strSyncCmdList.empty())
	{
		InitSyncCmdList();
	}
	
	m_nTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Message/TimeOut", "5"));

	m_oWorkThread.Start();

	this->OnTimer(NULL);

	//修改定时器，不使用ICC平台的定时器
	//m_pTimerMgr->AddTimer(MESSAGE_TIMER_CMD_NAME, m_pString->ToUInt(l_strLockTimeOut), m_pString->ToUInt(l_strLockTimeOut));
	StartHttpServer();
	/*m_strServerLocalIP = _GetNacosLocalIP();

	if (!m_strServerLocalIP.empty())
	{
		m_strClientID += "." + m_strServerLocalIP;
		m_pConfig->SetServerFlag(m_strClientID);
	}*/
	m_timerServiceLock.AddTimer(this, TIMERID_GETSERVICELOCK, m_pString->ToUInt(l_strLockTimeOut) * 1000);
	//m_timerServiceLock.AddTimer(this, TIMERID_CHECKHTTPSERVER, m_pString->ToUInt(l_strLockTimeOut) * 5000);
	m_timerServiceLock.AddTimer(this, TIMERID_RELOADCONFIG, 60 * 1000);
	m_timerServiceLock.AddTimer(this, TIMERID_INSPECTSHARDTABLE, 60 * 1000);

	m_threadProcessNacos.reset(new boost::thread(boost::bind(&CBusinessImpl::_ProcessNacosCheck, this)));


	m_timerServiceLock.AddTimer(this, TIMERID_INSPECTSHARDTABLE, 60 * 1000);

	int l_nMQCheckFlag = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Message/ActiveMQCheckEnable", "1"));
	if (l_nMQCheckFlag == 1)
	{
		m_nActiveCheckMaxValue = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Message/ActiveMQCheckMaxValue", "3"));
		if (m_nActiveCheckMaxValue <= 0)
		{
			m_nActiveCheckMaxValue = 3;
		}
		m_timerServiceLock.AddTimer(this, TIMERID_ACTIVEMQ_CHECK, m_pString->ToUInt(l_strLockTimeOut) * 1000);
	}
	// Redis 服务健康状态检测
	m_pRedisClient->Del(REDIS_HEALTH);

	m_pRedisClient->HSet(REDIS_HEALTH, "10086", "10086");

	ICC_LOG_DEBUG(m_pLog, "message start success. guid[%s], localip: %s,NacosWorkCheck:%d", strClientGuid.c_str(), m_strServerLocalIP.c_str(), m_bNacosWorkCheck);
}

void CBusinessImpl::OnStop()
{
	if (m_bServiceState)
	{
		UnSubscribe();

		m_pConsumer.reset();
		m_pProducer.reset();
	}
	
	m_oWorkThread.Stop(10);

	m_pTimerMgr.reset();

	m_bStopNacos = true;
	ICC_LOG_DEBUG(m_pLog, "message stop success.");
}

void CBusinessImpl::OnDestroy()
{

}


void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	if (msg_data.msg_id == HEART_BEAT_MSG)
	{
		DoTimerEvent();
	}
	else if (msg_data.msg_id == THREADID_SENDRESPONSE)
	{
		try
		{
			ICC_LOG_DEBUG(m_pLog, "ProcMessage send respond begin: %s", msg_data.str_msg.c_str());
			_HttpRespond(msg_data.str_msg, msg_data.strExt);
			ICC_LOG_DEBUG(m_pLog, "ProcMessage send respond end: %s", msg_data.str_msg.c_str());
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "_ProcQueryCallInfo exception!");
		}
	}
    else if (msg_data.msg_id == THREADID_RELOADCONFIG)
	{
		OnReceiveReloadConfigNotify(nullptr);
	}
	else if (msg_data.msg_id == THREADID_INSPECTSHARDTABLE)
	{
		OnCreateShardTable();
	}
	else if (msg_data.msg_id == THREADID_ACTIVEMQ_CHECK)
	{
		OnPostActiveMQCheck();
	}
	else if (msg_data.msg_id == THREADID_AMQ)
	{
		healthActivemqRequest();
	}
}

void CBusinessImpl::OnPostActiveMQCheck()
{
	if (m_bServiceState)
	{
		if (m_nActiveCheckCount > m_nActiveCheckMaxValue)
		{
			ICC_LOG_ERROR(m_pLog, "ActiveMQ Check timeout,count:[%d],MaxValue:[%d]", m_nActiveCheckCount, m_nActiveCheckMaxValue);
			m_pConsumer.reset();
			m_pProducer.reset();
			if (!ConnectMQ())
			{
				ICC_LOG_ERROR(m_pLog, "message connect amq failed.");
				return;
			}
			
			UnSubscribe();

			if (m_bServiceState)
			{
				Subscribe();
			}

			m_nActiveCheckCount = 0;
			return;
		}

		std::string l_strMQCheckQueueName = m_pString->Format("queue_%s", m_strApplicationID.c_str());

		PROTOCOL::CSyncServerLock l_SyncRequest;
		l_SyncRequest.m_oHeader.m_strSystemID = "ICC";
		l_SyncRequest.m_oHeader.m_strSubsystemID = "Message";
		l_SyncRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_SyncRequest.m_oHeader.m_strCmd = "sync_activemq_check";
		l_SyncRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_SyncRequest.m_oHeader.m_strRequest = l_strMQCheckQueueName;
		l_SyncRequest.m_oHeader.m_strRequestType = "0";

		l_SyncRequest.m_oBody.m_strServiceID = m_strApplicationID;
		l_SyncRequest.m_oBody.m_strClientID = m_strClientID;
		std::string l_strMsg = l_SyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());

		//通知观察者中心
		//m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg), ObserverPattern::ENotifyType::Post);
		m_nActiveCheckCount++;

		ICC_LOG_DEBUG(m_pLog, "ActiveMQ Check,count:[%d],MaxValue:[%d],msg:%s", m_nActiveCheckCount, m_nActiveCheckMaxValue, l_strMsg.c_str());
	}
	else
	{
		m_nActiveCheckCount = 0;
	}
}

void CBusinessImpl::OnCreateShardTable()
{
	ICC_LOG_DEBUG(m_pLog, "OnCreateShardTable Start");
	//先判断是否为当月1号
	std::string p_strCurTime = m_pDateTime->CurrentDateTimeStr();
	std::string strDay;
	std::string strMin;
	strDay = p_strCurTime.substr(11, 2);
	strMin = p_strCurTime.substr(14, 2);

	if (m_strCheckNum == 1 && (strDay.compare(m_strShardUpdateTime) != 0 && strMin.compare("00") != 0))
	{
		ICC_LOG_DEBUG(m_pLog, "Create Shard Not Set Time, Set Time Is : %s, Curent Time Is : %s", m_strShardUpdateTime.c_str(), m_pDateTime->CurrentDateTimeStr().c_str());
		return;
	}

	m_strCheckNum = 1;
	
	//执行更新数据库脚本
	DataBase::IResultSetPtr l_sqlResult;
	l_sqlResult = m_pDBConn->Exec("select create_shardtable_icc_t_record_files_item() as icc_t_recordfilesitem, create_shardtable_icc_t_fkdb() as icc_t_fkdb, create_shardtable_icc_t_jjdb() as icc_t_jjdb, create_shardtable_icc_t_pjdb() as icc_t_pjdb, create_shardtable_icc_t_alarm_log() as icc_t_alarm_log, create_shardtable_icc_t_sub_callevent() as icc_t_sub_callevent, create_shardtable_icc_t_callevent() as icc_t_callevent;");
	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "select create_shardtable, Error: [%s]",
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success,SQL: [%s]",
			l_sqlResult->GetSQL().c_str());

		ICC_LOG_DEBUG(m_pLog, "Create Shard: icc_t_recordfilesitem: %s, icc_t_fkdb: %s, icc_t_jjdb: %s, icc_t_pjdb: %s, icc_t_alarm_log: %s, icc_t_sub_callevent: %s, icc_t_callevent: %s",
			l_sqlResult->GetValue("icc_t_recordfilesitem").c_str(),
			l_sqlResult->GetValue("icc_t_fkdb").c_str(),
			l_sqlResult->GetValue("icc_t_jjdb").c_str(),
			l_sqlResult->GetValue("icc_t_pjdb").c_str(),
			l_sqlResult->GetValue("icc_t_alarm_log").c_str(),
			l_sqlResult->GetValue("icc_t_sub_callevent").c_str(),
			l_sqlResult->GetValue("icc_t_callevent").c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Create Shard UnKnown Error, SQL: [%s]",
			l_sqlResult->GetSQL().c_str());
	}
}

void CBusinessImpl::DoTimerEvent()
{

	crunprocessguard tmp_oProccess(&m_bDisposeHeartMsg, false);

	PROTOCOL::CSyncServerLock l_SyncRequest;
	l_SyncRequest.m_oHeader.m_strSystemID = "ICC";
	l_SyncRequest.m_oHeader.m_strSubsystemID = "Message";
	l_SyncRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_SyncRequest.m_oHeader.m_strCmd = "sync_server_lock";
	l_SyncRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_SyncRequest.m_oHeader.m_strRequest = "topic_doubleserver";
	l_SyncRequest.m_oHeader.m_strRequestType = "1";

	l_SyncRequest.m_oBody.m_strServiceID = m_strApplicationID;
	l_SyncRequest.m_oBody.m_strClientID = m_strClientID;

	ICC_LOG_DEBUG(m_pLog, "[%s] will get service lock begin, start time: %s", m_strClientID.c_str(), m_strStartTime.c_str());

	std::string p_strError;
	//获取服务锁
	//如果需要Nacos，一定获取到本地IP才正常工作
	if ("1" == m_strNeedNacos)
	{
		if (m_strServerLocalIP.empty())
		{
			m_strServerLocalIP = _GetNacosLocalIP();

			if (!m_strServerLocalIP.empty())
			{
				m_strClientID += "_" + m_strServerLocalIP;
				m_pConfig->SetServerFlag(m_strClientID);
				l_SyncRequest.m_oBody.m_strClientID = m_strClientID;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "[%s] can not get server local ip, do not get msater flag", m_strClientID.c_str());
				return;
			}
		}

		if (m_bNacosWorkCheck)
		{
			m_uiHeartBeatCount++;
		}
	}
	else
	{
		if (m_pConfig->ServerFlag().empty())
		{
			m_pConfig->SetServerFlag(m_strClientID);
			ICC_LOG_DEBUG(m_pLog, "set server flag[%s]", m_strClientID.c_str());
		}
	}

	if (m_uiHeartBeatCount > 24)
	{
		ICC_LOG_DEBUG(m_pLog, "Nacos beatthread is blocking,exit app,ApplicationID:[%s]", m_strApplicationID.c_str());
		printf("Nacos beatthread is blocking,exit app,ApplicationID:[%s]", m_strApplicationID.c_str());
		exit(0);
	}


	if (ServiceLock(m_strApplicationID, m_strClientID, p_strError))
	{
		m_bIsMaster.store(true);
		ICC_LOG_DEBUG(m_pLog, "[%s] get service lock success, master flag is %s,HeartBeatCount:%d", m_strClientID.c_str(), m_bIsMaster ? "true" : "false", m_uiHeartBeatCount);
		if (!m_bServiceState)
		{
			if (!ConnectMQ())
			{
				m_pConsumer.reset();
				m_pProducer.reset();
				ICC_LOG_ERROR(m_pLog, "message connect amq failed.");
				return;
			}

			m_bServiceState = true;
			Subscribe();
			ICC_LOG_DEBUG(m_pLog, "[%s] upgrade to master, subscribe", m_strClientID.c_str());
			l_SyncRequest.m_oBody.m_strServiceState = "master";

			std::string l_strMsg = l_SyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());
			//通知观察者中心
			//m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg), ObserverPattern::ENotifyType::Post);
			NotifyObserverList(boost::make_shared<CNotifiReceive>(l_strMsg, l_SyncRequest.m_oHeader.m_strMsgid));

			ICC_LOG_DEBUG(m_pLog, "sync service lock state: %s", l_strMsg.c_str());
		}


	}
	else
	{
		m_bIsMaster = false;
		ICC_LOG_DEBUG(m_pLog, "[%s] get service lock failed: %s, master flag: %s,HeartBeatCount:%d", m_strClientID.c_str(), p_strError.c_str(), m_bIsMaster ? "true" : "false", m_uiHeartBeatCount);
		if (m_bServiceState)
		{
			ICC_LOG_DEBUG(m_pLog, "[%s] get service lock failed, will switch service: %s", m_strClientID.c_str(), p_strError.c_str());
			m_bServiceState = false;
			UnSubscribe();
			ICC_LOG_DEBUG(m_pLog, "[%s] downgrade to slave, unsubscribe", m_strClientID.c_str());
			l_SyncRequest.m_oBody.m_strServiceState = "slave";

			std::string l_strMsg = l_SyncRequest.ToString(ICCGetIJsonFactory()->CreateJson());
			//通知观察者中心
			//m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg), ObserverPattern::ENotifyType::Post);
			NotifyObserverList(boost::make_shared<CNotifiReceive>(l_strMsg, l_SyncRequest.m_oHeader.m_strMsgid));

			ICC_LOG_DEBUG(m_pLog, "sync service lock state: %s", l_strMsg.c_str());

			m_pConsumer.reset();
			m_pProducer.reset();
		}
	}

	ICC_LOG_DEBUG(m_pLog, "[%s] will get service lock end", m_strClientID.c_str());
}

/*
void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//if (NULL != p_pNotifiRequest.get())
	//{
	//	ICC_LOG_DEBUG(m_pLog, "Msg: %s", p_pNotifiRequest->GetMessages().c_str());
	//}
	PROCESSGUARD;
	//不可重入
	m_oWorkThread.AddMessage(this, HEART_BEAT_MSG);
	//return;
	
}
*/

void CBusinessImpl::OnMessage(std::string p_strMsg, IResCallbackPtr p_callbackPtr)
{
	static unsigned int s_externalRecvCount_Queue = 0;
	static unsigned int s_externalRecvCount_Topic = 0;

	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	if (!l_JsonPtr->LoadJson(p_strMsg))
	{
		ICC_LOG_ERROR(m_pLog, "load json[%s] object failed.", p_strMsg.c_str());
		return;
	}

	std::string l_strMsgid = l_JsonPtr->GetNodeValue("/header/msgid", "");
	std::string l_strCmd = l_JsonPtr->GetNodeValue("/header/cmd", "");
	std::string l_strRequest = l_JsonPtr->GetNodeValue("/header/request", "");

	ICC_LOG_LOWDEBUG(m_pLog, "receive mq message: [%s]", p_strMsg.c_str());

	m_nActiveCheckCount = 0;
	if (l_strCmd == "sync_activemq_check")
	{
		ICC_LOG_DEBUG(m_pLog, "sta_client_request,activemq_check");
		return;
	}

	if (p_callbackPtr)
	{
		std::string l_strGuid = m_pString->CreateGuid();
		{
			Lock::AutoLock lock(m_callbackMutex);
			m_callbackMap[l_strGuid] = p_callbackPtr;
		}

		

		NotifyObserverList(boost::make_shared<CNotifiReceivePrivate>(this, p_strMsg, l_strGuid), l_strCmd);
		if (++s_externalRecvCount_Queue > MAX_UNSIGNED_INT) s_externalRecvCount_Queue = 0;

		ICC_LOG_DEBUG(m_pLog, "sta_client_request_push cmd[%s] queue[%d] with details cmd[%s:%s:%s:%s] count[%d]=queue[%d]+topic[%d] callbackMap[%d]",
			l_strCmd.c_str(), s_externalRecvCount_Queue,
			l_strCmd.c_str(), l_strMsgid.c_str(), p_callbackPtr ? "queue" : "topic", l_strRequest.c_str(),
			s_externalRecvCount_Queue + s_externalRecvCount_Topic, s_externalRecvCount_Queue, s_externalRecvCount_Topic, m_callbackMap.size());
	}
	else
	{
		NotifyObserverList(boost::make_shared<CNotifiReceive>(p_strMsg, l_strMsgid), l_strCmd);

		if (++s_externalRecvCount_Topic > MAX_UNSIGNED_INT) s_externalRecvCount_Topic = 0;

		ICC_LOG_DEBUG(m_pLog, "sta_client_sync_push cmd[%s] topic[%d] with details cmd[%s:%s:%s:%s] count[%d]=queue[%d]+topic[%d] callbackMap[%d]",
			l_strCmd.c_str(), s_externalRecvCount_Topic,
			l_strCmd.c_str(), l_strMsgid.c_str(), p_callbackPtr ? "queue" : "topic", l_strRequest.c_str(),
			s_externalRecvCount_Queue + s_externalRecvCount_Topic, s_externalRecvCount_Queue, s_externalRecvCount_Topic, m_callbackMap.size());
	}
}

bool CBusinessImpl::ConnectMQ()
{
	m_pConsumer = ICCGetAmqClient()->CreateAsyncConsumer();
	m_pProducer = ICCGetAmqClient()->CreateProducer();

	std::string strConsumerClientId = m_strClientID;
	if (!m_pConsumer->Connect(m_strbrokerURI, strConsumerClientId, m_strUserName, m_strPassword))
	{
		ICC_LOG_ERROR(m_pLog, "connect AMQ failed, brokerURI[%s], strClientID[%s], strUserName[%s], strPassword[%s]",
			m_strbrokerURI.c_str(), strConsumerClientId.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "consumer connect AMQ success, brokerURI[%s], strClientID[%s], strUserName[%s]",
		m_strbrokerURI.c_str(), m_strClientID.c_str(), m_strUserName.c_str());

	std::string strProducerClientId = m_strClientID;
	if (!m_pProducer->Connect(m_strbrokerURI, strProducerClientId, m_strUserName, m_strPassword))
	{
		ICC_LOG_ERROR(m_pLog, "connect AMQ failed, brokerURI[%s], strClientID[%s], strUserName[%s], strPassword[%s]",
			m_strbrokerURI.c_str(), strProducerClientId.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "producer connect AMQ success, brokerURI[%s], strClientID[%s], strUserName[%s]",
		m_strbrokerURI.c_str(), m_strClientID.c_str(), m_strUserName.c_str());

	return true;
}

bool CBusinessImpl::Subscribe()
{
	std::vector<std::string> l_vecSplitTopics;
	m_pString->Split(m_pString->ReplaceAll(m_strTopics, " ", ""), ";", l_vecSplitTopics, 1);
	for (auto strSplit : l_vecSplitTopics)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			std::string l_strTopicName = *l_vecSubSplit.begin();
			std::string l_strSelector = (l_vecSubSplit.size() > 1) ? *(l_vecSubSplit.begin() + 1) : "";
			if (!l_strTopicName.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "subscribe topic begin[%s]", l_strTopicName.c_str());
				if (!m_pConsumer->Subscribe(l_strTopicName, 0, boost::make_shared<CReceiveCallback>(this), l_strSelector))
				{
					ICC_LOG_ERROR(m_pLog, "subscribe topic end failed[%s]", l_strTopicName.c_str());
					return false;
				}
				ICC_LOG_DEBUG(m_pLog, "subscribe topic end success[%s]", l_strTopicName.c_str());
			}
		}
	}

	std::vector<std::string> l_vecSplitQueues;

	std::string l_strMQCheckQueueName = m_pString->Format("queue_%s", m_strApplicationID.c_str());
	std::string l_strQueues = m_pString->Format("%s;%s", m_strQueues.c_str(), l_strMQCheckQueueName.c_str());

	m_pString->Split(m_pString->ReplaceAll(l_strQueues, " ", ""), ";", l_vecSplitQueues, 1);
	for (auto strSplit :  l_vecSplitQueues)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			std::string l_strQueueName = *l_vecSubSplit.begin();
			std::string l_strSelector = (l_vecSubSplit.size() > 1) ? *(l_vecSubSplit.begin() + 1) : "";
			if (!l_strQueueName.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "subscribe queue begin[%s]", l_strQueueName.c_str());
				if (!m_pConsumer->Subscribe(l_strQueueName, 1, boost::make_shared<CReceiveCallback>(this), l_strSelector))
				{
					ICC_LOG_ERROR(m_pLog, "subscribe queue end failed[%s]", l_strQueueName.c_str());
					return false;
				}
				ICC_LOG_DEBUG(m_pLog, "subscribe queue end success[%s]", l_strQueueName.c_str());
			}
		}
	}
	return true;
}

bool CBusinessImpl::UnSubscribe()
{
	std::vector<std::string> l_vecSplitTopics;
	m_pString->Split(m_pString->ReplaceAll(m_strTopics, " ", ""), ";", l_vecSplitTopics, 1);
	for (auto strSplit : l_vecSplitTopics)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			std::string l_strTopicName = *l_vecSubSplit.begin();
			std::string l_strSelector = (l_vecSubSplit.size() > 1) ? *(l_vecSubSplit.begin() + 1) : "";
			if (!l_strTopicName.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "Unsubscribe topic begin[%s]", l_strTopicName.c_str());
				if (!m_pConsumer->Unsubscribe(l_strTopicName, 0))
				{
					ICC_LOG_ERROR(m_pLog, "unsubscribe topic end failed[%s]", l_strTopicName.c_str());
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "unsubscribe topic end success[%s]", l_strTopicName.c_str());
				}
			}
		}
	}

	std::vector<std::string> l_vecSplitQueues;

	std::string l_strMQCheckQueueName = m_pString->Format("queue_%s", m_strApplicationID.c_str());
	std::string l_strQueues = m_pString->Format("%s;%s", m_strQueues.c_str(), l_strMQCheckQueueName.c_str());

	m_pString->Split(m_pString->ReplaceAll(l_strQueues, " ", ""), ";", l_vecSplitQueues, 1);
	for (auto strSplit : l_vecSplitQueues)
	{
		if (!strSplit.empty())
		{
			std::vector<std::string> l_vecSubSplit;
			m_pString->Split(strSplit, "()", l_vecSubSplit, 1);
			std::string l_strQueueName = *l_vecSubSplit.begin();
			std::string l_strSelector = (l_vecSubSplit.size() > 1) ? *(l_vecSubSplit.begin() + 1) : "";
			if (!l_strQueueName.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "unsubscribe queue begin[%s]", l_strQueueName.c_str());
				if (!m_pConsumer->Unsubscribe(l_strQueueName, 1))
				{
					ICC_LOG_ERROR(m_pLog, "unsubscribe queue end failed[%s]", l_strQueueName.c_str());
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "unsubscribe queue end success[%s]", l_strQueueName.c_str());
				}				
			}
		}
	}
	return true;
}

void CBusinessImpl::ParseCmsProperty(std::string p_strJsonProperty, boost::shared_ptr<CMSHeader> pHeader)
{
	if (!p_strJsonProperty.empty() && pHeader)
	{
		std::string strKey;
		std::string strValue;
		JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
		if (!pJson)
		{
			ICC_LOG_ERROR(m_pLog, "create json object failed.");
			return;
		}
		if (!pJson->LoadJson(p_strJsonProperty))
		{
			ICC_LOG_ERROR(m_pLog, "load json object Property failed.");
			return;
		}
		//string
		int nCnt = pJson->GetCount("/string");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/string/%d/key", idx);
			strValue = m_pString->Format("/string/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				pHeader->msProperties.insert(make_pair(strKey, strValue));
			}
		}
		//longlong
		nCnt = pJson->GetCount("/longlong");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/longlong/%d/key", idx);
			strValue = m_pString->Format("/longlong/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				long long llValue = m_pString->ToInt64(strValue);
				pHeader->mlProperties.insert(make_pair(strKey, llValue));
			}
		}
		//int
		nCnt = pJson->GetCount("/int");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/int/%d/key", idx);
			strValue = m_pString->Format("/int/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				int nValue = m_pString->ToInt(strValue);
				pHeader->mnProperties.insert(make_pair(strKey, nValue));
			}
		}
		//float
		nCnt = pJson->GetCount("/float");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/float/%d/key", idx);
			strValue = m_pString->Format("/float/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				float fValue = m_pString->ToFloat(strValue);
				pHeader->mfProperties.insert(make_pair(strKey, fValue));
			}
		}
		//double
		nCnt = pJson->GetCount("/double");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/double/%d/key", idx);
			strValue = m_pString->Format("/double/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				double dValue = m_pString->ToDouble(strValue);
				pHeader->mdProperties.insert(make_pair(strKey, dValue));
			}
		}
		//unsigned char
		nCnt = pJson->GetCount("/unsignedchar");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/unsignedchar/%d/key", idx);
			strValue = m_pString->Format("/unsignedchar/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				unsigned char ucValue = m_pString->ToUShort(strValue);
				pHeader->mcProperties.insert(make_pair(strKey, ucValue));
			}
		}
		//bool
		nCnt = pJson->GetCount("/bool");
		for (int idx = 0; idx < nCnt; idx++)
		{
			strKey = m_pString->Format("/bool/%d/key", idx);
			strValue = m_pString->Format("/bool/%d/value", idx);
			strKey = pJson->GetNodeValue(strKey, "");
			strValue = pJson->GetNodeValue(strValue, "");
			if (!strKey.empty())
			{
				bool bValue = m_pString->ToUInt(strValue) ? 1 : 0;
				pHeader->mbProperties.insert(make_pair(strKey, bValue));
			}
		}
	}
}

void CBusinessImpl::OnReceiveReloadConfigNotify(ObserverPattern::INotificationPtr p_pNotifiSendRequest)
{
	ICC_LOG_DEBUG(m_pLog, "receive reload config notify.");

	if (m_pConfig->ReloadConfig())
	{
		PROTOCOL::CSyncReloadConfigSuccess l_syncLoadConfig;
		l_syncLoadConfig.m_oHeader.m_strSystemID = "ICC";
		l_syncLoadConfig.m_oHeader.m_strSubsystemID = "Message";
		l_syncLoadConfig.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_syncLoadConfig.m_oHeader.m_strCmd = "sync_load_config_success";
		l_syncLoadConfig.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_syncLoadConfig.m_oHeader.m_strRequest = "topic_doubleserver";
		l_syncLoadConfig.m_oHeader.m_strRequestType = "1";
		
		l_syncLoadConfig.m_oBody.m_strClientID = m_strClientID;

		std::string l_strMsg = l_syncLoadConfig.ToString(ICCGetIJsonFactory()->CreateJson());
		//通知观察者中心		
		NotifyObserverList(boost::make_shared<CNotifiReceive>(l_strMsg, l_syncLoadConfig.m_oHeader.m_strMsgid));

		ICC_LOG_DEBUG(m_pLog, "sync load conifg success complete: %s", l_strMsg.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "no reload config. not neet send message.");
	}
}

void CBusinessImpl::OnNotifiSendRequest(ObserverPattern::INotificationPtr p_pNotifiSendRequest)
{
	static unsigned int s_serverReqCount_Queue = 0;
	static unsigned int s_serverSyncCount_Topic = 0;

	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	std::string l_strMessage = p_pNotifiSendRequest->GetMessages();
	if (!l_JsonPtr->LoadJson(l_strMessage))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
		return;
	}

	std::string l_strCmd = l_JsonPtr->GetNodeValue("/header/cmd", "");
	std::string l_strMsgid = l_JsonPtr->GetNodeValue("/header/msgid", "");
	std::string l_strRequest = l_JsonPtr->GetNodeValue("/header/request", "");
	std::string l_strRequestType = l_JsonPtr->GetNodeValue("/header/request_type", "");
	std::string strProperty = l_JsonPtr->GetNodeValue("/header/cmsproperty", "");

	boost::shared_ptr<CMSHeader> pCMSHeader;
	if (!strProperty.empty())
	{
		pCMSHeader = boost::make_shared<CMSHeader>();
		ParseCmsProperty(strProperty, pCMSHeader);
	}

	int l_iMode = m_pString->ToInt(l_strRequestType);//0：代表队列，1：代表主题

	if (p_pNotifiSendRequest->GetRequestMode() == ObserverPattern::ERequestMode::Request_Respond)
	{
		++s_serverReqCount_Queue;
		if (!m_pProducer)
		{
			ICC_LOG_FATAL(m_pLog, "producer object is null when send request message.");
			return;
		}

		if (!m_pProducer->Request(l_strMessage, l_strRequest, boost::make_shared<CResCallImpl>(this), pCMSHeader.get()))  //请求应答模式
		{
			ICC_LOG_ERROR(m_pLog, "server_request_push send message: [%s] fail.", l_strMessage.c_str());
			return;
		}

		//转发请求的时候在这里缓存回复实体 [10/25/2018 w16314]
		if (p_pNotifiSendRequest->GetNotification())
		{
			{
				Lock::AutoLock lock(m_callbackMutex);
				m_notificationMap[l_strMsgid] = p_pNotifiSendRequest->GetNotification();
			}
		}

		ICC_LOG_DEBUG(m_pLog, "sta_server_request_push cmd[%s] queue[%d] with details cmd[%s:%s:%s:%s] count[%d]=queue[%d]+topic[%d] notificationMap[%d]",
			l_strCmd.c_str(), s_serverReqCount_Queue,
			l_strCmd.c_str(), l_strMsgid.c_str(), "queue", l_strRequest.c_str(),
			s_serverReqCount_Queue + s_serverSyncCount_Topic, s_serverReqCount_Queue, s_serverSyncCount_Topic, m_notificationMap.size());
	}
	else
	{
		++s_serverSyncCount_Topic;
		if (!m_pProducer)
		{
			ICC_LOG_FATAL(m_pLog, "producer object is null when send topic message.");
			return;
		}

		if (!m_pProducer->Post(l_strMessage, l_strRequest, (0 == l_iMode) ? 1 : 0, pCMSHeader.get()))
		{
			ICC_LOG_ERROR(m_pLog, "server_sync Post message: [%s] fail.", l_strMessage.c_str());
			return;
		}

		ICC_LOG_DEBUG(m_pLog, "sta_server_sync cmd[%s] topic[%d] with details cmd[%s:%s:%s:%s] count[%d]=queue[%d]+topic[%d] notificationMap[%d]",
			l_strCmd.c_str(), s_serverSyncCount_Topic,
			l_strCmd.c_str(), l_strMsgid.c_str(), "topic", l_strRequest.c_str(),
			s_serverReqCount_Queue + s_serverSyncCount_Topic, s_serverReqCount_Queue, s_serverSyncCount_Topic, m_notificationMap.size());
	}
}

void CBusinessImpl::OnNotifiHealthRequest(ObserverPattern::INotificationPtr p_pNotifiSendRequest)
{
	PROTOCOL::CHealthRequest l_HealthRequest;
	ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthRequest:[%s]", p_pNotifiSendRequest->GetMessages().c_str());
	if (!l_HealthRequest.ParseString(p_pNotifiSendRequest->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
	}
	ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthRequest: RequestTime - [%s]", m_strRequestTime.c_str());

	if (!m_bServiceState)
	{
		// 直接回复
		PROTOCOL::CHealthRequest l_HealthRespond;
		l_HealthRespond.m_oBody.m_strStatus = "UP";
		l_HealthRespond.m_oBody.m_strActivemqStatus = "UP";
		l_HealthRespond.m_oBody.m_strpostgreSqlStatus = "UP";
		l_HealthRespond.m_oBody.m_strRedisStatus = "UP";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_HealthRespond.ToString(l_pIJson);

		p_pNotifiSendRequest->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthRequest:  [%s]", l_strMessage.c_str());
		return;
	}

	if (!m_strRequestTime.empty())
	{
		if (l_HealthRequest.m_oHeader.m_strSendTime.empty())
		{
			l_HealthRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		}

		DateTime::CDateTime dateSendTime = m_pDateTime->FromString(l_HealthRequest.m_oHeader.m_strSendTime);
		DateTime::CDateTime dateRequestTime = m_pDateTime->FromString(m_strRequestTime);
		if (m_pDateTime->SecondsDifference(dateSendTime, dateRequestTime) < 5)
		{
			m_strpostgreSqlStatus = "DOWN";
			m_strRedisStatus = "DOWN";
			// postgreSQL 状态
			if (healthPostgreSQL())
			{
				// postgreSQL 连接有问题
				m_strpostgreSqlStatus = "UP";
			}
			// redis
			if (healthRedis())
			{
				// redis 连接有问题
				m_strRedisStatus = "UP";
			}
			m_strStatus = "UP";
			if (m_strActivemqStatus == "DOWN" && m_strpostgreSqlStatus == "DOWN" && m_strRedisStatus == "DOWN")
			{
				m_strStatus = "DOWN";
			}

			// 直接回复
			PROTOCOL::CHealthRequest l_HealthRespond;
			l_HealthRespond.m_oBody.m_strStatus = m_strStatus;
			l_HealthRespond.m_oBody.m_strActivemqStatus = m_strActivemqStatus;
			l_HealthRespond.m_oBody.m_strpostgreSqlStatus = m_strpostgreSqlStatus;
			l_HealthRespond.m_oBody.m_strRedisStatus = m_strRedisStatus;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMessage = l_HealthRespond.ToString(l_pIJson);

			p_pNotifiSendRequest->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthRequest: l_strMessage - [%s]", l_strMessage.c_str());
			return;
		}
	}

	m_strRequestTime = l_HealthRequest.m_oHeader.m_strSendTime.empty() ? m_pDateTime->CurrentDateTimeStr() : l_HealthRequest.m_oHeader.m_strSendTime;
	
	{
		std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
		m_mapHealthActivemq["health_activemq"] = p_pNotifiSendRequest;
	}
	
	std::string l_strguid = m_pString->CreateGuid();

	if(m_pProducer)
	{
		// 发送 mq 消息
		PROTOCOL::CHealthActivemq l_healthActivemq;
		l_healthActivemq.m_oHeader.m_strSystemID = "ICC Message";
		l_healthActivemq.m_oHeader.m_strSubsystemID = "ICC";// SUBSYSTEMID;
		l_healthActivemq.m_oHeader.m_strMsgid = l_strguid;
		l_healthActivemq.m_oHeader.m_strRelatedID = "";
		l_healthActivemq.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_healthActivemq.m_oHeader.m_strCmd = "health_activemq";
		l_healthActivemq.m_oHeader.m_strRequest = "topic_health";//?topic_alarm_sync
		l_healthActivemq.m_oHeader.m_strRequestType = "1";
		l_healthActivemq.m_oHeader.m_strResponse = "ICC";
		l_healthActivemq.m_oHeader.m_strResponseType = "";
		l_healthActivemq.m_oBody.m_strhealthActivemq = l_strguid;

		{
			std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
			m_mapHealthActivemq[l_strguid] = p_pNotifiSendRequest;
		}

		std::string  l_strMsg = l_healthActivemq.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
		ICC_LOG_DEBUG(m_pLog, "healthActivemq send message:[%s]", l_strMsg.c_str());
		
	}

	// 定时器
	m_timerServiceLock.AddTimer(this, TIMERID_AMQ, 1000);
}

void CBusinessImpl::healthActivemqRequest()
{
	m_strpostgreSqlStatus = "DOWN";
	m_strRedisStatus = "DOWN";
	// postgreSQL 状态
	if (healthPostgreSQL())
	{
		// postgreSQL 连接有问题
		m_strpostgreSqlStatus = "UP";
	}
	// redis
	if (healthRedis())
	{
		// redis 连接有问题
		m_strRedisStatus = "UP";
	}

	boost::thread l_oThreadhealthActivemqFlgg(boost::bind(&CBusinessImpl::healthActivemqFlag, this));

	l_oThreadhealthActivemqFlgg.join();

	if (!m_bHealthActivemq)
	{
		// Activemq 连接有问题
		m_strActivemqStatus = "DOWN";
	}
	else
	{
		m_strActivemqStatus = "UP";
		{
			std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
			m_mapHealthActivemq.erase("health_activemq");
		}
		m_timerServiceLock.RemoveTimer(TIMERID_AMQ);
		return;
	}

	m_strStatus = "UP";
	if (m_strActivemqStatus == "DOWN" && m_strpostgreSqlStatus == "DOWN" && m_strRedisStatus == "DOWN")
	{
		m_strStatus = "DOWN";
	}

	PROTOCOL::CHealthRequest l_HealthRespond;
	l_HealthRespond.m_oBody.m_strStatus = m_strStatus;
	l_HealthRespond.m_oBody.m_strActivemqStatus = m_strActivemqStatus;
	l_HealthRespond.m_oBody.m_strpostgreSqlStatus = m_strpostgreSqlStatus;
	l_HealthRespond.m_oBody.m_strRedisStatus = m_strRedisStatus;

	// 返回
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_HealthRespond.ToString(l_pIJson);
	ObserverPattern::INotificationPtr l_pNotifiSendRequest;
	{
		std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
		l_pNotifiSendRequest = m_mapHealthActivemq["health_activemq"];
		m_mapHealthActivemq.erase("health_activemq");
	}

	if (!l_pNotifiSendRequest)
	{
		m_bHealthActivemq = false;
		m_timerServiceLock.RemoveTimer(TIMERID_AMQ);
		return;
	}

	l_pNotifiSendRequest->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	m_bHealthActivemq = false;
	m_timerServiceLock.RemoveTimer(TIMERID_AMQ);

}

bool CBusinessImpl::healthPostgreSQL()
{
	std::string strSql = "select value from public.icc_t_param where name = 'healthPostgreSQL';;;";
	ICC_LOG_DEBUG(m_pLog, "sql = %s\n", strSql.c_str());
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
	if (!l_result->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "ExecQuery Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::healthRedis()
{
	std::string strRedisValue;
	if (!m_pRedisClient->HGet(REDIS_HEALTH, "10086", strRedisValue))
	{
		if (strRedisValue.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed!!!");
			return false;
		}
	}
	ICC_LOG_DEBUG(m_pLog, "HGet REDIS_HEALTH end!!!");
	return true;
}

void CBusinessImpl::healthActivemqFlag()
{
	ICC_LOG_DEBUG(m_pLog, "healthActivemqFlag");
	// 等待 5s
	double seconds;
	time_t start_time, now;
	time(&start_time);

	while (1)
	{
		time(&now);
		seconds = difftime(now, start_time);

		{
			if (m_bHealthActivemq)
			{
				break;
			}
		}

		if (seconds == 5)
		{
			break;
		}
	}
}

void  CBusinessImpl::OnNotifiHealthActivemqRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthActivemqRequest: [%s]", p_strMsg.c_str());
	PROTOCOL::CHealthActivemq l_healthActivemq;
	if (!l_healthActivemq.ParseString(p_strMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthActivemqRequest ParseString faile");
		
	}
	
	if (l_healthActivemq.m_oHeader.m_strResponse == "ICC")
	{
		ObserverPattern::INotificationPtr l_NotificationPtr;
		m_bHealthActivemq = true;
		{
			std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
			l_NotificationPtr = m_mapHealthActivemq[l_healthActivemq.m_oBody.m_strhealthActivemq];
		}
		if (!l_NotificationPtr)
		{
			return;
		}
		m_strActivemqStatus = "UP";
		m_strpostgreSqlStatus = "DOWN";
		m_strRedisStatus = "DOWN";
		// postgreSQL 状态
		if (healthPostgreSQL())
		{
			// postgreSQL 连接有问题
			m_strpostgreSqlStatus = "UP";
		}
		// redis
		if (healthRedis())
		{
			// redis 连接有问题
			m_strRedisStatus = "UP";
		}

		PROTOCOL::CHealthRequest l_HealthRespond;
		l_HealthRespond.m_oBody.m_strStatus = "UP";
		l_HealthRespond.m_oBody.m_strActivemqStatus = m_strActivemqStatus;
		l_HealthRespond.m_oBody.m_strpostgreSqlStatus = m_strpostgreSqlStatus;
		l_HealthRespond.m_oBody.m_strRedisStatus = m_strRedisStatus;

		//回复
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_HealthRespond.ToString(l_pIJson);

		l_NotificationPtr->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		{
			std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
			m_mapHealthActivemq.erase(l_healthActivemq.m_oBody.m_strhealthActivemq);
		}
		m_timerServiceLock.RemoveTimer(TIMERID_AMQ);
		return;
	}

	{
		std::lock_guard<std::mutex> guard(m_mutexHealthActivemq);
		m_mapHealthActivemq.erase(l_healthActivemq.m_oBody.m_strhealthActivemq);
	}

	ICC_LOG_DEBUG(m_pLog, "OnNotifiHealthActivemqRequest end faile");
	return;
}


//由服务内部发起的请求应答消息，对方的应答从这里进入
void CResCallImpl::OnResponse(std::string p_strMessage)
{	
	m_pBusinessImpl->OnResponse(p_strMessage);
}

void CBusinessImpl::OnResponse(std::string p_strMessage)
{
	static unsigned int s_onResponseCount_Queue = 0;

	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	if (!l_JsonPtr->LoadJson(p_strMessage))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
		return;
	}

	std::string l_strCmd = l_JsonPtr->GetNodeValue("/header/cmd", "");
	std::string l_strguid = l_JsonPtr->GetNodeValue("/header/msgid", "");
	std::string l_strRelated_id = l_JsonPtr->GetNodeValue("/header/related_id", "");


	// 如果中转消息，直接回复 [10/25/2018 w16314]
	std::map<std::string, ObserverPattern::INotificationPtr>::iterator l_iter = m_notificationMap.find(l_strRelated_id);
	if (l_iter != m_notificationMap.end())
	{
		ObserverPattern::INotificationPtr l_pNtfPtr = l_iter->second;
		NotifyObserverList(boost::make_shared<CNotifiReceivePrivate>(this, p_strMessage, l_pNtfPtr->GetRequestGuid()));
		{
			Lock::AutoLock lock(m_notificationMutex);
			m_notificationMap.erase(l_iter);
		}
	}
	else
	{
		//不是中转消息，抛给消息的观察者（业务层）
		NotifyObserverList(boost::make_shared<CNotifiReceive>(p_strMessage, l_strguid));
		++s_onResponseCount_Queue;

		ICC_LOG_DEBUG(m_pLog, "sta_server_respond cmd[%s] queue[%d] with details cmd[%s:%s] notificationMap[%d]",
			l_strCmd.c_str(), s_onResponseCount_Queue,
			l_strCmd.c_str(), l_strRelated_id.c_str(), m_notificationMap.size());
	}
}

//由客户端发起的请求应答消息，服务应答从这里进入
void CBusinessImpl::Response(std::string p_strGuid, std::string p_strMessage, bool p_IsSubPackage)
{
	static unsigned int s_respondCount_Queue = 0;

	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	if (!l_JsonPtr->LoadJson(p_strMessage))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
		return;
	}

	std::string l_strCmd = l_JsonPtr->GetNodeValue("/header/cmd", "");
	std::string l_strRelated_id = l_JsonPtr->GetNodeValue("/header/related_id", "");

	{
		Lock::AutoLock lock(m_callbackMutex);
		std::map<std::string, IResCallbackPtr>::iterator l_iter = m_callbackMap.find(p_strGuid);
		if (l_iter == m_callbackMap.end())
		{
			ICC_LOG_ERROR(m_pLog, "m_callbackMap not find[%s].", p_strGuid.c_str());
			return;
		}

		l_iter->second->OnResponse(p_strMessage);
		if (!p_IsSubPackage)
		{
			m_callbackMap.erase(l_iter);
			++s_respondCount_Queue;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "+ cmd[%s] queue[%d] with details cmd[%s:%s:%s] callbackMap[%d]",
		l_strCmd.c_str(), s_respondCount_Queue,
		l_strCmd.c_str(), l_strRelated_id.c_str(), p_IsSubPackage ? "subPackage" : "lastPackage", m_callbackMap.size());
}

void CBusinessImpl::InitSyncCmdList()
{
	std::vector<std::string> l_vecSplitSyncCmd;
	m_pString->Split(m_pString->ReplaceAll(m_strSyncCmdList, " ", ""), ";", l_vecSplitSyncCmd, 1);
	for (auto strSplit : l_vecSplitSyncCmd)
	{
		if (!strSplit.empty())
		{
			m_SyncCmdList[strSplit] = 0;
			ICC_LOG_DEBUG(m_pLog, "Sync Cmd:[%s]", strSplit.c_str());
		}
	}
}

bool CBusinessImpl::CheckIsSyncCmd(const std::string& p_strCmd)
{
	if (p_strCmd.empty())
	{
		return false;
	}
	
	if (m_SyncCmdList.find(p_strCmd) != m_SyncCmdList.end())
	{
		int nSize = m_SyncCmdList.size();
		ICC_LOG_DEBUG(m_pLog, "true CheckIsSyncCmd Cmd:[%s],%d", p_strCmd.c_str(),nSize);
		return true;
	}
	return false;
}

void CBusinessImpl::NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, const std::string& p_strCmd)
{
	for (auto l_observer : m_pObserverCenterList)
	{
		l_observer->Notify(p_pNotifiRequest);
	}


	/*bool l_bSync = false;
	if (CheckIsSyncCmd(p_strCmd))
	{
		l_bSync = true;
	}

	for (auto l_observer : m_pObserverCenterList)
	{
		if (l_bSync)
		{
			l_observer->Notify(p_pNotifiRequest, ObserverPattern::ENotifyType::PostSync);
		}
		else
		{
			l_observer->Notify(p_pNotifiRequest, ObserverPattern::ENotifyType::Post);
		}
		
	}*/
}

//////////////////////////////////////////////////////////////////////////
//HTTPS Server
class CHttpCallback : public IHttpCallback
{
public:
	CHttpCallback(CBusinessImpl* p_MSQBusiness);
	virtual ~CHttpCallback();
public:
	virtual void OnMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody);

private:
	CBusinessImpl* m_pMSQBusiness;
};

CHttpCallback::CHttpCallback(CBusinessImpl* p_MSQBusiness) : m_pMSQBusiness(p_MSQBusiness)
{
	//
}
CHttpCallback::~CHttpCallback()
{
	//
}
void CHttpCallback::OnMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody)
{
	m_pMSQBusiness->OnHttpMessage(p_strSrcIP, p_strGuid, p_strTarget, p_strBody);
}

void CBusinessImpl::OnTimer(unsigned long uMsgId)
{
	if (uMsgId == TIMERID_GETSERVICELOCK)
	{
		PROCESSGUARD_EX;
		//不可重入
		m_oWorkThread.AddMessage(this, HEART_BEAT_MSG);
	}
	else if (uMsgId == TIMERID_CHECKHTTPSERVER)
	{
		if (m_pHttpServer->CheckHttpIsStop())
		{
			m_pHttpServer->StartHttp(boost::make_shared<CHttpCallback>(this), m_strHttpLocalAddress, std::atoi(m_strHttpLocalPort.c_str()));

			ICC_LOG_ERROR(m_pLog, "OnCheckHttp Restart HttpServer");
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "OnCheckHttp HttpServer starting");
		}
	}
	else if (uMsgId == TIMERID_RELOADCONFIG)
	{
		m_oWorkThread.AddMessage(this, THREADID_RELOADCONFIG);
	}
	else if (uMsgId == TIMERID_INSPECTSHARDTABLE)
	{
		//分表检查
		m_oWorkThread.AddMessage(this, THREADID_INSPECTSHARDTABLE);
		ICC_LOG_DEBUG(m_pLog, "On TIMERID_INSPECTSHARDTABLE starting");
	}
	else if (uMsgId == TIMERID_ACTIVEMQ_CHECK)
	{
		m_oWorkThread.AddMessage(this, THREADID_ACTIVEMQ_CHECK);
	}
	else if (uMsgId == TIMERID_AMQ)
	{
		m_oWorkThread.AddMessage(this, THREADID_AMQ);
	}

}


bool CBusinessImpl::StartHttpServer()
{
	m_pHttpServer = ICCGetIHttpServerFactory()->CreateHttpServer();
	m_pHttpServer->StartHttp(boost::make_shared<CHttpCallback>(this), m_strHttpLocalAddress, std::atoi(m_strHttpLocalPort.c_str()));

	return true;
}
void CBusinessImpl::OnHttpMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody)
{
	static unsigned int s_externalRecvCount_Http = 0;

	ICC_LOG_LOWDEBUG(m_pLog, "receive message from http client: guid[%s], target[%s] body[%s]", p_strGuid.c_str(), p_strTarget.c_str(), p_strBody.c_str());

	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}
	// 监控平台 GET 调用  health 接口

	if (!l_JsonPtr->LoadJson(p_strBody) && p_strTarget != "health")
	{
		ICC_LOG_ERROR(m_pLog, "load json[%s] object failed.", p_strBody.c_str());
		return;
	}

	l_JsonPtr->SetNodeValue("/header/src_ip", p_strSrcIP);
	l_JsonPtr->SetNodeValue("/header/msgid", p_strGuid);
	l_JsonPtr->SetNodeValue("/header/cmd", p_strTarget);
	l_JsonPtr->SetNodeValue("/header/request", "http");

	std::string l_strRerquest = l_JsonPtr->ToString();
	
	if (m_pHttpServer)
	{
		NotifyObserverList(boost::make_shared<CNotifiReceiveHttpPrivate>(this, l_strRerquest, p_strGuid, p_strTarget), p_strTarget);
		if (++s_externalRecvCount_Http > MAX_UNSIGNED_INT) s_externalRecvCount_Http = 0;

		ICC_LOG_DEBUG(m_pLog, "http_client_request_push srcip:[%s] guid: [%s], target: [%s], total_count: [%d]",
			p_strSrcIP.c_str(), p_strGuid.c_str(), p_strTarget.c_str(), s_externalRecvCount_Http);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "HttpsServer object is null.");
	}
}
void CBusinessImpl::HttpRespond(const std::string& p_strGuid, const std::string& p_strContent)
{
	static unsigned int s_respondHttpCount = 0;

	/*JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	if (!l_JsonPtr->LoadJson(p_strContent))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
		return;
	}

	std::string l_strCmd = l_JsonPtr->GetNodeValue("/header/cmd", "");
	std::string l_strRelated_id = l_JsonPtr->GetNodeValue("/header/related_id", "");
	std::string l_strBody = l_JsonPtr->GetNodeValue("/body", "");*/	

	_GetThread().AddMessage(this, THREADID_SENDRESPONSE, 0, 0, 0, p_strGuid, p_strContent);

	ICC_LOG_DEBUG(m_pLog, "HttpRespond add message complete guid %s.", p_strGuid.c_str());


	/*if (m_pHttpServer)
	{
		m_pHttpServer->OnResponse(p_strGuid, p_strContent);
		if (++s_respondHttpCount > MAX_UNSIGNED_INT) s_respondHttpCount = 0;

		ICC_LOG_DEBUG(m_pLog, "http_server_repond_push guid: [%s], total_count: [%d]",
			p_strGuid.c_str(), s_respondHttpCount);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "HttpsServer object is null.");
	}*/
	
}

bool CBusinessImpl::_AnalyzeNacosHeartBeatResponse(const std::string& strReceive, std::string& strHeartBeat, std::string& strCode)
{
	JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	strHeartBeat = pJson->GetNodeValue("/clientBeatInterval", "");
	strCode = pJson->GetNodeValue("/code", "");
	
	return true;
}

bool CBusinessImpl::_AnalyzeNacosQueryResponse(const std::string& strReceive, std::map<std::string, std::string>& mapHosts)
{
	JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
		std::string strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
		mapHosts.insert(std::make_pair(strIp, strPort));
	}

	return true;
}

std::string CBusinessImpl::_GetNacosLocalIP()
{
	std::string strErrorMessage;
	std::string strLocalIp;
	
	IHttpClientPtr tmp_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	if (tmp_pHttpClient == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "Create http client failed!!!. ");
		return "";
	}

	if (!tmp_pHttpClient->GetLocalIP(m_strNacosServerIp, m_strNacosServerPort, strErrorMessage, strLocalIp))
	{
		ICC_LOG_ERROR(m_pLog, "Get Nacos LocalIP failed, error msg: %s", strErrorMessage.c_str());

		return strLocalIp;
	}

	ICC_LOG_DEBUG(m_pLog, "Get Nacos LocalIP, ip: %s", strLocalIp.c_str());

	std::string strTmp = _GetRealIp(strLocalIp);

	/*if (!strTmp.empty() && strTmp != strLocalIp && _IsSameSegment(strTmp, strLocalIp))
	{
		ICC_LOG_DEBUG(m_pLog, "Get real LocalIP, ip: %s", strTmp.c_str());
		return strTmp;
	}*/

	return strLocalIp;
}

bool CBusinessImpl::_ExistNacosService()
{
	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strLocalIp;
	std::string strReceive = m_pHttpClient->GetExWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, strLocalIp, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response. ");
		return false;
	}
	if (m_strNacosLocalIP.empty())
	{
		if (!strLocalIp.empty())
		{
			if (m_strNacosLocalIP != strLocalIp)
			{
				ICC_LOG_DEBUG(m_pLog, "update local ip : old[%s], new[%s] ", m_strNacosLocalIP.c_str(), strLocalIp.c_str());
				m_strNacosLocalIP = strLocalIp;
			}
		}
	}
	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());
	std::map<std::string, std::string> mapServices;
	if (!_AnalyzeNacosQueryResponse(strReceive, mapServices))
	{
		return false;
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapServices.begin(); itr_const != mapServices.end(); ++itr_const)
	{
		ICC_LOG_DEBUG(m_pLog, "nacos service : [%s:%s]. ", itr_const->first.c_str(), itr_const->second.c_str());
		if (itr_const->first == m_strNacosLocalIP && itr_const->second == m_strHttpLocalPort)
		{
			ICC_LOG_DEBUG(m_pLog, "exist nacos service : [%s:%s:%s]. ", m_strNacosServiceName.c_str(), m_strNacosLocalIP.c_str(), m_strHttpLocalPort.c_str());
			return true;
		}
	}

	return false;
}

bool CBusinessImpl::_HeartBeatNacosService()
{
	std::string strTarget("/nacos/v1/ns/instance/beat");

	ICC::CHeartBeatData tmp_oHeartBeart;
	tmp_oHeartBeart.m_strCluster = "DefaultCluster";
	tmp_oHeartBeart.m_strIp = m_strNacosLocalIP;
	tmp_oHeartBeart.m_strPort = m_strHttpLocalPort;
	tmp_oHeartBeart.m_oMetaData = m_oNacosMetaData;
	//tmp_oHeartBeart.m_strMetaData = m_oNacosMetaData.ToJson(m_pJsonFty->CreateJson());
	tmp_oHeartBeart.m_strScheduled = "false";
	tmp_oHeartBeart.m_strServiceName = m_strNacosGroupName+"@@"+ m_strNacosServiceName;
	tmp_oHeartBeart.m_strWeight = "1";

	//std::string strContent = m_pString->Format("ip=%s&port=%s&namespaceId=%s&clusterName=DefaultCluster&serviceName=%s@@%s&groupName=%s&ephemeral=true",
	//	m_strNacosLocalIP.c_str(), m_strHttpLocalPort.c_str(), m_strNacosNamespace.c_str(), m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str(), m_strNacosGroupName.c_str());

	std::string strContent = m_pString->Format("namespaceId=%s&serviceName=%s@@%s&groupName=%s&beat=%s",
		m_strNacosNamespace.c_str(), m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str(), m_strNacosGroupName.c_str(), tmp_oHeartBeart.ToJson(m_pJsonFty->CreateJson()).c_str());

	ICC_LOG_DEBUG(m_pLog, "nacos heartbeat: [%s:%s]. ", strTarget.c_str(), strContent.c_str());


	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strReceive(m_pHttpClient->PutWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_nTimeOut));

	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "heartbeat nacos receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "heartbeat nacos receive data: [%s]", strReceive.c_str());

	std::string tmp_strHeartBeat;
	std::string tmp_strCode;

	if (!_AnalyzeNacosHeartBeatResponse(strReceive, tmp_strHeartBeat, tmp_strCode))
	{
		ICC_LOG_DEBUG(m_pLog, "heartbeat nacos failed!!!");
		return false;
	}

	if (tmp_strHeartBeat.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "heartbeat nacos failed!!!, heart beat interval is empty");
		return false;
	}
	
	m_uiHeartBeatTime = m_pString->ToUInt(tmp_strHeartBeat)/1000; //返回来的是毫秒

	return true;
}

bool CBusinessImpl::_RegistNacosService()
{
	std::string strTarget = "/nacos/v1/ns/instance";
	//std::string strContent = m_pString->Format("app=&clusterName=DefaultCluster&enable=true&ephemeral=true&groupName=%s&healthy=true&ip=%s&metadata=&namespaceId=%s&port=%s&serviceName=%s@@%s&weight=10", 
	//	m_strNacosGroupName.c_str(), m_strNacosLocalIP.c_str(), m_strNacosNamespace.c_str(), m_strHttpLocalPort.c_str(), m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str());
	std::string strContent = m_pString->Format("ip=%s&port=%s&namespaceId=%s&enabled=true&healthy=true&clusterName=DefaultCluster&serviceName=%s@@%s&groupName=%s&ephemeral=true&metadata=%s", 
		 m_strNacosLocalIP.c_str(), m_strHttpLocalPort.c_str(), m_strNacosNamespace.c_str(), m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str(), m_strNacosGroupName.c_str(),m_oNacosMetaData.ToJson(m_pJsonFty->CreateJson()).c_str());
	ICC_LOG_DEBUG(m_pLog, "nacos regist : [%s:%s]. ", strTarget.c_str(), strContent.c_str());

	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->PostWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "regist nacos receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "regist nacos receive data: [%s]", strReceive.c_str());

	if (strReceive == m_strNacosSuccessFlag)
	{
		ICC_LOG_DEBUG(m_pLog, "regist nacos success!!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "regist nacos failed!!!");

	return false;
}

bool CBusinessImpl::_UnregistNacosService()
{
	std::string strTarget = "/nacos/v1/ns/instance";
	//std::string strContent = m_pString->Format("app=&clusterName=DefaultCluster&enable=true&ephemeral=true&groupName=%s&healthy=true&ip=%s&metadata=&namespaceId=%s&port=%s&serviceName=%s@@%s&weight=10",
	//	m_strNacosGroupName.c_str(), m_strNacosLocalIP.c_str(), m_strNacosNamespace.c_str(), m_strHttpLocalPort.c_str(), m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str());
	std::string strContent = m_pString->Format("serviceName=%s@@%s&groupName=%s&ip=%s&port=%s&clusterName=DefaultCluster&namespaceId=%s&ephemeral=true",
		m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str(), m_strNacosGroupName.c_str(), m_strNacosLocalIP.c_str(), m_strHttpLocalPort.c_str(), m_strNacosNamespace.c_str());

	ICC_LOG_DEBUG(m_pLog, "nacos unregist : [%s:%s]. ", strTarget.c_str(), strContent.c_str());

	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->DeletetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "unregist nacos receive nothing!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "unregist nacos receive data: [%s]", strReceive.c_str());

	if (strReceive == m_strNacosSuccessFlag)
	{
		ICC_LOG_DEBUG(m_pLog, "unregist nacos success!!!");
		return true;
	}

	return false;
}

std::string name_mapper(const std::string& any_name)
{	
	static std::map<std::string, std::string> nm = { {"UCMS_ADDR","ucms_addr"} };
	return nm[any_name];
}

std::string CBusinessImpl::_GetUCMSAddrFromEnv()
{
	ICC_LOG_DEBUG(m_pLog, "get ucms addr param begin.");

	std::string strUcmsaddr;
	boost::program_options::options_description desc_env;
	desc_env.add_options()("UCMS_ADDR", boost::program_options::value<std::string>(&strUcmsaddr));

	boost::program_options::variables_map vm_env;

	try {
		boost::program_options::store(boost::program_options::parse_environment(desc_env,
			[](const std::string& i_env_var)
			{
				return i_env_var == "UCMS_ADDR" ? "UCMS_ADDR" : "";
			}),
			vm_env);
	}
	catch (boost::program_options::error e)
	{
		std::cout << e.what() << std::endl;
		ICC_LOG_ERROR(m_pLog, "get ucms addr param error. [%s]", e.what());
		return "";
	}

	boost::program_options::notify(vm_env);

	ICC_LOG_DEBUG(m_pLog, "get ucms addr param success. ucms:[%s]", strUcmsaddr.c_str());

	return strUcmsaddr;
}

std::string CBusinessImpl::_GetNacosParams(const std::string& strUcmsAddr)
{
	ICC_LOG_DEBUG(m_pLog, "get nacos addr param begin.");

	if (strUcmsAddr.empty())
	{
		ICC_LOG_ERROR(m_pLog, "ucms addr param empty. ");
		return "";
	}

	std::string strUcmsIp;
	std::string strUcmsPort;
	std::string strUcmsUrl = "";
	std::string strTarget;
	std::vector<std::string> vecParams;
	m_pString->Split(strUcmsAddr, ":", vecParams, false);

	if (vecParams.size() < 2)
	{
		ICC_LOG_ERROR(m_pLog, "analyze ucms addr param error. ");
		return "";
	}

	strUcmsIp = vecParams[0];
	strUcmsPort = vecParams[1];

	int p = strUcmsPort.find("/");

	if (p > 0)
	{
		strUcmsUrl = strUcmsPort.substr(p, strUcmsPort.size());
		strUcmsPort = strUcmsPort.substr(0, p);
	}

	ICC_LOG_DEBUG(m_pLog, "get nacos addr param begin.ucms info[%s:%s]", strUcmsIp.c_str(), strUcmsPort.c_str());
	
	if (!strUcmsUrl.empty())
	{
		strTarget = m_pString->Format("%s%s", strUcmsUrl.c_str(), m_pConfig->GetValue("ICC/Component/HttpServer/ucmsurl", "/v1/health/getAddr").c_str());
	}
	else
	{
		strTarget = m_pConfig->GetValue("ICC/Component/HttpServer/ucmsurl", "/v1/health/getAddr");
	}

	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;
	std::string strLocalIp;
	std::string strReceive = m_pHttpClient->GetExWithTimeout(strUcmsIp, strUcmsPort, strTarget, mapHeaders, strContent, strErrorMessage, strLocalIp, m_nTimeOut);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive ucms response. ");
		return "";
	}

	ICC_LOG_DEBUG(m_pLog, "receive ucms get nacos param response. [%s]", strReceive.c_str());

	return strReceive;
}

bool CBusinessImpl::_AnalyzeNacosParams(const std::string& strNacosParam)
{
	if (strNacosParam.empty())
	{
		ICC_LOG_ERROR(m_pLog, "nacos param info is empty!!!!!");
		return false;
	}

	std::vector<std::string> vecParams;
	m_pString->Split(strNacosParam, "\n", vecParams, false);
	if (vecParams.size() != 3)
	{
		ICC_LOG_ERROR(m_pLog, "nacos param size is error. params:[%s]", strNacosParam.c_str());
		return false;
	}

	std::map<std::string, std::string> mapParams;
	for (unsigned int i = 0; i < vecParams.size(); ++i)
	{
		std::vector<std::string> vecTmps;
		m_pString->Split(vecParams[i], "=", vecTmps, false);
		if (vecTmps.size() == 2)
		{
			mapParams.insert(std::make_pair(vecTmps[0], vecTmps[1]));
		}
	}

	std::map<std::string, std::string>::const_iterator itr;
	itr = mapParams.find("NACOS_ADDR");
	if (itr != mapParams.end())
	{
		std::vector<std::string> vecAddr;
		m_pString->Split(itr->second, ":", vecAddr, false);
		if (vecAddr.size() >= 2)
		{
			m_strNacosServerIp = vecAddr[0];
			m_strNacosServerPort = vecAddr[1];		

			if (m_strNacosServerIp.empty() || m_strNacosServerPort.empty())
			{
				ICC_LOG_ERROR(m_pLog, "nacos info from env is empty!!!");
				return false;
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "nacos info from env is error!!!");
			return false;
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "not find nacos info from env!!!");
		return false;
	}

	itr = mapParams.find("NACOS_NAMESPACE");
	if (itr != mapParams.end())
	{
		m_strNacosNamespace = itr->second;		
	}

	itr = mapParams.find("NACOS_GROUP");
	if (itr != mapParams.end())
	{
		m_strNacosGroupName = itr->second;		
	}

	ICC_LOG_DEBUG(m_pLog, "get nacos param from ucms. ip:%s, port:%s, namespace:%s, group: %s", m_strNacosServerIp.c_str(),
		m_strNacosServerPort.c_str(), m_strNacosNamespace.c_str(), m_strNacosGroupName.c_str());

	return true;
}

void CBusinessImpl::_NotifyNacosParams()
{
	if (m_strNacosServerIp.empty() || m_strNacosServerPort.empty())
	{
		ICC_LOG_ERROR(m_pLog, "nacos params is error!!!");
		return;
	}

	std::string strGuid = m_pString->CreateGuid();

	PROTOCOL::CSyncNacosParams syn;
	syn.m_oHeader.m_strCmd = "syn_nacos_params";
	syn.m_oHeader.m_strMsgid = strGuid;
	syn.m_oHeader.m_strSubsystemID = "Sub-Message";


	syn.m_oBody.m_strNacosServerIp = m_strNacosServerIp;
	syn.m_oBody.m_strNacosServerPort = m_strNacosServerPort;
	syn.m_oBody.m_strNacosNamespace = m_strNacosNamespace;
	syn.m_oBody.m_strNacosGroupName = m_strNacosGroupName;

	std::string strMessage = syn.ToString(m_pJsonFty->CreateJson());

	NotifyObserverList(boost::make_shared<CNotifiReceivePrivate>(this, strMessage, strGuid));
}

void CBusinessImpl::_WriteNacosParams(const std::string& strValue)
{
	ICC_LOG_DEBUG(m_pLog, "_WriteNacosParams is begin!!! value:[%s]", strValue.c_str());

	if (!m_pRedisClient)
	{
		ICC_LOG_ERROR(m_pLog, "redis client is null!!!");
		return;
	}

	if (!m_pRedisClient->Set(NacosParams, strValue.c_str()))
	{
		ICC_LOG_ERROR(m_pLog, "write nacos param failed!!! key=%s, value=%s", NacosParams, strValue.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "_WriteNacosParams success!!! value:[%s]", strValue.c_str());
	}
}

std::string CBusinessImpl::_GetNacosParams()
{
	ICC_LOG_DEBUG(m_pLog, "_GetNacosParams is begin!!!");

	if (!m_pRedisClient)
	{
		ICC_LOG_ERROR(m_pLog, "redis client is null!!!");
		return "";
	}

	std::string strValue;
	if (!m_pRedisClient->Get(NacosParams, strValue))
	{
		ICC_LOG_ERROR(m_pLog, "get nacos param failed!!!key=%s", NacosParams);
		return "";
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "_GetNacosParams success!!! value:[%s]", strValue.c_str());
	}

	return strValue;
}

void CBusinessImpl::_ResetNacosParam(const std::string& strUcmsAddr)
{
	std::string strNacosParam = _GetNacosParams(strUcmsAddr);
	if (!strNacosParam.empty())
	{
		_WriteNacosParams(strNacosParam);
	}
	else
	{
		strNacosParam = _GetNacosParams();
	}
	if (!_AnalyzeNacosParams(strNacosParam))
	{
		strNacosParam = _GetNacosParams();
		if (!_AnalyzeNacosParams(strNacosParam))
		{
			return;
		}
	}
	_NotifyNacosParams();
}

void CBusinessImpl::_ProcessNacosCheck()
{
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	if (m_pHttpClient == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "Create http client failed!!!. ");
		return;
	}

	std::string strUcmsaddr;	
	strUcmsaddr = _GetUCMSAddrFromEnv();
	_ResetNacosParam(strUcmsaddr);

	ICC_LOG_DEBUG(m_pLog, "_ProcessNacosCheck enter. master flag is %s", m_bIsMaster ? "true" : "false");
	ICC_LOG_DEBUG(m_pLog, "nacos server address: %s:%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	ICC_LOG_DEBUG(m_pLog, "local service address: %s:%s", m_strNacosLocalIP.c_str(), m_strHttpLocalPort.c_str());
	ICC_LOG_DEBUG(m_pLog, "local service info: namesapce:%s, groupname:%s, servicename:%s", m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strNacosServiceName.c_str());
	ICC_LOG_DEBUG(m_pLog, "nacos check interval: %lld", m_lCheckInterval);

	

	while (true)
	{
		if (m_bStopNacos)
		{
			ICC_LOG_DEBUG(m_pLog, "stop, will exit nacos check!!!. ");
			break;
		}

		if (m_bIsMaster)
		{
			++m_uiCountTimes;

			m_uiHeartBeatCount = 0;

			ICC_LOG_DEBUG(m_pLog, "is master, will query nacos service!!!. ");
			if (!_ExistNacosService())
			{
				ICC_LOG_DEBUG(m_pLog, "is master, will regist nacos!!!. ");
				bool bRs = _RegistNacosService();
				if(bRs)
				{
					ICC_LOG_DEBUG(m_pLog, "regist nacos success!!!. ");
				}
			}
			else
			{
				if (m_uiCountTimes >= m_uiHeartBeatTime)
				{
					if (!_HeartBeatNacosService())
					{
						ICC_LOG_DEBUG(m_pLog, "is master, heartbeat failed!!!. ");
					}

					m_uiCountTimes = 0;
				}
				
			}
		}
		else
		{
			m_uiHeartBeatCount = 0;
			ICC_LOG_DEBUG(m_pLog, "is not master, will query nacos service!!!. ");
			if (_ExistNacosService())
			{
				ICC_LOG_DEBUG(m_pLog, "is not master, will unregist nacos!!!. ");
				bool bRs = _UnregistNacosService();
				if(bRs)
				{
					ICC_LOG_DEBUG(m_pLog, "unregist nacos success!!!. ");
				}
			}
		}		

		m_pHelpTool->Sleep(m_lCheckInterval);
	}

	ICC_LOG_DEBUG(m_pLog, "_ProcessNacosCheck exit.");
}


bool CBusinessImpl::_GetLocalAllIp(std::vector<std::string>& vecTmps)
{
	try
	{
		using boost::asio::ip::tcp;

		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(boost::asio::ip::host_name(), "");
		tcp::resolver::iterator iter = resolver.resolve(query);
		tcp::resolver::iterator end; // End marker.
		int i = 0;
		while (iter != end)
		{
			i++;
			tcp::endpoint ep = *iter++;
			std::string strTmpIp = ep.address().to_string();
			ICC_LOG_DEBUG(m_pLog, "the %d ip is %s .", i, strTmpIp.c_str());
			if (!strTmpIp.empty())
			{
				vecTmps.push_back(strTmpIp);
			}			
		}

		return true;
	}	
	catch (const std::exception& e)
	{
		ICC_LOG_ERROR(m_pLog, "_GetLocalAllIp exception:[%s]", e.what());
	}
	return false;
}

bool CBusinessImpl::_IsSameSegment(const std::string& strSourceIp, const std::string& strDestIp)
{
	std::vector<std::string> vecSources;
	std::vector<std::string> vecDests;

	m_pString->Split(strSourceIp, ".", vecSources, false);
	m_pString->Split(strDestIp, ".", vecDests, false);

	if (vecSources.size() != vecDests.size())
	{
		return false;
	}

	if (vecSources.size() != 4)
	{
		return false;
	}

	for (int i = 0; i < 3; ++i)
	{
		if (vecSources[i] != vecDests[i])
		{
			return false;
		}
	}

	return true;
}

std::string CBusinessImpl::_GetRealIp(const std::string& strTmpIp)
{
	std::vector<std::string> vecLocalIps;
	if (!_GetLocalAllIp(vecLocalIps))
	{
		return strTmpIp;
	}

	unsigned int iCount = vecLocalIps.size();
	if (iCount == 0)
	{
		return strTmpIp;
	}

	if (iCount == 1 && strTmpIp == vecLocalIps[0])
	{
		return strTmpIp;
	}
	else
	{
		for (unsigned int i = 0; i < iCount; ++i)
		{
			if (vecLocalIps[i].empty())
			{
				continue;
			}

			if (vecLocalIps[i] == strTmpIp)
			{
				continue;
			}

			if (_IsSameSegment(strTmpIp, vecLocalIps[i]))
			{
				return vecLocalIps[i];
			}
		}
	}

	return strTmpIp;
}


void CBusinessImpl::_HttpRespond(const std::string& p_strGuid, const std::string& p_strContent)
{
	//static unsigned int s_respondHttpCount = 0;

	ICC_LOG_DEBUG(m_pLog, "_HttpRespond begin guid: [%s]", p_strGuid.c_str());

	if (m_pHttpServer)
	{
		m_pHttpServer->OnResponse(p_strGuid, p_strContent);
		int iTmpCount = 0;
		{
			std::lock_guard<std::mutex> lock(m_mutexRespondHttpCount);
			if (++m_respondHttpCount > MAX_UNSIGNED_INT)
				m_respondHttpCount = 0;
			iTmpCount = m_respondHttpCount;
		}
		

		ICC_LOG_DEBUG(m_pLog, "_HttpRespond end http_server_repond_push guid: [%s], total_count: [%d]",
			p_strGuid.c_str(), iTmpCount);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "HttpsServer object is null.");
	}

}

CCommonWorkThread& CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uIndex >= 0x0fffffff)
		{
			m_uIndex = 0;
		}
		m_uIndex++;
		iIndex = m_uIndex & (MSGSEND_THREAD_COUNT - 1);
	}

	return m_msgSendThread[iIndex];
}