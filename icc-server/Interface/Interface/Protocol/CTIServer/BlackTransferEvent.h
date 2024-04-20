#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBlackTransferEvent :
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

				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmId);
				p_pJson->SetNodeValue("/body/urge_id", m_oBody.m_strUrgeId);
				p_pJson->SetNodeValue("/body/department", m_oBody.m_strDepartment);
				p_pJson->SetNodeValue("/body/voice_type", m_oBody.m_strVoiceType);
				p_pJson->SetNodeValue("/body/voice_id", m_oBody.m_strVoiceId);
				p_pJson->SetNodeValue("/body/tts_content", m_oBody.m_strTTSContent);
				p_pJson->SetNodeValue("/body/urge_called", m_oBody.m_strUrgeCalled);
				p_pJson->SetNodeValue("/body/urge_level", m_oBody.m_strUrgeLevel);
				p_pJson->SetNodeValue("/body/urge_count", m_oBody.m_strUrgeCount);
				p_pJson->SetNodeValue("/body/urge_start_time", m_oBody.m_strUrgeStartTime);
				p_pJson->SetNodeValue("/body/urge_interval", m_oBody.m_strUrgeInterval);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue("/body/update_user", m_oBody.m_strUpdateUser);
				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/body/remarks", m_oBody.m_strRemarks);
				p_pJson->SetNodeValue("/body/sms_content", m_oBody.m_strSMSContent);
				p_pJson->SetNodeValue("/body/extension_no", m_oBody.m_strExtensionNo);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAlarmId;
				std::string m_strUrgeId;
				std::string m_strDepartment;
				std::string m_strVoiceType;
				std::string m_strVoiceId;
				std::string m_strTTSContent;
				std::string m_strUrgeCalled;
				std::string m_strUrgeLevel;
				std::string m_strUrgeCount;
				std::string m_strUrgeStartTime;
				std::string m_strUrgeInterval;
				std::string m_strCreateUser;
				std::string m_strUpdateUser;
				std::string m_strSeatNo;
				std::string m_strRemarks;
				std::string m_strSMSContent;
				std::string m_strExtensionNo;
			};
			CBody m_oBody;
		};
	}
}
