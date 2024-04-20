#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CMonitorSeatApplyInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplyQueryRespond :
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
				
				
				p_pJson->SetNodeValue(l_strPrefixPath + "all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue(l_strPrefixPath + "count", m_oBody.m_strCount);
			
			    unsigned int l_uiIndex = 0;
				for (CMonitorSeatApplyInfo data : m_oBody.m_vecApplyInfo)
				{
					std::string l_strPrefixPathData("/body/data/" + std::to_string(l_uiIndex) + "/");
				
					p_pJson->SetNodeValue(l_strPrefixPathData + "guid", data.m_strGuid);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_type", data.m_strApplyType);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_target_id", data.m_strApplyTargetID);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_time", data.m_strApplyTime);

				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_body", data.m_strApplyBody);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_staff_code", data.m_strApplyStaffCode);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_staff_name", data.m_strApplyStaffName);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_org_code", data.m_strApplyOrgCode);
				
				    p_pJson->SetNodeValue(l_strPrefixPathData + "apply_remark", data.m_strApplyRemark);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "approve_staff_code", data.m_strApproveStaffCode);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "approve_staff_name", data.m_strApproveStaffName);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "approve_result", data.m_strApproveResult);
				
				    p_pJson->SetNodeValue(l_strPrefixPathData + "approve_time", data.m_strApproveTime);
				    p_pJson->SetNodeValue(l_strPrefixPathData + "approve_remark", data.m_strApproveRemark);
					
				    ++l_uiIndex;
					
				}
				return p_pJson->ToString();
			}

		public:
		
            class CBody
			{
			public:
			    std::string m_strAllCount;
				std::string m_strCount;
			    std::vector<CMonitorSeatApplyInfo> m_vecApplyInfo;
			};
		
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};	
	}
}
