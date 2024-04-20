#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetProcessCashRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				
				return true;
			}
			
		public:
			
			class CBody
			{
			public:
				std::string		m_strSeatNo;	//��ϯ��
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
