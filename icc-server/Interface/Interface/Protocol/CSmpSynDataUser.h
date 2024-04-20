#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataUser : public IReceive
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
					SmpDataUser user;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					user.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					user.m_strStaffGuid = p_pJson->GetNodeValue(l_strPrefixPath + "staffGuid", "");
					user.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
					user.m_strUserName = p_pJson->GetNodeValue(l_strPrefixPath + "userName", "");
					user.m_strUserPassword = p_pJson->GetNodeValue(l_strPrefixPath + "userPassword", "");
					user.m_strUserType = p_pJson->GetNodeValue(l_strPrefixPath + "userType", "");
					user.m_strUserIcon = p_pJson->GetNodeValue(l_strPrefixPath + "userIcon", "");
					user.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
					user.m_strLoginTime = p_pJson->GetNodeValue(l_strPrefixPath + "loginTime", "");
					user.m_strOnlineStatus = p_pJson->GetNodeValue(l_strPrefixPath + "onlineStatus", "");
					user.m_strPasswordErrorNum = p_pJson->GetNodeValue(l_strPrefixPath + "passwordErrorNum", "");
					user.m_strLockStatus = p_pJson->GetNodeValue(l_strPrefixPath + "lockStatus", "");
					user.m_strLockTime = p_pJson->GetNodeValue(l_strPrefixPath + "lockTime", "");
					user.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
					user.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					user.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					user.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					user.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					user.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");					
					user.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
									
					m_vecUsers.push_back(user);
				}
				return true;
			}

		public:			
			std::vector<SmpDataUser> m_vecUsers;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
