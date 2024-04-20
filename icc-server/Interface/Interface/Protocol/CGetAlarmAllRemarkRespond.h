#pragma once
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
        class CGetAlarmAllRemarkRespond :
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

                for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_vecData.at(i).m_strAlarmID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_code", m_oBody.m_vecData.at(i).m_strFeedBackCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_name", m_oBody.m_vecData.at(i).m_strFeedBackName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_code", m_oBody.m_vecData.at(i).m_strFeedBackDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_name", m_oBody.m_vecData.at(i).m_strFeedBackDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_time", m_oBody.m_vecData.at(i).m_strFeedBackTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_vecData.at(i).m_strContent);
                }
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
            class CData
            {
            public:
                std::string m_strAlarmID;				//����idid
                std::string m_strFeedBackCode;			//�����˱��
                std::string m_strFeedBackName;			//����������
                std::string m_strFeedBackDeptCode;		//������λ���
                std::string m_strFeedBackDeptName;		//������λ����
                std::string m_strFeedBackTime;			//����ʱ��
                std::string m_strContent;				//��ע����
            };

			class CBody
			{
			public:
                std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
