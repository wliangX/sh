#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAddOrUpdateLinkedRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmProcessSync :
            public IRespond, public IRequest
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
				p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);

				p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_ProcessData.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", m_oBody.m_ProcessData.m_strDispatchDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_ProcessData.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_ProcessData.m_strAlarmID);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", m_oBody.m_ProcessData.m_strDispatchDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", m_oBody.m_ProcessData.m_strDispatchCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", m_oBody.m_ProcessData.m_strDispatchName);
				p_pJson->SetNodeValue(l_strPrefixPath + "record_id", m_oBody.m_ProcessData.m_strRecordID);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_ProcessData.m_strDispatchSuggestion);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", m_oBody.m_ProcessData.m_strProcessDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", m_oBody.m_ProcessData.m_strTimeSubmit);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", m_oBody.m_ProcessData.m_strTimeArrived);
				p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", m_oBody.m_ProcessData.m_strTimeSigned);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_name", m_oBody.m_ProcessData.m_strProcessName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_code", m_oBody.m_ProcessData.m_strProcessCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", m_oBody.m_ProcessData.m_strDispatchPersonnel);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", m_oBody.m_ProcessData.m_strDispatchVehicles);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", m_oBody.m_ProcessData.m_strDispatchBoats);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_ProcessData.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_ProcessData.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_ProcessData.m_strUpdateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", m_oBody.m_ProcessData.m_strDispatchDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", m_oBody.m_ProcessData.m_strDispatchDeptOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", m_oBody.m_ProcessData.m_strProcessDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", m_oBody.m_ProcessData.m_strProcessDeptOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", m_oBody.m_ProcessData.m_strProcessObjectType);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", m_oBody.m_ProcessData.m_strProcessObjectName);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", m_oBody.m_ProcessData.m_strProcessObjectCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "business_status", m_oBody.m_ProcessData.m_strBusinessStatus);
				p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", m_oBody.m_ProcessData.m_strSeatCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", m_oBody.m_ProcessData.m_strCancelTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", m_oBody.m_ProcessData.m_strCancelReason);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", m_oBody.m_ProcessData.m_strIsAutoAssignJob);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_ProcessData.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_ProcessData.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "overtime_state", m_oBody.m_ProcessData.m_strOvertimeState);

				p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", m_oBody.m_ProcessData.m_strProcessObjectID);
				p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", m_oBody.m_ProcessData.m_strTransfDeptOrjCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_over", m_oBody.m_ProcessData.m_strIsOver);
				p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", m_oBody.m_ProcessData.m_strOverRemark);
				p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", m_oBody.m_ProcessData.m_strParentID);
				p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", m_oBody.m_ProcessData.m_strGZLDM);

				p_pJson->SetNodeValue(l_strPrefixPath + "centre_process_dept_code", m_oBody.m_ProcessData.m_strCentreProcessDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "centre_alarm_dept_code", m_oBody.m_ProcessData.m_strCentreAlarmDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_name", m_oBody.m_ProcessData.m_strDispatchDeptDistrictName);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_dispatch_code", m_oBody.m_ProcessData.m_strLinkedDispatchCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "over_time", m_oBody.m_ProcessData.m_strOverTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "finished_timeout_state", m_oBody.m_ProcessData.m_strFinishedTimeoutState);
				p_pJson->SetNodeValue(l_strPrefixPath + "police_type", m_oBody.m_ProcessData.m_strPoliceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_short_name", m_oBody.m_ProcessData.m_strProcessDeptShortName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_short_name", m_oBody.m_ProcessData.m_strDispatchDeptShortName);

				p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", m_oBody.m_ProcessData.m_strCreateTeminal);
				p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", m_oBody.m_ProcessData.m_strUpdateTeminal);


				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                std::string l_strPrefixPath("/body/");
				m_oBody.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue(l_strPrefixPath + "sync_type", "");

				m_oBody.m_ProcessData.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");
				m_oBody.m_ProcessData.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", "");
				m_oBody.m_ProcessData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
				m_oBody.m_ProcessData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
				m_oBody.m_ProcessData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_code", "");
				m_oBody.m_ProcessData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
				m_oBody.m_ProcessData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
				m_oBody.m_ProcessData.m_strRecordID = p_pJson->GetNodeValue(l_strPrefixPath + "record_id", "");
				m_oBody.m_ProcessData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");
				m_oBody.m_ProcessData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_code", "");
				m_oBody.m_ProcessData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "time_submit", "");
				m_oBody.m_ProcessData.m_strTimeArrived = p_pJson->GetNodeValue(l_strPrefixPath + "time_arrived", "");
				m_oBody.m_ProcessData.m_strTimeSigned = p_pJson->GetNodeValue(l_strPrefixPath + "time_signed", "");
				m_oBody.m_ProcessData.m_strProcessName = p_pJson->GetNodeValue(l_strPrefixPath + "process_name", "");
				m_oBody.m_ProcessData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_code", "");
				m_oBody.m_ProcessData.m_strDispatchPersonnel = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_personnel", "");
				m_oBody.m_ProcessData.m_strDispatchVehicles = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_vehicles", "");
				m_oBody.m_ProcessData.m_strDispatchBoats = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_boats", "");
				m_oBody.m_ProcessData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
				m_oBody.m_ProcessData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
				m_oBody.m_ProcessData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
				m_oBody.m_ProcessData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_name", "");
				m_oBody.m_ProcessData.m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", "");
				m_oBody.m_ProcessData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_name", "");
				m_oBody.m_ProcessData.m_strProcessDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_org_code", "");
				m_oBody.m_ProcessData.m_strProcessObjectType = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_type", "");
				m_oBody.m_ProcessData.m_strProcessObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_name", "");
				m_oBody.m_ProcessData.m_strProcessObjectCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_code", "");
				m_oBody.m_ProcessData.m_strBusinessStatus = p_pJson->GetNodeValue(l_strPrefixPath + "business_status", "");
				m_oBody.m_ProcessData.m_strSeatCode = p_pJson->GetNodeValue(l_strPrefixPath + "seat_code", "");
				m_oBody.m_ProcessData.m_strCancelTime = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_time", "");
				m_oBody.m_ProcessData.m_strCancelReason = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_reason", "");
				m_oBody.m_ProcessData.m_strIsAutoAssignJob = p_pJson->GetNodeValue(l_strPrefixPath + "is_auto_assign_job", "");
				m_oBody.m_ProcessData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
				m_oBody.m_ProcessData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
				m_oBody.m_ProcessData.m_strOvertimeState = p_pJson->GetNodeValue(l_strPrefixPath + "overtime_state", "");

				m_oBody.m_ProcessData.m_strProcessObjectID = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_id", "");
				m_oBody.m_ProcessData.m_strTransfDeptOrjCode = p_pJson->GetNodeValue(l_strPrefixPath + "transfers_dept_org_code", "");
				m_oBody.m_ProcessData.m_strIsOver = p_pJson->GetNodeValue(l_strPrefixPath + "is_over", "");
				m_oBody.m_ProcessData.m_strOverRemark = p_pJson->GetNodeValue(l_strPrefixPath + "over_remark", "");
				m_oBody.m_ProcessData.m_strParentID = p_pJson->GetNodeValue(l_strPrefixPath + "parent_id", "");
				m_oBody.m_ProcessData.m_strGZLDM = p_pJson->GetNodeValue(l_strPrefixPath + "flow_code", "");

				m_oBody.m_ProcessData.m_strCreateTeminal = p_pJson->GetNodeValue(l_strPrefixPath + "createTeminal", "");
				m_oBody.m_ProcessData.m_strUpdateTeminal = p_pJson->GetNodeValue(l_strPrefixPath + "updateTeminal", "");
				m_oBody.m_ProcessData.m_strReceivedTime = p_pJson->GetNodeValue(l_strPrefixPath + "received_time", "");

                return true;
            }
		public:		
			class CBody
			{
			public:
				std::string m_strMsgSource;					//消息来源
				std::string m_strSyncType;					//同步选项，1添加，2更新
				
				CAddOrUpdateProcessRequest::CProcessData m_ProcessData;
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};
	}
}
