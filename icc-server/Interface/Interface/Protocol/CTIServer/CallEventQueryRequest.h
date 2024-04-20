#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strAcdCall = p_pJson->GetNodeValue("/body/acd_call", "");
				m_oBody.m_strCount = p_pJson->GetNodeValue("/body/count", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strSeatNo;
				std::string m_strCallerId;
				std::string m_strAcdCall;
				std::string m_strCount;
			};
			CBody m_oBody;
		};
	}
}
