#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CPhoneInfoSync :
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

                p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
                p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
                p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strSyncType;
                std::string m_strPhone;
                std::string m_strAddress;
                std::string m_strName;
			};
			CBody m_oBody;
		};
	}
}
