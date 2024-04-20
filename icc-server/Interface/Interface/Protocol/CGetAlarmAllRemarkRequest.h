#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
    namespace PROTOCOL
    {
        class CGetAlarmAllRemarkRequest :
            public IRequest
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/body/alarm_id", "");
                return true;
            }

        public:
            CHeaderEx m_oHeader;
            class CBody
            {
            public:
                std::string m_strAlarmId;
            };
            CBody m_oBody;
        };
    }
}
