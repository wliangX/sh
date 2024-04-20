#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRefuseCallEvent :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/refuse_reason", m_oBody.m_strRefuseReason);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strCallerId;
				std::string m_strCalledId;
				std::string	m_strRefuseReason;	//	æ‹Ω”‘≠“Ú
				std::string m_strTime;
			};
			CBody m_oBody;
		};
	}
}
