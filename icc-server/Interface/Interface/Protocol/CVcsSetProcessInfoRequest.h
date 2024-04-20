#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetProcessInfoRequest :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strId = p_pJson->GetNodeValue("/body/id", "");
                m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
                m_oBody.m_strProcessFeedback = p_pJson->GetNodeValue("/body/process_feedback", "");                
                return true;
            }			

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strId;
				std::string m_strCaseID;
                std::string m_strProcessFeedback;                
			};
			CBody m_oBody;
		};
	}
}
