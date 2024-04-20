#include "CMQClientInfo.h"
#include <boost/format.hpp>
using namespace ICC;

CMQClientInfo::CMQClientInfo()
{

}

CMQClientInfo::~CMQClientInfo()
{

}

std::string CMQClientInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strRegisterInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/client_id", m_strClientID);
		p_pJson->SetNodeValue("/client_ip", m_strClientIP);
		p_pJson->SetNodeValue("/connection_id", m_strConnectionID);

		l_strRegisterInfo = p_pJson->ToString();
	}
	return l_strRegisterInfo;
}

bool CMQClientInfo::Parse(std::string p_strRegisterInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strRegisterInfo))
	{
		return false;
	}
	
	m_strClientID = p_pJson->GetNodeValue("/client_id", "");
	m_strClientIP = p_pJson->GetNodeValue("/client_ip", "");
	m_strConnectionID = p_pJson->GetNodeValue("/connection_id", "");
	
	return true;
}
