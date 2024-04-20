#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBJRDHTokenRequest : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{		
				if (nullptr == p_pJson)
				{
					return "";
				}
				
				p_pJson->SetNodeValue("/appid", m_strAppID);
				p_pJson->SetNodeValue("/secretkey", m_strPassword);

				return p_pJson->ToString();
			}

		public:
			std::string m_strAppID;
			std::string m_strPassword;
		};
	}
}
