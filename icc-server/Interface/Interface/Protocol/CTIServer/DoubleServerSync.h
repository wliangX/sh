#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDoubleServerSync :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strServiceID = p_pJson->GetNodeValue("/body/service_id", "");
				m_oBody.m_strServiceState = p_pJson->GetNodeValue("/body/service_state", "");

				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strServiceID;
				std::string m_strServiceState;
			};
			CBody m_oBody;
		};
	}
}
