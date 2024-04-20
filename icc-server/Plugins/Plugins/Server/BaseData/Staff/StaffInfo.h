#pragma once
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CStaffInfo		
	{	
	public:
		CStaffInfo();
		~CStaffInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson = nullptr);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//警员guid
		std::string m_strDeptGuid;		//警员所在单位guid
		std::string m_strCode;			//警员编码
		std::string m_strName;			//警员姓名
		std::string m_strSex;			//警员性别
		std::string m_strType;			//警员类型
		std::string m_strPosition;		//警员职务
		std::string m_strMobile;		//警员手机
		std::string m_strPhone;			//警员座机
		std::string m_strShortcut;		//警员快捷查询编写			
		std::string m_strSort;			//警员序号，一般从1开始排序
		std::string m_strLevel;			//层级
		std::string m_strIsLeader;		//是否为领导,是：1，否：0
	};

	// 警员信息缓存，后期弃用，使用Redis缓存
	typedef std::map<std::string, CStaffInfo> StaffInfoMap;
}