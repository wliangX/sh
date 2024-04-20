#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CMonitorSeatApplyInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplySync :
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
				std::string l_strPrefixPath("/body/");
				
				p_pJson->SetNodeValue(l_strPrefixPath + "guid", m_oBody.m_oSeatApplyInfo.m_strGuid);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_type", m_oBody.m_oSeatApplyInfo.m_strApplyType);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_target_id", m_oBody.m_oSeatApplyInfo.m_strApplyTargetID);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_time", m_oBody.m_oSeatApplyInfo.m_strApplyTime);

				p_pJson->SetNodeValue(l_strPrefixPath + "apply_body", m_oBody.m_oSeatApplyInfo.m_strApplyBody);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_staff_code", m_oBody.m_oSeatApplyInfo.m_strApplyStaffCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_staff_name", m_oBody.m_oSeatApplyInfo.m_strApplyStaffName);
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_org_code", m_oBody.m_oSeatApplyInfo.m_strApplyOrgCode);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "apply_remark", m_oBody.m_oSeatApplyInfo.m_strApplyRemark);
				p_pJson->SetNodeValue(l_strPrefixPath + "approve_staff_code", m_oBody.m_oSeatApplyInfo.m_strApproveStaffCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "approve_staff_name", m_oBody.m_oSeatApplyInfo.m_strApproveStaffName);
				p_pJson->SetNodeValue(l_strPrefixPath + "approve_result", m_oBody.m_oSeatApplyInfo.m_strApproveResult);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "approve_time", m_oBody.m_oSeatApplyInfo.m_strApproveTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "approve_remark", m_oBody.m_oSeatApplyInfo.m_strApproveRemark);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				CMonitorSeatApplyInfo m_oSeatApplyInfo;     //申请信息
				std::string m_strSyncType;					//同步选项，1：添加，2：更新，3：删除
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
