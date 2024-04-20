#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetChangeInfoRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strThridAlarmGuid = p_pJson->GetNodeValue("/body/third_alarm_guid", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strThridAlarmGuid;            //Ö÷¼ü
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
