#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
        class CGetUserStaffInfoRequest :
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
                    l_CData.m_strStaffGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staffGuid", "");
                    l_CData.m_strUserCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/userCode", "");
                    l_CData.m_strStaffCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staffCode", "");
                    l_CData.m_strOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/orgGuid", "");
                    l_CData.m_strOrgIdentifier = p_pJson->GetNodeValue(l_strPath + l_strNum + "/orgIdentifier", "");
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
                    std::string m_strStaffCode;
                    std::string m_strOrgGuid;
                    std::string m_strOrgIdentifier;
                    std::string m_strGuid;
                    std::string m_strStaffGuid;
                    std::string m_strUserCode;

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
