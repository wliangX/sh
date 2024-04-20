#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBlackListSync :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/limit_num", m_oBody.m_strLimit_Num);
				p_pJson->SetNodeValue("/body/limit_minute", m_oBody.m_strlimit_Minute);
				p_pJson->SetNodeValue("/body/limit_reason", m_oBody.m_strLimit_Reason);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strLimit_Num;
				std::string m_strlimit_Minute;
				std::string m_strLimit_Reason;
			};
			CBody m_oBody;
		};
	}
}
