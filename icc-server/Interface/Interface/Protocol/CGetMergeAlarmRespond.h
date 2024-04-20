#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>
namespace ICC
{
	namespace PROTOCOL
	{		
        class CGetMergeAlarmRespond :
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
                for (unsigned int i = 0; i < m_oBody.m_vecAlarmData.size(); i++)
                {
                    std::string l_strAlarmPrefixPath("/body/data/" + std::to_string(i) + "/");
                  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_district_code", m_oBody.m_vecAlarmData.at(i).m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_code", m_oBody.m_vecAlarmData.at(i).m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", m_oBody.m_vecAlarmData.at(i).m_strID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "merge_id", m_oBody.m_vecAlarmData.at(i).m_strMergeID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "merge_type", m_oBody.m_vecAlarmData.at(i).m_strMergeType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "called_no_type", m_oBody.m_vecAlarmData.at(i).m_strCalledNoType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_type", m_oBody.m_vecAlarmData.at(i).m_strSourceType);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "handle_type", m_oBody.m_vecAlarmData.at(i).m_strHandleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "source_id", m_oBody.m_vecAlarmData.at(i).m_strSourceID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_code", m_oBody.m_vecAlarmData.at(i).m_strReceiptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_name", m_oBody.m_vecAlarmData.at(i).m_strReceiptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receiving_time", m_oBody.m_vecAlarmData.at(i).m_strTime);  //time变更字段名
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "received_time", m_oBody.m_vecAlarmData.at(i).m_strReceivedTime);  //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_submit_time", m_oBody.m_vecAlarmData.at(i).m_strFirstSubmitTime); 
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_no", m_oBody.m_vecAlarmData.at(i).m_strCallerNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_user_name", m_oBody.m_vecAlarmData.at(i).m_strCallerUserName); //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_addr", m_oBody.m_vecAlarmData.at(i).m_strCallerAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_name", m_oBody.m_vecAlarmData.at(i).m_strCallerName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_gender", m_oBody.m_vecAlarmData.at(i).m_strCallerGender);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "contact_no", m_oBody.m_vecAlarmData.at(i).m_strContactNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id_type", m_oBody.m_vecAlarmData.at(i).m_strCallerIDType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_id", m_oBody.m_vecAlarmData.at(i).m_strCallerID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "alarm_addr", m_oBody.m_vecAlarmData.at(i).m_strAlarmAddr);  //新增  报警地址
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", m_oBody.m_vecAlarmData.at(i).m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", m_oBody.m_vecAlarmData.at(i).m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_code", m_oBody.m_vecAlarmData.at(i).m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", m_oBody.m_vecAlarmData.at(i).m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", m_oBody.m_vecAlarmData.at(i).m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "third_type", m_oBody.m_vecAlarmData.at(i).m_strThirdType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "fourth_type", m_oBody.m_vecAlarmData.at(i).m_strFourthType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "symbol_code", m_oBody.m_vecAlarmData.at(i).m_strSymbolCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "erpetrators_number", m_oBody.m_vecAlarmData.at(i).m_strErpetratorsNumber); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_armed", m_oBody.m_vecAlarmData.at(i).m_strIsArmed);  //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_hazardous_substances", m_oBody.m_vecAlarmData.at(i).m_strIsHazardousSubstances);  //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_explosion_or_leakage", m_oBody.m_vecAlarmData.at(i).m_strIsExplosionOrLeakage); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "desc_of_trapped", m_oBody.m_vecAlarmData.at(i).m_strDescOfTrapped); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "desc_of_injured", m_oBody.m_vecAlarmData.at(i).m_strDescOfInjured); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "desc_of_dead", m_oBody.m_vecAlarmData.at(i).m_strDescOfDead); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_foreign_language", m_oBody.m_vecAlarmData.at(i).m_strIsForeignLanguage); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "longitude", m_oBody.m_vecAlarmData.at(i).m_strLongitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "latitude", m_oBody.m_vecAlarmData.at(i).m_strLatitude);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "manual_longitude", m_oBody.m_vecAlarmData.at(i).m_strManualLongitude); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "manual_latitude", m_oBody.m_vecAlarmData.at(i).m_strManualLatitude);//新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "remark", m_oBody.m_vecAlarmData.at(i).m_strRemark);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "level", m_oBody.m_vecAlarmData.at(i).m_strLevel);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", m_oBody.m_vecAlarmData.at(i).m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "emergency_rescue_level", m_oBody.m_vecAlarmData.at(i).m_strEmergencyRescueLevel); //新增  
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_type", m_oBody.m_vecAlarmData.at(i).m_strVehicleType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "vehicle_no", m_oBody.m_vecAlarmData.at(i).m_strVehicleNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_hazardous_vehicle", m_oBody.m_vecAlarmData.at(i).m_strIsHazardousVehicle); //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", m_oBody.m_vecAlarmData.at(i).m_strCreateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", m_oBody.m_vecAlarmData.at(i).m_strUpdateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "label", m_oBody.m_vecAlarmData.at(i).m_strTitle);  //title变更字段名
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_privacy", m_oBody.m_vecAlarmData.at(i).m_strPrivacy);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_srv_name", m_oBody.m_vecAlarmData.at(i).m_strReceiptSrvName);  //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_name", m_oBody.m_vecAlarmData.at(i).m_strAdminDeptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_org_code", m_oBody.m_vecAlarmData.at(i).m_strAdminDeptOrgCode);   //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_name", m_oBody.m_vecAlarmData.at(i).m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_org_code", m_oBody.m_vecAlarmData.at(i).m_strReceiptDeptOrgCode);   //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_delete", m_oBody.m_vecAlarmData.at(i).m_strDeleteFlag);//is_delete不在协议中传递
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_invalid", m_oBody.m_vecAlarmData.at(i).m_strIsInvalid);   //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "business_status", m_oBody.m_vecAlarmData.at(i).m_strBusinessState);  //新增
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_seatno", m_oBody.m_vecAlarmData.at(i).m_strSeatNo);   //seatno改名
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_merge", m_oBody.m_vecAlarmData.at(i).m_strIsMerge);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", m_oBody.m_vecAlarmData.at(i).m_strCreateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", m_oBody.m_vecAlarmData.at(i).m_strUpdateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_visitor", m_oBody.m_vecAlarmData.at(i).m_strIsVisitor);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_feedback", m_oBody.m_vecAlarmData.at(i).m_strIsFeedBack);

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "receipt_dept_short_name", m_oBody.m_vecAlarmData.at(i).m_strReceiptDeptShortName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "admin_dept_short_name", m_oBody.m_vecAlarmData.at(i).m_strAdminDeptShortName);
                }
                return p_pJson->ToString();
			}

		public:
		          
            class CBody
            {
            public:                
                std::string m_strCount;
                std::vector<CAlarmInfo> m_vecAlarmData;                
            };
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}