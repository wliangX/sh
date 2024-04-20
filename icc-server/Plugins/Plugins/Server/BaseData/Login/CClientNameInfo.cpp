#include "CClientNameInfo.h"
#include <boost/format.hpp>
using namespace ICC;

CClientNameInfo::CClientNameInfo()
{

}

CClientNameInfo::~CClientNameInfo()
{

}

std::string CClientNameInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strClientNameInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/client_name", m_strClientName);

		l_strClientNameInfo = p_pJson->ToString();
	}
	return l_strClientNameInfo;
}

bool CClientNameInfo::Parse(std::string p_strClientNameInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strClientNameInfo))
	{
		return false;
	}
	
	m_strClientName = p_pJson->GetNodeValue("/client_name", "");
	
	return true;
}
