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
				std::string m_strIsSolveCrown;                   //是否破获刑事案件
				std::string m_strIsSolvePublicSecurity;          //是否查处治安案件
				std::string m_strIsInvolveForeign;               //是否涉外
				std::string m_strIsInvolvePolice;                //是否涉警
				std::string m_strIsInvolveGun;                   //是否涉枪
				std::string m_strIsInvolveKnife;                 //是否涉刀
				std::string m_strIsInvolveTerror;                //是否涉恐
				std::string m_strIsInvolvePornography;           //是否涉黄
				std::string m_strIsInvolveGamble;                //是否涉赌
				std::string m_strIsInvolvePoison;                //是否涉毒
				std::string m_strIsInvolveExt1;                  //扩展字段1
				std::string m_strIsInvolveExt2;                  //扩展字段2
				std::string m_strIsInvolveExt3;                  //扩展字段3
				std::string m_strEventType;

				std::string m_strCreateUser;					 //创建人
				std::string m_strCreateTime;					 //创建时间
				std::string m_strUpdateUser;					 //修改人
				std::string m_strUpdateTime;					 //修改时间
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