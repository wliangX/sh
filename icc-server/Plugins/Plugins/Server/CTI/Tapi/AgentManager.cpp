#include "Boost.h"

#include "AgentManager.h"
#include "DeviceManager.h"
#include "SysConfig.h"
#include "TaskManager.h"

//////////////////////////////////////////////////////////////////////////
CAgent::CAgent(std::string p_strACDGroup, std::string p_strAgent, std::string p_strPsw, std::string p_strDeviceNum)
{
	this->m_bDefaultReady = false;
	this->m_strACDGroup = p_strACDGroup;
	this->m_strAgent = p_strAgent;
	this->m_strPsw = p_strPsw;
	this->m_strDeviceNum = p_strDeviceNum;

	this->m_agentMode = AGENT_MODE_LOGOUT;
	this->m_agentReady = AGENT_NOTREADY;
}

CAgent::~CAgent()
{
	//
}

void CAgent::GetAgentPar(std::string& p_strACDGroup, std::string& p_strAgent, std::string& p_strPsw, std::string& p_strDeviceNum)
{
	p_strACDGroup = this->m_strACDGroup;
	p_strAgent = this->m_strAgent;
	p_strPsw = this->m_strPsw;
	p_strDeviceNum = this->m_strDeviceNum;
}

void CAgent::SetAgentStateString(const std::string& p_strAgentMode, const std::string& p_strAgentReady)
{
	if (p_strAgentMode.compare(AgentModeTypeString[AGENT_MODE_LOGIN]) == 0)
	{
		this->m_agentMode = AGENT_MODE_LOGIN;
	}
	else
	{
		this->m_agentMode = AGENT_MODE_LOGOUT;
	}

	if (p_strAgentReady.compare(AgentModeTypeString[AGENT_READY]) == 0)
	{
		this->m_agentReady = AGENT_READY;
	}
	else
	{
		this->m_agentReady = AGENT_NOTREADY;
	}
}

void CAgent::SetAgentMode(E_AGENT_MODE_TYPE p_agentMode, const std::string& p_strStateTime)
{
	m_agentMode = p_agentMode;
	m_strStateTime = p_strStateTime;
}
void CAgent::SetAgentState(E_AGENT_READY_TYPE p_agentState, const std::string& p_strStateTime)
{
	m_agentReady = p_agentState;
	m_strStateTime = p_strStateTime;
}
void CAgent::SetAgentState(E_AGENT_MODE_TYPE p_agentMode, E_AGENT_READY_TYPE p_agentReadyType, const std::string& p_strStateTime)
{
	SetAgentMode(p_agentMode, p_strStateTime);
	SetAgentState(p_agentReadyType, p_strStateTime);
}

bool CAgent::IsReady()
{
	if (!IsLogin())
		return false;

	if (this->m_agentReady != AGENT_READY)
		return false;

	return true;
}

bool CAgent::Login()
{
	this->m_agentMode = AGENT_MODE_LOGIN;

	return true;
}

bool CAgent::Logout()
{
	this->m_agentMode = AGENT_MODE_LOGOUT;
	this->m_agentReady = AGENT_NOTREADY;

	return true;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CAgentManager> CAgentManager::m_pAgentManager = nullptr;
CAgentManager::CAgentManager()
{
	m_bInitOver = false;
	m_pLog = nullptr;
	m_pDateTime = nullptr;
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
}

//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<CAgent> CAgentManager::GetAgentByDn(const std::string& p_strACDGrp, const std::string& p_strDeviceNum)
{
	boost::shared_ptr<CAgent> l_pAgent = nullptr;

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj->GetACDGroup().compare(p_strACDGrp) == 0 &&
			l_pAgentObj->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			l_pAgent = l_pAgentObj;

			break;
		}
	}

	return l_pAgent;
}
//////////////////////////////////////////////////////////////////////////

void CAgentManager::LoadAllAgent()
{
	//从配置文件获取 ACD 成员
	std::list<CACDGroup> l_acdGrpList = CSysConfig::Instance()->m_ACDGroupList;
	for each (auto l_pACDGrpObj in l_acdGrpList)
	{
		std::string l_strACDGrp = l_pACDGrpObj.m_strACDNum;
		std::vector<std::string> l_vTempAgentList = l_pACDGrpObj.m_strAgentList;

		for each (auto l_strAgent in l_vTempAgentList)
		{
			std::string l_strDeviceNum = l_strAgent;
			boost::shared_ptr<CAgent> l_pAgent = boost::make_shared<CAgent>(l_strACDGrp, l_strDeviceNum, "", l_strDeviceNum);
			if (l_pAgent)
			{
				l_pAgent->SetAgentStateTime(m_pDateTime->CurrentDateTimeStr());
				//if (CSysConfig::Instance()->FindDefaultReadyAgent(l_strAgent))
				//{
				//	l_pAgent->SetDefaultReady(true);
				//}

				{
					SAFE_LOCK(m_agentListMutex);
					m_agentList.push_back(l_pAgent);
				}
				

				ICC_LOG_DEBUG(m_pLog, "Load Agent, ACDGrp: [%s] AgentId: [%s], AgentPsw: [%s], DeviceNum: [%s]",
					l_strACDGrp.c_str(), l_strDeviceNum.c_str(), "", l_strDeviceNum.c_str());
			}
		}
	}
}
void CAgentManager::ClearAllAgent()
{
	SAFE_LOCK(m_agentListMutex);
	m_agentList.clear();
}
bool CAgentManager::AgentIsLogin(const std::string& p_strDevice, const std::string& p_strACDGrp)
{
	bool l_bIsLogin = false;

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_pAgentObj;
		if (l_pAgent && l_pAgent->GetDeviceNum().compare(p_strDevice) == 0)
		{
			l_bIsLogin = l_pAgent->IsLogin();

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
void CAgentManager::PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
	const std::string& p_strLoginMode, const std::string& p_strReadyState, const std::string& p_strStateTime, const std::string& p_strLogoutReason)
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
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask AgentStateEvent, TaskId: [%u] AgentId: [%s], ACD: [%s], LoginMode: [%s], ReadyState: [%s],strStateTime[%s]",
			l_lTaskId, p_strDevice.c_str(), p_strACDGrp.c_str(), p_strLoginMode.c_str(), p_strReadyState.c_str(), p_strStateTime.c_str());
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

	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj && l_pAgentObj->GetDeviceNum().compare(p_strDevice) == 0)
		{
			std::string l_strACDGrp = l_pAgentObj->GetACDGroup();

			if (l_pAgentObj->GetAgentMode() != l_agentMode || l_pAgentObj->GetAgentReady() != l_agentReady)
			{
				// Agent 状态发生改变，通知客户端
				l_pAgentObj->SetAgentMode(l_agentMode, l_strCurrentTime);
				l_pAgentObj->SetAgentState(l_agentReady, l_strCurrentTime);

				ICC_LOG_DEBUG(m_pLog, "QueryAgentStateSync, Agent: %s, ACD: %s, Mode:%s, ReadyState:%s",
					p_strDevice.c_str(), l_strACDGrp.c_str(), AgentModeTypeString[l_agentMode].c_str(), AgentReadyTypeString[l_agentReady].c_str());

				PostAgentState(p_strDevice, l_strACDGrp, AgentModeTypeString[l_agentMode], AgentReadyTypeString[l_agentReady], l_strCurrentTime);
			}
		}
	}
}

void CAgentManager::ProcesAgentState(const std::string& p_strDevice)
{
	std::string l_strACDGrp = "";
	std::string l_strReadyType = AgentReadyTypeString[AGENT_NOTREADY];
	boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(p_strDevice);
	if (l_pDevice)
	{
		if (0 != l_pDevice->GetDivertSettings(l_strReadyType))
		{
			ICC_LOG_ERROR(m_pLog, "ProcesAgentState,Device:%s,GetDivertSettings Error", p_strDevice.c_str());
			return;
		}
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "ProcesAgentState,Device:%s,Line == NULL", p_strDevice.c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "ProcesAgentState,Device[%s],GetDivertSettings State=[%s]", p_strDevice.c_str(), l_strReadyType.c_str());
	this->ReadyStateSync(p_strDevice, l_strACDGrp, l_strReadyType, "");
	return;
}

void CAgentManager::SetDefaultAgentState()
{
	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj)
		{
			bool l_bDefaultReady = l_pAgentObj->GetDefaultReady();
			std::string l_strAgentId= l_pAgentObj->GetDeviceNum();
			std::string l_strACDGrp = l_pAgentObj->GetACDGroup();

			ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
			if (l_pRequestNotif)
			{
				l_pRequestNotif->SetInitAgentState(true);
				l_pRequestNotif->SetAgentId(l_strAgentId);
				l_pRequestNotif->SetACDGrp(l_strACDGrp);
				l_pRequestNotif->SetReadyState(l_bDefaultReady ? ReadyState_Ready : ReadyState_NotReady);

				long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_SetAgentState, l_pRequestNotif);
				ICC_LOG_DEBUG(m_pLog, "AddCmdTask, SetAgentState, TaskId: [%u] AgentId: [%s]",
					l_lTaskId, l_strAgentId.c_str());
			}
			else
			{
				ICC_LOG_WARNING(m_pLog, "SetAgentState, Create CSetAgentStateNotif Obj Failed !!!");
			}
		}
	}
}
void CAgentManager::GetAgentListState()
{
	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj)
		{
			std::string l_strDeviceNum = l_pAgentObj->GetDeviceNum();

			IQueryAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CQueryAgentStateNotif>();
			if (l_pRequestNotif)
			{
				l_pRequestNotif->SetAgentId(l_strDeviceNum);

				long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_QueryAgentState, l_pRequestNotif);
				ICC_LOG_LOWDEBUG(m_pLog, "AddCmdTask, QueryAgentState, TaskId: [%u] AgentId: [%s]",
					l_lTaskId, l_strDeviceNum.c_str());
			}
			else
			{
				ICC_LOG_WARNING(m_pLog, "AQueryAgentState, Create CQueryAgentStateNotif Obj Failed !!!");
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
		{
			SAFE_LOCK(m_agentListMutex);
			for each (auto l_pAgentObj in m_agentList)
			{
				if (l_pAgentObj)
				{
					//	获取所有 ACD 的成员
					l_pResultNotif->AddAgentList(l_pAgentObj->GetDeviceNum(),
						l_pAgentObj->GetACDGroup(),
						l_pAgentObj->GetAgentStateTime());
				}
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetACDListRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetACDListRet, TaskId [%u], RequestId [%u]",l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "GetAgentList, Create CGetACDListResultNotif Obj Failed !!!");
	}
}
void CAgentManager::GetAgentList(long p_lRequestId, const std::string& p_strACDGrp)
{
	IGetAgentListResultNotifPtr l_pResultNotif = boost::make_shared<CGetAgentListResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		{
			SAFE_LOCK(m_agentListMutex);
			for each (auto l_pAgentObj in m_agentList)
			{
				if (l_pAgentObj)
				{
					if (!p_strACDGrp.empty())
					{
						//	获取某一 ACD 的成员
						if (l_pAgentObj->GetACDGroup().compare(p_strACDGrp) == 0)
						{
							l_pResultNotif->AddAgentList(l_pAgentObj->GetDeviceNum(),
								l_pAgentObj->GetACDGroup(),
								l_pAgentObj->GetAgentModeString(),
								l_pAgentObj->GetAgentReadyString(),
								l_pAgentObj->GetAgentStateTime());
						}
					}
					else
					{
						//	获取所有 ACD 的成员
						l_pResultNotif->AddAgentList(l_pAgentObj->GetDeviceNum(),
							l_pAgentObj->GetACDGroup(),
							l_pAgentObj->GetAgentModeString(),
							l_pAgentObj->GetAgentReadyString(),
							l_pAgentObj->GetAgentStateTime());
					}
				}
			}
		}
		
		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetAgentListRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetAgentListRet, TaskId [%u], RequestId [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "GetAgentList, Create CGetAgentListResultNotif Obj Failed !!!");
	}
}

std::string CAgentManager::GetLocalTime()
{
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	return l_strCurrentTime;
}
bool CAgentManager::GetACDGrpByDn(std::string& p_strDestACDGrp, const std::string& p_strSrcDeviceNum)
{
	bool l_bFind = false;

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
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
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj && l_pAgentObj->GetACDGroup().compare(p_strACDGrp) == 0 && l_pAgentObj->IsReady())
		{
			l_nCount++;
		}
	}

	return l_nCount;
}
void CAgentManager::GetReadyAgent(const std::string& p_strACDGrp, long p_lRequestId)
{
	IGetReadyAgentResultNotifPtr l_pResultNotif = boost::make_shared<CGetReadyAgentResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);

		{
			SAFE_LOCK(m_agentListMutex);
			for each (auto l_pAgentObj in m_agentList)
			{
				if (l_pAgentObj && l_pAgentObj->GetACDGroup().compare(p_strACDGrp) == 0 && l_pAgentObj->IsReady())
				{
					std::string l_strDeviceNum = l_pAgentObj->GetDeviceNum();
					if (CDeviceManager::Instance()->DeviceIsFree(l_strDeviceNum))
					{
						l_pResultNotif->SetResult(true);
						l_pResultNotif->SetReadyAgent(l_strDeviceNum);

						break;
					}
				}
			}
		}

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_GetReadyAgentRet, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask GetReadyAgentRet, TaskId [%u], RequestId [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "GetReadyAgent, Create CGetReadyAgentResultNotif Obj Failed !!!");
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

void CAgentManager::LoginModeSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE p_nLoginMode)
{
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj && l_pAgentObj->GetDeviceNum().compare(p_strDevice) == 0 /*&& l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0*/)
		{
			//  不同 ACD 组，同一 Agent 成员状态同时改变 
			l_pAgentObj->SetAgentMode(p_nLoginMode, l_strCurrentTime);
			if (p_nLoginMode == AGENT_MODE_LOGOUT)
			{
				l_pAgentObj->SetAgentState(AGENT_NOTREADY, l_strCurrentTime);
			}
			else
			{
				l_pAgentObj->SetAgentState(AGENT_READY, l_strCurrentTime);
			}

			std::string l_strACDGrp = l_pAgentObj->GetACDGroup();
			std::string l_strLoginMode = l_pAgentObj->GetAgentModeString();
			std::string l_strReadyState = l_pAgentObj->GetAgentReadyString();

			ICC_LOG_DEBUG(m_pLog, "LoginModeSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
				p_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

			PostAgentState(p_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime);
		}
	}
}
void CAgentManager::ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState, const std::string& p_strLogoutReason)
{
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	//std::lock_guard<std::mutex> guard(m_agentListMutex);
	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj && l_pAgentObj->GetDeviceNum().compare(p_strDevice) == 0 /*&& l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0*/)
		{
			//  不同 ACD 组，同一 Agent 成员状态同时改变 
			l_pAgentObj->SetAgentMode(AGENT_MODE_LOGIN, l_strCurrentTime);
			l_pAgentObj->SetAgentState(p_strReadyState.compare(AgentReadyTypeString[AGENT_READY]) == 0 ? AGENT_READY : AGENT_NOTREADY, l_strCurrentTime);

			std::string l_strACDGrp = l_pAgentObj->GetACDGroup();
			std::string l_strLoginMode = l_pAgentObj->GetAgentModeString();
			std::string l_strReadyState = l_pAgentObj->GetAgentReadyString();

			ICC_LOG_DEBUG(m_pLog, "ReadyStateSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
				p_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

			PostAgentState(p_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime, p_strLogoutReason);
		}
	}
}

void CAgentManager::SetInitAgentState()
{
	ICC_LOG_DEBUG(m_pLog, "SetInitAgentState begin");
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

	SAFE_LOCK(m_agentListMutex);
	for each (auto l_pAgentObj in m_agentList)
	{
		if (l_pAgentObj)
		{
			std::string l_strReadyType = AgentReadyTypeString[AGENT_NOTREADY];
			std::string l_strDevice = l_pAgentObj->GetDeviceNum();
			boost::shared_ptr<CTapiLine> l_pDevice = CDeviceManager::Instance()->FindLineByDn(l_strDevice);
			if (l_pDevice)
			{
				if (0 != l_pDevice->GetDivertSettings(l_strReadyType))
				{
					ICC_LOG_ERROR(m_pLog, "SetInitAgentState, Device: %s, GetDivertSettings Error", l_strDevice.c_str());
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "[%s] GetDivertSettings l_strReadyType:[%s]", l_strDevice.c_str(), l_strReadyType.c_str());
					l_pAgentObj->SetAgentState(l_strReadyType.compare(AgentReadyTypeString[AGENT_READY]) == 0 ? AGENT_READY : AGENT_NOTREADY, l_strCurrentTime);
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "SetInitAgentState, Device: %s, Line == NULL", l_strDevice.c_str());
			}
			l_pAgentObj->SetAgentMode(AGENT_MODE_LOGIN, l_strCurrentTime);
			std::string l_strACDGrp = l_pAgentObj->GetACDGroup();
			std::string l_strLoginMode = l_pAgentObj->GetAgentModeString();
			std::string l_strReadyState = l_pAgentObj->GetAgentReadyString();
			ICC_LOG_DEBUG(m_pLog, "SetInitAgentState, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
				l_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());
			PostAgentState(l_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime, "");
		}
	}
	ICC_LOG_DEBUG(m_pLog, "SetInitAgentState end");
}

