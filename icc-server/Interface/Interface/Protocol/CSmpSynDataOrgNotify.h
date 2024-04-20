#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataOrgNotify : public IReceive
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
				m_dataOrg.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
				m_dataOrg.m_strParentGuid = p_pJson->GetNodeValue(l_strPrefixPath + "parentGuid", "");
				m_dataOrg.m_strOrgGovCode = p_pJson->GetNodeValue(l_strPrefixPath + "orgGovCode", "");
				m_dataOrg.m_strPucOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "pucOrgIdentifier", "");
				m_dataOrg.m_strOrgFullName = p_pJson->GetNodeValue(l_strPrefixPath + "orgFullName", "");
				m_dataOrg.m_strOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "orgName", "");
				m_dataOrg.m_strOrgShortName = p_pJson->GetNodeValue(l_strPrefixPath + "orgShortName", "");
				m_dataOrg.m_strOrgSimpleName = p_pJson->GetNodeValue(l_strPrefixPath + "orgSimpleName", "");
				m_dataOrg.m_strOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "orgType", "");
				m_dataOrg.m_strOrgBusinessType = p_pJson->GetNodeValue(l_strPrefixPath + "orgBusinessType", "");
				m_dataOrg.m_strOrgRegionType = p_pJson->GetNodeValue(l_strPrefixPath + "orgRegionType", "");
				m_dataOrg.m_strLongitude = p_pJson->GetNodeValue(l_strPrefixPath + "longitude", "");
				m_dataOrg.m_strLatitude = p_pJson->GetNodeValue(l_strPrefixPath + "latitude", "");
				m_dataOrg.m_strOrgContact = p_pJson->GetNodeValue(l_strPrefixPath + "orgContact", "");
				m_dataOrg.m_strOrgContactNo = p_pJson->GetNodeValue(l_strPrefixPath + "orgContactNo", "");
				m_dataOrg.m_strOrgFax = p_pJson->GetNodeValue(l_strPrefixPath + "orgFax", "");
				m_dataOrg.m_strOrgSegmentContent = p_pJson->GetNodeValue(l_strPrefixPath + "orgSegmentContent", "");
				m_dataOrg.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
				m_dataOrg.m_strIcon = p_pJson->GetNodeValue(l_strPrefixPath + "icon", "");
				m_dataOrg.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
				m_dataOrg.m_strPucId = p_pJson->GetNodeValue(l_strPrefixPath + "pucId", "");
				m_dataOrg.m_strPudSystemId = p_pJson->GetNodeValue(l_strPrefixPath + "pucSystemId", "");
				m_dataOrg.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
				m_dataOrg.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
				m_dataOrg.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
				m_dataOrg.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
				m_dataOrg.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
				m_dataOrg.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
				m_dataOrg.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
				
				return true;
			}

		public:			
			SmpDataOrg m_dataOrg;
			std::string m_strCode;
			std::string m_strMessage;
		};
	}
}
