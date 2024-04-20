#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSyncNacosParams : public ISend, public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strNacosServerIp = p_pJson->GetNodeValue("/body/nacos_server_ip", "");
				m_oBody.m_strNacosServerPort = p_pJson->GetNodeValue("/body/nacos_server_port", "");
				m_oBody.m_strNacosNamespace = p_pJson->GetNodeValue("/body/nacos_namespace", "");
				m_oBody.m_strNacosGroupName = p_pJson->GetNodeValue("/body/nacos_groupname", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/nacos_server_ip", m_oBody.m_strNacosServerIp);
				p_pJson->SetNodeValue("/body/nacos_server_port", m_oBody.m_strNacosServerPort);
				p_pJson->SetNodeValue("/body/nacos_namespace", m_oBody.m_strNacosNamespace);
				p_pJson->SetNodeValue("/body/nacos_groupname", m_oBody.m_strNacosGroupName);
				return p_pJson->ToString();
			}
		
			class CBody
			{
			public:
				std::string m_strNacosServerIp;
				std::string m_strNacosServerPort;
				std::string m_strNacosNamespace;
				std::string m_strNacosGroupName;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
