#pragma once
#include "Json/IJsonFactory.h"

namespace ICC
{
	class CSeatInfo
	{
	public:
		CSeatInfo();
		~CSeatInfo();
	public:
		std::string	ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strSeatInfo, JsonParser::IJsonPtr p_pJson);

    public:
        std::string m_strNo;
		std::string m_strName;
        std::string m_strDeptCode;
       	std::string m_strDeptName;
	};
}