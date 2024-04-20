#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CClientRegisterSync :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strClientId = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientType = p_pJson->GetNodeValue("/body/client_type", "");
				m_oBody.m_strClientStatus = p_pJson->GetNodeValue("/body/client_status", "");

				return true;
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strClientId;
				std::string m_strClientType;
				std::string m_strClientStatus;
			};
			CBody m_oBody;
		};
	}
}
