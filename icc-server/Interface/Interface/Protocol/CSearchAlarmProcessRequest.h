#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmProcessRequest :
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
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strDispatchDeptCode = p_pJson->GetNodeValue("/body/dispatch_dept_code", "");
				m_oBody.m_strDispatchDeptCodeRecursive = p_pJson->GetNodeValue("/body/dispatch_dept_code_recursion", "");
				m_oBody.m_strDispatchCode = p_pJson->GetNodeValue("/body/dispatch_code", "");
				m_oBody.m_strDispatchLeaderCode = p_pJson->GetNodeValue("/body/dispatch_leader_code", "");
				m_oBody.m_strDispatchSuggestion = p_pJson->GetNodeValue("/body/dispatch_suggestion", "");
				m_oBody.m_strDispatchLeaderInstruction = p_pJson->GetNodeValue("/body/dispatch_leader_instruction", "");				
				m_oBody.m_strProcessDeptCode = p_pJson->GetNodeValue("/body/process_dept_code", "");
				m_oBody.m_strProcessDeptCodeRecursive = p_pJson->GetNodeValue("/body/process_dept_code_recursion", "");
				m_oBody.m_strProcessCode = p_pJson->GetNodeValue("/body/process_code", "");
				m_oBody.m_strProcessLeaderCode = p_pJson->GetNodeValue("/body/process_leader_code", "");
				m_oBody.m_strProcessFeedback = p_pJson->GetNodeValue("/body/process_feedback", "");
				m_oBody.m_strProcessLeaderInstruction = p_pJson->GetNodeValue("/body/process_leader_instruction", "");
				m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				return true;
			}	

		private:
			void _InitKeys();

		public:

			class CBody
			{
			public:
				std::string	m_strPageSize;					//每页数量（不能为空）
				std::string	m_strPageIndex;					//第几页，1表示第一页（不能为空）
				std::string	m_strBeginTime;					//查询开始时间（不能为空）
				std::string	m_strEndTime;					//查询结束时间（不能为空）
				std::string m_strID;						//处警id（模糊查询）
				std::string m_strAlarmID;					//警情id（模糊查询）
				std::string m_strState;						//处警单状态（模糊查询）
				std::string m_strDispatchDeptCode;			//派警单位代码（递归查询）
				std::string m_strDispatchDeptCodeRecursive;	//0表示不递归查询，1表示递归查询
				std::string m_strDispatchCode;				//派警人警号（多值查询，逗号分隔）
				std::string m_strDispatchLeaderCode;		//派警单位值班领导警号（多值查询，逗号分隔）
				std::string m_strDispatchSuggestion;		//派警单位派警意见（模糊查询）
				std::string m_strDispatchLeaderInstruction;	//派警单位领导指示（模糊查询）
				std::string m_strProcessDeptCode;			//处警单位代码（递归查询）
				std::string m_strProcessDeptCodeRecursive;	//0表示不递归查询，1表示递归查询
				std::string m_strProcessCode;				//处警人警号（多值查询，逗号分隔）
				std::string m_strProcessLeaderCode;			//处警单位值班领导警号（多值查询，逗号分隔）
				std::string m_strProcessFeedback;			//处警反馈（模糊查询）
				std::string m_strProcessLeaderInstruction;	//处警单位领导指示（模糊查询)
				std::string m_strFirstType;
				std::string m_strSecondType;
				std::string m_strThirdType;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;

			std::vector<std::string> m_vecKeys;
			std::map<std::string, std::string> m_mapKeyValues;
		};
	}
}
