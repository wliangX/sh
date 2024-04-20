#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CMajorAlarmConfigQueryResponse :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_vecData[i].m_guid);
					p_pJson->SetNodeValue(l_strPrefixPath + "type_name", m_oBody.m_vecData[i].m_type_name);
					p_pJson->SetNodeValue(l_strPrefixPath + "type_value", m_oBody.m_vecData[i].m_type_value);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_vecData[i].m_remark);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CData
			{
			public:
				std::string m_guid;
				std::string m_type_name;
				std::string m_type_value;
				std::string m_remark;
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
