#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTimer : public ISend, public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strTimerName = p_pJson->GetNodeValue("/body/timer_name", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/timer_name", m_oBody.m_strTimerName);
				return p_pJson->ToString();
			}
		
			class CBody
			{
			public:
				std::string m_strTimerName;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
