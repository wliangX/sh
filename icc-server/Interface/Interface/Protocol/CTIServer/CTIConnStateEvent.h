#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCTIConnStateEvent :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
			//	p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/switch_type", m_oBody.m_strSwitchType);

				return p_pJson->ToString();
			}

			virtual bool ParseString(const std::string& strMsg, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(strMsg, p_pJson))
				{
					return false;
				};
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strSwitchType = p_pJson->GetNodeValue("/body/switch_type","");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strState;
				std::string m_strResult;
				std::string m_strSwitchType;
			};
			CBody m_oBody;
		};
	}
}
