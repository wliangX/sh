#pragma once
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGetAlarmRemarkRespond :
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
                p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
                p_pJson->SetNodeValue("/body/feedback_code", m_oBody.m_strFeedBackCode);
                p_pJson->SetNodeValue("/body/feedback_name", m_oBody.m_strFeedBackName);
                p_pJson->SetNodeValue("/body/feedback_dept_code", m_oBody.m_strFeedBackDeptCode);
                p_pJson->SetNodeValue("/body/feedback_dept_name", m_oBody.m_strFeedBackDeptName);
                p_pJson->SetNodeValue("/body/feedback_time", m_oBody.m_strFeedBackTime);
                p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
                std::string m_strAlarmID;				//警情idid
                std::string m_strFeedBackCode;			//反馈人编号
                std::string m_strFeedBackName;			//反馈人姓名
                std::string m_strFeedBackDeptCode;		//反馈单位编号
                std::string m_strFeedBackDeptName;		//反馈单位名称
                std::string m_strFeedBackTime;			//反馈时间
                std::string m_strContent;				//备注内容
			};
			CBody m_oBody;
		};
	}
}
