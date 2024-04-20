#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CPhoneInfo
	{
	public:
		CPhoneInfo();
		~CPhoneInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strPhone;
		std::string m_strName;
		std::string m_strAddress;
	};

	// �û���Ϣ���棬�������ã�ʹ��Redis����
	typedef std::map<std::string, CPhoneInfo> PhoneInfoMap;
}