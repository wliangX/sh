#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CClientRegisterSync :
			public ISync, public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/client_id", m_oBody.m_strClientID);
				p_pJson->SetNodeValue("/body/client_type", m_oBody.m_strClientType);
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/client_status", m_oBody.m_strClientStatus);
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strClientIP);


				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/body/staff_name", m_oBody.m_strStaffName);

				p_pJson->SetNodeValue("/body/district_code", m_oBody.m_strDistrictCode);
				
				p_pJson->SetNodeValue("/body/logout_case", m_oBody.m_strLogoutCase);

				return p_pJson->ToString();
			};
			bool ParseString(std::string p_strRegisterInfo, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(p_strRegisterInfo))
				{
					return false;
				}

				m_oBody.m_strClientID = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientType = p_pJson->GetNodeValue("/body/client_type", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strClientStatus = p_pJson->GetNodeValue("/body/client_status", "");
				m_oBody.m_strClientIP = p_pJson->GetNodeValue("/body/client_ip", "");

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");

				m_oBody.m_strDistrictCode = p_pJson->GetNodeValue("/body/district_code", "");

				m_oBody.m_strLogoutCase = p_pJson->GetNodeValue("/body/logout_case", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strClientID;
				std::string m_strClientType;
				std::string m_strClientName;
				std::string m_strClientStatus;
				std::string m_strClientIP;

				std::string m_strDistrictCode; //所在行政区划编号

				std::string m_strDeptCode;		//用户所属的单位编码
				std::string m_strDeptName;		//用户所属的单位名称
				//std::string m_strSeatType;		//席位类型，默认暂定是接处警席
				std::string m_strStaffCode;		//警员编码
				std::string m_strStaffName;		//警员姓名

				std::string m_strLogoutCase;

			};
			CBody m_oBody;
		};

#define CLIENT_ONLINE "0"
#define Client_Offline "1"

	}
}
