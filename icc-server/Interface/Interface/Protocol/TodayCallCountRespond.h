#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayCallCountRespond :
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
				p_pJson->SetNodeValue("/body/call_in_count", m_oBody.m_strCallInCount);
				p_pJson->SetNodeValue("/body/call_out_count", m_oBody.m_strCallOutCount);
				p_pJson->SetNodeValue("/body/call_release_count", m_oBody.m_strReleaseCount);
				p_pJson->SetNodeValue("/body/average_talk_time", m_oBody.m_strAverageTalkTime);

				return p_pJson->ToString();
			}

		public:			
			class CBody
			{
			public:
				std::string	m_strCallInCount;				//呼入数量
				std::string	m_strCallOutCount;				//呼出数量
				std::string m_strReleaseCount;				//早释数量
				std::string m_strAverageTalkTime;			//平均通话时长	
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}