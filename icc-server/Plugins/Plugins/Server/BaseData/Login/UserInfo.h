#pragma once
#include <Json/IJson.h>

namespace ICC
{
	class CUserInfo
	{
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);
	public:
		std::string m_strGuid;
		std::string m_strCode;
		std::string m_strName;
		std::string m_strPwd;
		std::string m_strIsDelete;
        std::string m_strRemark;
		std::string m_strLevel;
		std::string m_strSort;
		std::string m_strShortCut;
		std::string m_strCreateUser;
		std::string m_strCreateTime;
		std::string m_strUpdateUser;
		std::string m_strUpdateTime;
	};
}