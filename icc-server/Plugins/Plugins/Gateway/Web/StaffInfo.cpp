#include "StaffInfo.h"


ICC::CStaffInfo::CStaffInfo()
{

}

ICC::CStaffInfo::~CStaffInfo()
{

}

std::string ICC::CStaffInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (p_pJson)
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/code", m_strCode);
		p_pJson->SetNodeValue("/dept_guid", m_strDeptGuid);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/sex", m_strSex);
		p_pJson->SetNodeValue("/type", m_strType);
		p_pJson->SetNodeValue("/position", m_strPosition);
		p_pJson->SetNodeValue("/mobile", m_strMobile);
		p_pJson->SetNodeValue("/phone", m_strPhone);
		p_pJson->SetNodeValue("/level", m_strLevel);
		p_pJson->SetNodeValue("/sort", m_strSort);
		p_pJson->SetNodeValue("/shortcut", m_strShortcut);
		p_pJson->SetNodeValue("/isleader", m_strIsLeader);
		return p_pJson->ToString();
	}
	return "";
}

bool ICC::CStaffInfo::Parse(std::string p_strStaffInfo, JsonParser::IJsonPtr p_pJson)
{
	if (p_pJson && p_pJson->LoadJson(p_strStaffInfo))
	{
		m_strGuid = p_pJson->GetNodeValue("/guid", "");
		m_strDeptGuid = p_pJson->GetNodeValue("/dept_guid", "");
		m_strCode = p_pJson->GetNodeValue("/code", "");
		m_strName = p_pJson->GetNodeValue("/name", "");
		m_strSex = p_pJson->GetNodeValue("/sex", "");
		m_strType = p_pJson->GetNodeValue("/type", "");
		m_strPosition = p_pJson->GetNodeValue("/position", "");
		m_strMobile = p_pJson->GetNodeValue("/mobile", "");
		m_strPhone = p_pJson->GetNodeValue("/phone", "");
		m_strLevel = p_pJson->GetNodeValue("/level", "");
		m_strSort = p_pJson->GetNodeValue("/sort", "");
		m_strShortcut = p_pJson->GetNodeValue("/shortcut", "");
		m_strIsLeader = p_pJson->GetNodeValue("/isleader", "");

		return true;
	}
	return false;
}

