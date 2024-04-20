#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmLogAlarmResRespond :
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
				int i = 0;
				for (auto iter = m_oBody.m_vData.begin(); iter != m_oBody.m_vData.end(); iter++)
				{

					std::string l_strPrefixPath("/body/data/" + std::to_string(i));

					p_pJson->SetNodeValue(l_strPrefixPath + "/guid", iter->m_strGuID);
					p_pJson->SetNodeValue(l_strPrefixPath + "/flag", iter->m_strFlag);
					iter->m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
					/*p_pJson->SetNodeValue(l_strPrefixPath + "id", iter->m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", iter->m_strMergeID);
					p_pJson->SetNodeValue(l_strPrefixPath + "title", iter->m_strTitle);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", iter->m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", iter->m_strTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", iter->m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "addr", iter->m_strAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "longitude", iter->m_strLongitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "latitude", iter->m_strLatitude);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", iter->m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "level", iter->m_strLevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_type", iter->m_strSourceType);
					p_pJson->SetNodeValue(l_strPrefixPath + "source_id", iter->m_strSourceID);
					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", iter->m_strHandleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", iter->m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", iter->m_strSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", iter->m_strThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", iter->m_strFourthType);
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", iter->m_strVehicleNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", iter->m_strVehicleType);
					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", iter->m_strSymbolCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", iter->m_strSymbolAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", iter->m_strFireBuildingType);

					p_pJson->SetNodeValue(l_strPrefixPath + "event_type", iter->m_strEventType);

					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", iter->m_strCalledNoType);
					p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", iter->m_strActualCalledNoType);

					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", iter->m_strCallerNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", iter->m_strCallerName);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", iter->m_strCallerAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", iter->m_strCallerID);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", iter->m_strCallerIDType);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", iter->m_strCallerGender);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", iter->m_strCallerAge);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", iter->m_strCallerBirthday);

					p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", iter->m_strContactNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", iter->m_strContactName);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", iter->m_strContactAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", iter->m_strContactID);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", iter->m_strContactIDType);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", iter->m_strContactGender);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", iter->m_strContactAge);
					p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", iter->m_strContactBirthday);

					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", iter->m_strAdminDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", iter->m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", iter->m_strAdminDeptName);

					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", iter->m_strReceiptDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", iter->m_strReceiptDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", iter->m_strReceiptDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", iter->m_strLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", iter->m_strLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", iter->m_strReceiptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", iter->m_strReceiptName);

					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", iter->m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "flag", iter->m_strFlag);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", iter->m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", iter->m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", iter->m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", iter->m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", iter->m_strPrivacy);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", iter->m_strRemark);*/
					i++;
				}
				return p_pJson->ToString();
			}
		public:
			class CData
			{
			public:
				std::string m_strGuID;	
				CAlarmInfo  m_oAlarm;
				//std::string	m_strID;						//警情id
				//std::string	m_strMergeID;					//警情合并id
				//std::string m_strTitle;						//警情标题
				//std::string m_strContent;					//警情内容
				//std::string m_strTime;						//警情id报警时间			
				//std::string m_strActualOccurTime;			//警情id实际发生时间
				//std::string m_strAddr;						//警情id详细发生地址
				//std::string m_strLongitude;					//警情id经度
				//std::string m_strLatitude;					//警情id维度
				//std::string m_strState;						//警情id状态
				//std::string m_strLevel;						//警情id级别
				//std::string m_strSourceType;				//警情id报警来源类型
				//std::string m_strSourceID;					//警情id报警来源id
				//std::string m_strHandleType;				//警情id处理类型
				//std::string m_strFirstType;					//警情id一级类型
				//std::string m_strSecondType;				//警情id二级类型
				//std::string m_strThirdType;					//警情id三级类型
				//std::string m_strFourthType;				//警情id四级类型
				//std::string m_strVehicleNo;					//警情id交通类型报警车牌号
				//std::string m_strVehicleType;				//警情id交通类型报警车类型
				//std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
				//std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
				//std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

				//std::string m_strEventType;					//事件类型，逗号隔开

				//std::string m_strCalledNoType;				//警情id报警号码字典类型
				//std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

				//std::string m_strCallerNo;					//警情id报警人号码
				//std::string m_strCallerName;				//警情id报警人姓名
				//std::string m_strCallerAddr;				//警情id报警人地址
				//std::string m_strCallerID;					//警情id报警人身份证
				//std::string m_strCallerIDType;				//警情id报警人身份证类型
				//std::string m_strCallerGender;				//警情id报警人性别
				//std::string m_strCallerAge;					//警情id报警人年龄
				//std::string m_strCallerBirthday;			//警情id报警人出生年月日

				//std::string m_strContactNo;					//警情id联系人号码
				//std::string m_strContactName;				//警情id联系人姓名
				//std::string m_strContactAddr;				//警情id联系人地址
				//std::string m_strContactID;					//警情id联系人身份证
				//std::string m_strContactIDType;				//警情id联系人身份证类型
				//std::string m_strContactGender;				//警情id联系人性别
				//std::string m_strContactAge;				//警情id联系人年龄
				//std::string m_strContactBirthday;			//警情id联系人出生年月日

				//std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
				//std::string m_strAdminDeptCode;				//警情id管辖单位编码
				//std::string m_strAdminDeptName;				//警情id管辖单位姓名

				//std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
				//std::string m_strReceiptDeptCode;			//警情id接警单位编码
				//std::string m_strReceiptDeptName;			//警情id接警单位名称
				//std::string m_strLeaderCode;				//警情id值班领导警号
				//std::string m_strLeaderName;				//警情id值班领导姓名
				//std::string m_strReceiptCode;				//警情id接警人警号
				//std::string m_strReceiptName;				//警情id接警人姓名

				//std::string m_strDispatchSuggestion;		//调派意见

				std::string m_strFlag;		

				//std::string m_strCreateUser;				//创建人
				//std::string m_strCreateTime;				//创建时间
				//std::string m_strUpdateUser;				//修改人,取最后一次修改值
				//std::string m_strUpdateTime;				//修改时间,取最后一次修改值	

				//std::string m_strPrivacy;				    //隐私保护 true or false
				//std::string m_strRemark;				    //补充信息
			};
			class CBody
			{
			public:
				std::vector<CData> m_vData;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
