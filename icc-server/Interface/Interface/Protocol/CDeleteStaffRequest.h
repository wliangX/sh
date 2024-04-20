#pragma once 
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteStaffRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strStaffGuid = p_pJson->GetNodeValue("/body/guid", "");
				return true;
			}
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strStaffGuid);
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strStaffGuid;
			};
			CBody m_oBody;
		};
	}
}
