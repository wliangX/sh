#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CClientRegisterRespond :
			public IRespond
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
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/client_type", m_oBody.m_strClientType);
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strIP);

				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/body/staff_name", m_oBody.m_strStaffName);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:

				std::string m_strClientID;
				std::string m_strClientType;
				std::string m_strClientName;
				std::string m_strIP;

				std::string m_strUserName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strStaffCode;
				std::string m_strStaffName;
			};
			CBody m_oBody;
		};
	}
}
