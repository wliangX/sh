#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CSetPhoneInfoRequest :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
                return true;
            }

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strPhone;
				std::string m_strName;
				std::string m_strAddress;
			};
			CBody m_oBody;
		};
	}
}
