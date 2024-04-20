/**/
#pragma once
#include <Protocol/CHeader.h>
#include <PGClient/IResultSet.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmInfo
		{
			public:
			bool ParseString(const std::string &p_strPrefix, JsonParser::IJsonPtr &p_pJson)
			{
				m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix+"/msg_source", "");

				m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue(p_strPrefix+"/receipt_dept_district_code", "");
				m_strReceiptDeptCode = p_pJson->GetNodeValue(p_strPrefix+"/receipt_dept_code", "");
				m_strID = p_pJson->GetNodeValue(p_strPrefix+"/id", "");
				m_strMergeID = p_pJson->GetNodeValue(p_strPrefix+"/merge_id", "");
				m_strMergeType = p_pJson->GetNodeValue(p_strPrefix + "/merge_type", "");
				m_strCalledNoType = p_pJson->GetNodeValue(p_strPrefix+"/called_no_type", "");
				m_strSourceType = p_pJson->GetNodeValue(p_strPrefix+"/source_type", "");
				m_strHandleType = p_pJson->GetNodeValue(p_strPrefix+"/handle_type", "");
				m_strSourceID = p_pJson->GetNodeValue(p_strPrefix+"/source_id", "");
				m_strReceiptCode = p_pJson->GetNodeValue(p_strPrefix+"/receipt_code", "");
				m_strReceiptName = p_pJson->GetNodeValue(p_strPrefix+"/receipt_name", "");
				m_strTime = p_pJson->GetNodeValue(p_strPrefix+"/receiving_time", "");  //time����ֶ���
				m_strReceivedTime = p_pJson->GetNodeValue(p_strPrefix+"/received_time", "");  //����
				m_strFirstSubmitTime = p_pJson->GetNodeValue(p_strPrefix + "/first_submit_time", "");  //first_submit_time  �����Լ��ǵ�
				m_strCallerNo = p_pJson->GetNodeValue(p_strPrefix+"/caller_no", "");
				m_strCallerUserName = p_pJson->GetNodeValue(p_strPrefix + "/caller_user_name", ""); //����
				m_strCallerAddr = p_pJson->GetNodeValue(p_strPrefix+"/caller_addr", "");
				m_strCallerName = p_pJson->GetNodeValue(p_strPrefix+"/caller_name", "");
				m_strCallerGender = p_pJson->GetNodeValue(p_strPrefix+"/caller_gender", "");
				m_strContactNo = p_pJson->GetNodeValue(p_strPrefix+"/contact_no", "");
				m_strCallerIDType = p_pJson->GetNodeValue(p_strPrefix+"/caller_id_type", "");
				m_strCallerID = p_pJson->GetNodeValue(p_strPrefix+"/caller_id", "");
				m_strAlarmAddr = p_pJson->GetNodeValue(p_strPrefix+"/alarm_addr", "");  //����  ������ַ
				m_strAddr = p_pJson->GetNodeValue(p_strPrefix+"/addr", ""); 
				m_strContent = p_pJson->GetNodeValue(p_strPrefix+"/content", "");
				m_strAdminDeptCode = p_pJson->GetNodeValue(p_strPrefix+"/admin_dept_code", "");
				m_strFirstType = p_pJson->GetNodeValue(p_strPrefix+"/first_type", "");
				m_strSecondType = p_pJson->GetNodeValue(p_strPrefix+"/second_type", "");
				m_strThirdType = p_pJson->GetNodeValue(p_strPrefix+"/third_type", "");
				m_strFourthType = p_pJson->GetNodeValue(p_strPrefix+"/fourth_type", "");
				m_strSymbolCode = p_pJson->GetNodeValue(p_strPrefix+"/symbol_code", "");
				m_strErpetratorsNumber = p_pJson->GetNodeValue(p_strPrefix+"/erpetrators_number", ""); //����  
				m_strIsArmed = p_pJson->GetNodeValue(p_strPrefix+"/is_armed", "");  //����  
				m_strHandleTypeCode = p_pJson->GetNodeValue(p_strPrefix + "/handle_type_code", "");  //����  
				if (m_strIsArmed.empty())
				{
					m_strIsArmed = "0";
				}
				m_strIsHazardousSubstances = p_pJson->GetNodeValue(p_strPrefix+"/is_hazardous_substances", "");  //����  
				if (m_strIsHazardousSubstances.empty())
				{
					m_strIsHazardousSubstances = "0";
				}
				m_strIsExplosionOrLeakage = p_pJson->GetNodeValue(p_strPrefix+"/is_explosion_or_leakage", ""); //����  
				if (m_strIsExplosionOrLeakage.empty())
				{
					m_strIsExplosionOrLeakage = "0";
				}
				m_strDescOfTrapped = p_pJson->GetNodeValue(p_strPrefix+"/desc_of_trapped", ""); //����  
				m_strDescOfInjured = p_pJson->GetNodeValue(p_strPrefix+"/desc_of_injured", ""); //����  
				m_strDescOfDead = p_pJson->GetNodeValue(p_strPrefix+"/desc_of_dead", ""); //����  
				m_strIsForeignLanguage = p_pJson->GetNodeValue(p_strPrefix+"/is_foreign_language", ""); //���� 
				if (m_strIsForeignLanguage.empty())
				{
					m_strIsForeignLanguage = "0";
				}
				m_strLongitude = p_pJson->GetNodeValue(p_strPrefix+"/longitude", "");
				m_strLatitude = p_pJson->GetNodeValue(p_strPrefix+"/latitude", "");
				m_strManualLongitude = p_pJson->GetNodeValue(p_strPrefix+"/manual_longitude", ""); //����  
				m_strManualLatitude = p_pJson->GetNodeValue(p_strPrefix+"/manual_latitude", "");//����  
				m_strRemark = p_pJson->GetNodeValue(p_strPrefix+"/remark", "");
				m_strLevel = p_pJson->GetNodeValue(p_strPrefix+"/level", "").empty() ? "04" : p_pJson->GetNodeValue(p_strPrefix+"/level", "");
				m_strState = p_pJson->GetNodeValue(p_strPrefix+"/state", "");
				m_strEmergencyRescueLevel = p_pJson->GetNodeValue(p_strPrefix+"/emergency_rescue_level", ""); //����  
				m_strVehicleType = p_pJson->GetNodeValue(p_strPrefix+"/vehicle_type", "");
				m_strVehicleNo = p_pJson->GetNodeValue(p_strPrefix+"/vehicle_no", "");
				int l_iCount = p_pJson->GetCount(p_strPrefix + "/vehicle_no");
				for (int i = 0; i < l_iCount; i++) {
					std::string l_strPrefixPath(p_strPrefix + "/vehicle_no/" + std::to_string(i));
					m_vecVehicleNo.push_back(p_pJson->GetNodeValue(l_strPrefixPath, ""));
				}
				m_strIsHazardousVehicle = p_pJson->GetNodeValue(p_strPrefix + "/is_hazardous_vehicle", ""); //����
				if (m_strIsHazardousVehicle.empty())
				{
					m_strIsHazardousVehicle = "0";
				}
				m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix+"/create_time", "");
				m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix+"/update_time", "");
				m_strTitle = p_pJson->GetNodeValue(p_strPrefix+"/label", "");  //title����ֶ���
				m_strPrivacy = p_pJson->GetNodeValue(p_strPrefix+"/is_privacy", "");
				if (m_strPrivacy.empty())
				{
					m_strPrivacy = "0";
				}
				m_strReceiptSrvName = p_pJson->GetNodeValue(p_strPrefix+"/receipt_srv_name", "");  //����
				
				m_strAdminDeptName = p_pJson->GetNodeValue(p_strPrefix+"/admin_dept_name", "");
				m_strAdminDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/admin_dept_org_code", "");   //����
				m_strReceiptDeptName = p_pJson->GetNodeValue(p_strPrefix+"/receipt_dept_name", "");
				m_strReceiptDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/receipt_dept_org_code", "");   //����
	
				m_strDeleteFlag = p_pJson->GetNodeValue(p_strPrefix + "/is_delete", "0");   //����
				if (m_strDeleteFlag.empty())
				{
					m_strDeleteFlag = "0";
				}
				m_strIsInvalid = p_pJson->GetNodeValue(p_strPrefix+"/is_invalid", "0");   //����
				if (m_strIsInvalid.empty())
				{
					m_strIsInvalid = "0";
				}
				m_strBusinessState = p_pJson->GetNodeValue(p_strPrefix+"/business_status", "");  //����
				m_strSeatNo = p_pJson->GetNodeValue(p_strPrefix+"/receipt_seatno", "");   //seatno����
				m_strIsMerge = p_pJson->GetNodeValue(p_strPrefix+"/is_merge", "0");
				if (m_strIsMerge.empty())
				{
					m_strIsMerge = "0";
				}
				m_strCreateUser = p_pJson->GetNodeValue(p_strPrefix+"/create_user", "");
				m_strUpdateUser = p_pJson->GetNodeValue(p_strPrefix+"/update_user", "");
				m_strUpdateUserName = p_pJson->GetNodeValue(p_strPrefix + "/update_user_name", "");
				m_strUpdateUserDeptCode = p_pJson->GetNodeValue(p_strPrefix + "/update_user_dept_code", "");
				m_strUpdateUserDeptName = p_pJson->GetNodeValue(p_strPrefix + "/update_user_dept_name", "");
				m_strIsVisitor = p_pJson->GetNodeValue(p_strPrefix + "/is_visitor", "0");//is_visitor
				if (m_strIsVisitor.empty())
				{
					m_strIsVisitor = "0";
				}
				m_strIsFeedBack = p_pJson->GetNodeValue(p_strPrefix + "/is_feedback", "0");//is_feedback
				if (m_strIsFeedBack.empty())
				{
					m_strIsFeedBack = "0";
				}
				m_strIsSameForBackground = p_pJson->GetNodeValue(p_strPrefix + "/is_sameforbackground", "0");//
				if (m_strIsSameForBackground.empty())
				{
					m_strIsSameForBackground = "0";
				}
				m_strIsProcessFlagSynchronized = p_pJson->GetNodeValue(p_strPrefix + "/is_processflagsynchronized", "0");//	
				if (m_strIsProcessFlagSynchronized.empty())
				{
					m_strIsProcessFlagSynchronized = "0";
				}
				m_strJurisdictionalOrgcode = p_pJson->GetNodeValue(p_strPrefix + "/jurisdictional_orgcode", "");
				m_strJurisdictionalOrgname = p_pJson->GetNodeValue(p_strPrefix + "/jurisdictional_orgname", "");
				m_strJurisdictionalOrgidentifier = p_pJson->GetNodeValue(p_strPrefix + "/jurisdictional_orgidentifier", "");
				m_strHadPush = p_pJson->GetNodeValue(p_strPrefix + "/had_push", "0");
				if (m_strHadPush.empty())
				{
					m_strHadPush = "0";
				}
				m_strTransAlarmFlag = p_pJson->GetNodeValue(p_strPrefix + "/transalam", "");
				m_strIsOver = p_pJson->GetNodeValue(p_strPrefix + "/is_over", "0");
				if (m_strIsOver.empty())
				{
					m_strIsOver = "0";
				}

				m_strCreateTeminal = p_pJson->GetNodeValue(p_strPrefix + "/createTeminal", "");
				m_strUpdateTeminal = p_pJson->GetNodeValue(p_strPrefix + "/updateTeminal", "");
				m_strAlarmSourceType = p_pJson->GetNodeValue(p_strPrefix + "/alarm_source_type", "");
				m_strAlarmSourceId = p_pJson->GetNodeValue(p_strPrefix + "/alarm_source_id", "");
				m_strCentreDeptCode = p_pJson->GetNodeValue(p_strPrefix + "/centre_dept_code", "");
				m_strCentreRelationDeptCode = p_pJson->GetNodeValue(p_strPrefix + "/centre_relation_dept_code", "");
				m_strReceiptDeptDistrictName = p_pJson->GetNodeValue(p_strPrefix + "/receipt_dept_district_name", "");
				m_strAlarmSystemReceiptDeptCode = p_pJson->GetNodeValue(p_strPrefix + "/develop_dept_code", "");
				m_strAlarmSystemReceiptDeptName = p_pJson->GetNodeValue(p_strPrefix + "/develop_dept_name", "");
				m_strLinkedDispatchCode = p_pJson->GetNodeValue(p_strPrefix + "/linked_dispatch_code", "");
				m_strIsSigned = p_pJson->GetNodeValue(p_strPrefix + "/is_signed", "");
				m_strOverTime = p_pJson->GetNodeValue(p_strPrefix + "/over_time", "");
				m_strInitialFirstType = p_pJson->GetNodeValue(p_strPrefix + "/initial_first_type", "");
				m_strInitialSecondType = p_pJson->GetNodeValue(p_strPrefix + "/initial_second_type", "");
				m_strInitialThirdType = p_pJson->GetNodeValue(p_strPrefix + "/initial_third_type", "");
				m_strInitialFourthType = p_pJson->GetNodeValue(p_strPrefix + "/initial_fourth_type", "");
				m_strInitialAdminDeptCode = p_pJson->GetNodeValue(p_strPrefix + "/initial_admin_dept_code", "");
				m_strInitialAdminDeptName = p_pJson->GetNodeValue(p_strPrefix + "/initial_admin_dept_name", "");
				m_strInitialAdminDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "/initial_admin_dept_org_code", "");
				m_strInitialAddr = p_pJson->GetNodeValue(p_strPrefix + "/initial_addr", "");
				m_strReceiptDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "/receipt_dept_short_name", "");
				m_strAdminDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "/admin_dept_short_name", "");
				return true;
			}
			
			void ComJson(const std::string& p_strPrefix, JsonParser::IJsonPtr& p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return;
				}

				p_pJson->SetNodeValue(p_strPrefix+"/msg_source", m_strMsgSource);

				p_pJson->SetNodeValue(p_strPrefix+"/receipt_dept_district_code", m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_dept_code", m_strReceiptDeptCode);
				p_pJson->SetNodeValue(p_strPrefix+"/id", m_strID);
				p_pJson->SetNodeValue(p_strPrefix+"/merge_id", m_strMergeID);
				p_pJson->SetNodeValue(p_strPrefix + "/merge_type", m_strMergeType);
				p_pJson->SetNodeValue(p_strPrefix+"/called_no_type", m_strCalledNoType);
				p_pJson->SetNodeValue(p_strPrefix+"/source_type", m_strSourceType);

				p_pJson->SetNodeValue(p_strPrefix+"/handle_type", m_strHandleType);
				p_pJson->SetNodeValue(p_strPrefix+"/source_id", m_strSourceID);
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_code", m_strReceiptCode);
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_name", m_strReceiptName);
				p_pJson->SetNodeValue(p_strPrefix+"/receiving_time", m_strTime);  //time����ֶ���
				p_pJson->SetNodeValue(p_strPrefix+"/received_time", m_strReceivedTime);  //����
				p_pJson->SetNodeValue(p_strPrefix+"/first_submit_time", m_strFirstSubmitTime); //first_submit_time  �����Լ��ǵ�	
				p_pJson->SetNodeValue(p_strPrefix+"/caller_no", m_strCallerNo);
				p_pJson->SetNodeValue(p_strPrefix+"/caller_user_name", m_strCallerUserName); //����
				p_pJson->SetNodeValue(p_strPrefix+"/caller_addr", m_strCallerAddr);
				p_pJson->SetNodeValue(p_strPrefix+"/caller_name", m_strCallerName);
				p_pJson->SetNodeValue(p_strPrefix+"/caller_gender", m_strCallerGender);
				p_pJson->SetNodeValue(p_strPrefix+"/contact_no", m_strContactNo);
				p_pJson->SetNodeValue(p_strPrefix+"/caller_id_type", m_strCallerIDType);
				p_pJson->SetNodeValue(p_strPrefix+"/caller_id", m_strCallerID);
				p_pJson->SetNodeValue(p_strPrefix+"/alarm_addr", m_strAlarmAddr);  //����  ������ַ
				p_pJson->SetNodeValue(p_strPrefix+"/addr", m_strAddr);
				p_pJson->SetNodeValue(p_strPrefix+"/content", m_strContent);
				p_pJson->SetNodeValue(p_strPrefix+"/admin_dept_code", m_strAdminDeptCode);
				p_pJson->SetNodeValue(p_strPrefix+"/first_type", m_strFirstType);
				p_pJson->SetNodeValue(p_strPrefix+"/second_type", m_strSecondType);
				p_pJson->SetNodeValue(p_strPrefix+"/third_type", m_strThirdType);
				p_pJson->SetNodeValue(p_strPrefix+"/fourth_type", m_strFourthType);
				p_pJson->SetNodeValue(p_strPrefix+"/symbol_code", m_strSymbolCode);
				p_pJson->SetNodeValue(p_strPrefix+"/erpetrators_number", m_strErpetratorsNumber); //����  
				if (m_strIsArmed.empty())
				{
					m_strIsArmed = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_armed", m_strIsArmed);  //����  
				if (m_strIsHazardousSubstances.empty())
				{
					m_strIsHazardousSubstances = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_hazardous_substances", m_strIsHazardousSubstances);  //����  
				if (m_strIsExplosionOrLeakage.empty())
				{
					m_strIsExplosionOrLeakage = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_explosion_or_leakage", m_strIsExplosionOrLeakage); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/desc_of_trapped", m_strDescOfTrapped); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/desc_of_injured", m_strDescOfInjured); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/desc_of_dead", m_strDescOfDead); //����  
				if (m_strIsForeignLanguage.empty())
				{
					m_strIsForeignLanguage = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_foreign_language", m_strIsForeignLanguage); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/longitude", m_strLongitude);
				p_pJson->SetNodeValue(p_strPrefix+"/latitude", m_strLatitude);
				p_pJson->SetNodeValue(p_strPrefix+"/manual_longitude", m_strManualLongitude); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/manual_latitude", m_strManualLatitude);//����  
				p_pJson->SetNodeValue(p_strPrefix+"/remark", m_strRemark);
				p_pJson->SetNodeValue(p_strPrefix+"/level", m_strLevel);
				p_pJson->SetNodeValue(p_strPrefix+"/state", m_strState);
				p_pJson->SetNodeValue(p_strPrefix+"/emergency_rescue_level", m_strEmergencyRescueLevel); //����  
				p_pJson->SetNodeValue(p_strPrefix+"/vehicle_type", m_strVehicleType);
				p_pJson->SetNodeValue(p_strPrefix+"/vehicle_no", m_strVehicleNo);
				if (m_strIsHazardousVehicle.empty())
				{
					m_strIsHazardousVehicle = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_hazardous_vehicle", m_strIsHazardousVehicle); //����
				p_pJson->SetNodeValue(p_strPrefix+"/create_time", m_strCreateTime);
				p_pJson->SetNodeValue(p_strPrefix+"/update_time", m_strUpdateTime);
				p_pJson->SetNodeValue(p_strPrefix+"/label", m_strTitle);  //title����ֶ���
				if (m_strPrivacy.empty())
				{
					m_strPrivacy = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_privacy", m_strPrivacy);
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_srv_name", m_strReceiptSrvName);  //����
				p_pJson->SetNodeValue(p_strPrefix+"/admin_dept_name", m_strAdminDeptName);
				p_pJson->SetNodeValue(p_strPrefix+"/admin_dept_org_code", m_strAdminDeptOrgCode);   //����
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_dept_name", m_strReceiptDeptName);
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_dept_org_code", m_strReceiptDeptOrgCode);   //����
				if (m_strDeleteFlag.empty())
				{
					m_strDeleteFlag = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_delete", m_strDeleteFlag);//is_delete����Э���д���
				if (m_strIsInvalid.empty())
				{
					m_strIsInvalid = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_invalid", m_strIsInvalid);   //����
				p_pJson->SetNodeValue(p_strPrefix+"/business_status", m_strBusinessState);  //����
				p_pJson->SetNodeValue(p_strPrefix+"/receipt_seatno", m_strSeatNo);   //seatno����
				if (m_strIsMerge.empty())
				{
					m_strIsMerge = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_merge", m_strIsMerge);
				p_pJson->SetNodeValue(p_strPrefix+"/create_user", m_strCreateUser);
				p_pJson->SetNodeValue(p_strPrefix+"/update_user", m_strUpdateUser);
				p_pJson->SetNodeValue(p_strPrefix + "/update_user_name", m_strUpdateUserName);
				p_pJson->SetNodeValue(p_strPrefix + "/update_user_dept_name", m_strUpdateUserDeptName);
				if (m_strIsVisitor.empty())
				{
					m_strIsVisitor = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_visitor", m_strIsVisitor);
				if (m_strIsFeedBack.empty())
				{
					m_strIsFeedBack = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix+"/is_feedback", m_strIsFeedBack);
				if (m_strIsSameForBackground.empty())
				{
					m_strIsSameForBackground = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/is_sameforbackground", m_strIsSameForBackground);
				if (m_strIsProcessFlagSynchronized.empty())
				{
					m_strIsProcessFlagSynchronized = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/is_processflagsynchronized", m_strIsProcessFlagSynchronized);

				p_pJson->SetNodeValue(p_strPrefix + "/jurisdictional_orgcode", m_strJurisdictionalOrgcode);
				p_pJson->SetNodeValue(p_strPrefix + "/jurisdictional_orgname", m_strJurisdictionalOrgname);
				p_pJson->SetNodeValue(p_strPrefix + "/jurisdictional_orgidentifier", m_strJurisdictionalOrgidentifier);
				if (m_strHadPush.empty())
				{
					m_strHadPush = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/had_push", m_strHadPush);

				if (m_strIsOver.empty())
				{
					m_strIsOver = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/is_over", m_strIsOver);

				p_pJson->SetNodeValue(p_strPrefix + "/createTeminal", m_strCreateTeminal);
				p_pJson->SetNodeValue(p_strPrefix + "/updateTeminal", m_strUpdateTeminal);

				if (m_strMajorAlarmFlag.empty())
				{
					m_strMajorAlarmFlag = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/majoralarm_flag", m_strMajorAlarmFlag); 
				if (m_strAttentionAlarmFlag.empty())
				{
					m_strAttentionAlarmFlag = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/attentionalarm_flag", m_strAttentionAlarmFlag);
				if (m_strReceiverAlarmFlag.empty())
				{
					m_strReceiverAlarmFlag = "0";
				}
				p_pJson->SetNodeValue(p_strPrefix + "/receiveralarm_flag", m_strReceiverAlarmFlag);
				p_pJson->SetNodeValue(p_strPrefix + "/alarm_source_type", m_strAlarmSourceType);
				p_pJson->SetNodeValue(p_strPrefix + "/alarm_source_id", m_strAlarmSourceId);
				p_pJson->SetNodeValue(p_strPrefix + "/record_file_id", m_strRecordFileID);
				p_pJson->SetNodeValue(p_strPrefix + "/centre_dept_code", m_strCentreDeptCode);
				p_pJson->SetNodeValue(p_strPrefix + "/centre_relation_dept_code", m_strCentreRelationDeptCode);
				p_pJson->SetNodeValue(p_strPrefix + "/receipt_dept_district_name", m_strReceiptDeptDistrictName);
				p_pJson->SetNodeValue(p_strPrefix + "/develop_dept_code", m_strAlarmSystemReceiptDeptCode);
				p_pJson->SetNodeValue(p_strPrefix + "/develop_dept_name", m_strAlarmSystemReceiptDeptName);
				p_pJson->SetNodeValue(p_strPrefix + "/linked_dispatch_code", m_strLinkedDispatchCode);
				p_pJson->SetNodeValue(p_strPrefix + "/is_signed", m_strIsSigned);
				p_pJson->SetNodeValue(p_strPrefix + "/over_time", m_strOverTime);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_first_type", m_strInitialFirstType);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_second_type", m_strInitialSecondType);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_third_type", m_strInitialThirdType);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_fourth_type", m_strInitialFourthType);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_admin_dept_code", m_strInitialAdminDeptCode);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_admin_dept_name", m_strInitialAdminDeptName);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_admin_dept_org_code", m_strInitialAdminDeptOrgCode);
				p_pJson->SetNodeValue(p_strPrefix + "/initial_addr", m_strInitialAddr);
				p_pJson->SetNodeValue(p_strPrefix + "/receipt_dept_short_name", m_strReceiptDeptShortName);
				p_pJson->SetNodeValue(p_strPrefix + "/admin_dept_short_name", m_strAdminDeptShortName);
			}

			bool ParseAlarmRecord(DataBase::IResultSetPtr &l_pRSet)
			{
				if (NULL == l_pRSet.get())
				{
					return false;
				}
				m_strID = l_pRSet->GetValue("id");
				m_strMergeID = l_pRSet->GetValue("merge_id");
				m_strMergeType = l_pRSet->GetValue("merge_type");
				m_strSeatNo = l_pRSet->GetValue("receipt_seatno");
				m_strTitle = l_pRSet->GetValue("label");
				m_strContent = l_pRSet->GetValue("content");
				m_strTime = l_pRSet->GetValue("receiving_time");
				m_strReceivedTime = l_pRSet->GetValue("received_time");
				m_strAddr = l_pRSet->GetValue("addr");
				m_strLongitude = l_pRSet->GetValue("longitude");
				m_strLatitude = l_pRSet->GetValue("latitude");
				m_strState = l_pRSet->GetValue("state");
				m_strLevel = l_pRSet->GetValue("level");
				m_strSourceType = l_pRSet->GetValue("source_type");
				m_strSourceID = l_pRSet->GetValue("source_id");
				m_strHandleType = l_pRSet->GetValue("handle_type");
				m_strFirstType = l_pRSet->GetValue("first_type");
				m_strSecondType = l_pRSet->GetValue("second_type");
				m_strThirdType = l_pRSet->GetValue("third_type");
				m_strFourthType = l_pRSet->GetValue("fourth_type");
				m_strVehicleNo = l_pRSet->GetValue("vehicle_no");
				m_strVehicleType = l_pRSet->GetValue("vehicle_type");
				m_strSymbolCode = l_pRSet->GetValue("symbol_code");
				m_strCalledNoType = l_pRSet->GetValue("called_no_type");
				m_strCallerNo = l_pRSet->GetValue("caller_no");
				m_strCallerName = l_pRSet->GetValue("caller_name");
				m_strCallerAddr = l_pRSet->GetValue("caller_addr");
				m_strCallerID = l_pRSet->GetValue("caller_id");
				m_strCallerIDType = l_pRSet->GetValue("caller_id_type");
				m_strCallerGender = l_pRSet->GetValue("caller_gender");
				m_strContactNo = l_pRSet->GetValue("contact_no");
				m_strAdminDeptCode = l_pRSet->GetValue("admin_dept_code");
				m_strAdminDeptName = l_pRSet->GetValue("admin_dept_name");
				m_strReceiptDeptDistrictCode = l_pRSet->GetValue("receipt_dept_district_code");
				m_strReceiptDeptCode = l_pRSet->GetValue("receipt_dept_code");
				m_strReceiptDeptName = l_pRSet->GetValue("receipt_dept_name");
				m_strReceiptCode = l_pRSet->GetValue("receipt_code");
				m_strReceiptName = l_pRSet->GetValue("receipt_name");
				m_strCreateTime = l_pRSet->GetValue("create_time");
				m_strCreateUser = l_pRSet->GetValue("create_user");
				m_strUpdateTime = l_pRSet->GetValue("update_time");
				m_strUpdateUser = l_pRSet->GetValue("update_user");
				m_strPrivacy = l_pRSet->GetValue("is_privacy");
				if (m_strPrivacy.empty())
				{
					m_strPrivacy = "0";
				}
				m_strRemark = l_pRSet->GetValue("remark");
				m_strIsVisitor = l_pRSet->GetValue("is_visitor");
				m_strIsFeedBack = l_pRSet->GetValue("is_feedback");
				//����
				m_strAlarmAddr = l_pRSet->GetValue("alarm_addr");
				m_strCallerUserName = l_pRSet->GetValue("caller_user_name");
				m_strErpetratorsNumber = l_pRSet->GetValue("erpetrators_number");
				m_strIsArmed = l_pRSet->GetValue("is_armed");
				if (m_strIsArmed.empty())
				{
					m_strIsArmed = "0";
				}
				m_strIsHazardousSubstances = l_pRSet->GetValue("is_hazardous_substances");
				if (m_strIsHazardousSubstances.empty())
				{
					m_strIsHazardousSubstances = "0";
				}
				m_strIsExplosionOrLeakage = l_pRSet->GetValue("is_explosion_or_leakage");
				if (m_strIsExplosionOrLeakage.empty())
				{
					m_strIsExplosionOrLeakage = "0";
				}
				m_strDescOfTrapped = l_pRSet->GetValue("desc_of_trapped");
				m_strDescOfInjured = l_pRSet->GetValue("desc_of_injured");
				m_strDescOfDead = l_pRSet->GetValue("desc_of_dead");
				m_strIsForeignLanguage = l_pRSet->GetValue("is_foreign_language");
				if (m_strIsForeignLanguage.empty())
				{
					m_strIsForeignLanguage = "0";
				}
				m_strManualLongitude = l_pRSet->GetValue("manual_longitude");
				m_strManualLatitude = l_pRSet->GetValue("manual_latitude");
				m_strEmergencyRescueLevel = l_pRSet->GetValue("emergency_rescue_level");
				m_strIsHazardousVehicle = l_pRSet->GetValue("is_hazardous_vehicle");
				if (m_strIsHazardousVehicle.empty())
				{
					m_strIsHazardousVehicle = "0";
				}
				m_strReceiptSrvName = l_pRSet->GetValue("receipt_srv_name");
				m_strAdminDeptOrgCode = l_pRSet->GetValue("admin_dept_org_code");
				m_strReceiptDeptOrgCode = l_pRSet->GetValue("receipt_dept_org_code");
				m_strDeleteFlag = l_pRSet->GetValue("is_delete");
				if (m_strDeleteFlag.empty())
				{
					m_strDeleteFlag = "0";
				}

				m_strIsInvalid = l_pRSet->GetValue("is_invalid");
				if (m_strIsInvalid.empty())
				{
					m_strIsInvalid = "0";
				}

				m_strBusinessState = l_pRSet->GetValue("business_status");
				m_strIsMerge = l_pRSet->GetValue("is_merge");
				if (m_strIsMerge.empty())
				{
					m_strIsMerge = "0";
				}
				m_strFirstSubmitTime = l_pRSet->GetValue("first_submit_time");

				m_strIsSameForBackground = l_pRSet->GetValue("is_sameforbackground");
				if (m_strIsSameForBackground.empty())
				{
					m_strIsSameForBackground = "0";
				}
				m_strIsProcessFlagSynchronized = l_pRSet->GetValue("is_processflagsynchronized");
				if (m_strIsProcessFlagSynchronized.empty())
				{
					m_strIsProcessFlagSynchronized = "0";
				}

				m_strJurisdictionalOrgcode = l_pRSet->GetValue("jurisdictional_orgcode");
				m_strJurisdictionalOrgname = l_pRSet->GetValue("jurisdictional_orgname");
				m_strJurisdictionalOrgidentifier = l_pRSet->GetValue("jurisdictional_orgidentifier");
				m_strHadPush = l_pRSet->GetValue("had_push");
				if (m_strHadPush.empty())
				{
					m_strHadPush = "0";
				}
				m_strIsOver = l_pRSet->GetValue("is_over");
				if (m_strIsOver.empty())
				{
					m_strIsOver = "0";
				}

				m_strCreateTeminal = l_pRSet->GetValue("createteminal");
				m_strUpdateTeminal = l_pRSet->GetValue("updateteminal");
				m_strAlarmSourceType = l_pRSet->GetValue("alarm_source_type");
				m_strAlarmSourceId = l_pRSet->GetValue("alarm_source_id");
				m_strCentreDeptCode = l_pRSet->GetValue("centre_dept_code");
				m_strCentreRelationDeptCode = l_pRSet->GetValue("centre_relation_dept_code");
				m_strReceiptDeptDistrictName = l_pRSet->GetValue("receipt_dept_district_name");
				m_strAlarmSystemReceiptDeptCode = l_pRSet->GetValue("develop_dept_code");
				m_strAlarmSystemReceiptDeptName = l_pRSet->GetValue("develop_dept_name");
				m_strLinkedDispatchCode = l_pRSet->GetValue("linked_dispatch_code");
				m_strIsSigned = l_pRSet->GetValue("is_signed");
				m_strOverTime = l_pRSet->GetValue("over_time");
				m_strInitialFirstType = l_pRSet->GetValue("initial_first_type");
				m_strInitialSecondType = l_pRSet->GetValue("initial_second_type");
				m_strInitialThirdType = l_pRSet->GetValue("initial_third_type");
				m_strInitialFourthType = l_pRSet->GetValue("initial_fourth_type");
				m_strInitialAdminDeptCode = l_pRSet->GetValue("initial_admin_dept_code");
				m_strInitialAdminDeptName = l_pRSet->GetValue("initial_admin_dept_name");
				m_strInitialAdminDeptOrgCode = l_pRSet->GetValue("initial_admin_dept_org_code");
				m_strInitialAddr = l_pRSet->GetValue("initial_addr");
				m_strReceiptDeptShortName = l_pRSet->GetValue("receipt_dept_short_name");
				m_strAdminDeptShortName = l_pRSet->GetValue("admin_dept_short_name");
				return true;
			}

			bool ParseAlarmRecordEx(DataBase::IResultSetPtr& l_pRSet)
			{
				if (NULL == l_pRSet.get())
				{
					return false;
				}

				m_strID = l_pRSet->GetValue("jjdbh");
				m_strMergeID = l_pRSet->GetValue("glzjjdbh");
				m_strMergeType = l_pRSet->GetValue("gllx");
				m_strSeatNo = l_pRSet->GetValue("jjxwh");
				m_strTitle = l_pRSet->GetValue("jqbq");
				m_strContent = l_pRSet->GetValue("bjnr");
				m_strTime = l_pRSet->GetValue("bjsj");
				m_strReceivedTime = l_pRSet->GetValue("jjsj");
				m_strAddr = l_pRSet->GetValue("jqdz");
				m_strLongitude = l_pRSet->GetValue("bjrxzb");
				m_strLatitude = l_pRSet->GetValue("bjryzb");
				m_strState = l_pRSet->GetValue("jqclztdm");
				m_strLevel = l_pRSet->GetValue("jqdjdm");
				m_strSourceType = l_pRSet->GetValue("jqlyfs");
				m_strSourceID = l_pRSet->GetValue("jjlyh");
				m_strHandleType = l_pRSet->GetValue("lhlx");
				m_strFirstType = l_pRSet->GetValue("jqlbdm");
				m_strSecondType = l_pRSet->GetValue("jqlxdm");
				m_strThirdType = l_pRSet->GetValue("jqxldm");
				m_strFourthType = l_pRSet->GetValue("jqzldm");
				m_strVehicleNo = l_pRSet->GetValue("sjcph");
				m_strVehicleType = l_pRSet->GetValue("sjchpzldm");
				m_strSymbolCode = l_pRSet->GetValue("tzdbh");
				m_strCalledNoType = l_pRSet->GetValue("jjlx");
				m_strCallerNo = l_pRSet->GetValue("bjdh");
				m_strCallerName = l_pRSet->GetValue("bjrmc");
				m_strCallerAddr = l_pRSet->GetValue("bjdhyhdz");
				m_strCallerID = l_pRSet->GetValue("bjrzjhm");
				m_strCallerIDType = l_pRSet->GetValue("bjrzjdm");
				m_strCallerGender = l_pRSet->GetValue("bjrxbdm");
				m_strContactNo = l_pRSet->GetValue("lxdh");
				m_strAdminDeptCode = l_pRSet->GetValue("gxdwdm");
				m_strAdminDeptName = l_pRSet->GetValue("gxdwmc");
				m_strReceiptDeptDistrictCode = l_pRSet->GetValue("xzqhdm");
				m_strReceiptDeptCode = l_pRSet->GetValue("jjdwdm");
				m_strReceiptDeptName = l_pRSet->GetValue("jjdwmc");
				m_strReceiptCode = l_pRSet->GetValue("jjybh");
				m_strReceiptName = l_pRSet->GetValue("jjyxm");
				m_strCreateTime = l_pRSet->GetValue("cjsj");
				m_strCreateUser = l_pRSet->GetValue("cjry");
				m_strUpdateTime = l_pRSet->GetValue("gxsj");
				m_strUpdateUser = l_pRSet->GetValue("gxry");
				m_strPrivacy = l_pRSet->GetValue("sfbm");
				if (m_strPrivacy.empty())
				{
					m_strPrivacy = "0";
				}
				m_strRemark = l_pRSet->GetValue("bcjjnr");
				m_strIsVisitor = l_pRSet->GetValue("hfbs");
				m_strIsFeedBack = l_pRSet->GetValue("fkbs");
				//����
				m_strAlarmAddr = l_pRSet->GetValue("bjdz");
				m_strCallerUserName = l_pRSet->GetValue("bjdhyhm");
				m_strErpetratorsNumber = l_pRSet->GetValue("zars");
				m_strIsArmed = l_pRSet->GetValue("ywcwq");
				if (m_strIsArmed.empty())
				{
					m_strIsArmed = "0";
				}
				m_strIsHazardousSubstances = l_pRSet->GetValue("ywwxwz");
				if (m_strIsHazardousSubstances.empty())
				{
					m_strIsHazardousSubstances = "0";
				}
				m_strIsExplosionOrLeakage = l_pRSet->GetValue("ywbzxl");
				if (m_strIsExplosionOrLeakage.empty())
				{
					m_strIsExplosionOrLeakage = "0";
				}
				m_strDescOfTrapped = l_pRSet->GetValue("bkryqksm");
				m_strDescOfInjured = l_pRSet->GetValue("ssryqksm");
				m_strDescOfDead = l_pRSet->GetValue("swryqksm");
				m_strIsForeignLanguage = l_pRSet->GetValue("sfswybj");
				if (m_strIsForeignLanguage.empty())
				{
					m_strIsForeignLanguage = "0";
				}
				m_strManualLongitude = l_pRSet->GetValue("fxdwjd");
				m_strManualLatitude = l_pRSet->GetValue("fxdwwd");
				m_strEmergencyRescueLevel = l_pRSet->GetValue("yjjydjdm");
				m_strIsHazardousVehicle = l_pRSet->GetValue("sfswhcl");
				if (m_strIsHazardousVehicle.empty())
				{
					m_strIsHazardousVehicle = "0";
				}
				m_strReceiptSrvName = l_pRSet->GetValue("jjfs");
				m_strAdminDeptOrgCode = l_pRSet->GetValue("gxdwdmbs");
				m_strReceiptDeptOrgCode = l_pRSet->GetValue("jjdwdmbs");
				m_strDeleteFlag = l_pRSet->GetValue("scbs");
				if (m_strDeleteFlag.empty())
				{
					m_strDeleteFlag = "0";
				}

				m_strIsInvalid = l_pRSet->GetValue("wxbs");
				if (m_strIsInvalid.empty())
				{
					m_strIsInvalid = "0";
				}

				m_strBusinessState = l_pRSet->GetValue("dqywzt");
				m_strIsMerge = l_pRSet->GetValue("hbbs");
				if (m_strIsMerge.empty())
				{
					m_strIsMerge = "0";
				}
				m_strFirstSubmitTime = l_pRSet->GetValue("jjwcsj");

				m_strIsSameForBackground = l_pRSet->GetValue("ybjxxyz");
				if (m_strIsSameForBackground.empty())
				{
					m_strIsSameForBackground = "0";
				}
				m_strIsProcessFlagSynchronized = l_pRSet->GetValue("tbcjbs");
				if (m_strIsProcessFlagSynchronized.empty())
				{
					m_strIsProcessFlagSynchronized = "0";
				}

				m_strJurisdictionalOrgcode = l_pRSet->GetValue("tzhgxdwdm");
				m_strJurisdictionalOrgname = l_pRSet->GetValue("tzhgxdwmc");
				m_strJurisdictionalOrgidentifier = l_pRSet->GetValue("tzhgxdwdmbs");
				m_strHadPush = l_pRSet->GetValue("sftsjz");
				if (m_strHadPush.empty())
				{
					m_strHadPush = "0";
				}
				m_strIsOver = l_pRSet->GetValue("sfja");
				if (m_strIsOver.empty())
				{
					m_strIsOver = "0";
				}
				m_strReceiptDeptShortName = l_pRSet->GetValue("jjdwjc");
				m_strAdminDeptShortName = l_pRSet->GetValue("gxdwjc");
				m_strCreateTeminal = l_pRSet->GetValue("createTeminal");
				m_strUpdateTeminal = l_pRSet->GetValue("updateTeminal");

				return true;
			}



			public:
				std::string	m_strMsgSource;					//��Ϣ��Դ
				std::string	m_strID;						//����id
				std::string	m_strMergeID;					//����ϲ�id
				std::string	m_strMergeType;					//����ϲ� type
				std::string m_strSeatNo;					//�Ӿ���ϯ��
				std::string m_strTitle;						//�������
				std::string m_strContent;					//��������
				std::string m_strTime;						//����id����ʱ��			
				//std::string m_strActualOccurTime;			//����idʵ�ʷ���ʱ��
				
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
				//std::string m_strSymbolAddr;				//����id������ַ������ʾ���ַ
				//std::string m_strFireBuildingType;			//����id������ȼ�ս�������

				//std::string m_strEventType;					//�¼����ͣ����Ÿ���

				std::string m_strCalledNoType;				//����id���������ֵ�����
				//std::string m_strActualCalledNoType;		//����idʵ�ʱ��������ֵ�����

				std::string m_strCallerNo;					//����id�����˺���
				std::string m_strCallerName;				//����id����������
				std::string m_strCallerAddr;				//����id�����˵�ַ
				std::string m_strCallerID;					//����id���������֤
				std::string m_strCallerIDType;				//����id���������֤����
				std::string m_strCallerGender;				//����id�������Ա�
				//std::string m_strCallerAge;					//����id����������
				//std::string m_strCallerBirthday;			//����id�����˳���������

				std::string m_strContactNo;					//����id��ϵ�˺���
				//std::string m_strContactName;				//����id��ϵ������
				//std::string m_strContactAddr;				//����id��ϵ�˵�ַ
				//std::string m_strContactID;					//����id��ϵ�����֤
				//std::string m_strContactIDType;				//����id��ϵ�����֤����
				//std::string m_strContactGender;				//����id��ϵ���Ա�
				//std::string m_strContactAge;				//����id��ϵ������
				//std::string m_strContactBirthday;			//����id��ϵ�˳���������

				//std::string m_strAdminDeptDistrictCode;		//����id��Ͻ��λ��������
				std::string m_strAdminDeptCode;				//����id��Ͻ��λ����
				std::string m_strAdminDeptName;				//����id��Ͻ��λ����

				std::string m_strReceiptDeptDistrictCode;	//����id�Ӿ���λ��������
				std::string m_strReceiptDeptCode;			//����id�Ӿ���λ����
				std::string m_strReceiptDeptName;			//����id�Ӿ���λ����
				//std::string m_strLeaderCode;				//����idֵ���쵼����
				//std::string m_strLeaderName;				//����idֵ���쵼����
				std::string m_strReceiptCode;				//����id�Ӿ��˾���
				std::string m_strReceiptName;				//����id�Ӿ�������

				//std::string m_strDispatchSuggestion;		//�������
				std::string m_strIsMerge;					//�Ƿ�ϲ�

				//std::string m_strOperatorCode;				//������ID����Ϣ��ԴΪ�ⲿ��Ԫ��VCS�� ʱ�����ⲿ��Ԫ����
				//std::string m_strOperatorName;				//��������������Ϣ��ԴΪ�ⲿ��Ԫ��VCS�� ʱ�����ⲿ��Ԫ����

				//std::string m_strCityCode;                  //���б���

				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				std::string m_strUpdateUserName;
				std::string m_strUpdateUserDeptCode;
				std::string m_strUpdateUserDeptName;
				std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
				std::string m_strPrivacy;					//��˽����
				std::string m_strRemark;				    //������Ϣ
				std::string m_strDeleteFlag;                //ֻ�����ݿ���洢ʱʹ�ã�����json�д���
				std::string m_strReceivedTime;              //�Ӿ�ʱ�� received_time  2022/4/2
				std::string m_strCallerUserName;            //�����绰�û��� caller_user_name
				std::string m_strAlarmAddr;                 //	������ַ alarm_addr
				std::string m_strErpetratorsNumber;         //	�������� erpetrators_number
				std::string m_strIsArmed;                   //	���޳�����  is_armed
				std::string m_strIsHazardousSubstances;     //	����Σ������ is_hazardous_substances
				std::string m_strIsExplosionOrLeakage;      //	���ޱ�ը / й©   is_explosion_or_leakage
				std::string m_strDescOfTrapped;             //	������Ա���˵�� desc_of_trapped
				std::string m_strDescOfInjured;             //	������Ա���˵��  desc_of_injured
				std::string m_strDescOfDead;                //	������Ա���˵��  desc_of_dead
				std::string m_strIsForeignLanguage;          //	�Ƿ������ﱨ��  is_foreign_language
				std::string m_strManualLongitude;            //	����λX���� manual_longitude 
				std::string m_strManualLatitude;             //	����λY���� manual_latitude
				std::string m_strEmergencyRescueLevel;      //	Ӧ����Ԯ�ȼ�����  emergency_rescue_level
				std::string m_strIsHazardousVehicle;        //	�Ƿ���Σ������  is_hazardous_vehicle
				std::string m_strReceiptSrvName;            //  ������vcs�Խӡ�icc�Ӿ���mpa�Ӿ� receipt_srv_name
				std::string m_strBusinessState;                  //  ��ǰҵ��״̬ business_status

				std::string m_strAdminDeptOrgCode;        //��Ͻ��λ����
				std::string m_strReceiptDeptOrgCode;        //�Ӿ���λ����
				std::string m_strIsInvalid;                 //�Ƿ���Ч

				std::string m_strFirstSubmitTime;          //�Ӿ����ʱ��
				std::string m_strIsVisitor;                //�Ƿ��ѻط�0��δ�طã�1���ѻط�
				std::string m_strIsFeedBack;               //�Ƿ��ѷ���0��δ������1���ѷ���

				std::string m_strIsSameForBackground;               //�Ƿ��뱳����Ϣһ�� 0����һ�£�1��һ�� ybjxxyz
				std::string m_strIsProcessFlagSynchronized;               //ͬ��������ʶ       0��δͬ����1����ͬ�� tbcjbs

				std::string m_strJurisdictionalOrgcode;			//������Ĺ�Ͻ��λ����
				std::string m_strJurisdictionalOrgname;			//������Ĺ�Ͻ��λ����
				std::string m_strJurisdictionalOrgidentifier;	//������Ĺ�Ͻ��λ����
				std::string m_strHadPush;//�Ƿ����͵�����
				std::string m_strIsOver;//�Ƿ�᰸

				std::string m_strTransAlarmFlag;

				std::string m_strCreateTeminal;//�����ն˱�ʶ
				std::string m_strUpdateTeminal;//�����ն˱�ʶ

				std::string m_strAlarmSourceType;					// �Ӿ���Դ����
				std::string m_strAlarmSourceId;						// �Ӿ���Դ���
				std::string m_strCentreDeptCode;					// �����Ӿ�������
				std::string m_strCentreRelationDeptCode;			// �����������Ӿ�������
				std::string m_strReceiptDeptDistrictName;			// ������������
				std::string m_strAlarmSystemReceiptDeptCode;		// �Ӵ���ϵͳ���赥λ����
				std::string m_strAlarmSystemReceiptDeptName;		// �Ӵ���ϵͳ���赥λ����
				std::string m_strLinkedDispatchCode;				// ���������λ����
				std::string m_strIsSigned;							// �Ƿ���ǩ��
				std::string m_strOverTime;							// �᰸ʱ��
				std::string m_strInitialFirstType;					// ��ʼ����������
				std::string m_strInitialSecondType;					// ��ʼ�������ʹ���
				std::string m_strInitialThirdType;					// ��ʼ����ϸ�����
				std::string m_strInitialFourthType;					// ��ʼ�����������
				std::string m_strInitialAdminDeptCode;				// ��ʼ��Ͻ��λ����
				std::string m_strInitialAdminDeptName;				// ��ʼ��Ͻ��λ����
				std::string m_strInitialAdminDeptOrgCode;			// ��ʼ��Ͻ��λ����
				std::string m_strInitialAddr;						// ��ʼ�����ַ
				std::string m_strReceiptDeptShortName;				// �Ӿ���λ�����Ӧ���
				std::string m_strAdminDeptShortName;				// ��Ͻ��λ���

				std::string m_strMajorAlarmFlag;          // �ش���
				std::string m_strAttentionAlarmFlag;      // ��ע����
				std::string m_strReceiverAlarmFlag;       // �ƽ����ҵľ���
				std::string m_strHandleTypeCode;         // �������ʹ���  ΢�ű���ʹ��
				std::string m_strRecordFileID;			 //¼���ļ�ID
				std::vector<std::string> m_vecVehicleNo; // ��ų��ƺ�    ΢�ű���ʹ��
				
		};	
	}
}
