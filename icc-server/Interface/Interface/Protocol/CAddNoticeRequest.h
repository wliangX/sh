#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddNoticeRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strTargetDeptCode = p_pJson->GetNodeValue("/body/target_dept_code", "");
				m_oBody.m_strTargetDeptName = p_pJson->GetNodeValue("/body/target_dept_name", "");

				m_oBody.m_strNoticeType = p_pJson->GetNodeValue("/body/notice_type", "");
				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

		public:
			class CBody
			{
			public:
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

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
