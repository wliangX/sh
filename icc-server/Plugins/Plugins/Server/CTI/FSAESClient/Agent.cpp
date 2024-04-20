#include "Boost.h"

#include "Agent.h"


CAgent::CAgent(std::string p_strACDGroup,std::string p_strAgent, std::string p_strPsw, std::string p_strDeviceNum)
{
	m_bDefaultReady = false;
	m_strACDGroup = p_strACDGroup;
	m_strAgent = p_strAgent;
	m_strPsw = p_strPsw;
	m_strDeviceNum = p_strDeviceNum;

	m_agentMode = AGENT_MODE_LOGOUT;
	m_agentReady = AGENT_NOTREADY;
}

CAgent::~CAgent()
{
	//
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

