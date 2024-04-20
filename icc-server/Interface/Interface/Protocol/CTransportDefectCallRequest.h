#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTransPortDeflectCallRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);

				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strCallrefID; //话务ID
				std::string m_strSponsor;   //偏转发起分机号
				std::string m_strTarget;    //偏转目标分机号
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}

