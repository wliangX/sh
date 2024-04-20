#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//************************************
		// Class:  CConnect
		// Brief:  MQ服务通知监控服务MQ客户端的连接，
		//		   不包括在请求应答中模式中
		//************************************
		class CConnect :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_strClientID = p_pJson->GetNodeValue("/header/ClientId", "");
				m_strClientIP = p_pJson->GetNodeValue("/header/ClientIp", "");
				m_strConnectID = p_pJson->GetNodeValue("/header/ConnectionId", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				std::string l_str;
				if (p_pJson)
				{
					p_pJson->SetNodeValue("/header/ClientId", m_strClientID);
					p_pJson->SetNodeValue("/header/ClientIp", m_strClientIP);
					p_pJson->SetNodeValue("/header/ConnectionId", m_strConnectID);
					l_str = p_pJson->ToString();
				}
				return l_str;
			}

		public:
			std::string m_strClientID;
			std::string m_strClientIP;
			std::string m_strConnectID;
		};

		class CAcdAgentState
		{
		public:
			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strAcd = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				return true;
			}
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strAgent;		  //座机号
				std::string m_strAcd;
				std::string m_strLoginMode;   //login、logout
				std::string m_strReadyState;  //忙闲状态
				std::string m_strTime;
			};
			CBody m_oBody;
		};
	}
}
