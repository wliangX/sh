#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAgentLoginModeRespond :
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

				p_pJson->SetNodeValue("/body/agent", m_oBody.m_strAgent);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strACD);
				p_pJson->SetNodeValue("/body/login_mode", m_oBody.m_strLoginMode);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAgent;
				std::string m_strACD;
				std::string m_strLoginMode;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
