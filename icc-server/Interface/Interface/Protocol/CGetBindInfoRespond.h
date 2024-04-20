#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
    namespace PROTOCOL
    {
		class CGetBindInfoRespond :
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
                p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
                for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/guid", m_oBody.m_vecData.at(i).m_strGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/type", m_oBody.m_vecData.at(i).m_strType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/from_guid", m_oBody.m_vecData.at(i).m_strFromGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/to_guid", m_oBody.m_vecData.at(i).m_strToGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/sort", m_oBody.m_vecData.at(i).m_strSort);
                }
                return p_pJson->ToString();
            }

        public:
            CHeader m_oHeader;
			class CBindInfo
			{
			public:
				std::string m_strGuid;
				std::string m_strType;
				std::string m_strFromGuid;
				std::string m_strToGuid;
				std::string m_strSort;
			};
            class CBody
            {
            public:
                std::string m_strCount;               
				std::vector<CBindInfo>m_vecData;
            };
            CBody m_oBody;
        };
    }
}
