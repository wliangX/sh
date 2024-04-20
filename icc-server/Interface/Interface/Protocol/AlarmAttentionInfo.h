#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmAttentionInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/alarm_id", m_strAlarmId);
				p_pJson->SetNodeValue("/attention_staff", m_strAttentionStaff);
				p_pJson->SetNodeValue("/attention_time", m_strAttentionTime);
				p_pJson->SetNodeValue("/is_update", m_strIsUpdate);
				p_pJson->SetNodeValue("/update_time", m_strUpdateTime);
				p_pJson->SetNodeValue("/terminal_time", m_strTerminalTime);
				p_pJson->SetNodeValue("/expire_time", m_strExpireTime);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_strAlarmId = p_pJson->GetNodeValue("/alarm_id", "");
				m_strAttentionStaff = p_pJson->GetNodeValue("/attention_staff", "");
				m_strAttentionTime = p_pJson->GetNodeValue("/attention_time", "");
				m_strIsUpdate = p_pJson->GetNodeValue("/is_update", "");
				m_strUpdateTime = p_pJson->GetNodeValue("/update_time", "");
				m_strTerminalTime = p_pJson->GetNodeValue("/terminal_time", "");
				m_strExpireTime = p_pJson->GetNodeValue("/expire_time", "");

				return true;
			}

		public:
			std::string m_strAlarmId;
			std::string m_strAttentionStaff;
			std::string m_strAttentionTime;
			std::string m_strIsUpdate;
			std::string m_strUpdateTime;
			std::string m_strTerminalTime;
			std::string m_strExpireTime;
		};
	}
};