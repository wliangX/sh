#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
    namespace PROTOCOL
    {
        class CGetDictRespond :
            public IRespond
        {
        public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
               // p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
                p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
                p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
                p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);

                for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strValueNum = std::to_string(i);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/parent_guid", m_oBody.m_vecData.at(i).m_strParentGuid);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/guid", m_oBody.m_vecData.at(i).m_strGuid);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/code", m_oBody.m_vecData.at(i).m_strCode);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/sort", m_oBody.m_vecData.at(i).m_strSort);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/shortcut", m_oBody.m_vecData.at(i).m_strShortCut);
                    p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/value", m_oBody.m_vecData.at(i).m_strValue);
                    /*for (size_t j = 0; j < m_oBody.m_vecData.at(i).m_vecValue.size();j++)
                    {
                        std::string l_strValueDataNum = std::to_string(j);
                        p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/value/" + l_strValueDataNum + "/guid", m_oBody.m_vecData.at(i).m_vecValue.at(j).m_strGuid);
                        p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/value/" + l_strValueDataNum + "/value", m_oBody.m_vecData.at(i).m_vecValue.at(j).m_strValue);
                        p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/value/" + l_strValueDataNum + "/lang_guid", m_oBody.m_vecData.at(i).m_vecValue.at(j).m_strLangGuid);
                    }*/
                }

                return p_pJson->ToString();
            }


        public:
            CHeaderEx m_oHeader;

            class CBody
            {
            public:
                std::string m_strAllCount;
                std::string m_strCount;
                std::string m_strPageIndex;
               // std::string m_strResult;

                class CData
                {
                public:
                    std::string m_strGuid;
                    std::string m_strParentGuid;
                    std::string m_strCode;
                    std::string m_strShortCut;
                    std::string m_strSort;
                    std::string m_strValue;

                   /* class CValue
                    {
                    public:
                        std::string m_strGuid;
                        std::string m_strValue;
                        std::string m_strLangGuid;
                    };
                    std::vector<CValue>m_vecValue;*/
                };
                std::vector<CData>m_vecData;
            };
            CBody m_oBody;
        };
    }
}
