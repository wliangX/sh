#include "Boost.h"

#include "SysConfig.h"
#include "AgentManager.h"
#include "DeviceManager.h"
#include "TaskManager.h"

#define SLEEP_TIMESPAN 100			//单位：毫秒

boost::shared_ptr<CAgentManager> CAgentManager::m_pAgentManager = nullptr;
CAgentManager::CAgentManager()
{
	m_pLog = nullptr;
	m_pDateTimePtr = nullptr;
	m_pQueryAgentStateThread = nullptr;

	m_bAgentIsLoad = false;
	m_bThreadAlive = false;

	m_oLastQueryTime = 0;
	m_bSwitchConnectFlag = false;
}

CAgentManager::~CAgentManager()
{
	//
}
boost::shared_ptr<CAgentManager> CAgentManager::Instance()
{
	if (m_pAgentManager == nullptr)
	{
		m_pAgentManager = boost::make_shared<CAgentManager>();
	}

	return m_pAgentManager;
}
void CAgentManager::ExitInstance()
{
	ClearAllAgent();

	if (m_pAgentManager)
	{
		m_pAgentManager.reset();
	}
}
//////////////////////////////////////////////////////////////////////////
void CAgentManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();

	LoadAllAgent();
}
void CAgentManager::OnStart()
{
	m_bThreadAlive = false;
	m_oLastQueryTime = m_pDateTimePtr->CurrentDateTime();
}
void CAgentManager::OnStop()
{
	StopQueryAgentStateThread();
	ResetDefaultReadyList();
}

void CAgentManager::SetSwitchConnect(bool p_bConnectFlag)
{
	m_bSwitchConnectFlag = p_bConnectFlag;
	if (!p_bConnectFlag)
	{
		ResetDefaultReadyList();
	}
	else
	{
		StartQueryAgentStateThread();
	}
}
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CAgent> CAgentManager::GetAgentByDn(const std::string& p_strACDGrp, const std::string& p_strDeviceNum)
{
	boost::shared_ptr<CAgent> l_pAgent = nullptr;

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		if (l_agentObj && l_agentObj->GetACDGroup().compare(p_strACDGrp) == 0)
		{
			l_pAgent = l_agentObj;

			break;
		}
	}

	return l_pAgent;
}
//////////////////////////////////////////////////////////////////////////

void CAgentManager::LoadAllAgent()
{
	if (!m_bAgentIsLoad)
	{
		m_bAgentIsLoad = true;

		//从配置文件获取 ACD 成员
		std::list<CACDGroup> l_acdGrpList = CSysConfig::Instance()->m_ACDGroupList;
		for (auto l_acdGrpObj : l_acdGrpList)
		{
			std::string l_strACDGrp = l_acdGrpObj.m_strACDNum;
			std::vector<std::string> l_agentList = l_acdGrpObj.m_strAgentList;

			for (auto l_agentObj : l_agentList)
			{
				std::string l_strDeviceNum = l_agentObj;
				boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindACDDevice(l_strDeviceNum);
				if (l_pDevice)
				{
					std::string l_strAgentId = l_pDevice->GetAgentId();
					std::string l_strAgentPsw = l_pDevice->GetAgentPsw();

					boost::shared_ptr<CAgent> pAgent = boost::make_shared<CAgent>(l_strACDGrp, l_strAgentId, l_strAgentPsw, l_strDeviceNum);
					pAgent->SetAgentStateTime(m_pDateTimePtr->CurrentDateTimeStr());
					if (CSysConfig::Instance()->FindDefaultReadyAgent(l_strDeviceNum))
					{
						pAgent->SetDefaultReady(true);
					}
					m_agentList.push_back(pAgent);

					ICC_LOG_DEBUG(m_pLog, "Load Agent, ACDGrp: [%s] AgentId: [%s], AgentPsw: [%s], DeviceNum: [%s]",
						l_strACDGrp.c_str(), l_strAgentId.c_str(), l_strAgentPsw.c_str(), l_strDeviceNum.c_str());
				}
			}// end agentList

		}
	}
}
void CAgentManager::ClearAllAgent()
{
	SAFE_LOCK(m_agentListMutex);
	m_agentList.clear();

	m_bAgentIsLoad = false;
}
void CAgentManager::ResetDefaultReadyList()
{
	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent)
		{
			std::string l_strDeviceNum = l_pAgent->GetDeviceNum();

			if (CSysConfig::Instance()->FindDefaultReadyAgent(l_strDeviceNum))
			{
				l_pAgent->SetDefaultReady(true);
			}
		}
	}
}
bool CAgentManager::AgentIsLogin(const std::string& p_strDevice, const std::string& p_strACDGrp)
{
	bool l_bIsLogin = false;

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		if (l_agentObj && l_agentObj->GetDeviceNum().compare(p_strDevice) == 0)
		{
			l_bIsLogin = l_agentObj->IsLogin();

			break;
		}
	}

	return l_bIsLogin;
}
void CAgentManager::AgentLogin(const std::string& p_strDevice, const std::string& p_strACDGrp)
{
	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strDevice);
		l_pRequestNotif->SetACDGrp(p_strACDGrp);
		l_pRequestNotif->SetLoginMode(LoginMode_Login);

		long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_AgentLogin, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask AgentLogin, TaskId: [%u] AgentId: [%s], ACD: [%s]",
			l_lTaskId, p_strDevice.c_str(), p_strACDGrp.c_str());
	}
	
}
void CAgentManager::SetAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState)
{
	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetInitAgentState(true);
		l_pRequestNotif->SetAgentId(p_strDevice);
		l_pRequestNotif->SetACDGrp(p_strACDGrp);
		l_pRequestNotif->SetReadyState(p_strReadyState);

		long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_SetAgentState, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask, SetAgentState, TaskId: [%u] AgentId: [%s], ACDGrp: [%s], State: [%s]",
			l_lTaskId, p_strDevice.c_str(), p_strACDGrp.c_str(), p_strReadyState.c_str());
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "SetAgentState, Create CSetAgentStateNotif Obj Failed !!!");
	}
}

void CAgentManager::PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
	const std::string& p_strLoginMode, const std::string& p_strReadyState, const std::string& p_strStateTime)
{
	IAgentStateNotifPtr l_pAgentNotif = boost::make_shared<CAgentStateNotif>();
	if (l_pAgentNotif)
	{
		l_pAgentNotif->SetAgentId(p_strDevice);
		l_pAgentNotif->SetACDGrp(p_strACDGrp);
		l_pAgentNotif->SetLoginMode(p_strLoginMode);
		l_pAgentNotif->SetReadyState(p_strReadyState);
		l_pAgentNotif->SetStateTime(p_strStateTime);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_AgentStateEvent, l_pAgentNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask AgentStateEvent, TaskId: [%u] AgentId: [%s], ACD: [%s], LoginMode: [%s], ReadyState: [%s]",
			l_lTaskId, p_strDevice.c_str(), p_strACDGrp.c_str(), p_strLoginMode.c_str(), p_strReadyState.c_str());
	}
}
void CAgentManager::OnAgentState(const std::string& p_strDevice, const std::string& p_strState)
{
	/*case AG_NOT_READY: return ("AG_NOT_READY");
	case AG_NULL: return ("AG_NULL");
	case AG_READY: return ("AG_READY");
	case AG_WORK_NOT_READY: return ("AG_WORK_NOT_READY");
	case AG_WORK_READY: return ("AG_WORK_READY");*/
	E_AGENT_MODE_TYPE	l_agentMode = AGENT_MODE_LOGOUT;		
	E_AGENT_READY_TYPE	l_agentReady = AGENT_NOTREADY;
	if (p_strState.compare("AG_NULL") == 0)
	{
		l_agentMode = AGENT_MODE_LOGOUT;
		l_agentReady = AGENT_NOTREADY;
	}
	else if (p_strState.compare("AG_NOT_READY") == 0)
	{
		l_agentMode = AGENT_MODE_LOGIN;
		l_agentReady = AGENT_NOTREADY;
	}
	else if (p_strState.compare("AG_READY") == 0)
	{
		l_agentMode = AGENT_MODE_LOGIN;
		l_agentReady = AGENT_READY;
	}
	else if (p_strState.compare("AG_WORK_NOT_READY") == 0)
	{
		l_agentMode = AGENT_MODE_LOGIN;
		l_agentReady = AGENT_NOTREADY;
	}
	else if (p_strState.compare("AG_WORK_READY") == 0)
	{
		l_agentMode = AGENT_MODE_LOGIN;
		l_agentReady = AGENT_READY;
	}
	else
	{
		l_agentMode = AGENT_MODE_LOGOUT;
		l_agentReady = AGENT_NOTREADY;
	}

	ICC_LOG_DEBUG(m_pLog, "OnAgentState enter, device: %s, state: %s, Mode:%s, ReadyState:%s", p_strDevice.c_str(), p_strState.c_str(),
		AgentModeTypeString[l_agentMode].c_str(), AgentReadyTypeString[l_agentReady].c_str());

	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent && l_pAgent->GetDeviceNum().compare(p_strDevice) == 0)
		{
			std::string l_strACDGrp = l_pAgent->GetACDGroup();

			if (l_pAgent->GetAgentMode() != l_agentMode || l_pAgent->GetAgentReady() != l_agentReady)
			{
				// Agent 状态发生改变，通知客户端
				l_pAgent->SetAgentMode(l_agentMode, l_strCurrentTime);
				l_pAgent->SetAgentState(l_agentReady, l_strCurrentTime);

				ICC_LOG_DEBUG(m_pLog, "QueryAgentStateSync, Agent: %s, ACD: %s, Mode:%s, ReadyState:%s",
					p_strDevice.c_str(), l_strACDGrp.c_str(), AgentModeTypeString[l_agentMode].c_str(), AgentReadyTypeString[l_agentReady].c_str());

				PostAgentState(p_strDevice, l_strACDGrp, AgentModeTypeString[l_agentMode], AgentReadyTypeString[l_agentReady], l_strCurrentTime);
			}

			if (l_pAgent->GetDefaultReady())
			{
				l_pAgent->SetDefaultReady(false);

				if (l_agentReady == AGENT_NOTREADY)
				{
					SetAgentState(p_strDevice, l_strACDGrp, AgentReadyTypeString[AGENT_READY]);
				}
			}
		}
	}
}

void CAgentManager::GetAgentListState()
{
	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent)
		{
			std::string l_strDeviceNum = l_pAgent->GetDeviceNum();
			
			IQueryAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CQueryAgentStateNotif>();
			if (l_pRequestNotif)
			{
				l_pRequestNotif->SetAgentId(l_strDeviceNum);

				long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_QueryAgentState, l_pRequestNotif);
				ICC_LOG_LOWDEBUG(m_pLog, "AddCmdTask, QueryAgentState, TaskId: [%u] AgentId: [%s]",
					l_lTaskId, l_strDeviceNum.c_str());
			}
		}
	}
}
void CAgentManager::GetACDList(long p_lRequestId)
{
	IGetACDListResultNotifPtr l_pResultNotif = boost::make_shared<CGetACDListResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		SAFE_LOCK(m_agentListMutex);
		for (auto l_agentObj : m_agentList)
		{
			boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
			if (l_pAgent)
			{
				//	获取所有 ACD 的成员
				l_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
					l_pAgent->GetACDGroup(),
					l_pAgent->GetAgentStateTime());
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetACDListRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetACDListRet, TaskId [%u], RequestId [%u]",
			l_lTaskId, p_lRequestId);
	}
}
void CAgentManager::GetAgentList(long p_lRequestId, const std::string& p_strACDGrp)
{
	IGetAgentListResultNotifPtr l_pResultNotif = boost::make_shared<CGetAgentListResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		SAFE_LOCK(m_agentListMutex);
		for (auto l_agentObj : m_agentList)
		{
			boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
			if (l_pAgent)
			{
				if (!p_strACDGrp.empty())
				{
					//	获取某一 ACD 的成员
					if (l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0)
					{
						l_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
							l_pAgent->GetACDGroup(),
							l_pAgent->GetAgentModeString(),
							l_pAgent->GetAgentReadyString(),
							l_pAgent->GetAgentStateTime());
					}
				}
				else
				{
					//	获取所有 ACD 的成员
					l_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
						l_pAgent->GetACDGroup(),
						l_pAgent->GetAgentModeString(),
						l_pAgent->GetAgentReadyString(),
						l_pAgent->GetAgentStateTime());
				}
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetAgentListRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetAgentListRet, TaskId [%u], RequestId [%u]",
			l_lTaskId, p_lRequestId);
	}
}

std::string CAgentManager::GetLocalTime()
{
	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();

	return l_strCurrentTime;
}
bool CAgentManager::GetACDGrpByDn(std::string& p_strDestACDGrp, const std::string& p_strSrcDeviceNum)
{
	bool l_bFind = false;

	SAFE_LOCK(m_agentListMutex);
	for (auto l_pAgentObj : m_agentList)
	{
		if (l_pAgentObj && l_pAgentObj->GetDeviceNum().compare(p_strSrcDeviceNum) == 0)
		{
			l_bFind = true;
			p_strDestACDGrp = l_pAgentObj->GetACDGroup();

			break;
		}
	}

	return l_bFind;
}
int CAgentManager::GetReadyAgentCount(const std::string& p_strACDGrp)
{
	int l_nCount = 0;

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent && l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0 && l_pAgent->IsReady())
		{
			l_nCount++;
		}
	}

	return l_nCount;
}

void CAgentManager::GetFreeAgentList(long p_lRequestId, const std::string& p_strACDGrp, const std::string& p_strDeptCode)
{
	IGetFreeAgentResultNotifPtr l_pResultNotif = boost::make_shared<CGetFreeAgentResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		
		std::map<std::string,boost::shared_ptr<CAgent>> l_mapFreeAgent;
		{
			SAFE_LOCK(m_agentListMutex);
			for (auto l_agentObj : m_agentList)
			{
				boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
				if (l_pAgent)
				{
					if (CDeviceManager::Instance()->DeviceIsFree(l_pAgent->GetDeviceNum()))
					{
						//获取空闲的成员
						if (l_pAgent->GetAgentReadyString() == ReadyState_Ready)
						{
							l_mapFreeAgent[l_pAgent->GetDeviceNum()] = l_pAgent;
						}
					}
				}
			}
		}
		int nCount = 0;
		for (auto l_freeAgentObj : l_mapFreeAgent)
		{
			l_pResultNotif->AddAgentList(l_freeAgentObj.second->GetDeviceNum(),
				l_freeAgentObj.second->GetACDGroup(),
				l_freeAgentObj.second->GetAgentModeString(),
				l_freeAgentObj.second->GetAgentReadyString(),
				l_freeAgentObj.second->GetAgentStateTime());
			nCount++;
		}
		l_pResultNotif->SetDeptCode(p_strDeptCode);
		l_pResultNotif->SetResult(true);
		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetFreeAgentListRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetFreeAgentList, TaskId [%u], RequestId [%u],nCount[%d]",l_lTaskId, p_lRequestId, nCount);
	}
}

void CAgentManager::GetReadyAgent(const std::string& p_strACDGrp, long p_lRequestId)
{
	IGetReadyAgentResultNotifPtr l_pResultNotif = boost::make_shared<CGetReadyAgentResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		SAFE_LOCK(m_agentListMutex);
		for (auto l_agentObj : m_agentList)
		{
			boost::shared_ptr<CAgent>l_pAgent = l_agentObj;
			if (l_pAgent && l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0 && l_pAgent->IsReady())
			{
				if (CDeviceManager::Instance()->DeviceIsFree(l_pAgent->GetDeviceNum()))
				{
					l_pResultNotif->SetResult(true);
					l_pResultNotif->SetReadyAgent(l_pAgent->GetDeviceNum());

					break;
				}
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetReadyAgentRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetReadyAgentRet, TaskId [%u], RequestId [%u]",
			l_lTaskId, p_lRequestId);
	}
}
void CAgentManager::GetAgentState(const std::string& p_strInDeviceNum, std::string& p_strOutLodinMode, std::string& p_strOutReadyState)
{
	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent>l_pAgent = l_agentObj;
		if (l_pAgent && l_pAgent->GetDeviceNum().compare(p_strInDeviceNum) == 0)
		{
			p_strOutLodinMode = l_pAgent->GetAgentModeString();
			p_strOutReadyState = l_pAgent->GetAgentReadyString();

			break;
		}
	}
}
/*
bool CAgentManager::GetReadyAgent(const std::string& p_strACDGrp, std::string& p_strDevice)
{
	bool bRet = false;

	m_agentListMutex.lock();
	auto itAgent = m_agentList.begin();
	while (itAgent != m_agentList.end())
	{
		boost::shared_ptr<CAgent> pAgent = *itAgent;
		if (pAgent && pAgent->GetACDGroup().compare(p_strACDGrp) == 0 && pAgent->IsReady())
		{
			if (CDeviceManager::Instance()->DeviceIsFree(pAgent->GetDeviceNum()))
			{
				bRet = true;
				p_strDevice = pAgent->GetDeviceNum();

				break;
			}
		}

		++itAgent;
	}
	m_agentListMutex.unlock(); 

	return bRet;
}*/

void CAgentManager::DoQueryAgentState()
{
	ICC_LOG_DEBUG(m_pLog, "=============== CheckAgentState Thread Start!! =====================");

	while (CAgentManager::Instance()->m_bThreadAlive)
	{
		if (CAgentManager::Instance()->QueryAgentStateIsTimeout())
		{
			if (CAgentManager::Instance()->m_bSwitchConnectFlag)
			{
				CAgentManager::Instance()->GetAgentListState();
				CAgentManager::Instance()->SetLastQueryTime();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIMESPAN));
			//boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIMESPAN));
		}
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ CheckAgentState Thread Exit!! ------------------");
}

void CAgentManager::SetLastQueryTime()
{
	m_oLastQueryTime = m_pDateTimePtr->CurrentDateTime();
}
bool CAgentManager::QueryAgentStateIsTimeout()
{
	bool l_bIsTimeout = false;

	int l_nQueryTimeSpan = CSysConfig::Instance()->GetAgentStateTime();
	DateTime::CDateTime m_oCurrentTime = m_pDateTimePtr->CurrentDateTime();
	DateTime::CDateTime m_oEndTime = m_pDateTimePtr->AddSeconds(m_oLastQueryTime, l_nQueryTimeSpan);
	if (m_oCurrentTime > m_oEndTime)
	{
		l_bIsTimeout = true;
	}

	return l_bIsTimeout;
}
void CAgentManager::StartQueryAgentStateThread()
{
	if (!m_bThreadAlive)
	{
		m_bThreadAlive = true;
		m_oLastQueryTime = m_pDateTimePtr->CurrentDateTime();

		m_pQueryAgentStateThread = boost::make_shared<boost::thread>(boost::bind(&CAgentManager::DoQueryAgentState, this));
	}
}
	
void CAgentManager::StopQueryAgentStateThread()
{
	if (m_bThreadAlive)
	{
		m_bThreadAlive = false;

		if (m_pQueryAgentStateThread)
		{
			m_pQueryAgentStateThread->join();
		}
	}
}

void CAgentManager::LoginModeSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE p_nLoginMode)
{
	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent && l_pAgent->GetDeviceNum().compare(p_strDevice) == 0 /*&& l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0*/)
		{
			//  不同 ACD 组，同一 Agent 成员状态同时改变 
			l_pAgent->SetAgentMode(p_nLoginMode, l_strCurrentTime);
			if (p_nLoginMode == AGENT_MODE_LOGOUT)
			{
				l_pAgent->SetAgentState(AGENT_NOTREADY, l_strCurrentTime);
			}

			std::string l_strACDGrp = l_pAgent->GetACDGroup();
			std::string l_strLoginMode = l_pAgent->GetAgentModeString();
			std::string l_strReadyState = l_pAgent->GetAgentReadyString();

			ICC_LOG_DEBUG(m_pLog, "LoginModeSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
				p_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

			PostAgentState(p_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime);
		}
	}
}
void CAgentManager::ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState)
{
	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent)
		{
			if (l_pAgent->GetDeviceNum().compare(p_strDevice) == 0 /*&& l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0*/)
			{
				//  不同 ACD 组，同一 Agent 成员状态同时改变 
				l_pAgent->SetAgentState(p_strReadyState.compare(AgentReadyTypeString[AGENT_READY]) == 0 ? AGENT_READY : AGENT_NOTREADY, l_strCurrentTime);

				std::string l_strACDGrp = l_pAgent->GetACDGroup();
				std::string l_strLoginMode = l_pAgent->GetAgentModeString();
				std::string l_strReadyState = l_pAgent->GetAgentReadyString();

				ICC_LOG_DEBUG(m_pLog, "ReadyStateSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
					p_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

				PostAgentState(p_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime);
			}
		}
	}
}

void CAgentManager::SetCTITestData()
{
	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();
	SAFE_LOCK(m_agentListMutex);
	for (auto l_agentObj : m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj;
		if (l_pAgent)
		{
			std::string l_strDevice = l_pAgent->GetDeviceNum();
			l_pAgent->SetAgentMode(AGENT_MODE_LOGIN, l_strCurrentTime);
			l_pAgent->SetAgentState(AGENT_READY, l_strCurrentTime);

			std::string l_strACDGrp = l_pAgent->GetACDGroup();
			std::string l_strLoginMode = l_pAgent->GetAgentModeString();
			std::string l_strReadyState = l_pAgent->GetAgentReadyString();

			ICC_LOG_DEBUG(m_pLog, "ReadyStateSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",l_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

			PostAgentState(l_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime);
		}
	}
}
