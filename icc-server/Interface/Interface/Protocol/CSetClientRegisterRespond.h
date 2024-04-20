#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetClientRegisterRespond :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");				

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strResult;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
