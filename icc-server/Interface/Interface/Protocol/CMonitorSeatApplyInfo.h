#pragma once


namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplyInfo
		{
		public:
			std::string	m_strGuid;					//guid
			std::string	m_strApplyType;						//申请类型
			std::string	m_strApplyTargetID;					//目标id
			std::string m_strApplyTime;					//申请时间
			std::string m_strApplyBody;						//申请对象json, 为黑名单才有，其它为空
			std::string m_strApplyStaffCode;					//申请人编码
			std::string m_strApplyStaffName;				    //申请人姓名		
			std::string m_strApplyOrgCode;			    //申请人所属部门编码
			std::string m_strApplyOrgName;				//申请人所属部门名称
			std::string m_strApplyRemark;					//申请人说明
			std::string m_strApproveStaffCode;					//审批人编码
			std::string m_strApproveStaffName;				    //审批人姓名
			std::string m_strApproveResult;						//审批结果 //默认为0
			std::string m_strApproveTime;				//审批时间
			std::string m_strApproveRemark;					//审批说明	
		};	
	}
}
