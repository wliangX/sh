#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsUpdateLogRequset : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				std::string l_strPrefixPath("/body/");
				m_oBody.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
				m_oBody.m_strProcessID = p_pJson->GetNodeValue(l_strPrefixPath + "process_id", "");
				m_oBody.m_strFeedbackID = p_pJson->GetNodeValue(l_strPrefixPath + "feedback_id", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue(l_strPrefixPath + "seat_no", "");
				m_oBody.m_strOperate = p_pJson->GetNodeValue(l_strPrefixPath + "operate", "");
				m_oBody.m_strOperateContent = p_pJson->GetNodeValue(l_strPrefixPath + "operate_content", "");
				m_oBody.m_strFromType = p_pJson->GetNodeValue(l_strPrefixPath + "from_type", "");
				m_oBody.m_strFromObject = p_pJson->GetNodeValue(l_strPrefixPath + "from_object", "");
				m_oBody.m_strFromObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_name", "");
				m_oBody.m_strFromObjectOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_name", "");
				m_oBody.m_strFromObjectOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_code", "");
				m_oBody.m_strToType = p_pJson->GetNodeValue(l_strPrefixPath + "to_type", "");
				m_oBody.m_strToObject = p_pJson->GetNodeValue(l_strPrefixPath + "to_object", "");
				m_oBody.m_strToObjectName = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_name", "");
				m_oBody.m_strToObjectOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_name", "");
				m_oBody.m_strToObjectOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_code", "");
				m_oBody.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
				m_oBody.m_strDeptOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "dept_org_code", "");
				m_oBody.m_strSourceName = p_pJson->GetNodeValue(l_strPrefixPath + "source_name", "");
				m_oBody.m_strOperateAttachDesc = p_pJson->GetNodeValue(l_strPrefixPath + "operate_attach_desc", "");

				m_oBody.m_strFromOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "from_object_org_identifier", "");//new
				m_oBody.m_strToObjectOrgIdentifier = p_pJson->GetNodeValue(l_strPrefixPath + "to_object_org_identifier", "");//new
				m_oBody.m_strDescription = p_pJson->GetNodeValue(l_strPrefixPath + "description", "");  //new
				m_oBody.m_strCreateOrg = p_pJson->GetNodeValue(l_strPrefixPath + "create_org", "");		//new
				m_oBody.m_strReceivedTime = p_pJson->GetNodeValue(l_strPrefixPath + "received_time", "");		//new
				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.m_strAlarmID);
				p_pJson->SetNodeValue(l_strPrefixPath + "process_id", m_oBody.m_strProcessID);
				p_pJson->SetNodeValue(l_strPrefixPath + "feedback_id", m_oBody.m_strFeedbackID);
				p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "operate", m_oBody.m_strOperate);
				p_pJson->SetNodeValue(l_strPrefixPath + "operate_content", m_oBody.m_strOperateContent);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_type", m_oBody.m_strFromType);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object", m_oBody.m_strFromObject);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_name", m_oBody.m_strFromObjectName);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_name", m_oBody.m_strFromObjectOrgName);
				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_code", m_oBody.m_strFromObjectOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_type", m_oBody.m_strToType);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object", m_oBody.m_strToObject);

				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_name", m_oBody.m_strToObjectName);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_name", m_oBody.m_strToObjectOrgName);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_code", m_oBody.m_strToObjectOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.m_strCreateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.m_strCreateTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "dept_org_code", m_oBody.m_strDeptOrgCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_name", m_oBody.m_strSourceName);
				p_pJson->SetNodeValue(l_strPrefixPath + "operate_attach_desc", m_oBody.m_strOperateAttachDesc);

				p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_identifier", m_oBody.m_strFromOrgIdentifier);
				p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_identifier", m_oBody.m_strToObjectOrgIdentifier);
				p_pJson->SetNodeValue(l_strPrefixPath + "description", m_oBody.m_strDescription);
				p_pJson->SetNodeValue(l_strPrefixPath + "received_time", m_oBody.m_strReceivedTime);

				return p_pJson->ToString();
			}
		public:
			
			class CBody
			{
			public:
				std::string	m_strID;						//������ˮid
				std::string	m_strAlarmID;					//�Ӿ������
				std::string	m_strProcessID;					//�ɾ������
				std::string	m_strFeedbackID;			    //���������
				std::string	m_strSeatNo;					//������ϯ
				std::string	m_strOperate;				    //��������
				std::string	m_strOperateContent;			//��������
				std::string	m_strFromType;				    //����������
				std::string	m_strFromObject;			    //�����ߴ���
				std::string	m_strFromObjectName;			//����������
				std::string	m_strFromObjectOrgName;			//��������֯����
				std::string	m_strFromObjectOrgCode;			//�����ߵ�λ����
				std::string	m_strToType;				    //����������
				std::string	m_strToObject;				    //�����ߴ���
				std::string	m_strToObjectName;			    //����������
				std::string	m_strToObjectOrgName;			//��������֯����
				std::string	m_strToObjectOrgCode;			//�����ߵ�λ����
				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strDeptOrgCode;				//�ɼ��ĵ�λ����
				std::string m_strSourceName;				//��־����Դ
				std::string m_strOperateAttachDesc;			//�����б�

				std::string m_strFromOrgIdentifier;         //�����ߵ�λ����
				std::string m_strToObjectOrgIdentifier;     //�����ߵ�λ����
				std::string m_strDescription;				//����
				std::string m_strCreateOrg;					//������λ
				std::string m_strReceivedTime;              //�Ӿ�ʱ�� received_time 
			};

			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}
