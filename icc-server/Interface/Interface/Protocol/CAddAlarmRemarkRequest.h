#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
		//��ICC������
        class CAddAlarmRemarkRequestEx :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strMsgSorce = p_pJson->GetNodeValue("/body/msg_source", "");
                m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
                m_oBody.m_strFeedBackCode = p_pJson->GetNodeValue("/body/feedback_code", "");
                m_oBody.m_strFeedBackName = p_pJson->GetNodeValue("/body/feedback_name", "");
                m_oBody.m_strFeedBackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
                m_oBody.m_strFeedBackDeptName = p_pJson->GetNodeValue("/body/feedback_dept_name", "");
                m_oBody.m_strFeedBackTime = p_pJson->GetNodeValue("/body/feedback_time", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strFileType = p_pJson->GetNodeValue("/body/file_type", "");
				m_oBody.m_strFileGuid = p_pJson->GetNodeValue("/body/file_guid", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");

				m_oBody.m_strFeedBackDeptOrgCode = p_pJson->GetNodeValue("/body/feedback_dept_org_code", "");
                return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSorce);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/feedback_code", m_oBody.m_strFeedBackCode);
				p_pJson->SetNodeValue("/body/feedback_name", m_oBody.m_strFeedBackName);
				p_pJson->SetNodeValue("/body/feedback_dept_code", m_oBody.m_strFeedBackDeptCode);
				p_pJson->SetNodeValue("/body/feedback_dept_name", m_oBody.m_strFeedBackDeptName);
				p_pJson->SetNodeValue("/body/feedback_time", m_oBody.m_strFeedBackTime);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/file_type", m_oBody.m_strFileType);
				p_pJson->SetNodeValue("/body/file_guid", m_oBody.m_strFileGuid);
				p_pJson->SetNodeValue("/body/longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue("/body/latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.m_strCreateTime);

				p_pJson->SetNodeValue("/body/feedback_dept_org_code", m_oBody.m_strFeedBackDeptOrgCode);
				return p_pJson->ToString();
			}

        public:

            class CBody
            {
            public:
				std::string m_strMsgSorce;				//��Ϣ��Դ
				std::string m_strGuid;					//��¼ID
                std::string m_strAlarmID;				//����idid
                std::string m_strFeedBackCode;			//�����˱��
                std::string m_strFeedBackName;			//����������
                std::string m_strFeedBackDeptCode;		//������λ���
                std::string m_strFeedBackDeptName;		//������λ����
                std::string m_strFeedBackTime;			//����ʱ��
				std::string m_strContent;				//��ע����
				std::string m_strFileType;				//APP�ļ����� ͼƬ0����Ƶ1������2����Ƶ3 ��λ��4������5���Զ���6��Ԥ����
				std::string m_strFileGuid;				//APP�ļ�id
				std::string m_strLongitude;				//APP �����ľ���
				std::string m_strLatitude;				//APP ������γ��
				std::string m_strCreateUser;
				std::string m_strCreateTime;

				std::string m_strFeedBackDeptOrgCode; //������λ����
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };


		//��VCS������
		class CAddAlarmRemarkRequest :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strMsgSorce = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strFeedBackCode = p_pJson->GetNodeValue("/body/feedback_code", "");
				m_oBody.m_strFeedBackName = p_pJson->GetNodeValue("/body/feedback_name", "");
				m_oBody.m_strFeedBackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
				m_oBody.m_strFeedBackDeptName = p_pJson->GetNodeValue("/body/feedback_dept_name", "");
				m_oBody.m_strFeedBackTime = p_pJson->GetNodeValue("/body/feedback_time", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strFileType = p_pJson->GetNodeValue("/body/file_type", "");
				m_oBody.m_strFileGuid = p_pJson->GetNodeValue("/body/file_guid", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue("/body/create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/create_time", "");
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSorce);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue("/body/feedback_code", m_oBody.m_strFeedBackCode);
				p_pJson->SetNodeValue("/body/feedback_name", m_oBody.m_strFeedBackName);
				p_pJson->SetNodeValue("/body/feedback_dept_code", m_oBody.m_strFeedBackDeptCode);
				p_pJson->SetNodeValue("/body/feedback_dept_name", m_oBody.m_strFeedBackDeptName);
				p_pJson->SetNodeValue("/body/feedback_time", m_oBody.m_strFeedBackTime);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/file_type", m_oBody.m_strFileType);
				p_pJson->SetNodeValue("/body/file_guid", m_oBody.m_strFileGuid);
				p_pJson->SetNodeValue("/body/longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue("/body/latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.m_strCreateTime);

				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strMsgSorce;				//��Ϣ��Դ
				std::string m_strGuid;					//��¼ID
				std::string m_strAlarmID;				//����idid
				std::string m_strFeedBackCode;			//�����˱��
				std::string m_strFeedBackName;			//����������
				std::string m_strFeedBackDeptCode;		//������λ���
				std::string m_strFeedBackDeptName;		//������λ����
				std::string m_strFeedBackTime;			//����ʱ��
				std::string m_strContent;				//��ע����
				std::string m_strFileType;				//APP�ļ����� ͼƬ0����Ƶ1������2����Ƶ3 ��λ��4������5���Զ���6��Ԥ����
				std::string m_strFileGuid;				//APP�ļ�id
				std::string m_strLongitude;				//APP �����ľ���
				std::string m_strLatitude;				//APP ������γ��
				std::string m_strCreateUser;
				std::string m_strCreateTime;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
    }
}

