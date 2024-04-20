#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CDeptInfoChangeRequest :
            public IRequest
        {
        public:
            bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_Header.ParseString(p_strJson, p_pJson))
				{
					return false;
				}

                if (m_Header.m_strAction == ACTION_DELETE)
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);

						std::string m_strPucSysType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/puc_sys_type", "");
						if (m_strPucSysType != "ICC")
						{
							continue;
						}

                        CBody::CData l_CData;
                        l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        l_CData.m_strOrgIdentifier = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_identifier", "");
                        l_CData.m_strOrgGovCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_gov_code", "");
                        l_CData.m_strOrgName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_name", "");
                        l_CData.m_strBusessType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/busess_type", "");
                        l_CData.m_strOrgType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_type", "");
                        l_CData.m_strSeq = p_pJson->GetNodeValue(l_strPath + l_strNum + "/seq", "");
                        l_CData.m_strParentOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/parent_org_guid", "");
                        l_CData.m_strOrgIcon = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_icon", "");
                        l_CData.m_strLatitude = p_pJson->GetNodeValue(l_strPath + l_strNum + "/latitude", "");
                        l_CData.m_strLongitude = p_pJson->GetNodeValue(l_strPath + l_strNum + "/longitude", "");
                        l_CData.m_strContact = p_pJson->GetNodeValue(l_strPath + l_strNum + "/contact", "");
                        l_CData.m_strContactno = p_pJson->GetNodeValue(l_strPath + l_strNum + "/contactno", "");
                        l_CData.m_strFax = p_pJson->GetNodeValue(l_strPath + l_strNum + "/fax", "");
                        l_CData.m_strEnableFlag = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_flag", "");
                        l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
                        l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
                        l_CData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_user", "");
                        l_CData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_time", "");
                        l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
                        l_CData.m_strDistrictCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/district_code", "");
                        l_CData.m_strOrgShortName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_short_name", "");
                        l_CData.m_strDeleteTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/delete_time", "");
                        l_CData.m_strUpOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/up_org_guid", "");
                        l_CData.m_strIs110 = p_pJson->GetNodeValue(l_strPath + l_strNum + "/is110", "");
                        l_CData.m_strUpOrgGuidName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/up_org_guid_name", "");
                        l_CData.m_strImportAllName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/import_all_name", "");
                        l_CData.m_strRegionType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/region_type", "");
                        l_CData.m_strIsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isvalid", "");
						l_CData.m_strSystemNo = p_pJson->GetNodeValue(l_strPath + l_strNum + "/system_no", "");
						l_CData.m_strPucSysType = m_strPucSysType;

                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
            }
        public:
            class CBody
            {
            public:
                class CData
                {
                public:
                    std::string m_strGuid;
                    std::string m_strOrgIdentifier;
                    std::string m_strOrgGovCode;
                    std::string m_strOrgName;
                    std::string m_strBusessType;
                    std::string m_strOrgType;
                    std::string m_strSeq;
                    std::string m_strParentOrgGuid;
                    std::string m_strOrgIcon;
                    std::string m_strLatitude;
                    std::string	m_strLongitude;
                    std::string m_strContact;
                    std::string m_strContactno;
                    std::string m_strFax;
                    std::string m_strEnableFlag;
                    std::string m_strCreateUser;
                    std::string m_strCreateTime;
                    std::string m_strUpdateUser;
                    std::string m_strUpdateTime;
                    std::string m_strRemark;
                    std::string m_strDistrictCode;
                    std::string m_strOrgShortName;
                    std::string m_strDeleteTime;
                    std::string m_strUpOrgGuid;
                    std::string m_strIs110;
                    std::string m_strUpOrgGuidName;
                    std::string m_strImportAllName;
                    std::string m_strRegionType;
                    std::string m_strIsValid;
                    std::string m_strSystemNo;
					std::string m_strPucSysType;
                };

                std::vector<CData>m_vecData;                
            };
            CSmpHeader m_Header;
            CBody m_Body;
        };
	}
}
