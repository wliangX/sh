#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpLogout : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{		
				if (nullptr == p_pJson)
				{
					return "";
				}
				
				p_pJson->SetNodeValue("/userId", m_strUser);
				p_pJson->SetNodeValue("/password", m_strPassword);
				p_pJson->SetNodeValue("/loginType", m_strLoginType);
				p_pJson->SetNodeValue("/systemCode", m_strSystemCode);

				return p_pJson->ToString();
			}

		public:
			std::string m_strUser;
			std::string m_strPassword;
			std::string m_strLoginType;
			std::string m_strSystemCode;
		};
	}
}
