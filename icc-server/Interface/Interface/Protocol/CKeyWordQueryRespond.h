#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordQueryRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/guid", m_oBody.m_vecData[i].m_strGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/key_word", m_oBody.m_vecData[i].m_strKeyWord);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/content", m_oBody.m_vecData[i].m_strContent);					
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
				std::vector<CKeyWordInfo> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
