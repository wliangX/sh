#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CSetSeatRequest :
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
                m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
                m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
                return true;
            }

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strNo;
				std::string m_strName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
			};
			CBody m_oBody;
		};
	}
}
