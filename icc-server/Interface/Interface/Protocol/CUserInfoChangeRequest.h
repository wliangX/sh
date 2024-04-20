#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CUserInfoChangeRequest :
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
                else if (m_Header.m_strAction == ACTION_BIND_STAFF)
                {//用户绑定警员消息
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/user_guid", "");
                        l_CData.m_strStaffGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staff_guid", "");
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else if (m_Header.m_strAction == ACTION_BIND_ROLE)
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/user_guid", "");
                        l_CData.m_strRoleGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/role_guid", "");
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else if (m_Header.m_strAction == ACTION_BIND_ORG)
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        l_CData.m_strOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_guid", "");
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else if (m_Header.m_strAction == ACTION_UNBIND_STAFF)
                {//用户解绑警员消息
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum+"/user_guid", "");                       
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else if (m_Header.m_strAction == ACTION_UNBIND_ROLE)
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/user_guid", "");
                        l_CData.m_strRoleGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/role_guid", "");
                        m_Body.m_vecData.push_back(l_CData);
                    }
                    return true;
                }
                else if (m_Header.m_strAction == ACTION_UNBIND_ORG)
                {
                    for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
                    {
                        std::string l_strPath = "/body/dataList/";
                        std::string l_strNum = std::to_string(i);
                        CBody::CData l_CData;
                        l_CData.m_strUserGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        l_CData.m_strOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_guid", "");
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
                        CBody::CData l_CData;
                        l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        l_CData.m_strUserCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/user_code", "");
                        l_CData.m_strPassword = p_pJson->GetNodeValue(l_strPath + l_strNum + "/password", "");
                        l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
                        l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
                        l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
                        l_CData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_user", "");
                        l_CData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_time", "");
                        l_CData.m_IsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isvalid", "");
                        l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
                        l_CData.m_strEnableFlag = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_flag", "");
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
                    std::string m_strUserCode;
                    std::string m_strPassword;
                    std::string m_strName;
                    std::string m_strCreateTime;
                    std::string m_strCreateUser;
                    std::string m_strUpdateTime;
                    std::string m_strUpdateUser;
                    std::string m_IsValid;
                    std::string m_strRemark;
                    std::string m_strEnableFlag;
                    std::string m_strStaffGuid;
                    std::string m_strUserGuid;
                    std::string m_strRoleGuid;
                    std::string m_strOrgGuid;
                };
                std::vector<CData>m_vecData;                
            };

            CSmpHeader m_Header;
            CBody m_Body;
        };
	}
}
