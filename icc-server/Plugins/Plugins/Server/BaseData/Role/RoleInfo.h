#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CRoleInfo
	{
	public:
		CRoleInfo();
		~CRoleInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;
		std::string m_strParentGuid;
		std::string m_strCode;
		std::string m_strName;
		std::string m_strSort;
	};
};