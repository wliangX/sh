#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CMQClientInfo
	{
	public:
		CMQClientInfo();
		~CMQClientInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strClientID;
		std::string m_strClientIP;
		std::string m_strConnectionID;
	};

	class CUserDept
	{
	public:
		CUserDept();
		~CUserDept();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strDeptDistrictCode;
		std::string m_strDeptCode;
		std::string m_strDeptName;
		std::string m_strUserName;//’À∫≈
		std::string m_strSeatNo;
		std::string m_strBuyIdle; //idle:œ–£¨busy:√¶
	};

}