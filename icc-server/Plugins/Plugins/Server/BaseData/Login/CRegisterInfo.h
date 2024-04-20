#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	// 用户注册信息
	// 添加了接触警分离所需的字段
	// [6/23/2020 151116314]
	class CRegisterInfo
	{
	public:
		CRegisterInfo();
		~CRegisterInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strDistrictCode; //所在行政区划编号

		std::string m_strClientID;		//MQClientID，对于客户端，这里是席位号
		std::string m_strClientName;	//对于登录的客户端，是UserCode
		std::string m_strClientType;
		std::string m_strClientIP;

		std::string  m_strHeartTime;    //心跳时间

		// 以下是新增字段				[6/23/2020 151116314]
		std::string m_strUserName;		//对于登录的客户端，是UserName
		std::string m_strDeptCode;		//用户所属的单位编码
		std::string m_strDeptName;		//用户所属的单位名称
		std::string m_strSeatType;		//席位类型，默认暂定是接处警席
		std::string m_strStaffCode;		//警员编码
		std::string m_strStaffName;		//警员姓名
	};

}