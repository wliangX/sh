#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateProcessRequest :
			public IRequest,public IRespond
		{
		public:

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");

				int l_iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i<l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CProcessData l_oData;
					l_oData.m_bIsNewProcess = false;
					l_oData.ParseString(l_strPrefixPath, p_pJson);
					/*
					l_oData.m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "msg_source", "");
					l_oData.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", "");
					l_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					l_oData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_code", "");
					l_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					l_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					l_oData.m_strRecordID = p_pJson->GetNodeValue(l_strPrefixPath + "record_id", "");
					l_oData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_suggestion", "");
					l_oData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_code", "");
					l_oData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "time_submit", "");
					l_oData.m_strTimeArrived = p_pJson->GetNodeValue(l_strPrefixPath + "time_arrived", "");
					l_oData.m_strTimeSigned = p_pJson->GetNodeValue(l_strPrefixPath + "time_signed", "");
					l_oData.m_strProcessName = p_pJson->GetNodeValue(l_strPrefixPath + "process_name", "");
					l_oData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_code", "");
					l_oData.m_strDispatchPersonnel = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_personnel", "");
					l_oData.m_strDispatchVehicles = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_vehicles", "");
					l_oData.m_strDispatchBoats = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_boats", "");
					l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					l_oData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					l_oData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_name", "");
					l_oData.m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", "");
					l_oData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_name", "");
					l_oData.m_strProcessDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_dept_org_code", "");
					l_oData.m_strProcessObjectType = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_type", "");
					l_oData.m_strProcessObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_name", "");
					l_oData.m_strProcessObjectCode = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_code", "");
					l_oData.m_strBusinessStatus = p_pJson->GetNodeValue(l_strPrefixPath + "business_status", "");
					l_oData.m_strSeatCode = p_pJson->GetNodeValue(l_strPrefixPath + "seat_code", "");
					l_oData.m_strCancelTime = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_time", "");
					l_oData.m_strCancelReason = p_pJson->GetNodeValue(l_strPrefixPath + "cancel_reason", "");
					l_oData.m_strIsAutoAssignJob = p_pJson->GetNodeValue(l_strPrefixPath + "is_auto_assign_job", "");
					l_oData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					l_oData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					l_oData.m_strOvertimeState = p_pJson->GetNodeValue(l_strPrefixPath + "overtime_state", "");

					l_oData.m_strUpdateType = p_pJson->GetNodeValue(l_strPrefixPath + "update_type", "");

					l_oData.m_strProcessObjectID = p_pJson->GetNodeValue(l_strPrefixPath + "process_object_id", "");
					l_oData.m_strTransfDeptOrjCode = p_pJson->GetNodeValue(l_strPrefixPath + "transfers_dept_org_code", "");
					l_oData.m_strIsOver = p_pJson->GetNodeValue(l_strPrefixPath + "is_over", "");
					l_oData.m_strOverRemark = p_pJson->GetNodeValue(l_strPrefixPath + "over_remark", "");
					l_oData.m_strParentID = p_pJson->GetNodeValue(l_strPrefixPath + "parent_id", "");
					l_oData.m_strGZLDM = p_pJson->GetNodeValue(l_strPrefixPath + "flow_code", "");
					*/
					m_oBody.m_vecData.push_back(l_oData);
				}
				l_iCount = p_pJson->GetCount("/body/linked_data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(i) + "/");
					CLinkedData l_oLinked;
					l_oLinked.m_bIsNewLinked = false;
					l_oLinked.ParseString(l_strPrefixPath, p_pJson);
					m_oBody.m_vecLinked.push_back(l_oLinked);
				}
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
				unsigned int l_uiIndex = 0;
				for (CProcessData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					
					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);
					if (!data.m_strUpdateType.empty())
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "update_type", data.m_strUpdateType);
					}

					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_district_code", data.m_strDispatchDeptDistrictCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_code", data.m_strDispatchDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_id", data.m_strRecordID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_suggestion", data.m_strDispatchSuggestion);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_code", data.m_strProcessDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_submit", data.m_strTimeSubmit);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_arrived", data.m_strTimeArrived);
					p_pJson->SetNodeValue(l_strPrefixPath + "time_signed", data.m_strTimeSigned);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_name", data.m_strProcessName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_code", data.m_strProcessCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_personnel", data.m_strDispatchPersonnel);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_vehicles", data.m_strDispatchVehicles);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_boats", data.m_strDispatchBoats);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_name", data.m_strDispatchDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_org_code", data.m_strDispatchDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_name", data.m_strProcessDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_org_code", data.m_strProcessDeptOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_type", data.m_strProcessObjectType);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_name", data.m_strProcessObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_code", data.m_strProcessObjectCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "business_status", data.m_strBusinessStatus);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_code", data.m_strSeatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_time", data.m_strCancelTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "cancel_reason", data.m_strCancelReason);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_auto_assign_job", data.m_strIsAutoAssignJob);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "overtime_state", data.m_strOvertimeState);

					p_pJson->SetNodeValue(l_strPrefixPath + "process_dept_short_name", data.m_strProcessDeptShortName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_dept_short_name", data.m_strDispatchDeptShortName);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_object_id", data.m_strProcessObjectID);
					p_pJson->SetNodeValue(l_strPrefixPath + "transfers_dept_org_code", data.m_strTransfDeptOrjCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_over", data.m_strIsOver);
					p_pJson->SetNodeValue(l_strPrefixPath + "over_remark", data.m_strOverRemark);
					p_pJson->SetNodeValue(l_strPrefixPath + "parent_id", data.m_strParentID);
					p_pJson->SetNodeValue(l_strPrefixPath + "flow_code", data.m_strGZLDM);

					p_pJson->SetNodeValue(l_strPrefixPath + "createTeminal", data.m_strCreateTeminal);
					p_pJson->SetNodeValue(l_strPrefixPath + "updateTeminal", data.m_strUpdateTeminal);

					p_pJson->SetNodeValue(l_strPrefixPath + "received_time", data.m_strReceivedTime);
					
					++l_uiIndex;
				}

				l_uiIndex = 0;
				for (CLinkedData data : m_oBody.m_vecLinked)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", data.m_strMsgSource);
					if (!data.m_strUpdateType.empty())
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "update_type", data.m_strUpdateType);
					}
					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", data.m_strLinkedOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", data.m_strLinkedOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", data.m_strLinkedOrgType);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "result", data.m_strResult);
					++l_uiIndex;
				}
				return p_pJson->ToString();
			}
		public:
			class CProcessData
			{
			public:
				CProcessData()
				{
					m_bIsNewProcess = false;
				}

			public:
				bool ParseString(const std::string& p_strPrefix, JsonParser::IJsonPtr& p_pJson)
				{
					m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix + "/msg_source", "");

					m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix + "msg_source", "");

					m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_district_code", "");
					m_strID = p_pJson->GetNodeValue(p_strPrefix + "id", "");
					m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix + "alarm_id", "");
					m_strDispatchDeptCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_code", "");
					m_strDispatchCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_code", "");
					m_strDispatchName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_name", "");
					m_strRecordID = p_pJson->GetNodeValue(p_strPrefix + "record_id", "");
					m_strDispatchSuggestion = p_pJson->GetNodeValue(p_strPrefix + "dispatch_suggestion", "");
					m_strProcessDeptCode = p_pJson->GetNodeValue(p_strPrefix + "process_dept_code", "");
					m_strTimeSubmit = p_pJson->GetNodeValue(p_strPrefix + "time_submit", "");
					m_strTimeArrived = p_pJson->GetNodeValue(p_strPrefix + "time_arrived", "");
					m_strTimeSigned = p_pJson->GetNodeValue(p_strPrefix + "time_signed", "");
					m_strProcessName = p_pJson->GetNodeValue(p_strPrefix + "process_name", "");
					m_strProcessCode = p_pJson->GetNodeValue(p_strPrefix + "process_code", "");
					m_strDispatchPersonnel = p_pJson->GetNodeValue(p_strPrefix + "dispatch_personnel", "");
					m_strDispatchVehicles = p_pJson->GetNodeValue(p_strPrefix + "dispatch_vehicles", "");
					m_strDispatchBoats = p_pJson->GetNodeValue(p_strPrefix + "dispatch_boats", "");
					m_strState = p_pJson->GetNodeValue(p_strPrefix + "state", "");
					m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix + "create_time", "");
					m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix + "update_time", "");
					m_strDispatchDeptName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_name", "");
					m_strDispatchDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_org_code", "");
					m_strProcessDeptName = p_pJson->GetNodeValue(p_strPrefix + "process_dept_name", "");
					m_strProcessDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "process_dept_org_code", "");
					m_strProcessObjectType = p_pJson->GetNodeValue(p_strPrefix + "process_object_type", "");
					m_strProcessObjectName = p_pJson->GetNodeValue(p_strPrefix + "process_object_name", "");
					m_strProcessObjectCode = p_pJson->GetNodeValue(p_strPrefix + "process_object_code", "");
					m_strBusinessStatus = p_pJson->GetNodeValue(p_strPrefix + "business_status", "");
					m_strSeatCode = p_pJson->GetNodeValue(p_strPrefix + "seat_code", "");
					m_strCancelTime = p_pJson->GetNodeValue(p_strPrefix + "cancel_time", "");
					m_strCancelReason = p_pJson->GetNodeValue(p_strPrefix + "cancel_reason", "");
					m_strIsAutoAssignJob = p_pJson->GetNodeValue(p_strPrefix + "is_auto_assign_job", "");
					m_strCreateUser = p_pJson->GetNodeValue(p_strPrefix + "create_user", "");
					m_strUpdateUser = p_pJson->GetNodeValue(p_strPrefix + "update_user", "");
					m_strOvertimeState = p_pJson->GetNodeValue(p_strPrefix + "overtime_state", "");

					m_strUpdateType = p_pJson->GetNodeValue(p_strPrefix + "update_type", "");

					m_strProcessObjectID = p_pJson->GetNodeValue(p_strPrefix + "process_object_id", "");
					m_strTransfDeptOrjCode = p_pJson->GetNodeValue(p_strPrefix + "transfers_dept_org_code", "");
					m_strIsOver = p_pJson->GetNodeValue(p_strPrefix + "is_over", "");
					m_strOverRemark = p_pJson->GetNodeValue(p_strPrefix + "over_remark", "");
					m_strParentID = p_pJson->GetNodeValue(p_strPrefix + "parent_id", "");
					m_strGZLDM = p_pJson->GetNodeValue(p_strPrefix + "flow_code", "");

					m_strCentreProcessDeptCode = p_pJson->GetNodeValue(p_strPrefix + "centre_process_dept_code", "");
					m_strCentreAlarmDeptCode = p_pJson->GetNodeValue(p_strPrefix + "centre_alarm_dept_code", "");
					m_strDispatchDeptDistrictName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_district_name", "");
					m_strLinkedDispatchCode = p_pJson->GetNodeValue(p_strPrefix + "linked_dispatch_code", "");
					m_strOverTime = p_pJson->GetNodeValue(p_strPrefix + "over_time", "");
					m_strFinishedTimeoutState = p_pJson->GetNodeValue(p_strPrefix + "finished_timeout_state", "");
					m_strPoliceType = p_pJson->GetNodeValue(p_strPrefix + "police_type", "");
					m_strProcessDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "process_dept_short_name", "");
					m_strDispatchDeptShortName = p_pJson->GetNodeValue(p_strPrefix + "dispatch_dept_short_name", "");
					m_strCreateTeminal = p_pJson->GetNodeValue(p_strPrefix + "createTeminal", "");
					m_strUpdateTeminal = p_pJson->GetNodeValue(p_strPrefix + "updateTeminal", "");
					m_strReceivedTime  = p_pJson->GetNodeValue(p_strPrefix + "received_time", "");

					return true;
				};
			public:
				std::string m_strMsgSource;
				std::string m_strUpdateType;					//
				std::string m_strDelFlag;

				std::string m_strProcessFeedback;   //tzx 为了编译通过加的，后面要去掉

				bool m_bIsNewProcess;							//是否是新增的处警单，true代表是，false代表否
				std::string m_strDispatchDeptDistrictCode;		//	通常与对应接警单行政区划一致。
				std::string m_strID;							//	派警单编号，产生规则为行政区划代码+系统派警单编号，以免不同地区产生同样编号；唯一号，主索引键。
				std::string m_strAlarmID;						//	外键，关联接警单表中的接警单编号字段。
				std::string m_strDispatchDeptCode;				//	下达派警指令的单位机构代码
				std::string m_strDispatchCode;					//	派警员编号，用来标识派警员身份。
				std::string m_strDispatchName;					//	派警员姓名
				std::string m_strRecordID;						//	派警录音号，派警时由录音系统自动产生，接处警系统自动关联。
				std::string m_strDispatchSuggestion;			//	派警意见的详细文字描述
				std::string m_strProcessDeptCode;				//	出警单位机构代码
				std::string m_strTimeSubmit;					//	派警员下达派警单的时间
				std::string m_strTimeArrived;					//	出警单位的应用系统自动接收到的时间
				std::string m_strTimeSigned;					//	派单签收时间
				std::string m_strProcessName;					//	签收警单人员姓名
				std::string m_strProcessCode;					//	签收警单人员证件号码（身份证
				std::string m_strDispatchPersonnel;				//	记录出警人员姓名等详细信息
				std::string m_strDispatchVehicles;				//	记录出警车辆的详细信息
				std::string m_strDispatchBoats;					//	记录出警船艇的详细信息
				std::string m_strState;							//	记录该条警情目前的业务状态
				std::string m_strCreateTime;					//	记录创建的时间
				std::string m_strUpdateTime;					//	记录新增、变更的时间
				std::string m_strDispatchDeptName;				//	派警单位名称
				std::string m_strDispatchDeptOrgCode;			//	派警单位短码
				std::string m_strProcessDeptName;				//	处警单位名称
				std::string m_strProcessDeptOrgCode;			//	处警单位短码
				std::string m_strProcessObjectType;				//	处警对象类型（单位、警员、车辆）
				std::string m_strProcessObjectName;				//	处警对象名称
				std::string m_strProcessObjectCode;				//	处警对象编码（单位是单位编码，警员是警号，车辆是车辆编码）？是单位时跟上面出警单位机构代码是不是重复
				std::string m_strBusinessStatus;				//	vcs内部业务状态
				std::string m_strSeatCode;						//	负责人警号(坐席编号)
				std::string m_strCancelTime;					//	退单申请时间
				std::string m_strCancelReason;					//	取消原因
				std::string m_strIsAutoAssignJob;				//	是否自动调派任务创建
				std::string m_strCreateUser;					//	创建人
				std::string m_strUpdateUser;					//	修改人,取最后一次修改值修改时间,取最后一次修改值
				std::string m_strOvertimeState;					//

				std::string m_strTransfDeptOrjCode;				//	移交处警单位短码
				std::string m_strIsOver;						//	是否结案
				std::string m_strOverRemark;					//	结案说明
				std::string m_strParentID;						//	上级派警单编号
				std::string m_strGZLDM;							//	工作流代码

				std::string m_strProcessObjectID;

				std::string m_strCentreProcessDeptCode;				//	部级派警单编码
				std::string m_strCentreAlarmDeptCode;				//	部级接警单编码
				std::string m_strDispatchDeptDistrictName;			//	行政区划名称
				std::string m_strLinkedDispatchCode;				//	社会联动单位代码
				std::string m_strOverTime;							//	结案时间
				std::string m_strFinishedTimeoutState;				//	完毕超时状态
				std::string m_strPoliceType;						//	警员类型编码
				std::string m_strProcessDeptShortName;				//	出警单位代码对应简称
				std::string m_strDispatchDeptShortName;				//	派警单位简称
				std::string m_strCreateTeminal;					//创建终端标识
				std::string m_strUpdateTeminal;					//更新终端标识
				std::string m_strReceivedTime;					//接警时间
			};

		class CLinkedData
			{
			public:
				CLinkedData()
				{
					m_bIsNewLinked = false;
				}
			public:
				bool ParseString(const std::string& l_strPrefixPath, JsonParser::IJsonPtr& p_pJson)
				{
					m_strMsgSource = p_pJson->GetNodeValue(l_strPrefixPath + "/msg_source", "");
					m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					m_strLinkedOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_code", "");
					m_strLinkedOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_name", "");
					m_strLinkedOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_type", "");
					m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					m_strResult = p_pJson->GetNodeValue(l_strPrefixPath + "result", "");
					return true;
				};
			public:
				std::string m_strMsgSource;
				std::string m_strUpdateType;					//
				std::string m_strDelFlag;

			public:
				bool m_bIsNewLinked;					//是否是新增的处警单，true代表是，false代表否

				std::string m_strID;							//	派警单编号，产生规则为行政区划代码+系统派警单编号，以免不同地区产生同样编号；唯一号，主索引键。
				std::string m_strAlarmID;						//	外键，关联接警单表中的接警单编号字段。
				std::string m_strState;							//	处理状态
				std::string m_strLinkedOrgCode;					//	联动单位代码
				std::string m_strLinkedOrgName;					//	联动单位名称
				std::string m_strLinkedOrgType;					//	联动单位类型
				std::string m_strDispatchCode;					//  发起调派警员编号
				std::string m_strDispatchName;					//	发起调派警员编号				
				std::string m_strCreateUser;					//	创建人
				std::string m_strUpdateUser;					//	修改人,取最后一次修改值修改时间,取最后一次修改值
				std::string m_strCreateTime;					//	创建的时间
				std::string m_strUpdateTime;					//	新增、变更的时间
				std::string m_strResult;						//	处置结果内容

			};
			class CBody
			{
			public:
				std::string m_strMsgSource;                 //消息来源，主要需要区分VCS来的，不写警情流水表
				std::vector<CProcessData> m_vecData;
				std::vector<CLinkedData> m_vecLinked;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
