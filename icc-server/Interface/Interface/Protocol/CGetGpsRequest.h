#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGetGpsRequest :
            public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
                m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/phone_no", "");                
				return true;
			}
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
                if (p_pJson == nullptr)
                {
                    return "";
                }
                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
                p_pJson->SetNodeValue("/body/phone_no", m_oBody.m_strPhoneNo);
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strCallrefID;
                std::string m_strPhoneNo;
			};
			CBody m_oBody;
		};
	}
}
