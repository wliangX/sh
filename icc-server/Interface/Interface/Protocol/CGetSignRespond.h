#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetSignRespond :
			public ISend
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
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					//std::string l_strAlarmPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strAlarmPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					alarm.ComJson(l_strAlarmPrefixPath, p_pJson);

					/*p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
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

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", alarm.m_strCreateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", alarm.m_strCreateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", alarm.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", alarm.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "is_privacy", alarm.m_strPrivacy);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "remark", alarm.m_strRemark);*/

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			//class CAlarm
			//{
			//public:
			//	std::string	m_strID;						//警情id
			//	std::string	m_strMergeID;					//警情合并id
			//	std::string m_strTitle;						//警情标题
			//	std::string m_strContent;					//警情内容
			//	std::string m_strTime;						//警情id报警时间			
			//	std::string m_strActualOccurTime;			//警情id实际发生时间
			//	std::string m_strAddr;						//警情id详细发生地址
			//	std::string m_strLongitude;					//警情id经度
			//	std::string m_strLatitude;					//警情id维度
			//	std::string m_strState;						//警情id状态
			//	std::string m_strLevel;						//警情id级别
			//	std::string m_strSourceType;				//警情id报警来源类型
			//	std::string m_strSourceID;					//警情id报警来源id
			//	std::string m_strHandleType;				//警情id处理类型
			//	std::string m_strFirstType;					//警情id一级类型
			//	std::string m_strSecondType;				//警情id二级类型
			//	std::string m_strThirdType;					//警情id三级类型
			//	std::string m_strFourthType;				//警情id四级类型
			//	std::string m_strVehicleNo;					//警情id交通类型报警车牌号
			//	std::string m_strVehicleType;				//警情id交通类型报警车类型
			//	std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
			//	std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
			//	std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

			//	std::string m_strEventType;					//事件类型，逗号隔开

			//	std::string m_strCalledNoType;				//警情id报警号码字典类型
			//	std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

			//	std::string m_strCallerNo;					//警情id报警人号码
			//	std::string m_strCallerName;				//警情id报警人姓名
			//	std::string m_strCallerAddr;				//警情id报警人地址
			//	std::string m_strCallerID;					//警情id报警人身份证
			//	std::string m_strCallerIDType;				//警情id报警人身份证类型
			//	std::string m_strCallerGender;				//警情id报警人性别
			//	std::string m_strCallerAge;					//警情id报警人年龄
			//	std::string m_strCallerBirthday;			//警情id报警人出生年月日

			//	std::string m_strContactNo;					//警情id联系人号码
			//	std::string m_strContactName;				//警情id联系人姓名
			//	std::string m_strContactAddr;				//警情id联系人地址
			//	std::string m_strContactID;					//警情id联系人身份证
			//	std::string m_strContactIDType;				//警情id联系人身份证类型
			//	std::string m_strContactGender;				//警情id联系人性别
			//	std::string m_strContactAge;				//警情id联系人年龄
			//	std::string m_strContactBirthday;			//警情id联系人出生年月日

			//	std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
			//	std::string m_strAdminDeptCode;				//警情id管辖单位编码
			//	std::string m_strAdminDeptName;				//警情id管辖单位姓名

			//	std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
			//	std::string m_strReceiptDeptCode;			//警情id接警单位编码
			//	std::string m_strReceiptDeptName;			//警情id接警单位名称
			//	std::string m_strLeaderCode;				//警情id值班领导警号
			//	std::string m_strLeaderName;				//警情id值班领导姓名
			//	std::string m_strReceiptCode;				//警情id接警人警号
			//	std::string m_strReceiptName;				//警情id接警人姓名


			//	std::string m_strDispatchSuggestion;		//调派意见

			//	std::string m_strCreateUser;				//创建人
			//	std::string m_strCreateTime;				//创建时间
			//	std::string m_strUpdateUser;				//修改人,取最后一次修改值
			//	std::string m_strUpdateTime;				//修改时间,取最后一次修改值
			//	std::string m_strPrivacy;				    //隐私保护 true or false
			//	std::string m_strRemark;				    //补充信息
			//};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CAlarmInfo> m_vecAlarm;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}