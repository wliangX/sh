#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSeatAlarmStatisticsRespond :
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
			
				for (size_t i = 0; i < m_oBody.m_vecSeatCount.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/seat_count/" + l_strIndex + "/seat_no", m_oBody.m_vecSeatCount[i].m_strSeatNo);
					p_pJson->SetNodeValue("/body/seat_count/" + l_strIndex + "/count", m_oBody.m_vecSeatCount[i].m_strCount);
										
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CSeatCount
			{
			public:
			    std::string m_strSeatNo;  //坐席号码
				std::string m_strCount;  //警情数量
			};
			
			class CBody
			{
			public:				
				std::string m_strAllCount;
				std::vector<CSeatCount> m_vecSeatCount;

			public:
				CBody() :m_strAllCount("0")
				{

				}
			};
			CBody m_oBody;
		};
	}
}
