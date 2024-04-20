#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/acd", m_acd);
				p_pJson->SetNodeValue("/dept_code", m_dept_code);
				p_pJson->SetNodeValue("/dept_name", m_dept_name);
				p_pJson->SetNodeValue("/remark", m_remark);
				p_pJson->SetNodeValue("/dept_code_path", m_dept_code_path);
				p_pJson->SetNodeValue("/dept_name_path", m_dept_name_path);
				p_pJson->SetNodeValue("/update_time", m_update_time);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_acd = p_pJson->GetNodeValue("/acd", "");
				m_dept_code = p_pJson->GetNodeValue("/dept_code", "");
				m_dept_name = p_pJson->GetNodeValue("/dept_name", "");
				m_remark = p_pJson->GetNodeValue("/remark", "");
				m_dept_code_path = p_pJson->GetNodeValue("/dept_code_path", "");
				m_dept_name_path = p_pJson->GetNodeValue("/dept_name_path", "");
				m_update_time = p_pJson->GetNodeValue("/update_time", "");

				return true;
			}

		public:
			std::string m_acd;
			std::string m_dept_code;
			std::string m_dept_name;
			std::string m_remark;
			std::string m_dept_code_path;
			std::string m_dept_name_path;
			std::string m_update_time;
		};
	}
};