#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSynBaseDataResultNotify :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSynResult = p_pJson->GetNodeValue("/body/synresult", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/synresult", m_oBody.m_strSynResult);

				return p_pJson->ToString();
			}
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:							;
				std::string m_strSynResult;
			};
			CBody m_oBody;
		};
	}

}
