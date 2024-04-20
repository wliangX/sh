#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTipsInfo :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strTipsId = p_pJson->GetNodeValue("/tips_id", "");
				m_strCalledNoType = p_pJson->GetNodeValue("/called_no_type", "");
				m_strLevel = p_pJson->GetNodeValue("/level", "");
				m_strFirstType = p_pJson->GetNodeValue("/first_type", "");
				m_strContent = p_pJson->GetNodeValue("/tips_content", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/tips_id", m_strTipsId);
				p_pJson->SetNodeValue("/called_no_type", m_strCalledNoType);
				p_pJson->SetNodeValue("/level", m_strLevel);
				p_pJson->SetNodeValue("/first_type", m_strFirstType);
				p_pJson->SetNodeValue("/tips_content", m_strContent);

				return p_pJson->ToString();
			}

		public:
			std::string m_strTipsId;			//提示ID
			std::string m_strCalledNoType;		//报警号码类型
			std::string m_strLevel;				//警情级别
			std::string m_strFirstType;			//警情一级类型
			std::string m_strContent;			//提示内容
		};
	}
}