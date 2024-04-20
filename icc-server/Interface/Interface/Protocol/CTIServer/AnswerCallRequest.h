#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAnswerCallRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{


				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strDevice = p_pJson->GetNodeValue("/body/device", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strDevice;
			};
			CBody m_oBody;
		};
	}
}
