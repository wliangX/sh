#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmLogInfoRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strGuID = p_pJson->GetNodeValue("/body/guid", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strGuID;						//����ID
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
