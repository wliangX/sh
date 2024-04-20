#include "PhoneInfo.h"
#include <boost/format.hpp>
using namespace ICC;

CPhoneInfo::CPhoneInfo()
{
	m_strPhone = "";
	m_strName = "";
	m_strAddress = "";
}

CPhoneInfo::~CPhoneInfo()
{

}

std::string CPhoneInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strUserInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/phone", m_strPhone);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/address", m_strAddress);

		l_strUserInfo = p_pJson->ToString();
	}
	return l_strUserInfo;
}

bool CPhoneInfo::Parse(std::string p_strUserInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strUserInfo))
	{
		return false;
	}
	m_strPhone = p_pJson->GetNodeValue("/phone", "");
	m_strName = p_pJson->GetNodeValue("/name", "");
	m_strAddress = p_pJson->GetNodeValue("/address", "");
	
	return true;
}
