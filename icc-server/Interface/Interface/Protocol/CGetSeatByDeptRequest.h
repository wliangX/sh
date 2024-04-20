#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetSeatByDeptRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
                return true;
            }

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
                std::string m_strDeptCode;
			};
			CBody m_oBody;
		};
	}
}
