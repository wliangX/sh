#include "UserInfo.h"
#include <boost/format.hpp>
using namespace ICC;

std::string CUserInfo::ToJson(JsonParser::IJsonPtr p_pJson)
{
	std::string l_strUserInfo("");
	if (p_pJson)	
	{
		p_pJson->SetNodeValue("/guid", m_strGuid);
		p_pJson->SetNodeValue("/code", m_strCode);
		p_pJson->SetNodeValue("/name", m_strName);
		p_pJson->SetNodeValue("/pwd", m_strPwd);
        p_pJson->SetNodeValue("/remark", m_strRemark);
		p_pJson->SetNodeValue("/is_delete", m_strIsDelete);
		p_pJson->SetNodeValue("/level", m_strLevel);
		p_pJson->SetNodeValue("/sort", m_strSort);
		p_pJson->SetNodeValue("/shortcut", m_strShortCut);
		p_pJson->SetNodeValue("/create_user", m_strCreateUser);
		p_pJson->SetNodeValue("/create_time", m_strCreateTime);
		p_pJson->SetNodeValue("/update_user", m_strUpdateUser);
		p_pJson->SetNodeValue("/update_time", m_strUpdateTime);
		l_strUserInfo = p_pJson->ToString();
	}
	return l_strUserInfo;
}

bool CUserInfo::Parse(std::string p_strUserInfo,JsonParser::IJsonPtr p_pJson)
{
	if (!p_pJson || !p_pJson->LoadJson(p_strUserInfo))
	{
		return false;
	}	
	m_strGuid		= p_pJson->GetNodeValue("/guid", "");
	m_strCode		= p_pJson->GetNodeValue("/code", "");
	m_strName		= p_pJson->GetNodeValue("/name", "");
	m_strPwd		= p_pJson->GetNodeValue("/pwd", "");
    m_strRemark     = p_pJson->GetNodeValue("/remark", "");
	m_strIsDelete	= p_pJson->GetNodeValue("/is_delete", "");
	m_strLevel		= p_pJson->GetNodeValue("/level", "");
	m_strSort		= p_pJson->GetNodeValue("/sort", "");
	m_strShortCut	= p_pJson->GetNodeValue("/shortcut", "");
	m_strCreateUser	= p_pJson->GetNodeValue("/create_user", "");
	m_strCreateTime	= p_pJson->GetNodeValue("/create_time", "");
	m_strUpdateUser	= p_pJson->GetNodeValue("/update_user", "");
	m_strUpdateTime	= p_pJson->GetNodeValue("/update_time", "");	
	return true;
}
