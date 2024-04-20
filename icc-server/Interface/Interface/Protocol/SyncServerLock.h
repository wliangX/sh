#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSyncServerLock : public ISend, public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strServiceID = p_pJson->GetNodeValue("/body/service_id", "");
				m_oBody.m_strServiceState = p_pJson->GetNodeValue("/body/service_state", "");
				m_oBody.m_strClientID = p_pJson->GetNodeValue("/body/clientid", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/service_id", m_oBody.m_strServiceID);
				p_pJson->SetNodeValue("/body/service_state", m_oBody.m_strServiceState);
				p_pJson->SetNodeValue("/body/clientid", m_oBody.m_strClientID);
				return p_pJson->ToString();
			}
		
			class CBody
			{
			public:
				std::string m_strServiceID;
				std::string m_strServiceState;				
				std::string m_strClientID;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
