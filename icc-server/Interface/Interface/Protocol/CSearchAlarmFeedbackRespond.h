#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmFeedbackRespond :
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

				unsigned int l_uiIndex = 0;
				auto it = m_oBody.m_vecData.begin();
				for (; it != m_oBody.m_vecData.end();++it)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "id", it->m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", it->m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_id", it->m_strProcessID);
					p_pJson->SetNodeValue(l_strPrefixPath + "result_type", it->m_strResultType);
					p_pJson->SetNodeValue(l_strPrefixPath + "result_content", it->m_strResultContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "leader_instruction", it->m_strLeaderInstruction);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", it->m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_edit", it->m_strTimeEdit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", it->m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", it->m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", it->m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_police_dispatch", it->m_strTimePoliceDispatch);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_police_arrived", it->m_strTimePoliceArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", it->m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_addr", it->m_strActualOccurAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_district_code", it->m_strFeedbackDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_code", it->m_strFeedbackDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_name", it->m_strFeedbackDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_code", it->m_strFeedbackCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_name", it->m_strFeedbackName);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_leader_code", it->m_strFeedbackLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_leader_name", it->m_strFeedbackLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_district_code", it->m_strProcessDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", it->m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", it->m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", it->m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", it->m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_code", it->m_strProcessLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_leader_name", it->m_strProcessLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", it->m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", it->m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", it->m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", it->m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", it->m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_code", it->m_strDispatchLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_leader_name", it->m_strDispatchLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_id", it->m_strPersonId);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_id_type", it->m_strPersonIdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_nationality", it->m_strPersonNationality);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_name", it->m_strPersonName);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_id", it->m_strPersonSlaveId);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_id_type", it->m_strPersonSlaveIdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_nationality", it->m_strPersonSlaveNationality);
					p_pJson->SetNodeValue(l_strPrefixPath + "person_slave_name", it->m_strPersonSlaveName);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_first_type", it->m_strAlarmFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_second_type", it->m_strAlarmSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_third_type", it->m_strAlarmThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_fourth_type", it->m_strAlarmFourthType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_dept_name", it->m_strAlarmAddrDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_first_type", it->m_strAlarmAddrFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_second_type", it->m_strAlarmAddrSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr_third_type", it->m_strAlarmAddrThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_longitude", it->m_strAlarmLongitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_latitude", it->m_strAlarmLatitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_region_type", it->m_strAlarmRegionType);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_location_type", it->m_strAlarmLocationType);
					p_pJson->SetNodeValue(l_strPrefixPath + "people_num_capture", it->m_strPeopleNumCapture);
					p_pJson->SetNodeValue(l_strPrefixPath + "people_num_rescue", it->m_strPeopleNumRescue);
					p_pJson->SetNodeValue(l_strPrefixPath + "people_num_slight_injury", it->m_strPeopleNumSlightInjury);
					p_pJson->SetNodeValue(l_strPrefixPath + "people_num_serious_injury", it->m_strPeopleNumSeriousInjury);
					p_pJson->SetNodeValue(l_strPrefixPath + "people_num_death", it->m_strPeopleNumDeath);
					p_pJson->SetNodeValue(l_strPrefixPath + "police_num_dispatch", it->m_strPoliceNumDispatch);
					p_pJson->SetNodeValue(l_strPrefixPath + "police_car_num_dispatch", it->m_strPoliceCarNumDispatch);
					p_pJson->SetNodeValue(l_strPrefixPath + "economy_loss", it->m_strEconomyLoss);
					p_pJson->SetNodeValue(l_strPrefixPath + "retrieve_economy_loss", it->m_strRetrieveEconomyLoss);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_put_out_time", it->m_strFirePutOutTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_first_type", it->m_strFireBuildingFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_second_type", it->m_strFireBuildingSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_third_type", it->m_strFireBuildingThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_source_type", it->m_strFireSourceType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_region_type", it->m_strFireRegionType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_first_type", it->m_strFireCauseFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_second_type", it->m_strFireCauseSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_cause_third_type", it->m_strFireCauseThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_area", it->m_strFireArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_road_level", it->m_strTrafficRoadLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_accident_level", it->m_strTrafficAccidentLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_vehicle_no", it->m_strTrafficVehicleNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_vehicle_type", it->m_strTrafficVehicleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_slave_vehicle_no", it->m_strTrafficSlaveVehicleNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "traffic_slave_vehicle_type", it->m_strTrafficSlaveVehicleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_solve_crown", it->m_strIsSolveCrown);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_solve_public_security", it->m_strIsSolvePublicSecurity);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", it->m_strIsInvolveForeign);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", it->m_strIsInvolvePolice);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", it->m_strIsInvolveGun);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", it->m_strIsInvolveKnife);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", it->m_strIsInvolveTerror);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", it->m_strIsInvolvePornography);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", it->m_strIsInvolveGamble);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", it->m_strIsInvolvePoison);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", it->m_strIsInvolveExt1);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", it->m_strIsInvolveExt2);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", it->m_strIsInvolveExt3);

					p_pJson->SetNodeValue(l_strPrefixPath + "event_type", it->m_strEventType);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", it->m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", it->m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", it->m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", it->m_strUpdateTime);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CFeedbackData
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
				std::string m_strIsSolveCrown;                   //�Ƿ��ƻ����°���
				std::string m_strIsSolvePublicSecurity;          //�Ƿ�鴦�ΰ�����
				std::string m_strIsInvolveForeign;               //�Ƿ�����
				std::string m_strIsInvolvePolice;                //�Ƿ��澯
				std::string m_strIsInvolveGun;                   //�Ƿ���ǹ
				std::string m_strIsInvolveKnife;                 //�Ƿ��浶
				std::string m_strIsInvolveTerror;                //�Ƿ����
				std::string m_strIsInvolvePornography;           //�Ƿ����
				std::string m_strIsInvolveGamble;                //�Ƿ����
				std::string m_strIsInvolvePoison;                //�Ƿ��涾
				std::string m_strIsInvolveExt1;                  //��չ�ֶ�1
				std::string m_strIsInvolveExt2;                  //��չ�ֶ�2
				std::string m_strIsInvolveExt3;                  //��չ�ֶ�3
				std::string m_strEventType;

				std::string m_strCreateUser;					 //������
				std::string m_strCreateTime;					 //����ʱ��
				std::string m_strUpdateUser;					 //�޸���
				std::string m_strUpdateTime;					 //�޸�ʱ��
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CFeedbackData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}