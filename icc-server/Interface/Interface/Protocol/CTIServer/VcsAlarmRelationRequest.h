#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsAlarmRelationRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strRelAlarmId = p_pJson->GetNodeValue("/body/rel_alram_id", "");
				m_oBody.m_strAcceptState = p_pJson->GetNodeValue("/body/accept_state", "");
				m_oBody.m_strRelType = p_pJson->GetNodeValue("/body/rel_type", "");
				m_oBody.m_strMessageId = p_pJson->GetNodeValue("/body/msg_id", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;	
				std::string m_strRelAlarmId;
				std::string m_strAcceptState;
				std::string m_strRelType;
				std::string m_strMessageId;
			};

			CBody m_oBody;
		};
	}
}
