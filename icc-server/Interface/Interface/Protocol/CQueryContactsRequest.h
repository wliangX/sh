#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		class CQueryContactsRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_staff_code = p_pJson->GetNodeValue("/body/staff_code", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_staff_code;
			};
			CBody m_oBody;
		};
	}
}

