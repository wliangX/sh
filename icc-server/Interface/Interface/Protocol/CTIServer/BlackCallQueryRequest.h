#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBlackCallQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

			//	m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
			//	std::string m_strSeatNo;
				std::string m_strBeginTime;
				std::string m_strEndTime;
			};
			CBody m_oBody;
		};
	}
}
