#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CUploadConfigRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strIp = p_pJson->GetNodeValue("/body/ip", "");
				m_oBody.m_strConfig = p_pJson->GetNodeValue("/body/config", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strIp;
				std::string m_strConfig;
			};
			CBody m_oBody;
		};
	}
}
