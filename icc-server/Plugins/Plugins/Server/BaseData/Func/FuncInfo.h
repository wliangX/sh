#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CFuncInfo
	{
	public:
		CFuncInfo();
		~CFuncInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;
		std::string m_strCode;
		std::string m_strName;
		std::string m_strParentGuid;
		std::string m_strSort;
	};

	// �û���Ϣ���棬�������ã�ʹ��Redis����
	typedef std::map<std::string, CFuncInfo> PhoneInfoMap;
}