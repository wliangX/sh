#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSimilarAlarm :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CAlarmInfo data : m_oBody.m_vecData)
				{
					//std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.ComJson(l_strPrefixPath, p_pJson);
//					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", data.m_strMergeID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "title", data.m_strTitle);
//					p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
//					p_pJson->SetNodeValue(l_strPrefixPath + "time", data.m_strTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", data.m_strActualOccurTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "addr", data.m_strAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "longitude", data.m_strLongitude);
//					p_pJson->SetNodeValue(l_strPrefixPath + "latitude", data.m_strLatitude);
//					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
//					p_pJson->SetNodeValue(l_strPrefixPath + "level", data.m_strLevel);
//					p_pJson->SetNodeValue(l_strPrefixPath + "source_type", data.m_strSourceType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "source_id", data.m_strSourceID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", data.m_strHandleType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_no", data.m_strVehicleNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "vehicle_type", data.m_strVehicleType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_code", data.m_strSymbolCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "symbol_addr", data.m_strSymbolAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "fire_building_type", data.m_strFireBuildingType);
//
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_foreign", data.m_strIsInvolveForeign);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_police", data.m_strIsInvolvePolice);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gun", data.m_strIsInvolveGun);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_knife", data.m_strIsInvolveKnife);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_terror", data.m_strIsInvolveTerror);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_pornography", data.m_strIsInvolvePornography);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_gamble", data.m_strIsInvolveGamble);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_poison", data.m_strIsInvolvePoison);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext1", data.m_strIsInvolveExt1);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext2", data.m_strIsInvolveExt2);
//// 					p_pJson->SetNodeValue(l_strPrefixPath + "is_involve_ext3", data.m_strIsInvolveExt3);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "event_type", data.m_strEventType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "called_no_type", data.m_strCalledNoType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "actual_called_no_type", data.m_strActualCalledNoType);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", data.m_strCallerNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", data.m_strCallerName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", data.m_strCallerAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id_type", data.m_strCallerIDType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", data.m_strCallerGender);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_age", data.m_strCallerAge);
//					p_pJson->SetNodeValue(l_strPrefixPath + "caller_birthday", data.m_strCallerBirthday);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", data.m_strContactNo);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", data.m_strContactName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_addr", data.m_strContactAddr);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id", data.m_strContactID);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_id_type", data.m_strContactIDType);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", data.m_strContactGender);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_age", data.m_strContactAge);
//					p_pJson->SetNodeValue(l_strPrefixPath + "contact_birthday", data.m_strContactBirthday);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", data.m_strAdminDeptDistrictCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", data.m_strAdminDeptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", data.m_strAdminDeptName);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_district_code", data.m_strReceiptDeptDistrictCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_code", data.m_strReceiptDeptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_dept_name", data.m_strReceiptDeptName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "leader_code", data.m_strLeaderCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "leader_name", data.m_strLeaderName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", data.m_strReceiptCode);
//					p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", data.m_strReceiptName);
//					p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strReceiptSeatno);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
//					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
//					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_merge", data.m_strIsMerge);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_feedback", data.m_strIsFeedBack);
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_visitor", data.m_strIsVisitor);
//
//					p_pJson->SetNodeValue(l_strPrefixPath + "is_privacy", data.m_strPrivacy);
//					p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
//			class CAlarm
//			{
//			public:
//				std::string	m_strID;						//警情id
//				std::string	m_strMergeID;					//警情合并id
//				std::string m_strTitle;						//警情标题
//				std::string m_strContent;					//警情内容
//				std::string m_strTime;						//警情id报警时间			
//				std::string m_strActualOccurTime;			//警情id实际发生时间
//				std::string m_strAddr;						//警情id详细发生地址
//				std::string m_strLongitude;					//警情id经度
//				std::string m_strLatitude;					//警情id维度
//				std::string m_strState;						//警情id状态
//				std::string m_strLevel;						//警情id级别
//				std::string m_strSourceType;				//警情id报警来源类型
//				std::string m_strSourceID;					//警情id报警来源id
//				std::string m_strHandleType;				//警情id处理类型
//				std::string m_strFirstType;					//警情id一级类型
//				std::string m_strSecondType;				//警情id二级类型
//				std::string m_strThirdType;					//警情id三级类型
//				std::string m_strFourthType;				//警情id四级类型
//				std::string m_strVehicleNo;					//警情id交通类型报警车牌号
//				std::string m_strVehicleType;				//警情id交通类型报警车类型
//				std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
//				std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
//				std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型
//
//// 				std::string m_strIsInvolveForeign;			//是否涉外
//// 				std::string m_strIsInvolvePolice;			//是否涉警
//// 				std::string m_strIsInvolveGun;				//是否涉枪
//// 				std::string m_strIsInvolveKnife;			//是否涉刀
//// 				std::string m_strIsInvolveTerror;			//是否涉恐
//// 				std::string m_strIsInvolvePornography;		//是否涉黄
//// 				std::string m_strIsInvolveGamble;			//是否涉赌
//// 				std::string m_strIsInvolvePoison;			//是否涉毒
//// 				std::string m_strIsInvolveExt1;				//扩展字段1
//// 				std::string m_strIsInvolveExt2;				//扩展字段2
//// 				std::string m_strIsInvolveExt3;				//扩展字段3
//				std::string m_strEventType;					//事件类型，逗号隔开
//
//				std::string m_strCalledNoType;				//警情id报警号码字典类型
//				std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型
//
//				std::string m_strCallerNo;					//警情id报警人号码
//				std::string m_strCallerName;				//警情id报警人姓名
//				std::string m_strCallerAddr;				//警情id报警人地址
//				std::string m_strCallerID;					//警情id报警人身份证
//				std::string m_strCallerIDType;				//警情id报警人身份证类型
//				std::string m_strCallerGender;				//警情id报警人性别
//				std::string m_strCallerAge;					//警情id报警人年龄
//				std::string m_strCallerBirthday;			//警情id报警人出生年月日
//
//				std::string m_strContactNo;					//警情id联系人号码
//				std::string m_strContactName;				//警情id联系人姓名
//				std::string m_strContactAddr;				//警情id联系人地址
//				std::string m_strContactID;					//警情id联系人身份证
//				std::string m_strContactIDType;				//警情id联系人身份证类型
//				std::string m_strContactGender;				//警情id联系人性别
//				std::string m_strContactAge;				//警情id联系人年龄
//				std::string m_strContactBirthday;			//警情id联系人出生年月日
//
//				std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
//				std::string m_strAdminDeptCode;				//警情id管辖单位编码
//				std::string m_strAdminDeptName;				//警情id管辖单位姓名
//
//				std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
//				std::string m_strReceiptDeptCode;			//警情id接警单位编码
//				std::string m_strReceiptDeptName;			//警情id接警单位名称
//				std::string m_strLeaderCode;				//警情id值班领导警号
//				std::string m_strLeaderName;				//警情id值班领导姓名
//				std::string m_strReceiptCode;				//警情id接警人警号
//				std::string m_strReceiptName;				//警情id接警人姓名
//				std::string m_strReceiptSeatno;				//接警坐席号
//
//				std::string m_strDispatchSuggestion;		//调派意见
//
//				std::string m_strCreateUser;				//创建人
//				std::string m_strCreateTime;				//创建时间
//				std::string m_strUpdateUser;				//修改人,取最后一次修改值
//				std::string m_strUpdateTime;				//修改时间,取最后一次修改值			
//				std::string m_strIsMerge;					//合并标志
//
//				std::string m_strIsFeedBack;				//是否已反馈0：未反馈，1：已反馈				
//				std::string m_strIsVisitor;					//是否已回访0：未回访，1：已回访	
//				std::string m_strPrivacy;					//隐私保护  true or false	
//				std::string m_strRemark;				    //补充信息
//			};

			class CBody
			{
			public:
				std::string m_strStartTime;		//搜索起始时间至当前时间，前端赋值
				std::string m_strPhoneNo;		//报警电话（或）
				std::string m_strContactNo;     //联系电话（或）
				std::string m_strAddr;			//地址
				std::string m_strSourceType;	//警情来源方式

				std::string m_strCount;
				std::vector<CAlarmInfo> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			
		};
	}
}
