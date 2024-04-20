#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CGetAlarmLogFeedBackResourceRespond :
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
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strValueNum = std::to_string(i);
                    std::string l_strPrefixPath = "/body/data/" + l_strValueNum;
                    p_pJson->SetNodeValue(l_strPrefixPath + "/guid", m_oBody.m_vecData.at(i).m_strGUID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/id", m_oBody.m_vecData.at(i).m_strID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_id", m_oBody.m_vecData.at(i).m_strAlarmID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_id", m_oBody.m_vecData.at(i).m_strProcessID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/result_type", m_oBody.m_vecData.at(i).m_strResultType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/result_content", m_oBody.m_vecData.at(i).m_strResultContent);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/leader_instruction", m_oBody.m_vecData.at(i).m_strLeaderInstruction);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/state", m_oBody.m_vecData.at(i).m_strState);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_edit", m_oBody.m_vecData.at(i).m_strTimeEdit);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_submit", m_oBody.m_vecData.at(i).m_strTimeSubmit);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_arrived", m_oBody.m_vecData.at(i).m_strTimeArrived);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_signed", m_oBody.m_vecData.at(i).m_strTimeSigned);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_police_dispatch", m_oBody.m_vecData.at(i).m_strTimePoliceDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/time_police_arrived", m_oBody.m_vecData.at(i).m_strTimePoliceArrived);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/actual_occur_time", m_oBody.m_vecData.at(i).m_strActualOccurTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/actual_occur_addr", m_oBody.m_vecData.at(i).m_strActualOccurAddr);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_dept_district_code", m_oBody.m_vecData.at(i).m_strFeedbackDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_dept_code", m_oBody.m_vecData.at(i).m_strFeedbackDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_dept_name", m_oBody.m_vecData.at(i).m_strFeedbackDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_code", m_oBody.m_vecData.at(i).m_strFeedbackCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_name", m_oBody.m_vecData.at(i).m_strFeedbackName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_leader_code", m_oBody.m_vecData.at(i).m_strFeedbackLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/feedback_leader_name", m_oBody.m_vecData.at(i).m_strFeedbackLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_dept_district_code", m_oBody.m_vecData.at(i).m_strProcessDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_dept_code", m_oBody.m_vecData.at(i).m_strProcessDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_dept_name", m_oBody.m_vecData.at(i).m_strProcessDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_code", m_oBody.m_vecData.at(i).m_strProcessCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_name", m_oBody.m_vecData.at(i).m_strProcessName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_leader_code", m_oBody.m_vecData.at(i).m_strProcessLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/process_leader_name", m_oBody.m_vecData.at(i).m_strProcessLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_dept_district_code", m_oBody.m_vecData.at(i).m_strDispatchDeptDistrictCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_dept_code", m_oBody.m_vecData.at(i).m_strDispatchDeptCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_dept_name", m_oBody.m_vecData.at(i).m_strDispatchDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_code", m_oBody.m_vecData.at(i).m_strDispatchCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_name", m_oBody.m_vecData.at(i).m_strDispatchName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_leader_code", m_oBody.m_vecData.at(i).m_strDispatchLeaderCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/dispatch_leader_name", m_oBody.m_vecData.at(i).m_strDispatchLeaderName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_id", m_oBody.m_vecData.at(i).m_strPersonId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_id_type", m_oBody.m_vecData.at(i).m_strPersonIdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_nationality", m_oBody.m_vecData.at(i).m_strPersonNationality);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_name", m_oBody.m_vecData.at(i).m_strPersonName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_slave_id", m_oBody.m_vecData.at(i).m_strPersonSlaveId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_slave_id_type", m_oBody.m_vecData.at(i).m_strPersonSlaveIdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_slave_nationality", m_oBody.m_vecData.at(i).m_strPersonSlaveNationality);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/person_slave_name", m_oBody.m_vecData.at(i).m_strPersonSlaveName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_first_type", m_oBody.m_vecData.at(i).m_strAlarmFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_second_type", m_oBody.m_vecData.at(i).m_strAlarmSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_third_type", m_oBody.m_vecData.at(i).m_strAlarmThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_fourth_type", m_oBody.m_vecData.at(i).m_strAlarmFourthType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_addr_dept_name", m_oBody.m_vecData.at(i).m_strAlarmAddrDeptName);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_addr_first_type", m_oBody.m_vecData.at(i).m_strAlarmAddrFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_addr_second_type", m_oBody.m_vecData.at(i).m_strAlarmAddrSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_addr_third_type", m_oBody.m_vecData.at(i).m_strAlarmAddrThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_longitude", m_oBody.m_vecData.at(i).m_strAlarmLongitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_latitude", m_oBody.m_vecData.at(i).m_strAlarmLatitude);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_region_type", m_oBody.m_vecData.at(i).m_strAlarmRegionType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_location_type", m_oBody.m_vecData.at(i).m_strAlarmLocationType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/people_num_capture", m_oBody.m_vecData.at(i).m_strPeopleNumCapture);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/people_num_rescue", m_oBody.m_vecData.at(i).m_strPeopleNumRescue);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/people_num_slight_injury", m_oBody.m_vecData.at(i).m_strPeopleNumSlightInjury);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/people_num_serious_injury", m_oBody.m_vecData.at(i).m_strPeopleNumSeriousInjury);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/people_num_death", m_oBody.m_vecData.at(i).m_strPeopleNumDeath);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/police_num_dispatch", m_oBody.m_vecData.at(i).m_strPoliceNumDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/police_car_num_dispatch", m_oBody.m_vecData.at(i).m_strPoliceCarNumDispatch);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/economy_loss", m_oBody.m_vecData.at(i).m_strEconomyLoss);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/retrieve_economy_loss", m_oBody.m_vecData.at(i).m_strRetrieveEconomyLoss);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_put_out_time", m_oBody.m_vecData.at(i).m_strFirePutOutTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_building_first_type", m_oBody.m_vecData.at(i).m_strFireBuildingFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_building_second_type", m_oBody.m_vecData.at(i).m_strFireBuildingSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_building_third_type", m_oBody.m_vecData.at(i).m_strFireBuildingThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_source_type", m_oBody.m_vecData.at(i).m_strFireSourceType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_region_type", m_oBody.m_vecData.at(i).m_strFireRegionType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_cause_first_type", m_oBody.m_vecData.at(i).m_strFireCauseFirstType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_cause_second_type", m_oBody.m_vecData.at(i).m_strFireCauseSecondType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_cause_third_type", m_oBody.m_vecData.at(i).m_strFireCauseThirdType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/fire_area", m_oBody.m_vecData.at(i).m_strFireArea);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_road_level", m_oBody.m_vecData.at(i).m_strTrafficRoadLevel);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_accident_level", m_oBody.m_vecData.at(i).m_strTrafficAccidentLevel);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_vehicle_no", m_oBody.m_vecData.at(i).m_strTrafficVehicleNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_vehicle_type", m_oBody.m_vecData.at(i).m_strTrafficVehicleType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_slave_vehicle_no", m_oBody.m_vecData.at(i).m_strTrafficSlaveVehicleNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/traffic_slave_vehicle_type", m_oBody.m_vecData.at(i).m_strTrafficSlaveVehicleType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/event_type", m_oBody.m_vecData.at(i).m_strEventType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/flag", m_oBody.m_vecData.at(i).m_strFlag);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_called_no_type", m_oBody.m_vecData.at(i).m_strAlarmCalledNoType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/create_user", m_oBody.m_vecData.at(i).m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/create_time", m_oBody.m_vecData.at(i).m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/update_user", m_oBody.m_vecData.at(i).m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "/update_time", m_oBody.m_vecData.at(i).m_strUpdateTime);
                } 
                    return p_pJson->ToString(); 
            }

        public:      

            class CData
            {
            public:
                std::string m_strGUID;                           //��Դguid
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
                std::string m_strEventType;                   //�Ƿ��ƻ����°���
                std::string m_strFlag;
                std::string m_strCreateUser;				//������
                std::string m_strCreateTime;				//����ʱ��
                std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
                std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ			
            };

          
            class CBody
            {
            public:                
                std::vector<CData> m_vecData;
            };
            CHeader m_oHeader;
            CBody	m_oBody;
        };
    }
}