#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsReceiveAlarmRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				
				unsigned int l_uiIndex = 0;
				for (const CData& data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", data.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_count", data.m_strReceiveAlarmCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_count", data.m_strProcessCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_count", data.m_strFeedbackCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "real_alarm_count", data.m_strRealAlarmCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "call_alarm_count", data.m_strCallAlarmCount);					

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strDeptCode;					//���Ŵ���
				std::string	m_strDeptName;					//��������
				std::string m_strReceiveAlarmCount;			//�Ӿ�����
				std::string m_strProcessCount;				//����������
				std::string m_strFeedbackCount;				//����������			
				std::string m_strRealAlarmCount;			//��Ч��������
				std::string m_strCallAlarmCount;			//�绰��������				
			};
			class CBody
			{
			public:
				std::string m_strCount;					//��ǰ����¼������
				std::string m_strAllCount;		//��������������
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}