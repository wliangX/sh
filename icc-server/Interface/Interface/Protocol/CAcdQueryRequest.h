#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include "Protocol/CAcdQueryRequest.h"

namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_acd = p_pJson->GetNodeValue("/body/acd", "");

				
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_acd;
			};
			CBody m_oBody;
		};
	}
}

