#pragma once
#include <CommonFunc.h>
class CAgentIPTable
{
public:
	static CAgentIPTable& GetInstance()
	{
		return m_instance;
	}

	void AddAgentPhoneIP(const std::string& strPhoneNumber, const std::string& strPhoneIP);
	void RmvAgentPhoneIP(const std::string& strPhoneNumber);
	void RmvPhoneIPAgent(const std::string& strPhoneNumber);
	std::string QueryIPByPhone(const std::string& strPhoneNumber);
	std::string QueryPhoneByIP(const std::string& strIPAddr);
	void ClearAgentPhoneIP();
private:
	std::map<std::string, std::string> m_mapAgentIP;
	std::map<std::string, std::string> m_mapIPAgent;
	static CAgentIPTable m_instance;
};