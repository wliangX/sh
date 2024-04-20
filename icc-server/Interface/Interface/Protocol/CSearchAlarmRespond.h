#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmRespond :
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
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strTotalAlarmCount);
				
				unsigned int l_uiIndex = 0;
				for (CData &data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.m_oAlarm.ComJson(l_strPrefixPath, p_pJson);
					//p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMergeID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "title", data.m_strTitle);
					//p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
					//p_pJson->SetNodeValue(l_strPrefixPath + "time", data.m_strTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", data.m_strActualOccurTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "addr", data.m_strAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
					//p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
					//p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					//p_pJson->SetNodeValue(l_strPrefixPath + "level", data.m_strLevel);
					//p_pJson->SetNodeValue(l_strPrefixPath + "source_type", data.m_strSourceType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "source_id", data.m_strSourceID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", data.m_strVehicleNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", data.m_strVehicleType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", data.m_strSymbolCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", data.m_strSymbolAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", data.m_strFireBuildingType);

					///*p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", data.m_strIsInvolveForeign);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", data.m_strIsInvolvePolice);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", data.m_strIsInvolveGun);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", data.m_strIsInvolveKnife);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", data.m_strIsInvolveTerror);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", data.m_strIsInvolvePornography);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", data.m_strIsInvolveGamble);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", data.m_strIsInvolvePoison);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", data.m_strIsInvolveExt1);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", data.m_strIsInvolveExt2);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", data.m_strIsInvolveExt3);*/
					//p_pJson->SetNodeValue(l_strPrefixPath + "event_type", data.m_strEventType);

					//p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", data.m_strCalledNoType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", data.m_strActualCalledNoType);

					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", data.m_strCallerNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", data.m_strCallerName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", data.m_strCallerAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", data.m_strCallerIDType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", data.m_strCallerGender);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", data.m_strCallerAge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", data.m_strCallerBirthday);

					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", data.m_strContactNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", data.m_strContactName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", data.m_strContactAddr);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", data.m_strContactID);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", data.m_strContactIDType);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", data.m_strContactGender);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", data.m_strContactAge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", data.m_strContactBirthday);

					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", data.m_strAdminDeptDistrictCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", data.m_strAdminDeptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", data.m_strAdminDeptName);

					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", data.m_strReceiptDeptDistrictCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", data.m_strReceiptDeptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", data.m_strReceiptDeptName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", data.m_strLeaderCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", data.m_strLeaderName);
					//p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strSeatNo);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", data.m_strReceiptCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", data.m_strReceiptName);

					//p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", data.m_strIsMerge);
					//p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					//
					//p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					//p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					//p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					//p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", data.m_strPrivacy);
					//p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					p_pJson->SetNodeValue(l_strPrefixPath + "/acd", data.m_strAcd);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				//std::string	m_strID;						//警情id
				//std::string	m_strMergeID;					//警情合并id
				//std::string m_strTitle;						//警情标题
				//std::string m_strContent;					//警情内容
				//std::string m_strTime;						//警情报警时间			
				//std::string m_strActualOccurTime;			//警情实际发生时间
				//std::string m_strAddr;						//警情详细发生地址
				//std::string m_strLongitude;					//警情经度
				//std::string m_strLatitude;					//警情维度
				//std::string m_strState;						//警情状态
				//std::string m_strLevel;						//警情级别
				//std::string m_strSourceType;				//警情报警来源类型
				//std::string m_strSourceID;					//警情报警来源id
				//std::string m_strHandleType;				//警情处理类型
				//std::string m_strFirstType;					//警情一级类型
				//std::string m_strSecondType;				//警情二级类型
				//std::string m_strThirdType;					//警情三级类型
				//std::string m_strFourthType;				//警情四级类型

				//std::string m_strVehicleNo;					//警情交通类型报警车牌号
				//std::string m_strVehicleType;				//警情交通类型报警车类型

				//std::string m_strSymbolCode;				//警情发生地址宣传标示物编号
				//std::string m_strSymbolAddr;				//警情发生地址宣传标示物地址

				//std::string m_strFireBuildingType;			//警情火警类型燃烧建筑类型
				//std::string m_strEventType;					//事件类型

				//std::string m_strCalledNoType;				//警情报警号码字典类型
				//std::string m_strActualCalledNoType;		//警情实际报警号码字典类型

				//std::string m_strCallerNo;					//警情报警人号码
				//std::string m_strCallerName;				//警情报警人姓名
				//std::string m_strCallerAddr;				//警情报警人地址
				//std::string m_strCallerID;					//警情报警人身份证
				//std::string m_strCallerIDType;				//警情报警人身份证类型
				//std::string m_strCallerGender;				//警情报警人性别
				//std::string m_strCallerAge;					//警情报警人年龄
				//std::string m_strCallerBirthday;			//警情报警人出生年月日

				//std::string m_strContactNo;					//警情联系人号码
				//std::string m_strContactName;				//警情联系人姓名
				//std::string m_strContactAddr;				//警情联系人地址
				//std::string m_strContactID;					//警情联系人身份证
				//std::string m_strContactIDType;				//警情联系人身份证类型
				//std::string m_strContactGender;				//警情联系人性别
				//std::string m_strContactAge;				//警情联系人年龄
				//std::string m_strContactBirthday;			//警情联系人出生年月日

				//std::string m_strAdminDeptDistrictCode;		//警情管辖单位行政区划
				//std::string m_strAdminDeptCode;				//警情管辖单位编码
				//std::string m_strAdminDeptName;				//警情管辖单位姓名

				//std::string m_strReceiptDeptDistrictCode;	//警情接警单位行政区划
				//std::string m_strReceiptDeptCode;			//警情接警单位编码
				//std::string m_strReceiptDeptName;			//警情接警单位名称

				//std::string m_strLeaderCode;				//警情值班领导警号
				//std::string m_strLeaderName;				//警情值班领导姓名

				//std::string m_strSeatNo;					//接警人座席号
				//std::string m_strReceiptCode;				//警情接警人警号
				//std::string m_strReceiptName;				//警情接警人姓名
				//
				//std::string m_strIsMerge;					//是否合并警情标志
				//std::string m_strDispatchSuggestion;		//调派意见

				//std::string m_strCreateUser;				//创建人
				//std::string m_strCreateTime;				//创建时间
				//std::string m_strUpdateUser;				//修改人,取最后一次修改值
				//std::string m_strUpdateTime;				//修改时间,取最后一次修改值
				CAlarmInfo m_oAlarm;

				std::string m_strAcd;

				//std::string m_strCityCode;
				//std::string m_strPrivacy;					//隐私保护  true or false	
				//std::string m_strRemark;				    //补充信息
			};
			class CBody
			{
			public:
				std::string m_strCount;					//分包总数
				std::string m_strTotalAlarmCount;		//警情总数
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}