#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGPSPhoneInfoSync :
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

				if (!m_oBody.m_strCallrefID.empty())
				{// 如果话务id不为空，走获取同步流程（包含callref_id）
					p_pJson->SetNodeValue("/body/data/callref_id", m_oBody.m_strCallrefID);
				}
				p_pJson->SetNodeValue("/body/data/phone_no", m_oBody.m_strPhoneNo);
				p_pJson->SetNodeValue("/body/data/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/data/address", m_oBody.m_strAddress);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;
				std::string m_strPhoneNo;
				std::string m_strName;
				std::string m_strAddress;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
