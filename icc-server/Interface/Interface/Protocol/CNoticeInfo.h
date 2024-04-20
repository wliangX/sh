#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CNoticeInfo :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strNoticeId = p_pJson->GetNodeValue("/notice_id", "");
				m_strUserCode = p_pJson->GetNodeValue("/user_code", "");
				m_strUserName = p_pJson->GetNodeValue("/user_name", "");
				m_strSeatNo = p_pJson->GetNodeValue("/seat_no", "");

				m_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
				m_strDeptName = p_pJson->GetNodeValue("/dept_name", "");
				m_strTargetDeptCode = p_pJson->GetNodeValue("/target_dept_code", "");
				m_strTargetDeptName = p_pJson->GetNodeValue("/target_dept_name", "");

				m_strNoticeType = p_pJson->GetNodeValue("/notice_type", "");
				m_strTitle = p_pJson->GetNodeValue("/title", "");
				m_strContent = p_pJson->GetNodeValue("/content", "");
				m_strTime = p_pJson->GetNodeValue("/time", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/notice_id", m_strNoticeId);
				p_pJson->SetNodeValue("/user_code", m_strUserCode);
				p_pJson->SetNodeValue("/user_name", m_strUserName);
				p_pJson->SetNodeValue("/seat_no", m_strSeatNo);

				p_pJson->SetNodeValue("/dept_code", m_strDeptCode);
				p_pJson->SetNodeValue("/dept_name", m_strDeptName);
				p_pJson->SetNodeValue("/target_dept_code", m_strTargetDeptCode);
				p_pJson->SetNodeValue("/target_dept_name", m_strTargetDeptName);

				p_pJson->SetNodeValue("/notice_type", m_strNoticeType);
				p_pJson->SetNodeValue("/title", m_strTitle);
				p_pJson->SetNodeValue("/content", m_strContent);
				p_pJson->SetNodeValue("/time", m_strTime);

				return p_pJson->ToString();
			}

		public:
			std::string m_strNoticeId;			//����/֪ͨID
			std::string m_strUserCode;			//�û�ID
			std::string m_strUserName;			//�û�����
			std::string m_strSeatNo;			//ϯλ��

			std::string m_strDeptCode;			//��λ����
			std::string m_strDeptName;			//��λ����
			std::string m_strTargetDeptCode;	//Ŀ�굥λ����
			std::string m_strTargetDeptName;	//Ŀ�굥λ����

			std::string m_strNoticeType;		//֪ͨ���ͣ�1�����棬2��֪ͨ��
			std::string m_strTitle;				//����
			std::string m_strContent;			//����/֪ͨ����
			std::string m_strTime;				//ʱ��
		};
	}
}