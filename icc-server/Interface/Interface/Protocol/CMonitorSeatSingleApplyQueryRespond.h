#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CMonitorSeatApplyInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatSingleApplyQueryRespond :
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
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_ApplyInfo.m_strGuid);
				p_pJson->SetNodeValue("/body/apply_type", m_oBody.m_ApplyInfo.m_strApplyType);
				p_pJson->SetNodeValue("/body/apply_target_id", m_oBody.m_ApplyInfo.m_strApplyTargetID);
				p_pJson->SetNodeValue("/body/apply_time", m_oBody.m_ApplyInfo.m_strApplyTime);

				p_pJson->SetNodeValue("/body/apply_body", m_oBody.m_ApplyInfo.m_strApplyBody);
				p_pJson->SetNodeValue("/body/apply_staff_code", m_oBody.m_ApplyInfo.m_strApplyStaffCode);
				p_pJson->SetNodeValue("/body/apply_staff_name", m_oBody.m_ApplyInfo.m_strApplyStaffName);
				p_pJson->SetNodeValue("/body/apply_org_code", m_oBody.m_ApplyInfo.m_strApplyOrgCode);

				p_pJson->SetNodeValue("/body/apply_remark", m_oBody.m_ApplyInfo.m_strApplyRemark);
				p_pJson->SetNodeValue("/body/approve_staff_code", m_oBody.m_ApplyInfo.m_strApproveStaffCode);
				p_pJson->SetNodeValue("/body/approve_staff_name", m_oBody.m_ApplyInfo.m_strApproveStaffName);
				p_pJson->SetNodeValue("/body/approve_result", m_oBody.m_ApplyInfo.m_strApproveResult);

				p_pJson->SetNodeValue("/body/approve_time", m_oBody.m_ApplyInfo.m_strApproveTime);
				p_pJson->SetNodeValue("/body/approve_remark", m_oBody.m_ApplyInfo.m_strApproveRemark);
					
				return p_pJson->ToString();
			}

		public:
		
            class CBody
			{
			public:
			    CMonitorSeatApplyInfo m_ApplyInfo;
			};
		
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};	
	}
}
