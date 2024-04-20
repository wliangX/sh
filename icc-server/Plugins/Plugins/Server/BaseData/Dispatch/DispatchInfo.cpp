#include "DispatchInfo.h"

using namespace ICC;

ICC::CDispatchInfo::CDispatchInfo()
{

}

ICC::CDispatchInfo::~CDispatchInfo()
{

}

std::string CDispatchInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strFuncInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
		p_pJson->SetNodeValue("/code", m_strCode);
		p_pJson->SetNodeValue("/district_code", m_strDistrictCode);
		p_pJson->SetNodeValue("/type", m_strType);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/phone", m_strPhone);

		l_strFuncInfo = p_pJson->ToString();
	}
	return l_strFuncInfo;
}

bool CDispatchInfo::Parse(std::string p_strDispatchInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strDispatchInfo))
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
	return true;
}

ICC::CCanDispatchInfo::CCanDispatchInfo()
{

}

ICC::CCanDispatchInfo::~CCanDispatchInfo()
{

}

std::string ICC::CCanDispatchInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strFuncInfo("");
	if (p_pJson)
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
		p_pJson->SetNodeValue("/bind_dept_code", m_strBindDeptCode);
		p_pJson->SetNodeValue("/bind_parent_dept_code", m_strBindParentDeptCode);
		p_pJson->SetNodeValue("/belong_dept_code", m_BelongDeptCode);
		p_pJson->SetNodeValue("/create_user", m_strCreateUser);
		p_pJson->SetNodeValue("/create_time", m_strCreateTime);
		p_pJson->SetNodeValue("/update_user", m_strUpdateUser);
		p_pJson->SetNodeValue("/update_time", m_strUpdateTime);

		l_strFuncInfo = p_pJson->ToString();
	}
	return l_strFuncInfo;
}

bool ICC::CCanDispatchInfo::Parse(std::string p_strCanDispatchInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strCanDispatchInfo))
	{
		return false;
	}
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
	m_strBindDeptCode = p_pJson->GetNodeValue("/bind_dept_code", "");
	m_strBindParentDeptCode = p_pJson->GetNodeValue("/bind_parent_dept_code", "");
	m_BelongDeptCode = p_pJson->GetNodeValue("/belong_dept_code", "");
	m_strCreateUser = p_pJson->GetNodeValue("/create_user", "");
	m_strCreateTime = p_pJson->GetNodeValue("/create_time", "");
	m_strUpdateUser = p_pJson->GetNodeValue("/update_user", "");
	m_strUpdateTime = p_pJson->GetNodeValue("/update_time", "");
	return true;
}
