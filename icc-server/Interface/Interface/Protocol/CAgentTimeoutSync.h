#pragma once 
#include <Protocol/ISync.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAgentTimeoutSync :
			public ISync,public IReceive
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
				p_pJson->SetNodeValue("/body/client_name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strClientIp);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (NULL == p_pJson.get())
				{
					return false;
				}

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/client_name", "");
				m_oBody.m_strClientIp = p_pJson->GetNodeValue("/body/client_ip", "");
				
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strAgent;		        //分机号
				std::string m_strClientName;			    //客户端用户名
				std::string m_strClientIp;			//客户端登陆的IP
			};
			CBody m_oBody;
		};
	}
}
