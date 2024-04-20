/*
坐席默认地址信息
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSeatDataInfo : public IRespond, public IRequest
		{
		public:
			class CBody
			{
			public:
				std::string m_strAcd;
				std::string m_strBeginTalkTime;
				std::string m_strCallDirection;
				std::string m_strCalledId;
				std::string m_strCallerId;
				std::string m_strCallrefId;
				std::string m_strOriginalCallId;
				std::string m_strDeviceType;
				std::string m_strTime;
				std::string m_strSeatNo;			//坐席号
				std::string m_strClientName;		//警员用户名称
				std::string m_strClientIp;			//警员IP
				std::string m_strStaffCode;			//警员警号
				std::string m_strStaffName;			//警员名字
				std::string m_strDeptCode;			//部门编码
				std::string m_strDeptName;			//部门名称
				std::string m_strLoginMode;			//登陆状态
				std::string m_strReadyState;		//话机状态
				std::string m_strDeviceState;		//通话状态
				std::string m_strAlarmCount;		//今日接警数量
				std::string m_strStaffCount;		//今日警员接警数量
				std::string m_strSeatType;			// 坐席类型
				std::string m_strIsRecursive;		// 是否包含下级 0 为不包含下级部门 1 为包含下级部门
				std::string m_strOrgIndentifier;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		public:
			//redis存取，解析使用
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/acd", m_oBody.m_strAcd);
				p_pJson->SetNodeValue("/begin_talk_time", m_oBody.m_strBeginTalkTime);
				p_pJson->SetNodeValue("/call_direction", m_oBody.m_strCallDirection);
				p_pJson->SetNodeValue("/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/callref_id", m_oBody.m_strCallrefId);
				p_pJson->SetNodeValue("/original_called_id", m_oBody.m_strOriginalCallId);
				p_pJson->SetNodeValue("/device_type", m_oBody.m_strDeviceType);
				p_pJson->SetNodeValue("/time", m_oBody.m_strTime);

				p_pJson->SetNodeValue("/client_id", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/client_ip", m_oBody.m_strClientIp);
				p_pJson->SetNodeValue("/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/staff_name", m_oBody.m_strStaffName);
				p_pJson->SetNodeValue("/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/login_mode", m_oBody.m_strLoginMode);
				p_pJson->SetNodeValue("/ready_state", m_oBody.m_strReadyState);
				p_pJson->SetNodeValue("/device_state", m_oBody.m_strDeviceState);
				p_pJson->SetNodeValue("/alarm_count", m_oBody.m_strAlarmCount);
				p_pJson->SetNodeValue("/staff_count", m_oBody.m_strStaffCount);
				p_pJson->SetNodeValue("/seat_type", m_oBody.m_strSeatType);
				return p_pJson->ToString();
			}

			std::string ToClient(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strAcd);
				p_pJson->SetNodeValue("/body/begin_talk_time", m_oBody.m_strBeginTalkTime);
				p_pJson->SetNodeValue("/body/call_direction", m_oBody.m_strCallDirection);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalledId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerId);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefId);
				p_pJson->SetNodeValue("/body/original_called_id", m_oBody.m_strOriginalCallId);
				p_pJson->SetNodeValue("/body/device_type", m_oBody.m_strDeviceType);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/client_id", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strClientIp);
				p_pJson->SetNodeValue("/body/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/body/staff_name", m_oBody.m_strStaffName);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/login_mode", m_oBody.m_strLoginMode);
				p_pJson->SetNodeValue("/body/ready_state", m_oBody.m_strReadyState);
				p_pJson->SetNodeValue("/body/device_state", m_oBody.m_strDeviceState);
				p_pJson->SetNodeValue("/body/alarm_count", m_oBody.m_strAlarmCount);
				p_pJson->SetNodeValue("/body/staff_count", m_oBody.m_strStaffCount);
				p_pJson->SetNodeValue("/body/seat_type", m_oBody.m_strSeatType);
				p_pJson->SetNodeValue("/body/org_indentifier", m_oBody.m_strOrgIndentifier);
				return p_pJson->ToString();
			}
			// redis 用
			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strAcd = p_pJson->GetNodeValue("/acd", "");
				m_oBody.m_strBeginTalkTime = p_pJson->GetNodeValue("/begin_talk_time", "");
				m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/call_direction", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/called_id", "");
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/caller_id", "");
				m_oBody.m_strCallrefId = p_pJson->GetNodeValue("/callref_id", "");
				m_oBody.m_strOriginalCallId = p_pJson->GetNodeValue("/original_called_id", "");
				m_oBody.m_strDeviceType = p_pJson->GetNodeValue("/device_type", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/time", "");

				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/client_name", "");
				m_oBody.m_strClientIp = p_pJson->GetNodeValue("/client_ip", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/staff_name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/ready_state", "");
				m_oBody.m_strDeviceState = p_pJson->GetNodeValue("/device_state", "");
				m_oBody.m_strAlarmCount = p_pJson->GetNodeValue("/alarm_count", "");
				m_oBody.m_strStaffCount = p_pJson->GetNodeValue("/staff_count", "");
				m_oBody.m_strSeatType = p_pJson->GetNodeValue("/seat_type", "");
				return true;
			}

			bool ParseStringClient(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strClientIp = p_pJson->GetNodeValue("/body/client_ip", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strDeviceState = p_pJson->GetNodeValue("/body/device_state", "");
				m_oBody.m_strAlarmCount = p_pJson->GetNodeValue("/body/alarm_count", "");
				m_oBody.m_strStaffCount = p_pJson->GetNodeValue("/body/staff_count", "");
				m_oBody.m_strSeatType = p_pJson->GetNodeValue("/body/seat_type", "");
				m_oBody.m_strIsRecursive = p_pJson->GetNodeValue("/body/is_recursive", "");
				return true;
			}

			bool ParseStringRegist(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/client_name", "");
				m_oBody.m_strClientIp = p_pJson->GetNodeValue("/client_ip", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/staff_name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/ready_state", "");
				m_oBody.m_strDeviceState = p_pJson->GetNodeValue("/device_state", "");
				m_oBody.m_strAlarmCount = p_pJson->GetNodeValue("/alarm_count", "");
				m_oBody.m_strStaffCount = p_pJson->GetNodeValue("/staff_count", "");
				return true;
			}

			bool ParseStringSeatInfo(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/no", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				return true;
			}

			bool ParseStringPhoneState(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/FJH", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/XZQHBH", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/JJYBH", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/JJYXM", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/BMMC", "");
				std::string strPhoneState = p_pJson->GetNodeValue("/XWZT", "");
				if (strPhoneState == "0") {
					m_oBody.m_strLoginMode = "logout";
					return true;
				}
				if (strPhoneState == "1") {
					m_oBody.m_strLoginMode = "login";
				}
				if (strPhoneState == "2") {
					
					m_oBody.m_strDeviceState = "dialstate";
				}
				if (strPhoneState == "3") {
					m_oBody.m_strDeviceState = "hangupstate";
				}
				if (strPhoneState == "4") {
					m_oBody.m_strReadyState = "busy";
				}
				if (strPhoneState == "5") {
					m_oBody.m_strReadyState = "idle";
				}
				if (strPhoneState == "6") {
					m_oBody.m_strDeviceState = "ringstate";
				}
				if (strPhoneState == "7") {
					m_oBody.m_strDeviceState = "talkstate";
				}
				m_oBody.m_strLoginMode = "login";
				return true;
			}

		};	

		class CSeatDataInfoClient : public IRespond, public IRequest
		{
		public:
			class CBody
			{
			public:
				class CList {
				public:
					std::string m_strSeatNo;			//坐席号
					std::string m_strClientName;		//警员用户名称
					std::string m_strClientIp;			//警员IP
					std::string m_strStaffCode;			//警员警号
					std::string m_strStaffName;			//警员名字
					std::string m_strDeptCode;			//部门编码
					std::string m_strDeptName;			//部门名称
					std::string m_strLoginMode;			//登陆状态
					std::string m_strReadyState;		//话机状态
					std::string m_strDeviceState;		//通话状态
					std::string m_strAlarmCount;		//今日接警数量
					std::string m_strSatffCount;		//今日警员接警数量
					std::string m_strSeatType;			//坐席号类型
					std::string m_strAcd;
					std::string m_strBeginTalkTime;
					std::string m_strCallDirection;
					std::string m_strCalledId;
					std::string m_strCallerId;
					std::string m_strCallrefId;
					std::string m_strOriginalCallId;
					std::string m_strDeviceType;
					std::string m_strTime;
				};

			public:
				std::string m_strCount;					// 总数
				std::vector<CList> m_vecLists;			// 结果

			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		public:

			//redis存取，解析使用
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				int iCount = m_oBody.m_vecLists.size();
				for (int i = 0; i < iCount; ++i)
				{
					std::string l_strPrefixPath("/body/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/client_id", m_oBody.m_vecLists.at(i).m_strSeatNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/client_name", m_oBody.m_vecLists.at(i).m_strClientName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/client_ip", m_oBody.m_vecLists.at(i).m_strClientIp);
					p_pJson->SetNodeValue(l_strPrefixPath + "/staff_code", m_oBody.m_vecLists.at(i).m_strStaffCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/staff_name", m_oBody.m_vecLists.at(i).m_strStaffName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/dept_code", m_oBody.m_vecLists.at(i).m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/dept_name", m_oBody.m_vecLists.at(i).m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/login_mode", m_oBody.m_vecLists.at(i).m_strLoginMode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/ready_state", m_oBody.m_vecLists.at(i).m_strReadyState);
					p_pJson->SetNodeValue(l_strPrefixPath + "/device_state", m_oBody.m_vecLists.at(i).m_strDeviceState);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_count", m_oBody.m_vecLists.at(i).m_strAlarmCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "/staff_count", m_oBody.m_vecLists.at(i).m_strSatffCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "/seat_type", m_oBody.m_vecLists.at(i).m_strSeatType);
					p_pJson->SetNodeValue(l_strPrefixPath + "/acd", m_oBody.m_vecLists.at(i).m_strAcd);
					p_pJson->SetNodeValue(l_strPrefixPath + "/begin_talk_time", m_oBody.m_vecLists.at(i).m_strBeginTalkTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/call_direction", m_oBody.m_vecLists.at(i).m_strCallDirection);
					p_pJson->SetNodeValue(l_strPrefixPath + "/called_id", m_oBody.m_vecLists.at(i).m_strCalledId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/caller_id", m_oBody.m_vecLists.at(i).m_strCallerId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/callref_id", m_oBody.m_vecLists.at(i).m_strCallrefId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/original_called_id", m_oBody.m_vecLists.at(i).m_strOriginalCallId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/device_type", m_oBody.m_vecLists.at(i).m_strDeviceType);
					p_pJson->SetNodeValue(l_strPrefixPath + "/time", m_oBody.m_vecLists.at(i).m_strTime);
				}
				return p_pJson->ToString();

			}

			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				/*m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strClientIp = p_pJson->GetNodeValue("/body/client_ip", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strDeviceState = p_pJson->GetNodeValue("/body/device_state", "");
				m_oBody.m_strAlarmCount = p_pJson->GetNodeValue("/body/alarm_count", "");*/
				return true;
			}

		};

		
		class CStaffDataInfo : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/staff_count", m_oBody.m_StaffInfo.m_strStaffCount);
				p_pJson->SetNodeValue("/staff_code", m_oBody.m_StaffInfo.m_strStaffCode);
				p_pJson->SetNodeValue("/staff_name", m_oBody.m_StaffInfo.m_strStaffName);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_StaffInfo.m_strStaffCode = p_pJson->GetNodeValue("/staff_code", "");
				m_oBody.m_StaffInfo.m_strStaffName = p_pJson->GetNodeValue("/staff_name", "");
				m_oBody.m_StaffInfo.m_strStaffCount = p_pJson->GetNodeValue("/staff_count", "");

				return true;
			}


		public:
			CHeaderEx m_oHeader;
			class CStaffInfo
			{
			public:
				std::string m_strStaffCode;   //坐席号码
				std::string m_strStaffName;   //坐席号码
				std::string m_strStaffCount;  //警情数量
			};

			class CBody
			{
			public:
				std::vector<CStaffInfo> m_vecStaffCount;
				CStaffInfo m_StaffInfo;
			};
			CBody m_oBody;
		};


	}
}
