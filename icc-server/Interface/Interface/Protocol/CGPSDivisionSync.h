#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGPSDivisionSync :
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
				{// 获取
					p_pJson->SetNodeValue("/body/data/callref_id", m_oBody.m_strCallrefID);
					p_pJson->SetNodeValue("/body/data/phone_no", m_oBody.m_strPhone);
				}
				else
				{// 设置
					p_pJson->SetNodeValue("/body/data/phone_pre", m_oBody.m_strPhonePre);
				}
				p_pJson->SetNodeValue("/body/data/province", m_oBody.m_strProvince);
				p_pJson->SetNodeValue("/body/data/city", m_oBody.m_strCity);
				p_pJson->SetNodeValue("/body/data/operator", m_oBody.m_strOperator);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;				
				std::string m_strProvince;
				std::string m_strCity;
				std::string m_strOperator;
				std::string m_strPhone;			// 获取时传入号码全称
				std::string m_strPhonePre;		// 设置时传入号码前缀
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
