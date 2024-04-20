#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetAlarmProcessRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strTimeEdit = p_pJson->GetNodeValue("/body/time_edit", "");
				m_oBody.m_strTimeSubmit = p_pJson->GetNodeValue("/body/time_submit", "");
				m_oBody.m_strTimeArrived = p_pJson->GetNodeValue("/body/time_arrived", "");
				m_oBody.m_strTimeSigned = p_pJson->GetNodeValue("/body/time_signed", "");
				m_oBody.m_strTimeFeedBack = p_pJson->GetNodeValue("/body/time_feedback", "");
				m_oBody.m_strIsNeedFeedback = p_pJson->GetNodeValue("/body/is_need_feedback", "");
				m_oBody.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue("/body/dispatch_dept_district_code", "");
				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptName = p_pJson->GetNodeValue("/body/dispatch_dept_name", "");
				m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
				m_oBody.m_strDispatchName = p_pJson->GetNodeValue("/body/dispatch_name", "");
				m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");
				m_oBody.m_strDispatchLeaderName = p_pJson->GetNodeValue("/body/dispatch_leader_name", "");
				m_oBody.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_strDispatchLeaderInstruction = p_pJson->GetNodeValue("/body/dispatch_leader_instruction", "");
				m_oBody.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue("/body/process_dept_district_code", "");
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptName = p_pJson->GetNodeValue("/body/process_dept_name", "");
				m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
				m_oBody.m_strProcessName = p_pJson->GetNodeValue("/body/process_name", "");
				m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");
				m_oBody.m_strProcessLeaderName = p_pJson->GetNodeValue("/body/process_leader_name", "");
				m_oBody.m_strProcessFeedback = p_pJson->GetNodeValue("/body/process_feedback", "");
				m_oBody.m_strProcessLeaderInstruction = p_pJson->GetNodeValue("/body/process_leader_instruction", "");
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_edit", m_oBody.m_strTimeEdit);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", m_oBody.m_strTimeSubmit);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", m_oBody.m_strTimeArrived);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", m_oBody.m_strTimeSigned);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_feedback", m_oBody.m_strTimeFeedBack);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_need_feedback", m_oBody.m_strIsNeedFeedback);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", m_oBody.m_strDispatchDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", m_oBody.m_strDispatchDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", m_oBody.m_strDispatchDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", m_oBody.m_strDispatchCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", m_oBody.m_strDispatchName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_code", m_oBody.m_strDispatchLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_name", m_oBody.m_strDispatchLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_strDispatchSuggestion);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_instruction", m_oBody.m_strDispatchLeaderInstruction);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_district_code", m_oBody.m_strProcessDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", m_oBody.m_strProcessDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", m_oBody.m_strProcessDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_code", m_oBody.m_strProcessCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_name", m_oBody.m_strProcessName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_code", m_oBody.m_strProcessLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_name", m_oBody.m_strProcessLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_feedback", m_oBody.m_strProcessFeedback);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_instruction", m_oBody.m_strProcessLeaderInstruction);
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strID;						//����ID
				std::string m_strAlarmID;					//����ID
				std::string m_strState;						//������״̬
				std::string m_strTimeEdit;					//�ɾ�����дʱ��
				std::string m_strTimeSubmit;				//�ɾ����ύʱ��
				std::string m_strTimeArrived;				//�ɾ���ϵͳ����ʱ��
				std::string m_strTimeSigned;				//������λǩ��ʱ��
				std::string m_strTimeFeedBack;				//������λ����ʱ��
				std::string m_strIsNeedFeedback;			//�Ƿ���Ҫ����
				std::string m_strDispatchDeptDistrictCode;	//�ɾ���λ��������
				std::string m_strDispatchDeptCode;			//�ɾ���λ����
				std::string m_strDispatchDeptName;			//�ɾ���λ����
				std::string m_strDispatchCode;				//�ɾ��˾���
				std::string m_strDispatchName;				//�ɾ�������
				std::string m_strDispatchLeaderCode;		//�ɾ���λֵ���쵼����
				std::string m_strDispatchLeaderName;		//�ɾ���λֵ���쵼����
				std::string m_strDispatchSuggestion;		//�ɾ���λ�ɾ����
				std::string m_strDispatchLeaderInstruction;	//�ɾ���λ�쵼ָʾ
				std::string m_strProcessDeptDistrictCode;	//������λ��������
				std::string m_strProcessDeptCode;			//������λ����
				std::string m_strProcessDeptName;			//������λ����
				std::string m_strProcessCode;				//�����˾���
				std::string m_strProcessName;				//����������
				std::string m_strProcessLeaderCode;			//������λֵ���쵼����
				std::string m_strProcessLeaderName;			//������λֵ���쵼����
				std::string m_strProcessFeedback;			//��������
				std::string m_strProcessLeaderInstruction;	//������λ�쵼ָʾ

				std::string m_strCreateUser;
				std::string m_strCreateTime;
				std::string m_strUpdateUser;
				std::string m_strUpdateTime;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
