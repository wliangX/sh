#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBlackListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimitType = p_pJson->GetNodeValue("/body/limit_type", "");
				m_oBody.m_strLimitNum = p_pJson->GetNodeValue("/body/limit_num", "");
				m_oBody.m_strLimitMin = p_pJson->GetNodeValue("/body/limit_min", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strLimitReason = p_pJson->GetNodeValue("/body/limit_reason", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");
				m_oBody.m_strApprover = p_pJson->GetNodeValue("/body/approver", "");
				m_oBody.m_strApprovedInfo = p_pJson->GetNodeValue("/body/approved_info", "");
				m_oBody.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strLimitType;		// 受限类型
				std::string m_strLimitNum;		// 受限号码
				std::string m_strLimitMin;		// 受限时长（单位：分钟）
				std::string m_strBeginTime;		// 受限开始时间
				std::string m_strEndTime;		// 受限结束时间
				std::string m_strLimitReason;	// 受限原因
				std::string m_strStaffCode;		// 警员编码
				std::string m_strStaffName;		// 警员名称
				std::string m_strApprover;		// 审批人
				std::string m_strApprovedInfo;		// 审批信息
				std::string m_strReceiptDeptName;		// 操作部门名称
				std::string m_strReceiptDeptDistrictCode;		// 行政区划
			};
			CBody m_oBody;
		};
	}
}
