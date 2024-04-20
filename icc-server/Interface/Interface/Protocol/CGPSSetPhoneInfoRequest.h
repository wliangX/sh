#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CGPSSetPhoneInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/phone_no", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strPhoneNo;            //电话号码
				std::string m_strName;				 //装机人姓名
				std::string m_strAddress;            //装机地址
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
