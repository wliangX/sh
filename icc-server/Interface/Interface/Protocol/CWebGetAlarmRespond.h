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

					// ��l_uiAlarmIndex+1���Ӿ�����Ӧ�Ĵ������б�
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

					// ��l_uiAlarmIndex+1���Ӿ�����Ӧ�ı�ע����ͨ�������б�
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

					// ��l_uiAlarmIndex+1���Ӿ�����Ӧ�ķ������б�
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
				std::string	m_strID;						//����id
				std::string	m_strMergeID;					//����ϲ�id
				std::string m_strTitle;						//�������
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
				std::string m_strAddr;						//����id��ϸ������ַ
				std::string m_strLongitude;					//����id����
				std::string m_strLatitude;					//����idά��
				std::string m_strState;						//����id״̬
				std::string m_strLevel;						//����id����
				std::string m_strSourceType;				//����id������Դ����
				std::string m_strSourceID;					//����id������Դid
				std::string m_strHandleType;				//����id��������
				std::string m_strFirstType;					//����idһ������
				std::string m_strSecondType;				//����id��������
				std::string m_strThirdType;					//����id��������
				std::string m_strFourthType;				//����id�ļ�����
				std::string m_strVehicleNo;					//����id��ͨ���ͱ������ƺ�
				std::string m_strVehicleType;				//����id��ͨ���ͱ���������
				std::string m_strSymbolCode;				//����id������ַ������ʾ����
				std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
				std::string m_strFireBuildingType;			//����id������ȼ�ս�������

				std::string m_strEventType;					//�¼����ͣ����Ÿ���

				std::string m_strCalledNoType;				//����id���������ֵ�����
				std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

				std::string m_strCallerNo;					//����id�����˺���
				std::string m_strCallerName;				//����id����������
				std::string m_strCallerAddr;				//����id�����˵�ַ
				std::string m_strCallerID;					//����id���������֤
				std::string m_strCallerIDType;				//����id���������֤����
				std::string m_strCallerGender;				//����id�������Ա�
				std::string m_strCallerAge;					//����id����������
				std::string m_strCallerBirthday;			//����id�����˳���������

				std::string m_strContactNo;					//����id��ϵ�˺���
				std::string m_strContactName;				//����id��ϵ������
				std::string m_strContactAddr;				//����id��ϵ�˵�ַ
				std::string m_strContactID;					//����id��ϵ�����֤
				std::string m_strContactIDType;				//����id��ϵ�����֤����
				std::string m_strContactGender;				//����id��ϵ���Ա�
				std::string m_strContactAge;				//����id��ϵ������
				std::string m_strContactBirthday;			//����id��ϵ�˳���������

				std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
				std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
				std::string m_strAdminDeptName;				//����id��Ͻ��λ����

				std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
				std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
				std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
				std::string m_strLeaderCode;				//����idֵ���쵼����
				std::string m_strLeaderName;				//����idֵ���쵼����
				std::string m_strReceiptCode;				//����id�Ӿ��˾���
				std::string m_strReceiptName;				//����id�Ӿ�������

				std::string m_strDispatchSuggestion;		//�������		
			};
			class CAlarmProcess
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
			};
			class CAlarmRemark
			{
			public:
				std::string m_strAlarmID;					//����ID
				std::string m_strFeedbackCode;				//�����˱��
				std::string m_strFeedbackName;				//����������
				std::string m_strFeedbackDeptCode;			//������λ���
				std::string m_strFeedbackDeptName;			//������λ����
				std::string m_strFeedbackTime;				//����ʱ��	
				std::string m_strContent;					//��ע����
			};
			class CAlarmFeedback
			{
			public:
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
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTotalCount;						// ��¼����
				std::string m_strStateID;							// ״̬����
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