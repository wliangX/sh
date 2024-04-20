#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CGPSGetDivisionRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone_no", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;			//话务ID
				std::string m_strPhone;             //电话号码
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
