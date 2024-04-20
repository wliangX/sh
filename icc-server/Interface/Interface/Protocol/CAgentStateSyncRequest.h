#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CSetInOutInfo :
            public IRequest
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
               if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                m_oBody.m_strClient = p_pJson->GetNodeValue("/body/agent", "");
                m_oBody.m_strACD = p_pJson->GetNodeValue("/body/acd", "");
                m_oBody.m_strLogMode = p_pJson->GetNodeValue("/body/login_mode", "");
                m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
             
                return true;
            }

        public:

            class CBody
            {
            public:
                std::string m_strClient;				//分机号
                std::string m_strACD;					//ACD 组名称（110、119、120、122）
                std::string m_strLogMode;				//Login/Logout 状态
                std::string m_strReadyState;			//Busy/Idle 状态
                std::string m_strTime;					//状态时间
            };
            CHeader m_oHeader;
            CBody	m_oBody;
        };
    }
}

