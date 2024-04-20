#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetLocationRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/x", m_oBody.m_strX);
				p_pJson->SetNodeValue("/body/y", m_oBody.m_strY);
				p_pJson->SetNodeValue("/body/vender_error", m_oBody.m_strVenderError);
				p_pJson->SetNodeValue("/body/description", m_oBody.m_strDescription);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strX;
				std::string m_strY;
				std::string m_strVenderError;
				std::string m_strDescription;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}