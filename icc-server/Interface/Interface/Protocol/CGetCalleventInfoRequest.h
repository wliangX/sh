#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGetCallEventInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id","");
                return true;
            }

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCallrefID;
			};
			CBody m_oBody;
		};
	}
}
