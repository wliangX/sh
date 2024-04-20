#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CMonitorSeatApplyInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		typedef CMonitorSeatApplyInfo CBody;
		
		class CMonitorSeatApplyRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strApplyType = p_pJson->GetNodeValue("/body/apply_type", "");
				m_oBody.m_strApplyTargetID = p_pJson->GetNodeValue("/body/apply_target_id", "");
				m_oBody.m_strApplyTime = p_pJson->GetNodeValue("/body/apply_time", "");
				m_oBody.m_strApplyBody = p_pJson->GetNodeValue("/body/apply_body", "");
				m_oBody.m_strApplyStaffCode = p_pJson->GetNodeValue("/body/apply_staff_code", "");
				m_oBody.m_strApplyStaffName = p_pJson->GetNodeValue("/body/apply_staff_name", "");
				m_oBody.m_strApplyOrgCode = p_pJson->GetNodeValue("/body/apply_org_code", "");
				m_oBody.m_strApplyOrgName = p_pJson->GetNodeValue("/body/apply_org_name", "");
				m_oBody.m_strApplyRemark = p_pJson->GetNodeValue("/body/apply_remark", "");
				m_oBody.m_strApproveStaffCode = p_pJson->GetNodeValue("/body/approve_staff_code", "");
				m_oBody.m_strApproveStaffName = p_pJson->GetNodeValue("/body/approve_staff_name", "");
				m_oBody.m_strApproveResult = p_pJson->GetNodeValue("/body/approve_result", "");
				m_oBody.m_strApproveTime = p_pJson->GetNodeValue("/body/approve_time", "");
				m_oBody.m_strApproveRemark = p_pJson->GetNodeValue("/body/approve_remark", "");
						
				
				return true;
			}

			virtual std::string ToStringOnlyBody(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/apply_type", m_oBody.m_strApplyType);
				p_pJson->SetNodeValue("/body/apply_target_id", m_oBody.m_strApplyTargetID);
				p_pJson->SetNodeValue("/body/apply_time", m_oBody.m_strApplyTime);
				p_pJson->SetNodeValue("/body/apply_body", m_oBody.m_strApplyBody);
				p_pJson->SetNodeValue("/body/apply_staff_code", m_oBody.m_strApplyStaffCode);
				p_pJson->SetNodeValue("/body/apply_staff_name", m_oBody.m_strApplyStaffName);
				p_pJson->SetNodeValue("/body/apply_org_code", m_oBody.m_strApplyOrgCode);
				p_pJson->SetNodeValue("/body/apply_org_name", m_oBody.m_strApplyOrgName);
				p_pJson->SetNodeValue("/body/apply_remark", m_oBody.m_strApplyRemark);
				p_pJson->SetNodeValue("/body/approve_staff_code", m_oBody.m_strApproveStaffCode);
				p_pJson->SetNodeValue("/body/approve_staff_name", m_oBody.m_strApproveStaffName);
				p_pJson->SetNodeValue("/body/approve_result", m_oBody.m_strApproveResult);
				p_pJson->SetNodeValue("/body/approve_time", m_oBody.m_strApproveTime);
				p_pJson->SetNodeValue("/body/approve_remark", m_oBody.m_strApproveRemark);

				return p_pJson->ToString();
			}

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
