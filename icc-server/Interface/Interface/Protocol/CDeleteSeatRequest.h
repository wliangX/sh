#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
        class CDeleteSeatRequest :
            public IRequest
		{
        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strNo = p_pJson->GetNodeValue("/body/no", "");
               
                return true;
            }

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
                std::string m_strNo;
			};
			CBody m_oBody;
		};
	}
}
