#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetConferencePartyRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strConferenceId = p_pJson->GetNodeValue("/body/conference_id", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCompere = p_pJson->GetNodeValue("/body/compere", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strConferenceId;
				std::string m_strCallRefId;
				std::string m_strCompere;
				std::string m_strTarget;
			};
			CBody m_oBody;
		};
	}
}