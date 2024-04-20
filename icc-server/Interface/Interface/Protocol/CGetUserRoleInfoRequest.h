#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
        class CGetUserRoleInfoRequest :
            public IRequest
        {
        public:
			bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_Header.ParseString(p_strJson, p_pJson))
				{
					return false;
				}
				m_Body.m_strCount = p_pJson->GetNodeValue("/body/count", "");
				m_Body.m_strVersionData = p_pJson->GetNodeValue("/body/version_data", "");
                int l_iCount = atoi(m_Body.m_strCount.c_str());
                for (int i = 0; i < p_pJson->GetCount("/body/data"); i++)
                {
                    std::string l_strPath = "/body/data/";
                    std::string l_strNum = std::to_string(i);
                    CBody::CData l_CData;
                    l_CData.m_strCustomerType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/customerType", "");
                    l_CData.m_IsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isvalid", "");
                    l_CData.m_strUserCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/userCode", "");
                    l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/userGuid", "");

                    int l_iValueCount = p_pJson->GetCount(l_strPath + l_strNum + "/roleInfo");
                    for (int j = 0; j < l_iValueCount; j++)
                    {
                        std::string l_strValueNum = std::to_string(j);
                        CBody::CData::CRoleInfo l_CRoleInfo;
                        l_CRoleInfo.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/guid", "");
                        l_CRoleInfo.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/name", "");
                        l_CRoleInfo.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/remark", "");
                        l_CRoleInfo.m_IsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/isvalid", "");
                        l_CRoleInfo.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/createTime", "");
                        l_CRoleInfo.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/createUser", "");
                        l_CRoleInfo.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/updateTime", "");
                        l_CRoleInfo.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/updateUser", "");
                        l_CRoleInfo.m_strParentGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/roleInfo/" + l_strValueNum + "/parentGuid", "");
                        l_CData.m_vecRoleInfo.push_back(l_CRoleInfo);
                    }

                    m_Body.m_vecData.push_back(l_CData);
                }
                return true;
            }

        public:
            class CBody
            {
            public:
                class CData
                {
                public:
                    std::string m_strUserGuid;
                    std::string m_strUserCode;
                    std::string m_strCustomerType;
                    std::string m_IsValid;

                    class CRoleInfo
                    {
                    public:
                        std::string m_IsValid;
                        std::string m_strRemark;
                        std::string m_strGuid;
                        std::string m_strName;
                        std::string m_strParentGuid;
                        std::string m_strCreateTime;
                        std::string m_strCreateUser;
                        std::string m_strUpdateTime;
                        std::string m_strUpdateUser;
                    };
                    std::vector<CRoleInfo>m_vecRoleInfo;
                };
                std::vector<CData>m_vecData;
                std::string m_strCount;
				std::string m_strVersionData;
            };

            CSmpHeader m_Header;
            CBody m_Body;
        };
    }
}
