#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CAlarmAttentionQueryResponse :
            public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);   
				
				p_pJson->SetNodeValue("/body/count", std::to_string(m_oBody.m_vecData.size()));
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_vecData[i].m_alarm_id);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_update", m_oBody.m_vecData[i].m_is_update);					
				}

                return p_pJson->ToString();
            }
			

		public:
			CHeaderEx m_oHeader;	

			class CData
			{
			public:
				std::string m_alarm_id;
				std::string m_is_update;
			};

			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;			
		};
	}
}
