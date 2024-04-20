#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetLocationRequest :
			public IReceive, ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strTelnum = p_pJson->GetNodeValue("/body/telnum", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/telnum", m_oBody.m_strTelnum);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strTelnum;            //µç»°ºÅÂë
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
