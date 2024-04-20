#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CClientRegisterRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strClientID = p_pJson->GetNodeValue("/body/client_id", "");
				m_oBody.m_strClientType = p_pJson->GetNodeValue("/body/client_type", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name","");
				m_oBody.m_strIP = p_pJson->GetNodeValue("/body/client_ip", "");


				m_oBody.m_strDistrictCode = p_pJson->GetNodeValue("/body/district_code", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");

				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");
				m_oBody.m_strStaffDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strStaffDeptName = p_pJson->GetNodeValue("/body/dept_name", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strDistrictCode; //所在行政区划编号
				std::string m_strReceiptCode; //接警员编号
				std::string m_strReceiptName; //接警员姓名

				std::string m_strClientID;
				std::string m_strClientType;
				std::string m_strClientName;
				std::string m_strIP;

				std::string m_strStaffCode;
				std::string m_strStaffName;
				std::string m_strStaffDeptCode;
				std::string m_strStaffDeptName;
			};
			CBody m_oBody;
		};


		class CDeleteClientRegisterRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strClientID = p_pJson->GetNodeValue("/body/client_id", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strClientID;
			};
			CBody m_oBody;
		};
	}
}
