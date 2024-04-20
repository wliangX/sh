#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
		class CGetRoleFuncBindRequest :
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
                    l_CData.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/name", "");
                    l_CData.m_strSystemNo = p_pJson->GetNodeValue(l_strPath + l_strNum + "/systemNo", "");
                    l_CData.m_IsValid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isvalid", "");
                    l_CData.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/code", "");

                    int l_iValueCount = p_pJson->GetCount(l_strPath + l_strNum + "/dataInfoList");
                    for (int j = 0; j < l_iValueCount; j++)
                    {
                        std::string l_strValueNum = std::to_string(j);
                        CBody::CData::CDataInfo l_CDataInfo;
                        l_CDataInfo.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dataInfoList/" + l_strValueNum + "/guid", "");
                        l_CDataInfo.m_strOrgIdentifier = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dataInfoList/" + l_strValueNum + "/org_identifier", "");
                        l_CDataInfo.m_strOrgGovCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dataInfoList/" + l_strValueNum + "/org_gov_code", "");
                        l_CDataInfo.m_strParentOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dataInfoList/" + l_strValueNum + "/parent_org_guid", "");
                        l_CData.m_vecDataInfo.push_back(l_CDataInfo);
                    }

                    l_iValueCount = p_pJson->GetCount(l_strPath + l_strNum + "/funcInfoList");
                    for (int j = 0; j < l_iValueCount; j++)
                    {
                        std::string l_strValueNum = std::to_string(j);
                        CBody::CData::CFuncInfo l_CFuncInfo;
                        l_CFuncInfo.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/funcInfoList/" + l_strValueNum + "/guid", "");
                        l_CFuncInfo.m_strCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/funcInfoList/" + l_strValueNum + "/code", "");
                        l_CFuncInfo.m_strName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/funcInfoList/" + l_strValueNum + "/name", "");
                        l_CFuncInfo.m_strParentCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/funcInfoList/" + l_strValueNum + "/parent_code", "");
                        l_CData.m_vecFuncInfo.push_back(l_CFuncInfo);
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
                    class CDataInfo
                    {
                    public:
                        std::string m_strGuid;
                        std::string m_strOrgIdentifier;
                        std::string m_strOrgGovCode;
                        std::string m_strParentOrgGuid;
                    };
                    class CFuncInfo
                    {
                    public:
                        std::string m_strGuid;
                        std::string m_strName;
                        std::string m_strCode;
                        std::string m_strParentCode;
                    };
                public:
                    std::string m_strGuid;
                    std::string m_strName;
                    std::string m_strSystemNo;
                    std::string m_IsValid;
                    std::string m_strCode;
                    std::vector<CDataInfo>m_vecDataInfo;
                    std::vector<CFuncInfo>m_vecFuncInfo;
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
