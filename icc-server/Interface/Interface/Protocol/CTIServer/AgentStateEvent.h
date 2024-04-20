#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAgentStateEvent :
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

				p_pJson->SetNodeValue("/body/agent", m_oBody.m_strAgent);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strACD);
				p_pJson->SetNodeValue("/body/login_mode", m_oBody.m_strLoginMode);
				p_pJson->SetNodeValue("/body/ready_state", m_oBody.m_strReadyState);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				if (!m_oBody.m_strMinOnlineNum.empty())
				{
					p_pJson->SetNodeValue("/body/min_online_agent_num", m_oBody.m_strMinOnlineNum);
				}
				if (!m_oBody.m_strCurrOnlineNum.empty())
				{
					p_pJson->SetNodeValue("/body/current_online_agent_num", m_oBody.m_strCurrOnlineNum);
				}
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strACD = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				m_oBody.m_strMinOnlineNum = p_pJson->GetNodeValue("/body/min_online_agent_num", "");
				m_oBody.m_strCurrOnlineNum = p_pJson->GetNodeValue("/body/current_online_agent_num", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strAgent;
				std::string m_strACD;
				std::string m_strLoginMode;
				std::string m_strReadyState;
				std::string m_strTime;

				std::string m_strMinOnlineNum; //最小在线坐席数
				std::string m_strCurrOnlineNum;//当前在线坐席数
			};
			CBody m_oBody;
		};
	}
}
