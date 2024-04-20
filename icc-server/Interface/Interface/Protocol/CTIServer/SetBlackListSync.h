#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetBlackListSync :
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
				p_pJson->SetNodeValue("/body/limit_type", m_oBody.m_strLimitType);
				p_pJson->SetNodeValue("/body/limit_num", m_oBody.m_strLimitNum);
				p_pJson->SetNodeValue("/body/limit_min", m_oBody.m_strlimitMinute);
				p_pJson->SetNodeValue("/body/limit_reason", m_oBody.m_strLimitReason);
				p_pJson->SetNodeValue("/body/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/body/staff_name", m_oBody.m_strStaffName);
				p_pJson->SetNodeValue("/body/begin_time", m_oBody.m_strBeginTime);
				p_pJson->SetNodeValue("/body/end_time", m_oBody.m_strEndTime);
				p_pJson->SetNodeValue("/body/is_update", m_oBody.m_strIsUpdate);
				p_pJson->SetNodeValue("/body/approver", m_oBody.m_strApprover);
				p_pJson->SetNodeValue("/body/approved_info", m_oBody.m_strApprovedInfo);
				p_pJson->SetNodeValue("/body/receipt_dept_name", m_oBody.m_strReceiptDeptName);
				p_pJson->SetNodeValue("/body/receipt_dept_district_code", m_oBody.m_strReceiptDeptDistrictCode);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strLimitType = p_pJson->GetNodeValue("/body/limit_type", "");
				m_oBody.m_strLimitNum = p_pJson->GetNodeValue("/body/limit_num", "");
				m_oBody.m_strlimitMinute = p_pJson->GetNodeValue("/body/limit_min", "");
				m_oBody.m_strLimitReason = p_pJson->GetNodeValue("/body/limit_reason", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/body/staff_name", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strIsUpdate = p_pJson->GetNodeValue("/body/is_update", "");
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
				std::string m_strLimitType;
				std::string m_strLimitNum;		//	受限号码
				std::string m_strlimitMinute;	//	受限时长
				std::string m_strLimitReason;	//	受限原因
				std::string m_strStaffCode;		//	接警员编号
				std::string m_strStaffName;		//	接警员姓名
				std::string m_strBeginTime;		//	受限开始时间
				std::string m_strEndTime;		//	受限结束时间
				std::string m_strApprover;		//	审批人
				std::string m_strApprovedInfo;		//	审批信息
				std::string m_strReceiptDeptName;		//	操作部门名称
				std::string m_strReceiptDeptDistrictCode;		//	行政区划
				std::string m_strIsUpdate;		//	是否为更新黑名单,0:更新，1:新增
			};
			CBody m_oBody;
		};
	}
}
