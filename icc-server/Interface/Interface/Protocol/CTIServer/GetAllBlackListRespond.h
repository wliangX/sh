#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllBlackListRespond :
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_type", m_oBody.m_vecData.at(i).m_strLimitType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_num", m_oBody.m_vecData.at(i).m_strLimitNum);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_min", m_oBody.m_vecData.at(i).m_strLimitMin);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/limit_reason", m_oBody.m_vecData.at(i).m_strLimitReason);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/staff_code", m_oBody.m_vecData.at(i).m_strStaffCode);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/staff_name", m_oBody.m_vecData.at(i).m_strStaffName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/begin_time", m_oBody.m_vecData.at(i).m_strBeginTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/end_time", m_oBody.m_vecData.at(i).m_strEndTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/approver", m_oBody.m_vecData.at(i).m_strApprover);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/approved_info", m_oBody.m_vecData.at(i).m_strApprovedInfo);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/receipt_dept_name", m_oBody.m_vecData.at(i).m_strReceiptDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/receipt_dept_district_code", m_oBody.m_vecData.at(i).m_strReceiptDeptDistrictCode);
				}

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

				int l_nCount = p_pJson->GetCount("/body/data");
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strLimitType = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/limit_type", "");
					l_oData.m_strLimitNum = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/limit_num", "");
					l_oData.m_strLimitMin = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/limit_min", "");
					l_oData.m_strLimitReason = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/limit_reason", "");
					l_oData.m_strStaffCode = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/staff_code", "");
					l_oData.m_strStaffName = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/staff_name", "");
					l_oData.m_strBeginTime = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/begin_time", "");
					l_oData.m_strEndTime = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/end_time", "");
					l_oData.m_strApprover = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/approver", "");
					l_oData.m_strApprovedInfo = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/approved_info", "");
					l_oData.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_name", "");
					l_oData.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_district_code", "");
					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;

				class CData
				{
				public:
					std::string m_strLimitType;
					std::string m_strLimitNum;
					std::string m_strLimitMin;
					std::string m_strLimitReason;
					std::string m_strStaffCode;
					std::string m_strStaffName;
					std::string m_strBeginTime;
					std::string m_strEndTime;
					std::string m_strApprover;				// 审批人
					std::string m_strApprovedInfo;			// 审批信息
					std::string m_strReceiptDeptName;			// 操作部门
					std::string m_strReceiptDeptDistrictCode;	// 行政区划
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
