#pragma once
#include <Json/IJson.h>

namespace ICC
{
	class CConfigInfo
	{
	public:
		CConfigInfo();
		~CConfigInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strClientID, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strIp;
		std::string m_strConfig;
	};

}