#include "TextTemplate.h"
#include <boost/format.hpp>
using namespace ICC;

CTextTemplate::CTextTemplate()
{

}

CTextTemplate::~CTextTemplate()
{

}

std::string CTextTemplate::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strUserInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/type", m_strType);
		p_pJson->SetNodeValue("/content", m_strContent);
		p_pJson->SetNodeValue("/alarm_type", m_strAlarm_type);
		p_pJson->SetNodeValue("/handly_type", m_strHandly_type);

		l_strUserInfo = p_pJson->ToString();
	}
	return l_strUserInfo;
}

bool CTextTemplate::Parse(std::string p_strUserInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strUserInfo))
	{
		return false;
	}
	
	m_strGuid		= p_pJson->GetNodeValue("/guid", "");
	m_strType = p_pJson->GetNodeValue("/type", "");
	m_strContent = p_pJson->GetNodeValue("/content", "");
	m_strAlarm_type = p_pJson->GetNodeValue("/alarm_type", "");
	m_strHandly_type = p_pJson->GetNodeValue("/handly_type", "");
	
	return true;
}
