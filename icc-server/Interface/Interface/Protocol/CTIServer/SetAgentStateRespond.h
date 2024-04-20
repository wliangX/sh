#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetAgentStateRespond :
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
				p_pJson->SetNodeValue("/body/ready_state", m_oBody.m_strReadyState);
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
				std::string m_strReadyState;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};


		class CSetAgentStateSetFailedSync :
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
				p_pJson->SetNodeValue("/body/ready_state", m_oBody.m_strReadyState);
				p_pJson->SetNodeValue("/body/case", m_oBody.m_strCase);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAgent;
				std::string m_strACD;
				std::string m_strReadyState;
				std::string m_strCase;
				//std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
