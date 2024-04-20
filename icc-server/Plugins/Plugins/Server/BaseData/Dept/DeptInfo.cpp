#include "DeptInfo.h"

using namespace ICC;

CDeptInfo::CDeptInfo()
{
}


CDeptInfo::~CDeptInfo()
{
}

std::string CDeptInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson)
	{
		return "";
	}
	p_pJson->SetNodeValue("/guid", m_strGuid);
	p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
	p_pJson->SetNodeValue("/code", m_strCode);
	p_pJson->SetNodeValue("/district_code", m_strDistrictCode);
	p_pJson->SetNodeValue("/type", m_strType);
	p_pJson->SetNodeValue("/name", m_strName);
	p_pJson->SetNodeValue("/phone", m_strPhone);
	p_pJson->SetNodeValue("/level", m_strLevel);
	p_pJson->SetNodeValue("/shortcut", m_strShortcut);
	p_pJson->SetNodeValue("/sort", m_strSort);
	return p_pJson->ToString();
}

bool CDeptInfo::Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strDeptInfo))
	{
		return false;
	}
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
	m_strCode = p_pJson->GetNodeValue("/code", "");
	m_strDistrictCode = p_pJson->GetNodeValue("/district_code", "");
	m_strType = p_pJson->GetNodeValue("/type", "");
	m_strName = p_pJson->GetNodeValue("/name", "");
	m_strPhone = p_pJson->GetNodeValue("/phone", "");
	m_strLevel = p_pJson->GetNodeValue("/level", "");
	m_strShortcut = p_pJson->GetNodeValue("/shortcut", "");
	m_strSort = p_pJson->GetNodeValue("/sort", "");
	return true;
}
