#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmProcLogResRespond :
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

				int i = 0;
				for (auto iter = m_oBody.m_vData.begin(); iter != m_oBody.m_vData.end(); iter++)
				{

					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "guid", iter->m_strGuID);
					p_pJson->SetNodeValue(l_strPrefixPath + "flag", iter->m_strFlag);

					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", iter->m_ProcessData.m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "id", iter->m_ProcessData.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", iter->m_ProcessData.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", iter->m_ProcessData.m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", iter->m_ProcessData.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", iter->m_ProcessData.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_id", iter->m_ProcessData.m_strRecordID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", iter->m_ProcessData.m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", iter->m_ProcessData.m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", iter->m_ProcessData.m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", iter->m_ProcessData.m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", iter->m_ProcessData.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", iter->m_ProcessData.m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", iter->m_ProcessData.m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", iter->m_ProcessData.m_strDispatchPersonnel);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", iter->m_ProcessData.m_strDispatchVehicles);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", iter->m_ProcessData.m_strDispatchBoats);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", iter->m_ProcessData.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", iter->m_ProcessData.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", iter->m_ProcessData.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", iter->m_ProcessData.m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", iter->m_ProcessData.m_strDispatchDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", iter->m_ProcessData.m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", iter->m_ProcessData.m_strProcessDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", iter->m_ProcessData.m_strProcessObjectType);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", iter->m_ProcessData.m_strProcessObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", iter->m_ProcessData.m_strProcessObjectCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "business_status", iter->m_ProcessData.m_strBusinessStatus);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", iter->m_ProcessData.m_strSeatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", iter->m_ProcessData.m_strCancelTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", iter->m_ProcessData.m_strCancelReason);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", iter->m_ProcessData.m_strIsAutoAssignJob);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", iter->m_ProcessData.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", iter->m_ProcessData.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "overtime_state", iter->m_ProcessData.m_strOvertimeState);

					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", iter->m_ProcessData.m_strProcessObjectID);
					p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", iter->m_ProcessData.m_strTransfDeptOrjCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_over", iter->m_ProcessData.m_strIsOver);
					p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", iter->m_ProcessData.m_strOverRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", iter->m_ProcessData.m_strParentID);
					p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", iter->m_ProcessData.m_strGZLDM);

					p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", iter->m_ProcessData.m_strCreateTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", iter->m_ProcessData.m_strUpdateTeminal);
					i++;
				}
				return p_pJson->ToString();
			}
		public:
			class CData
			{
			public:
				std::string m_strGuID;							
				std::string m_strFlag;

				CAddOrUpdateProcessRequest::CProcessData m_ProcessData;
			};
			class CBody
			{
			public:
				std::vector<CData> m_vData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		
		};	
	}
}
