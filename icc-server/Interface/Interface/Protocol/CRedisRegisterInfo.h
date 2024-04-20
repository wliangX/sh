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
		std::string ToJson(JsonParser::IJsonPtr p_pJson)
		{
			std::string l_strRegisterInfo("");
			if (p_pJson)
			{
				p_pJson->SetNodeValue("/client_id", m_strClientID);
				p_pJson->SetNodeValue("/client_name", m_strClientName);
				p_pJson->SetNodeValue("/client_type", m_strClientType);
				p_pJson->SetNodeValue("/client_ip", m_strClientIP);

				p_pJson->SetNodeValue("/user_name", m_strUserName);
				p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
				p_pJson->SetNodeValue("/dept_name", m_strDeptName);
				p_pJson->SetNodeValue("/seat_type", m_strSeatType);
				p_pJson->SetNodeValue("/staff_code", m_strStaffCode);
				p_pJson->SetNodeValue("/staff_name", m_strStaffName);


				p_pJson->SetNodeValue("/heartbeat_time", m_strHeartTime);

				p_pJson->SetNodeValue("/district_code", m_strDistrictCode);
				l_strRegisterInfo = p_pJson->ToString();
			}
			return l_strRegisterInfo;
		}

		bool Parse(std::string p_strRegisterInfo, JsonParser::IJsonPtr p_pJson)
		{
			if (!p_pJson || !p_pJson->LoadJson(p_strRegisterInfo))
			{
				return false;
			}

			m_strClientID = p_pJson->GetNodeValue("/client_id", "");
			m_strClientName = p_pJson->GetNodeValue("/client_name", "");
			m_strClientType = p_pJson->GetNodeValue("/client_type", "");
			m_strClientIP = p_pJson->GetNodeValue("/client_ip", "");

			m_strUserName = p_pJson->GetNodeValue("/user_name", "");
			m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
			m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
			m_strSeatType = p_pJson->GetNodeValue("/seat_type", "");
			m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
			m_strStaffName = p_pJson->GetNodeValue("/staff_name", "");

			m_strHeartTime = p_pJson->GetNodeValue("/heartbeat_time", "");

			m_strDistrictCode = p_pJson->GetNodeValue("/district_code", "");
			return true;
		}

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