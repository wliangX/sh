#include "CConfigInfo.h"
#include <boost/format.hpp>
using namespace ICC;

CConfigInfo::CConfigInfo()
{

}

CConfigInfo::~CConfigInfo()
{

}

std::string CConfigInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strConfigInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/ip", m_strIp);
		p_pJson->SetNodeValue("/config", m_strConfig);

		l_strConfigInfo = p_pJson->ToString();
	}
	return l_strConfigInfo;
}

bool CConfigInfo::Parse(std::string p_strAlarmOnlineList,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strAlarmOnlineList))
	{
		return false;
	}
	
	m_strIp = p_pJson->GetNodeValue("/ip", "");
	m_strConfig = p_pJson->GetNodeValue("/config", "");
	
	return true;
}
