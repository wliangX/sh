#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetLanguageRespond :
			public IRespond
		{
		public:
            virtual std::string ToString(IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
                for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strValueNum = std::to_string(i);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/code", m_oBody.m_vecData.at(i).m_strCode);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/guid", m_oBody.m_vecData.at(i).m_strGuid);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/name", m_oBody.m_vecData.at(i).m_strName);
                    p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/sort", m_oBody.m_vecData.at(i).m_strSort);
                }
                return p_pJson->ToString();
            }


		public:
			CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strCount;
                class CData
                {
                public:
                    std::string m_strGuid;
                    std::string m_strCode;
                    std::string m_strName;
                    std::string m_strSort;
                };
                std::vector<CData>m_vecData;
            };
			CBody m_oBody;
		};
	}
}
