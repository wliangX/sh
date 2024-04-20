#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CTelHangupRespond :
			public IRespond
		{
		public:
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/caseid", m_oBody.m_strCaseID);
                p_pJson->SetNodeValue("/body/callerno", m_oBody.m_strCallerId);
                p_pJson->SetNodeValue("/body/calledno", m_oBody.m_strCalledId);
                p_pJson->SetNodeValue("/body/hanguptime", m_oBody.m_strHangupTime);

                p_pJson->SetNodeValue("/body/userid", m_oBody.m_strSeatNo);

                p_pJson->SetNodeValue("/body/call_id", m_oBody.m_strCallID);
                return p_pJson->ToString();
            }


		public:
			CHeader m_oHeader;
            class CBody
            {
            public: 
                std::string m_strCaseID;
                std::string m_strCallerId;
                std::string m_strCalledId;
                std::string m_strHangupTime;
                std::string m_strSeatNo;

                std::string m_strCallID;
                
            };
			CBody m_oBody;
		};
	}
}
