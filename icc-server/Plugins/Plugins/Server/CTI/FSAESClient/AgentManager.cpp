#include "Boost.h"
#include "AgentManager.h"
#include "DeviceManager.h"
#include "SysConfig.h"
#include "CTIFSAesDefine.h"
#include "FSAesSwitchManager.h"
#define SLEEP_TIMESPAN 100			//单位：毫秒
//////////////////////////////////////////////////////////////////////
//
boost::shared_ptr<CAgentManager> CAgentManager::m_pAgentManager = nullptr;
CAgentManager::CAgentManager()
	:m_bAgentIsLoad(false),
	m_pLog(nullptr),
	m_pDateTimePtr(nullptr)
{

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
	_ClearAllAgent();
	if (m_pAgentManager)
	{
		m_pAgentManager.reset();
	}
}

void CAgentManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();
	_LoadAllAgent();
}

void CAgentManager::OnStart()
{

}

void CAgentManager::OnStop()
{

}

void CAgentManager::InitFreeAgentList(const std::vector<std::string>& p_aesFreeAgentList)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	for (size_t i = 0; i < p_aesFreeAgentList.size(); i++)
	{
		std::string l_strAgentId = p_aesFreeAgentList[i];
		for (auto l_AgentObj : l_mapAgents)
		{
			boost::shared_ptr<CAgent> l_pAgent = l_AgentObj.second;
			if (l_pAgent)
			{
				if (l_pAgent->GetAgent() == l_strAgentId)
				{
					l_pAgent->SetAgentState(AGENT_MODE_LOGIN, AGENT_READY, m_pDateTimePtr->CurrentDateTimeStr());
					ICC_LOG_DEBUG(m_pLog, "init agent state idle,agentId:[%s],key:[%s]", p_aesFreeAgentList[i].c_str(), l_AgentObj.first.c_str());
				}
			}
		}
	}
	
}
//////////////////////////////////////////////////////////////////////
//
void CAgentManager::AddAgent(std::string &p_strAgentId, std::string& p_strAgentPsw, std::string& p_strDeviceNum)
{
	if (p_strAgentId.empty() || p_strDeviceNum.empty())
	{
		return;
	}
	
	//从配置文件获取 ACD 成员
	std::list<CACDGroup> l_acdGrpList = CSysConfig::Instance()->m_ACDGroupList;
	for (auto l_acdGrpObj : l_acdGrpList)
	{
		std::string l_strACDGrp = l_acdGrpObj.m_strACDNum;
		std::vector<std::string> l_agentList = l_acdGrpObj.m_strAgentList;
		for (auto l_agentObj : l_agentList)
		{
			if (l_agentObj == p_strDeviceNum)
			{
				boost::shared_ptr<CAgent> pAgent = boost::make_shared<CAgent>(l_strACDGrp,p_strAgentId, p_strAgentPsw, p_strDeviceNum);
				pAgent->SetAgentStateTime(m_pDateTimePtr->CurrentDateTimeStr());
				if (CSysConfig::Instance()->FindDefaultReadyAgent(p_strDeviceNum))
				{
					pAgent->SetDefaultReady(true);
				}
				{
					SAFE_LOCK(m_mapAgentTBMutex);
					std::string l_strKey = p_strDeviceNum + "_" + l_strACDGrp;
					m_mapAgentTB[l_strKey] = pAgent;

					ICC_LOG_DEBUG(m_pLog, "AddAgent, ACDGrp: [%s] AgentId: [%s], AgentPsw: [%s], DeviceNum: [%s]", l_strACDGrp.c_str(), p_strAgentId.c_str(), p_strAgentPsw.c_str(), p_strDeviceNum.c_str());
				}

			}
		}
	}
}

bool CAgentManager::GetAgent(IGetAgentListResultNotifPtr p_pResultNotif, const std::string p_strACDNum)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}
	if (p_pResultNotif == nullptr)
	{
		return false;
	}

	for (auto l_AgentObj : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_AgentObj.second;
		if (l_pAgent)
		{
			if (!p_strACDNum.empty())
			{
				//	获取某一 ACD 的成员
				if (l_pAgent->GetACDGroup().compare(p_strACDNum) == 0)
				{
					p_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
						l_pAgent->GetACDGroup(),
						l_pAgent->GetAgentModeString(),
						l_pAgent->GetAgentReadyString(),
						l_pAgent->GetAgentStateTime());
				}
			}
			else
			{
				//	获取所有 ACD 的成员
				p_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
					l_pAgent->GetACDGroup(),
					l_pAgent->GetAgentModeString(),
					l_pAgent->GetAgentReadyString(),
					l_pAgent->GetAgentStateTime());
			}
		}
	}
	return true;
}

bool CAgentManager::GetAgent(IGetFreeAgentResultNotifPtr p_pResultNotif, const std::string p_strACDNum)
{
	if (p_pResultNotif == nullptr)
	{
		return false;
	}

	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	std::map<std::string, boost::shared_ptr<CAgent>> l_mapFreeAgent;
	{
		for (auto l_agentObj : l_mapAgents)
		{
			boost::shared_ptr<CAgent> l_pAgent = l_agentObj.second;
			if (l_pAgent)
			{
				//if (CDeviceManager::Instance()->DeviceIsFree(l_pAgent->GetDeviceNum()))
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

	for (auto l_freeAgentObj : l_mapFreeAgent)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_freeAgentObj.second;
		if (l_pAgent)
		{
			if (!l_pAgent->IsReady())
			{
				continue;
			}
			if (!p_strACDNum.empty())
			{
				//	获取某一 ACD 的成员
				if (l_pAgent->GetACDGroup().compare(p_strACDNum) == 0)
				{
					p_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
						l_pAgent->GetACDGroup(),
						l_pAgent->GetAgentModeString(),
						l_pAgent->GetAgentReadyString(),
						l_pAgent->GetAgentStateTime());
				}
			}
			else
			{
				//	获取所有 ACD 的成员
				p_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
					l_pAgent->GetACDGroup(),
					l_pAgent->GetAgentModeString(),
					l_pAgent->GetAgentReadyString(),
					l_pAgent->GetAgentStateTime());
			}
		}
	}
	return true;
}


bool CAgentManager::GetAgent(IGetReadyAgentResultNotifPtr p_pResultNotif, const std::string p_strACDNum)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}	

	IGetReadyAgentResultNotifPtr l_pResultNotif = boost::dynamic_pointer_cast<CGetReadyAgentResultNotif>(p_pResultNotif);
	if (nullptr == l_pResultNotif)
	{
		return false;
	}
	for (auto l_AgentObj : l_mapAgents)
	{
		boost::shared_ptr<CAgent>l_pAgent = l_AgentObj.second;
		if (l_pAgent && l_pAgent->GetACDGroup().compare(p_strACDNum) == 0 && l_pAgent->IsReady())
		{
			//if (CDeviceManager::Instance()->DeviceIsFree(l_pAgent->GetDeviceNum()))
			{
				l_pResultNotif->SetResult(true);
				l_pResultNotif->SetReadyAgent(l_pAgent->GetDeviceNum());

				return true;
			}
		}
	}
	return false;

}

bool CAgentManager::GetACDList(IGetACDListResultNotifPtr p_pResultNotif)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	for (auto l_AgentObj : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_AgentObj.second;
		if (l_pAgent)
		{
			//	获取所有 ACD 的成员
			p_pResultNotif->AddAgentList(l_pAgent->GetDeviceNum(),
				l_pAgent->GetACDGroup(),
				l_pAgent->GetAgentStateTime());
		}
	}
	return true;
}

void CAgentManager::ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE l_agentMode, E_AGENT_READY_TYPE l_agentReadyType)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	std::string l_strCurrentTime = m_pDateTimePtr->CurrentDateTimeStr();
	for (auto l_agentObj : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_agentObj.second;
		if (l_pAgent)
		{
			if (l_pAgent->GetDeviceNum().compare(p_strDevice) == 0 /*&& l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0*/)
			{
				//  不同 ACD 组，同一 Agent 成员状态同时改变 
				l_pAgent->SetAgentState(l_agentMode, l_agentReadyType, l_strCurrentTime);

				std::string l_strACDGrp = l_pAgent->GetACDGroup();
				std::string l_strLoginMode = l_pAgent->GetAgentModeString();
				std::string l_strReadyState = l_pAgent->GetAgentReadyString();

				ICC_LOG_DEBUG(m_pLog, "ReadyStateSync, Agent: %s, ACD: %s, Mode: %s, ReadyState: %s",
					p_strDevice.c_str(), l_strACDGrp.c_str(), l_strLoginMode.c_str(), l_strReadyState.c_str());

				_PostAgentState(p_strDevice, l_strACDGrp, l_strLoginMode, l_strReadyState, l_strCurrentTime);
			}
		}
	}
}

int CAgentManager::GetReadyAgentCount(const std::string& p_strACDGrp)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	int l_nCount = 0;
	for (auto l_iter : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_iter.second;
		if (p_strACDGrp.empty())
		{
			if (l_pAgent && l_pAgent->IsReady())
			{
				l_nCount++;
			}
		}
		else
		{
			if (l_pAgent && l_pAgent->GetACDGroup().compare(p_strACDGrp) == 0 && l_pAgent->IsReady())
			{
				l_nCount++;
			}
		}
		
	}
	return l_nCount;
}

void CAgentManager::GetAgentState(const std::string& p_strInDeviceNum, std::string& p_strOutLodinMode, std::string& p_strOutReadyState)
{
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	for (auto l_iter : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgent = l_iter.second;
		if (l_pAgent && l_pAgent->GetDeviceNum().compare(p_strInDeviceNum) == 0)
		{
			p_strOutLodinMode = l_pAgent->GetAgentModeString();
			p_strOutReadyState = l_pAgent->GetAgentReadyString();

			break;
		}
	}
}

std::string CAgentManager::GetAgentACDGrpList(const std::string& p_strDeviceNum)
{
	std::string l_strAcdGrp;
	std::map<std::string, boost::shared_ptr<CAgent>> l_mapAgents;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		l_mapAgents = m_mapAgentTB;
	}

	for (auto l_iter : l_mapAgents)
	{
		boost::shared_ptr<CAgent> l_pAgentObj = l_iter.second;
		if (l_pAgentObj && l_pAgentObj->GetDeviceNum().compare(p_strDeviceNum) == 0)
		{
			if (!l_pAgentObj->GetACDGroup().empty())
			{
				if (l_strAcdGrp.empty())
				{
					l_strAcdGrp = l_pAgentObj->GetACDGroup();
				}
				else
				{
					l_strAcdGrp = l_strAcdGrp + ";" + l_pAgentObj->GetACDGroup();
				}
			}
		}
	}
	return l_strAcdGrp;
}

void CAgentManager::_PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
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

		std::string l_strOnlineCount = m_pStrUtil->Format("%d", GetReadyAgentCount(p_strACDGrp));
		l_pAgentNotif->SetCurrOnlineNum(l_strOnlineCount);

		std::string l_strMinOnlineNum = m_pStrUtil->Format("%d", CSysConfig::Instance()->GetMinOnlineAgentNum());
		l_pAgentNotif->SetMinOnlineNum(l_strMinOnlineNum);

		//m_pSwitchEventCallback->Event_AgentState(l_pAgentNotif);
		ICC_LOG_DEBUG(m_pLog, "report event AgentState,AgentId:[%s], ACD:[%s],LoginMode:[%s],ReadyState:[%s]",
			p_strDevice.c_str(), p_strACDGrp.c_str(), p_strLoginMode.c_str(), p_strReadyState.c_str());

		CFSAesSwitchManager::Instance()->DispatchCTIEvent("Event_AgentState", l_pAgentNotif);
	}
}


//////////////////////////////////////////////////////////////////////
//
void CAgentManager::_LoadAllAgent()
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

					{
						SAFE_LOCK(m_mapAgentTBMutex);
						std::string l_strKey = l_strAgentId + "_" + l_strACDGrp;
						m_mapAgentTB[l_strKey] = pAgent;
					}
					ICC_LOG_DEBUG(m_pLog, "AddAgent, ACDGrp:[%s] agentId:[%s], agentPswd:[%s],DeviceNum:[%s]",
						l_strACDGrp.c_str(), l_strAgentId.c_str(), l_strAgentPsw.c_str(), l_strDeviceNum.c_str());
				}
				else
				{
					ICC_LOG_WARNING(m_pLog, "not found in device list,DeviceNum:[%s]", l_strDeviceNum.c_str());
				}
			}// end agentList

		}
	}
}

void CAgentManager::_ClearAllAgent()
{
	m_bAgentIsLoad = false;
	{
		SAFE_LOCK(m_mapAgentTBMutex);
		m_mapAgentTB.clear();
	}
}