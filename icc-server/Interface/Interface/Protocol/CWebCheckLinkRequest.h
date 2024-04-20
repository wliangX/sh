#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebCheckLinkRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strServerGuid = p_pJson->GetNodeValue("/body/server_guid", "");
				m_oBody.m_strWebiccServerCurrentTime = p_pJson->GetNodeValue("/body/webicc_server_currenttime", "");

				return true;
			}
		
			class CBody
			{
			public:
				std::string m_strServerGuid;				//Web后台服务guid
				std::string m_strWebiccServerCurrentTime;	//We后台服务系统时间	
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
