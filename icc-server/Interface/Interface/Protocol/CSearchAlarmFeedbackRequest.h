#pragma once 
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmFeedbackRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");

				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strProcessID = p_pJson->GetNodeValue("/body/process_id", "");

				m_oBody.m_strResultType = p_pJson->GetNodeValue("/body/result_type", "");
				m_oBody.m_strResultContent = p_pJson->GetNodeValue("/body/result_content", "");
				m_oBody.m_strLeaderInstruction = p_pJson->GetNodeValue("/body/leader_instruction", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");

				m_oBody.m_strFeedbackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
				m_oBody.m_strFeedbackDeptCodeRecursive = p_pJson->GetNodeValue("/body/feedback_dept_code_recursion", "");
				m_oBody.m_strFeedbackCode = p_pJson->GetNodeValue("/body/feedback_code", "");
				m_oBody.m_strFeedbackLeaderCode = p_pJson->GetNodeValue("/body/feedback_leader_code", "");				

				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptCodeRecursive = p_pJson->GetNodeValue("/body/process_dept_code_recursion", "");
				m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
				m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");

				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptCodeRecursive = p_pJson->GetNodeValue("/body/dispatch_dept_code_recursion", "");
				m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
				m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");

				m_oBody.m_strAlarmCalledNoType = p_pJson->GetNodeValue("/body/alarm_called_no_type", "");
				m_oBody.m_strAlarmFirstType = p_pJson->GetNodeValue("/body/alarm_first_type", "");
				m_oBody.m_strAlarmSecondType = p_pJson->GetNodeValue("/body/alarm_second_type", "");
				m_oBody.m_strAlarmThirdType = p_pJson->GetNodeValue("/body/alarm_third_type", "");
				m_oBody.m_strAlarmFourthType = p_pJson->GetNodeValue("/body/alarm_fourth_type", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strBeginTime;
				std::string m_strEndTime;
				
				std::string m_strID;
				std::string m_strAlarmID;
				std::string m_strProcessID;
				
				std::string m_strResultType;
				std::string m_strResultContent;				
				std::string m_strLeaderInstruction;

				std::string m_strState;

				std::string m_strFeedbackDeptCode;
				std::string m_strFeedbackDeptCodeRecursive;
				std::string m_strFeedbackCode;
				std::string m_strFeedbackLeaderCode;
				
				std::string m_strProcessDeptCode;
				std::string m_strProcessDeptCodeRecursive;
				std::string m_strProcessCode;
				std::string m_strProcessLeaderCode;

				std::string m_strDispatchDeptCode;
				std::string m_strDispatchDeptCodeRecursive;
				std::string m_strDispatchCode;
				std::string m_strDispatchLeaderCode;		
				
				std::string	m_strAlarmCalledNoType;				//警情报警号码类型（多值查询，逗号分隔）
				std::string	m_strAlarmFirstType;				//警情一级类型（多值查询，逗号分隔）
				std::string	m_strAlarmSecondType;				//警情二级类型（多值查询，逗号分隔）
				std::string	m_strAlarmThirdType;				//警情三级类型（多值查询，逗号分隔）
				std::string	m_strAlarmFourthType;				//警情四级类型（多值查询，逗号分隔)			
			};
			CBody m_oBody;
		};
	}
}