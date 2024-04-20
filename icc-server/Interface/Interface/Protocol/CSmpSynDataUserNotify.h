#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDataUserNotify : public IReceive
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
				m_dataUser.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataUser.m_strStaffGuid = p_pJson->GetNodeValue(l_strPrefixPath + "staffGuid", "");
				m_dataUser.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
				m_dataUser.m_strUserName = p_pJson->GetNodeValue(l_strPrefixPath + "userName", "");
				m_dataUser.m_strUserPassword = p_pJson->GetNodeValue(l_strPrefixPath + "userPassword", "");
				m_dataUser.m_strUserType = p_pJson->GetNodeValue(l_strPrefixPath + "userType", "");
				m_dataUser.m_strUserIcon = p_pJson->GetNodeValue(l_strPrefixPath + "userIcon", "");
				m_dataUser.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
				m_dataUser.m_strLoginTime = p_pJson->GetNodeValue(l_strPrefixPath + "loginTime", "");
				m_dataUser.m_strOnlineStatus = p_pJson->GetNodeValue(l_strPrefixPath + "onlineStatus", "");
				m_dataUser.m_strPasswordErrorNum = p_pJson->GetNodeValue(l_strPrefixPath + "passwordErrorNum", "");
				m_dataUser.m_strLockStatus = p_pJson->GetNodeValue(l_strPrefixPath + "lockStatus", "");
				m_dataUser.m_strLockTime = p_pJson->GetNodeValue(l_strPrefixPath + "lockTime", "");
				m_dataUser.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataUser.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataUser.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataUser.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataUser.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataUser.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataUser.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
									
				
				return true;
			}

		public:			
			SmpDataUser m_dataUser;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
