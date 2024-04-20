#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmVisitSync :
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

				p_pJson->SetNodeValue("/body/result/id", m_oBody.m_strID);
				p_pJson->SetNodeValue("/body/result/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/result/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/result/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/result/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/result/caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue("/body/result/caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue("/body/result/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/result/visitor_code", m_oBody.m_strVisitorCode);
				p_pJson->SetNodeValue("/body/result/visitor_name", m_oBody.m_strVisitorName);
				p_pJson->SetNodeValue("/body/result/satisfaction", m_oBody.m_strSatisfaction);
				p_pJson->SetNodeValue("/body/result/suggest", m_oBody.m_strSuggest);
				p_pJson->SetNodeValue("/body/result/result_content", m_oBody.m_strResultContent);
				p_pJson->SetNodeValue("/body/result/call_type", m_oBody.m_strCallType);
				p_pJson->SetNodeValue("/body/result/reason", m_oBody.m_strReason);
				
				return p_pJson->ToString();
			}
           
		public:		
			class CBody
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
			CHeader m_oHeader;			
			CBody	m_oBody;
		};		
	}
}