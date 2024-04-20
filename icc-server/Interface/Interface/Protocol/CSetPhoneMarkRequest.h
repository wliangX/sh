#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetPhoneMarkRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strPhone =   p_pJson->GetNodeValue("/body/phone","");
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strPhone;		
				std::string m_strGuid;     
				std::string m_strType;   
				std::string m_strStaffCode;
				std::string m_strRemark;
			};
			CBody m_oBody;
		};
	}
}
