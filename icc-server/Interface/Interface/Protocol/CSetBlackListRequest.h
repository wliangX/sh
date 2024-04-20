#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBlackListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimit_Num = p_pJson->GetNodeValue("/body/limit_num", "");
				m_oBody.m_strLimit_Minute = p_pJson->GetNodeValue("/body/limit_minute", "");
				m_oBody.m_strLimit_Reason = p_pJson->GetNodeValue("/body/limit_reason", "");
				m_oBody.m_strStaff_Code = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaff_Name = p_pJson->GetNodeValue("/body/staff_name", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strLimit_Num;
				std::string m_strLimit_Minute;
				std::string m_strLimit_Reason;
				std::string m_strStaff_Code;
				std::string m_strStaff_Name;
			};
			CBody m_oBody;
		};
	}
}
