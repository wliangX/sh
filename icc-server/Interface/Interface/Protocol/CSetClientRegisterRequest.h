#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetClientRegisterRequest :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/client_id", m_oBody.m_strClientID);
				p_pJson->SetNodeValue("/body/client_type", m_oBody.m_strClientType);
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strClientIP);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strClientID;
				std::string m_strClientType;
				std::string m_strClientName;
				std::string m_strClientIP;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
