#pragma once 
#include <map>
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSQLRespond :
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
				
				for (unsigned int l_uiIndex = 1; l_uiIndex < m_oBody.m_vecData.size(); ++l_uiIndex)
				{
					std::string l_strPrefixPath = "/body/data/" + std::to_string(l_uiIndex-1);
					std::vector<std::string>& l_rvecData = m_oBody.m_vecData[l_uiIndex];
					for (unsigned int l_uiResultIndex = 0; l_uiResultIndex != l_rvecData.size(); ++l_uiResultIndex)
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "/" + m_oBody.m_vecData[0][l_uiResultIndex], l_rvecData[l_uiResultIndex]);
					}
				}
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;			
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<std::vector<std::string>> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
