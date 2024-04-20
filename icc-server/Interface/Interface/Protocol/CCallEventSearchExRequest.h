#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventSearchExRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strAcd = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strAcd;
				std::string m_strSeatNo;
				std::string m_strStartTime;
				std::string m_strEndTime;
			};
			CBody m_oBody;
		};
	}
}
