#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataStaffNotify : public IReceive
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
				m_dataStaff.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataStaff.m_strStaffCode = p_pJson->GetNodeValue(l_strPrefixPath + "staffCode", "");
				m_dataStaff.m_strStaffIdNo = p_pJson->GetNodeValue(l_strPrefixPath + "staffIdNo", "");
				m_dataStaff.m_strStaffName = p_pJson->GetNodeValue(l_strPrefixPath + "staffName", "");
				m_dataStaff.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
				m_dataStaff.m_strPucOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "pucOrgIdentifier", "");
				m_dataStaff.m_strStaffSex = p_pJson->GetNodeValue(l_strPrefixPath + "staffSex", "");
				m_dataStaff.m_strStaffType = p_pJson->GetNodeValue(l_strPrefixPath + "staffType", "");
				m_dataStaff.m_strStaffStatus = p_pJson->GetNodeValue(l_strPrefixPath + "staffStatus", "");
				m_dataStaff.m_strStaffCategory = p_pJson->GetNodeValue(l_strPrefixPath + "staffCategory", "");
				m_dataStaff.m_strTelephone = p_pJson->GetNodeValue(l_strPrefixPath + "telephone", "");
				m_dataStaff.m_strStaffMobile = p_pJson->GetNodeValue(l_strPrefixPath + "staffMobile", "");
				m_dataStaff.m_strStaffAddress = p_pJson->GetNodeValue(l_strPrefixPath + "staffAddress", "");
				m_dataStaff.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
				m_dataStaff.m_strIsLeader = p_pJson->GetNodeValue(l_strPrefixPath + "isLeader", "");
				m_dataStaff.m_strStaffPosition = p_pJson->GetNodeValue(l_strPrefixPath + "staffPosition", "");
				m_dataStaff.m_strSystemNo = p_pJson->GetNodeValue(l_strPrefixPath + "systemNo", "");
				m_dataStaff.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
				m_dataStaff.m_strPucId = p_pJson->GetNodeValue(l_strPrefixPath + "pucId", "");
				m_dataStaff.m_strPucSystemId = p_pJson->GetNodeValue(l_strPrefixPath + "pucSystemId", "");
				m_dataStaff.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataStaff.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataStaff.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataStaff.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataStaff.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataStaff.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataStaff.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
				
				return true;
			}

		public:			
			SmpDataStaff m_dataStaff;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
