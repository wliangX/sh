#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CKeyWordInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetViolationStatisticsRespond :
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
			
				for (size_t i = 0; i < m_oBody.m_vecData.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/violation_type_count/" + l_strIndex + "/violation_type", m_oBody.m_vecData[i].m_strViolationType);
					p_pJson->SetNodeValue("/body/violation_type_count/" + l_strIndex + "/count", m_oBody.m_vecData[i].m_strCount);
										
				}
				
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			
			class CData
			{
			public:
			    std::string m_strViolationType;
				std::string m_strCount;
			};
			
			class CBody
			{
			public:				
				std::string m_strAllCount;
				std::vector<CData> m_vecData;
			public:
				CBody() :m_strAllCount("0")
				{

				}
			};
			CBody m_oBody;
		};
	}
}
