#include "DictInfo.h"

using namespace ICC;

CDictInfo::CDictInfo()
{
}


CDictInfo::~CDictInfo()
{
}

std::string CDictInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson)
	{
		return "";
	}
    p_pJson->SetNodeValue("/guid", m_strGuid);
    p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
    p_pJson->SetNodeValue("/code", m_strCode);
    p_pJson->SetNodeValue("/shortcut", m_strShortCut);
    p_pJson->SetNodeValue("/sort", m_strSort);

    for (size_t i = 0; i < m_vecValue.size(); i++)
    {
        std::string l_strPath = "/value/";
        std::string l_strNum = std::to_string(i);
        p_pJson->SetNodeValue(l_strPath + l_strNum + "/guid", m_vecValue.at(i).m_strGuid);
        p_pJson->SetNodeValue(l_strPath + l_strNum + "/value", m_vecValue.at(i).m_strValue);
        p_pJson->SetNodeValue(l_strPath + l_strNum + "/lang_guid", m_vecValue.at(i).m_strLangGuid);
    }
	return p_pJson->ToString();
}

bool CDictInfo::Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strDeptInfo))
	{
		return false;
	}
    m_strGuid = p_pJson->GetNodeValue("/guid", "");
    m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
    m_strCode = p_pJson->GetNodeValue("/code", "");
    m_strShortCut = p_pJson->GetNodeValue("/shortcut", "");
    m_strSort = p_pJson->GetNodeValue("/sort", "");

    int l_iCount = p_pJson->GetCount("/value");
    for (int i = 0; i < l_iCount; i++)
    {
        std::string l_strValueNum = std::to_string(i);
        CValue l_CValue;
        l_CValue.m_strValue = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/value", "");
        l_CValue.m_strGuid = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/guid", "");
        l_CValue.m_strLangGuid = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/lang_guid", "");
        m_vecValue.push_back(l_CValue);
    }    
    return true;
}