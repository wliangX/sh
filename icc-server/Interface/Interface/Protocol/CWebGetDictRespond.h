#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetDictRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				unsigned int l_uiIndex = 0;
				for (auto dict : m_oBody.m_vecDict)
				{
					std::string l_strPrefixPath("/body/dict/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "code", dict.m_strCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_code", dict.m_strParentCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "value", dict.m_strValue);
					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CDict
			{
			public:
				std::string m_strCode;				//×Öµä±àÂë
				std::string m_strParentCode;		//ÉÏ¼¶×Öµä±àÂë
				std::string m_strValue;				//×ÖµäÖµ
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::vector<CDict> m_vecDict;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}