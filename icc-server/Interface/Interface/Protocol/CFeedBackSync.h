#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CFeedBackSync :
			public IRespond,public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/id", m_oBody.m_strID);
                p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
                p_pJson->SetNodeValue("/body/process_id", m_oBody.m_strProcessID);
                p_pJson->SetNodeValue("/body/result_type", m_oBody.m_strResultType);
                p_pJson->SetNodeValue("/body/result_content", m_oBody.m_strResultContent);
                p_pJson->SetNodeValue("/body/leader_instruction", m_oBody.m_strLeaderInstruction);
                p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
                p_pJson->SetNodeValue("/body/time_edit", m_oBody.m_strTimeEdit);
                p_pJson->SetNodeValue("/body/time_submit", m_oBody.m_strTimeSubmit);
                p_pJson->SetNodeValue("/body/time_arrived", m_oBody.m_strTimeArrived);
                p_pJson->SetNodeValue("/body/time_signed", m_oBody.m_strTimeSigned);
                p_pJson->SetNodeValue("/body/time_police_dispatch", m_oBody.m_strTimePoliceDispatch);
                p_pJson->SetNodeValue("/body/time_police_arrived", m_oBody.m_strTimePoliceArrived);
                p_pJson->SetNodeValue("/body/actual_occur_time", m_oBody.m_strActualOccurTime);
                p_pJson->SetNodeValue("/body/actual_occur_addr", m_oBody.m_strActualOccurAddr);
                p_pJson->SetNodeValue("/body/feedback_dept_district_code", m_oBody.m_strFeedbackDeptDistrictCode);
                p_pJson->SetNodeValue("/body/feedback_dept_code", m_oBody.m_strFeedbackDeptCode);
                p_pJson->SetNodeValue("/body/feedback_dept_name", m_oBody.m_strFeedbackDeptName);
                p_pJson->SetNodeValue("/body/feedback_code", m_oBody.m_strFeedbackCode);
                p_pJson->SetNodeValue("/body/feedback_name", m_oBody.m_strFeedbackName);
                p_pJson->SetNodeValue("/body/feedback_leader_code", m_oBody.m_strFeedbackLeaderCode);
                p_pJson->SetNodeValue("/body/feedback_leader_name", m_oBody.m_strFeedbackLeaderName);
                p_pJson->SetNodeValue("/body/process_dept_district_code", m_oBody.m_strProcessDeptDistrictCode);
                p_pJson->SetNodeValue("/body/process_dept_code", m_oBody.m_strProcessDeptCode);
                p_pJson->SetNodeValue("/body/process_dept_name", m_oBody.m_strProcessDeptName);
                p_pJson->SetNodeValue("/body/process_code", m_oBody.m_strProcessCode);
                p_pJson->SetNodeValue("/body/process_name", m_oBody.m_strProcessName);
                p_pJson->SetNodeValue("/body/process_leader_code", m_oBody.m_strProcessLeaderCode);
                p_pJson->SetNodeValue("/body/process_leader_name", m_oBody.m_strProcessLeaderName);
                p_pJson->SetNodeValue("/body/dispatch_dept_district_code", m_oBody.m_strDispatchDeptDistrictCode);
                p_pJson->SetNodeValue("/body/dispatch_dept_code", m_oBody.m_strDispatchDeptCode);
                p_pJson->SetNodeValue("/body/dispatch_dept_name", m_oBody.m_strDispatchDeptName);
                p_pJson->SetNodeValue("/body/dispatch_code", m_oBody.m_strDispatchCode);
                p_pJson->SetNodeValue("/body/dispatch_name", m_oBody.m_strDispatchName);
                p_pJson->SetNodeValue("/body/dispatch_leader_code", m_oBody.m_strDispatchLeaderCode);
                p_pJson->SetNodeValue("/body/dispatch_leader_name", m_oBody.m_strDispatchLeaderName);
                p_pJson->SetNodeValue("/body/person_id", m_oBody.m_strPersonId);
                p_pJson->SetNodeValue("/body/person_id_type", m_oBody.m_strPersonIdType);
                p_pJson->SetNodeValue("/body/person_nationality", m_oBody.m_strPersonNationality);
                p_pJson->SetNodeValue("/body/person_name", m_oBody.m_strPersonName);
                p_pJson->SetNodeValue("/body/person_slave_id", m_oBody.m_strPersonSlaveId);
                p_pJson->SetNodeValue("/body/person_slave_id_type", m_oBody.m_strPersonSlaveIdType);
                p_pJson->SetNodeValue("/body/person_slave_nationality", m_oBody.m_strPersonSlaveNationality);
                p_pJson->SetNodeValue("/body/person_slave_name", m_oBody.m_strPersonSlaveName);
                p_pJson->SetNodeValue("/body/alarm_first_type", m_oBody.m_strAlarmFirstType);
                p_pJson->SetNodeValue("/body/alarm_second_type", m_oBody.m_strAlarmSecondType);
                p_pJson->SetNodeValue("/body/alarm_third_type", m_oBody.m_strAlarmThirdType);
                p_pJson->SetNodeValue("/body/alarm_fourth_type", m_oBody.m_strAlarmFourthType);
                p_pJson->SetNodeValue("/body/alarm_addr_dept_name", m_oBody.m_strAlarmAddrDeptName);
                p_pJson->SetNodeValue("/body/alarm_addr_first_type", m_oBody.m_strAlarmAddrFirstType);
                p_pJson->SetNodeValue("/body/alarm_addr_second_type", m_oBody.m_strAlarmAddrSecondType);
                p_pJson->SetNodeValue("/body/alarm_addr_third_type", m_oBody.m_strAlarmAddrThirdType);
                p_pJson->SetNodeValue("/body/alarm_longitude", m_oBody.m_strAlarmLongitude);
                p_pJson->SetNodeValue("/body/alarm_latitude", m_oBody.m_strAlarmLatitude);
                p_pJson->SetNodeValue("/body/alarm_region_type", m_oBody.m_strAlarmRegionType);
                p_pJson->SetNodeValue("/body/alarm_location_type", m_oBody.m_strAlarmLocationType);
                p_pJson->SetNodeValue("/body/people_num_capture", m_oBody.m_strPeopleNumCapture);
                p_pJson->SetNodeValue("/body/people_num_rescue", m_oBody.m_strPeopleNumRescue);
                p_pJson->SetNodeValue("/body/people_num_slight_injury", m_oBody.m_strPeopleNumSlightInjury);
                p_pJson->SetNodeValue("/body/people_num_serious_injury", m_oBody.m_strPeopleNumSeriousInjury);
                p_pJson->SetNodeValue("/body/people_num_death", m_oBody.m_strPeopleNumDeath);
                p_pJson->SetNodeValue("/body/police_num_dispatch", m_oBody.m_strPoliceNumDispatch);
                p_pJson->SetNodeValue("/body/police_car_num_dispatch", m_oBody.m_strPoliceCarNumDispatch);
                p_pJson->SetNodeValue("/body/economy_loss", m_oBody.m_strEconomyLoss);
                p_pJson->SetNodeValue("/body/retrieve_economy_loss", m_oBody.m_strRetrieveEconomyLoss);
                p_pJson->SetNodeValue("/body/fire_put_out_time", m_oBody.m_strFirePutOutTime);
                p_pJson->SetNodeValue("/body/fire_building_first_type", m_oBody.m_strFireBuildingFirstType);
                p_pJson->SetNodeValue("/body/fire_building_second_type", m_oBody.m_strFireBuildingSecondType);
                p_pJson->SetNodeValue("/body/fire_building_third_type", m_oBody.m_strFireBuildingThirdType);
                p_pJson->SetNodeValue("/body/fire_source_type", m_oBody.m_strFireSourceType);
                p_pJson->SetNodeValue("/body/fire_region_type", m_oBody.m_strFireRegionType);
                p_pJson->SetNodeValue("/body/fire_cause_first_type", m_oBody.m_strFireCauseFirstType);
                p_pJson->SetNodeValue("/body/fire_cause_second_type", m_oBody.m_strFireCauseSecondType);
                p_pJson->SetNodeValue("/body/fire_cause_third_type", m_oBody.m_strFireCauseThirdType);
                p_pJson->SetNodeValue("/body/fire_area", m_oBody.m_strFireArea);
                p_pJson->SetNodeValue("/body/traffic_road_level", m_oBody.m_strTrafficRoadLevel);
                p_pJson->SetNodeValue("/body/traffic_accident_level", m_oBody.m_strTrafficAccidentLevel);
                p_pJson->SetNodeValue("/body/traffic_vehicle_no", m_oBody.m_strTrafficVehicleNo);
                p_pJson->SetNodeValue("/body/traffic_vehicle_type", m_oBody.m_strTrafficVehicleType);
                p_pJson->SetNodeValue("/body/traffic_slave_vehicle_no", m_oBody.m_strTrafficSlaveVehicleNo);
                p_pJson->SetNodeValue("/body/traffic_slave_vehicle_type", m_oBody.m_strTrafficSlaveVehicleType);
                p_pJson->SetNodeValue("/body/event_type", m_oBody.m_strEventType);
                p_pJson->SetNodeValue("/body/alarm_called_no_type", m_oBody.m_strAlarmCalledNoType);
                p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
                p_pJson->SetNodeValue("/body/create_user", m_oBody.m_strCreateUser);
                p_pJson->SetNodeValue("/body/create_time", m_oBody.m_strCreateTime);
                p_pJson->SetNodeValue("/body/update_user", m_oBody.m_strUpdateUser);
                p_pJson->SetNodeValue("/body/update_time", m_oBody.m_strUpdateTime);
				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
                m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
                m_oBody.m_strProcessID = p_pJson->GetNodeValue("/body/process_id", "");
                m_oBody.m_strResultType = p_pJson->GetNodeValue("/body/result_type", "");
                m_oBody.m_strResultContent = p_pJson->GetNodeValue("/body/result_content", "");
                m_oBody.m_strLeaderInstruction = p_pJson->GetNodeValue("/body/leader_instruction", "");
                m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
                m_oBody.m_strTimeEdit = p_pJson->GetNodeValue("/body/time_edit", "");
                m_oBody.m_strTimeSubmit = p_pJson->GetNodeValue("/body/time_submit", "");
                m_oBody.m_strTimeArrived = p_pJson->GetNodeValue("/body/time_arrived", "");
                m_oBody.m_strTimeSigned = p_pJson->GetNodeValue("/body/time_signed", "");
                m_oBody.m_strTimePoliceDispatch = p_pJson->GetNodeValue("/body/time_police_dispatch", "");
                m_oBody.m_strTimePoliceArrived = p_pJson->GetNodeValue("/body/time_police_arrived", "");
                m_oBody.m_strActualOccurTime = p_pJson->GetNodeValue("/body/actual_occur_time", "");
                m_oBody.m_strActualOccurAddr = p_pJson->GetNodeValue("/body/actual_occur_addr", "");
                m_oBody.m_strFeedbackDeptDistrictCode = p_pJson->GetNodeValue("/body/feedback_dept_district_code", "");
                m_oBody.m_strFeedbackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
                m_oBody.m_strFeedbackDeptName = p_pJson->GetNodeValue("/body/feedback_dept_name", "");
                m_oBody.m_strFeedbackCode = p_pJson->GetNodeValue("/body/feedback_code", "");
                m_oBody.m_strFeedbackName = p_pJson->GetNodeValue("/body/feedback_name", "");
                m_oBody.m_strFeedbackLeaderCode = p_pJson->GetNodeValue("/body/feedback_leader_code", "");
                m_oBody.m_strFeedbackLeaderName = p_pJson->GetNodeValue("/body/feedback_leader_name", "");
                m_oBody.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue("/body/process_dept_district_code", "");
                m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
                m_oBody.m_strProcessDeptName = p_pJson->GetNodeValue("/body/process_dept_name", "");
                m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
                m_oBody.m_strProcessName = p_pJson->GetNodeValue("/body/process_name", "");
                m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");
                m_oBody.m_strProcessLeaderName = p_pJson->GetNodeValue("/body/process_leader_name", "");
                m_oBody.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue("/body/dispatch_dept_district_code", "");
                m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
                m_oBody.m_strDispatchDeptName = p_pJson->GetNodeValue("/body/dispatch_dept_name", "");
                m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
                m_oBody.m_strDispatchName = p_pJson->GetNodeValue("/body/dispatch_name", "");
                m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");
                m_oBody.m_strDispatchLeaderName = p_pJson->GetNodeValue("/body/dispatch_leader_name", "");
                m_oBody.m_strPersonId = p_pJson->GetNodeValue("/body/person_id", "");
                m_oBody.m_strPersonIdType = p_pJson->GetNodeValue("/body/person_id_type", "");
                m_oBody.m_strPersonNationality = p_pJson->GetNodeValue("/body/person_nationality", "");
                m_oBody.m_strPersonName = p_pJson->GetNodeValue("/body/person_name", "");
                m_oBody.m_strPersonSlaveId = p_pJson->GetNodeValue("/body/person_slave_id", "");
                m_oBody.m_strPersonSlaveIdType = p_pJson->GetNodeValue("/body/person_slave_id_type", "");
                m_oBody.m_strPersonSlaveNationality = p_pJson->GetNodeValue("/body/person_slave_nationality", "");
                m_oBody.m_strPersonSlaveName = p_pJson->GetNodeValue("/body/person_slave_name", "");
                m_oBody.m_strAlarmFirstType = p_pJson->GetNodeValue("/body/alarm_first_type", "");
                m_oBody.m_strAlarmSecondType = p_pJson->GetNodeValue("/body/alarm_second_type", "");
                m_oBody.m_strAlarmThirdType = p_pJson->GetNodeValue("/body/alarm_third_type", "");
                m_oBody.m_strAlarmFourthType = p_pJson->GetNodeValue("/body/alarm_fourth_type", "");
                m_oBody.m_strAlarmAddrDeptName = p_pJson->GetNodeValue("/body/alarm_addr_dept_name", "");
                m_oBody.m_strAlarmAddrFirstType = p_pJson->GetNodeValue("/body/alarm_addr_first_type", "");
                m_oBody.m_strAlarmAddrSecondType = p_pJson->GetNodeValue("/body/alarm_addr_second_type", "");
                m_oBody.m_strAlarmAddrThirdType = p_pJson->GetNodeValue("/body/alarm_addr_third_type", "");
                m_oBody.m_strAlarmLongitude = p_pJson->GetNodeValue("/body/alarm_longitude", "");
                m_oBody.m_strAlarmLatitude = p_pJson->GetNodeValue("/body/alarm_latitude", "");
                m_oBody.m_strAlarmRegionType = p_pJson->GetNodeValue("/body/alarm_region_type", "");
                m_oBody.m_strAlarmLocationType = p_pJson->GetNodeValue("/body/alarm_location_type", "");
                m_oBody.m_strPeopleNumCapture = p_pJson->GetNodeValue("/body/people_num_capture", "");
                m_oBody.m_strPeopleNumRescue = p_pJson->GetNodeValue("/body/people_num_rescue", "");
                m_oBody.m_strPeopleNumSlightInjury = p_pJson->GetNodeValue("/body/people_num_slight_injury", "");
                m_oBody.m_strPeopleNumSeriousInjury = p_pJson->GetNodeValue("/body/people_num_serious_injury", "");
                m_oBody.m_strPeopleNumDeath = p_pJson->GetNodeValue("/body/people_num_death", "");
                m_oBody.m_strPoliceNumDispatch = p_pJson->GetNodeValue("/body/police_num_dispatch", "");
                m_oBody.m_strPoliceCarNumDispatch = p_pJson->GetNodeValue("/body/police_car_num_dispatch", "");
                m_oBody.m_strEconomyLoss = p_pJson->GetNodeValue("/body/economy_loss", "");
                m_oBody.m_strRetrieveEconomyLoss = p_pJson->GetNodeValue("/body/retrieve_economy_loss", "");
                m_oBody.m_strFirePutOutTime = p_pJson->GetNodeValue("/body/fire_put_out_time", "");
                m_oBody.m_strFireBuildingFirstType = p_pJson->GetNodeValue("/body/fire_building_first_type", "");
                m_oBody.m_strFireBuildingSecondType = p_pJson->GetNodeValue("/body/fire_building_second_type", "");
                m_oBody.m_strFireBuildingThirdType = p_pJson->GetNodeValue("/body/fire_building_third_type", "");
                m_oBody.m_strFireSourceType = p_pJson->GetNodeValue("/body/fire_source_type", "");
                m_oBody.m_strFireRegionType = p_pJson->GetNodeValue("/body/fire_region_type", "");
                m_oBody.m_strFireCauseFirstType = p_pJson->GetNodeValue("/body/fire_cause_first_type", "");
                m_oBody.m_strFireCauseSecondType = p_pJson->GetNodeValue("/body/fire_cause_second_type", "");
                m_oBody.m_strFireCauseThirdType = p_pJson->GetNodeValue("/body/fire_cause_third_type", "");
                m_oBody.m_strFireArea = p_pJson->GetNodeValue("/body/fire_area", "");
                m_oBody.m_strTrafficRoadLevel = p_pJson->GetNodeValue("/body/traffic_road_level", "");
                m_oBody.m_strTrafficAccidentLevel = p_pJson->GetNodeValue("/body/traffic_accident_level", "");
                m_oBody.m_strTrafficVehicleNo = p_pJson->GetNodeValue("/body/traffic_vehicle_no", "");
                m_oBody.m_strTrafficVehicleType = p_pJson->GetNodeValue("/body/traffic_vehicle_type", "");
                m_oBody.m_strTrafficSlaveVehicleNo = p_pJson->GetNodeValue("/body/traffic_slave_vehicle_no", "");
                m_oBody.m_strTrafficSlaveVehicleType = p_pJson->GetNodeValue("/body/traffic_slave_vehicle_type", "");
                m_oBody.m_strEventType = p_pJson->GetNodeValue("/body/event_type", "");
                m_oBody.m_strAlarmCalledNoType = p_pJson->GetNodeValue("/body/alarm_called_no_type", "");

                m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
                m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
                m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
                m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");
                return true;
            }

		public:
			CHeader m_oHeader;
				class CBody
			{
			public:
                std::string m_strSyncType;
                std::string m_strID;
                std::string m_strAlarmID;
                std::string m_strProcessID;
                std::string m_strResultType;
                std::string m_strResultContent;
                std::string m_strLeaderInstruction;
                std::string m_strState;
                std::string m_strTimeEdit;
                std::string m_strTimeSubmit;
                std::string m_strTimeArrived;
                std::string m_strTimeSigned;
                std::string m_strTimePoliceDispatch;
                std::string m_strTimePoliceArrived;
                std::string m_strActualOccurTime;
                std::string m_strActualOccurAddr;
                std::string m_strFeedbackDeptDistrictCode;
                std::string m_strFeedbackDeptCode;
                std::string m_strFeedbackDeptName;
                std::string m_strFeedbackCode;
                std::string m_strFeedbackName;
                std::string m_strFeedbackLeaderCode;
                std::string m_strFeedbackLeaderName;
                std::string m_strProcessDeptDistrictCode;
                std::string m_strProcessDeptCode;
                std::string m_strProcessDeptName;
                std::string m_strProcessCode;
                std::string m_strProcessName;
                std::string m_strProcessLeaderCode;
                std::string m_strProcessLeaderName;
                std::string m_strDispatchDeptDistrictCode;
                std::string m_strDispatchDeptCode;
                std::string m_strDispatchDeptName;
                std::string m_strDispatchCode;
                std::string m_strDispatchName;
                std::string m_strDispatchLeaderCode;
                std::string m_strDispatchLeaderName;
                std::string m_strPersonId;
                std::string m_strPersonIdType;
                std::string m_strPersonNationality;
                std::string m_strPersonName;
                std::string m_strPersonSlaveId;
                std::string m_strPersonSlaveIdType;
                std::string m_strPersonSlaveNationality;
                std::string m_strPersonSlaveName;
                std::string m_strAlarmCalledNoType;              //警情报警电话类型
                std::string m_strAlarmFirstType;
                std::string m_strAlarmSecondType;
                std::string m_strAlarmThirdType;
                std::string m_strAlarmFourthType;
                std::string m_strAlarmAddrDeptName;
                std::string m_strAlarmAddrFirstType;
                std::string m_strAlarmAddrSecondType;
                std::string m_strAlarmAddrThirdType;
                std::string m_strAlarmLongitude;
                std::string m_strAlarmLatitude;
                std::string m_strAlarmRegionType;
                std::string m_strAlarmLocationType;
                std::string m_strPeopleNumCapture;
                std::string m_strPeopleNumRescue;
                std::string m_strPeopleNumSlightInjury;
                std::string m_strPeopleNumSeriousInjury;
                std::string m_strPeopleNumDeath;
                std::string m_strPoliceNumDispatch;
                std::string m_strPoliceCarNumDispatch;
                std::string m_strEconomyLoss;
                std::string m_strRetrieveEconomyLoss;
                std::string m_strFirePutOutTime;
                std::string m_strFireBuildingFirstType;
                std::string m_strFireBuildingSecondType;
                std::string m_strFireBuildingThirdType;
                std::string m_strFireSourceType;
                std::string m_strFireRegionType;
                std::string m_strFireCauseFirstType;
                std::string m_strFireCauseSecondType;
                std::string m_strFireCauseThirdType;
                std::string m_strFireArea;
                std::string m_strTrafficRoadLevel;
                std::string m_strTrafficAccidentLevel;
                std::string m_strTrafficVehicleNo;
                std::string m_strTrafficVehicleType;
                std::string m_strTrafficSlaveVehicleNo;
                std::string m_strTrafficSlaveVehicleType;
                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值
                std::string m_strEventType;
			};
			CBody m_oBody;
		};
	}
}
