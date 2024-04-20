#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CPutAlarmLogInfoRespond :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strId = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strAlarmTime = p_pJson->GetNodeValue("/body/alarm_time", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strAlarmContent = p_pJson->GetNodeValue("/body/alarm_content", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/id", m_oBody.m_strId);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmId);
				p_pJson->SetNodeValue("/body/alarm_time", m_oBody.m_strAlarmTime);
				p_pJson->SetNodeValue("/body/update_user", m_oBody.m_strUpdateUser);
				p_pJson->SetNodeValue("/body/alarm_content", m_oBody.m_strAlarmContent);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strId;
				std::string m_strAlarmId;
				std::string m_strAlarmTime;
				std::string m_strUpdateUser;
				std::string m_strAlarmContent;
				std::string m_strResult;
			};
			CBody m_oBody;
		};
	}
}
