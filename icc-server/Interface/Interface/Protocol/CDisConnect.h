#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//************************************
		// Class:  CDisConnect
		// Brief:  MQ服务通知监控服务MQ客户端的断开，
		//		   不包括在请求应答中模式中
		//************************************
		class CDisConnect :
			public IRequest
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
				//m_strClientID = p_pJson->GetNodeValue("/body/ClientId", "");
				//m_strClientIP = p_pJson->GetNodeValue("/body/ClientIp", "");
				m_strConnectID = p_pJson->GetNodeValue("/header/ConnectionId", "");
				return true;
			}

		public:
			std::string m_strClientID;
			std::string m_strClientIP;
			std::string m_strConnectID;
		};
	}
}
