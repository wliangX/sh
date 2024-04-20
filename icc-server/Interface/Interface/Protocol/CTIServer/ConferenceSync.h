#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CConferenceSync :
			public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_oBody.m_strConferenceId = p_pJson->GetNodeValue("/body/conference_id", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCompere = p_pJson->GetNodeValue("/body/compere", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/conference_id", m_oBody.m_strConferenceId);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strConferenceId;
				std::string m_strCallRefId;
				std::string m_strState;
				std::string m_strCompere;
				std::string m_strTarget;
				std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
