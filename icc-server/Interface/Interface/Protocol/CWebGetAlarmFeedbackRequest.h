#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"

namespace ICC
{
	namespace PROTOCOL
	{
        class CWebGetAlarmFeedBackRequest :
			public IReceive, public ISend
		{
        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/token", m_oBody.m_strToken);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);

				return p_pJson->ToString();
			}
			
		public:				
			class CBody
			{
			public:
				std::string m_strToken;								//ÊÚÈ¨Âë
				std::string m_strAlarmID;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
