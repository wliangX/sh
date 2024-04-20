#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateAlarmRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_oAlarm.m_strMsgSource = p_pJson->GetNodeValue("/body/alarm/msg_source", "");


				m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_district_code", "");
				m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_code", "");
				m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/alarm/id", "");
				m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/alarm/merge_id", "");
				m_oBody.m_oAlarm.m_strMergeType = p_pJson->GetNodeValue("/body/alarm/merge_type", "");
				m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/alarm/called_no_type", "");
				m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/alarm/source_type", "");
				m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/alarm/handle_type", "");
				m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue("/body/alarm/source_id", "");
				m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/alarm/receipt_code", "");
				m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/alarm/receipt_name", "");
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/receiving_time", "");  //time变更字段名
				m_oBody.m_oAlarm.m_strReceivedTime = p_pJson->GetNodeValue("/body/received_time", "");  //新增
				//first_submit_time  服务自己记的
				m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/alarm/caller_no", "");
				m_oBody.m_oAlarm.m_strCallerUserName = p_pJson->GetNodeValue("/body/caller_user_name", ""); //新增
				m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/alarm/caller_addr", "");
				m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue("/body/alarm/caller_name", "");
				m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue("/body/alarm/caller_gender", "");
				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/alarm/caller_id_type", "");
				m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/alarm/caller_id", "");
				m_oBody.m_oAlarm.m_strAlarmAddr = p_pJson->GetNodeValue("/body/alarm/alarm_addr", "");  //新增  报警地址
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", "");
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_code", "");
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");
				m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/alarm/symbol_code", "");
				m_oBody.m_oAlarm.m_strErpetratorsNumber = p_pJson->GetNodeValue("/body/erpetrators_number", ""); //新增  
				m_oBody.m_oAlarm.m_strIsArmed = p_pJson->GetNodeValue("/body/is_armed", "");  //新增  
				m_oBody.m_oAlarm.m_strIsHazardousSubstances = p_pJson->GetNodeValue("/body/is_hazardous_substances", "");  //新增  
				m_oBody.m_oAlarm.m_strIsExplosionOrLeakage = p_pJson->GetNodeValue("/body/is_explosion_or_leakage", ""); //新增  
				m_oBody.m_oAlarm.m_strDescOfTrapped = p_pJson->GetNodeValue("/body/desc_of_trapped", ""); //新增  
				m_oBody.m_oAlarm.m_strDescOfInjured = p_pJson->GetNodeValue("/body/desc_of_injured", ""); //新增  
				m_oBody.m_oAlarm.m_strDescOfDead = p_pJson->GetNodeValue("/body/desc_of_dead", ""); //新增  
				m_oBody.m_oAlarm.m_strIsForeignLanguage = p_pJson->GetNodeValue("/body/is_foreign_language", ""); //新增  
				m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/alarm/longitude", "");
				m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/alarm/latitude", "");
				m_oBody.m_oAlarm.m_strManualLongitude = p_pJson->GetNodeValue("/body/alarm/manual_longitude", ""); //新增  
				m_oBody.m_oAlarm.m_strManualLatitude = p_pJson->GetNodeValue("/body/alarm/manual_latitude", "");//新增  
				m_oBody.m_oAlarm.m_strRemark = p_pJson->GetNodeValue("/body/alarm/remark", "");
				m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "").empty() ? "04" : p_pJson->GetNodeValue("/body/alarm/level", "");
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				m_oBody.m_oAlarm.m_strEmergencyRescueLevel = p_pJson->GetNodeValue("/body/emergency_rescue_level", ""); //新增  
				m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue("/body/alarm/vehicle_type", "");
				m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/alarm/vehicle_no", "");
				m_oBody.m_oAlarm.m_strIsHazardousVehicle = p_pJson->GetNodeValue("/body/is_hazardous_vehicle", ""); //新增
				m_oBody.m_oAlarm.m_strCreateTime = p_pJson->GetNodeValue("/body/alarm/create_time", "");
				m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/alarm/update_time", "");
				m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/label", "");  //title变更字段名
				m_oBody.m_oAlarm.m_strPrivacy = p_pJson->GetNodeValue("/body/alarm/is_privacy", "");
				m_oBody.m_oAlarm.m_strReceiptSrvName = p_pJson->GetNodeValue("/body/receipt_srv_name", "");  //新增
				m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/alarm/admin_dept_name", "");
				m_oBody.m_oAlarm.m_strAdminDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_org_code", "");   //新增
				m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/alarm/receipt_dept_name", "");
				m_oBody.m_oAlarm.m_strReceiptDeptOrgCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_org_code", "");   //新增
				//is_delete不在协议中传递
				m_oBody.m_oAlarm.m_strIsInvalid = p_pJson->GetNodeValue("/body/alarm/is_invalid", "");   //新增
				m_oBody.m_oAlarm.m_strBusinessState = p_pJson->GetNodeValue("/body/business_status", "");  //新增
				m_oBody.m_oAlarm.m_strSeatNo = p_pJson->GetNodeValue("/body/alarm/receipt_seatno", "");   //seatno改名
				m_oBody.m_oAlarm.m_strIsMerge = p_pJson->GetNodeValue("/body/alarm/is_merge", "");
				m_oBody.m_oAlarm.m_strCreateUser = p_pJson->GetNodeValue("/body/alarm/create_user", "");
				m_oBody.m_oAlarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/alarm/update_user", "");
				/*m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/alarm/id", "");
				m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/alarm/merge_id", "");
				m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/title", "");
				m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/time", "");
				m_oBody.m_oAlarm.m_strActualOccurTime = p_pJson->GetNodeValue("/body/alarm/actual_occur_time", "");
				m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", "");
				m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/alarm/longitude", "");
				m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/alarm/latitude", "");
				m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "");
				m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/alarm/source_type", "");
				m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue("/body/alarm/source_id", "");
				m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/alarm/handle_type", "");
				m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");
				m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/alarm/vehicle_no", "");
				m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue("/body/alarm/vehicle_type", "");
				m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/alarm/symbol_code", "");
				m_oBody.m_oAlarm.m_strSymbolAddr = p_pJson->GetNodeValue("/body/alarm/symbol_addr", "");
				m_oBody.m_oAlarm.m_strFireBuildingType = p_pJson->GetNodeValue("/body/alarm/fire_building_type", "");

				m_oBody.m_oAlarm.m_strEventType = p_pJson->GetNodeValue("/body/alarm/event_type", "");

				m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/alarm/called_no_type", "");
				m_oBody.m_oAlarm.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/alarm/actual_called_no_type", "");

				m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/alarm/caller_no", "");
				m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue("/body/alarm/caller_name", "");
				m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/alarm/caller_addr", "");
				m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/alarm/caller_id", "");
				m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/alarm/caller_id_type", "");
				m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue("/body/alarm/caller_gender", "");
				m_oBody.m_oAlarm.m_strCallerAge = p_pJson->GetNodeValue("/body/alarm/caller_age", "");
				m_oBody.m_oAlarm.m_strCallerBirthday = p_pJson->GetNodeValue("/body/alarm/caller_birthday", "");

				m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/alarm/contact_name", "");
				m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue("/body/alarm/contact_addr", "");
				m_oBody.m_oAlarm.m_strContactID = p_pJson->GetNodeValue("/body/alarm/contact_id", "");
				m_oBody.m_oAlarm.m_strContactIDType = p_pJson->GetNodeValue("/body/alarm/contact_id_type", "");
				m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue("/body/alarm/contact_gender", "");
				m_oBody.m_oAlarm.m_strContactAge = p_pJson->GetNodeValue("/body/alarm/contact_age", "");
				m_oBody.m_oAlarm.m_strContactBirthday = p_pJson->GetNodeValue("/body/alarm/contact_birthday", "");

				m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_district_code", "");
				m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_code", "");
				m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/alarm/admin_dept_name", "");

				m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_district_code", "");
				m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_code", "");
				m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/alarm/receipt_dept_name", "");
				m_oBody.m_oAlarm.m_strLeaderCode = p_pJson->GetNodeValue("/body/alarm/leader_code", "");
				m_oBody.m_oAlarm.m_strLeaderName = p_pJson->GetNodeValue("/body/alarm/leader_name", "");
				m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/alarm/receipt_code", "");
				m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/alarm/receipt_name", "");

				m_oBody.m_oAlarm.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/alarm/dispatch_suggestion", "");

				m_oBody.m_oAlarm.m_strCreateUser = p_pJson->GetNodeValue("/body/alarm/create_user", "");
				m_oBody.m_oAlarm.m_strCreateTime = p_pJson->GetNodeValue("/body/alarm/create_time", "");
				m_oBody.m_oAlarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/alarm/update_user", "");
				m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/alarm/update_time", "");				
				m_oBody.m_oAlarm.m_strPrivacy = p_pJson->GetNodeValue("/body/alarm/is_privacy", "");
				m_oBody.m_oAlarm.m_strRemark = p_pJson->GetNodeValue("/body/alarm/remark", "");*/

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/alarm/");

				/*p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_oAlarm.m_strTitle);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_oAlarm.m_strTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_oAlarm.m_strActualOccurTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_oAlarm.m_strSymbolAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_oAlarm.m_strFireBuildingType);

				p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_oAlarm.m_strEventType);

				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_oAlarm.m_strActualCalledNoType);

				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_oAlarm.m_strCallerAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_oAlarm.m_strCallerBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_oAlarm.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_oAlarm.m_strContactAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_oAlarm.m_strContactID);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_oAlarm.m_strContactIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_oAlarm.m_strContactGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_oAlarm.m_strContactAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_oAlarm.m_strContactBirthday);

				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_oAlarm.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_oAlarm.m_strLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_oAlarm.m_strLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);

				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_oAlarm.m_strDispatchSuggestion);

				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", m_oBody.m_oAlarm.m_strPrivacy);
				p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_oAlarm.m_strRemark);*/
				p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarm.m_strMsgSource);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_type", m_oBody.m_oAlarm.m_strMergeType);
				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);

				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receiving_time", m_oBody.m_oAlarm.m_strTime);  //time变更字段名
				p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_oAlarm.m_strReceivedTime);  //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "first_submit_time", m_oBody.m_oAlarm.m_strFirstSubmitTime); //first_submit_time  服务自己记的	
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_user_name", m_oBody.m_oAlarm.m_strCallerUserName); //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr", m_oBody.m_oAlarm.m_strAlarmAddr);  //新增  报警地址
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "erpetrators_number", m_oBody.m_oAlarm.m_strErpetratorsNumber); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "is_armed", m_oBody.m_oAlarm.m_strIsArmed);  //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_substances", m_oBody.m_oAlarm.m_strIsHazardousSubstances);  //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "is_explosion_or_leakage", m_oBody.m_oAlarm.m_strIsExplosionOrLeakage); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_trapped", m_oBody.m_oAlarm.m_strDescOfTrapped); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_injured", m_oBody.m_oAlarm.m_strDescOfInjured); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_dead", m_oBody.m_oAlarm.m_strDescOfDead); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "is_foreign_language", m_oBody.m_oAlarm.m_strIsForeignLanguage); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "manual_longitude", m_oBody.m_oAlarm.m_strManualLongitude); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "manual_latitude", m_oBody.m_oAlarm.m_strManualLatitude);//新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_oAlarm.m_strRemark);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "emergency_rescue_level", m_oBody.m_oAlarm.m_strEmergencyRescueLevel); //新增  
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_vehicle", m_oBody.m_oAlarm.m_strIsHazardousVehicle); //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "label", m_oBody.m_oAlarm.m_strTitle);  //title变更字段名
				p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", m_oBody.m_oAlarm.m_strPrivacy);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_srv_name", m_oBody.m_oAlarm.m_strReceiptSrvName);  //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_org_code", m_oBody.m_oAlarm.m_strAdminDeptOrgCode);   //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_org_code", m_oBody.m_oAlarm.m_strReceiptDeptOrgCode);   //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "is_delete", m_oBody.m_oAlarm.m_strDeleteFlag);//is_delete不在协议中传递
				p_pJson->SetNodeValue(l_strPrefixPath + "is_invalid", m_oBody.m_oAlarm.m_strIsInvalid);   //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "business_status", m_oBody.m_oAlarm.m_strBusinessState);  //新增
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_seatno", m_oBody.m_oAlarm.m_strSeatNo);   //seatno改名
				p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_oAlarm.m_strIsMerge);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", m_oBody.m_oAlarm.m_strIsVisitor);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", m_oBody.m_oAlarm.m_strIsFeedBack);
				return p_pJson->ToString();
			}

		public:
			//class CAlarm
			//{
			//public:
			//	std::string	m_strMsgSource;					//消息来源
			//	std::string	m_strID;						//警情id
			//	std::string	m_strMergeID;					//警情合并id
			//	std::string m_strSeatNo;					//接警坐席号
			//	std::string m_strTitle;						//警情标题
			//	std::string m_strContent;					//警情内容
			//	std::string m_strTime;						//警情id报警时间			
			//	std::string m_strActualOccurTime;			//警情id实际发生时间
			//	std::string m_strAddr;						//警情id详细发生地址
			//	std::string m_strLongitude;					//警情id经度
			//	std::string m_strLatitude;					//警情id维度
			//	std::string m_strState;						//警情id状态
			//	std::string m_strLevel;						//警情id级别
			//	std::string m_strSourceType;				//警情id报警来源类型
			//	std::string m_strSourceID;					//警情id报警来源id
			//	std::string m_strHandleType;				//警情id处理类型
			//	std::string m_strFirstType;					//警情id一级类型
			//	std::string m_strSecondType;				//警情id二级类型
			//	std::string m_strThirdType;					//警情id三级类型
			//	std::string m_strFourthType;				//警情id四级类型
			//	std::string m_strVehicleNo;					//警情id交通类型报警车牌号
			//	std::string m_strVehicleType;				//警情id交通类型报警车类型
			//	std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
			//	std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
			//	std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

			//	std::string m_strEventType;					//事件类型，逗号隔开

			//	std::string m_strCalledNoType;				//警情id报警号码字典类型
			//	std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

			//	std::string m_strCallerNo;					//警情id报警人号码
			//	std::string m_strCallerName;				//警情id报警人姓名
			//	std::string m_strCallerAddr;				//警情id报警人地址
			//	std::string m_strCallerID;					//警情id报警人身份证
			//	std::string m_strCallerIDType;				//警情id报警人身份证类型
			//	std::string m_strCallerGender;				//警情id报警人性别
			//	std::string m_strCallerAge;					//警情id报警人年龄
			//	std::string m_strCallerBirthday;			//警情id报警人出生年月日

			//	std::string m_strContactNo;					//警情id联系人号码
			//	std::string m_strContactName;				//警情id联系人姓名
			//	std::string m_strContactAddr;				//警情id联系人地址
			//	std::string m_strContactID;					//警情id联系人身份证
			//	std::string m_strContactIDType;				//警情id联系人身份证类型
			//	std::string m_strContactGender;				//警情id联系人性别
			//	std::string m_strContactAge;				//警情id联系人年龄
			//	std::string m_strContactBirthday;			//警情id联系人出生年月日

			//	std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
			//	std::string m_strAdminDeptCode;				//警情id管辖单位编码
			//	std::string m_strAdminDeptName;				//警情id管辖单位姓名

			//	std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
			//	std::string m_strReceiptDeptCode;			//警情id接警单位编码
			//	std::string m_strReceiptDeptName;			//警情id接警单位名称
			//	std::string m_strLeaderCode;				//警情id值班领导警号
			//	std::string m_strLeaderName;				//警情id值班领导姓名
			//	std::string m_strReceiptCode;				//警情id接警人警号
			//	std::string m_strReceiptName;				//警情id接警人姓名

			//	std::string m_strDispatchSuggestion;		//调派意见
			//	std::string m_strIsMerge;					//是否合并

			//	std::string m_strCreateUser;				//创建人
			//	std::string m_strCreateTime;				//创建时间
			//	std::string m_strUpdateUser;				//修改人,取最后一次修改值
			//	std::string m_strUpdateTime;				//修改时间,取最后一次修改值		
			//	std::string m_strPrivacy;					//隐私保护  true or false
			//	std::string m_strRemark;				    //补充信息
			//};

			class CBody
			{
			public:
				CAlarmInfo m_oAlarm;
			};

			CHeader m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
