#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetRoleRespond :
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
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/guid", m_oBody.m_vecData[i].m_strGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/parent_guid", m_oBody.m_vecData[i].m_strParentGuid);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/code", m_oBody.m_vecData[i].m_strCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/name", m_oBody.m_vecData[i].m_strName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/sort", m_oBody.m_vecData[i].m_strSort);
				}
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strGuid;
					std::string m_strParentGuid;
					std::string m_strCode;
					std::string m_strName;
					std::string m_strSort;
				};

				std::string m_strCount;
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
