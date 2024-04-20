#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CAlarmOnlineList
	{
	public:
		CAlarmOnlineList();
		~CAlarmOnlineList();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strClientID, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strClientID;
	};

}