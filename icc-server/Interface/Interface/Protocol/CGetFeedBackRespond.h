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
				std::string m_strMsgSource;                      //��Ϣ��Դ
				std::string m_strID;                             //���������
                std::string m_strAlarmID;                        //�Ӿ������
                std::string m_strProcessID;                      //���������
                std::string m_strResultType;                     //��������
                std::string m_strResultContent;                  //����������
                std::string m_strLeaderInstruction;              //�쵼ָʾ
                std::string m_strState;                          //������״̬
                std::string m_strTimeEdit;                       //�������༭ʱ��
                std::string m_strTimeSubmit;                     //�������ύʱ��
                std::string m_strTimeArrived;                    //����������ʱ��
                std::string m_strTimeSigned;                     //������ǩ��ʱ��
                std::string m_strTimePoliceDispatch;             //ʵ���ɳ�����ʱ��
                std::string m_strTimePoliceArrived;              //���������ֳ�ʱ��
                std::string m_strActualOccurTime;                //����ʵ�ʷ���ʱ��
                std::string m_strActualOccurAddr;                //����ʵ�ʷ�����ַ
                std::string m_strFeedbackDeptDistrictCode;       //������λ��������
                std::string m_strFeedbackDeptCode;               //������λ���
                std::string m_strFeedbackDeptName;               //������λ����
                std::string m_strFeedbackCode;                   //�����˱��
                std::string m_strFeedbackName;                   //����������
                std::string m_strFeedbackLeaderCode;             //������λ�쵼���
                std::string m_strFeedbackLeaderName;             //������λ�쵼����
                std::string m_strProcessDeptDistrictCode;        //������λ��������
                std::string m_strProcessDeptCode;                //������λ���
                std::string m_strProcessDeptName;                //������λ����
                std::string m_strProcessCode;                    //�����˱��
                std::string m_strProcessName;                    //������ʱ��
                std::string m_strProcessLeaderCode;              //������λ�쵼���
                std::string m_strProcessLeaderName;              //������λ�쵼����
                std::string m_strDispatchDeptDistrictCode;       //�ɾ���λ��������
                std::string m_strDispatchDeptCode;               //�ɾ���λ���
                std::string m_strDispatchDeptName;               //�ɾ���λ����
                std::string m_strDispatchCode;                   //�ɾ��˱��
                std::string m_strDispatchName;                   //�ɾ�������
                std::string m_strDispatchLeaderCode;             //�ɾ���λ�쵼���
                std::string m_strDispatchLeaderName;             //�ɾ���λ�쵼����
                std::string m_strPersonId;                       //��һ��������֤����
                std::string m_strPersonIdType;                   //��һ��������֤������
                std::string m_strPersonNationality;              //��һ�������˹���
                std::string m_strPersonName;                     //��һ������������
                std::string m_strPersonSlaveId;                  //�ڶ���������֤����
                std::string m_strPersonSlaveIdType;              //�ڶ���������֤������
                std::string m_strPersonSlaveNationality;         //�ڶ��������˹���
                std::string m_strPersonSlaveName;                //�ڶ�������������
                std::string m_strAlarmCalledNoType;              //���鱨���绰����
                std::string m_strAlarmFirstType;                 //����һ������
                std::string m_strAlarmSecondType;                //�����������
                std::string m_strAlarmThirdType;                 //������������
                std::string m_strAlarmFourthType;                //���¾���ϸ����룻�磺���ѹ���
                std::string m_strAlarmAddrDeptName;              //���鷢���ص�λ����
                std::string m_strAlarmAddrFirstType;             //���鷢����һ������
                std::string m_strAlarmAddrSecondType;            //���鷢���ض�������
                std::string m_strAlarmAddrThirdType;             //���鷢������������
                std::string m_strAlarmLongitude;                 //���鷢���ؾ���
                std::string m_strAlarmLatitude;                  //���鷢����γ��
                std::string m_strAlarmRegionType;                //���鷢������������; ��:����������
                std::string m_strAlarmLocationType;              //���鷢���س������ʹ��룻�磺������
                std::string m_strPeopleNumCapture;               //ץ������
                std::string m_strPeopleNumRescue;                //��������
                std::string m_strPeopleNumSlightInjury;          //��������
                std::string m_strPeopleNumSeriousInjury;         //��������
                std::string m_strPeopleNumDeath;                 //��������
                std::string m_strPoliceNumDispatch;              //����������
                std::string m_strPoliceCarNumDispatch;           //����������
                std::string m_strEconomyLoss;                    //������ʧ
                std::string m_strRetrieveEconomyLoss;            //��ؾ�����ʧ
                std::string m_strFirePutOutTime;                 //���־��飺��������ʱ��
                std::string m_strFireBuildingFirstType;          //���־��飺��������һ������
                std::string m_strFireBuildingSecondType;         //���־��飺���������������
                std::string m_strFireBuildingThirdType;          //���־��飺����������������
                std::string m_strFireSourceType;                 //���־��飺���Դ���ʹ��룻�磺�Ҿ�
                std::string m_strFireRegionType;                 //���־��飺����������ʹ��룻�磺��̨��������
                std::string m_strFireCauseFirstType;             //���־��飺����ԭ��һ������
                std::string m_strFireCauseSecondType;            //���־��飺����ԭ���������
                std::string m_strFireCauseThirdType;             //���־��飺����ԭ����������
                std::string m_strFireArea;                       //���־��飺ȼ�����
                std::string m_strTrafficRoadLevel;               //��ͨ���飺��·�ȼ�����
                std::string m_strTrafficAccidentLevel;           //��ͨ���飺����ȼ�����
                std::string m_strTrafficVehicleNo;               //��ͨ���飺�¹ʳ������ƺ�
                std::string m_strTrafficVehicleType;             //��ͨ���飺�¹ʳ������ʹ���
                std::string m_strTrafficSlaveVehicleNo;          //��ͨ���飺�ڶ����������ƺ�
                std::string m_strTrafficSlaveVehicleType;        //��ͨ���飺�ڶ����������ʹ���
                std::string m_strEventType;                      //�¼�����
                std::string m_strCreateUser;				//������
                std::string m_strCreateTime;				//����ʱ��
                std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
                std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ		               
            };
            /*
            class CProcessData
            {
            public:
                std::string m_strID;						//����ID
                std::string m_strAlarmID;					//����ID
                std::string m_strState;						//������״̬
                std::string m_strTimeEdit;					//�ɾ�����дʱ��
                std::string m_strTimeSubmit;				//�ɾ����ύʱ��
                std::string m_strTimeArrived;				//�ɾ���ϵͳ����ʱ��
                std::string m_strTimeSigned;				//������λǩ��ʱ��
                std::string m_strTimeFeedBack;				//������λ����ʱ��
                std::string m_strIsNeedFeedback;			//�Ƿ���Ҫ����
                std::string m_strDispatchDeptDistrictCode;	//�ɾ���λ��������
                std::string m_strDispatchDeptCode;			//�ɾ���λ����
                std::string m_strDispatchDeptName;			//�ɾ���λ����
                std::string m_strDispatchCode;				//�ɾ��˾���
                std::string m_strDispatchName;				//�ɾ�������
                std::string m_strDispatchLeaderCode;		//�ɾ���λֵ���쵼����
                std::string m_strDispatchLeaderName;		//�ɾ���λֵ���쵼����
                std::string m_strDispatchSuggestion;		//�ɾ���λ�ɾ����
                std::string m_strDispatchLeaderInstruction;	//�ɾ���λ�쵼ָʾ
                std::string m_strProcessDeptDistrictCode;	//������λ��������
                std::string m_strProcessDeptCode;			//������λ����
                std::string m_strProcessDeptName;			//������λ����
                std::string m_strProcessCode;				//�����˾���
                std::string m_strProcessName;				//����������
                std::string m_strProcessLeaderCode;			//������λֵ���쵼����
                std::string m_strProcessLeaderName;			//������λֵ���쵼����
                std::string m_strProcessFeedback;			//��������
                std::string m_strProcessLeaderInstruction;	//������λ�쵼ָʾ

                std::string m_strCreateUser;				//������
                std::string m_strCreateTime;				//����ʱ��
                std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
                std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ			
            };
            */
    //        class CAlarm
    //        {
    //        public:
    //            std::string	m_strID;						//����id
				//std::string	m_strMergeID;					//����ϲ�id
				//std::string	m_strReceiptSeatno;				//�Ӿ���ϯ��
    //            std::string m_strTitle;						//�������
    //            std::string m_strContent;					//��������
    //            std::string m_strTime;						//����id����ʱ��			
    //            std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
    //            std::string m_strAddr;						//����id��ϸ������ַ
    //            std::string m_strLongitude;					//����id����
    //            std::string m_strLatitude;					//����idά��
    //            std::string m_strState;						//����id״̬
    //            std::string m_strLevel;						//����id����
    //            std::string m_strSourceType;				//����id������Դ����
    //            std::string m_strSourceID;					//����id������Դid
    //            std::string m_strHandleType;				//����id��������
    //            std::string m_strFirstType;					//����idһ������
    //            std::string m_strSecondType;				//����id��������
    //            std::string m_strThirdType;					//����id��������
    //            std::string m_strFourthType;				//����id�ļ�����
    //            std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
    //            std::string m_strVehicleType;				//����id��ͨ���ͱ���������
    //            std::string m_strSymbolCode;				//����id������ַ������ʾ����
    //            std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
    //            std::string m_strFireBuildingType;			//����id������ȼ�ս�������

    //            std::string m_strEventType;					//�¼����ͣ����Ÿ���

    //            std::string m_strCalledNoType;				//����id���������ֵ�����
    //            std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

    //            std::string m_strCallerNo;					//����id�����˺���
    //            std::string m_strCallerName;				//����id����������
    //            std::string m_strCallerAddr;				//����id�����˵�ַ
    //            std::string m_strCallerID;					//����id���������֤
    //            std::string m_strCallerIDType;				//����id���������֤����
    //            std::string m_strCallerGender;				//����id�������Ա�
    //            std::string m_strCallerAge;					//����id����������
    //            std::string m_strCallerBirthday;			//����id�����˳���������

    //            std::string m_strContactNo;					//����id��ϵ�˺���
    //            std::string m_strContactName;				//����id��ϵ������
    //            std::string m_strContactAddr;				//����id��ϵ�˵�ַ
    //            std::string m_strContactID;					//����id��ϵ�����֤
    //            std::string m_strContactIDType;				//����id��ϵ�����֤����
    //            std::string m_strContactGender;				//����id��ϵ���Ա�
    //            std::string m_strContactAge;				//����id��ϵ������
    //            std::string m_strContactBirthday;			//����id��ϵ�˳���������

    //            std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
    //            std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
    //            std::string m_strAdminDeptName;				//����id��Ͻ��λ����

    //            std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
    //            std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
    //            std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
    //            std::string m_strLeaderCode;				//����idֵ���쵼����
    //            std::string m_strLeaderName;				//����idֵ���쵼����
    //            std::string m_strReceiptCode;				//����id�Ӿ��˾���
    //            std::string m_strReceiptName;				//����id�Ӿ�������


    //            std::string m_strDispatchSuggestion;		//�������
				//std::string m_strIsMerge;					//�Ƿ�ϲ�

    //            std::string m_strCreateUser;				//������
    //            std::string m_strCreateTime;				//����ʱ��
    //            std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
    //            std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ			

    //            std::string m_strCityCode;                  //���б���
				//std::string m_strPrivacy;				    //��˽���� true or false
				//std::string m_strRemark;				    //������Ϣ
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