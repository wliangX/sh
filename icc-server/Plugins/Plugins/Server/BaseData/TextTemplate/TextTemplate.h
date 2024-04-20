#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	class CTextTemplate
	{
	public:
		CTextTemplate();
		~CTextTemplate();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;
		std::string m_strType;
		std::string m_strContent;
		std::string m_strAlarm_type;
		std::string m_strHandly_type;
	};

	// �û���Ϣ���棬�������ã�ʹ��Redis����
	typedef std::map<std::string, CTextTemplate> TextTemplateMap;
}