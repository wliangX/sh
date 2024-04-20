#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsGetLocationRequest :
			public IReceive
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
