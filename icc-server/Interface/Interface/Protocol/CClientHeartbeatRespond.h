#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CClientHeartbeatRespond :
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
				p_pJson->SetNodeValue("/body/interval", m_oBody.m_strInterval);
				
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:				
				std::string m_strInterval;
				
			};
			CBody m_oBody;
		};
	}
}
