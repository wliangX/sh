#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBJRDHTokenRespond : public IReceive
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

				m_strResult = p_pJson->GetNodeValue("/result", "");
				m_strToken = p_pJson->GetNodeValue("/token", "");

				return true;
			}

		public:
			std::string m_strResult;
			std::string m_strToken;
		};
	}
}
