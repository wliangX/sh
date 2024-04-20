#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CDBOnlineList
	{
	public:
		CDBOnlineList();
		~CDBOnlineList();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strClientID, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strClientID;
	};

}