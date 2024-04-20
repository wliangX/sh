#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynPackages : public IReceive
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

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");
				m_strPackageCount = p_pJson->GetNodeValue("/data/packageCount", "");
				m_strMaxVersion = p_pJson->GetNodeValue("/data/maxVersion", "");
				return true;
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strPackageCount;
			std::string m_strMaxVersion;
		};
	}
}
