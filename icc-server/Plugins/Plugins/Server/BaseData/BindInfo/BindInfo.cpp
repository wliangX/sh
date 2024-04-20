#include "BindInfo.h"

using namespace ICC;

CBindInfo::CBindInfo()
{
}


CBindInfo::~CBindInfo()
{
}

std::string CBindInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson)
	{
		return "";
	}
	p_pJson->SetNodeValue("/guid", m_strGuid);
	p_pJson->SetNodeValue("/type", m_strType);
	p_pJson->SetNodeValue("/from_guid", m_strFromGuid);
	p_pJson->SetNodeValue("/to_guid", m_strToGuid);
	p_pJson->SetNodeValue("/flag", m_strFlag);
	p_pJson->SetNodeValue("/level", m_strLevel);
	p_pJson->SetNodeValue("/sort", m_strSort);
	p_pJson->SetNodeValue("/short_code", m_strShortcut);
	p_pJson->SetNodeValue("/create_user", m_strCreateUser);
	p_pJson->SetNodeValue("/create_time", m_strCreateTime);
	p_pJson->SetNodeValue("/update_user", m_strUpdateUser);
	p_pJson->SetNodeValue("/update_time", m_strUpdatetime);
	p_pJson->SetNodeValue("/result", m_strResult);
	p_pJson->SetNodeValue("/sync_type", m_strSyncType);	
	return p_pJson->ToString();
}

bool CBindInfo::Parse(std::string p_strBindInfo, JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strBindInfo))
	{
		return false;
	}
	m_strGuid = p_pJson->GetNodeValue("/guid", "");
	m_strType = p_pJson->GetNodeValue("/type", "");
	m_strFromGuid = p_pJson->GetNodeValue("/from_guid", "");
	m_strToGuid = p_pJson->GetNodeValue("/to_guid", "");
	m_strFlag = p_pJson->GetNodeValue("/flag", "");
	m_strLevel = p_pJson->GetNodeValue("/level", "");
	m_strSort = p_pJson->GetNodeValue("/sort", "");
	m_strShortcut = p_pJson->GetNodeValue("/short_code", "");
	m_strCreateUser = p_pJson->GetNodeValue("/create_user", "");
	m_strCreateTime = p_pJson->GetNodeValue("/create_time", "");
	m_strUpdateUser = p_pJson->GetNodeValue("/update_user", "");
	m_strUpdatetime = p_pJson->GetNodeValue("/update_time", "");
	m_strResult = p_pJson->GetNodeValue("/result", "");
	m_strSyncType = p_pJson->GetNodeValue("/sync_type", "");
	return true;
}
