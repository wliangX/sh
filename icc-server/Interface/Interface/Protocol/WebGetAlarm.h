#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarm :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/total_count", m_oBody.m_strTotalCount);
				p_pJson->SetNodeValue("/body/state_id", m_oBody.m_strStateID);

				unsigned int l_uiAlarmIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					std::string l_strAlarmPrefixPath("/body/alarm/" + std::to_string(l_uiAlarmIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarm.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", alarm.m_strTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", alarm.m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", alarm.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", alarm.m_strLevel);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", alarm.m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", alarm.m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", alarm.m_strThirdType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", alarm.m_strFourthType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", alarm.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", alarm.m_strContactNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_name", alarm.m_strContactName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", alarm.m_strReceiptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", alarm.m_strSourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", alarm.m_strSourceId);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", alarm.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", alarm.m_strReceiptDeptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_id", alarm.m_alarmProcess.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_state", alarm.m_alarmProcess.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time_signed", alarm.m_alarmProcess.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time_feedback", alarm.m_alarmProcess.m_strTimeFeedBack);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "process_dept_code", alarm.m_alarmProcess.m_strProcessDeptCode);

					l_uiAlarmIndex++;
				}

				return p_pJson->ToString();
			}
			public:
			class CAlarmProcess
			{
			public:
				std::string m_strID;						//����ID
				std::string m_strAlarmID;					//����ID
				std::string m_strState;						//������״̬
				std::string m_strTimeSigned;				//������λǩ��ʱ��
				std::string m_strTimeFeedBack;				//������λ����ʱ��
				std::string m_strProcessDeptCode;			//������λ����	
			};
			class CAlarm
			{
			public:
				std::string	m_strID;						//����id
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				std::string m_strAddr;						//����id��ϸ������ַ
				std::string m_strState;						//����id״̬
				std::string m_strLevel;						//���鼶��
				std::string m_strSourceType;				//���鱨����Դ����
				std::string m_strSourceId;					//���鱨����ԴID
				std::string m_strReceiptDeptCode;			//����Ӿ���λ����
				std::string m_strReceiptDeptName;			//����Ӿ���λ����
				std::string m_strFirstType;					//����idһ������
				std::string m_strSecondType;				//����id��������
				std::string m_strThirdType;					//����id��������
				std::string m_strFourthType;				//����id�ļ�����
				std::string m_strCalledNoType;				//����id���������ֵ�����
				std::string m_strContactNo;					//����id��ϵ�˺���
				std::string m_strContactName;				//����id��ϵ������
				std::string m_strReceiptName;				//����id�Ӿ�������	
				CAlarmProcess m_alarmProcess;				//������Ϣ
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTotalCount;						// ��¼����
				std::string m_strStateID;							// ״̬����
				std::vector<CAlarm> m_vecAlarm;						// ������Ϣ
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}