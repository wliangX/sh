#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmIDRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");
				
                return true;
			}
		
		public:
			class CBody
			{
			public:
				std::string m_strReceiptDeptDistrictCode;   //坐席所处的行政区划
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
			
		};	
	}
}
