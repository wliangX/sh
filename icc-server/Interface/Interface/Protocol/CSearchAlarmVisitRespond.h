#pragma once
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmVisitRespond :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_vecData.at(i).m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_vecData.at(i).m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", m_oBody.m_vecData.at(i).m_strCallrefID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", m_oBody.m_vecData.at(i).m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_vecData.at(i).m_strCallerNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_vecData.at(i).m_strCallerName);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_vecData.at(i).m_strTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "visitor_code", m_oBody.m_vecData.at(i).m_strVisitorCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "visitor_name", m_oBody.m_vecData.at(i).m_strVisitorName);
					p_pJson->SetNodeValue(l_strPrefixPath + "satisfaction", m_oBody.m_vecData.at(i).m_strSatisfaction);
					p_pJson->SetNodeValue(l_strPrefixPath + "suggest", m_oBody.m_vecData.at(i).m_strSuggest);
					p_pJson->SetNodeValue(l_strPrefixPath + "result_content", m_oBody.m_vecData.at(i).m_strResultContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "call_type", m_oBody.m_vecData.at(i).m_strCallType);
					p_pJson->SetNodeValue(l_strPrefixPath + "reason", m_oBody.m_vecData.at(i).m_strReason);
                }
				return p_pJson->ToString();
			}

		public:
            class CData
            {
            public:
				std::string m_strID;								//�طõ�ID
				std::string m_strAlarmID;							//����ID
				std::string m_strCallrefID;							//����ID���طõ绰�Ļ���id��
				std::string m_strDeptCode;							//���������λ����
				std::string m_strDeptName;							//���������λ����
				std::string m_strCallerNo;							//�����˺���
				std::string m_strCallerName;						//����������
				std::string m_strTime;								//�ط�ʱ��
				std::string m_strVisitorCode;						//�ط��˱���
				std::string m_strVisitorName;						//�ط�������
				std::string m_strSatisfaction;						//����� 0�����⣬1�������⣬Ĭ��0
				std::string m_strSuggest;							//����
				std::string m_strResultContent;						//����������
				std::string m_strCallType;							//��������
				std::string m_strReason;							//ԭ��
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
