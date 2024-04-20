#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
        class CGetUserInfoRequest :
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
