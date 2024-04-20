#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataRoleNotify : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{		
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");				
				
				std::string l_strPrefixPath("/data/");
				m_dataRole.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataRole.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
				m_dataRole.m_strRoleName = p_pJson->GetNodeValue(l_strPrefixPath + "roleName", "");
				m_dataRole.m_strRoleCode = p_pJson->GetNodeValue(l_strPrefixPath + "roleCode", "");
				m_dataRole.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
				m_dataRole.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataRole.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataRole.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataRole.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataRole.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataRole.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataRole.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
				
				return true;
			}

		public:			
			SmpDataRole m_dataRole;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
