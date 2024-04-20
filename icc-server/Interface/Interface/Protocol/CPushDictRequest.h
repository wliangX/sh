#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
        class CPushDictRequest :
			public IRespond
		{
		public:
            virtual std::string ToString(IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/parent_guid", m_oBody.m_strParentGuid);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                p_pJson->SetNodeValue("/body/shortcut", m_oBody.m_strShortCut);
                p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);

                for (int i = 0; i < m_oBody.m_vecValue.size(); i++)
                {
                    std::string l_strPath = "/body/value";
                    std::string l_strNum = std::to_string(i);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "guid", m_oBody.m_vecValue.at(i).m_strGuid);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "value", m_oBody.m_vecValue.at(i).m_strValue);
                    p_pJson->SetNodeValue(l_strPath + l_strNum + "lang_guid", m_oBody.m_vecValue.at(i).m_strLangGuid);
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
