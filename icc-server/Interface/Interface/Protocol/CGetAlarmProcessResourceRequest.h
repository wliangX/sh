#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmProcLogResRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strResourceID = p_pJson->GetNodeValue("/body/resource_id", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strResourceID;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
