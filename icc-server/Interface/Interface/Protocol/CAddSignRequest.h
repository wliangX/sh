#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddSignRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strTimeEdit = p_pJson->GetNodeValue("/body/time_edit", "");
				m_oBody.m_strTimeSubmit = p_pJson->GetNodeValue("/body/time_submit", "");
				m_oBody.m_strTimeArrived = p_pJson->GetNodeValue("/body/time_arrived", "");
				m_oBody.m_strTimeSigned = p_pJson->GetNodeValue("/body/time_signed", "");
				m_oBody.m_strTimeFeedBack = p_pJson->GetNodeValue("/body/time_feedback", "");
				m_oBody.m_strIsNeedFeedback = p_pJson->GetNodeValue("/body/is_need_feedback", "");
				m_oBody.m_strDispatchDeptDistrictCode = p_pJson->GetNodeValue("/body/dispatch_dept_district_code", "");
				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptName = p_pJson->GetNodeValue("/body/dispatch_dept_name", "");
				m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
				m_oBody.m_strDispatchName = p_pJson->GetNodeValue("/body/dispatch_name", "");
				m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");
				m_oBody.m_strDispatchLeaderName = p_pJson->GetNodeValue("/body/dispatch_leader_name", "");
				m_oBody.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_strDispatchLeaderInstruction = p_pJson->GetNodeValue("/body/dispatch_leader_instruction", "");
				m_oBody.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue("/body/process_dept_district_code", "");
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptName = p_pJson->GetNodeValue("/body/process_dept_name", "");
				m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
				m_oBody.m_strProcessName = p_pJson->GetNodeValue("/body/process_name", "");
				m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");
				m_oBody.m_strProcessLeaderName = p_pJson->GetNodeValue("/body/process_leader_name", "");
				m_oBody.m_strProcessFeedback = p_pJson->GetNodeValue("/body/process_feedback", "");
				m_oBody.m_strProcessLeaderInstruction = p_pJson->GetNodeValue("/body/process_leader_instruction", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strMsgSource;                 //触发者
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

				std::string m_strCreateUser;				//创建人
				std::string m_strCreateTime;				//创建时间
				std::string m_strUpdateUser;				//修改人,取最后一次修改值
				std::string m_strUpdateTime;				//修改时间,取最后一次修改值	
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
