#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
        class CSetDictRequest :
            public IRequest, public IRespond
		{
        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
                m_oBody.m_strParentGuid = p_pJson->GetNodeValue("/body/parent_guid", "");
                m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
                m_oBody.m_strShortCut = p_pJson->GetNodeValue("/body/shortcut", "");
                m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

                int l_iCount = p_pJson->GetCount("/body/value");
                for (int i = 0; i < l_iCount; i++)
                {
                    std::string l_strValueNum = std::to_string(i);
                    CBody::CValue l_CValue;
                    l_CValue.m_strValue = p_pJson->GetNodeValue("/body/value/" + l_strValueNum + "/value", "");
                    l_CValue.m_strGuid = p_pJson->GetNodeValue("/body/value/" + l_strValueNum + "/guid", "");
                    l_CValue.m_strLangGuid = p_pJson->GetNodeValue("/body/value/" + l_strValueNum + "/lang_guid", "");
                    m_oBody.m_vecValue.push_back(l_CValue);
                }
                return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                p_pJson->SetNodeValue("/body/shortcut", m_oBody.m_strShortCut);
                p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

                for (size_t i = 0; i < m_oBody.m_vecValue.size(); i++)
                {
                    std::string l_strPath = "/body/value/";
                    std::string l_strNum = std::to_string(i);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "/guid", m_oBody.m_vecValue.at(i).m_strGuid);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "/value", m_oBody.m_vecValue.at(i).m_strValue);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "/lang_guid", m_oBody.m_vecValue.at(i).m_strLangGuid);
                }
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strParentGuid;
				std::string m_strCode;
                std::string m_strShortCut;
                std::string m_strSort;
				std::string m_strSyncType;

                class CValue
                {
                public:
                    std::string m_strGuid;
                    std::string m_strValue;
                    std::string m_strLangGuid;
                };
                std::vector<CValue>m_vecValue;
			};
			CBody m_oBody;
		};
	}
}
