#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebAddAlarmRemarkRequest :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");

				std::string l_strPrefixPath("/body/alarm_remark/");
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

				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_oAlarmRemark.m_strAlarmID);
				p_pJson->SetNodeValue("/body/feedback_code", m_oBody.m_oAlarmRemark.m_strFeedbackCode);
				p_pJson->SetNodeValue("/body/feedback_name", m_oBody.m_oAlarmRemark.m_strFeedbackName);
				p_pJson->SetNodeValue("/body/feedback_dept_code", m_oBody.m_oAlarmRemark.m_strFeedbackDeptCode);
				p_pJson->SetNodeValue("/body/feedback_dept_name", m_oBody.m_oAlarmRemark.m_strFeedbackDeptName);
				p_pJson->SetNodeValue("/body/feedback_time", m_oBody.m_oAlarmRemark.m_strFeedbackTime);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_oAlarmRemark.m_strContent);

				return p_pJson->ToString();
			}

			std::string ToString4Test(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/token", m_oBody.m_strToken);
				p_pJson->SetNodeValue("/body/alarm_remark/alarm_id", m_oBody.m_oAlarmRemark.m_strAlarmID);
				p_pJson->SetNodeValue("/body/alarm_remark/feedback_code", m_oBody.m_oAlarmRemark.m_strFeedbackCode);
				p_pJson->SetNodeValue("/body/alarm_remark/feedback_name", m_oBody.m_oAlarmRemark.m_strFeedbackName);
				p_pJson->SetNodeValue("/body/alarm_remark/feedback_dept_code", m_oBody.m_oAlarmRemark.m_strFeedbackDeptCode);
				p_pJson->SetNodeValue("/body/alarm_remark/feedback_dept_name", m_oBody.m_oAlarmRemark.m_strFeedbackDeptName);
				p_pJson->SetNodeValue("/body/alarm_remark/feedback_time", m_oBody.m_oAlarmRemark.m_strFeedbackTime);
				p_pJson->SetNodeValue("/body/alarm_remark/content", m_oBody.m_oAlarmRemark.m_strContent);

				return p_pJson->ToString();
			}

		public:
			class CAlarmRemark
			{
			public:
				std::string m_strAlarmID;					//����ID
				std::string m_strFeedbackCode;				//�����˱��
				std::string m_strFeedbackName;				//����������
				std::string m_strFeedbackDeptCode;			//������λ���
				std::string m_strFeedbackDeptName;			//������λ����
				std::string m_strFeedbackTime;				//����ʱ��	
				std::string m_strContent;					//��ע����
			};
			class CBody
			{
			public:
				std::string m_strToken;						//��Ȩ��
				CAlarmRemark m_oAlarmRemark;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
