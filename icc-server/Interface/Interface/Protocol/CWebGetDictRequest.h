#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetDictRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");
				m_oBody.m_strLangCode = p_pJson->GetNodeValue("/body/lang_code", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strToken;				//ÊÚÈ¨Âë
				std::string m_strLangCode;			//ÓïÑÔ±àÂë
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
