#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CParamInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/name", m_strName);
				p_pJson->SetNodeValue("/value", m_strValue);
				p_pJson->SetNodeValue("/is_visibility", m_strIsVisibility);
				p_pJson->SetNodeValue("/is_readonly", m_strIsReadonly);
				p_pJson->SetNodeValue("/description", m_strDescription);
				p_pJson->SetNodeValue("/create_user", m_strCreateUser);
				p_pJson->SetNodeValue("/create_time", m_strCreateTime);
				p_pJson->SetNodeValue("/updatre_user", m_UpdateUser);
				p_pJson->SetNodeValue("/update_time", m_UpdateTime);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_strName = p_pJson->GetNodeValue("/name", "");
				m_strValue = p_pJson->GetNodeValue("/value", "");
				m_strIsVisibility = p_pJson->GetNodeValue("/is_visibility", "");
				m_strIsReadonly = p_pJson->GetNodeValue("/remark", "");
				m_strDescription = p_pJson->GetNodeValue("/dept_code_path", "");
				m_strCreateUser = p_pJson->GetNodeValue("/create_user", "");
				m_strCreateTime = p_pJson->GetNodeValue("/update_time", "");
				m_UpdateUser = p_pJson->GetNodeValue("/update_user", "");
				m_UpdateTime = p_pJson->GetNodeValue("/update_time", "");

				return true;
			}

		public:
			std::string m_strName;
			std::string m_strValue;
			std::string m_strIsVisibility;
			std::string m_strIsReadonly;
			std::string m_strDescription;
			std::string m_strCreateUser;
			std::string m_strCreateTime;
			std::string m_UpdateUser;
			std::string m_UpdateTime;
		};
	}
};