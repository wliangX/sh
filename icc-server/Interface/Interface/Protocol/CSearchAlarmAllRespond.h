#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmAllRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strTotalAlarmCount);
				
				unsigned int l_uiIndex = 0;
				for (CData &data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					//std::string l_strAlarmPrefixPath("/body/data/alarm");

					//std::string l_strAlarmPrefixPath("/body/alarm");

					data.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
					

					for (unsigned int i = 0; i < data.m_vecProcessData.size(); i++)
					{
						std::string l_strPrefixPathProcess(l_strPrefixPath + "/process_data/" + std::to_string(i) + "/");

						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_code", data.m_vecProcessData.at(i).m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", data.m_vecProcessData.at(i).m_strID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", data.m_vecProcessData.at(i).m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_code", data.m_vecProcessData.at(i).m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", data.m_vecProcessData.at(i).m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", data.m_vecProcessData.at(i).m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "record_id", data.m_vecProcessData.at(i).m_strRecordID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_suggestion", data.m_vecProcessData.at(i).m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_code", data.m_vecProcessData.at(i).m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_submit", data.m_vecProcessData.at(i).m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_arrived", data.m_vecProcessData.at(i).m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_signed", data.m_vecProcessData.at(i).m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_name", data.m_vecProcessData.at(i).m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_code", data.m_vecProcessData.at(i).m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_personnel", data.m_vecProcessData.at(i).m_strDispatchPersonnel);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_vehicles", data.m_vecProcessData.at(i).m_strDispatchVehicles);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_boats", data.m_vecProcessData.at(i).m_strDispatchBoats);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", data.m_vecProcessData.at(i).m_strState);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_time", data.m_vecProcessData.at(i).m_strCreateTime);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_time", data.m_vecProcessData.at(i).m_strUpdateTime);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_name", data.m_vecProcessData.at(i).m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_org_code", data.m_vecProcessData.at(i).m_strDispatchDeptOrgCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_name", data.m_vecProcessData.at(i).m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_org_code", data.m_vecProcessData.at(i).m_strProcessDeptOrgCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_type", data.m_vecProcessData.at(i).m_strProcessObjectType);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_name", data.m_vecProcessData.at(i).m_strProcessObjectName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_code", data.m_vecProcessData.at(i).m_strProcessObjectCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "business_status", data.m_vecProcessData.at(i).m_strBusinessStatus);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "seat_code", data.m_vecProcessData.at(i).m_strSeatCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "cancel_time", data.m_vecProcessData.at(i).m_strCancelTime);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "cancel_reason", data.m_vecProcessData.at(i).m_strCancelReason);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_auto_assign_job", data.m_vecProcessData.at(i).m_strIsAutoAssignJob);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_user", data.m_vecProcessData.at(i).m_strCreateUser);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_user", data.m_vecProcessData.at(i).m_strUpdateUser);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "overtime_state", data.m_vecProcessData.at(i).m_strOvertimeState);

						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_id", data.m_vecProcessData.at(i).m_strProcessObjectID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "transfers_dept_org_code", data.m_vecProcessData.at(i).m_strTransfDeptOrjCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_over", data.m_vecProcessData.at(i).m_strIsOver);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "over_remark", data.m_vecProcessData.at(i).m_strOverRemark);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "parent_id", data.m_vecProcessData.at(i).m_strParentID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "flow_code", data.m_vecProcessData.at(i).m_strGZLDM);

						p_pJson->SetNodeValue(l_strPrefixPathProcess + "centre_process_dept_code", data.m_vecProcessData.at(i).m_strCentreProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "centre_alarm_dept_code", data.m_vecProcessData.at(i).m_strCentreAlarmDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_name", data.m_vecProcessData.at(i).m_strDispatchDeptDistrictName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "linked_dispatch_code", data.m_vecProcessData.at(i).m_strLinkedDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "over_time", data.m_vecProcessData.at(i).m_strOverTime);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "finished_timeout_state", data.m_vecProcessData.at(i).m_strFinishedTimeoutState);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "police_type", data.m_vecProcessData.at(i).m_strPoliceType);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_short_name", data.m_vecProcessData.at(i).m_strProcessDeptShortName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_short_name", data.m_vecProcessData.at(i).m_strDispatchDeptShortName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "received_time", data.m_vecProcessData.at(i).m_strReceivedTime);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "createTeminal", data.m_vecProcessData.at(i).m_strCreateTeminal);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "updateTeminal", data.m_vecProcessData.at(i).m_strUpdateTeminal);
					}
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				CAlarmInfo m_oAlarm;
				
				std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
			};
			class CBody
			{
			public:
				std::string m_strCount;					//分包总数
				std::string m_strTotalAlarmCount;		//警情总数
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}