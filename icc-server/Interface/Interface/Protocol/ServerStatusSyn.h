#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSyncServerStatus : public ISend, public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strServerID = p_pJson->GetNodeValue("/body/server_id", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/server_id", m_oBody.m_strServerID);
				return p_pJson->ToString();
			}
		
			class CBody
			{
			public:
				std::string m_strServerID;				
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
