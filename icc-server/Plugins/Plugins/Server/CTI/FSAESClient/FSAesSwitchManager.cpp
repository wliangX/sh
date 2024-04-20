
#include "Boost.h"
#include "WorkThread.h"
#include "FSAesSwitchManager.h"
#include "FSAesClientImpl.h"
#include "TaskManager.h"
#include "AgentManager.h"
#include "DeviceManager.h"
#include "ConferenceManager.h"
#include "SysConfig.h"
#include "CTIFSAesDefine.h"
#include "BlackListManager.h"
#define MAX_HEARTBEAT		3		//	心跳检测次数
#define CHECK_TIMESPAN		1		//	单位：秒
#define SLEEP_TIMESPAN		30		//	单位：毫秒
using namespace std;

boost::shared_ptr<CFSAesSwitchManager> CFSAesSwitchManager::m_pInstance = nullptr;
//定时器ID
const unsigned long TIMERID_AES_HeartBeat = 21001;
const unsigned long TIMERID_AES_acdMonitor  = 21002;
const unsigned long TIMERID_AES_agentMonitor = 21003;
const unsigned long TIMERID_AES_deviceMonitor = 21004;

const unsigned long TIMERID_AES_getExtensionList = 21005;

std::map<int, std::string> g_mapFSAesHttpReq
{
	{REQUEST_AES_LOGIN_CMD, "aesLogin"},
	{REQUEST_AES_LOGOUT_CMD, "aesLogout"},
	{REQUEST_AES_HEART_BEAT_CMD, "aesHeartbeat"},
	{REQUEST_MONITOR_AGENT_CMD, "monitorAgent"},
	{REQUEST_DISMONITOR_AGENT_CMD, "dismonitorAgent"},
	{REQUEST_MONITOR_DEVICE_CMD, "monitorDevice"},
	{REQUEST_DISMONITOR_DEVICE_CMD, "dismonitorDevice"},

	{REQUEST_MONITOR_ACD_CMD, "monitorSkill"},
	{REQUEST_DISMONITOR_ACD_CMD, "dismonitorSkill"},

	{GET_AGENT_LIST_CMD,"getAgentList"},
	{GET_EXTENSION_LIST_CMD,"getExtensionList"},
	

	{REQUEST_AGENT_LOGIN_CMD, "signIn"},
	{REQUEST_AGENT_LOGOUT_CMD, "signOut"},
	{REQUEST_SET_AGENTIDLE_CMD, "setAgentIdle"},
	{REQUEST_SET_AGENTBUSY_CMD, "setAgentBusy"},

	{REQUEST_MAKE_CALL_CMD, "makeCall"},
	{REQUEST_HOLD_CALL_CMD, "callHold"},
	{REQUEST_RETRIEVE_CALL_CMD, "callUnhold"},
	{REQUEST_ANSWER_CALL_CMD, "callAnswer"},
	{REQUEST_CONSULTATION_CALL_CMD, "consultationCall"},
	{REQUEST_RECONNECT_CALL_CMD,"consultcancel"},

	{REQUEST_HANGUP_CALL_CMD, "callHangup"},
	{REQUEST_REFUSE_CALL_CMD, "callHangup"},
	{REQUEST_CLEAR_CALL_CMD, "callHangup"},
	//{REQUEST_FORCEPOP_CALL_CMD, "callHangup"},

	{REQUEST_TRANSFER_CALL_CMD, "transferCall"}, //transferCall
	{REQUEST_TRANSFER_EX_CALL_CMD, "transferExCall"}, //transferCall 咨询转
	//{REQUEST_TAKEOVER_CALL_CMD, "transferCall"}, //接管
	{REQUEST_PICKUP_CALL_CMD, "callPickup"}, //代答

	{REQUEST_THIRD_WAY_CMD, "thirdway"},
	
	//{REQUEST_BARGEIN_CALL_CMD, "callBargein"},
	
	//{REQUEST_MONITOR_CALL_CMD, "monitorCall"},
	{REQUEST_MONITOR_CALL_CMD, "makeConference"},  //监听用会议实现
	{REQUEST_MONITOR_EX_CALL_CMD,"addConfMember"},
	{REQUEST_CONFERENCE_CREATE_CMD, "makeConference"},
	{REQUEST_CONFERENCE_ADDPARTY_CMD, "addConfMember"},
	//{REQUEST_CONFERENCE_DELPARTY_CMD, "delConfMember"},
	{REQUEST_CONFERENCE_DELPARTY_CMD, "callHangup"},	//删除成员改发呼叫挂断 change 2023-9-21

	{REQUEST_FORCEPOP_CALL_CMD, "releaseConference"},  //强拆 为释放监听创建的会议，ICC只有监听成功后会发强拆
	{REQUEST_CONFERENCE_RELEASE_CMD, "releaseConference"},
	{REQUEST_CONFMEMBER_MUTE_CMD,"setMemberMute"},
	{REQUEST_CONFMEMBER_UNMUTE_CMD,"setMemberUnmute"},
	{REQUEST_TAKEOVER_CALL_CMD, "setMemberUnmute"}, //接管
	{REQUEST_BARGEIN_CALL_CMD, "setMemberUnmute"},  //强插,ICC只有监听成功后才能发强插，监听通过会议实现

	{REQUEST_GET_HISTORY_CALL_LIST_CMD,"getHistorycallList"},
	{REQUEST_DEL_HISTORY_RECORD_CMD,"delHistoryRecord"}
};

std::map<std::string, int> g_mapFSAesRequestResult
{
	{"event_agent_signin_result",REQUEST_AGENT_LOGIN_CMD},
	{"event_agent_signout_result",REQUEST_AGENT_LOGOUT_CMD},
	{"event_set_agent_state_result",REQUEST_SET_AGENTIDLE_CMD},
	{"event_set_agent_state_result",REQUEST_SET_AGENTBUSY_CMD},

	{"event_make_call_result",REQUEST_MAKE_CALL_CMD},
	{"event_call_hold_result",REQUEST_HOLD_CALL_CMD},
	{"event_call_unhold_result",REQUEST_RETRIEVE_CALL_CMD},
	{"event_call_answer_result",REQUEST_ANSWER_CALL_CMD},
	{"event_consultation_result",REQUEST_CONSULTATION_CALL_CMD}, //咨询呼结果
	{"event_consultcancel_result",REQUEST_RECONNECT_CALL_CMD},
	{"event_call_hangup_result",REQUEST_HANGUP_CALL_CMD}, //挂断结果
	//{"callHangup",REQUEST_REFUSE_CALL_CMD},
	{"event_transfer_call_result",REQUEST_TRANSFER_CALL_CMD}, //转移结果
	{"event_transfer_ex_call_result",REQUEST_TRANSFER_EX_CALL_CMD}, //转移结果

	{"event_call_pickup_result",REQUEST_PICKUP_CALL_CMD}, //代答结果
	{"event_threeway_result",REQUEST_THIRD_WAY_CMD},
	{"event_monitor_call_result",REQUEST_MONITOR_CALL_CMD},
	{"event_bargein_call_result",REQUEST_BARGEIN_CALL_CMD}, //强插结果
	{"event_forcepop_call_result",REQUEST_FORCEPOP_CALL_CMD}, //强拆结果

	{"event_make_conference_result",REQUEST_CONFERENCE_CREATE_CMD},
	{"event_add_confmember_result",REQUEST_CONFERENCE_ADDPARTY_CMD},
	{"event_del_confmember_result",REQUEST_CONFERENCE_DELPARTY_CMD},
	{"event_release_conference_result",REQUEST_CONFERENCE_RELEASE_CMD},
	{"event_set_membermute_result",REQUEST_CONFMEMBER_MUTE_CMD},
	{"event_set_memberunmute_result",REQUEST_CONFMEMBER_UNMUTE_CMD}
};


std::string CFSAesSwitchManager::GetRequestCmdString(int p_nRequestType)
{
	auto l_iter = g_mapFSAesHttpReq.find(p_nRequestType);
	if (l_iter != g_mapFSAesHttpReq.end())
	{
		return l_iter->second;
	}
	ICC_LOG_FATAL(m_pLog, "get request name error,type:[%d]", p_nRequestType);
	return "";
}

bool CFSAesSwitchManager::_isRequestResultMsg(const std::string &p_strEventName)
{
	auto l_iter = g_mapFSAesRequestResult.find(p_strEventName);
	if (l_iter != g_mapFSAesRequestResult.end())
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//启动定义的所有工作线程
void CFSAesSwitchManager::_StartWorkThreads()
{
	if (!m_pCmdTaskThread)
	{
		m_pCmdTaskThread = boost::make_shared<CWorkThread<CFSAesSwitchManager>>(this, &CFSAesSwitchManager::_ThreadFunCmdTask);
	}
	if (m_pCmdTaskThread)
	{
		m_pCmdTaskThread->StartThread();
	}
	if (!m_pCheckExcusedTaskThread)
	{
		m_pCheckExcusedTaskThread = boost::make_shared<CWorkThread<CFSAesSwitchManager>>(this, &CFSAesSwitchManager::_ThreadFunCheckExcusedTask);
	}
	if (m_pCheckExcusedTaskThread)
	{
		m_oLastCheckExcusedTime = m_pDateTimePtr->CurrentDateTime();
		m_pCheckExcusedTaskThread->StartThread();
	}
	if (!m_pEventTaskThread)
	{
		m_pEventTaskThread = boost::make_shared<CWorkThread<CFSAesSwitchManager>>(this, &CFSAesSwitchManager::_ThreadFuncEventTask);
	}
	if (m_pEventTaskThread)
	{
		m_pEventTaskThread->StartThread();
	}
}

//停止定义的所有工作线程
void CFSAesSwitchManager::_StopWorkThreads()
{
	if (m_pCmdTaskThread)
	{
		m_pCmdTaskThread->StopThread();
	}
	if (m_pCheckExcusedTaskThread)
	{
		m_pCheckExcusedTaskThread->StopThread();
	}
	if (m_pEventTaskThread)
	{
		m_pEventTaskThread->StopThread();
	}
}

void CFSAesSwitchManager::_ThreadFunCmdTask()
{
	ICC_LOG_DEBUG(m_pLog, "------------------Cmd Task Thread Start!!------------------");
	while (m_pCmdTaskThread->isAlive())
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetCmdTaskHeader();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();
			int l_nTaskType = l_pTask->GetTaskType();

			// 执行处理流程
			_ExeCommand(l_nTaskType, l_lTaskId, l_pTask);
			CTaskManager::Instance()->DeleteCmdTask(l_lTaskId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}
	ICC_LOG_DEBUG(m_pLog, "------------------Cmd Task Thread Exit!!------------------");
}


void CFSAesSwitchManager::_ThreadFunCheckExcusedTask()
{
	ICC_LOG_DEBUG(m_pLog, "------------------DoCheckExcusedTask Task Thread Start!!------------------");
	while (m_pCheckExcusedTaskThread->isAlive())
	{
		std::string l_strTaskMsgId = "";
		int l_nTaskType = Task_NULL;
		if (CTaskManager::Instance()->ExecuteTaskTimeout(l_nTaskType, l_strTaskMsgId))
		{
			ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExecutedTask(l_strTaskMsgId);
			if (l_pExcutedTask)
			{
				IFSAesCmdRequestPtr l_pAesCmdRequest = boost::dynamic_pointer_cast<CFSAesRequestCmd>(l_pExcutedTask->GetSwitchNotif());
				// CMD 执行超时
				_DispAsyncRequestFailedCmd(ERROR_CMD_TIMEOUT, l_pAesCmdRequest);
			}
			CTaskManager::Instance()->DeleteExecutedTask(l_strTaskMsgId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN * 2));
		}
	}
	ICC_LOG_DEBUG(m_pLog, "------------------DoCheckExcusedTask Task Thread Exit!!------------------");
}

//
void CFSAesSwitchManager::_ThreadFuncEventTask()
{
	ICC_LOG_DEBUG(m_pLog, "------------------DoSwitchEvent Task Thread Start!!------------------");
	while (m_pEventTaskThread->isAlive())
	{
		ITaskPtr l_pTask = CTaskManager::Instance()->GetSwitchEventTaskHeader();
		if (l_pTask)
		{
			// 执行处理流程
			long l_lTaskId = l_pTask->GetTaskId();
			int l_nTaskType = l_pTask->GetTaskType();

			if (l_nTaskType == Task_ctiAsyncRequest_Resp)
			{
				_ExeAsyncResultEvent(l_lTaskId, l_pTask);
			}
			else if (l_nTaskType == Task_ctiSyncRequest_Resp)
			{
				_ExeSyncResultEvent(l_lTaskId, l_pTask);
			}
			else if (l_nTaskType == Task_ctiLocal_Query_Resp)
			{
				_ExeLocalGetEvent(l_lTaskId, l_pTask);
			}
			else if (l_nTaskType == Task_HCP_Notify_Event)
			{
				_ExeHcpNotifyEvent(l_nTaskType, l_lTaskId, l_pTask);
			}
			CTaskManager::Instance()->DeleteSwitchEventTask(l_lTaskId);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}
	ICC_LOG_DEBUG(m_pLog, "------------------DoSwitchEvent Task Thread Exit!!------------------");
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//
CFSAesSwitchManager::CFSAesSwitchManager()
	:m_pFsAesHttpHelper(nullptr),
	m_bLoginAesSuccess(false),
	m_bLoadExtensionFlag(false),
	m_strLocalServerIP(""),
	m_nAESLoginTick(0),
	m_bStart(false),
	m_nHeartBeatErrorCount(0),
	m_nSendLoginCount(0),
	m_bPhoneTransferEnable(true),
	m_pSwitchEventCallback(nullptr)
{


}

CFSAesSwitchManager::~CFSAesSwitchManager()
{
	//	m_bExitSys = true;
}

boost::shared_ptr<CFSAesSwitchManager> CFSAesSwitchManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CFSAesSwitchManager>();
	}

	return m_pInstance;
}

void CFSAesSwitchManager::ExitInstance()
{
}

void CFSAesSwitchManager::OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback)
{
	CSysConfig::Instance()->OnInit(p_pResourceManager);
	CTaskManager::Instance()->OnInit(p_pResourceManager);
	CDeviceManager::Instance()->OnInit(p_pResourceManager);
	CConferenceManager::Instance()->OnInit(p_pResourceManager);
	//要放CDeviceManager后面初始化
	CAgentManager::Instance()->OnInit(p_pResourceManager);

	CBlackListManager::Instance()->OnInit(p_pResourceManager);
	CCallManager::Instance()->OnInit(p_pResourceManager);

	m_pSwitchEventCallback = p_pCallback;
	m_pResourceManager = p_pResourceManager;

	m_pJsonFty = ICCGetIJsonFactory();
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();
	m_pStringPtr = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();

	m_CallbackInterface.SetCallbackInterface(p_pCallback);
	m_CallbackInterface.SetLog(m_pLog);
	_InitHcpAesEventFun();

	m_pFsAesHttpHelper = boost::make_shared<CFSAesHttpHelper>(m_pLog, p_pResourceManager);
	if (m_pFsAesHttpHelper)
	{
		m_pFsAesHttpHelper->SetNodeName(CSysConfig::Instance()->GetAESNodeName());
	}
}

 
void CFSAesSwitchManager::OnStart()
{
	m_CallbackInterface.Start();
	CTaskManager::Instance()->OnStart();
	CDeviceManager::Instance()->OnStart();
	CAgentManager::Instance()->OnStart();
	CBlackListManager::Instance()->OnStart();

	CCallManager::Instance()->OnStart();
	m_bLoginAesSuccess = false;
	m_bStart = true;
	_StartWorkThreads();

	m_bPhoneTransferEnable = CSysConfig::Instance()->GetPhoneTransferEnable();
	if (m_pFsAesHttpHelper)
	{
		std::string l_strLocalIP = CSysConfig::Instance()->GetLocalIP();
		std::string l_strLocalHttpPort = CSysConfig::Instance()->GetCTIServerPort();

		std::string l_strFSServerIP = CSysConfig::Instance()->GetAEServerIP();
		std::string l_strFSServerPort = CSysConfig::Instance()->GetAEServerPort();
		m_pFsAesHttpHelper->Start(l_strLocalIP, l_strLocalHttpPort, l_strFSServerIP, l_strFSServerPort);

		m_strCurrASEServerIp = l_strFSServerIP;
		m_strLocalServerIP = l_strLocalIP;
		if (m_strLocalServerIP.empty() || m_strLocalServerIP == "0.0.0.0")
		{
			m_pFsAesHttpHelper->GetLocalServerIp(m_strLocalServerIP);
		}

		ICC_LOG_DEBUG(m_pLog, "httpServer start, LocalIP:[%s],Port:[%s],LocalServerIp:[%s],PhoneTransferEnable:[%d]", l_strLocalIP.c_str(), l_strLocalHttpPort.c_str(), m_strLocalServerIP.c_str(), m_bPhoneTransferEnable);
	}

	m_myTimer.AddTimer(this, TIMERID_AES_HeartBeat, 5000);

	_AesLoginRequest();
	
}

void CFSAesSwitchManager::OnStop()
{
	if (m_bLoginAesSuccess)
	{
		_AesLogoutRequest();
	}

	m_myTimer.RemoveTimer(TIMERID_AES_HeartBeat);

	CCallManager::Instance()->OnStop();

	CTaskManager::Instance()->OnStop();
	CDeviceManager::Instance()->OnStop();
	CAgentManager::Instance()->OnStop();
	CBlackListManager::Instance()->OnStop();
	m_bStart = false;
}

void CFSAesSwitchManager::OnDestroy()
{
	_StopWorkThreads();
	CAgentManager::Instance()->ExitInstance();
	CDeviceManager::Instance()->ExitInstance();
	CTaskManager::Instance()->ExitInstance();
	CSysConfig::Instance()->ExitInstance();
	CBlackListManager::Instance()->ExitInstance();
}

void CFSAesSwitchManager::OnTimer(unsigned long uMsgId)
{
	ICC_LOG_DEBUG(m_pLog, "OnTimer TimeID:[%d]", uMsgId);
	if (uMsgId == TIMERID_AES_HeartBeat)
	{
		if (!m_bLoginAesSuccess)
		{
			m_nAESLoginTick++;
			if (m_nAESLoginTick >= 3)
			{
				m_nAESLoginTick = 0;

				std::string l_strSlaveAESServerIp = CSysConfig::Instance()->GetAESlaveServerIP();
				if (!l_strSlaveAESServerIp.empty())
				{
					if (m_strCurrASEServerIp != l_strSlaveAESServerIp)
					{
						m_strCurrASEServerIp = l_strSlaveAESServerIp;
					}
					else
					{
						m_strCurrASEServerIp = CSysConfig::Instance()->GetAEServerIP();
					}
					m_pFsAesHttpHelper->UpdateAESServerIp(m_strCurrASEServerIp);
				}
				_AesLoginRequest();
				m_nSendLoginCount++;
			}
		}
		else
		{
			_AesHeartBeatRequest();
		}
	}
	else if (uMsgId == TIMERID_AES_acdMonitor)
	{
		_AesMonitorRequest(CTI_DEVICE_TYPE_ACD);
	}
	else if (uMsgId == TIMERID_AES_agentMonitor)
	{
		_AesMonitorRequest(CTI_DEVICE_TYPE_AGENT);
	}
	else if (uMsgId == TIMERID_AES_deviceMonitor)
	{
		_AesMonitorRequest(CTI_DEVICE_TYPE_PHONE);
	}
	else if (uMsgId == TIMERID_AES_getExtensionList)
	{
		if (!m_bLoadExtensionFlag)
		{
			_AesGetExtensionListRequest();
		}
	}
}

void CFSAesSwitchManager::_AesLoginRequest()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AES_LOGIN_CMD);
	if (l_pCmdRequest)
	{
		std::string l_strFSLoginUser = CSysConfig::Instance()->GetAESUserName();
		std::string l_strFSLoginPswd = CSysConfig::Instance()->GetAESPassword();

		l_pCmdRequest->SetParam(CTI_PARAM_user_name, l_strFSLoginUser);
		l_pCmdRequest->SetParam(CTI_PARAM_user_pwd, l_strFSLoginPswd);
		l_pCmdRequest->SetParam(CTI_PARAM_clientid, m_pFsAesHttpHelper->GetCTIClientID());
		
		if (m_strLocalServerIP.empty() || m_strLocalServerIP == "0.0.0.0")
		{
			ICC_LOG_ERROR(m_pLog, "Local IP Address failed,localIp:[%s]", m_strLocalServerIP.c_str());
			m_pFsAesHttpHelper->GetLocalServerIp(m_strLocalServerIP);
		}
		else
		{
			std::string l_strHcpConnectLocalIP = CSysConfig::Instance()->GetHCPConnectLocalIP();
			if (l_strHcpConnectLocalIP.empty())
			{
				l_strHcpConnectLocalIP = m_strLocalServerIP;
			}
			std::string l_strCallbackUrl = l_strHcpConnectLocalIP + ":" + CSysConfig::Instance()->GetCTIServerPort();
			l_pCmdRequest->SetParam(CTI_PARAM_callback_url, l_strCallbackUrl);

			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
			ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
		}
		
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesLogoutRequest()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AES_LOGOUT_CMD);
	if (l_pCmdRequest)
	{
		std::string l_strFSLoginUser = CSysConfig::Instance()->GetAESUserName();
		std::string l_strFSLoginPswd = CSysConfig::Instance()->GetAESPassword();
		l_pCmdRequest->SetParam(CTI_PARAM_user_name, l_strFSLoginUser);
		l_pCmdRequest->SetParam(CTI_PARAM_user_pwd, l_strFSLoginPswd);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesHeartBeatRequest()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_AES_HEART_BEAT_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesGetFreeAgentListRequest()
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_AGENT_LIST_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId= CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesGetExtensionListRequest()
{
	//从AES获取分机列表
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(GET_EXTENSION_LIST_CMD);
	if (l_pCmdRequest)
	{
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesMonitorRequest(int l_nDeviceType)
{
	IFSAesCmdRequestPtr l_pCmdRequest;
	std::string l_strDsts;
	std::string l_strKey;
	if (l_nDeviceType == CTI_DEVICE_TYPE_ACD)
	{
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MONITOR_ACD_CMD);
		l_strKey = CTI_PARAM_skill;
		l_strDsts = CDeviceManager::Instance()->GetACDListString();
	}
	else if(l_nDeviceType == CTI_DEVICE_TYPE_AGENT)
	{
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MONITOR_AGENT_CMD);
		l_strKey = CTI_PARAM_agentid;
		l_strDsts = CDeviceManager::Instance()->GetAgentListString();
	}
	else
	{
		l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_MONITOR_DEVICE_CMD);
		l_strKey = CTI_PARAM_extension;
		l_strDsts = CDeviceManager::Instance()->GetDeviceListString();
	}
	
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(l_strKey, l_strDsts);
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u],cmd:[%s]", l_lRequestId, GetRequestCmdString(l_pCmdRequest->GetRequestCmdType()).c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesMemberMuteRequest(const std::string& p_strConferenceId,const std::string &p_strConferenceName, const std::string& p_strCallId, const std::string& p_strTarget)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFMEMBER_MUTE_CMD);
	if (l_pCmdRequest)
	{
		//l_pCmdRequest->SetParam(CTI_PARAM_moderator, p_strTarget); //
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCallId); //
		l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, p_strConferenceId); //
		l_pCmdRequest->SetParam(CTI_PARAM_conferencename, p_strConferenceName); //
		l_pCmdRequest->SetParam(CTI_PARAM_member, p_strTarget); //

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%d]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesReleaseConferenceRequest(const std::string& p_strConferenceId, const std::string& p_strConferenceName, const std::string& p_strCallId)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_RELEASE_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCallId); //
		l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, p_strConferenceId); 
		l_pCmdRequest->SetParam(CTI_PARAM_conferencename, p_strConferenceName); 
		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u]", l_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesAutoAnswserCallRequest(const std::string& p_strCallId, const std::string& p_strDeviceNum)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_ANSWER_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_extension, p_strDeviceNum);
		l_pCmdRequest->SetParam(CTI_PARAM_agentid, CDeviceManager::Instance()->GetAgentID(p_strDeviceNum));
		l_pCmdRequest->SetParam(CTI_PARAM_number, p_strDeviceNum);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u], answer call, CallRefId [%s], Device [%s]",
			l_lRequestId, p_strCallId.c_str(), p_strDeviceNum.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::_AesAutoHangupCallRequest(const std::string& p_strCallId, const std::string& p_strDeviceNum)
{
	IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_REFUSE_CALL_CMD);
	if (l_pCmdRequest)
	{
		l_pCmdRequest->SetParam(CTI_PARAM_callid, p_strCallId);
		l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, p_strDeviceNum);

		long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask.requestId:[%u], auto hangup call, CallRefId [%s], Device [%s]",
			l_lRequestId, p_strCallId.c_str(), p_strDeviceNum.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
void CFSAesSwitchManager::OnRecviceFSAesMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody)
{
	//ICC_LOG_DEBUG(m_pLog, "recv http message from aes server,cmd:%s,start:[%d],body:[%s]", p_strTarget.c_str(), m_bStart, p_strBody.c_str());
	//ICC_LOG_DEBUG(m_pLog, "recv http message,cmd:%s,start:[%d],body:[%s]", p_strTarget.c_str(), m_bStart, p_strBody.c_str());
	if (!m_bStart)
	{
		return;
	}
	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();

	if (!l_JsonPtr)
	{
		ICC_LOG_ERROR(m_pLog, "create json object failed.");
		return;
	}

	if (!l_JsonPtr->LoadJson(p_strBody))
	{
		ICC_LOG_ERROR(m_pLog, "load json[%s] object failed.");
		return;
	}
	//l_JsonPtr->SetNodeValue("/header/src_ip", p_strSrcIP);
	//l_JsonPtr->SetNodeValue("/header/msgid", p_strGuid);
	//l_JsonPtr->SetNodeValue("/header/cmd", p_strTarget);
	//l_JsonPtr->SetNodeValue("/header/request", "http");
	std::string l_strRerquestMsg = l_JsonPtr->ToString();
	if (_isRequestResultMsg(p_strTarget))
	{
		ICC_LOG_DEBUG(m_pLog, "recv result from aes server,cmd:%s,start:[%d],body:[%s]", p_strTarget.c_str(), m_bStart, p_strBody.c_str());
		_DispRecvRequestResult(l_strRerquestMsg);
	}
	else
	{
		//事件通知
		_DispRecvHCPNotifyEvent(p_strTarget,l_strRerquestMsg);
	}
}

void CFSAesSwitchManager::OnCheckCallTimeOut(const std::string &p_strCallId, const std::string& p_strCallState,const std::string& p_strCaller, const std::string& p_strCalled)
{
	ICC_LOG_WARNING(m_pLog, "check call time out cti server,cmd_state:[%s],CTICallRefId:[%s]", p_strCallState.c_str(),p_strCallId.c_str());

	CHcpDeviceCallEvent l_HcpDeviceStateEvt;
	l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId = p_strCallId;
	l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum = p_strCalled;
	l_HcpDeviceStateEvt.m_oBody.m_strTimeOutFlag = "1";
	//l_HcpDeviceStateEvt.m_oBody.m_strCaller = p_strCaller;
	//l_HcpDeviceStateEvt.m_oBody.m_strCalled = p_strCalled;

	std::string p_strNotifyEvent = l_HcpDeviceStateEvt.ToString(CTIEVT_CALL_HANGUP, m_pJsonFty->CreateJson());
	ICHCPNotifyEeventPtr l_pHcpEvent = boost::make_shared<CHCPNotifyEevent>(CTIEVT_CALL_HANGUP, p_strNotifyEvent);
	if (l_pHcpEvent)
	{
		CTaskManager::Instance()->AddSwitchEventTask(Task_HCP_Notify_Event, l_pHcpEvent);
	}
}

void CFSAesSwitchManager::_DispRecvHCPNotifyEvent(const std::string p_strEventName,const std::string& p_strNotifyEvent)
{
	ICHCPNotifyEeventPtr l_pHcpEvent = boost::make_shared<CHCPNotifyEevent>(p_strEventName, p_strNotifyEvent);
	if (l_pHcpEvent)
	{
		CTaskManager::Instance()->AddSwitchEventTask(Task_HCP_Notify_Event, l_pHcpEvent);
	}
}

void CFSAesSwitchManager::_DispRecvRequestResult(const std::string& p_strResultMsg)
{
	CAsyncResultEvent l_pResultEvent;
	if (l_pResultEvent.ParseString(p_strResultMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		std::string l_strMsgId = l_pResultEvent.m_strMsgID;
		ITaskPtr l_pExcutedTask = CTaskManager::Instance()->GetExecutedTask(l_strMsgId);
		if (l_pExcutedTask)
		{
			//获取对应的请求消息
			IFSAesCmdRequestPtr l_pAesCmdRequest = boost::dynamic_pointer_cast<CFSAesRequestCmd>(l_pExcutedTask->GetSwitchNotif());

			IHcpAsyncResultMsgPtr l_pAsyncResultMsg = boost::make_shared<CHCPAsyncResultMsg>(l_pAesCmdRequest);
			if (!l_pAsyncResultMsg)
			{
				ICC_LOG_ERROR(m_pLog, "Create Notif Object Failed, RequestId: [%u],msgId:[%s]", l_pAesCmdRequest->GetRequestId(), l_strMsgId.c_str());
				return;
			}
			
			l_pAsyncResultMsg->SetRequestId(l_pAesCmdRequest->GetRequestId());

			if (l_pResultEvent.m_strResult == "0")
			{
				l_pAsyncResultMsg->SetResult(true);
				l_pAsyncResultMsg->SetErrorMsg("");

				l_pAsyncResultMsg->SetResultParams(l_pResultEvent.m_tbResultParam);
			}
			else
			{
				l_pAsyncResultMsg->SetErrorCode(atoi(l_pResultEvent.m_strResult.c_str()));
				l_pAsyncResultMsg->SetErrorMsg(l_pResultEvent.m_strMessage);
			}

			//从执行任务队列中删除
			CTaskManager::Instance()->DeleteExecutedTask(l_strMsgId);
			//加到消息处理返回队列
			CTaskManager::Instance()->AddSwitchEventTask(Task_ctiAsyncRequest_Resp, l_pAsyncResultMsg);
			ICC_LOG_DEBUG(m_pLog, "disp async request result msg.requestId:[%d],msgId:[%s]", l_pAesCmdRequest->GetRequestId(), l_strMsgId.c_str());
		}
		else
		{
			//没有找到对应任务
			ICC_LOG_FATAL(m_pLog, "not find exec task,msgId:[%s]", l_strMsgId.c_str());
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.msg:[%s]", p_strResultMsg.c_str());
	}
}

void CFSAesSwitchManager::_ExeCommand(int p_nTaskType, long p_lTaskId, ITaskPtr p_pTask)
{
	int l_nResult = RESULT_SUCCESS;
	IFSAesCmdRequestPtr l_pAesCmdRequest = boost::dynamic_pointer_cast<CFSAesRequestCmd>(p_pTask->GetSwitchNotif());
	if(p_nTaskType == Task_ctiLocal_Query_cmd)
	{
		l_nResult = _DispLocalGetCmd( p_lTaskId, l_pAesCmdRequest);
		CTaskManager::Instance()->DeleteCmdTask(p_lTaskId);
		return;
	}
	else if (p_nTaskType == Task_ctiLocal_Set_cmd)
	{
		//本地设置，如黑名单等
		CTaskManager::Instance()->DeleteCmdTask(p_lTaskId);
		return;
	}
	else if (p_nTaskType == Task_ctiAsyncRquest_Cmd || p_nTaskType == Task_ctiSyncRquest_Cmd)
	{
		//需要发给AES处理的请求
		//检查请求参数是否正确
		if (l_pAesCmdRequest->GetParamError() != ERROR_CMD_SUCCESS)
		{
			l_nResult = l_pAesCmdRequest->GetParamError();//参数错误
		}
		else
		{
			if (!m_bLoginAesSuccess && l_pAesCmdRequest->GetRequestCmdType() != REQUEST_AES_LOGIN_CMD)
			{
				l_nResult = ERROR_NET_DISCONNECT;
			}
			else
			{
				std::string l_strCTICallid = l_pAesCmdRequest->GetRequestParam(CTI_PARAM_callid);
				if (!l_strCTICallid.empty())
				{
					//将CTI定义的CallId转换成AES服务的CallId
					std::string l_strHcpCallId = CCallManager::Instance()->GetHcpCallId(l_strCTICallid);
					if (!l_strHcpCallId.empty())
					{
						l_pAesCmdRequest->SetCTICallRefId(l_strCTICallid);
						l_pAesCmdRequest->SetParam(CTI_PARAM_callid, l_strHcpCallId);
						ICC_LOG_DEBUG(m_pLog, "get hcp callId ok,ctiCallId:[%s],hcpCallId:[%s]", l_strCTICallid.c_str(), l_strHcpCallId.c_str());
					}
					else
					{
						//话务无效
						l_nResult = ERROR_INVALID_CALL;
						ICC_LOG_ERROR(m_pLog, "get hcp callId failed,ctiCallId:[%s],hcpCallId:[%s]", l_strCTICallid.c_str(), l_strHcpCallId.c_str());
					}
				}
			}
		}
		
		std::string l_strMsgId = m_pStringPtr->CreateGuid();
		std::string l_strTaskName = GetRequestCmdString(l_pAesCmdRequest->GetRequestCmdType());
		string l_strResponse = "";
		//参数没有错误 向AES发请求
		if(l_nResult == RESULT_SUCCESS && m_pFsAesHttpHelper)
		{
			if (p_nTaskType == Task_ctiAsyncRquest_Cmd)
			{
				CTaskManager::Instance()->AddExecutedTask(l_strMsgId, p_pTask);// 添加到执行队列
				ICC_LOG_DEBUG(m_pLog, "End Cmd Task, Cmd Invoke success.cmdName:[%s] TaskId:[%u],msgID:[%s]", l_strTaskName.c_str(), p_lTaskId, l_strMsgId.c_str());
			}
			m_pFsAesHttpHelper->SendRequestToFSAes(l_strMsgId, l_pAesCmdRequest, l_strResponse);
		}
		
		//
		if (p_nTaskType == Task_ctiAsyncRquest_Cmd)
		{
			CCTISyncResponse l_CTISyncResponse;
			if (l_CTISyncResponse.ParseString(l_strResponse, ICCGetIJsonFactory()->CreateJson()))
			{
				if (l_CTISyncResponse.m_strCode == HTTP_SUCCESS_CODE)
				{
					l_nResult = RESULT_SUCCESS;
				}
			}
			if (l_nResult != RESULT_SUCCESS)
			{
				ICC_LOG_ERROR(m_pLog, "End Cmd Task,Cmd Invoke failed.cmdName:[%d] TaskId:[%u] ErrorCode:[%d]", l_strTaskName.c_str(), p_lTaskId, l_nResult);
				// 调用命令失败,通知应用层
				_DispAsyncRequestFailedCmd(l_nResult, l_pAesCmdRequest);
				CTaskManager::Instance()->DeleteExecutedTask(l_strMsgId);
			}
		}
		else
		{
			_DispSyncRquestResultMsg(l_pAesCmdRequest->GetRequestCmdType(), l_pAesCmdRequest->GetRequestId(),l_strResponse);
		}

		//从任务队列中删除
		CTaskManager::Instance()->DeleteCmdTask(p_lTaskId);
	}
}


int CFSAesSwitchManager::_DispSyncRquestResultMsg(int p_nRequestType,int p_nRequestId, const std::string  &p_strResponse)
{
	IHcpSyncResultMsgPtr l_pReqstResultMsg = boost::make_shared<CHCPSyncResultMsg>();
	if (l_pReqstResultMsg)
	{
		l_pReqstResultMsg->SetRequestCmdType(p_nRequestType);
		l_pReqstResultMsg->SetResponse(p_strResponse);
		l_pReqstResultMsg->SetRequestId(p_nRequestId);
		//加到消息处理返回队列
		CTaskManager::Instance()->AddSwitchEventTask(Task_ctiSyncRequest_Resp, l_pReqstResultMsg);
		ICC_LOG_DEBUG(m_pLog, "disp sync request resp msg.requestId:[%d],requestType:[%d],ReqstResultMsg:[%s]", p_nRequestId, p_nRequestType, p_strResponse.c_str());
	}
	return RESULT_SUCCESS;
}

//
int CFSAesSwitchManager::_DispLocalGetCmd(long p_lTaskId, IFSAesCmdRequestPtr p_pAesCmdRequest)
{
	long l_lRequestId = p_pAesCmdRequest->GetRequestId();
	IHcpAsyncResultMsgPtr l_pReqstResultMsg = boost::make_shared<CHCPAsyncResultMsg>(p_pAesCmdRequest);

	l_pReqstResultMsg->SetRequestId(l_lRequestId);
	l_pReqstResultMsg->SetRequestCmdType(p_pAesCmdRequest->GetRequestCmdType());
	//加到消息处理返回队列
	CTaskManager::Instance()->AddSwitchEventTask(Task_ctiLocal_Query_Resp, l_pReqstResultMsg);
	ICC_LOG_DEBUG(m_pLog, "disp local request msg.requestId:[%d],requestType:[%d]", l_lRequestId, p_pAesCmdRequest->GetRequestCmdType());
	return RESULT_SUCCESS;
}


void CFSAesSwitchManager::_ExeLocalGetEvent(long p_lTaskId, ITaskPtr p_pTask)
{
	int l_nResult = ERROR_UNKNOWN;
	std::string l_strTaskName;
	IHcpAsyncResultMsgPtr l_pFSAesReqstResult = boost::dynamic_pointer_cast<CHCPAsyncResultMsg>(p_pTask->GetSwitchNotif());
	int l_pRequestCmdType = l_pFSAesReqstResult->GetRequestCmdType();
	switch (l_pRequestCmdType)
	{
	case GET_CTICONNECT_STATE_CMD: //获取 CTI 网络状态结果返回
	{
		IGetCTIConnStateResultNotifPtr l_pResultNotif = boost::make_shared<CGetCTIConnStateResultNotif>();
		if (l_pResultNotif)
		{
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());
			l_pResultNotif->SetSwitchType("3");
			if (m_bLoginAesSuccess)
			{
				l_pResultNotif->SetStreamOpen(true);
			}
			else
			{
				l_pResultNotif->SetStreamOpen(false);
			}
			_DispatchRequestResult("CR_GetCTIConnState", l_pResultNotif);
		}
		break;
	}
	case GET_DEVICE_LIST_CMD: //获取话机设备状态结果返回
	{
		IGetDeviceListResultNotifPtr l_pResultNotif = boost::make_shared<CGetDeviceListResultNotif>();
		if (l_pResultNotif)
		{
			if (CDeviceManager::Instance()->GetDeviceList(l_pResultNotif))
			{
				l_pResultNotif->SetResult(true);
			}
			else
			{
				l_pResultNotif->SetResult(false);
			}
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());
			_DispatchRequestResult("CR_GetDeviceList", l_pResultNotif);
		}
		break;
	}
	case GET_CALL_LIST_CMD:
	{
		IGetCallListResultNotifPtr l_pResultNotif = boost::make_shared<CGetCallListResultNotif>();
		if (l_pResultNotif)
		{
			if (CCallManager::Instance()->GetCallList(l_pResultNotif))
			{
				l_pResultNotif->SetResult(true);
			}
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());

			_DispatchRequestResult("CR_GetCallList", l_pResultNotif);
		}
		
		break;
	}
	case GET_ACD_LIST_CMD:
	{
		IGetACDListResultNotifPtr l_pResultNotif = boost::make_shared<CGetACDListResultNotif>();
		if (l_pResultNotif)
		{
			if (CAgentManager::Instance()->GetACDList(l_pResultNotif))
			{
				l_pResultNotif->SetResult(true);
			}
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());

			_DispatchRequestResult("CR_GetACDList", l_pResultNotif);
		}
		break;
	}
	case GET_READY_AGENT_CMD: //GetReadyAgent获取 ACD 组一个空闲的 Agent
	{
		IGetReadyAgentResultNotifPtr l_pResultNotif = boost::make_shared<CGetReadyAgentResultNotif>();
		if (l_pResultNotif)
		{
			std::string l_strACDGrp = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_skill);
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());
			if (CAgentManager::Instance()->GetAgent(l_pResultNotif, l_strACDGrp))
			{
				l_pResultNotif->SetResult(true);
			}
			else
			{
				l_pResultNotif->SetResult(false);
			}
			_DispatchRequestResult("CR_GetReadyAgent", l_pResultNotif);
		}
		break;
	}
	case GET_AGENT_LIST_CMD:
	{
		IGetAgentListResultNotifPtr l_pResultNotif = boost::make_shared<CGetAgentListResultNotif>();
		if (l_pResultNotif)
		{
			std::string l_strACDGrp = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_skill);
			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());
			if (CAgentManager::Instance()->GetAgent(l_pResultNotif, l_strACDGrp))
			{
				l_pResultNotif->SetResult(true);
			}
			else
			{
				l_pResultNotif->SetResult(false);
			}
			_DispatchRequestResult("CR_GetAgentList", l_pResultNotif);
		}
		break;
	}
	case GET_FREE_AGENT_LIST_CMD: //获取所有空闲坐席，按照部门号和acd过滤 
	{
		IGetFreeAgentResultNotifPtr l_pResultNotif = boost::make_shared<CGetFreeAgentResultNotif>();
		if (l_pResultNotif)
		{
			std::string l_strACDGrp = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_skill);

			l_pResultNotif->SetRequestId(l_pFSAesReqstResult->GetRequestId());
			l_pResultNotif->SetDeptCode(l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_DEPT_CODE));
			if (CAgentManager::Instance()->GetAgent(l_pResultNotif, l_strACDGrp))
			{
				l_pResultNotif->SetResult(true);
			}
			else
			{
				l_pResultNotif->SetResult(false);
			}
			_DispatchRequestResult("CR_GetFreeAgentList", l_pResultNotif);
		}
		break;
	}
	default:
		break;
	}
}

void CFSAesSwitchManager::_DispAsyncRequestFailedCmd(int p_nErrorCode, IFSAesCmdRequestPtr p_pAesCmdRequest)
{
	long l_lRequestId = p_pAesCmdRequest->GetRequestId();
	ICC_LOG_ERROR(m_pLog, "Request failed,ErrorCode:[%u],cmdNameId:[%d],RequestId:[%d]", p_nErrorCode, p_pAesCmdRequest->GetRequestCmdType(), l_lRequestId);
	IHcpAsyncResultMsgPtr l_pResultNotif = boost::make_shared<CHCPAsyncResultMsg>(p_pAesCmdRequest);
	if (!l_pResultNotif)
	{
		ICC_LOG_ERROR(m_pLog, "Create Notif Object Failed, RequestId: [%u]", l_lRequestId);
		return;
	}
	l_pResultNotif->SetRequestId(l_lRequestId);
	l_pResultNotif->SetRequestCmdType(p_pAesCmdRequest->GetRequestCmdType());
	l_pResultNotif->SetResult(false);
	l_pResultNotif->SetErrorCode(p_nErrorCode);
	l_pResultNotif->SetErrorMsg("");

	//加到消息处理返回队列
	CTaskManager::Instance()->AddSwitchEventTask(Task_ctiAsyncRequest_Resp, l_pResultNotif);
	ICC_LOG_DEBUG(m_pLog, "disp async request result msg(error).requestId:[%d],requestType:[%d],errorCode:[%d]", l_lRequestId, p_pAesCmdRequest->GetRequestCmdType(), p_nErrorCode);
}

ISwitchResultNotifPtr _CreateRequestResultEvent(int l_nCmdType, IHcpAsyncResultMsgPtr p_pFSAesReqstResult)
{
	ISwitchResultNotifPtr l_pResultNotif;
	switch (l_nCmdType)
	{
	case REQUEST_AGENT_LOGIN_CMD:
	case REQUEST_AGENT_LOGOUT_CMD:
	case REQUEST_SET_AGENTIDLE_CMD:
	case REQUEST_SET_AGENTBUSY_CMD:
	{
		ISetAgentStateResultNotifPtr l_pAgentStateResultNotif = boost::make_shared<CSetAgentStateResultNotif>();
		if (l_pAgentStateResultNotif)
		{
			l_pAgentStateResultNotif->SetAgentId(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_extension));//ICC agentID返回的是分机号
			l_pAgentStateResultNotif->SetACDGrp(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_skill));
			std::string l_strLoginMode = "login";
			if (REQUEST_AGENT_LOGOUT_CMD == l_nCmdType)
			{
				l_strLoginMode = "logout";
			}
			l_pAgentStateResultNotif->SetLoginMode(l_strLoginMode);
			if (l_nCmdType == REQUEST_SET_AGENTBUSY_CMD)
			{
				l_pAgentStateResultNotif->SetReadyState(ReadyState_Busy);
			}
			else
			{
				l_pAgentStateResultNotif->SetReadyState(ReadyState_Idle);
			}
		}
		l_pResultNotif = l_pAgentStateResultNotif;
		break;
	}	
	case REQUEST_MAKE_CALL_CMD:
	{
		IMakeCallResultNotifPtr l_pMakeCallResultNotif = boost::make_shared<CMakeCallResultNotif>();
		if (l_pMakeCallResultNotif)
		{
			l_pMakeCallResultNotif->SetCallerId(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_caller));
			l_pMakeCallResultNotif->SetCalledId(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_called));
			l_pMakeCallResultNotif->SetCaseId(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_caseid));
		}
		l_pResultNotif = l_pMakeCallResultNotif;
		break;
	}
	case REQUEST_CONSULTATION_CALL_CMD:
	{
		IConsultationCallResultNotifPtr l_pConsultationResultNotif = boost::make_shared<CConsultationCallResultNotif>();
		if (l_pConsultationResultNotif)
		{
			l_pConsultationResultNotif->SetSponsor(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_caller));
			l_pConsultationResultNotif->SetTargetDevice(p_pFSAesReqstResult->GetRequestParam(CTI_PARAM_called));
			l_pConsultationResultNotif->SetCallHandle(0);
		}
		l_pResultNotif = l_pConsultationResultNotif;
		break;
	}
	case REQUEST_TRANSFER_EX_CALL_CMD:
	case REQUEST_TRANSFER_CALL_CMD:
	{
		ITransferCallResultNotifPtr l_pTransferCallResultNotif = boost::make_shared<CTransferCallResultNotif>();

		//l_pTransferCallResultNotif->SetActiveCSTACallRefId();
		l_pTransferCallResultNotif->SetActiveCTICallRefId(p_pFSAesReqstResult->GetCTICallRefId());
		l_pTransferCallResultNotif->SetHeldCTICallRefId(p_pFSAesReqstResult->GetCTICallRefId());
		//l_pTransferCallResultNotif->SetHeldCSTACallRefId();
		//l_pTransferCallResultNotif->SetHeldCTICallRefId();
		l_pTransferCallResultNotif->SetSponsor(p_pFSAesReqstResult->GetRequestParam(CTI_PRAM_src_number));
		l_pTransferCallResultNotif->SetTargetDevice(p_pFSAesReqstResult->GetRequestParam(CTI_PRAM_dest_number));

		l_pResultNotif = l_pTransferCallResultNotif;
		break;
	}
	case REQUEST_TAKEOVER_CALL_CMD:
	{
		ITakeOverCallResultNotifPtr l_pTakeOverCallResultNotif = boost::make_shared<CTakeOverCallResultNotif>();
		if (l_pTakeOverCallResultNotif)
		{
			l_pTakeOverCallResultNotif->SetSponsor(p_pFSAesReqstResult->GetRequestParam(CTI_PRAM_dest_number));
			l_pTakeOverCallResultNotif->SetTarget(p_pFSAesReqstResult->GetRequestParam(CTI_PRAM_src_number));
		}
		l_pResultNotif = l_pTakeOverCallResultNotif;
		break;
	}
	default:
		l_pResultNotif = boost::make_shared<CSwitchResultNotif>();
		break;
	}

	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_pFSAesReqstResult->GetRequestId());
		l_pResultNotif->SetResult(p_pFSAesReqstResult->GetResult());
		l_pResultNotif->SetErrorCode(p_pFSAesReqstResult->GetErrorCode());
		l_pResultNotif->SetErrorMsg(p_pFSAesReqstResult->GetErrorMsg());

		//话务ID
		l_pResultNotif->SetCTICallRefId(p_pFSAesReqstResult->GetCTICallRefId());
	}
	return l_pResultNotif;
}

void CFSAesSwitchManager::_AllCallOver()
{
	std::map<std::string, ICTICallPtr> l_mapCalls;
	if (CCallManager::Instance()->GetAllCall(l_mapCalls))
	{
		for (auto l_callIter : l_mapCalls)
		{
			PostCallOver(l_callIter.first, l_callIter.second, HangupTypeString[HANGUP_TYPE_UNKNOWN]);
		}
	}
}

void CFSAesSwitchManager::_DispatchRequestResult(const std::string& p_strCallbackFunName, ISwitchResultNotifPtr p_pResultNotify,const std::string& p_strCmdName)
{
	if (p_pResultNotify)
	{
		m_CallbackInterface.DispatchRequestResult(p_strCallbackFunName, p_pResultNotify, p_strCmdName);
	}
}

void CFSAesSwitchManager::DispatchCTIEvent(const std::string& p_strCallbackFunName, ISwitchNotifPtr m_pSwitchNotify, const std::string& p_strCmdName)
{
	if (m_pSwitchNotify)
	{
		if (p_strCallbackFunName == "Event_DeviceState")
		{
			IDeviceStateNotifPtr l_pDeviceNotify = boost::dynamic_pointer_cast<CDeviceStateNotif>(m_pSwitchNotify);
			if (l_pDeviceNotify)
			{
				std::string l_strCallRefId = l_pDeviceNotify->GetCTICallRefId();
				std::string l_strCompere;
				std::string l_strBargeInSponsor;
				std::string l_strConferenceId = CConferenceManager::Instance()->QueryConferenceByCallid(l_strCallRefId, l_strCompere, l_strBargeInSponsor);
				if (!l_strConferenceId.empty() && l_pDeviceNotify->GetDeviceState() != CallStateString[STATE_SILENTMONITOR])
				{
					l_pDeviceNotify->SetConferenceId(l_strConferenceId);
					l_pDeviceNotify->SetConfCompere(l_strCompere);
					if (!l_strBargeInSponsor.empty())
					{
						l_pDeviceNotify->SetConfBargeinSponsor(l_strBargeInSponsor);
					}

					if (l_pDeviceNotify->GetDeviceState() == CallStateString[STATE_TALK] && l_strBargeInSponsor.empty())
					{
						l_pDeviceNotify->SetDeviceState(CallStateString[STATE_CONFERENCE]);
					}
				}
			}
		}
		m_CallbackInterface.DispatchCTIEvent(p_strCallbackFunName, m_pSwitchNotify, p_strCmdName);
	}
	
}

//同步返回结果处理
void CFSAesSwitchManager::_ExeSyncResultEvent(long p_lTaskId, ITaskPtr p_pTask)
{
	IHcpSyncResultMsgPtr l_pSyncResult = boost::dynamic_pointer_cast<CHCPSyncResultMsg>(p_pTask->GetSwitchNotif());
	if (!l_pSyncResult)
	{
		return;
	}
	int l_pRequestCmdType = l_pSyncResult->GetRequestCmdType();
	CCTISyncResponse l_CTISyncResponse;
	
	ICC_LOG_DEBUG(m_pLog, "Sync result return.TaskId:[%u],cmdType:[%d],cmdName:[%s],requestId:[%d]", 
		p_lTaskId, l_pRequestCmdType, GetRequestCmdString(l_pRequestCmdType).c_str(), l_pSyncResult->GetRequestId());
	switch (l_pRequestCmdType)
	{
	case REQUEST_AES_LOGIN_CMD:
	case REQUEST_AES_LOGOUT_CMD:
	{
		if (!l_CTISyncResponse.ParseString(l_pSyncResult->GetResponse(), ICCGetIJsonFactory()->CreateJson()))
		{
			l_CTISyncResponse.m_strCode = "";
		}

		if (l_CTISyncResponse.m_strCode == HTTP_SUCCESS_CODE)
		{
			if (l_pRequestCmdType == REQUEST_AES_LOGIN_CMD)
			{
				m_nHeartBeatErrorCount = 0;
				m_nSendLoginCount = 0;
				m_bLoginAesSuccess = true; //登录AES服务端成功
				m_pFsAesHttpHelper->SetCTIClientID(l_CTISyncResponse.m_strClientID);

				ICC_LOG_DEBUG(m_pLog, "hcp login sucess,clientId:%s !", l_CTISyncResponse.m_strClientID.c_str());

				_AesMonitorRequest(CTI_DEVICE_TYPE_ACD);
				m_myTimer.AddTimer(this, TIMERID_AES_acdMonitor, 5000);

				_AesMonitorRequest(CTI_DEVICE_TYPE_AGENT);
				m_myTimer.AddTimer(this, TIMERID_AES_agentMonitor, 5000);

				_AesMonitorRequest(CTI_DEVICE_TYPE_PHONE);
				m_myTimer.AddTimer(this, TIMERID_AES_deviceMonitor, 5000);

				ICTIConnStateNotifPtr l_pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
				if (l_pAESStateNotif)
				{
					l_pAESStateNotif->SetStreamOpen(true);
					l_pAESStateNotif->SetSwitchType(SWITCH_TYPE_FREESWITCH);

					ICC_LOG_DEBUG(m_pLog, "report event aes connect success.");
					//m_pSwitchEventCallback->Event_CTIConnState(l_pAESStateNotif);
					DispatchCTIEvent("Event_CTIConnState", l_pAESStateNotif);
				}
			}
			else
			{
				//登出AES服务端成功
				m_nSendLoginCount = 0;
				m_bLoginAesSuccess = false; 
				m_bLoadExtensionFlag = false;
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "hcp login error,errorCode:%s,CmdType:%d,SendLoginCount:[%d]!!!!", l_CTISyncResponse.m_strCode.c_str(), l_pRequestCmdType, m_nSendLoginCount);
			if (l_pRequestCmdType == REQUEST_AES_LOGIN_CMD && m_nSendLoginCount >= 3 && CCallManager::Instance()->GetCallCount() > 0)
			{
				m_nSendLoginCount = 0;
				CDeviceManager::Instance()->ClearAllDeviceCallInfo();
				_AllCallOver();
				CCallManager::Instance()->ReleaseAllCall();
				CConferenceManager::Instance()->ReleaseAllConference();
			}
		}
		break;
	}
	case REQUEST_AES_HEART_BEAT_CMD:
	{
		if (!l_CTISyncResponse.ParseString(l_pSyncResult->GetResponse(), ICCGetIJsonFactory()->CreateJson()))
		{
			l_CTISyncResponse.m_strCode = HTTP_BadRequest;
		}

		if (l_CTISyncResponse.m_strCode != HTTP_SUCCESS_CODE)
		{
			ICC_LOG_ERROR(m_pLog, "HCP heart beat error,errorCode:%s,BeatErrorCount:%d!!!!", l_CTISyncResponse.m_strCode.c_str(), m_nHeartBeatErrorCount);
			if (HTTP_Unauthorized == l_CTISyncResponse.m_strCode)
			{
				//m_pFsAesHttpHelper->SetCTIClientID("");
				//重新登录
				m_nAESLoginTick = 3;
				m_nSendLoginCount = 0;
				m_bLoginAesSuccess = false; 
				m_bLoadExtensionFlag = false;
			}
			else
			{
				m_nHeartBeatErrorCount++;
				if (m_nHeartBeatErrorCount > 4)
				{
					m_nHeartBeatErrorCount = 0;
					m_pFsAesHttpHelper->SetCTIClientID("");
					//重新登录
					m_nAESLoginTick = 3;
					m_nSendLoginCount = 0;
					m_bLoginAesSuccess = false; //登出AES服务端成功
					m_bLoadExtensionFlag = false;

					CDeviceManager::Instance()->ClearAllDeviceCallInfo();
					_AllCallOver();
					CCallManager::Instance()->ReleaseAllCall();
					CConferenceManager::Instance()->ReleaseAllConference();
				}
			}
		}
		else
		{
			//心跳返回成功
			m_nHeartBeatErrorCount = 0;
		}
		break;
	}
		
	case GET_EXTENSION_LIST_CMD:
	{
		CResponseGetExtensions l_extensionListResp;
		if (!l_extensionListResp.ParseString(l_pSyncResult->GetResponse(), ICCGetIJsonFactory()->CreateJson()))
		{
			break;
		}
		if (l_extensionListResp.m_oBody.m_strCode == HTTP_SUCCESS_CODE)
		{
			m_myTimer.RemoveTimer(TIMERID_AES_getExtensionList);
			m_bLoadExtensionFlag = true;

			CDeviceManager::Instance()->SaveAESExtensionInfo(l_extensionListResp.m_oBody.m_vecData);
			ICC_LOG_DEBUG(m_pLog, "get aes extension success,count:[%d]!!!", l_extensionListResp.m_oBody.m_vecData.size());
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "get aes extension failed!!!");
		}
		break;
	}
	case GET_AGENT_LIST_CMD:
	{
		CResponseGetFreeAgents l_freeAgentListResp;
		if (!l_freeAgentListResp.ParseString(l_pSyncResult->GetResponse(), ICCGetIJsonFactory()->CreateJson()))
		{
			break;
		}

		if (l_freeAgentListResp.m_oBody.m_strCode == HTTP_SUCCESS_CODE)
		{
			CAgentManager::Instance()->InitFreeAgentList(l_freeAgentListResp.m_oBody.m_vecData);
			ICC_LOG_DEBUG(m_pLog, "get aes free agent success,count:[%d]!!!", l_freeAgentListResp.m_oBody.m_vecData.size());
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "get aes free agent failed!!!");
		}
	}

	case REQUEST_MONITOR_AGENT_CMD:
	case REQUEST_MONITOR_ACD_CMD:
	case REQUEST_MONITOR_DEVICE_CMD:
	{
		CCTISyncResponse l_CTISyncResponse;
		if (!l_CTISyncResponse.ParseString(l_pSyncResult->GetResponse(), ICCGetIJsonFactory()->CreateJson()))
		{
			break;
		}
		if (l_CTISyncResponse.m_strCode == HTTP_SUCCESS_CODE)
		{
			if (l_pRequestCmdType == REQUEST_MONITOR_AGENT_CMD)
			{
				m_myTimer.RemoveTimer(TIMERID_AES_agentMonitor);

				_AesGetFreeAgentListRequest();
				_AesGetExtensionListRequest();
				m_myTimer.AddTimer(this, TIMERID_AES_getExtensionList, 5000);
			}
			else if(l_pRequestCmdType == REQUEST_MONITOR_DEVICE_CMD)
			{
				m_myTimer.RemoveTimer(TIMERID_AES_deviceMonitor);
			}
			else
			{
				m_myTimer.RemoveTimer(TIMERID_AES_acdMonitor);
			}
		}
		break;
	}
	case REQUEST_GET_HISTORY_CALL_LIST_CMD:
	{
		IFSAesExEventNotifPtr l_pFSAesExEventNotif = boost::make_shared<CFSAesExEventNotif>();
		if (l_pFSAesExEventNotif)
		{
			l_pFSAesExEventNotif->SetRequestId(l_pSyncResult->GetRequestId());
			
			l_pFSAesExEventNotif->SetRequestId(l_pSyncResult->GetRequestId());
			l_pFSAesExEventNotif->SetEventData(l_pSyncResult->GetResponse());

			_DispatchRequestResult("CR_FSAesEventEx", l_pFSAesExEventNotif, "get_history_callList");
			//m_pSwitchEventCallback->CR_FSAesEventEx("get_history_callList", l_pFSAesExEventNotif);
		}
		break;
	}
	default:
		break;
	}
}

//异步返回结果处理
void CFSAesSwitchManager::_ExeAsyncResultEvent(long p_lTaskId, ITaskPtr p_pTask)
{
	int l_nResult = ERROR_UNKNOWN;
	std::string l_strTaskName;
	IHcpAsyncResultMsgPtr l_pFSAesReqstResult = boost::dynamic_pointer_cast<CHCPAsyncResultMsg>(p_pTask->GetSwitchNotif());
	if (l_pFSAesReqstResult == nullptr)
	{
		return;
	}
	int l_pRequestCmdType = l_pFSAesReqstResult->GetRequestCmdType();

	ISwitchResultNotifPtr l_pSwitchReqstResult = _CreateRequestResultEvent(l_pRequestCmdType, l_pFSAesReqstResult);
	switch (l_pRequestCmdType)
	{
	case REQUEST_AGENT_LOGIN_CMD:
	case REQUEST_AGENT_LOGOUT_CMD:
	case REQUEST_SET_AGENTIDLE_CMD:
	case REQUEST_SET_AGENTBUSY_CMD:
	{
		E_AGENT_MODE_TYPE l_agentMode = AGENT_MODE_LOGIN;
		E_AGENT_READY_TYPE l_agentReadyType = AGENT_READY;
		ISetAgentStateResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CSetAgentStateResultNotif>(l_pSwitchReqstResult);
		std::string l_strReturnName;
		if (l_pRequestCmdType == REQUEST_AGENT_LOGIN_CMD)
		{
			l_strReturnName = "CR_AgentLogin";
			//m_pSwitchEventCallback->CR_AgentLogin(l_pResultNotif);
		}
		else if (l_pRequestCmdType == REQUEST_AGENT_LOGOUT_CMD)
		{
			l_agentMode = AGENT_MODE_LOGOUT;
			l_agentReadyType = AGENT_NOTREADY;
			l_strReturnName = "CR_AgentLogout";
			//m_pSwitchEventCallback->CR_AgentLogout(l_pResultNotif);
		}
		else if (REQUEST_SET_AGENTIDLE_CMD == l_pRequestCmdType)
		{
			//m_pSwitchEventCallback->CR_SetAgentState(l_pResultNotif);
			l_strReturnName = "CR_SetAgentState";
			l_agentReadyType = AGENT_READY;
		}
		else if (REQUEST_SET_AGENTBUSY_CMD == l_pRequestCmdType)
		{
			//m_pSwitchEventCallback->CR_SetAgentState(l_pResultNotif);
			l_strReturnName = "CR_SetAgentState";
			l_agentReadyType = AGENT_NOTREADY;
		}
		_DispatchRequestResult(l_strReturnName, l_pResultNotif);

		if (l_pResultNotif->GetResult())
		{
			//设置返回成功,更新坐席状态，并发送坐席状态通知
			CAgentManager::Instance()->ReadyStateSync(l_pResultNotif->GetAgentId(), l_pResultNotif->GetACDGrp(), l_agentMode, l_agentReadyType);
		}
		break;
	}
	case REQUEST_MAKE_CALL_CMD:
	{
		IMakeCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CMakeCallResultNotif>(l_pSwitchReqstResult);
		if (l_pResultNotif)
		{
			bool l_bCreateCall = false;
			std::string l_strHcpCallid = l_pFSAesReqstResult->GetResultParam(CTI_PARAM_extend_info);//返回结果中获取话务id
			if (l_pSwitchReqstResult->GetResult())
			{
				if (!l_strHcpCallid.empty())
				{
					if (CCallManager::Instance()->GetCallByHcpCallId(l_strHcpCallid) == nullptr)
					{
						l_bCreateCall = true;
						ICTICallPtr l_pCTICall = CCallManager::Instance()->CreateCTICallOut(l_strHcpCallid, l_pResultNotif->GetCallerId(), l_pResultNotif->GetCalledId());
						if (l_pCTICall)
						{
							l_pResultNotif->SetCTICallRefId(l_pCTICall->GetCTICallRefId());
						}
						CDeviceManager::Instance()->UpdateDeviceCallState(l_pResultNotif->GetCallerId(), l_strHcpCallid, CallStateString[STATE_DIAL]);
					}
				}
				else
				{
					l_pResultNotif->SetResult(false);
					l_pResultNotif->SetErrorCode(ERROR_INVALID_DEVICE);
				}
			}

			ICC_LOG_DEBUG(m_pLog, "makeCall result,res:[%d],RequestId:[%u],hcpCallId:[%s],createCallFlag:[%d]",
				l_pSwitchReqstResult->GetResult(), l_pFSAesReqstResult->GetRequestId(), l_strHcpCallid.c_str(), l_bCreateCall);

			//m_pSwitchEventCallback->CR_MakeCall(l_pResultNotif);
			_DispatchRequestResult("CR_MakeCall", l_pResultNotif);
		}
		break;
	}
	case REQUEST_CONSULTATION_CALL_CMD:
	{
		IConsultationCallResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CConsultationCallResultNotif>(l_pSwitchReqstResult);
		if (l_pResultNotif)
		{
			std::string l_strCTICallId = l_pSwitchReqstResult->GetCTICallRefId();

			ICC_LOG_DEBUG(m_pLog, "consultationCall result,res:[%d],RequestId:[%u],ctiCallId:[%s]",
				l_pSwitchReqstResult->GetResult(), l_pFSAesReqstResult->GetRequestId(), l_strCTICallId.c_str());

			l_pResultNotif->SetActiveCTICallRefId(l_pSwitchReqstResult->GetCTICallRefId());
			l_pResultNotif->SetHeldCTICallRefId(l_pSwitchReqstResult->GetCTICallRefId());
			if (!l_pSwitchReqstResult->GetResult())
			{
				//咨询返回失败
				CCallManager::Instance()->SetCallTransferType(l_pSwitchReqstResult->GetCTICallRefId(), TransferType_null, "");
			}
			else
			{
				l_pResultNotif->SetErrorCode(ERROR_UNKNOWN);
			}
			//m_pSwitchEventCallback->CR_ConsultationCall(l_pResultNotif);
			_DispatchRequestResult("CR_ConsultationCall", l_pResultNotif);
		}

		break;
	}
	case REQUEST_HOLD_CALL_CMD:			//呼叫保留
		//m_pSwitchEventCallback->CR_HoldCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_HoldCall", l_pSwitchReqstResult);
		break;
	case REQUEST_RETRIEVE_CALL_CMD:		//Call重拾,呼叫取回
		//m_pSwitchEventCallback->CR_RetrieveCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_RetrieveCall", l_pSwitchReqstResult);
		break;
	case REQUEST_ANSWER_CALL_CMD:		//话务应答
		//m_pSwitchEventCallback->CR_AnswerCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_AnswerCall", l_pSwitchReqstResult);
		break;
	case REQUEST_HANGUP_CALL_CMD:
		//m_pSwitchEventCallback->CR_Hangup(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_Hangup", l_pSwitchReqstResult);
		break;
	case REQUEST_REFUSE_CALL_CMD:
		//m_pSwitchEventCallback->CR_RefuseAnswer(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_RefuseAnswer", l_pSwitchReqstResult);
		break;
	case REQUEST_CLEAR_CALL_CMD:			//挂断整个话务
		//m_pSwitchEventCallback->CR_ClearCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ClearCall", l_pSwitchReqstResult);
		break;
	case REQUEST_TRANSFER_EX_CALL_CMD:
	case REQUEST_TRANSFER_CALL_CMD:	//呼叫转移 tzx?? 
	{
		if (!l_pSwitchReqstResult->GetResult())
		{
			//转移返回失败
			CCallManager::Instance()->SetCallTransferType(l_pSwitchReqstResult->GetCTICallRefId(), TransferType_null, "");
		}
		ICC_LOG_DEBUG(m_pLog, "call transfer result,ctiCallId:[%s],res:%d", l_pSwitchReqstResult->GetCTICallRefId().c_str(), l_pSwitchReqstResult->GetResult());

		//m_pSwitchEventCallback->CR_TransferCall(boost::dynamic_pointer_cast<CTransferCallResultNotif>(l_pSwitchReqstResult));
		_DispatchRequestResult("CR_TransferCall", l_pSwitchReqstResult);
		break;
	}
	case REQUEST_PICKUP_CALL_CMD:
		//m_pSwitchEventCallback->CR_PickupCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_PickupCall", l_pSwitchReqstResult);
		break;
	case REQUEST_THIRD_WAY_CMD:			//三方通话
		//m_pSwitchEventCallback->CR_T
		break;
	case REQUEST_MONITOR_CALL_CMD:
	{
		//m_pSwitchEventCallback->CR_ListenCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ListenCall", l_pSwitchReqstResult);
		//监听用会议实现
		std::string l_strConferenceName = l_pFSAesReqstResult->GetResultParam(CTI_PARAM_extend_info);//返回结果中获取会议id
		std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);

		std::string l_strCTICallId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_callid);
		std::string l_strTarget = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_moderator);
		if (l_pFSAesReqstResult->GetResult())
		{
			if (!l_strConferenceName.empty())
			{
				CConferenceManager::Instance()->InitCallMember(l_strCTIConferenceId, l_strConferenceName);
			}
		}

		ICC_LOG_DEBUG(m_pLog, "monitor create conference result,res:[%u],conferenceName:[%s],CTIConferenceId:[%s],callId:[%s],target:[%s]", 
			l_pFSAesReqstResult->GetResult(), l_strConferenceName.c_str(), l_strCTIConferenceId.c_str(), l_strCTICallId.c_str(), l_strTarget.c_str());

		break;
	}
	case REQUEST_MONITOR_EX_CALL_CMD:
	{
		//m_pSwitchEventCallback->CR_ListenCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ListenCall", l_pSwitchReqstResult);
		std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
		std::string l_strConferenceName = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_conferencename);
		std::string l_strCTICallId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_callid);
		std::string l_strTarget = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member);

		ICC_LOG_DEBUG(m_pLog, "monitorEx result,res:[%u],conferenceName:[%s],CTIConferenceId:[%s],callId:[%s],target:[%s]",
			l_pFSAesReqstResult->GetResult(), l_strConferenceName.c_str(), l_strCTIConferenceId.c_str(), l_strCTICallId.c_str(), l_strTarget.c_str());
		break;
	}
	case REQUEST_BARGEIN_CALL_CMD:		//强插
		if (l_pFSAesReqstResult->GetResult())
		{
			std::string l_strConferenceId =l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
			std::string l_strCTICallId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_callid);
			std::string l_strDevice = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member); 
			std::string l_strTarget = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_moderator);
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strConferenceId);

			if (l_pConferencePtr)
			{
				//add 
				l_pConferencePtr->UpdateMemberCallState(l_strDevice, CallStateString[STATE_TALK], m_pDateTimePtr->CurrentDateTimeStr());
				CDeviceManager::Instance()->UpdateDeviceCallState(l_strDevice, l_strCTICallId, CallStateString[STATE_CONFERENCE]);
				PostInitateDeviceState(l_strDevice, l_strCTICallId, CallStateString[STATE_CONFERENCE]);

				l_pConferencePtr->UpdateMemberCallState(l_strTarget, CallStateString[STATE_TALK], m_pDateTimePtr->CurrentDateTimeStr());
				CDeviceManager::Instance()->UpdateDeviceCallState(l_strTarget, l_strCTICallId, CallStateString[STATE_CONFERENCE]);
				PostInitateDeviceState(l_strTarget, l_strCTICallId, CallStateString[STATE_CONFERENCE]);

				ICC_LOG_DEBUG(m_pLog, "bargein result,ctiCallId:[%s],conferenceId:[%s],taskId:[%d]", l_strCTICallId.c_str(), l_strConferenceId.c_str(), p_lTaskId);
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "bargein result,call not find,ctiCallId:[%s],taskId:[%d]", l_strCTICallId.c_str(), p_lTaskId);
			}
		}
		//m_pSwitchEventCallback->CR_BargeInCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_BargeInCall", l_pSwitchReqstResult);
		break;
	case REQUEST_TAKEOVER_CALL_CMD:		//接管话务
	{
		std::string l_strCTICallId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_callid);

		if (l_pSwitchReqstResult->GetResult())
		{
			std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
			std::string l_strCTIConferenceName = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_conferencename);
			std::string l_strDevice = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member);
			std::string l_strTarget = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_moderator);
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId);
			bool l_bFlag = false;
			if (l_pConferencePtr)
			{
				l_pConferencePtr->SetTakeover();
				l_pConferencePtr->SetCompere(l_strDevice);
				l_pConferencePtr->UpdateMemberCallState(l_strDevice, CallStateString[STATE_TALK], m_pDateTimePtr->CurrentDateTimeStr());
				l_bFlag = true;

				//CDeviceManager::Instance()->UpdateDeviceCallState(l_strDevice, l_strCTICallId, CallStateString[STATE_TALK]);
				//PostInitateDeviceState(l_strDevice, l_strCTICallId, CallStateString[STATE_TALK], l_pConferencePtr->GetSrcCaller(), l_strTarget);

			}

			ICC_LOG_DEBUG(m_pLog, "takeover result ok,takeover,conferenceId:[%s],ctiCallId:[%s],callId_1:[%s],target:[%s],l_bFlag:%d",
				l_strCTIConferenceId.c_str(),
				l_pSwitchReqstResult->GetCTICallRefId().c_str(),
				l_strCTICallId.c_str(),
				l_strTarget.c_str(), l_bFlag);

			_AesAutoHangupCallRequest(l_strCTICallId, l_strTarget);
			//_AesDelConferenceMemberRequest(l_strCTIConferenceId, l_strCTIConferenceName, l_strCTICallId, l_strTarget);
			

		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "takeover result failed,ctiCallId:[%s],taskId:[%d]", l_strCTICallId.c_str(), p_lTaskId);
		}
		//m_pSwitchEventCallback->CR_TakeOverCall(boost::dynamic_pointer_cast<CTakeOverCallResultNotif>(l_pSwitchReqstResult));
		_DispatchRequestResult("CR_TakeOverCall", l_pSwitchReqstResult);
		break;
	}
		
	case REQUEST_CONFMEMBER_UNMUTE_CMD:
		break;
	case REQUEST_FORCEPOP_CALL_CMD:
		//m_pSwitchEventCallback->CR_ForcePopCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ForcePopCall", l_pSwitchReqstResult);
		break;
	case REQUEST_CONFERENCE_RELEASE_CMD:
		break;
	case REQUEST_DEFLECT_CALL_CMD:		//偏转
		//m_pSwitchEventCallback->CR_DeflectCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_DeflectCall", l_pSwitchReqstResult);
		break;
	case REQUEST_CONFERENCE_CALL_CMD:
		//m_pSwitchEventCallback->CR_ConferenceCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ConferenceCall", l_pSwitchReqstResult);
		break;
	case REQUEST_RECONNECT_CALL_CMD:		//Call重连
		//m_pSwitchEventCallback->CR_ReconnectCall(l_pSwitchReqstResult);
		_DispatchRequestResult("CR_ReconnectCall", l_pSwitchReqstResult);
		break;
	case REQUEST_CONFERENCE_CREATE_CMD:
	{
		std::string l_strConferenceName = l_pFSAesReqstResult->GetResultParam(CTI_PARAM_extend_info);//返回结果中获取会议id
		std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
		if (l_pFSAesReqstResult->GetResult())
		{
			if (!l_strConferenceName.empty())
			{
				CConferenceManager::Instance()->InitCallMember(l_strCTIConferenceId, l_strConferenceName);
			}
		}
		ICC_LOG_DEBUG(m_pLog, "create conference result,res:[%u],conferenceName:[%s],CTIConferenceId:[%s]", l_pFSAesReqstResult->GetResult(), l_strConferenceName.c_str(), l_strCTIConferenceId.c_str());
		break;
	}
	case REQUEST_CONFERENCE_ADDPARTY_CMD:
	{
		std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
		std::string l_strMember = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member);
		std::string l_strType = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member_type);
		std::string l_strCallId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_callid);

		ICC_LOG_DEBUG(m_pLog, "add conference result,res:[%u],member:[%s],CTIConferenceId:[%s]", l_pFSAesReqstResult->GetResult(), l_strMember.c_str(), l_strCTIConferenceId.c_str());
		if (l_pFSAesReqstResult->GetResult())
		{
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId);
			if (l_pConferencePtr)
			{
				if (l_pConferencePtr->IsTakeoverCreate() || l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_isListen) == "1")
				{
					//接管后再拉会或者是监听中被拉会
					l_pConferencePtr->SetNormalMode();
					PostInitateDeviceState(l_pConferencePtr->GetCompere(), l_pConferencePtr->GetSrcCTICallId(), CallStateString[STATE_TALK]);
				}
				else
				{
					if (!l_pConferencePtr->IsListenCreate())
					{
						//PostInitateDeviceState(l_strMember, l_pConferencePtr->GetSrcCTICallId(), CallStateString[STATE_DIAL]);
						l_pConferencePtr->UpdateMemberCallState(l_strMember, CallStateString[STATE_DIAL], m_pDateTimePtr->CurrentDateTimeStr());
						CHcpDeviceCallEvent l_HcpAcdCallStateEvent;
						l_HcpAcdCallStateEvent.m_oBody.m_strHcpCallId = l_strCallId;
						PostConferenceState(l_strCTIConferenceId, l_strMember, CallStateString[STATE_DIAL], l_HcpAcdCallStateEvent);
					}
					
				}
			}
		}
		break;
	}
	case REQUEST_CONFERENCE_DELPARTY_CMD:
	{
		std::string l_strCTIConferenceId = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_cti_conferenceid);
		std::string l_strMember = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member);
		std::string l_strType = l_pFSAesReqstResult->GetRequestParam(CTI_PARAM_member_type);
		
		if (l_pFSAesReqstResult->GetResult())
		{
			if (!CDeviceManager::Instance()->IsExtension(l_strMember))
			{
				//上个版本没有外线设备状态通知，所以加了模拟，可以去掉
				//模拟通知设备挂机
				IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId);
				if (l_pConferencePtr)
				{
					l_pConferencePtr->UpdateMemberCallState(l_strMember, CallStateString[STATE_HANGUP], m_pDateTimePtr->CurrentDateTimeStr());
					CHcpDeviceCallEvent l_HcpAcdCallStateEvent;
					l_HcpAcdCallStateEvent.m_oBody.m_strHcpCallId = "";
					if (!l_pConferencePtr->IsTakeoverCreate() && !l_pConferencePtr->IsListenCreate() && !l_pConferencePtr->IsBargeInCreate())
					{
						PostConferenceState(l_strCTIConferenceId, l_strMember, CallStateString[STATE_HANGUP], l_HcpAcdCallStateEvent);
					}
				}
			}
		}
		
		ICC_LOG_DEBUG(m_pLog, "delete conference result,res:[%u],member:[%s],CTIConferenceId:[%s]", l_pFSAesReqstResult->GetResult(), l_strMember.c_str(), l_strCTIConferenceId.c_str());
		break;
	}
	default:
		ICC_LOG_ERROR(m_pLog, "cmdType Error,CmdType:[%d]", l_pRequestCmdType);
		break;
	}
}


void CFSAesSwitchManager::_InitHcpAesEventFun()
{
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_AGENT_STATUS, &CFSAesSwitchManager::_DoHcpAgentStateEvent));

	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_INCOMMING, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_WAITING, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_ASSIGNED, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_RING, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_RINGBACK, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_CONNECTED, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_HOLD, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_UNHOLD, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_HANGUP, &CFSAesSwitchManager::_DoHcpCallStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CALL_MONITOR, &CFSAesSwitchManager::_DoHcpCallStateEvent));

	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_DEVICE_DIAL, &CFSAesSwitchManager::_DoHcpDeviceStateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_DEVICE_FREE, &CFSAesSwitchManager::_DoHcpDeviceStateEvent));

	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CONFERENCE_CREATE, &CFSAesSwitchManager::_DoHcpConferenceCreateEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CONFERENCE_RELEASE, &CFSAesSwitchManager::_DoHcpConferenceReleaseEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CONFERENCE_MEMBER_ADD, &CFSAesSwitchManager::_DoHcpConferenceMemberEvent));
	m_mapHcpAesEvent.insert(std::pair<std::string, PHCPAesEvent>(CTIEVT_CONFERENCE_MEMBER_DEL, &CFSAesSwitchManager::_DoHcpConferenceMemberEvent));
}

void CFSAesSwitchManager::_ExeHcpNotifyEvent(int p_nTaskType, long p_lTaskId, ITaskPtr p_pTask)
{
	ICHCPNotifyEeventPtr l_pHcpNotifyEvent = boost::dynamic_pointer_cast<CHCPNotifyEevent>(p_pTask->GetSwitchNotif());
	if (l_pHcpNotifyEvent == nullptr)
	{
		return;
	}

	std::string l_strEevntName = l_pHcpNotifyEvent->GetEventName();
	auto l_iter = m_mapHcpAesEvent.find(l_strEevntName);
	if (l_iter != m_mapHcpAesEvent.end())
	{
		PHCPAesEvent l_pFunc = l_iter->second;
		if (l_pFunc)
		{
			(this->*l_pFunc)(l_strEevntName, l_pHcpNotifyEvent);
		}
	}
}

void CFSAesSwitchManager::_DoHcpAgentStateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpAgentStateEvent l_pCAgentStateEvt;
	if (!l_pCAgentStateEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse Error,msg:[%s]", p_pHcpNotifyEvent->GetEventData().c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "processing state msg form aes server,body:[%s]", p_pHcpNotifyEvent->GetEventData().c_str());

	E_AGENT_MODE_TYPE l_agentMode = AGENT_MODE_LOGIN;
	E_AGENT_READY_TYPE l_agentReadyType = AGENT_NOTREADY;
	if (l_pCAgentStateEvt.m_strLoginState == CTI_AGENT_STATE_LOGOUT) //
	{
		l_agentMode = AGENT_MODE_LOGOUT;
	}

	if (l_pCAgentStateEvt.m_strStatus == CTI_AGENT_STATUS_IDLE)
	{
		l_agentReadyType = AGENT_READY;
	}
	CAgentManager::Instance()->ReadyStateSync(l_pCAgentStateEvt.m_strExtension, l_pCAgentStateEvt.m_strSkill, l_agentMode, l_agentReadyType);
}

void CFSAesSwitchManager::_DoHcpDeviceStateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpDeviceCallEvent l_HcpDeviceStateEvt;

	l_HcpDeviceStateEvt.SetEventName(p_strEventName);
	if (!l_HcpDeviceStateEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse Error,msg:[%s]", p_pHcpNotifyEvent->GetEventData().c_str());
		return;
	}
	if (l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum.empty())
	{
		l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum = l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;
	}

	std::string l_strCurrState = CDeviceManager::Instance()->GetDeviceCurrState(l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum);

	ICC_LOG_DEBUG(m_pLog, "EventName:[%s],extension:[%s],currState:[%s]", p_strEventName.c_str(), l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum.c_str(), l_strCurrState.c_str());
	if (p_strEventName == CTIEVT_DEVICE_DIAL)
	{
		
		if (l_strCurrState == CallStateString[STATE_FREE] || l_strCurrState == CallStateString[STATE_HANGUP] || l_strCurrState == "")
		{
			l_HcpDeviceStateEvt.m_strCurrState = CallStateString[STATE_DIAL];
		}
		else
		{
			return;
		}
	}
	else
	{
		if (CallStateString[STATE_DIAL] == CDeviceManager::Instance()->GetDeviceCurrState(l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum))
		{
			l_HcpDeviceStateEvt.m_strCurrState = CallStateString[STATE_FREE];
		}
		else
		{
			return;
		}
	}

	l_HcpDeviceStateEvt.m_oBody.m_strCaller = l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;
	PostDeviceState("", 0, l_HcpDeviceStateEvt);
}

void CFSAesSwitchManager::_DoHcpCallStateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpDeviceCallEvent l_HcpDeviceStateEvt;
	l_HcpDeviceStateEvt.SetEventName(p_strEventName);
	if (!l_HcpDeviceStateEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "processing msg from aes server,cmd:[%s],hcpCallid:[%s]!!!!", p_strEventName.c_str(), p_pHcpNotifyEvent->GetEventData().c_str());

	std::string l_strCTICallRefId;
	std::string l_strCurrDeviceState;
	std::string l_strLastDeviceState;
	long l_lCallIndex = 0;
	std::string l_strDirction = "";

	ICTICallPtr l_pCTICallPtr = nullptr;
	if (p_strEventName == CTIEVT_CALL_CONNECTED && !l_HcpDeviceStateEvt.m_oBody.m_strConferenceName.empty())
	{
		//话机上拉会适配
		if (CCallManager::Instance()->GetCallByHcpCallId(l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId) == nullptr)
		{
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceByName(l_HcpDeviceStateEvt.m_oBody.m_strConferenceName);
			if (l_pConferencePtr != nullptr)
			{
				l_pConferencePtr->SetCompere(l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension);
				ICC_LOG_DEBUG(m_pLog, "phone create meeting update,conferenceName:[%s],compere:[%s]",
					l_HcpDeviceStateEvt.m_oBody.m_strConferenceName.c_str(),
					l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension.c_str());

				return;
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "not find conference,conferenceName:[%s]", l_HcpDeviceStateEvt.m_oBody.m_strConferenceName.c_str());
				return;
			}
		}
	}
	
	if (l_pCTICallPtr == nullptr)
	{
		l_pCTICallPtr = CCallManager::Instance()->GetCallByHcpCallId(l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId);
	}

	if(l_pCTICallPtr == nullptr)
	{
		l_strCurrDeviceState = l_HcpDeviceStateEvt.InitCTICallState("");
		if (p_strEventName == CTIEVT_CALL_INCOMMING ||
			p_strEventName == CTIEVT_CALL_RING ||
			p_strEventName == CTIEVT_CALL_RINGBACK ||
			p_strEventName == CTIEVT_CALL_ASSIGNED ||
			p_strEventName == CTIEVT_CALL_WAITING ||
			p_strEventName == CTIEVT_CALL_CONNECTED)
		{
			bool l_bPhoneTransfer = false;
			if (m_bPhoneTransferEnable && (p_strEventName == CTIEVT_CALL_RINGBACK || p_strEventName == CTIEVT_CALL_RING) && CDeviceManager::Instance()->IsExtension(l_HcpDeviceStateEvt.m_oBody.m_strCaller))
			{
				//话机上操作转移适配（话机转移产生一个新的话务，主被叫都是分机号码，挂断只有一个分机的挂断消息
				//分机呼叫
				std::string l_strCurrCallerState = CDeviceManager::Instance()->GetDeviceCurrState(l_HcpDeviceStateEvt.m_oBody.m_strCaller);
				if (l_strCurrCallerState == CallStateString[STATE_HOLD])
				{
					ICTICallPtr l_pHoldCallPtr = CCallManager::Instance()->GetCallByCTICallId(CDeviceManager::Instance()->GetDeviceCurrCallId(l_HcpDeviceStateEvt.m_oBody.m_strCaller));
					if (l_pHoldCallPtr)
					{
						ICC_LOG_WARNING(m_pLog, "phone operate transfer,hcpCallid:[%s],Caller:[%s],orgCaller:[%s]",
							l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId.c_str(),
							l_HcpDeviceStateEvt.m_oBody.m_strCaller.c_str(),
							l_pHoldCallPtr->GetCallerId().c_str());
						l_bPhoneTransfer = true;
					}
				}
			}

			//创建新的呼叫
			l_pCTICallPtr = CCallManager::Instance()->CreateCTICall(l_HcpDeviceStateEvt, l_lCallIndex);
			if (l_pCTICallPtr == nullptr)
			{
				ICC_LOG_FATAL(m_pLog, "create CTI Call failed,cmdName:[%s],hcpCallid:[%s]!!!!", p_strEventName.c_str(), l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId.c_str());
				return;
			}

			//话机上操作转移适配
			if (l_bPhoneTransfer)
			{
				l_pCTICallPtr->SetCallTransferType(TransferType_phoneOperate,l_HcpDeviceStateEvt.m_oBody.m_strCaller, CallStateString[STATE_RING]);
			}
			l_strDirction = l_pCTICallPtr->GetCallDirection();
			l_strCTICallRefId = l_pCTICallPtr->GetCTICallRefId();

			if (p_strEventName == CTIEVT_CALL_INCOMMING|| p_strEventName == CTIEVT_CALL_RING)
			{
				if (CBlackListManager::Instance()->FindBlackNumber(l_HcpDeviceStateEvt.m_oBody.m_strCaller))
				{
					//黑名单拒接
					l_pCTICallPtr->SetBlackCall(true);
					_AesAutoHangupCallRequest(l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId, l_HcpDeviceStateEvt.m_oBody.m_strCaller);
					return;
				}
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "not find cti call,cmdName:[%s],hcpCallid:[%s],msg:[%s]!!!!", p_strEventName.c_str(),
				l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId.c_str(), p_pHcpNotifyEvent->GetEventData().c_str());

			//没有找到呼叫信息
			if (l_HcpDeviceStateEvt.GetCTICallState() == CallStateString[STATE_HANGUP])
			{
				PostDeviceState("", 0, l_HcpDeviceStateEvt);

				l_HcpDeviceStateEvt.m_strCurrState = CallStateString[STATE_FREE];
				PostDeviceState("", 0, l_HcpDeviceStateEvt);

				if (!l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId.empty())
				{
					ICC_LOG_DEBUG(m_pLog, "delete orgcall:orgcall:[%s]", l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId.c_str());
					//删除呼叫
					CCallManager::Instance()->DeleteCTICall(l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId);
				}
			}
			return;
		}
	}
	else  //呼叫存在的处理
	{
		l_strCTICallRefId = l_pCTICallPtr->GetCTICallRefId();
		if (l_strCTICallRefId != l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId)
		{
			//话机拉会适配
			ICC_LOG_DEBUG(m_pLog, "ReplaceCallId,ctiBusinessCallId:[%s],Callid:[%s]!", l_strCTICallRefId.c_str(), l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId.c_str());
			l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId = l_strCTICallRefId;
		}

		if (p_strEventName == CTIEVT_CALL_ASSIGNED)
		{
			if (!l_pCTICallPtr->GetTalkTime().empty())
			{
				ICC_LOG_ERROR(m_pLog, "error state,cmdName:[%s],hcpCallid:[%s]!", p_strEventName.c_str(),l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId.c_str());
				return;
			}
		}

		l_strCurrDeviceState = l_HcpDeviceStateEvt.InitCTICallState(l_pCTICallPtr->GetCallState());
		if (l_strCurrDeviceState.empty())
		{
			ICC_LOG_ERROR(m_pLog, "init cti call state error!!!!");
		}
		l_strLastDeviceState = l_pCTICallPtr->GetCallState();
		l_pCTICallPtr->UpdateCTICall(l_HcpDeviceStateEvt, m_pDateTimePtr->CurrentDateTimeStr());

		l_lCallIndex = l_pCTICallPtr->GetCallIndex();
		l_strDirction = l_pCTICallPtr->GetCallDirection();
		std::string l_strCTIConferenceId = l_pCTICallPtr->GetCTIConferenceID();

		ICC_LOG_DEBUG(m_pLog, "update call,event_name:[%s],ctiCallId:[%s],curr_state:[%s],last_state:[%s],conferenceId:[%s]", p_strEventName.c_str(),
			l_pCTICallPtr->GetCTICallRefId().c_str(), l_strCurrDeviceState.c_str(), l_strLastDeviceState.c_str(), l_strCTIConferenceId.c_str());

		if (l_strCTIConferenceId.empty())
		{
			if (p_strEventName == CTIEVT_CALL_RINGBACK)
			{
				l_pCTICallPtr->SetCallDirection(CallDirectionString[CALL_DIRECTION_OUT]);

				ICC_LOG_DEBUG(m_pLog, "update callDirection,event_name:[%s],ctiCallId:[%s],dir_1:[%s],dir_2:[%s]", p_strEventName.c_str(),
					l_pCTICallPtr->GetCTICallRefId().c_str(), CallDirectionString[CALL_DIRECTION_OUT].c_str(), l_pCTICallPtr->GetCallDirection().c_str());
			}
			else if (p_strEventName == CTIEVT_CALL_CONNECTED)
			{
				//if (l_pCTICallPtr->GetCallTransferType() > TransferType_outline && !l_pCTICallPtr->GetAcdNum().empty())
				if (l_pCTICallPtr->GetCallTransferType() > TransferType_outline)
				{
					l_HcpDeviceStateEvt.m_oBody.m_strOrgCaller = l_HcpDeviceStateEvt.m_oBody.m_strCaller;
					l_HcpDeviceStateEvt.m_oBody.m_strCaller = l_pCTICallPtr->GetCallerId();
					l_HcpDeviceStateEvt.m_oBody.m_strTransferType = "1";
					//l_HcpDeviceStateEvt.m_oBody.m_strCalled = l_pCTICallPtr->GetCalledId();
				}
			}

			//非会议
			if (l_HcpDeviceStateEvt.m_oBody.m_strSkill.empty())
			{
				l_HcpDeviceStateEvt.m_oBody.m_strSkill = l_pCTICallPtr->GetAcdNum();
			}
		}
		else
		{
			//呼叫在会议里，更会议新成员状态
			std::string l_strDeviceNum = l_HcpDeviceStateEvt.m_oBody.m_strDeviceNum;
			if (l_strDeviceNum.empty())
			{
				l_strDeviceNum = l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;
			}
			
			//会议呼新成员，方向为固定为呼出
			l_strDirction = CallDirectionString[CALL_DIRECTION_OUT];

			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(l_strCTIConferenceId);
			if (l_pConferencePtr)
			{
				l_pConferencePtr->UpdateMemberCallState(l_strDeviceNum, l_strCurrDeviceState, m_pDateTimePtr->CurrentDateTimeStr());
				if (l_pConferencePtr->IsListenCreate())
				{
					ICC_LOG_DEBUG(m_pLog, "post_ex listen call state:[%s],ctiCallId:[%s],DeviceNum:[%s],ListenTarget:[%s],compere:[%s],caller:[%s],called:[%s],CallTransferType:[%d]",
						l_strCurrDeviceState.c_str(),l_strCTICallRefId.c_str(),l_strDeviceNum.c_str(),
						l_pConferencePtr->GetListenTarget().c_str(), l_pConferencePtr->GetCompere().c_str(),
						l_pCTICallPtr->GetCallerId().c_str(),
						l_pCTICallPtr->GetCalledId().c_str(), l_pCTICallPtr->GetCallTransferType());

					if (l_pCTICallPtr->GetCallTransferType() > TransferType_outline && l_strDeviceNum == l_pCTICallPtr->GetCallTransferDstDevice())
					{
						l_HcpDeviceStateEvt.m_oBody.m_strOrgCaller = l_HcpDeviceStateEvt.m_oBody.m_strCaller;
						l_HcpDeviceStateEvt.m_oBody.m_strCaller = l_pCTICallPtr->GetCallerId();
						l_HcpDeviceStateEvt.m_oBody.m_strCalled = l_pCTICallPtr->GetCalledId();

						//l_pCTICallPtr->SetCTIConferenceID("");
						PostDeviceState(l_strCTICallRefId, l_lCallIndex, l_HcpDeviceStateEvt, l_strDirction);
						return;
					}

					//if (l_strDeviceNum != l_pCTICallPtr->GetCalledId() && l_strDeviceNum != l_pCTICallPtr->GetCallerId() && !l_strDeviceNum.empty())
					if(!l_strDeviceNum.empty() && l_strDeviceNum != l_pConferencePtr->GetCompere())
					{
						//监听发起者状态处理
						if (l_strCurrDeviceState == CallStateString[STATE_TALK])
						{
							l_pConferencePtr->UpdateMemberCallState(l_strDeviceNum, CallStateString[STATE_SILENTMONITOR], m_pDateTimePtr->CurrentDateTimeStr());
							l_HcpDeviceStateEvt.m_strCurrState = CallStateString[STATE_SILENTMONITOR];
							l_HcpDeviceStateEvt.m_oBody.m_strCaller = l_pCTICallPtr->GetCallerId();
						}
						else if (l_strCurrDeviceState == CallStateString[STATE_RING])
						{
							l_pConferencePtr->UpdateMemberCallState(l_strDeviceNum, CallStateString[STATE_SILENTMONITOR], m_pDateTimePtr->CurrentDateTimeStr());
							//监听发起者振铃后自动接听
							_AesAutoAnswserCallRequest(l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId, l_strDeviceNum);
							return;
						}
						PostDeviceState(l_strCTICallRefId, l_lCallIndex, l_HcpDeviceStateEvt, l_strDirction);
						return;
					}
				}
				else if (l_pConferencePtr->IsTakeoverCreate())
				{
					ICC_LOG_DEBUG(m_pLog, "post_ex takeover call state:[%s],ctiCallId:[%s],DeviceNum:[%s],compere:[%s],state:[%s],calledId:[%s]", l_strCTICallRefId.c_str(),
						l_strCTICallRefId.c_str(), l_strDeviceNum.c_str(), l_pConferencePtr->GetCompere().c_str(), l_strCurrDeviceState.c_str(), l_pCTICallPtr->GetCallerId().c_str());
					
					//会议接管模式,接管的发起者是主持人
					if (l_strCurrDeviceState == CallStateString[STATE_HANGUP])
					{
						if (l_pConferencePtr->GetCompere() != l_strDeviceNum && l_strDeviceNum != l_pCTICallPtr->GetCallerId())
						{
							//被接管的坐席挂机, 重新发送接管发起者坐席状态为通话态
							CDeviceManager::Instance()->UpdateDeviceCallState(l_pConferencePtr->GetCompere(), l_strCTICallRefId, CallStateString[STATE_TALK]);
							PostInitateDeviceState(l_pConferencePtr->GetCompere(), l_strCTICallRefId, CallStateString[STATE_TALK], l_pCTICallPtr->GetCallerId(), l_pCTICallPtr->GetCalledId());
							//l_pCTICallPtr->SetCalled(l_pConferencePtr->GetCompere());
							
							//接管成功后挂断其他监听的呼叫
							l_pConferencePtr->ClearMonitorMember();
						}
					}

				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "post_ex conference call state,ctiCallId:[%s],DeviceNum:[%s],compere:[%s],state:[%s],isBargeIn:[%d]",
						l_strCTICallRefId.c_str(),
						l_strDeviceNum.c_str(),
						l_pConferencePtr->GetCompere().c_str(),
						l_strCurrDeviceState.c_str(), l_pConferencePtr->IsBargeInCreate());

					if (l_pConferencePtr->GetCompere() != l_strDeviceNum)
					{
						//会议成员设备状态通知主叫改成主持人
						l_HcpDeviceStateEvt.m_oBody.m_strCaller = l_pConferencePtr->GetCompere();
					}
					PostConferenceState(l_strCTIConferenceId, l_strDeviceNum, l_strCurrDeviceState, l_HcpDeviceStateEvt);

					if (l_strCurrDeviceState == CallStateString[STATE_HANGUP])
					{
						if (!l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId.empty() && l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId != l_HcpDeviceStateEvt.m_oBody.m_strHcpCallId)
						{
							ICC_LOG_DEBUG(m_pLog, "conference,delete orgcall:orgcall:[%s]", l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId.c_str());
							//删除呼叫
							CCallManager::Instance()->DeleteCTICall(l_HcpDeviceStateEvt.m_oBody.m_strOrgHcpCallId);
						}
					}
				}
			}			
		}
	}

	//同步设备状态or ACD call状态
	if (l_HcpDeviceStateEvt.IsSkillCallState(p_strEventName) || l_strCurrDeviceState == CallStateString[STATE_RELEASE])
	{
		PostCallState(l_strCTICallRefId, l_lCallIndex, l_HcpDeviceStateEvt);
	}
	else
	{
		if (l_strCurrDeviceState == CallStateString[STATE_HANGUP])
		{
			//已经分配给坐席，坐席没有接听后的挂断,判断为早释呼叫
			if (l_pCTICallPtr != nullptr && l_strLastDeviceState == CallStateString[STATE_RING] && !l_pCTICallPtr->GetAcdNum().empty())
			{
				if (l_pCTICallPtr->GetCallTransferType() > TransferType_outline && l_pCTICallPtr->GetCallTransferDstDevice() != l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension)
				{
					//转移的呼叫，源设备挂断不算早释
				}
				else
				{
					if (l_pCTICallPtr->GetTalkTime().empty())
					{
						l_strCurrDeviceState = CallStateString[STATE_RELEASE]; //早释
					}
				}
				l_HcpDeviceStateEvt.m_strCurrState = l_strCurrDeviceState;
			}
		}

		std::string l_strOrgCaller;
		if (l_pCTICallPtr != nullptr)
		{
			l_strOrgCaller = l_pCTICallPtr->GetCallerId();
		}
		PostDeviceState(l_strCTICallRefId, l_lCallIndex, l_HcpDeviceStateEvt, l_strDirction, l_strOrgCaller);

		//分机与分机之间呼叫hold状态，AES只会给一个设备发hold， CTI做适配
		if (l_strCurrDeviceState == CallStateString[STATE_HOLD] || l_strCurrDeviceState == CallStateString[STATE_CANCELHOLD])
		{
			if (l_pCTICallPtr != nullptr && l_pCTICallPtr->GetAcdNum().empty())
			{
				std::string l_strHoldDeviceNum = l_HcpDeviceStateEvt.m_oBody.m_strAgentExtension;
				if (l_strCurrDeviceState == CallStateString[STATE_HOLD])
				{
					l_pCTICallPtr->SetHoldDeviceNum(l_strHoldDeviceNum);
				}
				else
				{
					l_pCTICallPtr->SetHoldDeviceNum("");
				}
				
				if (!l_strHoldDeviceNum.empty())
				{
					if (l_pCTICallPtr->GetCalledId() == l_strHoldDeviceNum)
					{
						l_strHoldDeviceNum = l_pCTICallPtr->GetCallerId();
					}
					else
					{
						l_strHoldDeviceNum = l_pCTICallPtr->GetCalledId();
					}

					ICC_LOG_DEBUG(m_pLog, "post_ex hold call state,ctiCallId:[%s],DeviceNum:[%s],caller:[%s],called:[%s]",
						l_strCTICallRefId.c_str(), l_strHoldDeviceNum.c_str(), l_pCTICallPtr->GetCallerId().c_str(), l_pCTICallPtr->GetCalledId().c_str());

					bool bRes = CDeviceManager::Instance()->UpdateDeviceCallState(l_strHoldDeviceNum, l_strCTICallRefId, l_strCurrDeviceState);
					if(bRes)
					{
						PostInitateDeviceState(l_strHoldDeviceNum, l_strCTICallRefId, l_strCurrDeviceState, l_pCTICallPtr->GetCallerId(), l_pCTICallPtr->GetCalledId());
					}
				}
			}
		}
	}

	//
	if (l_strCurrDeviceState == CallStateString[STATE_RELEASE] || l_strCurrDeviceState == CallStateString[STATE_HANGUP])
	{
		_ProcessHangupCheckCallOver(l_pCTICallPtr, l_HcpDeviceStateEvt);
	}
}

void CFSAesSwitchManager::_ProcessHangupCheckCallOver(ICTICallPtr p_pCTICallPtr, const CHcpDeviceCallEvent& p_HcpDeviceStateEvent)
{
	if (p_pCTICallPtr == nullptr)
	{
		return;
	}

	if (!p_pCTICallPtr->GetCTIConferenceID().empty())
	{
		return;
	}

	std::string l_strCTICallId = p_pCTICallPtr->GetCTICallRefId();
	std::string l_strHangupType = p_HcpDeviceStateEvent.GetHangupType();
	std::string l_strExtension = p_HcpDeviceStateEvent.m_oBody.m_strAgentExtension;
	if (l_strExtension.empty())
	{
		l_strExtension = p_HcpDeviceStateEvent.m_oBody.m_strDeviceNum;
	}

	int l_nTransferType = p_pCTICallPtr->GetCallTransferType();
	if (l_nTransferType >= TransferType_outline)
	{
		std::string l_strTransferDstNum = p_pCTICallPtr->GetCallTransferDstDevice();
		if (l_strTransferDstNum != l_strExtension)
		{
			//发起转移的坐席分机挂机，转移完成
			ICC_LOG_DEBUG(m_pLog, "call transfer complete,type:[%d],ctiCallId:[%s],TransferTarget:[%s],Agent:[%s]",
				l_nTransferType,
				l_strCTICallId.c_str(), l_strTransferDstNum.c_str(), l_strExtension.c_str());

			if (l_nTransferType != TransferType_outline)
			{
				//转移到内线
				p_pCTICallPtr->SetCallTransferType(TransferType_null,
					l_strTransferDstNum,
					CDeviceManager::Instance()->GetDeviceCurrState(l_strTransferDstNum));
			}
			else
			{
				if (CDeviceManager::Instance()->IsExtension(p_pCTICallPtr->GetCallerId()))
				{
					//内部分机发起的呼叫,转移到外线
					p_pCTICallPtr->SetCallTransferType(TransferType_null, "");
				}
				else
				{
					//外线呼入，转移到了外线,发呼叫结束
					PostCallOver(p_HcpDeviceStateEvent.m_oBody.m_strHcpCallId, p_pCTICallPtr, HangupTypeString[HANGUP_TYPE_TRANSFER]);
					//删除呼叫
					CCallManager::Instance()->DeleteCTICall(l_strCTICallId);
				}
			}
		}
		else
		{
			//被转移方拒绝
			ICC_LOG_DEBUG(m_pLog, "call transfer refuse,type:[%d],ctiCallId:[%s],TransferTarget:[%s],Agent:[%s]",
				l_nTransferType,
				l_strCTICallId.c_str(), 
				l_strTransferDstNum.c_str(),
				l_strExtension.c_str());

			//被转移方拒绝接听
			p_pCTICallPtr->SetCallTransferType(TransferType_null, "");
		}

	}
	else
	{
		//非会议+不是转移到内线的呼叫挂断
		//呼叫挂机,通知callover
		PostCallOver(p_HcpDeviceStateEvent.m_oBody.m_strHcpCallId, p_pCTICallPtr, l_strHangupType);
		//删除呼叫
		CCallManager::Instance()->DeleteCTICall(p_HcpDeviceStateEvent.m_oBody.m_strHcpCallId);
	}
}

void CFSAesSwitchManager::PostDeviceState(const std::string& p_strCTICallRefId, long p_lCallIndex, const CHcpDeviceCallEvent& p_HcpDeviceStateEvent, const std::string& p_strDirection,
	const std::string &p_strOrgCaller)
{
	std::string l_strDeviceNum = p_HcpDeviceStateEvent.m_oBody.m_strDeviceNum;
	if (l_strDeviceNum.empty())
	{
		l_strDeviceNum = p_HcpDeviceStateEvent.m_oBody.m_strAgentExtension;
	}
	if (!CDeviceManager::Instance()->IsExtension(l_strDeviceNum))
	{
		bool l_bCallerIsExtension = false;
		std::string l_strOrgCaller;
		if (p_HcpDeviceStateEvent.GetEventName() == CTIEVT_CALL_CONNECTED || p_HcpDeviceStateEvent.GetEventName() == CTIEVT_CALL_HANGUP)
		{
			l_strOrgCaller = p_HcpDeviceStateEvent.m_oBody.m_strOrgCaller;
			if (l_strOrgCaller.empty())
			{
				l_strOrgCaller = p_HcpDeviceStateEvent.m_oBody.m_strCaller;
			}
			if (CDeviceManager::Instance()->IsExtension(l_strOrgCaller))
			{
				l_bCallerIsExtension = true;
			}
		}
		if (!l_bCallerIsExtension)
		{
			ICC_LOG_DEBUG(m_pLog, "not find extension,ctiCallId:[%s],device:[%s],caller:[%s]",
				p_strCTICallRefId.c_str(),
				l_strDeviceNum.c_str(),
				l_strOrgCaller.c_str());

			return;
		}
	}

	std::string l_strCallState = p_HcpDeviceStateEvent.GetCTICallState();

	std::string l_strCurrCTICallId = p_strCTICallRefId;
	if (l_strCurrCTICallId.empty())
	{
		l_strCurrCTICallId = p_HcpDeviceStateEvent.m_oBody.m_strHcpCallId;
	}
	CDeviceManager::Instance()->UpdateDeviceCallState(l_strDeviceNum, l_strCurrCTICallId, l_strCallState);

	if (p_HcpDeviceStateEvent.GetEventName() == CTIEVT_CALL_CONNECTED && !p_HcpDeviceStateEvent.m_oBody.m_strRecordFile.empty())
	{
		CDeviceManager::Instance()->SetDeviceRecordInfo(l_strDeviceNum, p_strCTICallRefId, p_HcpDeviceStateEvent.m_oBody.m_strSwitchAddr, p_HcpDeviceStateEvent.m_oBody.m_strRecordFile);
	}

	IDeviceStateNotifPtr l_pDeviceNotify = boost::make_shared<CDeviceStateNotif>();
	if (l_pDeviceNotify)
	{
		//std::string l_strDeviceNum = p_HcpDeviceStateEvent.m_oBody.m_strAgentExtension;
		std::string l_strTalkTime = "";
		
		l_pDeviceNotify->SetDeviceNum(l_strDeviceNum);
		l_pDeviceNotify->SetDeviceType(DEVICE_TYPE_IPPHONE);
		l_pDeviceNotify->SetCSTACallRefId(p_lCallIndex);
		l_pDeviceNotify->SetDeviceState(l_strCallState);
		l_pDeviceNotify->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

		if (p_strDirection.empty())
		{
			l_pDeviceNotify->SetCallDirection(p_strDirection);
		}
		else
		{
			l_pDeviceNotify->SetCallDirection(p_HcpDeviceStateEvent.GetCTICallDirection());
		}

		l_pDeviceNotify->SetCallDirection(p_HcpDeviceStateEvent.GetCTICallDirection());

		if (p_strOrgCaller.empty())
		{
			l_pDeviceNotify->SetCallerId(p_HcpDeviceStateEvent.m_oBody.m_strCaller);
		}
		else
		{
			l_pDeviceNotify->SetCallerId(p_strOrgCaller);
		}
		
		if (!p_HcpDeviceStateEvent.m_oBody.m_strSkill.empty())
		{
			l_pDeviceNotify->SetACDGrp(p_HcpDeviceStateEvent.m_oBody.m_strSkill);
		}
		l_pDeviceNotify->SetCalledId(p_HcpDeviceStateEvent.m_oBody.m_strCalled);

		//l_pDeviceNotify->SetOriginalCallerId(l_strOriginalCallerId);
		l_pDeviceNotify->SetOriginalCalledId(p_HcpDeviceStateEvent.m_oBody.m_strOrgCalled);
		l_pDeviceNotify->SetCTICallRefId(l_strCurrCTICallId);
		
		if (!p_HcpDeviceStateEvent.m_oBody.m_strTransferType.empty())
		{
			l_pDeviceNotify->SetTransferType(p_HcpDeviceStateEvent.m_oBody.m_strTransferType);
		}
		//
		if (l_strCallState == CallStateString[STATE_TALK])
		{
			l_pDeviceNotify->SetTalkTime(m_pDateTimePtr->CurrentDateTimeStr());
		}
		else if (l_strCallState == CallStateString[STATE_HANGUP])
		{
			if (p_HcpDeviceStateEvent.m_oBody.m_strRecordFile.empty())
			{
				std::string l_strRecordFile;
				std::string l_strSwitchAddr;
				CDeviceManager::Instance()->GetDeviceRecordInfo(l_strDeviceNum, p_strCTICallRefId, l_strSwitchAddr, l_strRecordFile);

				l_pDeviceNotify->SetSwitchAddress(l_strSwitchAddr);
				l_pDeviceNotify->SetRecordFilePath(l_strRecordFile);
			}
			else
			{
				l_pDeviceNotify->SetSwitchAddress(p_HcpDeviceStateEvent.m_oBody.m_strSwitchAddr);
				l_pDeviceNotify->SetRecordFilePath(p_HcpDeviceStateEvent.m_oBody.m_strRecordFile);
			}
		}

		if (l_pDeviceNotify->GetTalkTime().empty())
		{
			l_pDeviceNotify->SetTalkTime(CDeviceManager::Instance()->GetCurrTalkTime(l_strDeviceNum));
		}
		
		//if (m_pSwitchEventCallback)
		{
			ICC_LOG_DEBUG(m_pLog, "report event DeviceState,CTICallRefId:[%s],AcdGrp:[%s],device:[%s],state:[%s]",
				p_strCTICallRefId.c_str(), l_pDeviceNotify->GetACDGrp().c_str(), l_strDeviceNum.c_str(), l_pDeviceNotify->GetDeviceState().c_str());
			//m_pSwitchEventCallback->Event_DeviceState(l_pDeviceNotify);
			DispatchCTIEvent("Event_DeviceState", l_pDeviceNotify);
		}

		//与TSAPIClient逻辑保持一致,设备挂机后再发一条空闲事件
		if (p_HcpDeviceStateEvent.GetEventName() == CTIEVT_CALL_HANGUP && !p_strCTICallRefId.empty())
		{
			CDeviceManager::Instance()->UpdateDeviceCallState(l_strDeviceNum, l_strCurrCTICallId, CallStateString[STATE_FREE]);
			IDeviceStateNotifPtr l_pDeviceFreeNotify = boost::make_shared<CDeviceStateNotif>();
			if (l_pDeviceFreeNotify)
			{
				l_pDeviceFreeNotify->SetDeviceNum(l_strDeviceNum);
				l_pDeviceFreeNotify->SetDeviceType(DEVICE_TYPE_IPPHONE);
				l_pDeviceFreeNotify->SetCSTACallRefId(p_lCallIndex);
				l_pDeviceFreeNotify->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

				l_pDeviceFreeNotify->SetDeviceState(CallStateString[STATE_FREE]);
				l_pDeviceFreeNotify->SetSwitchAddress("");
				l_pDeviceFreeNotify->SetRecordFilePath("");

				ICC_LOG_DEBUG(m_pLog, "report event DeviceState,CTICallRefId:[%s],AcdGrp:[%s],device:[%s],state:[%s]",
					p_strCTICallRefId.c_str(), 
					l_pDeviceFreeNotify->GetACDGrp().c_str(),
					l_strDeviceNum.c_str(),
					l_pDeviceFreeNotify->GetDeviceState().c_str());

				//m_pSwitchEventCallback->Event_DeviceState(l_pDeviceNotify);
				DispatchCTIEvent("Event_DeviceState", l_pDeviceFreeNotify);
			}

			std::string l_strNextCallState;
			l_strCurrCTICallId = CDeviceManager::Instance()->GetDeviceNextCallState(l_strDeviceNum, l_strNextCallState);
			if (!l_strCurrCTICallId.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "report event DeviceState(next),CTICallRefId:[%s],device:[%s],state:[%s]",
					l_strCurrCTICallId.c_str(),
					l_strDeviceNum.c_str(),
					l_strNextCallState.c_str());

				//话机转移适配
				ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByCTICallId(l_strCurrCTICallId);
				if (l_pCTICallPtr)
				{
					if (l_pCTICallPtr->GetCallTransferType() == TransferType_phoneOperate)
					{
						CDeviceManager::Instance()->UpdateDeviceCallState(l_strDeviceNum, l_strCurrCTICallId, CallStateString[STATE_FREE]);
						if (CDeviceManager::Instance()->IsExtension(l_pCTICallPtr->GetCalledId()) && p_HcpDeviceStateEvent.m_oBody.m_strConferenceName.empty())
						{
							ICC_LOG_DEBUG(m_pLog, "phone operate transfer success,modified the caller,CTICallRefId:[%s],called:[%s],srcCaller:[%s],newCaller:[%s],state:[%s]",
								l_strCurrCTICallId.c_str(),
								l_pCTICallPtr->GetCalledId().c_str(),
								l_pCTICallPtr->GetCallerId().c_str(),
								p_HcpDeviceStateEvent.m_oBody.m_strCaller.c_str(), l_pCTICallPtr->GetCallState().c_str());

							l_pCTICallPtr->SetCaller(p_HcpDeviceStateEvent.m_oBody.m_strCaller);
							PostInitateDeviceState(l_pCTICallPtr->GetCalledId(), l_strCurrCTICallId, l_pCTICallPtr->GetCallState(), p_HcpDeviceStateEvent.m_oBody.m_strCaller, l_pCTICallPtr->GetCallerId());
						}
						return;
					}
				}

				if (PostInitateDeviceState(l_strDeviceNum, l_strCurrCTICallId, l_strNextCallState) == 0)
				{
					CDeviceManager::Instance()->UpdateDeviceCallState(l_strDeviceNum, l_strCurrCTICallId, CallStateString[STATE_FREE]);
				}
			}
		}
	}
}

int CFSAesSwitchManager::PostInitateDeviceState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId,const std::string &p_strState, const std::string& p_strCaller, const std::string& p_strCalled)
{
	ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByHcpCallId(p_strCTICallRefId);
	if (l_pCTICallPtr == nullptr)
	{
		ICC_LOG_DEBUG(m_pLog, "not find call,hcpCallId:[%s]", p_strCTICallRefId.c_str());
		return 0;
	}

	IDeviceStateNotifPtr l_pDeviceNotify = boost::make_shared<CDeviceStateNotif>();
	if (l_pDeviceNotify == nullptr)
	{
		return -1;
	}

	std::string l_strCTICallRefId = l_pCTICallPtr->GetCTICallRefId();
	std::string l_strCallState = p_strState;
	if (l_strCallState.empty())
	{
		l_strCallState = l_pCTICallPtr->GetCallState();
	}

	l_pDeviceNotify->SetDeviceNum(p_strDeviceNum);
	l_pDeviceNotify->SetDeviceType(DEVICE_TYPE_IPPHONE);
	l_pDeviceNotify->SetCSTACallRefId(l_pCTICallPtr->GetCallIndex());
	l_pDeviceNotify->SetDeviceState(l_strCallState);
	l_pDeviceNotify->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

	if (l_strCallState == CallStateString[STATE_TALK] || l_strCallState == CallStateString[STATE_CONFERENCE])
	{
		l_pDeviceNotify->SetTalkTime(m_pDateTimePtr->CurrentDateTimeStr());
	}

	if (l_strCallState == CallStateString[STATE_CANCELHOLD])
	{
		l_pDeviceNotify->SetTalkTime(l_pCTICallPtr->GetTalkTime());
	}

	l_pDeviceNotify->SetCallDirection(CallDirectionString[CALL_DIRECTION_OUT]);

	if (p_strCaller.empty())
	{
		l_pDeviceNotify->SetCallerId(l_pCTICallPtr->GetCallerId());
	}
	else
	{
		l_pDeviceNotify->SetCallerId(p_strCaller);
	}
	
	l_pDeviceNotify->SetACDGrp(l_pCTICallPtr->GetAcdNum());
	if (p_strCalled.empty())
	{
		l_pDeviceNotify->SetCalledId(p_strDeviceNum);
	}
	else
	{
		if (p_strState == CallStateString[STATE_HOLD] || p_strState == CallStateString[STATE_CANCELHOLD])
		{
			l_pDeviceNotify->SetCalledId(p_strCalled);
		}
		else
		{
			//l_pDeviceNotify->SetCalledId(p_strCalled);
			l_pDeviceNotify->SetCalledId(p_strDeviceNum);
			l_pDeviceNotify->SetTransferType("2");
		}
	}
	

	//l_pDeviceNotify->SetOriginalCallerId(l_strOriginalCallerId);
	l_pDeviceNotify->SetOriginalCalledId(l_pCTICallPtr->GetOriginalCalled());
	l_pDeviceNotify->SetCTICallRefId(l_strCTICallRefId);
	//if (m_pSwitchEventCallback)
	{
		ICC_LOG_DEBUG(m_pLog, "report event DeviceState,CTICallRefId:[%s],device:[%s],state:[%s]",l_strCTICallRefId.c_str(), p_strDeviceNum.c_str(), l_strCallState.c_str());

		//m_pSwitchEventCallback->Event_DeviceState(l_pDeviceNotify);
		DispatchCTIEvent("Event_DeviceState", l_pDeviceNotify);
	}

	return 1;
}

void CFSAesSwitchManager::PostCallState(const std::string& p_strCTICallRefId, long p_lCallIndex, CHcpDeviceCallEvent& p_HcpAcdCallStateEvent)
{
	ICallStateNotifPtr l_pCallStateNotif = boost::make_shared<CCallStateNotif>();
	if (l_pCallStateNotif)
	{
		if (p_HcpAcdCallStateEvent.GetEventName() == CTIEVT_CALL_ASSIGNED)
		{
			l_pCallStateNotif->SetAgentId(p_HcpAcdCallStateEvent.m_oBody.m_strCalled); //iter->second->GetCalledParty()
		}
		
		std::string l_strACDGrp = p_HcpAcdCallStateEvent.m_oBody.m_strSkill;
		//std::string l_strACDGrp = iter->second->GetCalledId();
		l_pCallStateNotif->SetACDGrp(l_strACDGrp);
		l_pCallStateNotif->SetCSTACallRefId(p_lCallIndex);
		l_pCallStateNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pCallStateNotif->SetCallerId(p_HcpAcdCallStateEvent.m_oBody.m_strCaller);
		l_pCallStateNotif->SetCalledId(p_HcpAcdCallStateEvent.m_oBody.m_strCalled);

		l_pCallStateNotif->SetCallState(p_HcpAcdCallStateEvent.GetCTICallState());
		l_pCallStateNotif->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

		l_pCallStateNotif->SetOriginalCalledId(p_HcpAcdCallStateEvent.m_oBody.m_strOrgCalled);
		//if (m_pSwitchEventCallback)
		{
			ICC_LOG_DEBUG(m_pLog, "report event CallState,CTICallRefId:[%s],ACDGrp:[%s],state:[%s]",
				p_strCTICallRefId.c_str(),l_strACDGrp.c_str(), l_pCallStateNotif->GetCallState().c_str());

			//m_pSwitchEventCallback->Event_CallState(l_pCallStateNotif);
			DispatchCTIEvent("Event_CallState", l_pCallStateNotif);
		}
	}
}


void CFSAesSwitchManager::PostCallOver(const std::string& p_strHcpCallRefId, ICTICallPtr p_pCTICallPtr, const std::string& p_strHangupType)
{
	ICallOverNotifPtr l_pCallOverNotif = boost::make_shared<CCallOverNotif>();
	if (l_pCallOverNotif && p_pCTICallPtr != nullptr)
	{
		std::string l_strCallerId = p_pCTICallPtr->GetCallerId();
		std::string l_strCalledId = p_pCTICallPtr->GetCalledId();

		std::string l_strOriginalCallerId;
		std::string l_strOriginalCalledId = p_pCTICallPtr->GetOriginalCalled();

		/*if (!l_strOriginalCallerId.empty() && !l_strOriginalCalledId.empty())
		{
			if (iter->second->GetCallMode().compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
			{
				//	会议，修改原始主被叫
				l_strCallerId = l_strOriginalCallerId;
				l_strCalledId = l_strOriginalCalledId;
			}
			else if (iter->second->GetIsTransferCall())
			{
				//	转移，修改原始主叫
				l_strCallerId = l_strOriginalCallerId;
			}
		}*/
		std::string l_strCTICallRefId = p_pCTICallPtr->GetCTICallRefId();

		l_pCallOverNotif->SetAgentId(p_pCTICallPtr->GetAgentExtension());
		l_pCallOverNotif->SetACDGrp(p_pCTICallPtr->GetAcdNum());
		l_pCallOverNotif->SetCSTACallRefId(p_pCTICallPtr->GetCallIndex());
		l_pCallOverNotif->SetCTICallRefId(l_strCTICallRefId);

		std::string l_strHangupType = p_strHangupType;
		if (p_pCTICallPtr->GetBlackCall())
		{
			l_pCallOverNotif->SetIsBlackCall(true);
			l_strHangupType = HangupTypeString[HANGUP_TYPE_BLACKCALL];
		}
		else
		{
			l_pCallOverNotif->SetIsBlackCall(false);
		}
		
		l_pCallOverNotif->SetCallerId(l_strCallerId);
		l_pCallOverNotif->SetCalledId(l_strCalledId);
		l_pCallOverNotif->SetOriginalCalledId(l_strOriginalCalledId);
		//l_pCallOverNotif->SetCallState(iter->second->GetCallState());
		l_pCallOverNotif->SetCallDirection(p_pCTICallPtr->GetCallDirection());

		l_pCallOverNotif->SetDialTime(p_pCTICallPtr->GetDialTime());
		l_pCallOverNotif->SetIncomingTime(p_pCTICallPtr->GetIncomingTime());
		l_pCallOverNotif->SetAssignTime(p_pCTICallPtr->GetAssignTime());
		l_pCallOverNotif->SetRingTime(p_pCTICallPtr->GetRingTime());
		l_pCallOverNotif->SetSignalBackTime(p_pCTICallPtr->GetRingbackTime());
		l_pCallOverNotif->SetReleaseTime(p_pCTICallPtr->GetReleaseTime());
		l_pCallOverNotif->SetTalkTime(p_pCTICallPtr->GetTalkTime());
		if (p_pCTICallPtr->GetHangupTime().empty())
		{
			l_pCallOverNotif->SetHangupTime(m_pDateTimePtr->CurrentDateTimeStr());
		}
		else
		{
			l_pCallOverNotif->SetHangupTime(p_pCTICallPtr->GetHangupTime());
		}

		if (l_strHangupType.empty())
		{
			l_pCallOverNotif->SetHangupType(HangupTypeString[HANGUP_TYPE_UNKNOWN]);
		}
		else
		{
			l_pCallOverNotif->SetHangupType(l_strHangupType);
		}
		
		l_pCallOverNotif->SetSwitchType(SWITCH_TYPE_FREESWITCH);

		//if (m_pSwitchEventCallback)
		{
			ICC_LOG_DEBUG(m_pLog, "report event CallOver,CTICallRefId:[%s]", l_strCTICallRefId.c_str());
			//m_pSwitchEventCallback->Event_CallOver(l_pCallOverNotif);
			DispatchCTIEvent("Event_CallOver", l_pCallOverNotif);
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CallOverNotif Object Failed !!!");
	}
}

void CFSAesSwitchManager::PostConferenceState(const std::string& p_strCTIConferenceId, const std::string& p_strTarget, const std::string& p_strCallState, const CHcpDeviceCallEvent& p_HcpAcdCallStateEvent)
{
	std::string l_strSrcCallId;
	_tagMember l_member;
	if (CConferenceManager::Instance()->GetMeetingMember(p_strCTIConferenceId, p_strTarget, l_member, l_strSrcCallId))
	{
		if (p_strCallState == CallStateString[STATE_HANGUP])
		{
			if (!p_HcpAcdCallStateEvent.m_oBody.m_strHcpCallId.empty() && !CDeviceManager::Instance()->IsExtension(p_strTarget))
			{
				//会议里的外线号码
				IDeviceStateNotifPtr l_pDeviceNotify = boost::make_shared<CDeviceStateNotif>();
				if (l_pDeviceNotify)
				{
					l_pDeviceNotify->SetDeviceNum(p_strTarget);
					l_pDeviceNotify->SetDeviceType(DEVICE_TYPE_OUTPHONE);
					l_pDeviceNotify->SetDeviceState(p_strCallState);
					l_pDeviceNotify->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

					l_pDeviceNotify->SetCallerId(l_member.m_strCompere);
					l_pDeviceNotify->SetCalledId(p_strTarget);
					l_pDeviceNotify->SetTalkTime(l_member.m_strTalkTime);
					l_pDeviceNotify->SetCTICallRefId(l_strSrcCallId);
					l_pDeviceNotify->SetRecordFilePath(p_HcpAcdCallStateEvent.m_oBody.m_strRecordFile);
					l_pDeviceNotify->SetSwitchAddress(p_HcpAcdCallStateEvent.m_oBody.m_strSwitchAddr);
					if (!p_HcpAcdCallStateEvent.m_oBody.m_strRecordFile.empty())
					{
						ICC_LOG_DEBUG(m_pLog, "report event outphone state,number:[%s],state:[%s]", p_strTarget.c_str(), p_strCallState.c_str());
						//m_pSwitchEventCallback->Event_DeviceState(l_pDeviceNotify);
						DispatchCTIEvent("Event_DeviceState", l_pDeviceNotify);
					}
				}
			}
		}
		
		JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
		if (!l_JsonPtr)
		{
			ICC_LOG_ERROR(m_pLog, "create json object failed.");
			return;
		}

		l_JsonPtr->SetNodeValue("/body/conference_id", p_strCTIConferenceId);
		//会议的呼叫状态call_id用源呼叫id(会议话务合并)
		l_JsonPtr->SetNodeValue("/body/callref_id", l_strSrcCallId);
		
		l_JsonPtr->SetNodeValue("/body/target", l_member.m_strTarget);
		l_JsonPtr->SetNodeValue("/body/state", l_member.m_strCallState);
		l_JsonPtr->SetNodeValue("/body/compere", l_member.m_strCompere);
		l_JsonPtr->SetNodeValue("/body/result", "0");
		std::string l_strEventInfo;
		l_strEventInfo = l_JsonPtr->ToString();
		IFSAesExEventNotifPtr l_pFSAesExEventNotif = boost::make_shared<CFSAesExEventNotif>();
		if (l_pFSAesExEventNotif)
		{
			ICC_LOG_DEBUG(m_pLog, "report event conference sync.conferenceId:[%s],ctiCallId:[%s],data:[%s]", p_strCTIConferenceId.c_str(), l_strSrcCallId.c_str(), l_strEventInfo.c_str());
			l_pFSAesExEventNotif->SetEventData(l_strEventInfo);
			//m_pSwitchEventCallback->CR_FSAesEventEx("conference_sync", l_pFSAesExEventNotif);
			DispatchCTIEvent("CR_FSAesEventEx", l_pFSAesExEventNotif, "conference_sync");
		}
	}
	else
	{
		if (p_strCallState == CallStateString[STATE_HANGUP])
		{
			IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceById(p_strCTIConferenceId);
			if (l_pConferencePtr == nullptr)
			{
				return;
			}

			l_strSrcCallId = l_pConferencePtr->GetSrcCTICallId();
			std::string l_strCompere = l_pConferencePtr->GetCompere();

			JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
			if (!l_JsonPtr)
			{
				ICC_LOG_ERROR(m_pLog, "create json object failed.");
				return;
			}

			l_JsonPtr->SetNodeValue("/body/conference_id", p_strCTIConferenceId);
			//会议的呼叫状态call_id用源呼叫id(会议话务合并)
			l_JsonPtr->SetNodeValue("/body/callref_id", l_strSrcCallId);

			l_JsonPtr->SetNodeValue("/body/target", p_strTarget);
			l_JsonPtr->SetNodeValue("/body/state", CallStateString[STATE_HANGUP]);
			l_JsonPtr->SetNodeValue("/body/compere", l_strCompere);
			l_JsonPtr->SetNodeValue("/body/result", "0");
			std::string l_strEventInfo;
			l_strEventInfo = l_JsonPtr->ToString();
			
			IFSAesExEventNotifPtr l_pFSAesExEventNotif = boost::make_shared<CFSAesExEventNotif>();
			if (l_pFSAesExEventNotif)
			{
				ICC_LOG_DEBUG(m_pLog, "report event conference sync(ex).conferenceId:[%s],ctiCallId:[%s],data:[%s]", p_strCTIConferenceId.c_str(), l_strSrcCallId.c_str(), l_strEventInfo.c_str());
				l_pFSAesExEventNotif->SetEventData(l_strEventInfo);
				//m_pSwitchEventCallback->CR_FSAesEventEx("conference_sync", l_pFSAesExEventNotif);
				DispatchCTIEvent("CR_FSAesEventEx", l_pFSAesExEventNotif, "conference_sync");
			}
		}
	}
}

//会议创建处理
void CFSAesSwitchManager::_DoHcpConferenceCreateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpConferenceEvent l_hcpConferenceEvt;
	if (!l_hcpConferenceEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "processing msg from aes server,cmd:[%s],hcpCallid:[%s]!!!!", p_strEventName.c_str(), p_pHcpNotifyEvent->GetEventData().c_str());

	IConferencePtr l_pConference = CConferenceManager::Instance()->GetConferenceByName(l_hcpConferenceEvt.m_oBody.m_strConferenceName);
	if (nullptr == l_pConference)
	{
		//l_pConference->GetSrcCTICallId
		ICC_LOG_WARNING(m_pLog, "not find conference,ConferenceName:[%s],hcpCallId:[%s]!!!!",
			l_hcpConferenceEvt.m_oBody.m_strConferenceName.c_str(),
			l_hcpConferenceEvt.m_oBody.m_strHcpCallId.c_str());

		CConferenceManager::Instance()->CreateCTIConferenceEx(l_hcpConferenceEvt.m_oBody.m_strConferenceName, l_hcpConferenceEvt.m_oBody.m_strHcpCallId);
	}
}


std::string CFSAesSwitchManager::_GetConferenceHangupType(const std::string& p_strCallDirection, const std::string& p_strSrcCaller, const std::string& p_strLastCallOverMember)
{
	std::string l_strHangupType = HangupTypeString[HANGUP_TYPE_CALLED];
	if (p_strCallDirection == CallDirectionString[CALL_DIRECTION_IN])
	{
		//呼入的,报警人最后挂机
		if (p_strSrcCaller != p_strLastCallOverMember)
		{
			//报警人比其他成员先挂机，就是主叫挂机
			l_strHangupType = HangupTypeString[HANGUP_TYPE_CALLER];
		}

	}
	else
	{
		if (p_strSrcCaller != p_strLastCallOverMember)
		{
			//主叫挂断
			l_strHangupType = HangupTypeString[HANGUP_TYPE_CALLER];
		}
	}
	return l_strHangupType;
}

//会议释放处理
void CFSAesSwitchManager::_DoHcpConferenceReleaseEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpConferenceEvent l_hcpConferenceEvt;
	if (!l_hcpConferenceEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "processing msg from aes server,cmd:[%s],data:[%s]", p_strEventName.c_str(), p_pHcpNotifyEvent->GetEventData().c_str());

	const std::string l_strConferenceName = l_hcpConferenceEvt.m_oBody.m_strConferenceName;
	IConferencePtr l_pConference = CConferenceManager::Instance()->GetConferenceByName(l_strConferenceName);
	if (l_pConference)
	{
		std::string l_strConferenceId = l_pConference->GetConferenceId();
		std::string l_strSrcCTICallId = l_pConference->GetSrcCTICallId();
		std::string l_strCompere = l_pConference->GetCompere();
		std::string l_strHangupType = l_pConference->GetConferenceHangupType();

		std::string l_strLastCallOverMember = l_pConference->GetLastCallOverMember();
		CConferenceManager::Instance()->ReleaseConference(l_strConferenceId);

		ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByHcpCallId(l_hcpConferenceEvt.m_oBody.m_strHcpCallId);
		if (l_pCTICallPtr)
		{
			if (l_strHangupType.empty())
			{
				l_strHangupType = HangupTypeString[HANGUP_TYPE_CALLED];
				if (l_pCTICallPtr->GetCallerId() == l_strLastCallOverMember)
				{
					//发起者挂机导致的会议结束就是主机挂机
					l_strHangupType = HangupTypeString[HANGUP_TYPE_CALLER];
				}
			}

			ICC_LOG_DEBUG(m_pLog, "release conference,ConferenceName:[%s],ConferenceID:[%s],hcpCallId:[%s],srcCTICallId:[%s],HangupType:[%s],caller:[%s],LastCallOverMember:[%s]",
				l_strConferenceName.c_str(),
				l_strConferenceId.c_str(),
				l_hcpConferenceEvt.m_oBody.m_strHcpCallId.c_str(), l_strSrcCTICallId.c_str(), l_strHangupType.c_str(),
				l_pCTICallPtr->GetCallerId().c_str(),
				l_strLastCallOverMember.c_str());

			PostCallOver(l_hcpConferenceEvt.m_oBody.m_strHcpCallId, l_pCTICallPtr, l_strHangupType);
			//删除呼叫
			CCallManager::Instance()->DeleteCTICall(l_pCTICallPtr->GetCTICallRefId());
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "not find ctiCall,ConferenceName:[%s],hcpCallId:[%s]!!!!",l_strConferenceName.c_str(),l_hcpConferenceEvt.m_oBody.m_strHcpCallId.c_str());
		}
		
	}
	else
	{
		//CConferenceManager::Instance()->CreateCTIConference()
		ICC_LOG_ERROR(m_pLog, "not find conference,ConferenceName:[%s],hcpCallId:[%s]!!!!",l_strConferenceName.c_str(),l_hcpConferenceEvt.m_oBody.m_strHcpCallId.c_str());
	}
}

//会议成员增加删除事件处理
void CFSAesSwitchManager::_DoHcpConferenceMemberEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent)
{
	CHcpConferenceEvent l_hcpConferenceEvt;
	if (!l_hcpConferenceEvt.ParseString(p_pHcpNotifyEvent->GetEventData(), ICCGetIJsonFactory()->CreateJson()))
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "processing msg from aes server,cmd:[%s],data:[%s]", p_strEventName.c_str(), p_pHcpNotifyEvent->GetEventData().c_str());

	std::string l_strConferenceName = l_hcpConferenceEvt.m_oBody.m_strConferenceName;

	IConferencePtr l_pConferencePtr = CConferenceManager::Instance()->GetConferenceByName(l_strConferenceName);
	if (nullptr == l_pConferencePtr)
	{
		ICC_LOG_ERROR(m_pLog, "not find conference,ConferenceName:[%s],hcpCallId:[%s]!!!!",
			l_hcpConferenceEvt.m_oBody.m_strConferenceName.c_str(),
			l_hcpConferenceEvt.m_oBody.m_strHcpCallId.c_str());

		return;
	}


	bool l_bAddMeeting = false;
	if (p_strEventName == CTIEVT_CONFERENCE_MEMBER_ADD)
	{
		l_bAddMeeting = true;
	}

	std::string l_strMemberNum = l_hcpConferenceEvt.m_oBody.m_strMember;
	std::string l_strCTIConferenceId = l_pConferencePtr->GetConferenceId();
	std::string l_strHcpCallId = l_hcpConferenceEvt.m_oBody.m_strHcpCallId;
	std::string l_strCTICallId = l_pConferencePtr->GetSrcCTICallId();
	if (!l_bAddMeeting)
	{
		l_pConferencePtr->UpdateMemberCallState(l_strMemberNum, CallStateString[STATE_HANGUP], m_pDateTimePtr->CurrentDateTimeStr());
		int l_nAutoHangupFlag = 0;
		std::string l_strLastCallingMember;
		if (l_pConferencePtr->CheckConferenceIsOver(l_strLastCallingMember))
		{
			//会议中如果只剩下一个成员在呼叫中，CTI发呼叫挂断，结束会议
			if (!l_strLastCallingMember.empty())
			{
				//l_pConferencePtr->UpdateLastCallOverMember(l_strLastCallingMember);
				l_pConferencePtr->UpdateLastCallOverMember(l_strMemberNum);

				l_nAutoHangupFlag = 1;
				_AesAutoHangupCallRequest(l_strHcpCallId, l_strLastCallingMember);
			}
			else
			{
				l_nAutoHangupFlag = 2;
				_AesReleaseConferenceRequest(l_strCTIConferenceId, l_pConferencePtr->GetConferenceName(), l_strHcpCallId);
			}
		}
		l_pConferencePtr->DeleteMember(l_strMemberNum,"");
		ICC_LOG_DEBUG(m_pLog, "exit meeting,conferenceName:[%s],CTIConferenceId:[%s],hcpCallId:[%s],ctiCallId:[%s],target:[%s],AutoHangupFlag:[%d]",
			l_strConferenceName.c_str(), l_strCTIConferenceId.c_str(), l_strHcpCallId.c_str(), l_strCTICallId.c_str(), l_strMemberNum.c_str(), l_nAutoHangupFlag);
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "join meeting,conferenceName:[%s],CTIConferenceId:[%s],hcpCallId:[%s],ctiCallId:[%s],target:[%s],srcCalled:[%s],srcCaller:[%s]",
			l_strConferenceName.c_str(), l_strCTIConferenceId.c_str(), l_strHcpCallId.c_str(), l_strCTICallId.c_str(), l_strMemberNum.c_str(),
			l_pConferencePtr->GetSrcCalled().c_str(), l_pConferencePtr->GetSrcCaller().c_str());
	
		if (l_pConferencePtr->GetSrcCTICallId() == l_strHcpCallId && 
			!l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId.empty() && 
			l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId != l_strHcpCallId)
		{
			//话机拉会适配
			ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByHcpCallId(l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId);
			if (l_pCTICallPtr)
			{
				l_strCTICallId = l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId;
				l_pConferencePtr->SetSrcCTICallId(l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId);

				l_pCTICallPtr->SetCTIConferenceID(l_pConferencePtr->GetConferenceId(), l_pConferencePtr->GetCompere());
				l_pCTICallPtr->SetReplaceCallId(l_hcpConferenceEvt.m_oBody.m_strHcpCallId);
				l_pCTICallPtr->SetCallState(CallStateString[STATE_CONFERENCE], m_pDateTimePtr->CurrentDateTimeStr());

				ICC_LOG_DEBUG(m_pLog, "phone create meeting update,conferenceName:[%s],compere:[%s],ReplaceCallId:[%s],deviceCurrCallId:[%s],srcCalled:[%s],srcCaller:[%s]",
					l_strConferenceName.c_str(),
					l_pConferencePtr->GetCompere().c_str(),
					l_strHcpCallId.c_str(),
					l_hcpConferenceEvt.m_oBody.m_strOrgHcpCallId.c_str(), l_pConferencePtr->GetSrcCalled().c_str(), l_pConferencePtr->GetSrcCaller().c_str());

				//主持人 模拟发一条会议态通知
				CDeviceManager::Instance()->UpdateDeviceCallState(l_pConferencePtr->GetCompere(), l_strCTICallId, CallStateString[STATE_CONFERENCE],true);
				PostInitateDeviceState(l_pConferencePtr->GetCompere(), l_strCTICallId, CallStateString[STATE_CONFERENCE], l_pCTICallPtr->GetCallerId());
			}
		}

		if (CDeviceManager::Instance()->IsExtension(l_strMemberNum))
		{
			std::string l_strCallState = CDeviceManager::Instance()->GetDeviceCallState(l_strMemberNum, l_strCTICallId);
			if (l_strCallState.empty())
			{
				l_strCallState = CallStateString[STATE_TALK];
			}
			else if (l_strCallState == CallStateString[STATE_SILENTMONITOR])
			{
				//监听用会议实现，监听的发起者作为会议的主持人和会议的第三方加入当前呼叫，呼通后设置成员禁言
				_AesMemberMuteRequest(l_strCTIConferenceId, l_pConferencePtr->GetConferenceName(), l_strCTICallId, l_strMemberNum);
			}

			l_pConferencePtr->AddMemberEx(l_strMemberNum, TARGET_DEVICE_TYPE_INSIDE, l_strCallState, m_pDateTimePtr->CurrentDateTimeStr());

			if (!l_pConferencePtr->IsBargeInCreate() && !l_pConferencePtr->IsListenCreate() && !l_pConferencePtr->IsTakeoverCreate())
			{
				if (l_strMemberNum == l_pConferencePtr->GetCompere())
				{
					//模拟发一条会议态通知
					PostInitateDeviceState(l_strMemberNum, l_strCTICallId, CallStateString[STATE_CONFERENCE], l_pConferencePtr->GetSrcCaller());
				}
				else if (l_strMemberNum == l_pConferencePtr->GetSrcCaller() || l_strMemberNum == l_pConferencePtr->GetSrcCalled())
				{
					//模拟发一条会议态通知
					PostInitateDeviceState(l_strMemberNum, l_strCTICallId, CallStateString[STATE_CONFERENCE], l_pConferencePtr->GetCompere());
				}
				else
				{
					if (l_pConferencePtr->GetSrcCalled().empty() && l_pConferencePtr->GetSrcCaller().empty())
					{
						//话机上拉会 ,模拟发一条会议态通知
						CDeviceManager::Instance()->UpdateDeviceCallState(l_strMemberNum, l_strCTICallId, CallStateString[STATE_CONFERENCE],true);
						PostInitateDeviceState(l_strMemberNum, l_strCTICallId, CallStateString[STATE_CONFERENCE], l_pConferencePtr->GetCompere());
					}
				}
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "conference create type,conferenceName:[%s],IsBargeInCreate:[%d],IsListenCreate:[%d],IsTakeoverCreate:[%d]",
					l_strConferenceName.c_str(),
					l_pConferencePtr->IsBargeInCreate(),
					l_pConferencePtr->IsListenCreate(),
					l_pConferencePtr->IsTakeoverCreate());
			}
		}
		else
		{
			l_pConferencePtr->AddMemberEx(l_strMemberNum, TARGET_DEVICE_TYPE_OUTSIDE, CallStateString[STATE_TALK], m_pDateTimePtr->CurrentDateTimeStr());
		}
		
	}

}

