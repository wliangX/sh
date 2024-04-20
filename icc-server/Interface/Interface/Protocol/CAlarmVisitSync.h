#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmVisitSync :
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

				p_pJson->SetNodeValue("/body/result/id", m_oBody.m_strID);
				p_pJson->SetNodeValue("/body/result/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/result/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/result/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/result/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/result/caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue("/body/result/caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue("/body/result/time", m_oBody.m_strTime);
				p_pJson->SetNodeValue("/body/result/visitor_code", m_oBody.m_strVisitorCode);
				p_pJson->SetNodeValue("/body/result/visitor_name", m_oBody.m_strVisitorName);
				p_pJson->SetNodeValue("/body/result/satisfaction", m_oBody.m_strSatisfaction);
				p_pJson->SetNodeValue("/body/result/suggest", m_oBody.m_strSuggest);
				p_pJson->SetNodeValue("/body/result/result_content", m_oBody.m_strResultContent);
				p_pJson->SetNodeValue("/body/result/call_type", m_oBody.m_strCallType);
				p_pJson->SetNodeValue("/body/result/reason", m_oBody.m_strReason);
				
				return p_pJson->ToString();
			}
           
		public:		
			class CBody
			{
			public:
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
			CHeader m_oHeader;			
			CBody	m_oBody;
		};		
	}
}