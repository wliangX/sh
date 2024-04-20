#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAStateSync
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strGuid;				//±¨¾¯Ö÷¼ü
				std::string m_strState;				//×´Ì¬£¬0Î´¶Á£¬1ÒÑ¶Á£¬2ÒÑ´´¾¯
			};
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
