#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransferCallRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strActiveCallRefId = p_pJson->GetNodeValue("/body/active_callref_id", "");
				m_oBody.m_strHeldCallRefId = p_pJson->GetNodeValue("/body/held_callref_id", "");
				m_oBody.m_strSponsor = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strActiveCallRefId;
				std::string m_strHeldCallRefId;
				std::string m_strSponsor;
				std::string m_strTarget;
			};
			CBody m_oBody;
		};
	}
}
