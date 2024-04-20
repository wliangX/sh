#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmProcessRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");				
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptCodeRecursive = p_pJson->GetNodeValue("/body/dispatch_dept_code_recursion", "");
				m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
				m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");
				m_oBody.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_strDispatchLeaderInstruction = p_pJson->GetNodeValue("/body/dispatch_leader_instruction", "");				
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptCodeRecursive = p_pJson->GetNodeValue("/body/process_dept_code_recursion", "");
				m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
				m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");
				m_oBody.m_strProcessFeedback = p_pJson->GetNodeValue("/body/process_feedback", "");
				m_oBody.m_strProcessLeaderInstruction = p_pJson->GetNodeValue("/body/process_leader_instruction", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				return true;
			}	

		private:
			void _InitKeys();

		public:

			class CBody
			{
			public:
				std::string	m_strPageSize;					//ÿҳ����������Ϊ�գ�
				std::string	m_strPageIndex;					//�ڼ�ҳ��1��ʾ��һҳ������Ϊ�գ�
				std::string	m_strBeginTime;					//��ѯ��ʼʱ�䣨����Ϊ�գ�
				std::string	m_strEndTime;					//��ѯ����ʱ�䣨����Ϊ�գ�
				std::string m_strID;						//����id��ģ����ѯ��
				std::string m_strAlarmID;					//����id��ģ����ѯ��
				std::string m_strState;						//������״̬��ģ����ѯ��
				std::string m_strDispatchDeptCode;			//�ɾ���λ���루�ݹ��ѯ��
				std::string m_strDispatchDeptCodeRecursive;	//0��ʾ���ݹ��ѯ��1��ʾ�ݹ��ѯ
				std::string m_strDispatchCode;				//�ɾ��˾��ţ���ֵ��ѯ�����ŷָ���
				std::string m_strDispatchLeaderCode;		//�ɾ���λֵ���쵼���ţ���ֵ��ѯ�����ŷָ���
				std::string m_strDispatchSuggestion;		//�ɾ���λ�ɾ������ģ����ѯ��
				std::string m_strDispatchLeaderInstruction;	//�ɾ���λ�쵼ָʾ��ģ����ѯ��
				std::string m_strProcessDeptCode;			//������λ���루�ݹ��ѯ��
				std::string m_strProcessDeptCodeRecursive;	//0��ʾ���ݹ��ѯ��1��ʾ�ݹ��ѯ
				std::string m_strProcessCode;				//�����˾��ţ���ֵ��ѯ�����ŷָ���
				std::string m_strProcessLeaderCode;			//������λֵ���쵼���ţ���ֵ��ѯ�����ŷָ���
				std::string m_strProcessFeedback;			//����������ģ����ѯ��
				std::string m_strProcessLeaderInstruction;	//������λ�쵼ָʾ��ģ����ѯ)
				std::string m_strFirstType;
				std::string m_strSecondType;
				std::string m_strThirdType;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;

			std::vector<std::string> m_vecKeys;
			std::map<std::string, std::string> m_mapKeyValues;
		};
	}
}
