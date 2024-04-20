#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CConsultationCallRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strHeldCallRefId = p_pJson->GetNodeValue("/body/held_callref_id", "");
				m_oBody.m_strSponsor = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");
				m_oBody.m_strTargetDeviceType = p_pJson->GetNodeValue("/body/target_device_type", "");
				m_oBody.m_strIsACD = p_pJson->GetNodeValue("/body/is_acd", "");
				m_oBody.m_strIsConference = p_pJson->GetNodeValue("/body/is_conference", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strHeldCallRefId;
				std::string m_strSponsor;
				std::string m_strTarget;
				std::string m_strTargetDeviceType;
				std::string m_strIsACD;
				std::string m_strIsConference;
			};
			CBody m_oBody;
		};
	}
}
