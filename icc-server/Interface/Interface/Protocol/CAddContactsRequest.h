#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		class CAddContactsRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_guid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_staff_code = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_contact_number = p_pJson->GetNodeValue("/body/contact_number", "");
				m_oBody.m_contact_name = p_pJson->GetNodeValue("/body/contact_name", "");
				m_oBody.m_contact_dept = p_pJson->GetNodeValue("/body/contact_dept", "");
				m_oBody.m_contact_number2 = p_pJson->GetNodeValue("/body/contact_number2", "");
				m_oBody.m_contact_number3 = p_pJson->GetNodeValue("/body/contact_number3", "");
				m_oBody.m_contact_number4 = p_pJson->GetNodeValue("/body/contact_number4", "");
				m_oBody.m_contact_guid = p_pJson->GetNodeValue("/body/contact_guid","");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_guid;
				std::string m_staff_code;
				std::string m_contact_number;
				std::string m_contact_name;
				std::string m_contact_dept;
				std::string m_contact_number2;
				std::string m_contact_number3;
				std::string m_contact_number4;
				std::string m_contact_guid;
			};
			CBody m_oBody;
		};
	}
}

