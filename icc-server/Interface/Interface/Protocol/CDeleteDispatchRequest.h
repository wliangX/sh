#pragma once 
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteDispatchRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuid = p_pJson->GetNodeValue("body/guid", "");
				return true;
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
