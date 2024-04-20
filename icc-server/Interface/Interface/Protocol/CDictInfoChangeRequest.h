#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CDictInfoChangeRequest :
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

						std::string l_strSystemNo = p_pJson->GetNodeValue(l_strPath + l_strNum + "/system_no", "");
						if (l_strSystemNo != "ICC")
						{
							continue;
						}

                        CBody::CData l_CData;
                        l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                        l_CData.m_strParentGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/parent_guid", "");
                        l_CData.m_strDictInfoKey = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_info_key", "");
                        l_CData.m_strDictInfoCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_info_code", "");
                        l_CData.m_strSystemGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/system_guid", "");
                        l_CData.m_strLevel = p_pJson->GetNodeValue(l_strPath + l_strNum + "/level", "");
                        l_CData.m_strStatus = p_pJson->GetNodeValue(l_strPath + l_strNum + "/status", "");
                        l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
                        l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
                        l_CData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_user", "");
                        l_CData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_time", "");
                        l_CData.m_strSort = p_pJson->GetNodeValue(l_strPath + l_strNum + "/sort", "");
                        l_CData.m_strDictType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_type", "");
						l_CData.m_strSystemNo = l_strSystemNo;
                        
                        int l_iValueCount = p_pJson->GetCount(l_strPath + l_strNum + "/dict_value");
                        for (int j = 0; j < l_iValueCount; j++)
                        {
                            std::string l_strValueNum = std::to_string(j);
                            CBody::CData::CDictValue l_CDictValue;
                            l_CDictValue.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/guid", "");
                            l_CDictValue.m_strValue = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/value", "");
                            l_CDictValue.m_strLanguageGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/language_guid", "");
                            l_CDictValue.m_strDictGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/dict_guid", "");
                            l_CDictValue.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/create_time", "");
                            l_CDictValue.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/create_user", "");
                            l_CDictValue.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/update_time", "");
                            l_CDictValue.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/update_user", "");
                            l_CDictValue.m_strSort = p_pJson->GetNodeValue(l_strPath + l_strNum + "/dict_value/" + l_strValueNum + "/sort", "");
                            l_CData.m_vecDictValue.push_back(l_CDictValue);
                        }
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
                    std::string m_strParentGuid;
                    std::string m_strDictInfoKey;
                    std::string m_strDictInfoCode;
                    std::string m_strSystemGuid;
                    std::string m_strLevel;
                    std::string m_strStatus;
                    std::string m_strSort;
                    std::string m_strCreateTime;
                    std::string m_strCreateUser;
                    std::string m_strUpdateTime;
                    std::string m_strUpdateUser;
                    std::string m_strDictType;
                    std::string m_strSystemNo;
                    class CDictValue
                    {
                    public:
                        std::string m_strGuid;
                        std::string m_strValue;
                        std::string m_strLanguageGuid;
                        std::string m_strDictGuid;
                        std::string m_strSort;
                        std::string m_strCreateTime;
                        std::string m_strCreateUser;
                        std::string m_strUpdateTime;
                        std::string m_strUpdateUser;
                    };
                    std::vector<CDictValue>m_vecDictValue;
                };
                std::vector<CData>m_vecData;                
            };

            CSmpHeader m_Header;
            CBody m_Body;
        };
	}
}
