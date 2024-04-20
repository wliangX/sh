#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CSmpSynDataDef.h>

namespace ICC
{
	namespace PROTOCOL
	{
		

		class CSmpSynDataStaff : public IReceive
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
					SmpDataStaff staff;
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					staff.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "guid", "");
					staff.m_strStaffCode = p_pJson->GetNodeValue(l_strPrefixPath + "staffCode", "");
					staff.m_strStaffIdNo = p_pJson->GetNodeValue(l_strPrefixPath + "staffIdNo", "");
					staff.m_strStaffName = p_pJson->GetNodeValue(l_strPrefixPath + "staffName", "");
					staff.m_strBelongOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "belongOrgGuid", "");
					staff.m_strPucOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "pucOrgIdentifier", "");
					staff.m_strStaffSex = p_pJson->GetNodeValue(l_strPrefixPath + "staffSex", "");
					staff.m_strStaffType = p_pJson->GetNodeValue(l_strPrefixPath + "staffType", "");
					staff.m_strStaffStatus = p_pJson->GetNodeValue(l_strPrefixPath + "staffStatus", "");
					staff.m_strStaffCategory = p_pJson->GetNodeValue(l_strPrefixPath + "staffCategory", "");
					staff.m_strTelephone = p_pJson->GetNodeValue(l_strPrefixPath + "telephone", "");
					staff.m_strStaffMobile = p_pJson->GetNodeValue(l_strPrefixPath + "staffMobile", "");
					staff.m_strStaffAddress = p_pJson->GetNodeValue(l_strPrefixPath + "staffAddress", "");
					staff.m_strRemark = p_pJson->GetNodeValue(l_strPrefixPath + "remark", "");
					staff.m_strIsLeader = p_pJson->GetNodeValue(l_strPrefixPath + "isLeader", "");
					staff.m_strStaffPosition = p_pJson->GetNodeValue(l_strPrefixPath + "staffPosition", "");
					staff.m_strSystemNo = p_pJson->GetNodeValue(l_strPrefixPath + "systemNo", "");
					staff.m_strSort = p_pJson->GetNodeValue(l_strPrefixPath + "sort", "");
					staff.m_strPucId = p_pJson->GetNodeValue(l_strPrefixPath + "pucId", "");
					staff.m_strPucSystemId = p_pJson->GetNodeValue(l_strPrefixPath + "pucSystemId", "");
					staff.m_strEnableFlag = p_pJson->GetNodeValue(l_strPrefixPath + "enableFlag", "");
					staff.m_strVersion = p_pJson->GetNodeValue(l_strPrefixPath + "version", "");
					staff.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "createUser", "");
					staff.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "updateUser", "");
					staff.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "createTime", "");
					staff.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "updateTime", "");
					staff.m_strSynVersion = p_pJson->GetNodeValue(l_strPrefixPath + "syncVersion", "");
					staff.m_strbusinessOrgGuid = p_pJson->GetNodeValue(l_strPrefixPath + "businessOrgGuid", "");
									
					m_vecStaffs.push_back(staff);
				}
				return true;
			}

		public:			
			std::vector<SmpDataStaff> m_vecStaffs;
			std::string m_strCurrentPackageCount;
			std::string m_strPackageSize;
		};
	}
}
