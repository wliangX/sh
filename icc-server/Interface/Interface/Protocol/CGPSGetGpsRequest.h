#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGPSGetGpsRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/phone_no", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;			//话务ID
				std::string m_strPhoneNo;            //电话号码
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
