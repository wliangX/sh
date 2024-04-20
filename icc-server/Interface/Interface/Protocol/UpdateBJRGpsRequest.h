#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		class CUpdateBJRGpsRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_alarm_id = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_bjr_gps_x = p_pJson->GetNodeValue("/body/bjr_gps_x", "");
				m_oBody.m_bjr_gps_y = p_pJson->GetNodeValue("/body/bjr_gps_y", "");
				m_oBody.m_bjr_dz = p_pJson->GetNodeValue("/body/bjr_dz", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_alarm_id;
				std::string m_bjr_gps_x;
				std::string m_bjr_gps_y;
				std::string m_bjr_dz;
			};
			CBody m_oBody;
		};
	}
}

