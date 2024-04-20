#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayAlarmInfoByFirstTypeRespond :
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
				
			    if (m_oBody.m_strCount.empty())
			    {
					m_oBody.m_strCount = "0";
			    }
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				int iCount = m_oBody.m_vecDatas.size();				

				unsigned int l_uiIndex = 0;
				for (CAlarmInfo data : m_oBody.m_vecDatas)
				{					
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.ComJson(l_strPrefixPath, p_pJson);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:					
			class CBody
			{
			public:
				CBody()
				{
					m_strAllCount = "0";
					m_strCount = "0";
				}

			public:		
				std::string m_strAllCount;
				std::string m_strCount;
				std::vector<CAlarmInfo> m_vecDatas;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}