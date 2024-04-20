#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmVisitRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strVisitorCode = p_pJson->GetNodeValue("/body/visitor_code", "");
				m_oBody.m_strVisitorName = p_pJson->GetNodeValue("/body/visitor_name", "");
				m_oBody.m_strSatisfaction = p_pJson->GetNodeValue("/body/satisfaction", "");
				m_oBody.m_strSuggest = p_pJson->GetNodeValue("/body/suggest", "");
				m_oBody.m_strResultContent = p_pJson->GetNodeValue("/body/result_content", "");
				m_oBody.m_strCallType = p_pJson->GetNodeValue("/body/call_type", "");
				m_oBody.m_strReason = p_pJson->GetNodeValue("/body/reason", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strType;								//1����������2����༭
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
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
