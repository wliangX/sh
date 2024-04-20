#include "RoleInfo.h"

using namespace ICC;

CRoleInfo::CRoleInfo()
{
}


CRoleInfo::~CRoleInfo()
{
}

std::string CRoleInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{	
	if (nullptr == p_pJson)
	{
		return "";
	}

	p_pJson->SetNodeValue("/guid", m_strGuid);
	p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
	p_pJson->SetNodeValue("/code", m_strCode);
	p_pJson->SetNodeValue("/name", m_strName);
	p_pJson->SetNodeValue("/sort", m_strSort);
	return p_pJson->ToString();
}

bool CRoleInfo::Parse(std::string p_strRoleInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strRoleInfo))
	{
		return false;
	}
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
	m_strCode = p_pJson->GetNodeValue("/code", "");
	m_strName = p_pJson->GetNodeValue("/name", "");
	m_strSort = p_pJson->GetNodeValue("/sort", "");
	return true;
}
