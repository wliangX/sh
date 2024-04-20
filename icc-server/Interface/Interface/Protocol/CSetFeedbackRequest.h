#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
        class CSetFeedBackRequest :
            public IRequest, public IRespond
		{
        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
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
                m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
                m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
                m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");
                m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
                return true;
            }

            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
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
                p_pJson->SetNodeValue("/body/feedback_dept_district_cod", m_oBody.m_strFeedbackDeptDistrictCode);
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
                p_pJson->SetNodeValue("/body/dispatch_dept_district_cod", m_oBody.m_strDispatchDeptDistrictCode);
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
                p_pJson->SetNodeValue("/body/alarm_called_no_type", m_oBody.m_strAlarmCalledNoType);
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
                p_pJson->SetNodeValue("/body/create_user", m_oBody.m_strCreateUser);
                p_pJson->SetNodeValue("/body/create_time", m_oBody.m_strCreateTime);
                p_pJson->SetNodeValue("/body/update_user", m_oBody.m_strUpdateUser);
                p_pJson->SetNodeValue("/body/update_time", m_oBody.m_strUpdateTime);
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strMsgSource;                         //消息来源
                std::string m_strID;                                //反馈单编号
                std::string m_strAlarmID;                           //接警单编号
                std::string m_strProcessID;                         //处警单编号
                std::string m_strResultType;                        //反馈类型
                std::string m_strResultContent;                     //处理结果内容
                std::string m_strLeaderInstruction;                 //领导指示
                std::string m_strState;                             //反馈单状态
                std::string m_strTimeEdit;                          //反馈单编辑时间
                std::string m_strTimeSubmit;                        //反馈单提交时间
                std::string m_strTimeArrived;                       //反馈单到达时间
                std::string m_strTimeSigned;                        //反馈单签收时间
                std::string m_strTimePoliceDispatch;                //实际派出警力时间
                std::string m_strTimePoliceArrived;                 //警力到达现场时间
                std::string m_strActualOccurTime;                   //警情实际发生时间
                std::string m_strActualOccurAddr;                   //警情实际发生地址
                std::string m_strFeedbackDeptDistrictCode;          //反馈单位行政区划
                std::string m_strFeedbackDeptCode;                  //反馈单位编号
                std::string m_strFeedbackDeptName;                  //反馈单位名称
                std::string m_strFeedbackCode;                      //反馈人编号
                std::string m_strFeedbackName;                      //反馈人姓名
                std::string m_strFeedbackLeaderCode;                //反馈单位领导编号
                std::string m_strFeedbackLeaderName;                //反馈单位领导姓名
                std::string m_strProcessDeptDistrictCode;           //处警单位行政区划
                std::string m_strProcessDeptCode;                   //处警单位编号
                std::string m_strProcessDeptName;                   //处警单位名称
                std::string m_strProcessCode;                       //处警人编号
                std::string m_strProcessName;                       //处警人时间
                std::string m_strProcessLeaderCode;                 //处警单位领导编号
                std::string m_strProcessLeaderName;                 //处警单位领导姓名
                std::string m_strDispatchDeptDistrictCode;          //派警单位行政区划
                std::string m_strDispatchDeptCode;                  //派警单位编号
                std::string m_strDispatchDeptName;                  //派警单位名称
                std::string m_strDispatchCode;                      //派警人编号
                std::string m_strDispatchName;                      //派警人姓名
                std::string m_strDispatchLeaderCode;                //派警单位领导编号
                std::string m_strDispatchLeaderName;                //派警单位领导姓名
                std::string m_strPersonId;                          //第一个当事人证件号
                std::string m_strPersonIdType;                      //第一个当事人证件类型
                std::string m_strPersonNationality;                 //第一个当事人国籍
                std::string m_strPersonName;                        //第一个当事人姓名
                std::string m_strPersonSlaveId;                     //第二个当事人证件号
                std::string m_strPersonSlaveIdType;                 //第二个当事人证件类型
                std::string m_strPersonSlaveNationality;            //第二个当事人国籍
                std::string m_strPersonSlaveName;                   //第二个当事人姓名
                std::string m_strAlarmCalledNoType;                 //警情报警电话类型
                std::string m_strAlarmFirstType;                    //警情一级类型
                std::string m_strAlarmSecondType;                   //警情二级类型
                std::string m_strAlarmThirdType;                    //警情三级类型
                std::string m_strAlarmFourthType;                   //刑事警情细类代码；如：分裂国家
                std::string m_strAlarmAddrDeptName;                 //警情发生地单位名称
                std::string m_strAlarmAddrFirstType;                //警情发生地一级类型
                std::string m_strAlarmAddrSecondType;               //警情发生地二级类型
                std::string m_strAlarmAddrThirdType;                //警情发生地三级类型
                std::string m_strAlarmLongitude;                    //警情发生地经度
                std::string m_strAlarmLatitude;                     //警情发生地纬度
                std::string m_strAlarmRegionType;                   //警情发生地区域类型; 如:郊区、城区
                std::string m_strAlarmLocationType;                 //警情发生地场所类型代码；如：居民区
                std::string m_strPeopleNumCapture;                  //抓获人数
                std::string m_strPeopleNumRescue;                   //救助人数
                std::string m_strPeopleNumSlightInjury;             //轻伤人数
                std::string m_strPeopleNumSeriousInjury;            //重伤人数
                std::string m_strPeopleNumDeath;                    //死亡人数
                std::string m_strPoliceNumDispatch;                 //出动警力数
                std::string m_strPoliceCarNumDispatch;              //出动车辆数
                std::string m_strEconomyLoss;                       //经济损失
                std::string m_strRetrieveEconomyLoss;               //挽回经济损失
                std::string m_strFirePutOutTime;                    //火灾警情：火灾扑灭时间
                std::string m_strFireBuildingFirstType;             //火灾警情：起火建筑类别一级代码
                std::string m_strFireBuildingSecondType;            //火灾警情：起火建筑类别二级代码
                std::string m_strFireBuildingThirdType;             //火灾警情：起火建筑类别三级代码
                std::string m_strFireSourceType;                    //火灾警情：起火源类型代码；如：家具
                std::string m_strFireRegionType;                    //火灾警情：起火区域类型代码；如：阳台、客厅等
                std::string m_strFireCauseFirstType;                //火灾警情：火灾原因一级代码
                std::string m_strFireCauseSecondType;               //火灾警情：火灾原因二级代码
                std::string m_strFireCauseThirdType;                //火灾警情：火灾原因三级代码
                std::string m_strFireArea;                          //火灾警情：燃烧面积
                std::string m_strTrafficRoadLevel;                  //交通警情：道路等级代码
                std::string m_strTrafficAccidentLevel;              //交通警情：警情等级代码
                std::string m_strTrafficVehicleNo;                  //交通警情：事故车辆车牌号
                std::string m_strTrafficVehicleType;                //交通警情：事故车辆类型代码
                std::string m_strTrafficSlaveVehicleNo;             //交通警情：第二个车辆车牌号
                std::string m_strTrafficSlaveVehicleType;           //交通警情：第二个车辆类型代码
                std::string m_strEventType;                      //是否破获刑事案件
                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值
			};
			CBody m_oBody;
		};
	}
}
