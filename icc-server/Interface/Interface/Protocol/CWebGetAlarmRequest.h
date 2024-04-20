#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");
				m_oBody.m_strLangCode = p_pJson->GetNodeValue("/body/lang_code", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strStateID = p_pJson->GetNodeValue("/body/state_id", "");

				m_oBody.m_oAlarm.m_strId = p_pJson->GetNodeValue("/body/alarm/id", "");
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/time", "");
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", "");;
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");

				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/alarm/contact_name", "");

				return true;
			}

		public:
			class CAlarm
			{
			public:
				std::string m_strId;						//����ID
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				std::string m_strAddr;						//����id��ϸ������ַ
				std::string m_strState;						//����id״̬
				std::string m_strFirstType;					//����idһ������
				std::string m_strSecondType;				//����id��������
				std::string m_strThirdType;					//����id��������
				std::string m_strFourthType;				//����id�ļ�����

				std::string m_strContactNo;					//����id��ϵ�˺���
				std::string m_strContactName;				//����id��ϵ������
			};
			class CBody
			{
			public:
				std::string m_strToken;				//��Ȩ��
				std::string m_strLangCode;			//���Ա���
				std::string m_strPageSize;			//ÿҳ����
				std::string m_strPageIndex;			//ҳ�룬1��ʾ��һҳ
				std::string m_strBeginTime;			//��ʼʱ��
				std::string m_strEndTime;			//����ʱ��
				std::string m_strStateID;			//״̬����
				CAlarm m_oAlarm;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
