#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CProcessDoneRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				//m_oBody.m_strProcessID = p_pJson->GetNodeValue("/body/process_id", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				
				return true;
			}
			
		public:
			
			class CBody
			{
			public:
				std::string		m_strAlarmID;	//警情ID
				//std::string		m_strProcessID; //处警单
				std::string		m_strSeatNo;	//坐席号
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};


		class CProcessAgainRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");

				return true;
			}

		public:

			class CBody
			{
			public:
				std::string		m_strAlarmID;	//警情ID
				std::string		m_strSeatNo;	//坐席号
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}
