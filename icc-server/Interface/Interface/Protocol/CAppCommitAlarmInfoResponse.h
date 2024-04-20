#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAppGetAlarmIdResponse :
			public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmCreateTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_strAlarmId);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strAlarmCreateTime);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strAlarmId;			//警单ID
				std::string m_strAlarmCreateTime;	//警单创建时间
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}