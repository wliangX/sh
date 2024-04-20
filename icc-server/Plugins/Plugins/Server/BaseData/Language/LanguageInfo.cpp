#include "LanguageInfo.h"

using namespace ICC;

CLanguageInfo::CLanguageInfo()
{
}


CLanguageInfo::~CLanguageInfo()
{
}

std::string CLanguageInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson)
	{
		return "";
	}
    p_pJson->SetNodeValue("/guid",m_strGuid);
    p_pJson->SetNodeValue("/code",m_strCode);
    p_pJson->SetNodeValue("/name",m_strName);
    p_pJson->SetNodeValue("/sort",m_strSort);
	return p_pJson->ToString();
}

bool CLanguageInfo::Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strDeptInfo))
	{
		return false;
	}
    m_strGuid = p_pJson->GetNodeValue("/guid", "");
    m_strCode = p_pJson->GetNodeValue("/code", "");
    m_strName = p_pJson->GetNodeValue("/name", "");
    m_strSort = p_pJson->GetNodeValue("/sort", "");
	return true;
}
