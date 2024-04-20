#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//	简单的请求可用此类
		class CBaseRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strContent;			//请求的内容
				std::string m_strToken;
			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
