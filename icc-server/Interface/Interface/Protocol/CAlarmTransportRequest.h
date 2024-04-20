#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CTransPortAlarmRequest :
            public IRequest
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strCallRefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strTargetSeatNo = p_pJson->GetNodeValue("/body/target_seat_no", "");
				m_oBody.m_strTargetDeptCode = p_pJson->GetNodeValue("/body/target_dept_code", "");
				m_oBody.m_strTargetDeptName = p_pJson->GetNodeValue("/body/target_dept_name", "");
				m_oBody.m_strTargetDeptOrgCode = p_pJson->GetNodeValue("/body/target_dept_org_code", "");

				m_oBody.m_strTargetUserName = p_pJson->GetNodeValue("/body/target_user_name", "");
				m_oBody.m_strTargetUserCode = p_pJson->GetNodeValue("/body/target_user_code", "");

				m_oBody.m_strTransportReason = p_pJson->GetNodeValue("/body/transport_reason", "");
				m_oBody.m_strTransportType = p_pJson->GetNodeValue("/body/transport_type", "");
				m_oBody.m_strAssignFlag = p_pJson->GetNodeValue("/body/assignflag", "");

				m_oBody.m_oAlarm.ParseString("/body/alarm", p_pJson);
				//m_oBody.m_oAlarm.m_strMsgSource = p_pJson->GetNodeValue("/body/alarm/msg_source", "");
				//m_oBody.m_oAlarm.m_strID = p_pJson->GetNodeValue("/body/alarm/id", "");
				//m_oBody.m_oAlarm.m_strMergeID = p_pJson->GetNodeValue("/body/alarm/merge_id", "");
				//m_oBody.m_oAlarm.m_strSeatNo = p_pJson->GetNodeValue("/body/alarm/seatno", "");
				//m_oBody.m_oAlarm.m_strTitle = p_pJson->GetNodeValue("/body/alarm/title", "");
				//m_oBody.m_oAlarm.m_strContent = p_pJson->GetNodeValue("/body/alarm/content", "");
				//m_oBody.m_oAlarm.m_strTime = p_pJson->GetNodeValue("/body/alarm/time", "");
				////m_oBody.m_oAlarm.m_strActualOccurTime = p_pJson->GetNodeValue("/body/alarm/actual_occur_time", "");
				//m_oBody.m_oAlarm.m_strAddr = p_pJson->GetNodeValue("/body/alarm/addr", "");
				//m_oBody.m_oAlarm.m_strLongitude = p_pJson->GetNodeValue("/body/alarm/longitude", "");
				//m_oBody.m_oAlarm.m_strLatitude = p_pJson->GetNodeValue("/body/alarm/latitude", "");
				//m_oBody.m_oAlarm.m_strState = p_pJson->GetNodeValue("/body/alarm/state", "");
				//m_oBody.m_oAlarm.m_strLevel = p_pJson->GetNodeValue("/body/alarm/level", "");
				//m_oBody.m_oAlarm.m_strSourceType = p_pJson->GetNodeValue("/body/alarm/source_type", "");
				//m_oBody.m_oAlarm.m_strSourceID = p_pJson->GetNodeValue("/body/alarm/source_id", "");
				//m_oBody.m_oAlarm.m_strHandleType = p_pJson->GetNodeValue("/body/alarm/handle_type", "");
				//m_oBody.m_oAlarm.m_strFirstType = p_pJson->GetNodeValue("/body/alarm/first_type", "");
				//m_oBody.m_oAlarm.m_strSecondType = p_pJson->GetNodeValue("/body/alarm/second_type", "");
				//m_oBody.m_oAlarm.m_strThirdType = p_pJson->GetNodeValue("/body/alarm/third_type", "");
				//m_oBody.m_oAlarm.m_strFourthType = p_pJson->GetNodeValue("/body/alarm/fourth_type", "");
				//m_oBody.m_oAlarm.m_strVehicleNo = p_pJson->GetNodeValue("/body/alarm/vehicle_no", "");
				//m_oBody.m_oAlarm.m_strVehicleType = p_pJson->GetNodeValue("/body/alarm/vehicle_type", "");
				//m_oBody.m_oAlarm.m_strSymbolCode = p_pJson->GetNodeValue("/body/alarm/symbol_code", "");
				////m_oBody.m_oAlarm.m_strSymbolAddr = p_pJson->GetNodeValue("/body/alarm/symbol_addr", "");
				////m_oBody.m_oAlarm.m_strFireBuildingType = p_pJson->GetNodeValue("/body/alarm/fire_building_type", "");
				////m_oBody.m_oAlarm.m_strEventType = p_pJson->GetNodeValue("/body/alarm/event_type", "");
				//m_oBody.m_oAlarm.m_strCalledNoType = p_pJson->GetNodeValue("/body/alarm/called_no_type", "");
				//m_oBody.m_oAlarm.m_strActualCalledNoType = p_pJson->GetNodeValue("/body/alarm/actual_called_no_type", "");
				//m_oBody.m_oAlarm.m_strCallerNo = p_pJson->GetNodeValue("/body/alarm/caller_no", "");
				//m_oBody.m_oAlarm.m_strCallerName = p_pJson->GetNodeValue("/body/alarm/caller_name", "");
				//m_oBody.m_oAlarm.m_strCallerAddr = p_pJson->GetNodeValue("/body/alarm/caller_addr", "");
				//m_oBody.m_oAlarm.m_strCallerID = p_pJson->GetNodeValue("/body/alarm/caller_id", "");
				//m_oBody.m_oAlarm.m_strCallerIDType = p_pJson->GetNodeValue("/body/alarm/caller_id_type", "");
				//m_oBody.m_oAlarm.m_strCallerGender = p_pJson->GetNodeValue("/body/alarm/caller_gender", "");
				////m_oBody.m_oAlarm.m_strCallerAge = p_pJson->GetNodeValue("/body/alarm/caller_age", "");
				////m_oBody.m_oAlarm.m_strCallerBirthday = p_pJson->GetNodeValue("/body/alarm/caller_birthday", "");
				//m_oBody.m_oAlarm.m_strContactNo = p_pJson->GetNodeValue("/body/alarm/contact_no", "");
				////m_oBody.m_oAlarm.m_strContactName = p_pJson->GetNodeValue("/body/alarm/contact_name", "");
				////m_oBody.m_oAlarm.m_strContactAddr = p_pJson->GetNodeValue("/body/alarm/contact_addr", "");
				////m_oBody.m_oAlarm.m_strContactID = p_pJson->GetNodeValue("/body/alarm/contact_id", "");
				////m_oBody.m_oAlarm.m_strContactIDType = p_pJson->GetNodeValue("/body/alarm/contact_id_type", "");
				////m_oBody.m_oAlarm.m_strContactGender = p_pJson->GetNodeValue("/body/alarm/contact_gender", "");
				////m_oBody.m_oAlarm.m_strContactAge = p_pJson->GetNodeValue("/body/alarm/contact_age", "");
				////m_oBody.m_oAlarm.m_strContactBirthday = p_pJson->GetNodeValue("/body/alarm/contact_birthday", "");
				////m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_district_code", "");
				//m_oBody.m_oAlarm.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/alarm/admin_dept_code", "");
				//m_oBody.m_oAlarm.m_strAdminDeptName = p_pJson->GetNodeValue("/body/alarm/admin_dept_name", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_district_code", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/alarm/receipt_dept_code", "");
				//m_oBody.m_oAlarm.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/alarm/receipt_dept_name", "");
				////m_oBody.m_oAlarm.m_strLeaderCode = p_pJson->GetNodeValue("/body/alarm/leader_code", "");
				////m_oBody.m_oAlarm.m_strLeaderName = p_pJson->GetNodeValue("/body/alarm/leader_name", "");
				//m_oBody.m_oAlarm.m_strReceiptCode = p_pJson->GetNodeValue("/body/alarm/receipt_code", "");
				//m_oBody.m_oAlarm.m_strReceiptName = p_pJson->GetNodeValue("/body/alarm/receipt_name", "");
				////m_oBody.m_oAlarm.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/alarm/dispatch_suggestion", "");
				//m_oBody.m_oAlarm.m_strIsMerge = p_pJson->GetNodeValue("/body/alarm/is_merge", "");
				////m_oBody.m_oAlarm.m_strCityCode = p_pJson->GetNodeValue("/body/alarm/city_code", "");
				//m_oBody.m_oAlarm.m_strCreateUser = p_pJson->GetNodeValue("/body/alarm/create_user", "");
				//m_oBody.m_oAlarm.m_strCreateTime = p_pJson->GetNodeValue("/body/alarm/create_time", "");
				//m_oBody.m_oAlarm.m_strUpdateUser = p_pJson->GetNodeValue("/body/alarm/update_user", "");
				//m_oBody.m_oAlarm.m_strUpdateTime = p_pJson->GetNodeValue("/body/alarm/update_time", "");
                return true;
            }
			virtual std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefID);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCallerID);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/target_seat_no", m_oBody.m_strTargetSeatNo);

				p_pJson->SetNodeValue("/body/target_dept_code", m_oBody.m_strTargetDeptCode);
				p_pJson->SetNodeValue("/body/target_dept_name", m_oBody.m_strTargetDeptName);

				p_pJson->SetNodeValue("/body/target_user_name", m_oBody.m_strTargetUserName);
				p_pJson->SetNodeValue("/body/target_user_code", m_oBody.m_strTargetUserCode);

				p_pJson->SetNodeValue("/body/transport_reason", m_oBody.m_strTransportReason);
				p_pJson->SetNodeValue("/body/transport_type", m_oBody.m_strTransportType);
				p_pJson->SetNodeValue("/body/assignflag", m_oBody.m_strAssignFlag);

				std::string l_strPrefixPath("/body/alarm");

				m_oBody.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
				/*p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_oAlarm.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_oAlarm.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_oAlarm.m_strMergeID);
				p_pJson->SetNodeValue(l_strPrefixPath + "seatno", m_oBody.m_oAlarm.m_strSeatNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "title", m_oBody.m_oAlarm.m_strTitle);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarm.m_strContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_oAlarm.m_strTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_oAlarm.m_strActualOccurTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_oAlarm.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_oAlarm.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_oAlarm.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_oAlarm.m_strState);
				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_oAlarm.m_strLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_oAlarm.m_strSourceType);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_id", m_oBody.m_oAlarm.m_strSourceID);
				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_oAlarm.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_oAlarm.m_strFirstType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_oAlarm.m_strSecondType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_oAlarm.m_strThirdType);
				p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", m_oBody.m_oAlarm.m_strFourthType);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", m_oBody.m_oAlarm.m_strVehicleNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", m_oBody.m_oAlarm.m_strVehicleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", m_oBody.m_oAlarm.m_strSymbolCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", m_oBody.m_oAlarm.m_strSymbolAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", m_oBody.m_oAlarm.m_strFireBuildingType);
				p_pJson->SetNodeValue(l_strPrefixPath + "event_type", m_oBody.m_oAlarm.m_strEventType);
				p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", m_oBody.m_oAlarm.m_strCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", m_oBody.m_oAlarm.m_strActualCalledNoType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_oAlarm.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_oAlarm.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_oAlarm.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", m_oBody.m_oAlarm.m_strCallerID);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", m_oBody.m_oAlarm.m_strCallerIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_oAlarm.m_strCallerGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", m_oBody.m_oAlarm.m_strCallerAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", m_oBody.m_oAlarm.m_strCallerBirthday);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_oAlarm.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_oAlarm.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", m_oBody.m_oAlarm.m_strContactAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", m_oBody.m_oAlarm.m_strContactID);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", m_oBody.m_oAlarm.m_strContactIDType);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_oAlarm.m_strContactGender);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", m_oBody.m_oAlarm.m_strContactAge);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", m_oBody.m_oAlarm.m_strContactBirthday);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_oAlarm.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_oAlarm.m_strAdminDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_oAlarm.m_strAdminDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", m_oBody.m_oAlarm.m_strReceiptDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", m_oBody.m_oAlarm.m_strReceiptDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", m_oBody.m_oAlarm.m_strLeaderCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", m_oBody.m_oAlarm.m_strLeaderName);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_oAlarm.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_oAlarm.m_strReceiptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", m_oBody.m_oAlarm.m_strDispatchSuggestion);
				p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", m_oBody.m_oAlarm.m_strIsMerge);
				p_pJson->SetNodeValue(l_strPrefixPath + "city_code", m_oBody.m_oAlarm.m_strCityCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_oAlarm.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_oAlarm.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_oAlarm.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_oAlarm.m_strUpdateTime);*/
				
				std::string l_strInfo = p_pJson->ToString();
				return l_strInfo;
			}

        public:

            class CBody
            {
            public:
				std::string m_strAlarmID;
				std::string m_strCallRefID;
				std::string m_strCallerID;		//主叫号码
				std::string m_strUserCode;
				std::string m_strUserName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strSeatNo;
				std::string m_strTargetSeatNo;
				std::string m_strTargetUserName;
				std::string m_strTargetUserCode;
				std::string m_strTargetDeptCode;
				std::string m_strTargetDeptName;
				std::string m_strTransportReason;
				std::string m_strTransportType;			//转警类型（1：主动转警，2：话务接管,3:主动请求接管）
				std::string m_strAssignFlag;			//是否指定坐席（1：指定，0：不指定）
				PROTOCOL::CAlarmInfo m_oAlarm;
				std::string m_strTargetDeptOrgCode;		//受转警单位短码
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };
    }
}

