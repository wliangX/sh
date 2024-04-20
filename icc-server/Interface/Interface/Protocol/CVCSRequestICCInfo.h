#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
    namespace PROTOCOL
    {
        class CVCSRequestICCInfo :
            public IRequest
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/updatetime", "");
                return true;
            }

        public:
            CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strUpdateTime;
            };
            CBody m_oBody;
        };
    }
}
