#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDBConnectionStatusSync :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/client_id", "ICC.Server.Dbagent");
				p_pJson->SetNodeValue("/body/client_type", "ctype_server_dbagent");
				p_pJson->SetNodeValue("/body/client_name", "dbagent");
				p_pJson->SetNodeValue("/body/client_ip", m_oBody.m_strClientIP);
				p_pJson->SetNodeValue("/body/client_status", m_oBody.m_strDBStatus);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strDBStatus; // 1连接成功，0连接失败
				std::string m_strClientIP;
			};

			CHeader m_oHeader;			
			CBody m_oBody;
		};
	}
}
