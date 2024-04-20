#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CBindInfo
	{
	public:
		CBindInfo();
		~CBindInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strBindInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;
		std::string m_strType;
		std::string m_strFromGuid;
		std::string m_strToGuid;
		std::string m_strFlag;
		std::string m_strLevel;
		std::string m_strSort;
		std::string m_strShortcut;
		std::string m_strCreateUser;
		std::string m_strCreateTime;
		std::string m_strUpdateUser;
		std::string m_strUpdatetime;
		std::string m_strResult;
	    std::string m_strSyncType;
	};
};