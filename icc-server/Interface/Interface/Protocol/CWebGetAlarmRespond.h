#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarmRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/total_count", m_oBody.m_strTotalCount);
				p_pJson->SetNodeValue("/body/state_id", m_oBody.m_strStateID);

				unsigned int l_uiAlarmIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					std::string l_strAlarmPrefixPath("/body/alarm/" + std::to_string(l_uiAlarmIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "merge_id", alarm.m_strMergeID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "title", alarm.m_strTitle);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarm.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", alarm.m_strTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_occur_time", alarm.m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", alarm.m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "longitude", alarm.m_strLongitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "latitude", alarm.m_strLatitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", alarm.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", alarm.m_strLevel);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", alarm.m_strSourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", alarm.m_strSourceID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "handle_type", alarm.m_strHandleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", alarm.m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", alarm.m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", alarm.m_strThirdType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", alarm.m_strFourthType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_no", alarm.m_strVehicleNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_type", alarm.m_strVehicleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_code", alarm.m_strSymbolCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_addr", alarm.m_strSymbolAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fire_building_type", alarm.m_strFireBuildingType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "event_type", alarm.m_strEventType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", alarm.m_strCalledNoType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_called_no_type", alarm.m_strActualCalledNoType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_no", alarm.m_strCallerNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_name", alarm.m_strCallerName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_addr", alarm.m_strCallerAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id", alarm.m_strCallerID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id_type", alarm.m_strCallerIDType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_gender", alarm.m_strCallerGender);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_age", alarm.m_strCallerAge);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_birthday", alarm.m_strCallerBirthday);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", alarm.m_strContactNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_name", alarm.m_strContactName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_addr", alarm.m_strContactAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id", alarm.m_strContactID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_id_type", alarm.m_strContactIDType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_gender", alarm.m_strContactGender);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_age", alarm.m_strContactAge);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_birthday", alarm.m_strContactBirthday);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_district_code", alarm.m_strAdminDeptDistrictCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_code", alarm.m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_name", alarm.m_strAdminDeptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_district_code", alarm.m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", alarm.m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", alarm.m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_code", alarm.m_strLeaderCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "leader_name", alarm.m_strLeaderName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_code", alarm.m_strReceiptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", alarm.m_strReceiptName);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "dispatch_suggestion", alarm.m_strDispatchSuggestion);

					// 第l_uiAlarmIndex+1个接警单对应的处警单列表
					for (auto process : m_oBody.m_vecAlarmProcess.at(l_uiAlarmIndex))
					{
						std::string l_strPrefixPathProcess("/body/alarm_process/" + std::to_string(l_uiProcessIndex) + "/");
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", process.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", process.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", process.m_strState);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_edit", process.m_strTimeEdit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_submit", process.m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_arrived", process.m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_signed", process.m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_feedback", process.m_strTimeFeedBack);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_need_feedback", process.m_strIsNeedFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_code", process.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_code", process.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_name", process.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", process.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", process.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_code", process.m_strDispatchLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_name", process.m_strDispatchLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_suggestion", process.m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_instruction", process.m_strDispatchLeaderInstruction);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_district_code", process.m_strProcessDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_code", process.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_name", process.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_code", process.m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_name", process.m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_code", process.m_strProcessLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_name", process.m_strProcessLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_feedback", process.m_strProcessFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_instruction", process.m_strProcessLeaderInstruction);
						l_uiProcessIndex++;
					}

					// 第l_uiAlarmIndex+1个接警单对应的备注（普通反馈）列表
					for (auto remark : m_oBody.m_vecAlarmRemark.at(l_uiAlarmIndex))
					{
						std::string l_strPrefixPathRemark("/body/alarm_remark/" + std::to_string(l_uiRemarkIndex) + "/");
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "alarm_id", remark.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "feedback_code", remark.m_strFeedbackCode);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "feedback_name", remark.m_strFeedbackName);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "feedback_dept_code", remark.m_strFeedbackDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "feedback_dept_name", remark.m_strFeedbackDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "feedback_time", remark.m_strFeedbackTime);
						p_pJson->SetNodeValue(l_strPrefixPathRemark + "content", remark.m_strContent);
						l_uiRemarkIndex++;
					}

					// 第l_uiAlarmIndex+1个接警单对应的反馈单列表
					for (auto feedback : m_oBody.m_vecAlarmFeedback.at(l_uiAlarmIndex))
					{
						std::string l_strPrefixPathFeedback("/body/alarm_feedback/" + std::to_string(l_uiFeedbackIndex) + "/");
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "id", feedback.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_id", feedback.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_id", feedback.m_strProcessID);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "result_type", feedback.m_strResultType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "result_content", feedback.m_strResultContent);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "leader_instruction", feedback.m_strLeaderInstruction);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "state", feedback.m_strState);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_edit", feedback.m_strTimeEdit);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_submit", feedback.m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_arrived", feedback.m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_signed", feedback.m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_police_dispatch", feedback.m_strTimePoliceDispatch);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "time_police_arrived", feedback.m_strTimePoliceArrived);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "actual_occur_time", feedback.m_strActualOccurTime);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "actual_occur_addr", feedback.m_strActualOccurAddr);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_dept_district_code", feedback.m_strFeedbackDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_dept_code", feedback.m_strFeedbackDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_dept_name", feedback.m_strFeedbackDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_code", feedback.m_strFeedbackCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_name", feedback.m_strFeedbackName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_leader_code", feedback.m_strFeedbackLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "feedback_leader_name", feedback.m_strFeedbackLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_dept_district_code", feedback.m_strProcessDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_dept_code", feedback.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_dept_name", feedback.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_code", feedback.m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_name", feedback.m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_leader_code", feedback.m_strProcessLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "process_leader_name", feedback.m_strProcessLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_dept_district_code", feedback.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_dept_code", feedback.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_dept_name", feedback.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_code", feedback.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_name", feedback.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_leader_code", feedback.m_strDispatchLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "dispatch_leader_name", feedback.m_strDispatchLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_id", feedback.m_strPersonId);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_id_type", feedback.m_strPersonIdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_nationality", feedback.m_strPersonNationality);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_name", feedback.m_strPersonName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_slave_id", feedback.m_strPersonSlaveId);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_slave_id_type", feedback.m_strPersonSlaveIdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_slave_nationality", feedback.m_strPersonSlaveNationality);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "person_slave_name", feedback.m_strPersonSlaveName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_first_type", feedback.m_strAlarmFirstType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_second_type", feedback.m_strAlarmSecondType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_third_type", feedback.m_strAlarmThirdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_fourth_type", feedback.m_strAlarmFourthType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_addr_dept_name", feedback.m_strAlarmAddrDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_addr_first_type", feedback.m_strAlarmAddrFirstType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_addr_second_type", feedback.m_strAlarmAddrSecondType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_addr_third_type", feedback.m_strAlarmAddrThirdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_longitude", feedback.m_strAlarmLongitude);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_latitude", feedback.m_strAlarmLatitude);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_region_type", feedback.m_strAlarmRegionType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_location_type", feedback.m_strAlarmLocationType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "people_num_capture", feedback.m_strPeopleNumCapture);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "people_num_rescue", feedback.m_strPeopleNumRescue);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "people_num_slight_injury", feedback.m_strPeopleNumSlightInjury);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "people_num_serious_injury", feedback.m_strPeopleNumSeriousInjury);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "people_num_death", feedback.m_strPeopleNumDeath);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "police_num_dispatch", feedback.m_strPoliceNumDispatch);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "police_car_num_dispatch", feedback.m_strPoliceCarNumDispatch);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "economy_loss", feedback.m_strEconomyLoss);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "retrieve_economy_loss", feedback.m_strRetrieveEconomyLoss);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_put_out_time", feedback.m_strFirePutOutTime);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_building_first_type", feedback.m_strFireBuildingFirstType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_building_second_type", feedback.m_strFireBuildingSecondType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_building_third_type", feedback.m_strFireBuildingThirdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_source_type", feedback.m_strFireSourceType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_region_type", feedback.m_strFireRegionType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_cause_first_type", feedback.m_strFireCauseFirstType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_cause_second_type", feedback.m_strFireCauseSecondType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_cause_third_type", feedback.m_strFireCauseThirdType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "fire_area", feedback.m_strFireArea);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_road_level", feedback.m_strTrafficRoadLevel);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_accident_level", feedback.m_strTrafficAccidentLevel);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_vehicle_no", feedback.m_strTrafficVehicleNo);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_vehicle_type", feedback.m_strTrafficVehicleType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_slave_vehicle_no", feedback.m_strTrafficSlaveVehicleNo);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "traffic_slave_vehicle_type", feedback.m_strTrafficSlaveVehicleType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "event_type", feedback.m_strEventType);
						p_pJson->SetNodeValue(l_strPrefixPathFeedback + "alarm_called_no_type", feedback.m_strAlarmCalledNoType);
						l_uiFeedbackIndex++;
					}

					l_uiAlarmIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CAlarm
			{
			public:
				std::string	m_strID;						//警情id
				std::string	m_strMergeID;					//警情合并id
				std::string m_strTitle;						//警情标题
				std::string m_strContent;					//警情内容
				std::string m_strTime;						//警情id报警时间			
				std::string m_strActualOccurTime;			//警情id实际发生时间
				std::string m_strAddr;						//警情id详细发生地址
				std::string m_strLongitude;					//警情id经度
				std::string m_strLatitude;					//警情id维度
				std::string m_strState;						//警情id状态
				std::string m_strLevel;						//警情id级别
				std::string m_strSourceType;				//警情id报警来源类型
				std::string m_strSourceID;					//警情id报警来源id
				std::string m_strHandleType;				//警情id处理类型
				std::string m_strFirstType;					//警情id一级类型
				std::string m_strSecondType;				//警情id二级类型
				std::string m_strThirdType;					//警情id三级类型
				std::string m_strFourthType;				//警情id四级类型
				std::string m_strVehicleNo;					//警情id交通类型报警车牌号
				std::string m_strVehicleType;				//警情id交通类型报警车类型
				std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
				std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
				std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

				std::string m_strEventType;					//事件类型，逗号隔开

				std::string m_strCalledNoType;				//警情id报警号码字典类型
				std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

				std::string m_strCallerNo;					//警情id报警人号码
				std::string m_strCallerName;				//警情id报警人姓名
				std::string m_strCallerAddr;				//警情id报警人地址
				std::string m_strCallerID;					//警情id报警人身份证
				std::string m_strCallerIDType;				//警情id报警人身份证类型
				std::string m_strCallerGender;				//警情id报警人性别
				std::string m_strCallerAge;					//警情id报警人年龄
				std::string m_strCallerBirthday;			//警情id报警人出生年月日

				std::string m_strContactNo;					//警情id联系人号码
				std::string m_strContactName;				//警情id联系人姓名
				std::string m_strContactAddr;				//警情id联系人地址
				std::string m_strContactID;					//警情id联系人身份证
				std::string m_strContactIDType;				//警情id联系人身份证类型
				std::string m_strContactGender;				//警情id联系人性别
				std::string m_strContactAge;				//警情id联系人年龄
				std::string m_strContactBirthday;			//警情id联系人出生年月日

				std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
				std::string m_strAdminDeptCode;				//警情id管辖单位编码
				std::string m_strAdminDeptName;				//警情id管辖单位姓名

				std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
				std::string m_strReceiptDeptCode;			//警情id接警单位编码
				std::string m_strReceiptDeptName;			//警情id接警单位名称
				std::string m_strLeaderCode;				//警情id值班领导警号
				std::string m_strLeaderName;				//警情id值班领导姓名
				std::string m_strReceiptCode;				//警情id接警人警号
				std::string m_strReceiptName;				//警情id接警人姓名

				std::string m_strDispatchSuggestion;		//调派意见		
			};
			class CAlarmProcess
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
			};
			class CAlarmRemark
			{
			public:
				std::string m_strAlarmID;					//警情ID
				std::string m_strFeedbackCode;				//反馈人编号
				std::string m_strFeedbackName;				//反馈人姓名
				std::string m_strFeedbackDeptCode;			//反馈单位编号
				std::string m_strFeedbackDeptName;			//反馈单位名称
				std::string m_strFeedbackTime;				//反馈时间	
				std::string m_strContent;					//备注内容
			};
			class CAlarmFeedback
			{
			public:
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
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTotalCount;						// 记录总数
				std::string m_strStateID;							// 状态类型
				std::vector<CAlarm> m_vecAlarm;
				std::vector<std::vector<CAlarmProcess>> m_vecAlarmProcess;
				std::vector<std::vector<CAlarmRemark>> m_vecAlarmRemark;
				std::vector<std::vector<CAlarmFeedback>> m_vecAlarmFeedback;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}