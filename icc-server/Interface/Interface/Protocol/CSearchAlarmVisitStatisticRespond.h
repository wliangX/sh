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
				std::string m_strDeptCode;							//警情归属单位编码
				std::string m_strDeptName;							//警情归属单位名称
				unsigned int m_strAlarmCount;						//警情数
				unsigned int m_strVisitCount;						//回访数
				unsigned int m_strUnsatisfactoryCount;				//不满意数
				unsigned int m_strSatisfactoryCount;				//满意数
				double m_strSatisfaction;							//满意度
				double m_strVisitRate;								//回访率
            };

			class CBody
			{
			public:
				std::string m_strCount;								//分包总数
                std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
