#pragma once


namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplyInfo
		{
		public:
			std::string	m_strGuid;					//guid
			std::string	m_strApplyType;						//��������
			std::string	m_strApplyTargetID;					//Ŀ��id
			std::string m_strApplyTime;					//����ʱ��
			std::string m_strApplyBody;						//�������json, Ϊ���������У�����Ϊ��
			std::string m_strApplyStaffCode;					//�����˱���
			std::string m_strApplyStaffName;				    //����������		
			std::string m_strApplyOrgCode;			    //�������������ű���
			std::string m_strApplyOrgName;				//������������������
			std::string m_strApplyRemark;					//������˵��
			std::string m_strApproveStaffCode;					//�����˱���
			std::string m_strApproveStaffName;				    //����������
			std::string m_strApproveResult;						//������� //Ĭ��Ϊ0
			std::string m_strApproveTime;				//����ʱ��
			std::string m_strApproveRemark;					//����˵��	
		};	
	}
}
