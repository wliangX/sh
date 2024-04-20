#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	/*
	* 该类用于Redis缓存调派信息
	*/
	class CDispatchInfo
	{
	public:
		CDispatchInfo();
		~CDispatchInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDispatchInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//可调派单位guid
		std::string m_strParentGuid;	//上级可调派单位guid
		std::string m_strCode;			//原单位编码
		std::string m_strDistrictCode;	//原单位行政区划编码
		std::string m_strType;			//原单位类型
		std::string m_strName;			//原单位名称
		std::string m_strPhone;			//原单位座机
	};


	/*
	* 该类用于Redis缓存可调派信息
	*/
	class CCanDispatchInfo
	{
	public:
		CCanDispatchInfo();
		~CCanDispatchInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strCanDispatchInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;					//可调派单位guid
		std::string m_strParentGuid;			//上级可调派单位guid
		std::string m_strBindDeptCode;			//该节点所绑定的单位guid
		std::string m_strBindParentDeptCode;	//该节点上级所绑定的单位guid
		std::string m_BelongDeptCode;			//该节点所归属的单位guid
		std::string m_strCreateUser;			//创建用户
		std::string m_strCreateTime;			//创建时间
		std::string m_strUpdateUser;			//更新用户
		std::string m_strUpdateTime;			//更新时间
	};

	typedef std::map<std::string, CCanDispatchInfo> CanDispatchInfoMap;
}