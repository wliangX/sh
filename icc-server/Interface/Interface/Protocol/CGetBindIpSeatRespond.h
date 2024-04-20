#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//	简单的回复 成功/失败 可用此类
		class CGetBindIpSeatRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
				
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strSeatNo;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}