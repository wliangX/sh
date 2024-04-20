#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetCurrentTimeRespond :
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

				p_pJson->SetNodeValue("/body/current_time", m_oBody.m_strCurrentTime);
				p_pJson->SetNodeValue("/body/current_utctime", m_oBody.m_strCurrentUTCTime);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCurrentTime;
				std::string m_strCurrentUTCTime;
			};
			CBody m_oBody;
		};
	}
}