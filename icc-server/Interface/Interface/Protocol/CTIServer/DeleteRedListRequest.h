#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteRedListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				//m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strPhoneNum = p_pJson->GetNodeValue("/body/phone_num", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				//std::string m_strGuid;	
				std::string m_strPhoneNum;
			};

			CBody m_oBody;
		};
	}
}
