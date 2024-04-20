#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CDictSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                p_pJson->SetNodeValue("/body/shortcut", m_oBody.m_strShortCut);
                p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
               
                for (size_t i = 0; i < m_oBody.m_vecValue.size();i++)
                {
                    std::string l_strValueNum = std::to_string(i);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/value", m_oBody.m_vecValue.at(i).m_strValue);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/guid", m_oBody.m_vecValue.at(i).m_strGuid);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/lang_guid", m_oBody.m_vecValue.at(i).m_strLangGuid);
                }

				return p_pJson->ToString();
			}

		public:
            CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strSyncType;
                std::string m_strGuid;
                std::string m_strParentGuid;
                std::string m_strCode;
                std::string m_strShortCut;
                std::string m_strSort;

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
