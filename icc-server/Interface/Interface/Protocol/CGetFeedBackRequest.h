#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetFeedBackRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strProcessID = p_pJson->GetNodeValue("/body/process_id", "");
				m_oBody.m_strIgnorePrivacy = p_pJson->GetNodeValue("/body/ignore_privacy", "false");
				m_oBody.m_strLinkedID = p_pJson->GetNodeValue("/body/lindked_id", "");
				m_oBody.m_strCallRefID = p_pJson->GetNodeValue("/body/callref_id","");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;
				std::string m_strProcessID;
				std::string m_strIgnorePrivacy;   //忽略隐私保护
				std::string m_strLinkedID;			// 联动单位ID
				std::string m_strCallRefID;			//话务ID
			};
			CBody m_oBody;
		};
	}
}
