#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAddOrUpdateLinkedRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{		
		class CGetFeedBackRespond :
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

                std::string l_strAlarmPrefixPath("/body/alarm");
                m_oBody.m_oAlarm.ComJson(l_strAlarmPrefixPath, p_pJson);
               /* p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "seatno", m_oBody.m_oAlarm.m_strReceiptSeatno);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "title", m_oBody.m_oAlarm.m_strTitle);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", m_oBody.m_oAlarm.m_strTime);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_occur_time", m_oBody.m_oAlarm.m_strActualOccurTime);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_addr", m_oBody.m_oAlarm.m_strSymbolAddr);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fire_building_type", m_oBody.m_oAlarm.m_strFireBuildingType);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "event_type", m_oBody.m_oAlarm.m_strEventType);


                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_called_no_type", m_oBody.m_oAlarm.m_strActualCalledNoType);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_age", m_oBody.m_oAlarm.m_strCallerAge);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_birthday", m_oBody.m_oAlarm.m_strCallerBirthday);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_name", m_oBody.m_oAlarm.m_strContactName);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_addr", m_oBody.m_oAlarm.m_strContactAddr);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id", m_oBody.m_oAlarm.m_strContactID);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id_type", m_oBody.m_oAlarm.m_strContactIDType);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_gender", m_oBody.m_oAlarm.m_strContactGender);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_age", m_oBody.m_oAlarm.m_strContactAge);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_birthday", m_oBody.m_oAlarm.m_strContactBirthday);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_district_code", m_oBody.m_oAlarm.m_strAdminDeptDistrictCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_code", m_oBody.m_oAlarm.m_strLeaderCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_name", m_oBody.m_oAlarm.m_strLeaderName);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "dispatch_suggestion", m_oBody.m_oAlarm.m_strDispatchSuggestion);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_merge", m_oBody.m_oAlarm.m_strIsMerge);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);

                p_pJson->SetNodeValue(l_strAlarmPrefixPath + "city_code", m_oBody.m_oAlarm.m_strCityCode);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_privacy", m_oBody.m_oAlarm.m_strPrivacy);
				p_pJson->SetNodeValue(l_strAlarmPrefixPath + "remark", m_oBody.m_oAlarm.m_strRemark);*/

                for (unsigned int i = 0; i < m_oBody.m_vecFeedbackData.size(); i++)
                {
                    std::string l_strPrefixPath("/body/feedback_data/" + std::to_string(i) + "/");
                    p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_vecFeedbackData.at(i).m_strID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_vecFeedbackData.at(i).m_strAlarmID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_id", m_oBody.m_vecFeedbackData.at(i).m_strProcessID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "result_type", m_oBody.m_vecFeedbackData.at(i).m_strResultType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "result_content", m_oBody.m_vecFeedbackData.at(i).m_strResultContent);
                    p_pJson->SetNodeValue(l_strPrefixPath + "leader_instruction", m_oBody.m_vecFeedbackData.at(i).m_strLeaderInstruction);
                    p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_vecFeedbackData.at(i).m_strState);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_edit", m_oBody.m_vecFeedbackData.at(i).m_strTimeEdit);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", m_oBody.m_vecFeedbackData.at(i).m_strTimeSubmit);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", m_oBody.m_vecFeedbackData.at(i).m_strTimeArrived);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", m_oBody.m_vecFeedbackData.at(i).m_strTimeSigned);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_police_dispatch", m_oBody.m_vecFeedbackData.at(i).m_strTimePoliceDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "time_police_arrived", m_oBody.m_vecFeedbackData.at(i).m_strTimePoliceArrived);
                    p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_vecFeedbackData.at(i).m_strActualOccurTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_addr", m_oBody.m_vecFeedbackData.at(i).m_strActualOccurAddr);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_district_code", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_code", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_name", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_code", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_name", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_leader_code", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedback_leader_name", m_oBody.m_vecFeedbackData.at(i).m_strFeedbackLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_district_code", m_oBody.m_vecFeedbackData.at(i).m_strProcessDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", m_oBody.m_vecFeedbackData.at(i).m_strProcessDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", m_oBody.m_vecFeedbackData.at(i).m_strProcessDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_code", m_oBody.m_vecFeedbackData.at(i).m_strProcessCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_name", m_oBody.m_vecFeedbackData.at(i).m_strProcessName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_code", m_oBody.m_vecFeedbackData.at(i).m_strProcessLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_name", m_oBody.m_vecFeedbackData.at(i).m_strProcessLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", m_oBody.m_vecFeedbackData.at(i).m_strDispatchDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", m_oBody.m_vecFeedbackData.at(i).m_strDispatchDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", m_oBody.m_vecFeedbackData.at(i).m_strDispatchDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", m_oBody.m_vecFeedbackData.at(i).m_strDispatchCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", m_oBody.m_vecFeedbackData.at(i).m_strDispatchName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_code", m_oBody.m_vecFeedbackData.at(i).m_strDispatchLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_name", m_oBody.m_vecFeedbackData.at(i).m_strDispatchLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_id", m_oBody.m_vecFeedbackData.at(i).m_strPersonId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_id_type", m_oBody.m_vecFeedbackData.at(i).m_strPersonIdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_nationality", m_oBody.m_vecFeedbackData.at(i).m_strPersonNationality);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_name", m_oBody.m_vecFeedbackData.at(i).m_strPersonName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_id", m_oBody.m_vecFeedbackData.at(i).m_strPersonSlaveId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_id_type", m_oBody.m_vecFeedbackData.at(i).m_strPersonSlaveIdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_nationality", m_oBody.m_vecFeedbackData.at(i).m_strPersonSlaveNationality);
                    p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_name", m_oBody.m_vecFeedbackData.at(i).m_strPersonSlaveName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_first_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_second_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_third_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_fourth_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmFourthType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_dept_name", m_oBody.m_vecFeedbackData.at(i).m_strAlarmAddrDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_first_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmAddrFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_second_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmAddrSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_third_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmAddrThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_longitude", m_oBody.m_vecFeedbackData.at(i).m_strAlarmLongitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_latitude", m_oBody.m_vecFeedbackData.at(i).m_strAlarmLatitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_region_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmRegionType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_location_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmLocationType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "people_num_capture", m_oBody.m_vecFeedbackData.at(i).m_strPeopleNumCapture);
                    p_pJson->SetNodeValue(l_strPrefixPath + "people_num_rescue", m_oBody.m_vecFeedbackData.at(i).m_strPeopleNumRescue);
                    p_pJson->SetNodeValue(l_strPrefixPath + "people_num_slight_injury", m_oBody.m_vecFeedbackData.at(i).m_strPeopleNumSlightInjury);
                    p_pJson->SetNodeValue(l_strPrefixPath + "people_num_serious_injury", m_oBody.m_vecFeedbackData.at(i).m_strPeopleNumSeriousInjury);
                    p_pJson->SetNodeValue(l_strPrefixPath + "people_num_death", m_oBody.m_vecFeedbackData.at(i).m_strPeopleNumDeath);
                    p_pJson->SetNodeValue(l_strPrefixPath + "police_num_dispatch", m_oBody.m_vecFeedbackData.at(i).m_strPoliceNumDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "police_car_num_dispatch", m_oBody.m_vecFeedbackData.at(i).m_strPoliceCarNumDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "economy_loss", m_oBody.m_vecFeedbackData.at(i).m_strEconomyLoss);
                    p_pJson->SetNodeValue(l_strPrefixPath + "retrieve_economy_loss", m_oBody.m_vecFeedbackData.at(i).m_strRetrieveEconomyLoss);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_put_out_time", m_oBody.m_vecFeedbackData.at(i).m_strFirePutOutTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_first_type", m_oBody.m_vecFeedbackData.at(i).m_strFireBuildingFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_second_type", m_oBody.m_vecFeedbackData.at(i).m_strFireBuildingSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_third_type", m_oBody.m_vecFeedbackData.at(i).m_strFireBuildingThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_source_type", m_oBody.m_vecFeedbackData.at(i).m_strFireSourceType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_region_type", m_oBody.m_vecFeedbackData.at(i).m_strFireRegionType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_first_type", m_oBody.m_vecFeedbackData.at(i).m_strFireCauseFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_second_type", m_oBody.m_vecFeedbackData.at(i).m_strFireCauseSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_third_type", m_oBody.m_vecFeedbackData.at(i).m_strFireCauseThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fire_area", m_oBody.m_vecFeedbackData.at(i).m_strFireArea);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_road_level", m_oBody.m_vecFeedbackData.at(i).m_strTrafficRoadLevel);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_accident_level", m_oBody.m_vecFeedbackData.at(i).m_strTrafficAccidentLevel);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_vehicle_no", m_oBody.m_vecFeedbackData.at(i).m_strTrafficVehicleNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_vehicle_type", m_oBody.m_vecFeedbackData.at(i).m_strTrafficVehicleType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_slave_vehicle_no", m_oBody.m_vecFeedbackData.at(i).m_strTrafficSlaveVehicleNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "traffic_slave_vehicle_type", m_oBody.m_vecFeedbackData.at(i).m_strTrafficSlaveVehicleType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_vecFeedbackData.at(i).m_strEventType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "alarm_called_no_type", m_oBody.m_vecFeedbackData.at(i).m_strAlarmCalledNoType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_vecFeedbackData.at(i).m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_vecFeedbackData.at(i).m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_vecFeedbackData.at(i).m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_vecFeedbackData.at(i).m_strUpdateTime);
                }

				for (unsigned int i = 0; i < m_oBody.m_vecProcessData.size(); i++)
                {
                    std::string l_strPrefixPathProcess("/body/process_data/" + std::to_string(i) + "/");
                   
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_code", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", m_oBody.m_vecProcessData.at(i).m_strID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", m_oBody.m_vecProcessData.at(i).m_strAlarmID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_code", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", m_oBody.m_vecProcessData.at(i).m_strDispatchCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", m_oBody.m_vecProcessData.at(i).m_strDispatchName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "record_id", m_oBody.m_vecProcessData.at(i).m_strRecordID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_suggestion", m_oBody.m_vecProcessData.at(i).m_strDispatchSuggestion);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_code", m_oBody.m_vecProcessData.at(i).m_strProcessDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_submit", m_oBody.m_vecProcessData.at(i).m_strTimeSubmit);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_arrived", m_oBody.m_vecProcessData.at(i).m_strTimeArrived);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_signed", m_oBody.m_vecProcessData.at(i).m_strTimeSigned);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_name", m_oBody.m_vecProcessData.at(i).m_strProcessName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_code", m_oBody.m_vecProcessData.at(i).m_strProcessCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_personnel", m_oBody.m_vecProcessData.at(i).m_strDispatchPersonnel);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_vehicles", m_oBody.m_vecProcessData.at(i).m_strDispatchVehicles);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_boats", m_oBody.m_vecProcessData.at(i).m_strDispatchBoats);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", m_oBody.m_vecProcessData.at(i).m_strState);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_time", m_oBody.m_vecProcessData.at(i).m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_time", m_oBody.m_vecProcessData.at(i).m_strUpdateTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_name", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_org_code", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptOrgCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_name", m_oBody.m_vecProcessData.at(i).m_strProcessDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_org_code", m_oBody.m_vecProcessData.at(i).m_strProcessDeptOrgCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_type", m_oBody.m_vecProcessData.at(i).m_strProcessObjectType);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_name", m_oBody.m_vecProcessData.at(i).m_strProcessObjectName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_code", m_oBody.m_vecProcessData.at(i).m_strProcessObjectCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "business_status", m_oBody.m_vecProcessData.at(i).m_strBusinessStatus);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "seat_code", m_oBody.m_vecProcessData.at(i).m_strSeatCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "cancel_time", m_oBody.m_vecProcessData.at(i).m_strCancelTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "cancel_reason", m_oBody.m_vecProcessData.at(i).m_strCancelReason);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_auto_assign_job", m_oBody.m_vecProcessData.at(i).m_strIsAutoAssignJob);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_user", m_oBody.m_vecProcessData.at(i).m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_user", m_oBody.m_vecProcessData.at(i).m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "overtime_state", m_oBody.m_vecProcessData.at(i).m_strOvertimeState);

                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_object_id", m_oBody.m_vecProcessData.at(i).m_strProcessObjectID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "transfers_dept_org_code", m_oBody.m_vecProcessData.at(i).m_strTransfDeptOrjCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_over", m_oBody.m_vecProcessData.at(i).m_strIsOver);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "over_remark", m_oBody.m_vecProcessData.at(i).m_strOverRemark);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "parent_id", m_oBody.m_vecProcessData.at(i).m_strParentID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "flow_code", m_oBody.m_vecProcessData.at(i).m_strGZLDM);

                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "centre_process_dept_code", m_oBody.m_vecProcessData.at(i).m_strCentreProcessDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "centre_alarm_dept_code", m_oBody.m_vecProcessData.at(i).m_strCentreAlarmDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_name", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptDistrictName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "linked_dispatch_code", m_oBody.m_vecProcessData.at(i).m_strLinkedDispatchCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "over_time", m_oBody.m_vecProcessData.at(i).m_strOverTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "finished_timeout_state", m_oBody.m_vecProcessData.at(i).m_strFinishedTimeoutState);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "police_type", m_oBody.m_vecProcessData.at(i).m_strPoliceType);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_short_name", m_oBody.m_vecProcessData.at(i).m_strProcessDeptShortName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_short_name", m_oBody.m_vecProcessData.at(i).m_strDispatchDeptShortName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "createTeminal", m_oBody.m_vecProcessData.at(i).m_strCreateTeminal);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "updateTeminal", m_oBody.m_vecProcessData.at(i).m_strUpdateTeminal);
                }

                for (unsigned int i = 0; i < m_oBody.m_vectLinkeData.size(); i++)
                {
                    std::string l_strPrefixPathProcess("/body/linked_data/" + std::to_string(i) + "/");

                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", m_oBody.m_vectLinkeData.at(i).m_strID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", m_oBody.m_vectLinkeData.at(i).m_strAlarmID);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", m_oBody.m_vectLinkeData.at(i).m_strState);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "linked_org_code", m_oBody.m_vectLinkeData.at(i).m_strLinkedOrgCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "linked_org_name", m_oBody.m_vectLinkeData.at(i).m_strLinkedOrgName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "linked_org_type", m_oBody.m_vectLinkeData.at(i).m_strLinkedOrgType);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", m_oBody.m_vectLinkeData.at(i).m_strDispatchCode);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", m_oBody.m_vectLinkeData.at(i).m_strDispatchName);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_user", m_oBody.m_vectLinkeData.at(i).m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "create_time", m_oBody.m_vectLinkeData.at(i).m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_user", m_oBody.m_vectLinkeData.at(i).m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "update_time", m_oBody.m_vectLinkeData.at(i).m_strUpdateTime);
                    p_pJson->SetNodeValue(l_strPrefixPathProcess + "result", m_oBody.m_vectLinkeData.at(i).m_strResult);
                }
				return p_pJson->ToString();
			}

		public:
		
            class CFeedbackData
            {
            public:
				std::string m_strMsgSource;                      //消息来源
				std::string m_strID;                             //反馈单编号
                std::string m_strAlarmID;                        //接警单编号
                std::string m_strProcessID;                      //处警单编号
                std::string m_strResultType;                     //反馈类型
                std::string m_strResultContent;                  //处理结果内容
                std::string m_strLeaderInstruction;              //领导指示
                std::string m_strState;                          //反馈单状态
                std::string m_strTimeEdit;                       //反馈单编辑时间
                std::string m_strTimeSubmit;                     //反馈单提交时间
                std::string m_strTimeArrived;                    //反馈单到达时间
                std::string m_strTimeSigned;                     //反馈单签收时间
                std::string m_strTimePoliceDispatch;             //实际派出警力时间
                std::string m_strTimePoliceArrived;              //警力到达现场时间
                std::string m_strActualOccurTime;                //警情实际发生时间
                std::string m_strActualOccurAddr;                //警情实际发生地址
                std::string m_strFeedbackDeptDistrictCode;       //反馈单位行政区划
                std::string m_strFeedbackDeptCode;               //反馈单位编号
                std::string m_strFeedbackDeptName;               //反馈单位名称
                std::string m_strFeedbackCode;                   //反馈人编号
                std::string m_strFeedbackName;                   //反馈人姓名
                std::string m_strFeedbackLeaderCode;             //反馈单位领导编号
                std::string m_strFeedbackLeaderName;             //反馈单位领导姓名
                std::string m_strProcessDeptDistrictCode;        //处警单位行政区划
                std::string m_strProcessDeptCode;                //处警单位编号
                std::string m_strProcessDeptName;                //处警单位名称
                std::string m_strProcessCode;                    //处警人编号
                std::string m_strProcessName;                    //处警人时间
                std::string m_strProcessLeaderCode;              //处警单位领导编号
                std::string m_strProcessLeaderName;              //处警单位领导姓名
                std::string m_strDispatchDeptDistrictCode;       //派警单位行政区划
                std::string m_strDispatchDeptCode;               //派警单位编号
                std::string m_strDispatchDeptName;               //派警单位名称
                std::string m_strDispatchCode;                   //派警人编号
                std::string m_strDispatchName;                   //派警人姓名
                std::string m_strDispatchLeaderCode;             //派警单位领导编号
                std::string m_strDispatchLeaderName;             //派警单位领导姓名
                std::string m_strPersonId;                       //第一个当事人证件号
                std::string m_strPersonIdType;                   //第一个当事人证件类型
                std::string m_strPersonNationality;              //第一个当事人国籍
                std::string m_strPersonName;                     //第一个当事人姓名
                std::string m_strPersonSlaveId;                  //第二个当事人证件号
                std::string m_strPersonSlaveIdType;              //第二个当事人证件类型
                std::string m_strPersonSlaveNationality;         //第二个当事人国籍
                std::string m_strPersonSlaveName;                //第二个当事人姓名
                std::string m_strAlarmCalledNoType;              //警情报警电话类型
                std::string m_strAlarmFirstType;                 //警情一级类型
                std::string m_strAlarmSecondType;                //警情二级类型
                std::string m_strAlarmThirdType;                 //警情三级类型
                std::string m_strAlarmFourthType;                //刑事警情细类代码；如：分裂国家
                std::string m_strAlarmAddrDeptName;              //警情发生地单位名称
                std::string m_strAlarmAddrFirstType;             //警情发生地一级类型
                std::string m_strAlarmAddrSecondType;            //警情发生地二级类型
                std::string m_strAlarmAddrThirdType;             //警情发生地三级类型
                std::string m_strAlarmLongitude;                 //警情发生地经度
                std::string m_strAlarmLatitude;                  //警情发生地纬度
                std::string m_strAlarmRegionType;                //警情发生地区域类型; 如:郊区、城区
                std::string m_strAlarmLocationType;              //警情发生地场所类型代码；如：居民区
                std::string m_strPeopleNumCapture;               //抓获人数
                std::string m_strPeopleNumRescue;                //救助人数
                std::string m_strPeopleNumSlightInjury;          //轻伤人数
                std::string m_strPeopleNumSeriousInjury;         //重伤人数
                std::string m_strPeopleNumDeath;                 //死亡人数
                std::string m_strPoliceNumDispatch;              //出动警力数
                std::string m_strPoliceCarNumDispatch;           //出动车辆数
                std::string m_strEconomyLoss;                    //经济损失
                std::string m_strRetrieveEconomyLoss;            //挽回经济损失
                std::string m_strFirePutOutTime;                 //火灾警情：火灾扑灭时间
                std::string m_strFireBuildingFirstType;          //火灾警情：起火建筑类别一级代码
                std::string m_strFireBuildingSecondType;         //火灾警情：起火建筑类别二级代码
                std::string m_strFireBuildingThirdType;          //火灾警情：起火建筑类别三级代码
                std::string m_strFireSourceType;                 //火灾警情：起火源类型代码；如：家具
                std::string m_strFireRegionType;                 //火灾警情：起火区域类型代码；如：阳台、客厅等
                std::string m_strFireCauseFirstType;             //火灾警情：火灾原因一级代码
                std::string m_strFireCauseSecondType;            //火灾警情：火灾原因二级代码
                std::string m_strFireCauseThirdType;             //火灾警情：火灾原因三级代码
                std::string m_strFireArea;                       //火灾警情：燃烧面积
                std::string m_strTrafficRoadLevel;               //交通警情：道路等级代码
                std::string m_strTrafficAccidentLevel;           //交通警情：警情等级代码
                std::string m_strTrafficVehicleNo;               //交通警情：事故车辆车牌号
                std::string m_strTrafficVehicleType;             //交通警情：事故车辆类型代码
                std::string m_strTrafficSlaveVehicleNo;          //交通警情：第二个车辆车牌号
                std::string m_strTrafficSlaveVehicleType;        //交通警情：第二个车辆类型代码
                std::string m_strEventType;                      //事件类型
                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值		               
            };
            /*
            class CProcessData
            {
            public:
                std::string m_strID;						//处警ID
                std::string m_strAlarmID;					//警情ID
                std::string m_strState;						//处警单状态
                std::string m_strTimeEdit;					//派警单填写时间
                std::string m_strTimeSubmit;				//派警单提交时间
                std::string m_strTimeArrived;				//派警单系统到达时间
                std::string m_strTimeSigned;				//处警单位签收时间
                std::string m_strTimeFeedBack;				//处警单位反馈时间
                std::string m_strIsNeedFeedback;			//是否需要反馈
                std::string m_strDispatchDeptDistrictCode;	//派警单位行政区划
                std::string m_strDispatchDeptCode;			//派警单位代码
                std::string m_strDispatchDeptName;			//派警单位名称
                std::string m_strDispatchCode;				//派警人警号
                std::string m_strDispatchName;				//派警人姓名
                std::string m_strDispatchLeaderCode;		//派警单位值班领导警号
                std::string m_strDispatchLeaderName;		//派警单位值班领导姓名
                std::string m_strDispatchSuggestion;		//派警单位派警意见
                std::string m_strDispatchLeaderInstruction;	//派警单位领导指示
                std::string m_strProcessDeptDistrictCode;	//处警单位行政区划
                std::string m_strProcessDeptCode;			//处警单位代码
                std::string m_strProcessDeptName;			//处警单位名称
                std::string m_strProcessCode;				//处警人警号
                std::string m_strProcessName;				//处警人姓名
                std::string m_strProcessLeaderCode;			//处警单位值班领导警号
                std::string m_strProcessLeaderName;			//处警单位值班领导姓名
                std::string m_strProcessFeedback;			//处警反馈
                std::string m_strProcessLeaderInstruction;	//处警单位领导指示

                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值			
            };
            */
    //        class CAlarm
    //        {
    //        public:
    //            std::string	m_strID;						//警情id
				//std::string	m_strMergeID;					//警情合并id
				//std::string	m_strReceiptSeatno;				//接警坐席号
    //            std::string m_strTitle;						//警情标题
    //            std::string m_strContent;					//警情内容
    //            std::string m_strTime;						//警情id报警时间			
    //            std::string m_strActualOccurTime;			//警情id实际发生时间
    //            std::string m_strAddr;						//警情id详细发生地址
    //            std::string m_strLongitude;					//警情id经度
    //            std::string m_strLatitude;					//警情id维度
    //            std::string m_strState;						//警情id状态
    //            std::string m_strLevel;						//警情id级别
    //            std::string m_strSourceType;				//警情id报警来源类型
    //            std::string m_strSourceID;					//警情id报警来源id
    //            std::string m_strHandleType;				//警情id处理类型
    //            std::string m_strFirstType;					//警情id一级类型
    //            std::string m_strSecondType;				//警情id二级类型
    //            std::string m_strThirdType;					//警情id三级类型
    //            std::string m_strFourthType;				//警情id四级类型
    //            std::string m_strVehicleNo;					//警情id交通类型报警车牌号
    //            std::string m_strVehicleType;				//警情id交通类型报警车类型
    //            std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
    //            std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
    //            std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

    //            std::string m_strEventType;					//事件类型，逗号隔开

    //            std::string m_strCalledNoType;				//警情id报警号码字典类型
    //            std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

    //            std::string m_strCallerNo;					//警情id报警人号码
    //            std::string m_strCallerName;				//警情id报警人姓名
    //            std::string m_strCallerAddr;				//警情id报警人地址
    //            std::string m_strCallerID;					//警情id报警人身份证
    //            std::string m_strCallerIDType;				//警情id报警人身份证类型
    //            std::string m_strCallerGender;				//警情id报警人性别
    //            std::string m_strCallerAge;					//警情id报警人年龄
    //            std::string m_strCallerBirthday;			//警情id报警人出生年月日

    //            std::string m_strContactNo;					//警情id联系人号码
    //            std::string m_strContactName;				//警情id联系人姓名
    //            std::string m_strContactAddr;				//警情id联系人地址
    //            std::string m_strContactID;					//警情id联系人身份证
    //            std::string m_strContactIDType;				//警情id联系人身份证类型
    //            std::string m_strContactGender;				//警情id联系人性别
    //            std::string m_strContactAge;				//警情id联系人年龄
    //            std::string m_strContactBirthday;			//警情id联系人出生年月日

    //            std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
    //            std::string m_strAdminDeptCode;				//警情id管辖单位编码
    //            std::string m_strAdminDeptName;				//警情id管辖单位姓名

    //            std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
    //            std::string m_strReceiptDeptCode;			//警情id接警单位编码
    //            std::string m_strReceiptDeptName;			//警情id接警单位名称
    //            std::string m_strLeaderCode;				//警情id值班领导警号
    //            std::string m_strLeaderName;				//警情id值班领导姓名
    //            std::string m_strReceiptCode;				//警情id接警人警号
    //            std::string m_strReceiptName;				//警情id接警人姓名


    //            std::string m_strDispatchSuggestion;		//调派意见
				//std::string m_strIsMerge;					//是否合并

    //            std::string m_strCreateUser;				//创建人
    //            std::string m_strCreateTime;				//创建时间
    //            std::string m_strUpdateUser;				//修改人,取最后一次修改值
    //            std::string m_strUpdateTime;				//修改时间,取最后一次修改值			

    //            std::string m_strCityCode;                  //城市编码
				//std::string m_strPrivacy;				    //隐私保护 true or false
				//std::string m_strRemark;				    //补充信息
    //        };
            class CBody
            {
            public:
                CAlarmInfo m_oAlarm;
                std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
                std::vector<CFeedbackData> m_vecFeedbackData;
                std::vector<CAddOrUpdateLinkedRequest::CLinkedData> m_vectLinkeData;
            };
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}