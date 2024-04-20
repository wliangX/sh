#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteBlackListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimit_Num = p_pJson->GetNodeValue("/body/limit_num", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strLimit_Num;
			};
			CBody m_oBody;
		};
	}
}
