#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynCmds : public IReceive
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
				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					std::string strValue = p_pJson->GetNodeValue(l_strPrefixPath, "");
					if (!strValue.empty())
					{
						m_mapCmds.insert(std::make_pair(strValue, strValue));
					}
					
				}
				return true;
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::map<std::string, std::string> m_mapCmds;
		};
	}
}
