#pragma once
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CAdminInfo
	{
	public:
		CAdminInfo();
		~CAdminInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strAdminInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;				//可管辖单位guid
		std::string m_strParentGuid;		//上级可管辖单位guid
		std::string m_strBindDeptCode;		//该节点所绑定的单位code
		std::string m_strBindParentDeptCode;//该节点上级所绑定的单位code
		std::string m_strBelongDeptCode;	//该节点所归属的单位code
	};
};