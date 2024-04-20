#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebAlarmRemarkSync :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				std::string l_strPrefixPath("/body/");
				m_oBody.m_oAlarmRemark.m_strSyncType = p_pJson->GetNodeValue(l_strPrefixPath + "sync_type", "");
				m_oBody.m_oAlarmRemark.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
				m_oBody.m_oAlarmRemark.m_strFeedbackCode = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_code", "");
				m_oBody.m_oAlarmRemark.m_strFeedbackName = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_name", "");
				m_oBody.m_oAlarmRemark.m_strFeedbackDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_dept_code", "");
				m_oBody.m_oAlarmRemark.m_strFeedbackDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_dept_name", "");
				m_oBody.m_oAlarmRemark.m_strFeedbackTime = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_time", "");
				m_oBody.m_oAlarmRemark.m_strContent = p_pJson->GetNodeValue(l_strPrefixPath + "content", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/alarm_remark/");

				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_oAlarmRemark.m_strSyncType);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_oAlarmRemark.m_strAlarmID);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_code", m_oBody.m_oAlarmRemark.m_strFeedbackCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_name", m_oBody.m_oAlarmRemark.m_strFeedbackName);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_code", m_oBody.m_oAlarmRemark.m_strFeedbackDeptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_dept_name", m_oBody.m_oAlarmRemark.m_strFeedbackDeptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_time", m_oBody.m_oAlarmRemark.m_strFeedbackTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_oAlarmRemark.m_strContent);

				unsigned int l_uiIndex = 0;
				for (auto processdept : m_oBody.m_vecProcessDept)
				{
					std::string l_strProcessDeptPrefixPath("/body/process_dept/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strProcessDeptPrefixPath + "dept_code", processdept.m_strDeptCode);
					p_pJson->SetNodeValue(l_strProcessDeptPrefixPath + "dept_name", processdept.m_strDeptName);
					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CAlarmRemark
			{
			public:
				std::string m_strSyncType;					//1：表示添加，2：表示更新
				std::string m_strAlarmID;					//警情ID
				std::string m_strFeedbackCode;				//反馈人编号
				std::string m_strFeedbackName;				//反馈人姓名
				std::string m_strFeedbackDeptCode;			//反馈单位编号
				std::string m_strFeedbackDeptName;			//反馈单位名称
				std::string m_strFeedbackTime;				//反馈时间	
				std::string m_strContent;					//备注内容
			};
			class CBody
			{
			public:
				CAlarmRemark m_oAlarmRemark;
				std::vector<CProcessDept> m_vecProcessDept;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}