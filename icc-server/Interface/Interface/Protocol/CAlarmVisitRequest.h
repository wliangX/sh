#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmVisitRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strVisitorCode = p_pJson->GetNodeValue("/body/visitor_code", "");
				m_oBody.m_strVisitorName = p_pJson->GetNodeValue("/body/visitor_name", "");
				m_oBody.m_strSatisfaction = p_pJson->GetNodeValue("/body/satisfaction", "");
				m_oBody.m_strSuggest = p_pJson->GetNodeValue("/body/suggest", "");
				m_oBody.m_strResultContent = p_pJson->GetNodeValue("/body/result_content", "");
				m_oBody.m_strCallType = p_pJson->GetNodeValue("/body/call_type", "");
				m_oBody.m_strReason = p_pJson->GetNodeValue("/body/reason", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strType;								//1代表新增，2代表编辑
				std::string m_strID;								//回访单ID
				std::string m_strAlarmID;							//警情ID
				std::string m_strCallrefID;							//话务ID（回访电话的话务id）
				std::string m_strDeptCode;							//警情归属单位编码
				std::string m_strDeptName;							//警情归属单位名称
				std::string m_strCallerNo;							//报警人号码
				std::string m_strCallerName;						//报警人姓名
				std::string m_strTime;								//回访时间
				std::string m_strVisitorCode;						//回访人编码
				std::string m_strVisitorName;						//回访人姓名
				std::string m_strSatisfaction;						//满意度 0：满意，1：不满意，默认0
				std::string m_strSuggest;							//建议
				std::string m_strResultContent;						//处理结果内容
				std::string m_strCallType;							//话务类型
				std::string m_strReason;							//原因
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
