#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTakeOverCallSync :
			public IRequest,
			public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strSponsor = p_pJson->GetNodeValue("/body/sponsor", "");
				m_oBody.m_strTarget = p_pJson->GetNodeValue("/body/target", "");
				m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/case_id", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				p_pJson->SetNodeValue("/body/case_id", m_oBody.m_strCaseId);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strSponsor;
				std::string m_strTarget;
				std::string m_strCaseId;
				std::string m_strTime;
			};
			CBody m_oBody;
		};
	}
}
