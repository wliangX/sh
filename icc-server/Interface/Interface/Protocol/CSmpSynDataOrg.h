#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataOrg : public IReceive
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
					SmpDataOrg org;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					org.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					org.m_strParentGuid = p_pJson->GetNodeValue(l_strPrefixPath + "parentGuid", "");
					org.m_strOrgGovCode = p_pJson->GetNodeValue(l_strPrefixPath + "orgGovCode", "");
					org.m_strPucOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "pucOrgIdentifier", "");
					org.m_strOrgFullName = p_pJson->GetNodeValue(l_strPrefixPath + "orgFullName", "");
					org.m_strOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "orgName", "");
					org.m_strOrgShortName = p_pJson->GetNodeValue(l_strPrefixPath + "orgShortName", "");
					org.m_strOrgSimpleName = p_pJson->GetNodeValue(l_strPrefixPath + "orgSimpleName", "");
					org.m_strOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "orgType", "");
					org.m_strOrgBusinessType = p_pJson->GetNodeValue(l_strPrefixPath + "orgBusinessType", "");
					org.m_strOrgRegionType = p_pJson->GetNodeValue(l_strPrefixPath + "districtCode", "");
					org.m_strLongitude = p_pJson->GetNodeValue(l_strPrefixPath + "longitude", "");
					org.m_strLatitude = p_pJson->GetNodeValue(l_strPrefixPath + "latitude", "");
					org.m_strOrgContact = p_pJson->GetNodeValue(l_strPrefixPath + "orgContact", "");
					org.m_strOrgContactNo = p_pJson->GetNodeValue(l_strPrefixPath + "orgContactNo", "");
					org.m_strOrgFax = p_pJson->GetNodeValue(l_strPrefixPath + "orgFax", "");
					org.m_strOrgSegmentContent = p_pJson->GetNodeValue(l_strPrefixPath + "orgSegmentContent", "");
					org.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
					org.m_strIcon = p_pJson->GetNodeValue(l_strPrefixPath + "icon", "");
					org.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
					org.m_strPucId = p_pJson->GetNodeValue(l_strPrefixPath + "pucId", "");
					org.m_strPudSystemId = p_pJson->GetNodeValue(l_strPrefixPath + "pucSystemId", "");
					org.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
					org.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					org.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					org.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					org.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					org.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
					org.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
					org.m_strDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "districtCode", "");
					m_vecOrgs.push_back(org);
				}
				return true;
			}

		public:			
			std::vector<SmpDataOrg> m_vecOrgs;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
