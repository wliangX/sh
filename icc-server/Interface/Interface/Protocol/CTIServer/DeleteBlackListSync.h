#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteBlackListSync :
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
				p_pJson->SetNodeValue("/body/limit_type", m_oBody.m_strLimitType);
				p_pJson->SetNodeValue("/body/limit_num", m_oBody.m_strLimitNum);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimitType = p_pJson->GetNodeValue("/body/limit_type", "");
				m_oBody.m_strLimitNum = p_pJson->GetNodeValue("/body/limit_num", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strLimitType;
				std::string m_strLimitNum;
			};
			CBody m_oBody;
		};
	}
}
