#pragma once
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmVisitStatisticRespond :
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

                for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", m_oBody.m_vecData.at(i).m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_count", std::to_string(m_oBody.m_vecData.at(i).m_strAlarmCount));
					p_pJson->SetNodeValue(l_strPrefixPath + "visit_count", std::to_string(m_oBody.m_vecData.at(i).m_strVisitCount));
					p_pJson->SetNodeValue(l_strPrefixPath + "unsatisfactory_count", std::to_string(m_oBody.m_vecData.at(i).m_strUnsatisfactoryCount));
					p_pJson->SetNodeValue(l_strPrefixPath + "satisfactory_count", std::to_string(m_oBody.m_vecData.at(i).m_strSatisfactoryCount));
					p_pJson->SetNodeValue(l_strPrefixPath + "satisfaction", std::to_string(m_oBody.m_vecData.at(i).m_strSatisfaction));
					p_pJson->SetNodeValue(l_strPrefixPath + "visit_rate", std::to_string(m_oBody.m_vecData.at(i).m_strVisitRate));
                }
				return p_pJson->ToString();
			}

		public:
            class CData
            {
            public:
				std::string m_strDeptCode;							//���������λ����
				std::string m_strDeptName;							//���������λ����
				unsigned int m_strAlarmCount;						//������
				unsigned int m_strVisitCount;						//�ط���
				unsigned int m_strUnsatisfactoryCount;				//��������
				unsigned int m_strSatisfactoryCount;				//������
				double m_strSatisfaction;							//�����
				double m_strVisitRate;								//�ط���
            };

			class CBody
			{
			public:
				std::string m_strCount;								//�ְ�����
                std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
