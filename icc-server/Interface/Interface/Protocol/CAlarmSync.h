#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmSync :
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
				std::string l_strPrefixPath("/body/");
				//p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarmInfo.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				m_oBody.m_oAlarmInfo.ComJson("/body", p_pJson);
				//p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarmInfo.m_strMsgSource);

				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarmInfo.m_strReceiptDeptDistrictCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarmInfo.m_strReceiptDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarmInfo.m_strID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarmInfo.m_strMergeID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarmInfo.m_strCalledNoType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarmInfo.m_strSourceType);

				//p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarmInfo.m_strHandleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarmInfo.m_strSourceID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarmInfo.m_strReceiptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarmInfo.m_strReceiptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receiving_time", m_oBody.m_oAlarmInfo.m_strTime);  //time变更字段名
				//p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_oAlarmInfo.m_strReceivedTime);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "first_submit_time", m_oBody.m_oAlarmInfo.m_strFirstSubmitTime); //first_submit_time  服务自己记的	
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarmInfo.m_strCallerNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_user_name", m_oBody.m_oAlarmInfo.m_strCallerUserName); //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarmInfo.m_strCallerAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarmInfo.m_strCallerName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarmInfo.m_strCallerGender);
				//p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarmInfo.m_strContactNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarmInfo.m_strCallerIDType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarmInfo.m_strCallerID);
				//p_pJson->SetNodeValue(l_strPrefixPath + "alarm_addr", m_oBody.m_oAlarmInfo.m_strAlarmAddr);  //新增  报警地址
				//p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarmInfo.m_strAddr);
				//p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarmInfo.m_strContent);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarmInfo.m_strAdminDeptCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarmInfo.m_strFirstType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarmInfo.m_strSecondType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarmInfo.m_strThirdType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarmInfo.m_strFourthType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarmInfo.m_strSymbolCode);
				//p_pJson->SetNodeValue(l_strPrefixPath + "erpetrators_number", m_oBody.m_oAlarmInfo.m_strErpetratorsNumber); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_armed", m_oBody.m_oAlarmInfo.m_strIsArmed);  //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_substances", m_oBody.m_oAlarmInfo.m_strIsHazardousSubstances);  //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_explosion_or_leakage", m_oBody.m_oAlarmInfo.m_strIsExplosionOrLeakage); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_trapped", m_oBody.m_oAlarmInfo.m_strDescOfTrapped); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_injured", m_oBody.m_oAlarmInfo.m_strDescOfInjured); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "desc_of_dead", m_oBody.m_oAlarmInfo.m_strDescOfDead); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_foreign_language", m_oBody.m_oAlarmInfo.m_strIsForeignLanguage); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarmInfo.m_strLongitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarmInfo.m_strLatitude);
				//p_pJson->SetNodeValue(l_strPrefixPath + "manual_longitude", m_oBody.m_oAlarmInfo.m_strManualLongitude); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "manual_latitude", m_oBody.m_oAlarmInfo.m_strManualLatitude);//新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_oAlarmInfo.m_strRemark);
				//p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarmInfo.m_strLevel);
				//p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarmInfo.m_strState);
				//p_pJson->SetNodeValue(l_strPrefixPath + "emergency_rescue_level", m_oBody.m_oAlarmInfo.m_strEmergencyRescueLevel); //新增  
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarmInfo.m_strVehicleType);
				//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarmInfo.m_strVehicleNo);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_hazardous_vehicle", m_oBody.m_oAlarmInfo.m_strIsHazardousVehicle); //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarmInfo.m_strCreateTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarmInfo.m_strUpdateTime);
				//p_pJson->SetNodeValue(l_strPrefixPath + "label", m_oBody.m_oAlarmInfo.m_strTitle);  //title变更字段名
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", m_oBody.m_oAlarmInfo.m_strPrivacy);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_srv_name", m_oBody.m_oAlarmInfo.m_strReceiptSrvName);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarmInfo.m_strAdminDeptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_org_code", m_oBody.m_oAlarmInfo.m_strAdminDeptOrgCode);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarmInfo.m_strReceiptDeptName);
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_org_code", m_oBody.m_oAlarmInfo.m_strReceiptDeptOrgCode);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_delete", m_oBody.m_oAlarmInfo.m_strDeleteFlag);//is_delete不在协议中传递
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_invalid", m_oBody.m_oAlarmInfo.m_strIsInvalid);   //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "business_status", m_oBody.m_oAlarmInfo.m_strBusinessState);  //新增
				//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_seatno", m_oBody.m_oAlarmInfo.m_strSeatNo);   //seatno改名
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_oAlarmInfo.m_strIsMerge);
				//p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarmInfo.m_strCreateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarmInfo.m_strUpdateUser);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", m_oBody.m_oAlarmInfo.m_strIsVisitor);
				//p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", m_oBody.m_oAlarmInfo.m_strIsFeedBack);

				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				//m_oBody.m_oAlarmInfo.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_oAlarmInfo.ParseString("/body", p_pJson);
				//m_oBody.m_oAlarmInfo.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/receipt_dept_district_code", "");
				//m_oBody.m_oAlarmInfo.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				//m_oBody.m_oAlarmInfo.m_strID = p_pJson->GetNodeValue("/body/id", "");
				//m_oBody.m_oAlarmInfo.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				//m_oBody.m_oAlarmInfo.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				//m_oBody.m_oAlarmInfo.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");
				//m_oBody.m_oAlarmInfo.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				//m_oBody.m_oAlarmInfo.m_strSourceID = p_pJson->GetNodeValue("/body/source_id", "");
				//m_oBody.m_oAlarmInfo.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				//m_oBody.m_oAlarmInfo.m_strReceiptName = p_pJson->GetNodeValue("/body/receipt_name", "");
				//m_oBody.m_oAlarmInfo.m_strTime = p_pJson->GetNodeValue("/body/receiving_time", "");  //time变更字段名
				//m_oBody.m_oAlarmInfo.m_strReceivedTime = p_pJson->GetNodeValue("/body/received_time", "");  //新增
				//m_oBody.m_oAlarmInfo.m_strFirstSubmitTime = p_pJson->GetNodeValue("/body/first_submit_time", ""); 
				//m_oBody.m_oAlarmInfo.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				//m_oBody.m_oAlarmInfo.m_strCallerUserName = p_pJson->GetNodeValue("/body/caller_user_name", ""); //新增
				//m_oBody.m_oAlarmInfo.m_strCallerAddr = p_pJson->GetNodeValue("/body/caller_addr", "");
				//m_oBody.m_oAlarmInfo.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				//m_oBody.m_oAlarmInfo.m_strCallerGender = p_pJson->GetNodeValue("/body/caller_gender", "");
				//m_oBody.m_oAlarmInfo.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				//m_oBody.m_oAlarmInfo.m_strCallerIDType = p_pJson->GetNodeValue("/body/caller_id_type", "");
				//m_oBody.m_oAlarmInfo.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				//m_oBody.m_oAlarmInfo.m_strAlarmAddr = p_pJson->GetNodeValue("/body/alarm_addr", "");  //新增  报警地址
				//m_oBody.m_oAlarmInfo.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				//m_oBody.m_oAlarmInfo.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				//m_oBody.m_oAlarmInfo.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				//m_oBody.m_oAlarmInfo.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				//m_oBody.m_oAlarmInfo.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				//m_oBody.m_oAlarmInfo.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				//m_oBody.m_oAlarmInfo.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");
				//m_oBody.m_oAlarmInfo.m_strSymbolCode = p_pJson->GetNodeValue("/body/symbol_code", "");
				//m_oBody.m_oAlarmInfo.m_strErpetratorsNumber = p_pJson->GetNodeValue("/body/erpetrators_number", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strIsArmed = p_pJson->GetNodeValue("/body/is_armed", "");  //新增  
				//m_oBody.m_oAlarmInfo.m_strIsHazardousSubstances = p_pJson->GetNodeValue("/body/is_hazardous_substances", "");  //新增  
				//m_oBody.m_oAlarmInfo.m_strIsExplosionOrLeakage = p_pJson->GetNodeValue("/body/is_explosion_or_leakage", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strDescOfTrapped = p_pJson->GetNodeValue("/body/desc_of_trapped", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strDescOfInjured = p_pJson->GetNodeValue("/body/desc_of_injured", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strDescOfDead = p_pJson->GetNodeValue("/body/desc_of_dead", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strIsForeignLanguage = p_pJson->GetNodeValue("/body/is_foreign_language", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				//m_oBody.m_oAlarmInfo.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				//m_oBody.m_oAlarmInfo.m_strManualLongitude = p_pJson->GetNodeValue("/body/manual_longitude", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strManualLatitude = p_pJson->GetNodeValue("/body/manual_latitude", "");//新增  
				//m_oBody.m_oAlarmInfo.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				//m_oBody.m_oAlarmInfo.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				//m_oBody.m_oAlarmInfo.m_strState = p_pJson->GetNodeValue("/body/state", "");
				//m_oBody.m_oAlarmInfo.m_strEmergencyRescueLevel = p_pJson->GetNodeValue("/body/emergency_rescue_level", ""); //新增  
				//m_oBody.m_oAlarmInfo.m_strVehicleType = p_pJson->GetNodeValue("/body/vehicle_type", "");
				//m_oBody.m_oAlarmInfo.m_strVehicleNo = p_pJson->GetNodeValue("/body/vehicle_no", "");
				//m_oBody.m_oAlarmInfo.m_strIsHazardousVehicle = p_pJson->GetNodeValue("/body/is_hazardous_vehicle", ""); //新增
				//m_oBody.m_oAlarmInfo.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				//m_oBody.m_oAlarmInfo.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");
				//m_oBody.m_oAlarmInfo.m_strTitle = p_pJson->GetNodeValue("/body/label", "");  //title变更字段名
				//m_oBody.m_oAlarmInfo.m_strPrivacy = p_pJson->GetNodeValue("/body/is_privacy", "");
				//m_oBody.m_oAlarmInfo.m_strReceiptSrvName = p_pJson->GetNodeValue("/body/receipt_srv_name", "");  //新增
				//m_oBody.m_oAlarmInfo.m_strAdminDeptName = p_pJson->GetNodeValue("/body/admin_dept_name", "");
				//m_oBody.m_oAlarmInfo.m_strAdminDeptOrgCode = p_pJson->GetNodeValue("/body/admin_dept_org_code", "");   //新增
				//m_oBody.m_oAlarmInfo.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				//m_oBody.m_oAlarmInfo.m_strReceiptDeptOrgCode = p_pJson->GetNodeValue("/body/receipt_dept_org_code", "");   //新增
				//m_oBody.m_oAlarmInfo.m_strDeleteFlag = p_pJson->GetNodeValue("/body/is_delete", "");
				//m_oBody.m_oAlarmInfo.m_strIsInvalid = p_pJson->GetNodeValue("/body/is_invalid", "");   //新增
				//m_oBody.m_oAlarmInfo.m_strBusinessState = p_pJson->GetNodeValue("/body/business_status", "");  //新增
				//m_oBody.m_oAlarmInfo.m_strSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");   //seatno改名
				//m_oBody.m_oAlarmInfo.m_strIsMerge = p_pJson->GetNodeValue("/body/is_merge", "");
				//m_oBody.m_oAlarmInfo.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				//m_oBody.m_oAlarmInfo.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				//m_oBody.m_oAlarmInfo.m_strIsVisitor = p_pJson->GetNodeValue("/body/is_visitor", "");
				//m_oBody.m_oAlarmInfo.m_strIsFeedBack = p_pJson->GetNodeValue("/body/is_feedback", "");
                return true;
            }
		public:		
			class CBody
			{
			public:
				
				std::string	m_strSyncType;					//警情同步类型：1-添加，2-更新
				CAlarmInfo m_oAlarmInfo;    //警情消息体
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
		};		
	}
}