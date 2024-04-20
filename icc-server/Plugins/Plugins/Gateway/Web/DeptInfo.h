#pragma once
#include <Json/IJsonFactory.h>

#define DEPT_INFO_KEY "DeptInfoKey"

namespace ICC
{
	class CDeptInfo
	{
	public:
		CDeptInfo();
		~CDeptInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//单位guid
		std::string m_strParentGuid;	//上级单位guid
		std::string m_strCode;			//单位编码
		std::string m_strDistrictCode;	//单位行政区划编码
		std::string m_strType;			//单位类型
		std::string m_strName;			//单位名称
		std::string m_strPhone;			//单位座机
		std::string m_strShortcut;		//单位快捷查询缩写
		std::string m_strLevel;
		std::string m_strSort;			//单位序号，一般从1开始排序	
	};
};