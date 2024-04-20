#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CQueryFreeSeatRequest:
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_type = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_seat_nos = p_pJson->GetNodeValue("/body/seat_nos", "");
				m_oBody.m_dept_code = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_district_code = p_pJson->GetNodeValue("/body/district_code", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_type;
				std::string m_seat_nos; //1：指定席位号查询；2：按部门查询；3：按行政区划查询
				std::string m_dept_code;
				std::string m_district_code;

			};
			CBody m_oBody;
		};
	}
}
