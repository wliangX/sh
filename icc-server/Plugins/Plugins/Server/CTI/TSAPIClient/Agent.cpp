#include "Boost.h"

#include "Agent.h"


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

