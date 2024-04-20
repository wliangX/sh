#include "AdminInfo.h"

using namespace ICC;

CAdminInfo::CAdminInfo()
{
}


CAdminInfo::~CAdminInfo()
{
}

std::string CAdminInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson)
	{
		return "";
	}
	p_pJson->SetNodeValue("/guid", m_strGuid);
	p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
	p_pJson->SetNodeValue("/bind_dept_code", m_strBindDeptCode);
	p_pJson->SetNodeValue("/bind_parent_dept_code", m_strBindParentDeptCode);
	p_pJson->SetNodeValue("/belong_dept_code", m_strBelongDeptCode);
	return p_pJson->ToString();
}

bool CAdminInfo::Parse(std::string p_strAdminInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strAdminInfo))
	{
		return false;
	}
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
	m_strBindDeptCode = p_pJson->GetNodeValue("/bind_dept_code", "");
	m_strBindParentDeptCode = p_pJson->GetNodeValue("/bind_parent_dept_code", "");
	m_strBelongDeptCode = p_pJson->GetNodeValue("/belong_dept_code", "");
	return true;
}
