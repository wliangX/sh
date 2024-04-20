#include <AgentIPMap.h>

CAgentIPTable CAgentIPTable::m_instance;

void CAgentIPTable::AddAgentPhoneIP(const std::string& strPhoneNumber, const std::string& strPhoneIP)
{
	m_mapAgentIP[strPhoneNumber] = strPhoneIP;
	m_mapIPAgent[strPhoneIP] = strPhoneNumber;
}
void CAgentIPTable::RmvAgentPhoneIP(const std::string& strPhoneNumber)
{
	if (m_mapAgentIP.find(strPhoneNumber) == m_mapAgentIP.end())
	{
		return;
	}
	std::string strPhoneIP = m_mapAgentIP[strPhoneNumber];
	m_mapAgentIP.erase(strPhoneNumber);
	m_mapIPAgent.erase(strPhoneIP);

}
void CAgentIPTable::RmvPhoneIPAgent(const std::string& strPhoneIP)
{
	if (m_mapIPAgent.find(strPhoneIP) == m_mapIPAgent.end())
	{
		return;
	}
	std::string strPhoneNumber = m_mapIPAgent[strPhoneIP];
	m_mapIPAgent.erase(strPhoneIP);
	m_mapAgentIP.erase(strPhoneNumber);
}
std::string CAgentIPTable::QueryIPByPhone(const std::string& strPhoneNumber)
{
	if (m_mapAgentIP.find(strPhoneNumber) == m_mapAgentIP.end())
	{
		return "";
	}
	return m_mapAgentIP[strPhoneNumber];
}
std::string CAgentIPTable::QueryPhoneByIP(const std::string& strIPAddr)
{
	if (m_mapIPAgent.find(strIPAddr) == m_mapIPAgent.end())
	{
		return "";
	}
	return m_mapIPAgent[strIPAddr];
}
void CAgentIPTable::ClearAgentPhoneIP()
{
	m_mapAgentIP.clear();
	m_mapIPAgent.clear();
}