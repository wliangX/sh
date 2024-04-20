#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAgentLoginModeRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strACD = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAgent;
				std::string m_strACD;
				std::string m_strLoginMode;
			};
			CBody m_oBody;
		};
	}
}
