#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
		//SMP 功能信息接口请求协议
        class CGetAuthInfoRequest :
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
                    l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");
                    l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
                    l_CData.m_strParentCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/parentCode", "");
                    l_CData.m_strIsAvailable = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isavailable", "");
                    l_CData.m_strSort= p_pJson->GetNodeValue(l_strPath + l_strNum + "/sort", "");
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
                    std::string m_strCode;
                    std::string m_strName;
					std::string m_strParentCode;
                    std::string m_strRemark;
					std::string m_strIsAvailable;
					std::string m_strSort;
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
