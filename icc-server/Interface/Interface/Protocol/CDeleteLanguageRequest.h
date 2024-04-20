#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteLanguageRequest :
			public IRequest,IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
			};
			CBody m_oBody;
		};
	}
}
