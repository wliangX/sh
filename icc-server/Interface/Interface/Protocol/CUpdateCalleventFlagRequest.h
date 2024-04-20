#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CUpdateCalleventFlagRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallEventId = p_pJson->GetNodeValue("/body/callevent_id", "");
				m_oBody.m_strFlag = p_pJson->GetNodeValue("/body/flag", "");			

				return true;
			}	

		public:
			class CBody
			{
			public:
				std::string m_strCallEventId;
				std::string m_strFlag;//标记，骚扰电话等
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}
