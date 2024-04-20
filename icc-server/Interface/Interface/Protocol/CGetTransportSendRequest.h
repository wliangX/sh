#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{		
		class CSendGetDeviceListRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
		};

		class CSendGetReadAgentRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				//p_pJson->SetNodeValue("/body/held_callref_id", m_oBody.m_strHeldCallrefID);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/acd", m_oBody.m_strTarget);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				//std::string m_strHeldCallrefID;	//被转话务ID
				std::string m_strSponsor;		//转移发起分机号
				std::string m_strTarget;		//部门ACD分组号
			};
			CBody m_oBody;
			CHeader m_oHeader;
		};

		class CSendTransferCallRequest :
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

				p_pJson->SetNodeValue("/body/held_callref_id", m_oBody.m_strHeldCallrefID);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				p_pJson->SetNodeValue("/body/target_device_type", m_oBody.m_strTargetDeviceType);
				p_pJson->SetNodeValue("/body/is_acd", m_oBody.m_strIsAcd);
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strHeldCallrefID;		//被转话务ID
				std::string m_strSponsor;			//转移发起分机号
				std::string m_strTarget;			//内部分机号或部门分组号
				std::string m_strTargetDeviceType;  //呼叫目标类型：inside：内部的座机号outside：外部的座机号或手机号wireless：无线 PDT 号groupcall：无线 PDT 组号
				std::string m_strIsAcd;				//转移目标类型: 分机号：0，ACD：1		

			};
			CBody m_oBody;
			CHeader m_oHeader;
		};

		class CAddAlarm
		{
		public:
			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarm.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_alarm.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				return true;
			}

			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/transalam", m_oBody.m_strTransAlarm);
				m_oBody.m_alarm.ComJson("/body", p_pJson);
				/*p_pJson->SetNodeValue("/body/id", m_oBody.m_alarm.m_strID);
				p_pJson->SetNodeValue("/body/merge_id", m_oBody.m_alarm.m_strMergeID);
				p_pJson->SetNodeValue("/body/seatno", m_oBody.m_alarm.m_strSeatNo);
				p_pJson->SetNodeValue("/body/title", m_oBody.m_alarm.m_strTitle);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_alarm.m_strContent);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_alarm.m_strTime);
				p_pJson->SetNodeValue("/body/actual_occur_time", m_oBody.m_alarm.m_strActualOccurTime);
				p_pJson->SetNodeValue("/body/addr", m_oBody.m_alarm.m_strAddr);
				p_pJson->SetNodeValue("/body/longitude", m_oBody.m_alarm.m_strLongitude);
				p_pJson->SetNodeValue("/body/latitude", m_oBody.m_alarm.m_strLatitude);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_alarm.m_strState);
				p_pJson->SetNodeValue("/body/level", m_oBody.m_alarm.m_strLevel);
				p_pJson->SetNodeValue("/body/source_type", m_oBody.m_alarm.m_strSourceType);
				p_pJson->SetNodeValue("/body/source_id", m_oBody.m_alarm.m_strSourceID);
				p_pJson->SetNodeValue("/body/handle_type", m_oBody.m_alarm.m_strHandleType);
				p_pJson->SetNodeValue("/body/first_type", m_oBody.m_alarm.m_strFirstType);
				p_pJson->SetNodeValue("/body/second_type", m_oBody.m_alarm.m_strSecondType);
				p_pJson->SetNodeValue("/body/third_type", m_oBody.m_alarm.m_strThirdType);
				p_pJson->SetNodeValue("/body/fourth_type", m_oBody.m_alarm.m_strFourthType);
				p_pJson->SetNodeValue("/body/vehicle_no", m_oBody.m_alarm.m_strVehicleNo);
				p_pJson->SetNodeValue("/body/vehicle_type", m_oBody.m_alarm.m_strVehicleType);
				p_pJson->SetNodeValue("/body/symbol_code", m_oBody.m_alarm.m_strSymbolCode);
				p_pJson->SetNodeValue("/body/symbol_addr", m_oBody.m_alarm.m_strSymbolAddr);
				p_pJson->SetNodeValue("/body/fire_building_type", m_oBody.m_alarm.m_strFireBuildingType);
				p_pJson->SetNodeValue("/body/dispatch_suggestion", m_oBody.m_alarm.m_strDispatchSuggestion);
				p_pJson->SetNodeValue("/body/event_type", m_oBody.m_alarm.m_strEventType);
				p_pJson->SetNodeValue("/body/called_no_type", m_oBody.m_alarm.m_strCalledNoType);
				p_pJson->SetNodeValue("/body/actual_called_no_type", m_oBody.m_alarm.m_strActualCalledNoType);
				p_pJson->SetNodeValue("/body/caller_no", m_oBody.m_alarm.m_strCallerNo);
				p_pJson->SetNodeValue("/body/caller_name", m_oBody.m_alarm.m_strCallerName);
				p_pJson->SetNodeValue("/body/caller_addr", m_oBody.m_alarm.m_strCallerAddr);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_alarm.m_strCallerID);
				p_pJson->SetNodeValue("/body/caller_id_type", m_oBody.m_alarm.m_strCallerIDType);
				p_pJson->SetNodeValue("/body/caller_gender", m_oBody.m_alarm.m_strCallerGender);
				p_pJson->SetNodeValue("/body/caller_age", m_oBody.m_alarm.m_strCallerAge);
				p_pJson->SetNodeValue("/body/caller_birthday", m_oBody.m_alarm.m_strCallerBirthday);
				p_pJson->SetNodeValue("/body/contact_no", m_oBody.m_alarm.m_strContactNo);
				p_pJson->SetNodeValue("/body/contact_name", m_oBody.m_alarm.m_strContactName);
				p_pJson->SetNodeValue("/body/contact_addr", m_oBody.m_alarm.m_strContactAddr);
				p_pJson->SetNodeValue("/body/contact_id", m_oBody.m_alarm.m_strContactID);
				p_pJson->SetNodeValue("/body/contact_id_type", m_oBody.m_alarm.m_strContactIDType);
				p_pJson->SetNodeValue("/body/contact_gender", m_oBody.m_alarm.m_strContactGender);
				p_pJson->SetNodeValue("/body/contact_age", m_oBody.m_alarm.m_strContactAge);
				p_pJson->SetNodeValue("/body/contact_birthday", m_oBody.m_alarm.m_strContactBirthday);
				p_pJson->SetNodeValue("/body/admin_dept_ district_code", m_oBody.m_alarm.m_strAdminDeptDistrictCode);
				p_pJson->SetNodeValue("/body/admin_dept_code", m_oBody.m_alarm.m_strAdminDeptCode);
				p_pJson->SetNodeValue("/body/admin_dept_name", m_oBody.m_alarm.m_strAdminDeptName);
				p_pJson->SetNodeValue("/body/receipt_dept_district_code", m_oBody.m_alarm.m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue("/body/receipt_dept_code", m_oBody.m_alarm.m_strReceiptDeptCode);
				p_pJson->SetNodeValue("/body/receipt_dept_name", m_oBody.m_alarm.m_strReceiptDeptName);
				p_pJson->SetNodeValue("/body/leader_code", m_oBody.m_alarm.m_strLeaderCode);
				p_pJson->SetNodeValue("/body/leader_name", m_oBody.m_alarm.m_strLeaderName);
				p_pJson->SetNodeValue("/body/receipt_code", m_oBody.m_alarm.m_strReceiptCode);
				p_pJson->SetNodeValue("/body/receipt_name", m_oBody.m_alarm.m_strReceiptName);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.m_alarm.m_strCreateUser);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.m_alarm.m_strCreateTime);
				p_pJson->SetNodeValue("/body/update_user", m_oBody.m_alarm.m_strUpdateUser);
				p_pJson->SetNodeValue("/body/update_time", m_oBody.m_alarm.m_strUpdateTime);
				p_pJson->SetNodeValue("/body/is_merge", m_oBody.m_alarm.m_strIsMerg);
				p_pJson->SetNodeValue("/body/city_code", m_oBody.m_alarm.m_strCityCode);
				p_pJson->SetNodeValue("/body/is_privacy", m_oBody.m_alarm.m_strPrivacy);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_alarm.m_strRemark);*/

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

			//	std::string m_strSeatNo;
			//	std::string m_strIsMerg;
			//	std::string m_strCityCode;

			//	std::string m_strPrivacy;					//隐私保护
			//	std::string m_strRemark;				    //补充信息

			//};

			class CBody
			{
			public:
				CAlarmInfo m_alarm;
				std::string m_strTransAlarm;
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};





		class CancelTransferRequest :
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

				//p_pJson->SetNodeValue("/body/active_callref_id", m_oBody.m_strActiveCallRefID);
				p_pJson->SetNodeValue("/body/held_callref_id", m_oBody.m_strHeldCallrefID);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strDevice);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				//std::string m_strActiveCallRefID;   //活动话务 ID
				std::string m_strHeldCallrefID;		//被保留话务 ID
				std::string m_strSponsor;			//发起恢复连接话务的分机号
				std::string m_strDevice;			//恢复连接话务的目标分机号
			};
			CBody m_oBody;
			CHeader m_oHeader;
		};

		class CAcdAgentState
		{
		public:
			bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strAcd = p_pJson->GetNodeValue("/body/acd", "");
				m_oBody.m_strLoginMode = p_pJson->GetNodeValue("/body/login_mode", "");
				m_oBody.m_strReadyState = p_pJson->GetNodeValue("/body/ready_state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				return true;
			}
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strAgent;		  //座机号
				std::string m_strAcd;
				std::string m_strLoginMode;   //login、logout
				std::string m_strReadyState;  //忙闲状态
				std::string m_strTime;
			};
			CBody m_oBody;
		};

	}
}