#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>
#include <Protocol/CAddOrUpdateLinkedRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateAlarmWithProcessRequest :
			public IRequest,IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strSaveFlag = p_pJson->GetNodeValue("/body/save_flag", "");
				m_oBody.m_strIsEdit = p_pJson->GetNodeValue("/body/is_edit", "");
				m_oBody.m_strIsVcsSyncFlag = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");
				m_oBody.m_strSupplement = p_pJson->GetNodeValue("/body/supplement", "");
				m_oBody.m_strAllowNull = p_pJson->GetNodeValue("/body/allow_null","");

				m_oBody.m_oAlarm.ParseString("/body/alarm", p_pJson);
				//m_oBody.m_oAlarm.m_strMsgSource = p_pJson->GetNodeValue("/body/alarm/msg_source", "");


				//m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_district_code", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_code", "");
				//m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/alarm/id", "");
				//m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/alarm/merge_id", "");
				//m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/alarm/called_no_type", "");
				//m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/alarm/source_type", "");
				//m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/alarm/handle_type", "");
				//m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue("/body/alarm/source_id", "");
				//m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/alarm/receipt_code", "");
				//m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/alarm/receipt_name", "");
				//m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/receiving_time", "");  //time变更字段名
				//m_oBody.m_oAlarm.m_strReceivedTime = p_pJson->GetNodeValue("/body/alarm/received_time", "");  //新增
				////first_submit_time  服务自己记的
				//m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/alarm/caller_no", "");
				//m_oBody.m_oAlarm.m_strCallerUserName = p_pJson->GetNodeValue("/body/alarm/caller_user_name", ""); //新增
				//m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/alarm/caller_addr", "");
				//m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue("/body/alarm/caller_name", "");
				//m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue("/body/alarm/caller_gender", "");
				//m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				//m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/alarm/caller_id_type", "");
				//m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/alarm/caller_id", "");
				//m_oBody.m_oAlarm.m_strAlarmAddr = p_pJson->GetNodeValue("/body/alarm/alarm_addr", "");  //新增  报警地址
				//m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", ""); 
				//m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				//m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_code", "");
				//m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				//m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				//m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				//m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");
				//m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/alarm/symbol_code", "");
				//m_oBody.m_oAlarm.m_strErpetratorsNumber = p_pJson->GetNodeValue("/body/erpetrators_number", ""); //新增  
				//m_oBody.m_oAlarm.m_strIsArmed = p_pJson->GetNodeValue("/body/alarm/is_armed", "");  //新增  
				//m_oBody.m_oAlarm.m_strIsHazardousSubstances = p_pJson->GetNodeValue("/body/alarm/is_hazardous_substances", "");  //新增  
				//m_oBody.m_oAlarm.m_strIsExplosionOrLeakage = p_pJson->GetNodeValue("/body/alarm/is_explosion_or_leakage", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfTrapped = p_pJson->GetNodeValue("/body/alarm/desc_of_trapped", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfInjured = p_pJson->GetNodeValue("/body/alarm/desc_of_injured", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfDead = p_pJson->GetNodeValue("/body/alarm/desc_of_dead", ""); //新增  
				//m_oBody.m_oAlarm.m_strIsForeignLanguage = p_pJson->GetNodeValue("/body/alarm/is_foreign_language", ""); //新增  
				//m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/alarm/longitude", "");
				//m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/alarm/latitude", "");
				//m_oBody.m_oAlarm.m_strManualLongitude = p_pJson->GetNodeValue("/body/alarm/manual_longitude", ""); //新增  
				//m_oBody.m_oAlarm.m_strManualLatitude = p_pJson->GetNodeValue("/body/alarm/manual_latitude", "");//新增  
				//m_oBody.m_oAlarm.m_strRemark = p_pJson->GetNodeValue("/body/alarm/remark", "");
				//m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "").empty() ? "04" : p_pJson->GetNodeValue("/body/alarm/level", "");
				//m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				//m_oBody.m_oAlarm.m_strEmergencyRescueLevel = p_pJson->GetNodeValue("/body/alarm/emergency_rescue_level", ""); //新增  
				//m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue("/body/alarm/vehicle_type", "");
				//m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/alarm/vehicle_no", "");
				//m_oBody.m_oAlarm.m_strIsHazardousVehicle = p_pJson->GetNodeValue("/body/alarm/is_hazardous_vehicle", ""); //新增
				//m_oBody.m_oAlarm.m_strCreateTime = p_pJson->GetNodeValue("/body/alarm/create_time", "");
				//m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/alarm/update_time", "");
				//m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/label", "");  //title变更字段名
				//m_oBody.m_oAlarm.m_strPrivacy = p_pJson->GetNodeValue("/body/alarm/is_privacy", "");

				if (m_oBody.m_oAlarm.m_strLevel.empty())
				{
					m_oBody.m_oAlarm.m_strLevel = "04";
				}
				//m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "").empty() ? "04" : p_pJson->GetNodeValue("/body/alarm/level", "");
				if (m_oBody.m_oAlarm.m_strPrivacy.empty())
				{
					m_oBody.m_oAlarm.m_strPrivacy = "0";
				}
				else if (m_oBody.m_oAlarm.m_strPrivacy == "true")
				{
					m_oBody.m_oAlarm.m_strPrivacy = "1";
				}
				else if (m_oBody.m_oAlarm.m_strPrivacy == "false")
				{
					m_oBody.m_oAlarm.m_strPrivacy = "0";
				}

				//m_oBody.m_oAlarm.m_strReceiptSrvName = p_pJson->GetNodeValue("/body/receipt_srv_name", "");  //新增
				
				if (m_oBody.m_oAlarm.m_strReceiptSrvName.empty())
				{
					m_oBody.m_oAlarm.m_strReceiptSrvName = "icc";
				}

				if (m_oBody.m_oAlarm.m_strIsMerge.empty())
				{
					m_oBody.m_oAlarm.m_strIsMerge = "0";
				}
				//m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/alarm/admin_dept_name", "");
				//m_oBody.m_oAlarm.m_strAdminDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_org_code", "");   //新增
				//m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/alarm/receipt_dept_name", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_org_code", "");   //新增
				////is_delete不在协议中传递
				//m_oBody.m_oAlarm.m_strIsInvalid = p_pJson->GetNodeValue("/body/alarm/is_invalid", "");   //新增
				//m_oBody.m_oAlarm.m_strBusinessState = p_pJson->GetNodeValue("/body/alarm/business_status", "");  //新增
				//m_oBody.m_oAlarm.m_strSeatNo = p_pJson->GetNodeValue("/body/alarm/receipt_seatno", "");   //seatno改名
				//m_oBody.m_oAlarm.m_strIsMerge = p_pJson->GetNodeValue("/body/alarm/is_merge", "");
				//m_oBody.m_oAlarm.m_strCreateUser = p_pJson->GetNodeValue("/body/alarm/create_user", "");
				//m_oBody.m_oAlarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/alarm/update_user", "");
			    //is_visitor
				//is_feedback

				
			    /*--------------      以下为删除   --------------------*/
				//m_oBody.m_oAlarm.m_strActualOccurTime = p_pJson->GetNodeValue("/body/alarm/actual_occur_time", "");
				
				//m_oBody.m_oAlarm.m_strSymbolAddr = p_pJson->GetNodeValue("/body/alarm/symbol_addr", "");
				//m_oBody.m_oAlarm.m_strFireBuildingType = p_pJson->GetNodeValue("/body/alarm/fire_building_type", "");

				//m_oBody.m_oAlarm.m_strEventType = p_pJson->GetNodeValue("/body/alarm/event_type", "");

	
				//m_oBody.m_oAlarm.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/alarm/actual_called_no_type", "");

				//m_oBody.m_oAlarm.m_strCallerAge = p_pJson->GetNodeValue("/body/alarm/caller_age", "");
				//m_oBody.m_oAlarm.m_strCallerBirthday = p_pJson->GetNodeValue("/body/alarm/caller_birthday", "");

				
				//m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/alarm/contact_name", "");
				//m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue("/body/alarm/contact_addr", "");
				//m_oBody.m_oAlarm.m_strContactID = p_pJson->GetNodeValue("/body/alarm/contact_id", "");
				//m_oBody.m_oAlarm.m_strContactIDType = p_pJson->GetNodeValue("/body/alarm/contact_id_type", "");
				//m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue("/body/alarm/contact_gender", "");
				//m_oBody.m_oAlarm.m_strContactAge = p_pJson->GetNodeValue("/body/alarm/contact_age", "");
				//m_oBody.m_oAlarm.m_strContactBirthday = p_pJson->GetNodeValue("/body/alarm/contact_birthday", "");

				//m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_district_code", "");
				
				

				
				
				
				//m_oBody.m_oAlarm.m_strLeaderCode = p_pJson->GetNodeValue("/body/alarm/leader_code", "");
				//m_oBody.m_oAlarm.m_strLeaderName = p_pJson->GetNodeValue("/body/alarm/leader_name", "");
			
			

				//m_oBody.m_oAlarm.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/alarm/dispatch_suggestion", "");
				

				//m_oBody.m_oAlarm.m_strOperatorCode = p_pJson->GetNodeValue("/body/alarm/operator_code", "");
				//m_oBody.m_oAlarm.m_strOperatorName = p_pJson->GetNodeValue("/body/alarm/operator_name", "");

				//m_oBody.m_oAlarm.m_strCityCode = p_pJson->GetNodeValue("/body/alarm/city_code", "");

				
				
				
				

				//2022/04/02 新增
	
				//m_oBody.m_oAlarm.m_strAlarmAddr = p_pJson->GetNodeValue("/body/alarm_addr", "");
			
	

				//m_oBody.m_oAlarm.m_strManualLongitude = p_pJson->GetNodeValue("/body/manual_longitude", "");
				//m_oBody.m_oAlarm.m_strManualLatitude = p_pJson->GetNodeValue("/body/manual_latitude", "");
			
				
				// 			
				int l_iCount = p_pJson->GetCount("/body/process_data");
				for (int i = 0; i<l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/process_data/" + std::to_string(i) + "/");
					CAddOrUpdateProcessRequest::CProcessData l_oData;
					l_oData.m_bIsNewProcess = false;
					l_oData.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");

					l_oData.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", "");
					l_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					l_oData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_code", "");
					l_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					l_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					l_oData.m_strRecordID = p_pJson->GetNodeValue(l_strPrefixPath + "record_id", "");
					l_oData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");
					l_oData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_code", "");
					l_oData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "time_submit", "");
					l_oData.m_strTimeArrived = p_pJson->GetNodeValue(l_strPrefixPath + "time_arrived", "");
					l_oData.m_strTimeSigned = p_pJson->GetNodeValue(l_strPrefixPath + "time_signed", "");
					l_oData.m_strProcessName = p_pJson->GetNodeValue(l_strPrefixPath + "process_name", "");
					l_oData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_code", "");
					l_oData.m_strDispatchPersonnel = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_personnel", "");
					l_oData.m_strDispatchVehicles = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_vehicles", "");
					l_oData.m_strDispatchBoats = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_boats", "");
					l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					l_oData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					l_oData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_name", "");
					l_oData.m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", "");
					l_oData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_name", "");
					l_oData.m_strProcessDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_org_code", "");
					l_oData.m_strProcessObjectType = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_type", "");
					l_oData.m_strProcessObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_name", "");
					l_oData.m_strProcessObjectCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_code", "");
					l_oData.m_strBusinessStatus = p_pJson->GetNodeValue(l_strPrefixPath + "business_status", "");
					l_oData.m_strSeatCode = p_pJson->GetNodeValue(l_strPrefixPath + "seat_code", "");
					l_oData.m_strCancelTime = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_time", "");
					l_oData.m_strCancelReason = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_reason", "");
					l_oData.m_strIsAutoAssignJob = p_pJson->GetNodeValue(l_strPrefixPath + "is_auto_assign_job", "");
					l_oData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					l_oData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					l_oData.m_strOvertimeState = p_pJson->GetNodeValue(l_strPrefixPath + "overtime_state", "");

					l_oData.m_strProcessObjectID = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_id", "");
					l_oData.m_strTransfDeptOrjCode = p_pJson->GetNodeValue(l_strPrefixPath + "transfers_dept_org_code", "");
					l_oData.m_strIsOver = p_pJson->GetNodeValue(l_strPrefixPath + "is_over", "");
					l_oData.m_strOverRemark = p_pJson->GetNodeValue(l_strPrefixPath + "over_remark", "");
					l_oData.m_strParentID = p_pJson->GetNodeValue(l_strPrefixPath + "parent_id", "");
					l_oData.m_strGZLDM = p_pJson->GetNodeValue(l_strPrefixPath + "flow_code", "");
					l_oData.m_strCentreProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "centre_process_dept_code", "");
					l_oData.m_strCentreAlarmDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "centre_alarm_dept_code", "");
					l_oData.m_strDispatchDeptDistrictName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_name", "");
					l_oData.m_strLinkedDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "linked_dispatch_code", "");
					l_oData.m_strPoliceType = p_pJson->GetNodeValue(l_strPrefixPath + "police_type", "");
					l_oData.m_strProcessDeptShortName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_short_name", "");
					l_oData.m_strDispatchDeptShortName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_short_name", "");
					l_oData.m_strOverTime = p_pJson->GetNodeValue(l_strPrefixPath + "over_time", "");
					l_oData.m_strFinishedTimeoutState = p_pJson->GetNodeValue(l_strPrefixPath + "finished_timeout_state", "");
					l_oData.m_strCreateTeminal = p_pJson->GetNodeValue(l_strPrefixPath + "createTeminal", "");
					l_oData.m_strUpdateTeminal = p_pJson->GetNodeValue(l_strPrefixPath + "updateTeminal", "");

					m_oBody.m_vecProcessData.push_back(l_oData);
				}

				l_iCount = p_pJson->GetCount("/body/linked_data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(i) + "/");
					CAddOrUpdateLinkedRequest::CLinkedData t_oData;
					t_oData.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");
					t_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					t_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					t_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					t_oData.m_strLinkedOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_code", "");
					t_oData.m_strLinkedOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_name", "");
					t_oData.m_strLinkedOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_type", "");
					t_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					t_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					t_oData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					t_oData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					t_oData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					t_oData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					t_oData.m_strResult = p_pJson->GetNodeValue(l_strPrefixPath + "result", "");
					m_oBody.m_vecLinkedData.push_back(t_oData);
				}
				
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				if (!m_oBody.m_strIsVcsSyncFlag.empty())
				{
					p_pJson->SetNodeValue("/body/vcs_sync_flag", m_oBody.m_strIsVcsSyncFlag);
				}
				

				//std::string l_strPrefixPath("/body/alarm/");
				p_pJson->SetNodeValue("/body/is_edit", m_oBody.m_strIsEdit);
				std::string l_strPrefixPath("/body/alarm");
				m_oBody.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
				//p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarm.m_strMsgSource);


				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);

				//p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receiving_time", m_oBody.m_oAlarm.m_strTime);  //time变更字段名
				//p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_oAlarm.m_strReceivedTime);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "first_submit_time", m_oBody.m_oAlarm.m_strFirstSubmitTime); //first_submit_time  服务自己记的	
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_user_name", m_oBody.m_oAlarm.m_strCallerUserName); //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr", m_oBody.m_oAlarm.m_strAlarmAddr);  //新增  报警地址
				//p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "erpetrators_number", m_oBody.m_oAlarm.m_strErpetratorsNumber); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_armed", m_oBody.m_oAlarm.m_strIsArmed);  //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_substances", m_oBody.m_oAlarm.m_strIsHazardousSubstances);  //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_explosion_or_leakage", m_oBody.m_oAlarm.m_strIsExplosionOrLeakage); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_trapped", m_oBody.m_oAlarm.m_strDescOfTrapped); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_injured", m_oBody.m_oAlarm.m_strDescOfInjured); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_dead", m_oBody.m_oAlarm.m_strDescOfDead); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_foreign_language", m_oBody.m_oAlarm.m_strIsForeignLanguage); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "manual_longitude", m_oBody.m_oAlarm.m_strManualLongitude); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "manual_latitude", m_oBody.m_oAlarm.m_strManualLatitude);//新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_oAlarm.m_strRemark);
				//p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				//p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				//p_pJson->SetNodeValue(l_strPrefixPath + "emergency_rescue_level", m_oBody.m_oAlarm.m_strEmergencyRescueLevel); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_vehicle", m_oBody.m_oAlarm.m_strIsHazardousVehicle); //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "label", m_oBody.m_oAlarm.m_strTitle);  //title变更字段名
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", m_oBody.m_oAlarm.m_strPrivacy);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_srv_name", m_oBody.m_oAlarm.m_strReceiptSrvName);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_org_code", m_oBody.m_oAlarm.m_strAdminDeptOrgCode);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_org_code", m_oBody.m_oAlarm.m_strReceiptDeptOrgCode);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_delete", m_oBody.m_oAlarm.m_strDeleteFlag);//is_delete不在协议中传递
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_invalid", m_oBody.m_oAlarm.m_strIsInvalid);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "business_status", m_oBody.m_oAlarm.m_strBusinessState);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_seatno", m_oBody.m_oAlarm.m_strSeatNo);   //seatno改名
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_oAlarm.m_strIsMerge);
				//p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", m_oBody.m_oAlarm.m_strIsVisitor);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", m_oBody.m_oAlarm.m_strIsFeedBack);
				// 
				//p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarm.m_strMsgSource);
				//p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "seatno", m_oBody.m_oAlarm.m_strSeatNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_oAlarm.m_strTitle);
				//p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				//p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_oAlarm.m_strTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_oAlarm.m_strActualOccurTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				//p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_oAlarm.m_strSymbolAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_oAlarm.m_strFireBuildingType);

				//p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_oAlarm.m_strEventType);

				//p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_oAlarm.m_strActualCalledNoType);

				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_oAlarm.m_strCallerAge);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_oAlarm.m_strCallerBirthday);

				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_oAlarm.m_strContactName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_oAlarm.m_strContactAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_oAlarm.m_strContactID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_oAlarm.m_strContactIDType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_oAlarm.m_strContactGender);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_oAlarm.m_strContactAge);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_oAlarm.m_strContactBirthday);

				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_oAlarm.m_strAdminDeptDistrictCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);

				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_oAlarm.m_strLeaderCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_oAlarm.m_strLeaderName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);

				//p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_oAlarm.m_strDispatchSuggestion);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_oAlarm.m_strIsMerge);

				//p_pJson->SetNodeValue(l_strPrefixPath + "operator_code", m_oBody.m_oAlarm.m_strOperatorCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "operator_name", m_oBody.m_oAlarm.m_strOperatorName);

				//p_pJson->SetNodeValue(l_strPrefixPath + "city_code", m_oBody.m_oAlarm.m_strCityCode);

				//p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);
				//
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", m_oBody.m_oAlarm.m_strPrivacy);
				//p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_oAlarm.m_strRemark);

				////2022/04/02 新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_oAlarm.m_strReceivedTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_user_name", m_oBody.m_oAlarm.m_strCallerUserName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr", m_oBody.m_oAlarm.m_strAlarmAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "erpetrators_number", m_oBody.m_oAlarm.m_strErpetratorsNumber);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_armed", m_oBody.m_oAlarm.m_strIsArmed);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_substances", m_oBody.m_oAlarm.m_strIsHazardousSubstances);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_explosion_or_leakage", m_oBody.m_oAlarm.m_strIsExplosionOrLeakage);
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_trapped", m_oBody.m_oAlarm.m_strDescOfTrapped);
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_injured", m_oBody.m_oAlarm.m_strDescOfInjured);
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_dead", m_oBody.m_oAlarm.m_strDescOfDead);

				//p_pJson->SetNodeValue(l_strPrefixPath + "is_foreign_language", m_oBody.m_oAlarm.m_strIsForeignLanguage);

				//p_pJson->SetNodeValue(l_strPrefixPath +"manual_longitude", m_oBody.m_oAlarm.m_strManualLongitude);
				//p_pJson->SetNodeValue(l_strPrefixPath +"manual_latitude", m_oBody.m_oAlarm.m_strManualLatitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "emergency_rescue_level", m_oBody.m_oAlarm.m_strEmergencyRescueLevel);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_vehicle", m_oBody.m_oAlarm.m_strIsHazardousVehicle);

				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_srv_name", m_oBody.m_oAlarm.m_strReceiptSrvName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "business_status", m_oBody.m_oAlarm.m_strBusinessState);

				unsigned int l_uiIndex = 0;
				for (CAddOrUpdateProcessRequest::CProcessData data : m_oBody.m_vecProcessData)
				{
					std::string l_strPrefixPath("/body/process_data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);
					
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", data.m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", data.m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_id", data.m_strRecordID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", data.m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", data.m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", data.m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", data.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", data.m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", data.m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", data.m_strDispatchPersonnel);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", data.m_strDispatchVehicles);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", data.m_strDispatchBoats);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", data.m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", data.m_strDispatchDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", data.m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", data.m_strProcessDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", data.m_strProcessObjectType);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", data.m_strProcessObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", data.m_strProcessObjectCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "business_status", data.m_strBusinessStatus);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", data.m_strSeatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", data.m_strCancelTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", data.m_strCancelReason);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", data.m_strIsAutoAssignJob);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "overtime_state", data.m_strOvertimeState);

					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", data.m_strProcessObjectID);
					p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", data.m_strTransfDeptOrjCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_over", data.m_strIsOver);
					p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", data.m_strOverRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", data.m_strParentID);
					p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", data.m_strGZLDM);

					p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", data.m_strCreateTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", data.m_strUpdateTeminal);

					++l_uiIndex;
				}

				l_uiIndex = 0;
				for (CAddOrUpdateLinkedRequest::CLinkedData l_data : m_oBody.m_vecLinkedData)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "id", l_data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", l_data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", l_data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", l_data.m_strLinkedOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", l_data.m_strLinkedOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", l_data.m_strLinkedOrgType);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", l_data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", l_data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", l_data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", l_data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", l_data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", l_data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "result", l_data.m_strResult);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			bool ParseString4AddAlarm(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strSaveFlag = p_pJson->GetNodeValue("/body/save_flag", "");
				m_oBody.m_strIsEdit = p_pJson->GetNodeValue("/body/is_edit", "");
				m_oBody.m_strIsVcsSyncFlag = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");

				m_oBody.m_strSupplement = p_pJson->GetNodeValue("/body/supplement","");
				m_oBody.m_oAlarm.ParseString("/body", p_pJson);
				m_oBody.m_oAlarm.m_strLevel = m_oBody.m_oAlarm.m_strLevel.empty() ? "04" : m_oBody.m_oAlarm.m_strLevel;
				//m_oBody.m_oAlarm.m_strMsgSource = p_pJson->GetNodeValue("/body/alarm/msg_source", "");

				//m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_district_code", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_code", "");
				//m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/alarm/id", "");
				//m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/alarm/merge_id", "");
				//m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/alarm/called_no_type", "");
				//m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/alarm/source_type", "");
				//m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/alarm/handle_type", "");
				//m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue("/body/alarm/source_id", "");
				//m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/alarm/receipt_code", "");
				//m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/alarm/receipt_name", "");
				//m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/receiving_time", "");  //time变更字段名
				//m_oBody.m_oAlarm.m_strReceivedTime = p_pJson->GetNodeValue("/body/received_time", "");  //新增
				////first_submit_time  服务自己记的
				//m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/alarm/caller_no", "");
				//m_oBody.m_oAlarm.m_strCallerUserName = p_pJson->GetNodeValue("/body/caller_user_name", ""); //新增
				//m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/alarm/caller_addr", "");
				//m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue("/body/alarm/caller_name", "");
				//m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue("/body/alarm/caller_gender", "");
				//m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				//m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/alarm/caller_id_type", "");
				//m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/alarm/caller_id", "");
				//m_oBody.m_oAlarm.m_strAlarmAddr = p_pJson->GetNodeValue("/body/alarm/alarm_addr", "");  //新增  报警地址
				//m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", "");
				//m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				//m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_code", "");
				//m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				//m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				//m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				//m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");
				//m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/alarm/symbol_code", "");
				//m_oBody.m_oAlarm.m_strErpetratorsNumber = p_pJson->GetNodeValue("/body/erpetrators_number", ""); //新增  
				//m_oBody.m_oAlarm.m_strIsArmed = p_pJson->GetNodeValue("/body/is_armed", "");  //新增  
				//m_oBody.m_oAlarm.m_strIsHazardousSubstances = p_pJson->GetNodeValue("/body/is_hazardous_substances", "");  //新增  
				//m_oBody.m_oAlarm.m_strIsExplosionOrLeakage = p_pJson->GetNodeValue("/body/is_explosion_or_leakage", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfTrapped = p_pJson->GetNodeValue("/body/desc_of_trapped", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfInjured = p_pJson->GetNodeValue("/body/desc_of_injured", ""); //新增  
				//m_oBody.m_oAlarm.m_strDescOfDead = p_pJson->GetNodeValue("/body/desc_of_dead", ""); //新增  
				//m_oBody.m_oAlarm.m_strIsForeignLanguage = p_pJson->GetNodeValue("/body/is_foreign_language", ""); //新增  
				//m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/alarm/longitude", "");
				//m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/alarm/latitude", "");
				//m_oBody.m_oAlarm.m_strManualLongitude = p_pJson->GetNodeValue("/body/alarm/manual_longitude", ""); //新增  
				//m_oBody.m_oAlarm.m_strManualLatitude = p_pJson->GetNodeValue("/body/alarm/manual_latitude", "");//新增  
				//m_oBody.m_oAlarm.m_strRemark = p_pJson->GetNodeValue("/body/alarm/remark", "");
				//m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "").empty() ? "04" : p_pJson->GetNodeValue("/body/alarm/level", "");
				//m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				//m_oBody.m_oAlarm.m_strEmergencyRescueLevel = p_pJson->GetNodeValue("/body/emergency_rescue_level", ""); //新增  
				//m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue("/body/alarm/vehicle_type", "");
				//m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/alarm/vehicle_no", "");
				//m_oBody.m_oAlarm.m_strIsHazardousVehicle = p_pJson->GetNodeValue("/body/is_hazardous_vehicle", ""); //新增
				//m_oBody.m_oAlarm.m_strCreateTime = p_pJson->GetNodeValue("/body/alarm/create_time", "");
				//m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/alarm/update_time", "");
				//m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/label", "");  //title变更字段名
				//m_oBody.m_oAlarm.m_strPrivacy = p_pJson->GetNodeValue("/body/alarm/is_privacy", "");
				//m_oBody.m_oAlarm.m_strReceiptSrvName = p_pJson->GetNodeValue("/body/receipt_srv_name", "");  //新增
				//m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/alarm/admin_dept_name", "");
				//m_oBody.m_oAlarm.m_strAdminDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_org_code", "");   //新增
				//m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/alarm/receipt_dept_name", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_org_code", "");   //新增
				////is_delete不在协议中传递
				//m_oBody.m_oAlarm.m_strIsInvalid = p_pJson->GetNodeValue("/body/alarm/is_invalid", "");   //新增
				//m_oBody.m_oAlarm.m_strBusinessState = p_pJson->GetNodeValue("/body/business_status", "");  //新增
				//m_oBody.m_oAlarm.m_strSeatNo = p_pJson->GetNodeValue("/body/alarm/receipt_seatno", "");   //seatno改名
				//m_oBody.m_oAlarm.m_strIsMerge = p_pJson->GetNodeValue("/body/alarm/is_merge", "");
				//m_oBody.m_oAlarm.m_strCreateUser = p_pJson->GetNodeValue("/body/alarm/create_user", "");
				//m_oBody.m_oAlarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/alarm/update_user", "");
				return true;
			}
		public:
			class CBody
			{
			public:
				CAlarmInfo m_oAlarm;
				std::string m_strSaveFlag;//1:暂存

				std::string m_strIsEdit; //是否编辑，值为1时才写BS001002024流水，否则不写

				std::string m_strIsVcsSyncFlag; //VCS 1:补偿新增,2:补偿更新

				std::string m_strAllowNull;   //前端传1时，desc_of_dead，desc_of_injured，desc_of_trapped，caller_no，label为空

				std::string m_strSupplement; 
				std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
				std::vector<CAddOrUpdateLinkedRequest::CLinkedData> m_vecLinkedData;
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
			
		};	

		typedef std::shared_ptr<CAddOrUpdateAlarmWithProcessRequest> AddOrUpdateAlarmWithProcessRequestPtr;
	}
}
