#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CConferenceCallRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/held_callref_id", m_oBody.m_strHeldCallRefId);
				p_pJson->SetNodeValue("/body/active_callref_id", m_oBody.m_strAvtiveCallRefId);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strHeldCallRefId;
				std::string m_strAvtiveCallRefId;
				std::string m_strSponsor;
				std::string m_strTarget;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
