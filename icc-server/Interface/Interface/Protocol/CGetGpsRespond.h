#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CGetGpsRespond :
            public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
                return true;
            }
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
                return p_pJson->ToString();
            }

        public:
            CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strResult;
            };
            CBody m_oBody;
        };
    }
}
