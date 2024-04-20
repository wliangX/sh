#pragma once
#include <Json/IJsonFactory.h>
#include <vector>

namespace ICC
{
	struct  CValue
	{
	public:
		std::string m_strGuid;
		std::string m_strValue;
		std::string m_strLangGuid;
	};

	struct  CRelation
	{
	public:
		std::string m_strGuid;
		std::string m_strValue;
		std::string m_strParentGuid;
	};

	class CDictInfo
	{
	public:
		CDictInfo();
		~CDictInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson);

	public:
        std::string m_strGuid;
        std::string m_strParentGuid;
        std::string m_strCode;
        std::string m_strShortCut;
        std::string m_strSort;       
        std::vector<CValue>m_vecValue;
	};

	typedef std::pair<std::string, CDictInfo> PAIR;
	class CCodeCompare
	{
	public:
		bool operator()(const PAIR& p_infoFirst, const PAIR& p_infoSecond) const
		{
			return p_infoFirst.second.m_strParentGuid < p_infoSecond.second.m_strParentGuid;
		}
	};
};