#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsUserStatusSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (auto unitid : m_oBody.m_vecUnitIdList)
				{
					std::string l_strPrefixPath("/body/unitidlist/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "unitid", unitid);

					l_uiIndex++;

				}

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strType;
				std::string m_strCount;
				std::vector<std::string> m_vecUnitIdList;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
