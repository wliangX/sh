#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmAssignRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strSrcSeat = p_pJson->GetNodeValue("/body/src_seat", "");
				m_oBody.m_strDescSeat = p_pJson->GetNodeValue("/body/desc_seat", "");
				
				return true;
			}
			
		public:
			
			class CBody
			{
			public:
				std::string		m_strAlarmID;	//警情ID
				std::string		m_strSrcSeat;	//接警席
				std::string		m_strDescSeat;//处警单分配的席位，可为空
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};



	}
}
