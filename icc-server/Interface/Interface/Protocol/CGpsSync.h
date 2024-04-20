#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGpsSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/data/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/data/phone_no", m_oBody.m_strPhoneNo);
				p_pJson->SetNodeValue("/body/data/longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue("/body/data/latitude", m_oBody.m_strLatitude);
				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/data/callref_id", "");
                m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/data/phone_no", "");
                m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/data/longitude", "");
                m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/data/latitude", "");
                return true;
            }
		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;
				std::string m_strPhoneNo;
				std::string m_strLongitude;
				std::string m_strLatitude;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
