#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmCountRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/total_alarm_count", m_oBody.m_strTotalAlarmCount);
				p_pJson->SetNodeValue("/body/total_invalid_alarm_count", m_oBody.m_strTotalInvalidAlarmCount);

				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "user_code", data.m_strUserCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_count", data.m_strAlarmCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "invalid_alarm_count", data.m_strInvalidAlarmCount);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strUserCode;				//�û��ʺ�
				std::string	m_strAlarmCount;			//������
				std::string m_strInvalidAlarmCount;		//��Ч������
			};
			class CBody
			{
			public:
				std::string m_strCount;					//�ְ�����
				std::string m_strTotalAlarmCount;		//��������
				std::string m_strTotalInvalidAlarmCount;//��Ч��������
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}