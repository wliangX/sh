#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include "Protocol/CProcessInfoChange.h"
#include <Protocol/CAddOrUpdateProcessRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CAlarmInfoChange :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);

				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);

					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JJDBH", m_oBody.m_vecData[i].m_oAlarm.m_strID);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQGJZ", m_oBody.m_vecData[i].m_oAlarm.m_strTitle);// 警情关键词（比如涉疫、涉校、涉医等，是对特定警情核心要素的描述，是对警情分类的重要补充，接警员手工选择）
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/GLZJJDBH", m_oBody.m_vecData[i].m_oAlarm.m_strMergeID);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/BJDH", m_oBody.m_vecData[i].m_oAlarm.m_strCallerNo);
					////p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/LHLX", m_oBody.m_vecData[i].m_oAlarm.m_strCalledNo);
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/LHLX", m_oBody.m_vecData[i].m_oAlarm.m_strHandleType);

					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/BJSJ", m_oBody.m_vecData[i].m_oAlarm.m_strTime);  //??
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JJSJ", 
					//	m_oBody.m_vecData[i].m_oAlarm.m_strReceivedTime.empty() ? m_oBody.m_vecData[i].m_oAlarm.m_strTime : m_oBody.m_vecData[i].m_vecData[i].m_vecData[i].m_oAlarm.m_strReceivedTime);
				
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "msg_source", m_oBody.m_vecData[i].m_oAlarm.m_strMsgSource);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_district_code", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/id", m_oBody.m_vecData[i].m_oAlarm.m_strID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/merge_id", m_oBody.m_vecData[i].m_oAlarm.m_strMergeID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/called_no_type", m_oBody.m_vecData[i].m_oAlarm.m_strCalledNoType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/source_type", m_oBody.m_vecData[i].m_oAlarm.m_strSourceType);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/handle_type", m_oBody.m_vecData[i].m_oAlarm.m_strHandleType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/source_id", m_oBody.m_vecData[i].m_oAlarm.m_strSourceID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_code", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_name", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receiving_time", m_oBody.m_vecData[i].m_oAlarm.m_strTime);  //time变更字段名
					//p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/received_time", m_oBody.m_vecData[i].m_oAlarm.m_strReceivedTime);  //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/received_time",
						m_oBody.m_vecData[i].m_oAlarm.m_strReceivedTime.empty() ? m_oBody.m_vecData[i].m_oAlarm.m_strTime : m_oBody.m_vecData[i].m_oAlarm.m_strReceivedTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/first_submit_time", m_oBody.m_vecData[i].m_oAlarm.m_strFirstSubmitTime); //first_submit_time  服务自己记的	
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_no", m_oBody.m_vecData[i].m_oAlarm.m_strCallerNo);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_user_name", m_oBody.m_vecData[i].m_oAlarm.m_strCallerUserName); //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_addr", m_oBody.m_vecData[i].m_oAlarm.m_strCallerAddr);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_name", m_oBody.m_vecData[i].m_oAlarm.m_strCallerName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_gender", m_oBody.m_vecData[i].m_oAlarm.m_strCallerGender);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/contact_no", m_oBody.m_vecData[i].m_oAlarm.m_strContactNo);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_id_type", m_oBody.m_vecData[i].m_oAlarm.m_strCallerIDType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/caller_id", m_oBody.m_vecData[i].m_oAlarm.m_strCallerID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/alarm_addr", m_oBody.m_vecData[i].m_oAlarm.m_strAlarmAddr);  //新增  报警地址
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/addr", m_oBody.m_vecData[i].m_oAlarm.m_strAddr);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/content", m_oBody.m_vecData[i].m_oAlarm.m_strContent);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/admin_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strAdminDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/first_type", m_oBody.m_vecData[i].m_oAlarm.m_strFirstType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/second_type", m_oBody.m_vecData[i].m_oAlarm.m_strSecondType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/third_type", m_oBody.m_vecData[i].m_oAlarm.m_strThirdType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/fourth_type", m_oBody.m_vecData[i].m_oAlarm.m_strFourthType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/symbol_code", m_oBody.m_vecData[i].m_oAlarm.m_strSymbolCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/erpetrators_number", m_oBody.m_vecData[i].m_oAlarm.m_strErpetratorsNumber); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_armed", m_oBody.m_vecData[i].m_oAlarm.m_strIsArmed);  //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_hazardous_substances", m_oBody.m_vecData[i].m_oAlarm.m_strIsHazardousSubstances);  //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_explosion_or_leakage", m_oBody.m_vecData[i].m_oAlarm.m_strIsExplosionOrLeakage); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/desc_of_trapped", m_oBody.m_vecData[i].m_oAlarm.m_strDescOfTrapped); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/desc_of_injured", m_oBody.m_vecData[i].m_oAlarm.m_strDescOfInjured); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/desc_of_dead", m_oBody.m_vecData[i].m_oAlarm.m_strDescOfDead); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_foreign_language", m_oBody.m_vecData[i].m_oAlarm.m_strIsForeignLanguage); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/longitude", m_oBody.m_vecData[i].m_oAlarm.m_strLongitude);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/latitude", m_oBody.m_vecData[i].m_oAlarm.m_strLatitude);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/manual_longitude", m_oBody.m_vecData[i].m_oAlarm.m_strManualLongitude); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/manual_latitude", m_oBody.m_vecData[i].m_oAlarm.m_strManualLatitude);//新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/remark", m_oBody.m_vecData[i].m_oAlarm.m_strRemark);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/level", m_oBody.m_vecData[i].m_oAlarm.m_strLevel);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/state", m_oBody.m_vecData[i].m_oAlarm.m_strState);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/emergency_rescue_level", m_oBody.m_vecData[i].m_oAlarm.m_strEmergencyRescueLevel); //新增  
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/vehicle_type", m_oBody.m_vecData[i].m_oAlarm.m_strVehicleType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/vehicle_no", m_oBody.m_vecData[i].m_oAlarm.m_strVehicleNo);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_hazardous_vehicle", m_oBody.m_vecData[i].m_oAlarm.m_strIsHazardousVehicle); //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/create_time", m_oBody.m_vecData[i].m_oAlarm.m_strCreateTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/update_time", m_oBody.m_vecData[i].m_oAlarm.m_strUpdateTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/label", m_oBody.m_vecData[i].m_oAlarm.m_strTitle);  //title变更字段名
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_privacy", m_oBody.m_vecData[i].m_oAlarm.m_strPrivacy);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_srv_name", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptSrvName);  //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/admin_dept_name", m_oBody.m_vecData[i].m_oAlarm.m_strAdminDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/admin_dept_org_code", m_oBody.m_vecData[i].m_oAlarm.m_strAdminDeptOrgCode);   //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_name", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_org_code", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptOrgCode);   //新增
					if (!m_oBody.m_vecData[i].m_oAlarm.m_strDeleteFlag.empty())
					{
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_delete", m_oBody.m_vecData[i].m_oAlarm.m_strDeleteFlag);//is_delete不在协议中传递
					}
					else
					{
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_delete", "0");
					}
					
					if (!m_oBody.m_vecData[i].m_oAlarm.m_strIsInvalid.empty())
					{
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_invalid", m_oBody.m_vecData[i].m_oAlarm.m_strIsInvalid);
					}
					else
					{
						p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_invalid", "0");
					}
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/business_status", m_oBody.m_vecData[i].m_oAlarm.m_strBusinessState);  //新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_seatno", m_oBody.m_vecData[i].m_oAlarm.m_strSeatNo);   //seatno改名
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_merge", m_oBody.m_vecData[i].m_oAlarm.m_strIsMerge);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/create_user", m_oBody.m_vecData[i].m_oAlarm.m_strCreateUser);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/update_user", m_oBody.m_vecData[i].m_oAlarm.m_strUpdateUser);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_visitor", m_oBody.m_vecData[i].m_oAlarm.m_strIsVisitor);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_feedback", m_oBody.m_vecData[i].m_oAlarm.m_strIsFeedBack);

					//合表新增
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/jurisdictional_orgcode", m_oBody.m_vecData[i].m_oAlarm.m_strJurisdictionalOrgcode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/jurisdictional_orgname", m_oBody.m_vecData[i].m_oAlarm.m_strJurisdictionalOrgname);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/jurisdictional_orgidentifier", m_oBody.m_vecData[i].m_oAlarm.m_strJurisdictionalOrgidentifier);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/had_push", m_oBody.m_vecData[i].m_oAlarm.m_strHadPush);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_over", m_oBody.m_vecData[i].m_oAlarm.m_strIsOver);

					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/createTeminal", m_oBody.m_vecData[i].m_oAlarm.m_strCreateTeminal);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/updateTeminal", m_oBody.m_vecData[i].m_oAlarm.m_strUpdateTeminal);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/alarm_source_type", m_oBody.m_vecData[i].m_oAlarm.m_strAlarmSourceType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/alarm_source_id", m_oBody.m_vecData[i].m_oAlarm.m_strAlarmSourceId);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/centre_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strCentreDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/centre_relation_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strCentreRelationDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_district_name", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptDistrictName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/develop_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strAlarmSystemReceiptDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/develop_dept_name", m_oBody.m_vecData[i].m_oAlarm.m_strAlarmSystemReceiptDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/linked_dispatch_code", m_oBody.m_vecData[i].m_oAlarm.m_strLinkedDispatchCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/is_signed", m_oBody.m_vecData[i].m_oAlarm.m_strIsSigned);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/over_time", m_oBody.m_vecData[i].m_oAlarm.m_strOverTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_first_type", m_oBody.m_vecData[i].m_oAlarm.m_strInitialFirstType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_second_type", m_oBody.m_vecData[i].m_oAlarm.m_strInitialSecondType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_third_type", m_oBody.m_vecData[i].m_oAlarm.m_strInitialThirdType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_fourth_type", m_oBody.m_vecData[i].m_oAlarm.m_strInitialFourthType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_admin_dept_code", m_oBody.m_vecData[i].m_oAlarm.m_strInitialAdminDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_admin_dept_name", m_oBody.m_vecData[i].m_oAlarm.m_strInitialAdminDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_admin_dept_org_code", m_oBody.m_vecData[i].m_oAlarm.m_strInitialAdminDeptOrgCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/initial_addr", m_oBody.m_vecData[i].m_oAlarm.m_strInitialAddr);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/receipt_dept_short_name", m_oBody.m_vecData[i].m_oAlarm.m_strReceiptDeptShortName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/admin_dept_short_name", m_oBody.m_vecData[i].m_oAlarm.m_strAdminDeptShortName);

					for (size_t j = 0; j < m_oBody.m_vecData[i].m_vecProcessData.size(); ++j)
					{
						std::string l_strProcessIndex = std::to_string(j);
						CAddOrUpdateProcessRequest::CProcessData& tmp_oProcessData = m_oBody.m_vecData[i].m_vecProcessData[j];

						std::string l_strPrefixPath("/body/data/" + l_strIndex + "/process_data/" + l_strProcessIndex + "/");
						/*
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJDBH", tmp_oProcessData.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/XZQHDM", tmp_oProcessData.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/JJDBH", tmp_oProcessData.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/ZXBH", tmp_oProcessData.m_strSeatCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJDWDM", tmp_oProcessData.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJDWID", tmp_oProcessData.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJDWMC", tmp_oProcessData.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJYBH", tmp_oProcessData.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJYID", tmp_oProcessData.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJYXM", tmp_oProcessData.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/PJYJ", tmp_oProcessData.m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/CJSJ", tmp_oProcessData.m_strCreateTime);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/CJDWDM", tmp_oProcessData.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/CJDWID", tmp_oProcessData.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/CJDWMC", tmp_oProcessData.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/JQCLZTDM", tmp_oProcessData.m_strState);

						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/createuser", m_oBody.m_vecData[i].m_oAlarm.m_strCreateUser);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/RKSJ", m_oBody.m_vecData[i].m_oAlarm.m_strCreateTime);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/updateuser", m_oBody.m_vecData[i].m_oAlarm.m_strUpdateUser);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/GXSJ", m_oBody.m_vecData[i].m_oAlarm.m_strUpdateTime);
						p_pJson->SetNodeValue(l_strPrefixPath + l_strProcessIndex + "/delflag", m_oBody.m_vecData[i].m_oAlarm.m_strDeleteFlag);
						*/
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", tmp_oProcessData.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "id", tmp_oProcessData.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", tmp_oProcessData.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", tmp_oProcessData.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", tmp_oProcessData.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", tmp_oProcessData.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPath + "record_id", tmp_oProcessData.m_strRecordID);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", tmp_oProcessData.m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", tmp_oProcessData.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", tmp_oProcessData.m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", tmp_oProcessData.m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", tmp_oProcessData.m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_name", tmp_oProcessData.m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_code", tmp_oProcessData.m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", tmp_oProcessData.m_strDispatchPersonnel);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", tmp_oProcessData.m_strDispatchVehicles);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", tmp_oProcessData.m_strDispatchBoats);
						p_pJson->SetNodeValue(l_strPrefixPath + "state", tmp_oProcessData.m_strState);
						p_pJson->SetNodeValue(l_strPrefixPath + "create_time", tmp_oProcessData.m_strCreateTime);
						p_pJson->SetNodeValue(l_strPrefixPath + "update_time", tmp_oProcessData.m_strUpdateTime);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", tmp_oProcessData.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", tmp_oProcessData.m_strDispatchDeptOrgCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", tmp_oProcessData.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", tmp_oProcessData.m_strProcessDeptOrgCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", tmp_oProcessData.m_strProcessObjectType);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", tmp_oProcessData.m_strProcessObjectName);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", tmp_oProcessData.m_strProcessObjectCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "business_status", tmp_oProcessData.m_strBusinessStatus);
						p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", tmp_oProcessData.m_strSeatCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", tmp_oProcessData.m_strCancelTime);
						p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", tmp_oProcessData.m_strCancelReason);
						p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", tmp_oProcessData.m_strIsAutoAssignJob);
						p_pJson->SetNodeValue(l_strPrefixPath + "create_user", tmp_oProcessData.m_strCreateUser);
						p_pJson->SetNodeValue(l_strPrefixPath + "update_user", tmp_oProcessData.m_strUpdateUser);
									
						//合表新增
						p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", tmp_oProcessData.m_strProcessObjectID);
						
						p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", tmp_oProcessData.m_strTransfDeptOrjCode);//	移交处警单位短码
						p_pJson->SetNodeValue(l_strPrefixPath + "is_over", tmp_oProcessData.m_strIsOver);//	是否结案
						p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", tmp_oProcessData.m_strOverRemark);//	结案说明
						p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", tmp_oProcessData.m_strParentID);//	上级派警单编号
						p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", tmp_oProcessData.m_strGZLDM);//	工作流代码

						p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", tmp_oProcessData.m_strCreateTeminal);//	
						p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", tmp_oProcessData.m_strUpdateTeminal);//
						p_pJson->SetNodeValue(l_strPrefixPath + "received_time", tmp_oProcessData.m_strReceivedTime);//	

						p_pJson->SetNodeValue(l_strPrefixPath + "centre_process_dept_code", tmp_oProcessData.m_strCentreProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "centre_alarm_dept_code", tmp_oProcessData.m_strCentreAlarmDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_name", tmp_oProcessData.m_strDispatchDeptDistrictName);
						p_pJson->SetNodeValue(l_strPrefixPath + "linked_dispatch_code", tmp_oProcessData.m_strLinkedDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "over_time", tmp_oProcessData.m_strOverTime);
						p_pJson->SetNodeValue(l_strPrefixPath + "finished_timeout_state", tmp_oProcessData.m_strFinishedTimeoutState);
						p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_short_name", tmp_oProcessData.m_strProcessDeptShortName);
						p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_short_name", tmp_oProcessData.m_strDispatchDeptShortName);
					}
				}

				return p_pJson->ToString();
			}


		public:
			CHeader m_oHeader;

            class CBody
            {
            public:

				//class CProcessData
				//{
				//	public:
				//		std::string m_strID;
				//		//   std::string m_strCaseId;					//警情ID
				//		std::string m_strSeatNo;		    		//处置坐席
				//	 //  std::string m_strDispatchOrgId;				//调派单位                
				//	 //   std::string m_strDispatchTime;				//调派时间           
				//		std::string m_strCreateUser;				//创建人
				//		std::string m_strCreateTime;				//创建时间
				//		std::string m_strUpdateUser;				//修改人,取最后一次修改值
				//		std::string m_strUpdateTime;				//修改时间,取最后一次修改值
				//		std::string m_strDelFlag;				    //0有效，1删除
				//	   // std::string m_strStatus;				    //状态
				//	   // std::string m_strProcessCode;

				//		std::string m_strAlarmID;					//警情ID
				//		std::string m_strState;						//处警单状态
				//		std::string m_strTimeEdit;					//派警单填写时间
				//		std::string m_strTimeSubmit;				//派警单提交时间
				//		std::string m_strTimeArrived;				//派警单系统到达时间
				//		std::string m_strTimeSigned;				//处警单位签收时间
				//		std::string m_strTimeFeedBack;				//处警单位反馈时间
				//		std::string m_strIsNeedFeedback;			//是否需要反馈
				//		std::string m_strDispatchDeptDistrictCode;	//派警单位行政区划
				//		std::string m_strDispatchDeptCode;			//派警单位代码
				//		std::string m_strDispatchDeptName;			//派警单位名称
				//		std::string m_strDispatchCode;				//派警人警号
				//		std::string m_strDispatchName;				//派警人姓名
				//		std::string m_strDispatchLeaderCode;		//派警单位值班领导警号
				//		std::string m_strDispatchLeaderName;		//派警单位值班领导姓名
				//		std::string m_strDispatchSuggestion;		//派警单位派警意见
				//		std::string m_strDispatchLeaderInstruction;	//派警单位领导指示
				//		std::string m_strProcessDeptDistrictCode;	//处警单位行政区划
				//		std::string m_strProcessDeptCode;			//处警单位代码
				//		std::string m_strProcessDeptName;			//处警单位名称
				//		std::string m_strProcessCode;				//处警人警号
				//		std::string m_strProcessName;				//处警人姓名
				//		std::string m_strProcessLeaderCode;			//处警单位值班领导警号
				//		std::string m_strProcessLeaderName;			//处警单位值班领导姓名
				//		std::string m_strProcessFeedback;			//处警反馈
				//		std::string m_strProcessLeaderInstruction;	//处警单位领导指示
				//		std::string m_strCancelReason;				//取消原因
				//};

                class CData
                {
                public:
					CAlarmInfo                  m_oAlarm;
     //               std::string m_strID;
					//std::string m_strTitle;                     //警情标签
     //               std::string m_strMerge_ID;					//警情合并ID
     //               std::string m_strCallerNo;					//警情id报警人号码
     //               std::string m_strCalledNo;		    		//警情id报警号码字典类型
     //               std::string m_strCallingTime;		    	//呼入时间
     //               std::string m_strAnsweringTime;		    	//应答时间
     //               std::string m_strSeatNo;		    		//接警坐席号
     //               std::string m_strContact;				    //联系人
     //               std::string m_strContactNo;					//联系电话
     //               std::string m_strContactSex;				//联系人性别
     //               std::string m_strCaseDestrict;		        //案发行政区划
     //              // std::string m_strAreaOrg;		            //辖区单位
     //               std::string m_strCaseAddress;		        //案发地址
     //               std::string m_strLongitude;					//经度
     //               std::string m_strLatitude;					//维度
     //               std::string m_strCaseTime;					//案发时间
     //               std::string m_strCaseType;					//案件类型
     //               std::string m_strCaseSubType;				//案件中类
     //               std::string m_strCaseThreeType;				//案件小类
     //               std::string m_strCaseSource;				//案件来源
     //               std::string m_strCaseLevel;					//案件级别
     //               std::string m_strCaseDesc;					//案情描述
     //               std::string m_strCreateUser;				//创建人
     //               std::string m_strCreateTime;				//创建时间
     //               std::string m_strUpdateUser;				//修改人,取最后一次修改值
     //               std::string m_strUpdateTime;				//修改时间,取最后一次修改值
     //               std::string m_strDelFlag;				    //0有效，1删除
     //               std::string m_strStatus;				    //警情状态

					//std::string m_strReceiptDeptCode;		    //接警单位代码
					//std::string m_strCalledNoType;				//接警类型（110、122、119自接警和其他接警等）
					//std::string m_strRecordId;					//接警录音号
					//std::string m_strReceiptCode;				//接警员编号
					//std::string m_strReceiptName;				//接警员姓名
					//std::string m_strCommitTime;				//接警完成时间
					//std::string m_strPhoneName;					//报警电话用户姓名（由查号系统自动产生）
					//std::string m_strPhoneId;					//用户身份证编号（由查号系统自动产生）
					//std::string m_strPhoneAddr;					//报警电话用户地址（由查号系统自动产生）
					//std::string m_strCallerName;				//报警人姓名
					//std::string m_strCallerId;					//报警人身份证编号
					//std::string m_strCallerGender;				//报警人性别代码
					//std::string m_strAdminDeptCode;				//管辖单位代码
					//std::string m_strAdminDeptName;				//管辖单位名称
					//std::string m_strSignNum;					// 特征点编号（如路灯杆、视频安装点等辅助定位设施的编号）

					//std::string m_strHandleType;				//来话类型

					std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;   //处警情况
                };

				std::string m_strCount;
				std::string m_strAllCount;

				std::vector<CData> m_vecData;
            };
			CBody m_oBody;
		};
	}
}
