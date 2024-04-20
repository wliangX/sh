#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynListResult : public IReceive
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
				m_strCurrent = p_pJson->GetCount("/data/current");
				m_strTotalPage = p_pJson->GetCount("/data/pages");

				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; ++i)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					vecDatas.push_back(p_pJson->GetNodeValue(l_strPrefixPath, ""));
				}

				return true;
			}

			virtual bool ParseStringEx(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
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
				m_strData = p_pJson->GetNodeValue("/data", "");

				return true;
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strCurrent;
			std::string m_strTotalPage;
			std::string m_strData;
			std::vector<std::string> vecDatas;
		};
	}
}
