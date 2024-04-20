#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmidBySourceid :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strSourceID = p_pJson->GetNodeValue("/body/source_id", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/alarmid", m_strAlarmID);
				
				return p_pJson->ToString();
			}

		public:

			CHeaderEx m_oHeader;
			std::string m_strSourceID;
			std::string m_strAlarmID;
		};
	}
}
