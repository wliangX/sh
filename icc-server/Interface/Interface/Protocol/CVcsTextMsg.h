#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsTextMsg : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
				m_oBody.m_strSender = p_pJson->GetNodeValue("/body/sender", "");
				m_oBody.m_strReceiver = p_pJson->GetNodeValue("/body/receiver", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");

                return true;
            }
		

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCaseID;
				std::string m_strSender;
				std::string m_strReceiver;
				std::string m_strContent;
			};
			CBody m_oBody;
		};
	}
}
