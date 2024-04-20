#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataRole : public IReceive
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
				
				m_strCurrentPackageCount = p_pJson->GetNodeValue("/currentPackage", "");
				m_strPackageSize = p_pJson->GetNodeValue("/step", "");

				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; i++)
				{
					SmpDataRole role;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					role.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					role.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
					role.m_strRoleName = p_pJson->GetNodeValue(l_strPrefixPath + "roleName", "");
					role.m_strRoleCode = p_pJson->GetNodeValue(l_strPrefixPath + "roleCode", "");
					role.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
					role.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
					role.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					role.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					role.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					role.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					role.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
					role.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
									
					m_vecRoles.push_back(role);
				}
				return true;
			}

		public:			
			std::vector<SmpDataRole> m_vecRoles;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
