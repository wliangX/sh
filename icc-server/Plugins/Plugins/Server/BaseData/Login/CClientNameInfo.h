#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CClientNameInfo
	{
	public:
		CClientNameInfo();
		~CClientNameInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strClientName;
	};

}