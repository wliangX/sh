#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebGetAlarmDetailRespond :
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
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				
				unsigned int l_uiAlarmIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				unsigned int l_uiAlarmLogIndex = 0;
				for (auto alarm : m_oBody.m_vecAlarm)
				{
					std::string l_strAlarmPrefixPath("/body/alarm/" + std::to_string(l_uiAlarmIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "id", alarm.m_strID);
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

					// 第l_uiAlarmIndex+1个接警单对应的处警单列表
					for (auto process : m_oBody.m_vecAlarmProcess.at(l_uiAlarmIndex))
					{
						std::string l_strPrefixPathProcess("/body/alarm_process/" + std::to_string(l_uiProcessIndex) + "/");
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "id", process.m_strID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "alarm_id", process.m_strAlarmID);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "state", process.m_strState);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_edit", process.m_strTimeEdit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_submit", process.m_strTimeSubmit);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_arrived", process.m_strTimeArrived);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_signed", process.m_strTimeSigned);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "time_feedback", process.m_strTimeFeedBack);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "is_need_feedback", process.m_strIsNeedFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_district_code", process.m_strDispatchDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_code", process.m_strDispatchDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_dept_name", process.m_strDispatchDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_code", process.m_strDispatchCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_name", process.m_strDispatchName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_code", process.m_strDispatchLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_name", process.m_strDispatchLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_suggestion", process.m_strDispatchSuggestion);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "dispatch_leader_instruction", process.m_strDispatchLeaderInstruction);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_district_code", process.m_strProcessDeptDistrictCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_code", process.m_strProcessDeptCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_dept_name", process.m_strProcessDeptName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_code", process.m_strProcessCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_name", process.m_strProcessName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_code", process.m_strProcessLeaderCode);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_name", process.m_strProcessLeaderName);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_feedback", process.m_strProcessFeedback);
						p_pJson->SetNodeValue(l_strPrefixPathProcess + "process_leader_instruction", process.m_strProcessLeaderInstruction);
						l_uiProcessIndex++;
					}
					
					l_uiAlarmIndex++;
				}
				for (auto alarmLog : m_oBody.m_vecAlarmLog)
				{
					std::string l_strAlarmPrefixPath("/body/alarm_log/" + std::to_string(l_uiAlarmLogIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "alarm_id", alarmLog.m_strAlarmID);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "code", alarmLog.m_strCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", alarmLog.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "resource_type", alarmLog.m_strResourceType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "resource_id", alarmLog.m_strResourceId);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_user", alarmLog.m_strCreateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "create_time", alarmLog.m_strCreateTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_user", alarmLog.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "update_time", alarmLog.m_strUpdateTime);

					l_uiAlarmLogIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CAlarm
			{
			public:
				std::string	m_strID;						//警情id
				std::string	m_strMergeID;					//警情合并id
				std::string m_strTitle;						//警情标题
				std::string m_strContent;					//警情内容
				std::string m_strTime;						//警情id报警时间			
				std::string m_strActualOccurTime;			//警情id实际发生时间
				std::string m_strAddr;						//警情id详细发生地址
				std::string m_strLongitude;					//警情id经度
				std::string m_strLatitude;					//警情id维度
				std::string m_strState;						//警情id状态
				std::string m_strLevel;						//警情id级别
				std::string m_strSourceType;				//警情id报警来源类型
				std::string m_strSourceID;					//警情id报警来源id
				std::string m_strHandleType;				//警情id处理类型
				std::string m_strFirstType;					//警情id一级类型
				std::string m_strSecondType;				//警情id二级类型
				std::string m_strThirdType;					//警情id三级类型
				std::string m_strFourthType;				//警情id四级类型
				std::string m_strVehicleNo;					//警情id交通类型报警车牌号
				std::string m_strVehicleType;				//警情id交通类型报警车类型
				std::string m_strSymbolCode;				//警情id发生地址宣传标示物编号
				std::string m_strSymbolAddr;				//警情id发生地址宣传标示物地址
				std::string m_strFireBuildingType;			//警情id火警类型燃烧建筑类型

				std::string m_strEventType;					//事件类型，逗号隔开

				std::string m_strCalledNoType;				//警情id报警号码字典类型
				std::string m_strActualCalledNoType;		//警情id实际报警号码字典类型

				std::string m_strCallerNo;					//警情id报警人号码
				std::string m_strCallerName;				//警情id报警人姓名
				std::string m_strCallerAddr;				//警情id报警人地址
				std::string m_strCallerID;					//警情id报警人身份证
				std::string m_strCallerIDType;				//警情id报警人身份证类型
				std::string m_strCallerGender;				//警情id报警人性别
				std::string m_strCallerAge;					//警情id报警人年龄
				std::string m_strCallerBirthday;			//警情id报警人出生年月日

				std::string m_strContactNo;					//警情id联系人号码
				std::string m_strContactName;				//警情id联系人姓名
				std::string m_strContactAddr;				//警情id联系人地址
				std::string m_strContactID;					//警情id联系人身份证
				std::string m_strContactIDType;				//警情id联系人身份证类型
				std::string m_strContactGender;				//警情id联系人性别
				std::string m_strContactAge;				//警情id联系人年龄
				std::string m_strContactBirthday;			//警情id联系人出生年月日

				std::string m_strAdminDeptDistrictCode;		//警情id管辖单位行政区划
				std::string m_strAdminDeptCode;				//警情id管辖单位编码
				std::string m_strAdminDeptName;				//警情id管辖单位姓名

				std::string m_strReceiptDeptDistrictCode;	//警情id接警单位行政区划
				std::string m_strReceiptDeptCode;			//警情id接警单位编码
				std::string m_strReceiptDeptName;			//警情id接警单位名称
				std::string m_strLeaderCode;				//警情id值班领导警号
				std::string m_strLeaderName;				//警情id值班领导姓名
				std::string m_strReceiptCode;				//警情id接警人警号
				std::string m_strReceiptName;				//警情id接警人姓名

				std::string m_strDispatchSuggestion;		//调派意见		
			};
			class CAlarmProcess
			{
			public:
				std::string m_strID;						//处警ID
				std::string m_strAlarmID;					//警情ID
				std::string m_strState;						//处警单状态
				std::string m_strTimeEdit;					//派警单填写时间
				std::string m_strTimeSubmit;				//派警单提交时间
				std::string m_strTimeArrived;				//派警单系统到达时间
				std::string m_strTimeSigned;				//处警单位签收时间
				std::string m_strTimeFeedBack;				//处警单位反馈时间
				std::string m_strIsNeedFeedback;			//是否需要反馈
				std::string m_strDispatchDeptDistrictCode;	//派警单位行政区划
				std::string m_strDispatchDeptCode;			//派警单位代码
				std::string m_strDispatchDeptName;			//派警单位名称
				std::string m_strDispatchCode;				//派警人警号
				std::string m_strDispatchName;				//派警人姓名
				std::string m_strDispatchLeaderCode;		//派警单位值班领导警号
				std::string m_strDispatchLeaderName;		//派警单位值班领导姓名
				std::string m_strDispatchSuggestion;		//派警单位派警意见
				std::string m_strDispatchLeaderInstruction;	//派警单位领导指示
				std::string m_strProcessDeptDistrictCode;	//处警单位行政区划
				std::string m_strProcessDeptCode;			//处警单位代码
				std::string m_strProcessDeptName;			//处警单位名称
				std::string m_strProcessCode;				//处警人警号
				std::string m_strProcessName;				//处警人姓名
				std::string m_strProcessLeaderCode;			//处警单位值班领导警号
				std::string m_strProcessLeaderName;			//处警单位值班领导姓名
				std::string m_strProcessFeedback;			//处警反馈
				std::string m_strProcessLeaderInstruction;	//处警单位领导指示		
			};	
			class CAlarmLog
			{
			public:
				std::string m_strAlarmID;					//警情ID
				std::string m_strCode;						//流水编码
				std::string m_strContent;					//警情流水参数列表
				std::string m_strResourceType;				//1.接警2.处警3.反馈4.备注5.IP电话录音6.pdt录音
				std::string m_strResourceId;				//警情流水资源id
				std::string m_strCreateUser;				//创建者
				std::string m_strCreateTime;				//创建时间
				std::string m_strUpdateUser;			    //更新者
				std::string m_strUpdateTime;	            //更新时间
			};
			class CBody
			{
			public:
				std::string m_strResult;	//0-成功，1-失败
				std::vector<CAlarm> m_vecAlarm;
				std::vector<std::vector<CAlarmProcess>> m_vecAlarmProcess;
				std::vector<CAlarmLog> m_vecAlarmLog;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}