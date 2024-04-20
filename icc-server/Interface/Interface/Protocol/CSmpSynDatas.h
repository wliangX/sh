#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmpSynDatas : public IReceive
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
				m_strCmd = p_pJson->GetNodeValue("/data/cmd", "");
				m_strData = p_pJson->GetNodeValue("/data", "");
				m_strCurrentPackageCount = p_pJson->GetNodeValue("/data/currentPackage", "");
				m_strStep = p_pJson->GetNodeValue("/data/step", "");
				m_strMaxVersion = p_pJson->GetNodeValue("/data/maxVersion", "");

				return true;
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strCmd;
			std::string m_strData;
			std::string m_strCurrentPackageCount;
			std::string m_strStep;
			std::string m_strMaxVersion;
		};
	}
}
